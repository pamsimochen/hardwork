 /*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "grpxLink_priv.h"

static Void vpsGrpxSetParams(GrpxLink_Obj *pObj)
{
    Vps_GrpxRtParams    *grpxRegPrm;;
    pObj->grpxParamsList.numRegions = 1u;
    pObj->grpxParamsList.gParams = &pObj->grpxRegionParams[0];
    pObj->grpxParamsList.clutPtr = NULL;
    pObj->grpxParamsList.scParams = NULL;
    grpxRegPrm = &pObj->grpxRegionParams[0];

    grpxRegPrm->regId = 0;
    grpxRegPrm->format = pObj->grpxFormat.dataFormat;
    grpxRegPrm->pitch[0] = pObj->grpxFormat.pitch[0];
    grpxRegPrm->pitch[1] = pObj->grpxFormat.pitch[1];
    grpxRegPrm->pitch[2] = pObj->grpxFormat.pitch[2];
    grpxRegPrm->rotation = VPS_MEM_0_ROTATION;
    grpxRegPrm->scParams = NULL;
    grpxRegPrm->stenPtr = NULL;
    grpxRegPrm->stenPitch = 16;

    /*all grpxRegPrm share the same regId, this is only for the single region display*/
    grpxRegPrm->regParams.regionWidth = pObj->grpxFormat.width;
    grpxRegPrm->regParams.regionHeight = pObj->grpxFormat.height;
    grpxRegPrm->regParams.regionPosX = 0;
    grpxRegPrm->regParams.regionPosY = 0;
    grpxRegPrm->regParams.dispPriority = 1;
    grpxRegPrm->regParams.firstRegion = 1u;
    grpxRegPrm->regParams.lastRegion = 1u;
    grpxRegPrm->regParams.scEnable = 0;
    grpxRegPrm->regParams.stencilingEnable = 0;
    grpxRegPrm->regParams.bbEnable = 0;
    grpxRegPrm->regParams.bbAlpha = 0;
    grpxRegPrm->regParams.blendAlpha = 0;
    grpxRegPrm->regParams.blendType = VPS_GRPX_BLEND_NO;
    grpxRegPrm->regParams.transEnable = 0;
    grpxRegPrm->regParams.transType = VPS_GRPX_TRANS_NO_MASK;
    grpxRegPrm->regParams.transColorRgb24 = 0xFFFFFF;

#ifdef SYSTEM_VERBOSE_PRINTS
    Vps_printf(" %d: GRPX: %d: Window size %dx%d, %dB",
        Clock_getTicks(),
        pObj->grpxInstId, grpxRegPrm->regParams.regionWidth,
        grpxRegPrm->regParams.regionHeight,
        grpxRegPrm->pitch[0]
    );
#endif
}

Int32 GrpxLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    GrpxLink_Obj *pObj = (GrpxLink_Obj *)appData;
    UInt32 elaspedTime, curTime;

    VpsUtils_tskSendCmd(&pObj->tsk, GRPX_LINK_CMD_DO_DEQUE);

    pObj->cbCount++;

    curTime = Clock_getTicks();

    if(pObj->cbCount>10)
    {
        elaspedTime = curTime - pObj->lastCbTime;

        if(elaspedTime>pObj->maxCbTime)
            pObj->maxCbTime = elaspedTime;

        if(elaspedTime<pObj->minCbTime)
            pObj->minCbTime = elaspedTime;
    }

    pObj->lastCbTime = curTime;

    return FVID2_SOK;
}

Int32 GrpxLink_drvSwitchCh(GrpxLink_Obj *pObj, UInt32 channelNum)
{
    GrpxLink_drvLock(pObj);

    pObj->curGrpxChannelNum  = channelNum;

    GrpxLink_drvUnlock(pObj);

    return FVID2_SOK;
}

Int32 GrpxLink_drvGrpxAllocAndQueBlankFrame(GrpxLink_Obj *pObj)
{
    FVID2_FrameList frameList;
    UInt32 numQue;
    Int32 status;

    status = VpsUtils_memFrameAlloc(&pObj->grpxFormat, &pObj->blankFrame, 1);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->blankFrame.channelNum = GRPX_LINK_BLANK_FRAME_CHANNEL_NUM;

    for(numQue=0; numQue<GRPX_LINK_BLANK_FRAME_INIT_QUE; numQue++)
    {
        frameList.numFrames = 1;
        frameList.frames[0] = &pObj->blankFrame;
        frameList.perListCfg = NULL;

        pObj->blankFrame.addr[1][0] = (UInt8*)pObj->blankFrame.addr[0][0] + pObj->grpxFormat.pitch[0];

        status = FVID2_queue(
                    pObj->grpxHndl,
                    &frameList,
                    0
                    );
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
        pObj->queueCount++;
    }

    return status;
}

Int32 GrpxLink_drvGrpxCreate(GrpxLink_Obj *pObj)
{
    FVID2_CbParams cbParams;
    System_LinkChInfo *pInChInfo;
    FVID2_Format *pFormat;
    Int32 status;

    pInChInfo = &pObj->inQueInfo.chInfo[0];

    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn = GrpxLink_drvFvidCb;
    cbParams.appData = pObj;

    pObj->grpxCreateArgs.memType = pInChInfo->memType;
    pObj->grpxCreateArgs.drvMode = VPS_GRPX_STREAMING_MODE;
    pObj->grpxCreateArgs.periodicCallbackEnable = TRUE;

    pFormat = &pObj->grpxFormat;

    pFormat->channelNum       = 0;
    pFormat->width            = pInChInfo->width;
    pFormat->height           = pInChInfo->height;
    pFormat->pitch[0]         = pInChInfo->pitch[0];
    pFormat->pitch[1]         = pInChInfo->pitch[1];
    pFormat->pitch[2]         = pInChInfo->pitch[2];
    pFormat->dataFormat       = pInChInfo->dataFormat;

    switch(pObj->tskId)
    {
        default:
        case SYSTEM_LINK_ID_GRPX_0:
            pObj->grpxInstId = VPS_DISP_INST_GRPX0;
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
            break;
        case SYSTEM_LINK_ID_GRPX_1:
            pObj->grpxInstId = VPS_DISP_INST_GRPX1;
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
            break;
        case SYSTEM_LINK_ID_GRPX_2:
            pObj->grpxInstId = VPS_DISP_INST_GRPX2;
            pFormat->scanFormat = FVID2_SF_INTERLACED;
            break;
    }

    pFormat->fieldMerged[0]   = FALSE;

    if(pObj->createArgs.displayRes==SYSTEM_DISPLAY_RES_1080I60
    || pObj->createArgs.displayRes==SYSTEM_DISPLAY_RES_NTSC
    || pObj->createArgs.displayRes==SYSTEM_DISPLAY_RES_PAL
    )
    {
        pFormat->scanFormat = FVID2_SF_INTERLACED;
        pFormat->fieldMerged[0]   = TRUE;
    }
    if(pObj->createArgs.displayRes==SYSTEM_DISPLAY_RES_NTSC)
    {
        if(pFormat->width>720)
            pFormat->width = 720;
        if(pFormat->height>480)
            pFormat->height = 480;
    }
    if(pObj->createArgs.displayRes==SYSTEM_DISPLAY_RES_PAL)
    {
        if(pFormat->width>720)
            pFormat->width = 720;
        if(pFormat->height>576)
            pFormat->height = 576;
    }



    pFormat->fieldMerged[1]   = pFormat->fieldMerged[0];
    pFormat->fieldMerged[2]   = pFormat->fieldMerged[0];
    pFormat->bpp              = FVID2_BPP_BITS16;
    pFormat->reserved         = NULL;

#ifdef SYSTEM_VERBOSE_PRINTS
    Vps_printf(" %d: GRPX: %d: Window size %dx%d, %dB",
        Clock_getTicks(),
        pObj->grpxInstId, pFormat->width, pFormat->height, pFormat->pitch[0]
    );
#endif

    pObj->grpxHndl = FVID2_create(
                            FVID2_VPS_DISP_GRPX_DRV,
                            pObj->grpxInstId,
                            &pObj->grpxCreateArgs,
                            &pObj->grpxCreateStatus,
                            &cbParams
                        );
    GT_assert( GT_DEFAULT_MASK, pObj->grpxHndl!=NULL);

    status = FVID2_setFormat(pObj->grpxHndl, pFormat);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    vpsGrpxSetParams(pObj);
    status = FVID2_control(pObj->grpxHndl,
                            IOCTL_VPS_SET_GRPX_PARAMS,
                            &pObj->grpxParamsList,
                            NULL);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->queueCount = 0;

    return FVID2_SOK;
}

Int32 GrpxLink_drvCreate(GrpxLink_Obj *pObj, GrpxLink_CreateParams *pPrm)
{
    Semaphore_Params semParams;

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Create in progress !!!\n", Clock_getTicks());
    #endif

    pObj->cbCount = 0;
    pObj->dequeCount = 0;
    pObj->totalTime = 0;
    pObj->minCbTime = 0xFFFF;
    pObj->maxCbTime = 0;
    pObj->lastCbTime = 0;


    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert( GT_DEFAULT_MASK, pObj->pInTskInfo != NULL);
    GT_assert( GT_DEFAULT_MASK, pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo)
            );

    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->lock = Semaphore_create ( 1u, &semParams, NULL );
    GT_assert( GT_DEFAULT_MASK, pObj->lock!=NULL);

    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->complete = Semaphore_create ( 0u, &semParams, NULL );
    GT_assert( GT_DEFAULT_MASK, pObj->complete!=NULL);

    GrpxLink_drvGrpxCreate(pObj);
    GrpxLink_drvGrpxAllocAndQueBlankFrame(pObj);
    GrpxLink_drvSwitchCh(pObj, 0);

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Create Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}


Int32 GrpxLink_drvPrintRtStatus(GrpxLink_Obj *pObj, UInt32 elaspedTime)
{
    char *grpxName[] = { "HDDAC(GRPX0) ", "DVO2(GRPX1)  ", "SDDAC(GRPX2)" };

    Vps_rprintf(" %d: GRPX: %s: %d fps, Callback Interval: Max = %d, Min = %d !!! \r\n",
        Clock_getTicks(),
        grpxName[pObj->grpxInstId],
        pObj->cbCount*1000/elaspedTime,
        pObj->maxCbTime,
        pObj->minCbTime
    );

    #if 0
    /* reset max time */
    pObj->maxCbTime = pObj->minCbTime;
    #endif

    return 0;
}

Int32 GrpxLink_drvProcessData(GrpxLink_Obj *pObj)
{
    FVID2_FrameList frameList;
    FVID2_FrameList freeFrameList;
    FVID2_FrameList grpxFrameList;
    UInt32 freeFrameNum, frameId, grpxFrameId, elaspedTime;
    System_LinkInQueParams *pInQueParams;
    FVID2_Frame *pFrame;
    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    freeFrameNum = 0;


    elaspedTime = Clock_getTicks() - pObj->startTime;

    if((elaspedTime - pObj->prevTime) > 30*1000)
    {
        GrpxLink_drvPrintRtStatus(pObj, elaspedTime);

        pObj->prevTime = elaspedTime;
    }

    do
    {
        grpxFrameList.numFrames = 0;

        /* dequeue all completed frames */
        status = FVID2_dequeue(
                    pObj->grpxHndl,
                    &grpxFrameList,
                    0,
                    BIOS_NO_WAIT
                );
        if ((status !=  FVID2_SOK) && (grpxFrameList.numFrames != 0))
        {
            GT_assert(GT_DEFAULT_MASK, FALSE);
        }
        pObj->dequeCount += grpxFrameList.numFrames;

        for(grpxFrameId=0; grpxFrameId<grpxFrameList.numFrames; grpxFrameId++)
        {
            pFrame = grpxFrameList.frames[grpxFrameId];
            if(pFrame->channelNum!=GRPX_LINK_BLANK_FRAME_CHANNEL_NUM)
            {
                freeFrameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        if(grpxFrameList.numFrames)
        {
            #ifdef SYSTEM_DEBUG_GRPX_RT
            Vps_printf(" %d: GRPX: Deque %d frames\n", Clock_getTicks(), grpxFrameList.numFrames);
            #endif
        }

    } while(status==FVID2_SOK);

    /* que frames if any */

    System_getLinksFullFrames(
        pInQueParams->prevLinkId,
        pInQueParams->prevLinkQueId,
        &frameList
       );

    if(frameList.numFrames)
    {
        GrpxLink_drvLock(pObj);

        for(frameId=0; frameId<frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            if(pFrame->channelNum==pObj->curGrpxChannelNum)
            {
                /* queue frame for grpx */
                grpxFrameList.numFrames = 0;

                grpxFrameList.frames[grpxFrameList.numFrames] = pFrame;
                grpxFrameList.numFrames++;
                grpxFrameList.perListCfg = NULL;

                pFrame->addr[1][0] = (UInt8*)pFrame->addr[0][0] + pObj->grpxFormat.pitch[0];

                #ifdef SYSTEM_DEBUG_GRPX_RT
                Vps_printf(" %d: GRPX: Queue %d frames\n", Clock_getTicks(), grpxFrameList.numFrames);
                #endif

                status = FVID2_queue(
                            pObj->grpxHndl,
                            &grpxFrameList,
                            0
                            );
                GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

                pObj->queueCount++;
            }
            else {
                freeFrameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        GrpxLink_drvUnlock(pObj);
    }

    if(freeFrameNum)
    {
        freeFrameList.numFrames = freeFrameNum;

        System_putLinksEmptyFrames(pInQueParams->prevLinkId, pInQueParams->prevLinkQueId, &freeFrameList);
    }

    return FVID2_SOK;
}


Int32 GrpxLink_drvDelete(GrpxLink_Obj *pObj)
{
    FVID2_FrameList frameList;
    Int32 status;

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: GRPX: Frames = %d (fps = %d) !!!\n",
        Clock_getTicks(),
        pObj->dequeCount,
        pObj->dequeCount*100/(pObj->totalTime/10)
        );
    #endif

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Delete in progress !!!\n", Clock_getTicks());
    #endif

    do
    {
        // dequeue queued buffer's
        status = FVID2_dequeue(
                    pObj->grpxHndl,
                    &frameList,
                    0,
                    BIOS_NO_WAIT
                );
    } while(status==FVID2_SOK);

    FVID2_delete(pObj->grpxHndl, NULL);

    Semaphore_delete(&pObj->lock);
    Semaphore_delete(&pObj->complete);

    status = VpsUtils_memFrameFree(&pObj->grpxFormat, &pObj->blankFrame, 1);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Delete Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

Int32 GrpxLink_drvStart(GrpxLink_Obj *pObj)
{
    Int32 status=FVID2_SOK;

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Start in progress !!!\n", Clock_getTicks());
    #endif

    pObj->lastCbTime = Clock_getTicks();
    pObj->startTime  = Clock_getTicks();
    pObj->prevTime   = pObj->startTime;

    status = FVID2_start(pObj->grpxHndl, NULL);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->totalTime = Clock_getTicks();


    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Start Done !!!\n", Clock_getTicks());
    #endif

    return status;
}

Int32 GrpxLink_drvStop(GrpxLink_Obj *pObj)
{
    Int32 status=FVID2_SOK;

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Stop in progress !!!\n", Clock_getTicks());
    #endif

    pObj->totalTime = Clock_getTicks() - pObj->totalTime;

    status = FVID2_stop(pObj->grpxHndl, NULL);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    #ifdef SYSTEM_DEBUG_GRPX
    Vps_printf(" %d: GRPX: Stop Done !!!\n", Clock_getTicks());
    #endif

    return status;
}

Int32 GrpxLink_drvLock(GrpxLink_Obj *pObj)
{
    return Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
}

Int32 GrpxLink_drvUnlock(GrpxLink_Obj *pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

