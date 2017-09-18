/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2012 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/* TODO
    1. Fine tune the stack required
    2. Co-effs are hard coded, should determine based on scaling ratio
 */

#include "mpSclrLink_priv.h"

#define MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE                (1900)
/**< Detemines width of sub-windows for individual scaling. Dot no change this*/
#define MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE_RESTRICTED     (1900)
/**< Detemines width of sub-windows for individual scaling. Dot no change this.
    Currently, we support YUV420 as input and this restriction is not
    applicable for this data type. Keeping this to ensure we do not face issue
    when support for other data formats are added. */

#define MP_SCLR_LINK_MAX_LINES_INPUT_COUNT                  (1060)
/**< The scalar internally splits MP frame into multiple sub-windows, this macro
        control the maximum number of lines per window. Do not change this */

#define MP_SCLR_LINK_SLICE_SIZE_MAX_RE_TRY                  (4)
/**< When the driver is updated for the slice size, update could fails due to
        in-correct width calculated by driver. We would require to try with
        different slice sizes. This macro control the number of re-tries */
#define MP_SCLR_LINK_PIXEL_COUNT_TO_REDUCE                  (16)
/**< When driver is unable to arrive at proper slice size for a given pixel
        count, we would require to try with next slice size.
        The value is used to get the next slice size, please do not change this
        value */
#ifndef debugMpSclrPrintStr
void debugMpSclrPrintStr (char *str)
{
    Vps_printf(str);
}
#endif

#ifndef debugMpSclrPrintReqObj
void debugMpSclrPrintReqObj(MpSclrLink_ReqObj *reqObj, char *str)
{
    Vps_printf(str);
    Vps_printf("Request Obj Addr = %x", reqObj);
    Vps_printf("InFrame List is = %x - should be %x", reqObj->processList.inFrameList[0], &reqObj->inFrameList);
    Vps_printf("OutFrame List is = %x - should be %x", reqObj->processList.outFrameList[0], &reqObj->outFrameList);
    Vps_printf("In / Out Frame List count = %x & %x ", reqObj->processList.numInLists, reqObj->processList.numOutLists);
    return;
}
#endif

#ifndef debugMpSclrPrintFrame
void debugMpSclrPrintFrame(FVID2_Frame *ptr, char *str)
{
    Vps_printf(str);
    Vps_printf("Frame Addr = %x", ptr);
    Vps_printf("Addr [0][0] = %x , Addr [0][1] = %x", ptr->addr[0][0], ptr->addr[0][1]);
    return;
}
#endif

static Int32 dvrCbMpSclrLink(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    MpSclrLink_Obj *pObj = (MpSclrLink_Obj *) appData;
    Semaphore_post(pObj->scalingDone);

    return FVID2_SOK;
}

static Int32 dvrErrCbMpSclrLink(FVID2_Handle handle,
                                Ptr appData, Ptr errList, Ptr reserved)
{
    /* TBD process error list */
    return FVID2_SOK;
}

static void initMpSclrDefaultInFmt(FVID2_Format *inFmt, UInt32 chId)
{
    inFmt->channelNum = chId;
    /* Allocate memory for max, we will use whats required */
    inFmt->width = MP_SCLR_LINK_DEF_IN_WIDTH;
    inFmt->height = MP_SCLR_LINK_DEF_IN_HEIGHT;
    inFmt->pitch[0] = MP_SCLR_LINK_DEF_IN_PITCH;
    inFmt->pitch[1] = MP_SCLR_LINK_DEF_IN_PITCH;
    inFmt->pitch[2] = MP_SCLR_LINK_DEF_IN_PITCH;
    inFmt->dataFormat = MP_SCLR_LINK_DEF_IN_DATAFORMAT;
    inFmt->scanFormat = SYSTEM_SF_PROGRESSIVE;
    inFmt->reserved = NULL;
    inFmt->fieldMerged[0] = FALSE;
    inFmt->fieldMerged[1] = FALSE;
    inFmt->fieldMerged[2] = FALSE;
    inFmt->bpp = FVID2_BPP_BITS16;
}


static void
    initMpSclrDefaultOutFmt(FVID2_Format *oFmt, UInt32 width, UInt32 chId)
{
    oFmt->channelNum = chId;
    /* Allocate memory for max, we will use whats required */
    oFmt->width = width;
    oFmt->height = MP_SCLR_LINK_OUT_MAX_HEIGHT;
    oFmt->pitch[0] = VpsUtils_align(oFmt->width * 2, VPS_BUFFER_ALIGNMENT);
    oFmt->pitch[1] = 0;
    oFmt->pitch[2] = 0;
    oFmt->dataFormat = MP_SCLR_LINK_OUT_DATATYPE;
    oFmt->scanFormat = SYSTEM_SF_PROGRESSIVE;
    oFmt->reserved = NULL;
    oFmt->fieldMerged[0] = FALSE;
    oFmt->fieldMerged[1] = FALSE;
    oFmt->fieldMerged[2] = FALSE;
    oFmt->bpp = FVID2_BPP_BITS16;
}

/* Local Functions */
static Int32 allocChOutputFrameBuffers (MpSclrLink_Obj *pObj, UInt32 chId)
{
    Int32 status;
    UInt32 frameId;
    FVID2_Frame *pFrame;
    MpSclrLink_ChObj *pChObj;
    System_FrameInfo *frameInfo;
    FVID2_Format tOutFmt;

    pChObj = &pObj->chObj[chId];

    *(&tOutFmt) = *(&pChObj->outFmt);
    tOutFmt.width = MP_SCLR_LINK_OUT_MAX_WIDTH;
    tOutFmt.pitch[0] = VpsUtils_align(MP_SCLR_LINK_OUT_MAX_WIDTH * 2,
                                        VPS_BUFFER_ALIGNMENT);

    for (frameId = 0; frameId < MP_SCLR_LINK_MAX_OUTPUT_FREE_FRAMES; frameId++)
    {
        /* Allocate memory required for maximum size, we will use whats
           required */
        pFrame = &pChObj->chFrames[frameId];
        status = Utils_memFrameAlloc(&tOutFmt, pFrame, 1);
        if (status == FVID2_SOK)
        {
            debugMpSclrPrintFrame(pFrame, "Allocating output frame");
            frameInfo = &pChObj->chFrameInfo[frameId];
            memset(frameInfo, 0, sizeof(System_FrameInfo));

            /* Mark as MP_SCLR frame and associate sys frame info */
            frameInfo->isMpFrame = MP_SCLR_LINK_MP_FRAME;
            frameInfo->rtChInfoUpdate = TRUE;
            frameInfo->rtChInfo.bufType = SYSTEM_BUF_TYPE_VIDFRAME;
            frameInfo->rtChInfo.dataFormat = tOutFmt.dataFormat;
            frameInfo->rtChInfo.memType = SYSTEM_MT_NONTILEDMEM;
            frameInfo->rtChInfo.startX = 0;
            frameInfo->rtChInfo.startY = 0;
            frameInfo->rtChInfo.width = pObj->maxWidthSupported;
            frameInfo->rtChInfo.height = MP_SCLR_LINK_OUT_MAX_HEIGHT;
            frameInfo->rtChInfo.pitch[0] = tOutFmt.pitch[0];
            frameInfo->rtChInfo.pitch[1] = tOutFmt.pitch[1];
            frameInfo->rtChInfo.pitch[2] = tOutFmt.pitch[2];
            frameInfo->rtChInfo.scanFormat = tOutFmt.scanFormat;

            pFrame->appData = frameInfo;

            status = Utils_bufPutEmptyFrame(&pObj->linkBufQ, pFrame);
            UTILS_assert(status == FVID2_SOK);
        }
        else
        {
            Vps_printf(" %d: MP_SCLR: Allocation of frame buffer memory"
                       " fails !!!\n", Utils_getCurTimeInMsec());
            break;
        }
    }

    if (status == FVID2_SOK)
    {
        pChObj->isFrameBufAlloced = TRUE;
    }

    return status;
}

static Int32 freeAllChOutputBuffers (MpSclrLink_Obj *pObj)
{
    UInt32 i, j;
    Int32 status = FVID2_SOK;
    MpSclrLink_ChObj *pChObj;
    FVID2_Format tOutFmt;

    for (i = 0; i < MP_SCLR_LINK_MAX_CH; i++)
    {
        pChObj = &pObj->chObj[i];
        if (pChObj->isFrameBufAlloced == FALSE)
        {
            continue;
        }
        *(&tOutFmt) = *(&pChObj->outFmt);
        tOutFmt.width = MP_SCLR_LINK_OUT_MAX_WIDTH;
        tOutFmt.pitch[0] = VpsUtils_align(MP_SCLR_LINK_OUT_MAX_WIDTH * 2,
                                            VPS_BUFFER_ALIGNMENT);
        for (j = 0; j < MP_SCLR_LINK_MAX_OUTPUT_FREE_FRAMES; j++)
        {
            status =
                Utils_memFrameFree(&tOutFmt, &pChObj->chFrames[j], 1);
            UTILS_assert(status == FVID2_SOK);
        }

        pChObj->isFrameBufAlloced = FALSE;
    }

    return FVID2_SOK;
}

static Int32 modifyFramePointer(FVID2_Frame *pIpFrm, System_FrameInfo *pFInfo,
                                Bool addOffset)
{
    Int32 offset[2], bytesPerPixel;
    System_LinkChInfo *rtInfo;

    rtInfo = &pFInfo->rtChInfo;

    if(rtInfo->dataFormat == SYSTEM_DF_YUV422I_YUYV)
        bytesPerPixel = 2;
    else
        bytesPerPixel = 1;

    offset[0] = rtInfo->pitch[0] * rtInfo->startY +
                               rtInfo->startX * bytesPerPixel;
    offset[1] = rtInfo->pitch[1] * rtInfo->startY / 2  +
                               rtInfo->startX * bytesPerPixel;

    if(addOffset == FALSE)
    {
        offset[0] = -offset[0];
        offset[1] = -offset[1];
    }

   pIpFrm->addr[0][0] = (Ptr) ((Int32)pIpFrm->addr[0][0] + offset[0]);
   pIpFrm->addr[0][1] = (Ptr) ((Int32)pIpFrm->addr[0][1] + offset[1]);

    return FVID2_SOK;
}


static MpSclrLink_ReqObj *getReqObjInst(MpSclrLink_Obj *pObj)
{
    Int32 status = FVID2_SOK;
    MpSclrLink_ReqObj *pListObj = NULL;

    status = Utils_queGet(&pObj->freeReqQ, (Ptr *)&pListObj, 1,
                            BIOS_WAIT_FOREVER);
    /* While closing, this queue would be worken up, even though a free Request
        object is not available, so check if we were to exit, before other
        oprations. */
    if (pObj->exitScTsk == TRUE)
    {
        return NULL;
    }

    if (status == FVID2_SOK)
    {
        pListObj->processList.inFrameList[0] = &pListObj->inFrameList;
        pListObj->processList.numInLists = 1;
        pListObj->processList.outFrameList[0] = &pListObj->outFrameList;
        pListObj->processList.numOutLists = 1;

        pListObj->outFrameList.numFrames = 1;
        pListObj->outFrameList.perListCfg = NULL;
        pListObj->outFrameList.appData = pListObj;

        pListObj->inFrameList.numFrames = 1;
        pListObj->inFrameList.perListCfg = NULL;
        pListObj->inFrameList.appData = NULL;
    }

    return (pListObj);
}

static inline Int32 isChanMapped(MpSclrLink_Obj *pObj, UInt32 chId)
{
    Int32 i;
    for (i = 0; i < MP_SCLR_LINK_MAX_CH; i++)
    {
        if (pObj->ipChaNumToChanMap[i] == chId)
        {
            break;
        }
    }
    if (i < MP_SCLR_LINK_MAX_CH)
    {
        return (i);
    }
    else
    {
        return -1;
    }

}

static Int32 mpSclrLinkMapChanAllocMem(MpSclrLink_Obj *pObj, Int32 *mappedCh,
                    UInt32 chId)
{
    Int32 i, status = FVID2_SOK;

    /*
     1. Look for available slot
     2. Check if output buffers are allocated, if not allocated
     3. Map the channel
    */
    for (i = 0; i < MP_SCLR_LINK_MAX_CH; i++)
    {
        if (pObj->ipChaNumToChanMap[i] == -1)
        {
            break;
        }
    }
    if (i < MP_SCLR_LINK_MAX_CH)
    {
        /* Check if output buffers are allocated */
        if (pObj->chObj[i].isFrameBufAlloced == FALSE)
        {
            /* Alloc output frames */
            status = allocChOutputFrameBuffers(pObj, i);
        }
        pObj->ipChaNumToChanMap[i] = chId;
        pObj->chObj[i].inChanNum = chId;
        *mappedCh = i;
        pObj->chObj[i].statsStartTime = Utils_getCurTimeInMsec();
    }
    else
    {
        status = FVID2_EFAIL;
        *mappedCh = -1;
    }
    return (status);
}


static Int32 updateIpFrameCfg(MpSclrLink_Obj *pObj, UInt32 chId)
{
    UInt32 decCnt, reTry;
    Vps_M2mScChParams *pDrvChPrm;
    Int32 status;

    pDrvChPrm = &pObj->drvChArgs[chId];

    /* The update slice size (SET SC CHAN INFO) could fail, if the no of
        pixel / line, result in un-aligned, exceed max, etc...
        try couple of times before giving it up */
    decCnt = MP_SCLR_LINK_PIXEL_COUNT_TO_REDUCE;

    for (reTry = 0; reTry < MP_SCLR_LINK_SLICE_SIZE_MAX_RE_TRY; reTry++)
    {
        /* Configure the channel param - input width / height */
        status = FVID2_control(pObj->fvidHandle,
                            IOCTL_VPS_SET_SC_CHANNEL_INFO, pDrvChPrm, NULL);

        if (status == FVID2_SOK)
        {
            /* Every thing went well */
            break;
        }

        /* For this slice size, we have issue, retry with next */
        pDrvChPrm->subFrameParams->numPixelsPerLine -= decCnt;
    }

    if (status != FVID2_SOK)
    {
        Vps_printf(" %d: MP_SCLR: ERROR could not update"
                " channel info!!!\n", Utils_getCurTimeInMsec());
    }
    else
    {

        /* Get the output buffer size requirements */
        pObj->chObj[chId].splitWinParms.chNum = chId;

        if (pObj->maxWidthSupported == MP_SCLR_LINK_OUT_MAX_WIDTH)
            pObj->chObj[chId].splitWinParms.subFrmSize =
                MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE;
        else
            pObj->chObj[chId].splitWinParms.subFrmSize =
                MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE_RESTRICTED;

        status = FVID2_control(pObj->fvidHandle,
                                IOCTL_VPS_GET_SC_HORZ_SUB_FRAME_INFO,
                                &pObj->chObj[chId].splitWinParms,
                                NULL);
        if (status != FVID2_SOK)
        {
            Vps_printf(" %d: MP_SCLR: ERROR could not determine memory required"
                    " by MP Scalar driver !!!\n", Utils_getCurTimeInMsec());

        }

    }
    return status;
}


static Int32 mpSclrLinkDvrHndlCreate(MpSclrLink_Obj *pObj)
{
    Int32 status;
    Vps_M2mScChParams *pDrvChPrm;
    UInt32 chId, sliceAtPixels, drvInstId;
    System_LinkChInfo *pChInfo;
    FVID2_CbParams cbParams;
    Vps_ScCoeffParams coEffParms;

    /* In the channel object, the informat and outformats are updated.
        check the input dataformat for tiled input */
    pObj->scCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    pObj->scCreateParams.numChannels = pObj->createArgs.numCh;
    pObj->scCreateParams.chParams = (Vps_M2mScChParams *) pObj->drvChArgs;

    for (chId = 0; chId < pObj->createArgs.numCh; chId++)
    {
        pDrvChPrm = &pObj->drvChArgs[chId];

        /* assume all CHs are of same input size, format, pitch */
        pChInfo = &pObj->inQueInfo.chInfo[0];

        pDrvChPrm->inFmt.channelNum = chId;

        initMpSclrDefaultInFmt(&pDrvChPrm->inFmt, chId);
        /* This is a workaround (01) - required by the driver.
            Set the default size to know params where in slice calculation
            will not fail. Otherwise, driver cannot tell us in which channel
            slice calculation failed. Hence default to 1920X1080, we will update
            slice information after the driver is create. */
        pDrvChPrm->inFmt.width = MP_SCLR_LINK_OUT_MAX_WIDTH;
        pDrvChPrm->inFmt.height = MP_SCLR_LINK_OUT_MAX_HEIGHT;

        initMpSclrDefaultOutFmt(&pDrvChPrm->outFmt,
                pObj->maxWidthSupported, chId);

        pDrvChPrm->inMemType = pChInfo->memType;
        pDrvChPrm->outMemType = VPS_VPDMA_MT_NONTILEDMEM;

        pDrvChPrm->scCfg = &pObj->chObj[chId].scCfg;
        pDrvChPrm->srcCropCfg = NULL;

        pDrvChPrm->scCfg->bypass = FALSE;
        pDrvChPrm->scCfg->nonLinear = FALSE;
        pDrvChPrm->scCfg->stripSize = 0;
        pDrvChPrm->scCfg->vsType = VPS_SC_VST_POLYPHASE;
        pDrvChPrm->scCfg->hsType = VPS_SC_HST_POLYPHASE;
        pDrvChPrm->scCfg->enablePeaking = TRUE;

        if(pObj->createArgs.enableLineSkip &&
           pChInfo->memType == VPS_VPDMA_MT_NONTILEDMEM)
        {
            /* half the height and double the pitch possible
               only when input is non-tiled */
            pDrvChPrm->inFmt.pitch[0] *= 2;
            pDrvChPrm->inFmt.pitch[1] *= 2;
            pDrvChPrm->inFmt.height /= 2;

        }

        /* Do not enable sub frame based processing here, once FVID2 handle
            is created, update the slice info, at which point, set the subframe
            to valid - behaviour of the driver.
        */
        pDrvChPrm->subFrameParams = NULL;
    }

    cbParams.cbFxn = dvrCbMpSclrLink;
    cbParams.errCbFxn = dvrErrCbMpSclrLink;
    cbParams.errList = &pObj->errProcessList;
    cbParams.appData = pObj;

    if (pObj->createArgs.pathId == MP_SCLR_LINK_SEC0_SC3_VIP0)
    {
        drvInstId = VPS_M2M_INST_SEC0_SC3_VIP0;
    }
    else if (pObj->createArgs.pathId == MP_SCLR_LINK_SEC1_SC4_VIP1)
    {
        drvInstId = VPS_M2M_INST_SEC1_SC4_VIP1;
    }
    else
    {
        drvInstId = VPS_M2M_INST_SEC0_SC5_WB2;
    }

    pObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
                                    drvInstId,
                                    &pObj->scCreateParams,
                                    &pObj->scCreateStatus, &cbParams);

    UTILS_assert(pObj->scCreateStatus.retVal == FVID2_SOK);
    UTILS_assert(pObj->fvidHandle != NULL);

    if (drvInstId == 0)
    {
        Vps_printf(" %d: MP_SCLR: Configured to use scalar instance 5 !!! \n"
            , Utils_getCurTimeInMsec());
    }
    else
    {
        Vps_printf(" %d: MP_SCLR: Configured to use scalar instance %d !!! \n"
            , Utils_getCurTimeInMsec(), drvInstId);
    }

    /* For each channel update the slice info */
    for (chId = 0; chId < pObj->createArgs.numCh; chId++)
    {
        pDrvChPrm = &pObj->drvChArgs[chId];

        /* This is a workaround (01).
           Override, what was done during create with default values */
        initMpSclrDefaultInFmt(&pDrvChPrm->inFmt, chId);

        pDrvChPrm->subFrameParams = &pObj->chObj[chId].subWinParams;

        pDrvChPrm->subFrameParams->subFrameModeEnable = TRUE;


        pDrvChPrm->subFrameParams->numLinesPerSubFrame =
                                            MP_SCLR_LINK_MAX_LINES_INPUT_COUNT;
        if (pDrvChPrm->inFmt.height < MP_SCLR_LINK_MAX_LINES_INPUT_COUNT)
        {
            pDrvChPrm->subFrameParams->numLinesPerSubFrame = 
                                                        pDrvChPrm->inFmt.height;
        }

        if (pObj->maxWidthSupported == MP_SCLR_LINK_OUT_MAX_WIDTH)
        {
            sliceAtPixels = MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE;
            if (pDrvChPrm->inFmt.width < MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE)
            {
                sliceAtPixels = pDrvChPrm->inFmt.width;
            }
        }
        else
        {
            sliceAtPixels = MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE_RESTRICTED;
            if (pDrvChPrm->inFmt.width < 
                            MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE_RESTRICTED)
            {
                sliceAtPixels = pDrvChPrm->inFmt.width;
            }
        }

        pDrvChPrm->subFrameParams->numPixelsPerLine =
            (sliceAtPixels * pDrvChPrm->outFmt.width)
                / pDrvChPrm->inFmt.width;
        /* Should be 16bytes aligned, mask out last 4 bits */
        pDrvChPrm->subFrameParams->numPixelsPerLine &= 0xFFFFFFF0;

        status = updateIpFrameCfg(pObj, chId);
        UTILS_assert(status == FVID2_SOK);
    }

    /* Load the co-effs */
    coEffParms.hScalingSet = VPS_SC_DS_SET_15_16;
    coEffParms.vScalingSet = VPS_SC_DS_SET_15_16;
    coEffParms.coeffPtr = NULL;
    coEffParms.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;
    status = FVID2_control(pObj->fvidHandle,
                            IOCTL_VPS_SET_COEFFS,
                            &coEffParms,
                            NULL);
    UTILS_assert(status == FVID2_SOK);

    return status;
}

static Int32 mpSclrLinkDvrHndlDelete(MpSclrLink_Obj *pObj)
{
    return FVID2_delete(pObj->fvidHandle, NULL);
}

static Int32 mpScleLinkDvrChDelete(MpSclrLink_Obj *pObj)
{
    Int32 status, chId;
    MpSclrLink_ChObj *pChObj;

    status = FVID2_EFAIL;
    /* For each channel perform the following
     1. Delete input queue
     2. Initialize channel as un-used
     */
    for (chId = 0; chId < MP_SCLR_LINK_MAX_CH; chId++)
    {
        pChObj = &pObj->chObj[chId];

        status = Utils_bufDelete(&pChObj->inQue);
        UTILS_assert(status == FVID2_SOK);

        pChObj->inChanNum = MP_SCLR_INVALID_U32_VALUE;
    }

    return (status);
}

static Int32 mpSclrLinkDvrChCreate(MpSclrLink_Obj *pObj)
{
    Int32 status, chId;
    MpSclrLink_ChObj *pChObj;

    status = FVID2_EFAIL;
    /* For each channel perform the following
     1. Create input queue
     2. Initialize channel as un-used
     3. Initialize counters to zero
     4. Setup input and output formats.
     */
    for (chId = 0; chId < MP_SCLR_LINK_MAX_CH; chId++)
    {
        pChObj = &pObj->chObj[chId];
        /* Step 1 */
        status = Utils_bufCreate(&pChObj->inQue, FALSE, FALSE);
        UTILS_assert(status == FVID2_SOK);
        /* Step 2 */
        pChObj->inChanNum = MP_SCLR_INVALID_U32_VALUE;
        pChObj->isFrameBufAlloced = FALSE;

        /* Step 3 */
        pChObj->inFrameProcessCount = 0;
        pChObj->outFrameCount = 0;

        pChObj->statsStartTime = 0;
        pChObj->minLatencyDrv = 0xFFFF;
        pChObj->maxLatencyDrv = 0x0;

        pChObj->minLatency = 0;
        pChObj->maxLatency = 0;

        /* Step 4 */
        initMpSclrDefaultInFmt(&pChObj->inFmt, chId);
        UTILS_assert(MP_SCLR_LINK_OUT_DATATYPE == SYSTEM_DF_YUV422I_YUYV);
        initMpSclrDefaultOutFmt(&pChObj->outFmt, pObj->maxWidthSupported, chId);
		
		pChObj->enableOut = TRUE;
    }

    return (status);
}

static Int32 mpSclrLinkDrvResetInstVars(MpSclrLink_Obj *pObj)
{
    UInt32 chId;
    /*
     1. Clear input frames channel to MP Scalar channel mappings
     2. Setup the frame lists
    */
    for (chId = 0; chId < MP_SCLR_LINK_MAX_CH; chId++)
    {
        pObj->ipChaNumToChanMap[chId] = -1;
    }

    pObj->inFrameFwdCount = 0;
    pObj->inFrameRecvCount = 0;
    pObj->inFrameNotProcessed = 0;

    return FVID2_SOK;
}

static void mpSclrLinkDrvCheckCreateArgs(MpSclrLink_CreateParams *args)
{
    if ((args->pathId != MP_SCLR_LINK_SC5) && 
        (args->pathId != MP_SCLR_LINK_SEC0_SC3_VIP0) && 
        (args->pathId != MP_SCLR_LINK_SEC1_SC4_VIP1))
    {
        args->pathId = MP_SCLR_LINK_SC5;
        Vps_printf(" %d: MP_SCLR: Warning - SC5, SC3 & SC4 are supported"
        " - Switching to SC 5!!!\n", Utils_getCurTimeInMsec());
    }
    if (args->numCh > MP_SCLR_LINK_MAX_CH)
    {
        args->numCh = MP_SCLR_LINK_MAX_CH;
        Vps_printf(" %d: MP_SCLR: Warning - %d Channels supported."
        " Defaulting to same !!!\n", Utils_getCurTimeInMsec(),
        MP_SCLR_LINK_MAX_CH);
    }

}

static void mpSclrLinkDrvUpdateIpResChange(MpSclrLink_Obj *pObj,
                                            System_LinkChInfo *pIpRtInfo,
                                            UInt32 chId)
{
    Int32 status;
    UInt32 updateCfg, sliceAtPixels;
    FVID2_Format *pCurrCfg;
    Vps_M2mScChParams *pDrvChArgs;
    /* Check if the input width / height / pitch has changed, if so,
        update the drivers channel arguments, re-calculate the slice info */

    UTILS_assert(pObj != NULL);
    updateCfg = FALSE;

    pCurrCfg = &pObj->chObj[chId].inFmt;
    pDrvChArgs = &pObj->drvChArgs[chId];

    if ((pIpRtInfo->width != pCurrCfg->width) ||
        (pIpRtInfo->height != pCurrCfg->height))
    {
        updateCfg = TRUE;
    }

    if ((pIpRtInfo->pitch[0] != pCurrCfg->pitch[0]) ||
        (pIpRtInfo->pitch[1] != pCurrCfg->pitch[1]))
    {
        updateCfg = TRUE;
    }

    if (updateCfg == TRUE)
    {
        Vps_printf (" %d: MP_SCLR: - Input Resolution Changed, updating... ",
            Utils_getCurTimeInMsec());
        pDrvChArgs->inFmt.width = pIpRtInfo->width;
        pDrvChArgs->inFmt.height = pIpRtInfo->height;

        pDrvChArgs->inFmt.pitch[0] = pIpRtInfo->pitch[0];
        pDrvChArgs->inFmt.pitch[1] = pIpRtInfo->pitch[1];
        pDrvChArgs->inFmt.pitch[2] = pIpRtInfo->pitch[2];

        if(pObj->createArgs.enableLineSkip &&
           pDrvChArgs->inMemType == VPS_VPDMA_MT_NONTILEDMEM)
        {
            /* half the height and double the pitch possible
               only when input is non-tiled */
            pDrvChArgs->inFmt.pitch[0] *= 2;
            pDrvChArgs->inFmt.pitch[1] *= 2;
            pDrvChArgs->inFmt.height /= 2;

        }

        pCurrCfg->width = pDrvChArgs->inFmt.width;
        pCurrCfg->height = pDrvChArgs->inFmt.height;
        pCurrCfg->pitch[0] = pDrvChArgs->inFmt.pitch[0];
        pCurrCfg->pitch[1] = pDrvChArgs->inFmt.pitch[1];
        pCurrCfg->pitch[2] = pDrvChArgs->inFmt.pitch[2];

        
        pDrvChArgs->subFrameParams->numLinesPerSubFrame =
                                            MP_SCLR_LINK_MAX_LINES_INPUT_COUNT;
        if (pDrvChArgs->inFmt.height < MP_SCLR_LINK_MAX_LINES_INPUT_COUNT)
        {
            pDrvChArgs->subFrameParams->numLinesPerSubFrame = 
                                                    pDrvChArgs->inFmt.height;
        }

        if (pObj->maxWidthSupported == MP_SCLR_LINK_OUT_MAX_WIDTH)
        {
            sliceAtPixels = MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE;
            if (pDrvChArgs->inFmt.width < MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE)
            {
                sliceAtPixels = pDrvChArgs->inFmt.width;
            }
        }
        else
        {
            sliceAtPixels = MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE_RESTRICTED;
            if (pDrvChArgs->inFmt.width < 
                            MP_SCLR_LINK_MAX_VERTICAL_SLICE_SIZE_RESTRICTED)
            {
                sliceAtPixels = pDrvChArgs->inFmt.width;
            }
        }
        pDrvChArgs->subFrameParams->numPixelsPerLine =
            (sliceAtPixels * pDrvChArgs->outFmt.width)
                / pDrvChArgs->inFmt.width;
        /* Should be 16bytes aligned, mask out last 4 bits */
        pDrvChArgs->subFrameParams->numPixelsPerLine &= 0xFFFFFFF0;

        status = updateIpFrameCfg(pObj, chId);
        UTILS_assert(status == FVID2_SOK);
        Vps_printf (" %d: MP_SCLR: - Input Resolution Updated ",
            Utils_getCurTimeInMsec());
    }
}

static MpSclrLink_ReqObj * mpSclrLinkDrvMakeReqObj(MpSclrLink_Obj * pObj,
                                                    FVID2_Frame *pIpFrame,
                                                    UInt32 chId)
{
    MpSclrLink_ReqObj *pReqObj = NULL;
    Int32 status = 0x0;
    FVID2_Frame *pOpFrame;

    UTILS_assert(pIpFrame != NULL);
    pOpFrame = NULL;

    while (status == 0x0)
    {
        pReqObj = getReqObjInst(pObj);
        if (pObj->exitScTsk == TRUE)
        {
            /* Expected exit sequence for scaling task */
            break;
        }
        if (pReqObj == NULL)
        {
            Vps_printf(" %d: MP_SCLR: ERROR request object is NULL \n"
                " This was not expected!!!\n", Utils_getCurTimeInMsec());
            break;
        }

        /* Allocate a output frame */
        Utils_bufGetEmptyFrame(&pObj->linkBufQ, &pOpFrame, BIOS_WAIT_FOREVER);
        if (pObj->exitScTsk == TRUE)
        {
            /* Expected exit sequence for scaling task */
            break;
        }
        if (pOpFrame == NULL)
        {
            Vps_printf(" %d: MP_SCLR: ERROR output frame is NULL \n"
                " This was not expected!!!\n", Utils_getCurTimeInMsec());
            status = Utils_quePut(&pObj->freeReqQ, (Ptr)pReqObj, BIOS_NO_WAIT);
            UTILS_assert(status == FVID2_SOK);
            pReqObj = NULL;
            break;
        }

        /* Over-Cautios check */
        if ((pIpFrame->channelNum & MP_SCLR_LINK_MP_FRAME) ==
                MP_SCLR_LINK_MP_FRAME)
        {
            Vps_printf(" %d: MP_SCLR: ERROR Channel number is incorrect !!!\n "
                " Clashes with MP Frame marker, not processing this frame!!!\n"
                , Utils_getCurTimeInMsec());
            /* Clear out MP flag of prev link */
            ((System_FrameInfo *)pIpFrame->appData)->isMpFrame = 0;

            status = Utils_bufPutEmptyFrame(&pObj->linkBufQ, pOpFrame);
            UTILS_assert(status == FVID2_SOK);
            status = Utils_quePut(&pObj->freeReqQ, (Ptr)pReqObj, BIOS_NO_WAIT);
            UTILS_assert(status == FVID2_SOK);
            pReqObj = NULL;
            break;
        }

        debugMpSclrPrintFrame(pOpFrame, "Make Request Obj -> Output Frame");

        pReqObj->inFrameList.numFrames = 1;
        pReqObj->outFrameList.numFrames = 1;

        pReqObj->inFrameList.frames[0] = pIpFrame;
        pReqObj->outFrameList.frames[0] = pOpFrame;

        pReqObj->processList.numInLists = 1;
        pReqObj->processList.numOutLists = 1;

        break;
    }

    return pReqObj;
}


Int32 mpSclrLinkDrvMpPostProcessedFrames(MpSclrLink_Obj *pObj)
{
    UInt32 i;
    Int32 status = FVID2_SOK;
    System_FrameInfo *pFInfo;
    FVID2_FrameList *inFrameList, *outFrameList;
    MpSclrLink_ReqObj *pCompReqObj;

    /* 1. Get the processed frames,
       2. Release the input frames to previous link
       3. Release the output frames to next link and let it know
       4. Only in debug, check if all the output frames are indeed marked as MP
            frame.
    */
    status = Utils_queGet(&pObj->compReqQ, (Ptr *)&pCompReqObj, 1, BIOS_NO_WAIT);

    if (status != FVID2_SOK)
    {
        /* Why were we woken up */
        return FVID2_SOK;
    }
    debugMpSclrPrintReqObj(pCompReqObj, "Post Completed - Request Obj");

    inFrameList = pCompReqObj->processList.inFrameList[0];
    outFrameList = pCompReqObj->processList.outFrameList[0];

    UTILS_assert(pCompReqObj->processList.numInLists == 1);
    UTILS_assert(inFrameList->numFrames != 0);
    UTILS_assert(inFrameList->frames[0] != NULL);
    UTILS_assert(pCompReqObj->processList.numOutLists == 1);
    UTILS_assert(outFrameList->numFrames != 0);

    /* Restore the original channel number of input frames */
    for (i = 0; i < inFrameList->numFrames; i++)
    {
        if (inFrameList->frames[i] == NULL)
        {
            Vps_printf(" MP_SCLR: ERROR - reqObj - %x !!!\n",
                outFrameList->appData);

            debugMpSclrPrintStr("Input frame returned after scaling is NULL");
        }
        pFInfo = (System_FrameInfo *)inFrameList->frames[i]->appData;
        inFrameList->frames[i]->channelNum = pFInfo->isMpFrame;
        inFrameList->frames[i]->subFrameInfo = NULL;
        /* Clear out MP flag of prev link */
        pFInfo->isMpFrame = 0;
        debugMpSclrPrintFrame(inFrameList->frames[i],
                "Returning to previous link InFrame");
    }

    status = System_putLinksEmptyFrames(
                            pObj->createArgs.inQueParams.prevLinkId,
                            pObj->createArgs.inQueParams.prevLinkQueId,
                            inFrameList);

    for (i = 0; i < outFrameList->numFrames; i++)
    {
        if (outFrameList->frames[i] == NULL)
        {
            debugMpSclrPrintStr("Output frame returned after scaling is NULL");
        }
        pFInfo = (System_FrameInfo *)outFrameList->frames[i]->appData;
        if ((pFInfo->isMpFrame & MP_SCLR_LINK_MP_FRAME) != MP_SCLR_LINK_MP_FRAME)
        {
            Vps_printf(" %d: MP_SCLR: ERROR - outframe not marked as MP frame!!!\n",
                Utils_getCurTimeInMsec());
        }
        debugMpSclrPrintFrame(outFrameList->frames[i],
                "Sending Output frame to next link");
    }

    status = Utils_bufPutFull(&pObj->linkBufQ, outFrameList);
    UTILS_assert(status == FVID2_SOK);
    debugMpSclrPrintReqObj(pCompReqObj, "Post Completed - Freeing Request Obj");
    status = Utils_quePut(&pObj->freeReqQ, (Ptr)pCompReqObj, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);
    System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                       SYSTEM_CMD_NEW_DATA);
    return (status);
}

static Int32 mpSclrLinkDrvScaleMpFrame(MpSclrLink_Obj *pObj,
                                        MpSclrLink_ReqObj *pReqObj)
{
    System_FrameInfo *pFInfo;
    FVID2_Frame *pIpFrame, *pOpFrame;
    UInt32 temp, vSubFrmNum, vIdx, hIdx, chId;
    Int status = FVID2_EFAIL;
    Vps_M2mScChParams *pDrvChPrm;
    UInt32 latency, startTime;

    /*
    1. While we have input frames to be processed
        a. Get an request object, input frame & output frame
            a.1 If any fails move to next channel
        b. if processing the first slice
            b.1. Update the channel number with the mapped channel number,
                    ensure to store the original channel number
            b.2. Update the Sysinfo, update the rtParam with output format
        c. Scale them
    */

    while (TRUE)
    {
        /* Check args */
        if (pReqObj == NULL)
        {
            Vps_printf(" MP_SCLR: ERROR - A NULL request object received!!!\n");
            break;
        }

        UTILS_assert(pReqObj->processList.numInLists == 1);
        UTILS_assert(pReqObj->processList.numOutLists == 1);
        UTILS_assert(pReqObj->processList.inFrameList[0] != NULL);
        UTILS_assert(pReqObj->processList.outFrameList[0] != NULL);
        UTILS_assert(pReqObj->processList.inFrameList[0]->numFrames == 1);
        UTILS_assert(pReqObj->processList.outFrameList[0]->numFrames == 1);

        pIpFrame = pReqObj->inFrameList.frames[0];
        pOpFrame = pReqObj->outFrameList.frames[0];

        if ((pIpFrame == NULL) || (pOpFrame == NULL))
        {
            Vps_printf(" MP_SCLR: ERROR - In or Out Frame is NULL!!!\n");
            break;
        }

        /* Update the input / output channel numbers */
        pFInfo = (System_FrameInfo *)pIpFrame->appData;
        temp = pIpFrame->channelNum;
        pIpFrame->channelNum = pFInfo->isMpFrame;
        chId = pFInfo->isMpFrame;
        /* Ensure to remmember actual channel number */
        pFInfo->isMpFrame = temp;

        /* Check if the input resolution has changed, if so, recalculate
            the vertical / horizontals slices, re-configure */

        mpSclrLinkDrvUpdateIpResChange(pObj, &pFInfo->rtChInfo,
                                        pIpFrame->channelNum);

        /* Update the sub frame info */
        pIpFrame->subFrameInfo = &pObj->chObj[chId].ipSelSubWin;

        pOpFrame->channelNum = pIpFrame->channelNum;

        pFInfo = (System_FrameInfo *)pOpFrame->appData;
        UTILS_assert(pFInfo != NULL);
        *pFInfo = *((System_FrameInfo *)pIpFrame->appData);

        /* Require to re-initialize sysInfo, as we have copied the input
            sysFrameInfo completely, not required, if we do not copy complete
            sysFrameInfo */
        pFInfo->isMpFrame = MP_SCLR_LINK_MP_FRAME;
        pFInfo->rtChInfoUpdate = TRUE;

        pFInfo->rtChInfo.bufType = SYSTEM_BUF_TYPE_VIDFRAME;
        pFInfo->rtChInfo.dataFormat = pObj->chObj[chId].outFmt.dataFormat;
        pFInfo->rtChInfo.memType = SYSTEM_MT_NONTILEDMEM;
        pFInfo->rtChInfo.startX = 0;
        pFInfo->rtChInfo.startY = 0;
        pFInfo->rtChInfo.width = pObj->maxWidthSupported;
        pFInfo->rtChInfo.height = MP_SCLR_LINK_OUT_MAX_HEIGHT;
        pFInfo->rtChInfo.pitch[0] = pObj->chObj[chId].outFmt.pitch[0];
        pFInfo->rtChInfo.pitch[1] = pObj->chObj[chId].outFmt.pitch[1];
        pFInfo->rtChInfo.pitch[2] = pObj->chObj[chId].outFmt.pitch[2];
        pFInfo->rtChInfo.scanFormat = pObj->chObj[chId].outFmt.scanFormat;

        /* Ensure to take care of start x and start y, as our driver cannot
            account for it */
        modifyFramePointer(pIpFrame, (System_FrameInfo *)pIpFrame->appData,
                            TRUE);
        /* Determine the number of sub-windows */
        pDrvChPrm = &pObj->drvChArgs[chId];

        /* Number of coloums */
        vSubFrmNum = pDrvChPrm->inFmt.height /
                        pDrvChPrm->subFrameParams->numLinesPerSubFrame;
        if (pDrvChPrm->inFmt.height %
                        pDrvChPrm->subFrameParams->numLinesPerSubFrame)
        {
            vSubFrmNum++;
        }

        startTime = Utils_getCurTimeInMsec();
        /* Scale the 1st coloumn sub-windows (on the vertical axis) */
        for (vIdx = 0; vIdx < vSubFrmNum; vIdx++)
        {
            if (vIdx == vSubFrmNum - 1)
            {
                pIpFrame->subFrameInfo->numInLines =  pObj->chObj[chId].inFmt.height;
            }
            else
            {
                pIpFrame->subFrameInfo->numInLines =
                                (vIdx + 1) * MP_SCLR_LINK_MAX_LINES_INPUT_COUNT;
            }
            pIpFrame->subFrameInfo->subFrameNum = vIdx;

            pObj->chObj[chId].waitingForScCompletion = TRUE;

            status =
                FVID2_processFrames(pObj->fvidHandle, &pReqObj->processList);
            UTILS_assert(status == FVID2_SOK);

            Semaphore_pend(pObj->scalingDone, BIOS_WAIT_FOREVER);

            pObj->chObj[chId].waitingForScCompletion = FALSE;
            /* Get the completed frame list */
            status =
                FVID2_getProcessedFrames(pObj->fvidHandle,
                                            &pReqObj->processList,
                                            BIOS_NO_WAIT);
            UTILS_assert(status == FVID2_SOK);
        }

        /* 0th Horizontal subframe was completed in the previous loop */
        for (hIdx = 1; hIdx < pObj->chObj[chId].splitWinParms.numSubFrm; hIdx++)
        {
            /* Scale on the Vertical */
            for (vIdx = 0; vIdx < vSubFrmNum; vIdx++)
            {
                if (vIdx == vSubFrmNum - 1)
                {
                    /* What ever height remains */
                    pIpFrame->subFrameInfo->numInLines =
                            pObj->chObj[chId].inFmt.height;
                }
                else
                {
                    /* Standard input size of 1080 * vertical window no */
                    pIpFrame->subFrameInfo->numInLines =
                            (vIdx + 1) * MP_SCLR_LINK_MAX_LINES_INPUT_COUNT;
                }
                pIpFrame->subFrameInfo->subFrameNum =
                        (hIdx << 16 & 0xFFFF0000) | (vIdx & 0xFFFF);

                status =
                FVID2_processFrames(pObj->fvidHandle, &pReqObj->processList);
                UTILS_assert(status == FVID2_SOK);

                Semaphore_pend(pObj->scalingDone, BIOS_WAIT_FOREVER);

                status =
                    FVID2_getProcessedFrames(pObj->fvidHandle,
                                                &pReqObj->processList,
                                                BIOS_NO_WAIT);
                UTILS_assert(status == FVID2_SOK);

            }
        }

        latency = Utils_getCurTimeInMsec() - startTime;
        if (latency < pObj->chObj[chId].minLatencyDrv)
        {
            pObj->chObj[chId].minLatencyDrv = latency;
        }
        if (latency > pObj->chObj[chId].maxLatencyDrv)
        {
            pObj->chObj[chId].maxLatencyDrv = latency;
        }
        /* Restore the modified start address */
        modifyFramePointer(pIpFrame, (System_FrameInfo *)pIpFrame->appData,
                            FALSE);
        /* Associate input frames channel number to output frames channel no */
        pOpFrame->channelNum =
            ((System_FrameInfo *)pIpFrame->appData)->isMpFrame;

        if (status == FVID2_SOK)
        {
            debugMpSclrPrintReqObj(pReqObj, "Scaleing Completed Request Obj");
            pObj->chObj[chId].inFrameProcessCount++;
            pObj->chObj[chId].outFrameCount++;
        }
        else
        {
            Vps_printf(" MP_SCLR: ERROR - Could not scale!!!\n");
        }

        /* Even on failures, post the un-processed frames */
        status = Utils_quePut(&pObj->compReqQ,
                                (Ptr)pReqObj, BIOS_NO_WAIT);
        UTILS_assert(status == FVID2_SOK);
        status = mpSclrLinkDrvMpPostProcessedFrames(pObj);
        UTILS_assert(status == FVID2_SOK);

        break;
    }

    return (status);
}

/*******************************Exported Functions ****************************/

Int32 MpSclrLink_drvDelete(MpSclrLink_Obj *pObj)
{
    Int32 status;
    /*
     1. Release memory acquired for output frames
     2. Delete driver
     3. Delete Channel objects
     4. Delete Scaling completed semaphore
     5. Delete Request Q and completed Q
     6. Delete DMA channel
     */
    status = freeAllChOutputBuffers(pObj);
    UTILS_assert(status == FVID2_SOK);

    status = mpSclrLinkDvrHndlDelete(pObj);
    UTILS_assert(status == FVID2_SOK);

    status = mpScleLinkDvrChDelete(pObj);
    UTILS_assert(status == FVID2_SOK);

    Semaphore_delete(&pObj->scalingDone);

    Vps_printf(" %d: MP_SCLR: deleting linkBufQ !!!\n", Utils_getCurTimeInMsec());
    status = Utils_bufDelete(&pObj->linkBufQ);
    UTILS_assert(status == FVID2_SOK);

    status = Utils_queDelete(&pObj->freeReqQ);
    UTILS_assert(status == FVID2_SOK);

    status = Utils_queDelete(&pObj->reqQ);
    UTILS_assert(status == FVID2_SOK);

    status = Utils_queDelete(&pObj->compReqQ);
    UTILS_assert(status == FVID2_SOK);

    return status;
}

Int32 MpSclrLink_drvPrintStatistics(MpSclrLink_Obj *pObj, UInt32 flag)
{
    UInt32 i, elaspedTime;

    Vps_printf( " \n"
            " *** [%s] Statistics *** \n"
            " \n"
            " Total Frames Received  : %d \n"
            " Total Frames Forwarded : %d \n"
            " \n"
            " \n"
            " CH  | In Recv In Reject Processed  Latency(DRV) Processed  Rejected\n"
            " Num | FPS     FPS       FPS        Min / Max    Frames     Frames  \n"
            " -------------------------------------------------------------------\n",
            pObj->mpScTskName,
            pObj->inFrameRecvCount,
            pObj->inFrameFwdCount);

    for (i = 0; i < MP_SCLR_LINK_MAX_CH; i++)
    {
        if (pObj->ipChaNumToChanMap[i] < 0)
            continue;

        elaspedTime = Utils_getCurTimeInMsec() -
                        pObj->chObj[i].statsStartTime; /*in msecs */
        elaspedTime /= 1000; /* convert to secs */
#ifndef debugMpSclrPrintFrame
        Vps_printf( " %3d | %-8d %-9d %-10d %-3d %-3d     %-10d %-10d %d \n",
            pObj->chObj[i].inChanNum,
            (pObj->chObj[i].inFrameProcessCount +
                 pObj->chObj[i].inFrameRejectedCount) / elaspedTime,
            pObj->chObj[i]. / elaspedTime,
            pObj->chObj[i].inFrameProcessCount / elaspedTime,
            pObj->chObj[i].minLatencyDrv,
            pObj->chObj[i].maxLatencyDrv,
            pObj->chObj[i].inFrameProcessCount,
            pObj->chObj[i].inFrameRejectedCount,
            pObj->chObj[i].waitingForScCompletion);
#else
        Vps_printf( " %3d | %-8d %-9d %-10d %-3d %-3d     %-10d %-10d\n",
            pObj->chObj[i].inChanNum,
            (pObj->chObj[i].inFrameProcessCount +
                 pObj->chObj[i].inFrameRejectedCount) / elaspedTime,
            pObj->chObj[i].inFrameRejectedCount / elaspedTime,
            pObj->chObj[i].inFrameProcessCount / elaspedTime,
            pObj->chObj[i].minLatencyDrv,
            pObj->chObj[i].maxLatencyDrv,
            pObj->chObj[i].inFrameProcessCount,
            pObj->chObj[i].inFrameRejectedCount);
#endif

    }

    return FVID2_SOK;
}

Int32 MpSclrLink_drvSetChOutputRes(MpSclrLink_Obj *pObj,
                                    MpSclrLink_chDynamicSetOutRes *params)
{
    return FVID2_SOK;
}

Int32 MpSclrLink_drvGetChOutputRes(MpSclrLink_Obj *pObj,
                                    MpSclrLink_chDynamicSetOutRes *params)
{
    return FVID2_SOK;
}

Int32 MpSclrLink_drvMpProcessAllCh(MpSclrLink_Obj *pObj)
{
    Int chId, status = FVID2_SOK;
    FVID2_Frame *pFrame = NULL;
    System_LinkInQueParams *pInQueParams;
    System_FrameInfo *pFInfo;
    UInt32 i, moreFrames;
    FVID2_FrameList frameList, fwdFrames, *freeFrames;
    MpSclrLink_chDynSetHmpFwd *hmpFwd;
    /*
     1. Get the frames from the previous link
     2. For each frame received
     3.     Check if Mega Pixel sclaing is required
     4.     If so,
     4.1        queue into channels input queue
     5.     else,
     5.1        Mark the frame as NON MP frame
     5.2        Put the frame into output queue
     5.3        Let the next link know about new data
     6. If there are any MP frames that would require scaling - Post them to be
            Scaled
     */
    pInQueParams = &pObj->createArgs.inQueParams;

    freeFrames = &pObj->freeFrameList;

    freeFrames->numFrames = 0;
    fwdFrames.numFrames = 0;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                          pInQueParams->prevLinkQueId, &frameList);
    if (frameList.numFrames)
    {
        pObj->inFrameRecvCount += frameList.numFrames;

        for (i = 0; i < frameList.numFrames; i++)
        {
            pFrame = frameList.frames[i];
            UTILS_assert(pFrame != NULL);
            UTILS_assert(pFrame->appData != NULL);
            pFInfo = (System_FrameInfo *)pFrame->appData;
            hmpFwd = &pObj->hmpFwd[pFrame->channelNum];

            pFInfo->isDataTypeChange = TRUE;
            if (((pFInfo->rtChInfo.width <= MP_SCLR_LINK_MIN_WIDTH_MP_SCALING) &&
                 (pFInfo->rtChInfo.height <= MP_SCLR_LINK_MIN_HEIGHT_MP_SCALING)) ||
                 (hmpFwd->enableHmpFwdFlag == TRUE))
            {
                if (hmpFwd->enableHmpFwdFlag == TRUE)
                {
                    UTILS_assert(hmpFwd->chId == pFrame->channelNum);
                }
                /* Forwarding */
                fwdFrames.frames[fwdFrames.numFrames] = pFrame;
                fwdFrames.numFrames++;

                /* Ensure MP Flag is cleared, we cannot rely on the previous
                    link to enusre this flag is set as required */
                ((System_FrameInfo *)pFrame->appData)->isMpFrame = 0;
            }
            else
            {
                /* MP Frame - requires MP Scaling */
                /*
                 A. Check if mapped
                 B. If not mapped to a channel, map to a channel
                 C. Q up
                 D. If error due to anything, send it back to previous link
                 */
                status = FVID2_SOK;
                chId = isChanMapped(pObj, pFrame->channelNum);
                if (chId == -1)
                {
                    /* Channel not mapped, map now & check if output frame
                        buffers requies to be allocated */
                    status = mpSclrLinkMapChanAllocMem(pObj, &chId,
                                    pFrame->channelNum);
                }
                if (status == FVID2_SOK)
                {
                    /* Check if we can indeed queue the request */
                    if ((Utils_bufGetFullFrameCount(&pObj->chObj[chId].inQue)
                        < (MP_SCLR_LINK_MAX_REQUESTS_QUE_AT_IP - 0x1u)) && 
						(pObj->chObj[chId].enableOut == TRUE))
                    {
                        /* Queue for scaling, only the 31st bit used to mark
                            the frame as an MP frame. Other bits hold the
                            associated MP_SCLR channel number */
                        pFInfo->isMpFrame = chId;
                        status = Utils_bufPutFullFrame(
                                                &pObj->chObj[chId].inQue,
                                                pFrame);
                        UTILS_assert(status == FVID2_SOK);
                    }
                    else
                    {
                        /* Depth of queue exceded, return back the frame */
                        freeFrames->frames[freeFrames->numFrames] = pFrame;
                        freeFrames->numFrames++;
                    }
                }
                else
                {
                    /* No channels free, return back to previous link */
                    freeFrames->frames[freeFrames->numFrames] = pFrame;
                    freeFrames->numFrames++;
                }
            }
        }

        if (fwdFrames.numFrames)
        {
            pObj->inFrameFwdCount += fwdFrames.numFrames;
            status = Utils_bufPutFull(&pObj->linkBufQ, &fwdFrames);
            UTILS_assert(status == FVID2_SOK);
            System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                               SYSTEM_CMD_NEW_DATA);
        }

        if (freeFrames->numFrames)
        {
            pObj->inFrameNotProcessed += freeFrames->numFrames;
            status = System_putLinksEmptyFrames(
                            pObj->createArgs.inQueParams.prevLinkId,
                            pObj->createArgs.inQueParams.prevLinkQueId,
                            freeFrames);
        }
    }
    do
    {
        moreFrames = FALSE;
        /* If there are MP frames, post them them to be scaled*/
        for (chId = 0; chId < MP_SCLR_LINK_MAX_CH; chId++)
        {
            i = Utils_queGetQueuedCount(&pObj->reqQ);
            if ( i < (MP_SCLR_MAX_REQESTS_PEND_SCLR_TSK - 1))
            {
                status = Utils_bufGetFullFrame(&pObj->chObj[chId].inQue, &pFrame,
                                                BIOS_NO_WAIT);
                if ((status == FVID2_SOK) && (pFrame != NULL))
                {
                    status = Utils_quePut(&pObj->reqQ, (Ptr)pFrame, BIOS_NO_WAIT);
                    UTILS_assert(status == FVID2_SOK);
                    moreFrames = TRUE;
                }
            }
        }
    }
    while (moreFrames == TRUE);
    return status;
}

Int32 MpSclrLink_drvScaleMpFrame (MpSclrLink_Obj *pObj, FVID2_Frame *pIpFrame)
{
    Int32 status = FVID2_SOK;

    MpSclrLink_ReqObj *reqObj;

    UTILS_assert(pIpFrame != NULL);
    UTILS_assert(pIpFrame->appData != NULL);

    reqObj = mpSclrLinkDrvMakeReqObj(pObj, pIpFrame,
                            ((System_FrameInfo *)pIpFrame->appData)->isMpFrame);
    if (pObj->exitScTsk == FALSE)
    {
        UTILS_assert(reqObj != NULL);

        status = mpSclrLinkDrvScaleMpFrame(pObj, reqObj);
        UTILS_assert(status == FVID2_SOK);
    }
    return status;
}

Int32 MpSclrLink_drvCreate(MpSclrLink_Obj *pObj, MpSclrLink_CreateParams *pPrm)
{
    /*
     1. Copy the create arguments, check and default for erroneous inputs
     2. Get previos link q info and copy to links in Q info.
     2.1. Setup out queue info
     3. Determine the maximum width that can be supported
     4. Create links Q
     5. Create Scaling completed semaphore and instance lock semaphore
     6. Create Channel objects
     7. Create driver
     9. Reset instance specifics
     */
    Int32 status, chId;
    Semaphore_Params semParams;

#ifdef TI_816X_BUILD
    Vps_PlatformCpuRev cpuRev;
#endif /* TI_816X_BUILD */

    Vps_printf(" %d: MP_SCLR: Create in progress !!!\n", Utils_getCurTimeInMsec());

    /* Step 1 */
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));
    mpSclrLinkDrvCheckCreateArgs(&pObj->createArgs);

    /* Step 2 */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);
    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* Step 2.1 */
    pObj->info.numQue = 1;
    memcpy(&pObj->info.queInfo[0], &pObj->inQueInfo,
        sizeof(System_LinkQueInfo));

    /* Step 3 */
    pObj->maxWidthSupported = MP_SCLR_LINK_OUT_MAX_WIDTH;

#ifdef TI_816X_BUILD
    cpuRev = Vps_platformGetCpuRev();
#endif /* TI_816X_BUILD */

    for (chId = 0; chId < pObj->info.queInfo[0].numCh; chId++)
    {

#ifdef TI_816X_BUILD
        if (cpuRev < VPS_PLATFORM_CPU_REV_2_0)
        {
            /* Do not worry about the in-comming data type. MP SCLR will output
                422 I always, hence limit the output width. The following var
                will ensure MP SCLR will output width specified here and height
                will be 1080. */
            pObj->maxWidthSupported =
                        DEI_SC_DRV_422FMT_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0;
        }
#endif /* TI_816X_BUILD */

        /* Check if any of the channels requires to MP Scaling, if, we will have
            to tell the next link that, we will output 1920X1080 422I. As the
            next link / driver will not support beyond HD */
        if ((pObj->info.queInfo[0].chInfo[chId].width >
                MP_SCLR_LINK_MIN_WIDTH_MP_SCALING) ||
            (pObj->info.queInfo[0].chInfo[chId].height >
                MP_SCLR_LINK_MIN_HEIGHT_MP_SCALING))
        {
            pObj->info.queInfo[0].chInfo[chId].width = pObj->maxWidthSupported;
            pObj->info.queInfo[0].chInfo[chId].height =
                        MP_SCLR_LINK_OUT_MAX_HEIGHT;
            pObj->info.queInfo[0].chInfo[chId].pitch[0] = VpsUtils_align
                        (pObj->maxWidthSupported * 2, VPS_BUFFER_ALIGNMENT);
            pObj->info.queInfo[0].chInfo[chId].bufType =
                        SYSTEM_BUF_TYPE_VIDFRAME;
            pObj->info.queInfo[0].chInfo[chId].scanFormat =
                        SYSTEM_SF_PROGRESSIVE;
            pObj->info.queInfo[0].chInfo[chId].bufType =
                        SYSTEM_BUF_TYPE_VIDFRAME;
            pObj->info.queInfo[0].chInfo[chId].dataFormat =
                        MP_SCLR_LINK_OUT_DATATYPE;
            pObj->info.queInfo[0].chInfo[chId].memType = SYSTEM_MT_NONTILEDMEM;
        }
    }

    /* Step 4 */

    /* Its expected to pend on queue get, as if there are not output buffers
        we do not plan to return the input frames, instead the new frames
        that come in, would be released. */
    status = Utils_bufCreate(&pObj->linkBufQ, TRUE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    /* Again, we would require to block, when there are no requests object.
        Ideally, if we could get an output buffer, we should be able to get an
        request object. */
    status = Utils_queCreate(&pObj->reqQ,
                                MP_SCLR_MAX_REQESTS_PEND_SCLR_TSK,
                                pObj->memReqQ,
                                UTILS_QUE_FLAG_BLOCK_QUE_GET);
    UTILS_assert(status == FVID2_SOK);

    status = Utils_queCreate(&pObj->freeReqQ,
                                MP_SCLR_MAX_REQESTS_OBJECTS,
                                pObj->memFreeReqQ,
                                UTILS_QUE_FLAG_BLOCK_QUE_GET);
    UTILS_assert(status == FVID2_SOK);
    for(chId = 0; chId < MP_SCLR_MAX_REQESTS_OBJECTS; chId++)
    {
        status = Utils_quePut(&pObj->freeReqQ,
                              &pObj->reqObjs[chId], BIOS_NO_WAIT);
        UTILS_assert(!UTILS_ISERROR(status));

    }
    status = Utils_queCreate(&pObj->compReqQ,
                                MP_SCLR_MAX_REQESTS_OBJECTS,
                                pObj->memCompReqQ,
                                UTILS_QUE_FLAG_NO_BLOCK_QUE);
    UTILS_assert(status == FVID2_SOK);

    /* Step 5 */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->scalingDone = Semaphore_create(0u, &semParams, NULL);
    UTILS_assert(pObj->scalingDone != NULL);

    /* Step 6 */
    status = mpSclrLinkDvrChCreate(pObj);
    UTILS_assert(status == FVID2_SOK);

    /* Step 7 */
    status = mpSclrLinkDvrHndlCreate(pObj);
    UTILS_assert(status == FVID2_SOK);

    /* Step 9 */
    status = mpSclrLinkDrvResetInstVars(pObj);
    UTILS_assert(status == FVID2_SOK);

    Vps_printf(" %d: MP_SCLR: Create Done !!!\n", Utils_getCurTimeInMsec());

    return status;
}

Int32 MpSclrLink_drvDynamicSetHmpFwd(MpSclrLink_Obj *pObj,
                                     MpSclrLink_chDynSetHmpFwd *pPrm)
{
    Int32 status = FVID2_SOK;
    MpSclrLink_chDynSetHmpFwd *hmpFwdPrm;

    UTILS_assert (pPrm->chId <= MP_SCLR_LINK_MAX_FWD_CH);
    hmpFwdPrm = &pObj->hmpFwd[pPrm->chId];

    hmpFwdPrm->enableHmpFwdFlag = pPrm->enableHmpFwdFlag;
    hmpFwdPrm->chId = pPrm->chId;

    return status;
}

Int32 MpSclrLink_drvSetChannelInfo(MpSclrLink_Obj * pObj, MpSclrLink_ChannelInfo *pChannelInfo)
{
	Int32 status = FVID2_SOK,chId;
    MpSclrLink_ChObj *pChObj;

	chId = isChanMapped(pObj, pChannelInfo->channelId);	
	
	if(chId != -1)
	{
		pChObj = &pObj->chObj[chId];
		pChObj->enableOut = pChannelInfo->enable;
	}

    return status;
}
