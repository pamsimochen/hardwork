/** ==================================================================
 *  @file   swMsLink_drv.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/swMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "swMsLink_priv.h"

/* ===================================================================
 *  @func     SwMsLink_drvTimerCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void SwMsLink_drvTimerCb(UArg arg)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) arg;

    VpsUtils_tskSendCmd(&pObj->tsk, SW_MS_LINK_CMD_DO_SCALING);
}

/* ===================================================================
 *  @func     SwMsLink_drvDeiFvidCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvDeiFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) appData;

    Semaphore_post(pObj->deiDrvObj.complete);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvDeiBypassFvidCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvDeiBypassFvidCb(FVID2_Handle handle, Ptr appData,
                                  Ptr reserved)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) appData;

    Semaphore_post(pObj->deiBypassDrvObj.complete);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvScFvidCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvScFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) appData;

    Semaphore_post(pObj->scDrvObj.complete);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvFvidErrCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvFvidErrCb(FVID2_Handle handle,
                            Ptr appData, Ptr errList, Ptr reserved)
{
    return FVID2_SOK;
}

Int32 SwMsLink_drvSwitchLayout
    (SwMsLink_Obj * pObj, SwMsLink_LayoutParams * layoutParams) {
    UInt32 winId, chNum;

    SwMsLink_OutWinObj *pWinObj;

    SwMsLink_drvLock(pObj);

    if (pObj->layoutPrm.outLayoutMode != layoutParams->outLayoutMode &&
        pObj->layoutPrm.outLayoutMode != SYSTEM_LAYOUT_MODE_MAX)
    {
        pObj->switchLayout = TRUE;
        pObj->skipProcessing = SW_MS_SKIP_PROCESSING;
    }

    memcpy(&pObj->layoutPrm, layoutParams, sizeof(*layoutParams));

    SwMsLink_getLayoutInfo(&pObj->layoutPrm,
                           &pObj->layoutInfo, pObj->outFrameFormat.pitch[0]);

    for (winId = 0; winId < pObj->layoutInfo.numWin; winId++)
    {
        pWinObj = &pObj->winObj[winId];

        pWinObj->scRtOutFrmPrm.width = pObj->layoutInfo.winInfo[winId].width;
        pWinObj->scRtOutFrmPrm.height = pObj->layoutInfo.winInfo[winId].height;
        pWinObj->scRtOutFrmPrm.pitch[0] = pObj->outFrameFormat.pitch[0];
        pWinObj->scRtOutFrmPrm.pitch[1] = pObj->outFrameFormat.pitch[1];
        pWinObj->scRtOutFrmPrm.pitch[2] = pObj->outFrameFormat.pitch[2];
        pWinObj->scRtOutFrmPrm.memType =
            pObj->info.queInfo[0].chInfo[0].memType;

        if (pObj->isDei)
        {
            memset(&pWinObj->deiRtPrm, 0, sizeof(pWinObj->deiRtPrm));

            pWinObj->deiRtPrm.deiOutFrmPrms = &pWinObj->scRtOutFrmPrm;
            pWinObj->deiRtPrm.deiRtCfg = &pWinObj->deiRtCfg;

            pWinObj->deiRtCfg.resetDei = FALSE;
            pWinObj->deiRtCfg.fldRepeat = FALSE;
        }
        else
        {
            memset(&pWinObj->scRtPrm, 0, sizeof(pWinObj->scRtPrm));

            pWinObj->scRtPrm.outFrmPrms = &pWinObj->scRtOutFrmPrm;
            pWinObj->scRtPrm.scCfg = &pWinObj->scRtCfg;

            chNum = pObj->layoutInfo.winInfo[winId].channelNum;

            /* In case captured data is progressive and window is bypass
             * (don't care quality so much), SC takes only even lines and
             * make S/W mosaic. This is sometimes needed due to SC
             * performance. */
            if ((FVID2_SF_PROGRESSIVE ==
                 pObj->inQueInfo.chInfo[chNum].scanFormat) &&
                pObj->layoutInfo.winInfo[winId].bypass)
            {
                pWinObj->scRtPrm.inFrmPrms = &pWinObj->scRtInFrmPrm;

                pWinObj->scRtInFrmPrm.width =
                    pObj->inQueInfo.chInfo[chNum].width;
                pWinObj->scRtInFrmPrm.height =
                    pObj->inQueInfo.chInfo[chNum].height / 2;
                pWinObj->scRtInFrmPrm.pitch[0] =
                    pObj->inQueInfo.chInfo[chNum].pitch[0] * 2;
                pWinObj->scRtInFrmPrm.pitch[1] =
                    pObj->inQueInfo.chInfo[chNum].pitch[1] * 2;
                pWinObj->scRtInFrmPrm.pitch[2] =
                    pObj->inQueInfo.chInfo[chNum].pitch[2] * 2;
                pWinObj->scRtInFrmPrm.memType =
                    pObj->inQueInfo.chInfo[chNum].memType;
            }
            else
            {
                pWinObj->scRtPrm.inFrmPrms = NULL;
            }
        }

        pWinObj->applyRtPrm = TRUE;
    }

    SwMsLink_drvUnlock(pObj);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvCreateOutInfo                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvCreateOutInfo(SwMsLink_Obj * pObj, UInt32 outRes)
{
    Int32 status;

    System_LinkChInfo *pChInfo;

    UInt32 frameId, bufferPitch;

    UInt16 bufferWidth, bufferHeight;

    pObj->info.numQue = 1;
    pObj->info.queInfo[0].numCh = 1;

    pChInfo = &pObj->info.queInfo[0].chInfo[0];

    pChInfo->dataFormat = FVID2_DF_YUV422I_YUYV;
    pChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;

    SwMsLink_getOutSize(SYSTEM_DISPLAY_RES_MAX, &bufferWidth, &bufferHeight);

    bufferPitch = VpsUtils_align(bufferWidth, VPS_BUFFER_ALIGNMENT) * 2;

    SwMsLink_getOutSize(outRes, &pChInfo->width, &pChInfo->height);

    pChInfo->pitch[0] = bufferPitch;
    pChInfo->pitch[1] = pChInfo->pitch[2] = 0;

    pChInfo->scanFormat = FVID2_SF_PROGRESSIVE;

    pObj->bufferFrameFormat.channelNum = 0;
    pObj->bufferFrameFormat.width = bufferWidth;
    pObj->bufferFrameFormat.height = bufferHeight;
    pObj->bufferFrameFormat.pitch[0] = pChInfo->pitch[0];
    pObj->bufferFrameFormat.pitch[1] = pChInfo->pitch[1];
    pObj->bufferFrameFormat.pitch[2] = pChInfo->pitch[2];
    pObj->bufferFrameFormat.fieldMerged[0] = FALSE;
    pObj->bufferFrameFormat.fieldMerged[1] = FALSE;
    pObj->bufferFrameFormat.fieldMerged[2] = FALSE;
    pObj->bufferFrameFormat.dataFormat = pChInfo->dataFormat;
    pObj->bufferFrameFormat.scanFormat = pChInfo->scanFormat;
    pObj->bufferFrameFormat.bpp = FVID2_BPP_BITS16;
    pObj->bufferFrameFormat.reserved = NULL;

    status = VpsUtils_bufCreate(&pObj->bufOutQue, TRUE, FALSE);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    /* alloc buffer of max possible size but use only what is needed for a
     * given resolution */
    status = VpsUtils_memFrameAlloc(&pObj->bufferFrameFormat,
                                    pObj->outFrames, SW_MS_LINK_MAX_OUT_FRAMES);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    /* set actual required width x height */
    pObj->outFrameFormat = pObj->bufferFrameFormat;
    pObj->outFrameFormat.width = pChInfo->width;
    pObj->outFrameFormat.height = pChInfo->height;

    for (frameId = 0; frameId < SW_MS_LINK_MAX_OUT_FRAMES; frameId++)
    {
        status = VpsUtils_bufPutEmptyFrame(&pObj->bufOutQue,
                                           &pObj->outFrames[frameId]);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        pObj->outFrames[frameId].appData = &pObj->outFrameInfo[frameId];
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvCreateWinObj                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvCreateWinObj(SwMsLink_Obj * pObj, UInt32 winId)
{
    SwMsLink_OutWinObj *pWinObj;

    Int32 status;

    pWinObj = &pObj->winObj[winId];

    /* assume all CHs are of same input size, format, pitch */
    pWinObj->scanFormat = pObj->inQueInfo.chInfo[0].scanFormat;
    pWinObj->expectedFid = 0;
    pWinObj->applyRtPrm = FALSE;

    if (winId == 0)
    {
        /* alloc buffer of max possible size as input blank buffer */
        status = VpsUtils_memFrameAlloc(&pObj->bufferFrameFormat,
                                        &pWinObj->blankFrame, 1);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        pWinObj->blankFrame.addr[0][1] = pWinObj->blankFrame.addr[0][0];
    }
    else
    {
        pWinObj->blankFrame = pObj->winObj[0].blankFrame;
    }

    pWinObj->blankFrame.channelNum = winId;

    memset(&pWinObj->curOutFrame, 0, sizeof(pWinObj->curOutFrame));
    pWinObj->curOutFrame.addr[0][0] = NULL;
    pWinObj->curOutFrame.channelNum = winId;

    pWinObj->pCurInFrame = NULL;

    pWinObj->repeatFrameCount = 0;

    status = VpsUtils_bufCreate(&pWinObj->bufInQue, FALSE, FALSE);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvAllocCtxMem                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvAllocCtxMem(SwMsLink_DeiDrvObj * pObj)
{
    Int32 retVal = FVID2_SOK;

    Vps_DeiCtxInfo deiCtxInfo;

    Vps_DeiCtxBuf deiCtxBuf;

    UInt32 chCnt, bCnt;

    for (chCnt = 0u; chCnt < pObj->deiCreateParams.numCh; chCnt++)
    {
        /* Get the number of buffers to allocate */
        deiCtxInfo.channelNum = chCnt;
        retVal = FVID2_control(pObj->fvidHandle,
                               IOCTL_VPS_GET_DEI_CTX_INFO, &deiCtxInfo, NULL);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);

        /* Allocate the buffers as requested by the driver */
        for (bCnt = 0u; bCnt < deiCtxInfo.numFld; bCnt++)
        {
            deiCtxBuf.fldBuf[bCnt] = VpsUtils_memAlloc(deiCtxInfo.fldBufSize,
                                                       VPS_BUFFER_ALIGNMENT);
            GT_assert(GT_DEFAULT_MASK, NULL != deiCtxBuf.fldBuf[bCnt]);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMv; bCnt++)
        {
            deiCtxBuf.mvBuf[bCnt] = VpsUtils_memAlloc(deiCtxInfo.mvBufSize,
                                                      VPS_BUFFER_ALIGNMENT);
            GT_assert(GT_DEFAULT_MASK, NULL != deiCtxBuf.mvBuf[bCnt]);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMvstm; bCnt++)
        {
            deiCtxBuf.mvstmBuf[bCnt] =
                VpsUtils_memAlloc(deiCtxInfo.mvstmBufSize,
                                  VPS_BUFFER_ALIGNMENT);
            GT_assert(GT_DEFAULT_MASK, NULL != deiCtxBuf.mvstmBuf[bCnt]);
        }

        /* Provided the allocated buffer to driver */
        deiCtxBuf.channelNum = chCnt;
        retVal = FVID2_control(pObj->fvidHandle,
                               IOCTL_VPS_SET_DEI_CTX_BUF, &deiCtxBuf, NULL);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);
    }

    return (retVal);
}

/* ===================================================================
 *  @func     SwMsLink_drvFreeCtxMem                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvFreeCtxMem(SwMsLink_DeiDrvObj * pObj)
{
    Int32 retVal = FVID2_SOK;

    Vps_DeiCtxInfo deiCtxInfo;

    Vps_DeiCtxBuf deiCtxBuf;

    UInt32 chCnt, bCnt;

    for (chCnt = 0u; chCnt < pObj->deiCreateParams.numCh; chCnt++)
    {
        /* Get the number of buffers to allocate */
        deiCtxInfo.channelNum = chCnt;
        retVal = FVID2_control(pObj->fvidHandle,
                               IOCTL_VPS_GET_DEI_CTX_INFO, &deiCtxInfo, NULL);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);

        /* Get the allocated buffer back from the driver */
        deiCtxBuf.channelNum = chCnt;
        retVal = FVID2_control(pObj->fvidHandle,
                               IOCTL_VPS_GET_DEI_CTX_BUF, &deiCtxBuf, NULL);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);

        /* Free the buffers */
        for (bCnt = 0u; bCnt < deiCtxInfo.numFld; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.fldBuf[bCnt], deiCtxInfo.fldBufSize);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMv; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.mvBuf[bCnt], deiCtxInfo.mvBufSize);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMvstm; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.mvstmBuf[bCnt], deiCtxInfo.mvstmBufSize);
        }
    }

    return (retVal);
}

Int32 SwMsLink_drvSetScCoeffs
    (FVID2_Handle fvidHandle, Bool loadUpsampleCoeffs, Bool isDei) {
    Int32 retVal = FVID2_SOK;

    Vps_ScCoeffParams coeffPrms;

    if (loadUpsampleCoeffs)
    {
        Vps_rprintf(" %d: SWMS    : Loading Up-scaling Co-effs\n",
                    Clock_getTicks());

        coeffPrms.hScalingSet = VPS_SC_US_SET;
        coeffPrms.vScalingSet = VPS_SC_US_SET;
    }
    else
    {
        Vps_rprintf(" %d: SWMS    : Loading Down-scaling Co-effs\n",
                    Clock_getTicks());

        coeffPrms.hScalingSet = VPS_SC_DS_SET_0;
        coeffPrms.vScalingSet = VPS_SC_DS_SET_0;
    }
    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = isDei
        ? VPS_M2M_DEI_SCALAR_ID_DEI_SC : VPS_M2M_SC_SCALAR_ID_DEFAULT;

    /* Program DEI scalar coefficient - Always used */
    retVal = FVID2_control(fvidHandle, IOCTL_VPS_SET_COEFFS, &coeffPrms, NULL);
    GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);

    return (retVal);
}

/* ===================================================================
 *  @func     SwMsLink_drvCreateDeiDrv                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvCreateDeiDrv(SwMsLink_Obj * pObj, Bool bypassDrv)
{
    Semaphore_Params semParams;

    SwMsLink_DeiDrvObj *pDrvObj;

    Vps_M2mDeiChParams *pDrvChPrm;

    UInt32 winId;

    System_LinkChInfo *pChInfo;

    FVID2_CbParams cbParams;

    if (bypassDrv)
    {
        pDrvObj = &pObj->deiBypassDrvObj;
    }
    else
    {
        pDrvObj = &pObj->deiDrvObj;
    }

    pDrvObj->bypassDei = bypassDrv;

#ifdef TI_816X_BUILD
    switch (pObj->linkId)
    {
        case SYSTEM_LINK_ID_SW_MS_DEI_0:
            pDrvObj->drvInstId = VPS_M2M_INST_AUX_DEI_SC2_WB1;
            break;
        case SYSTEM_LINK_ID_SW_MS_DEI_HQ_0:
            pDrvObj->drvInstId = VPS_M2M_INST_MAIN_DEIH_SC1_WB0;
            break;
        default:
            GT_assert(GT_DEFAULT_MASK, 0);
    }
#endif                                                     /* TI_816X_BUILD */

    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pDrvObj->complete = Semaphore_create(0u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pDrvObj->complete != NULL);

    pDrvObj->deiCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    if (bypassDrv)
    {
        pDrvObj->deiCreateParams.numCh = SYSTEM_SW_MS_MAX_WIN;
    }
    else
    {
        pDrvObj->deiCreateParams.numCh = SW_MS_MAX_DEI_CH;
    }
    pDrvObj->deiCreateParams.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;
    pDrvObj->deiCreateParams.chParams =
        (const Vps_M2mDeiChParams *) pDrvObj->deiChParams;
    pDrvObj->deiCreateParams.isVipScReq = FALSE;

    for (winId = 0; winId < pDrvObj->deiCreateParams.numCh; winId++)
    {
        pDrvChPrm = &pDrvObj->deiChParams[winId];

        /* assume all CHs are of same input size, format, pitch */
        pChInfo = &pObj->inQueInfo.chInfo[0];

        pDrvChPrm->inFmt.channelNum = winId;
        pDrvChPrm->inFmt.width = pChInfo->width;
        pDrvChPrm->inFmt.height = pChInfo->height;
        pDrvChPrm->inFmt.pitch[0] = pChInfo->pitch[0];
        pDrvChPrm->inFmt.pitch[1] = pChInfo->pitch[1];
        pDrvChPrm->inFmt.pitch[2] = pChInfo->pitch[2];
        pDrvChPrm->inFmt.fieldMerged[0] = FALSE;
        pDrvChPrm->inFmt.fieldMerged[1] = FALSE;
        pDrvChPrm->inFmt.fieldMerged[0] = FALSE;
        pDrvChPrm->inFmt.dataFormat = pChInfo->dataFormat;
        if (bypassDrv)
        {
            pDrvChPrm->inFmt.scanFormat = FVID2_SF_PROGRESSIVE;
        }
        else
        {
            pDrvChPrm->inFmt.scanFormat = FVID2_SF_INTERLACED;
        }
        pDrvChPrm->inFmt.bpp = FVID2_BPP_BITS16;

        pDrvChPrm->outFmtDei = &pDrvObj->drvOutFormat[winId];
        pDrvChPrm->outFmtDei->channelNum = winId;
        pDrvChPrm->outFmtDei->width = pChInfo->width;
        pDrvChPrm->outFmtDei->height = pChInfo->height;
        pDrvChPrm->outFmtDei->pitch[0] = pObj->outFrameFormat.pitch[0];
        pDrvChPrm->outFmtDei->pitch[1] = pObj->outFrameFormat.pitch[1];
        pDrvChPrm->outFmtDei->pitch[2] = pObj->outFrameFormat.pitch[2];
        pDrvChPrm->outFmtDei->fieldMerged[0] = FALSE;
        pDrvChPrm->outFmtDei->fieldMerged[1] = FALSE;
        pDrvChPrm->outFmtDei->fieldMerged[0] = FALSE;
        pDrvChPrm->outFmtDei->dataFormat = pObj->outFrameFormat.dataFormat;
        pDrvChPrm->outFmtDei->scanFormat = FVID2_SF_PROGRESSIVE;
        pDrvChPrm->outFmtDei->bpp = pObj->outFrameFormat.bpp;

        pDrvChPrm->inMemType = pChInfo->memType;
        pDrvChPrm->outMemTypeDei = VPS_VPDMA_MT_NONTILEDMEM;
        pDrvChPrm->outMemTypeVip = VPS_VPDMA_MT_NONTILEDMEM;
        pDrvChPrm->drnEnable = FALSE;
        pDrvChPrm->comprEnable = FALSE;

        pDrvChPrm->deiHqCfg = &pDrvObj->deiHqCfg;
        pDrvChPrm->deiCfg = &pDrvObj->deiCfg;
        pDrvChPrm->scCfg = &pDrvObj->scCfg;
        pDrvChPrm->deiCropCfg = &pDrvObj->scCropCfg[winId];

        pDrvChPrm->deiHqCfg->bypass = bypassDrv;
        pDrvChPrm->deiHqCfg->inpMode = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
        pDrvChPrm->deiHqCfg->tempInpEnable = TRUE;
        pDrvChPrm->deiHqCfg->tempInpChromaEnable = TRUE;
        pDrvChPrm->deiHqCfg->spatMaxBypass = FALSE;
        pDrvChPrm->deiHqCfg->tempMaxBypass = FALSE;
        pDrvChPrm->deiHqCfg->fldMode = VPS_DEIHQ_FLDMODE_5FLD;
        pDrvChPrm->deiHqCfg->lcModeEnable = TRUE;
        pDrvChPrm->deiHqCfg->mvstmEnable = TRUE;
        pDrvChPrm->deiHqCfg->tnrEnable = TRUE;
        pDrvChPrm->deiHqCfg->snrEnable = TRUE;
        pDrvChPrm->deiHqCfg->sktEnable = FALSE;
        pDrvChPrm->deiHqCfg->chromaEdiEnable = TRUE;

        pDrvChPrm->deiCfg->bypass = bypassDrv;
        pDrvChPrm->deiCfg->inpMode = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
        pDrvChPrm->deiCfg->tempInpEnable = TRUE;
        pDrvChPrm->deiCfg->tempInpChromaEnable = TRUE;
        pDrvChPrm->deiCfg->spatMaxBypass = FALSE;
        pDrvChPrm->deiCfg->tempMaxBypass = FALSE;

        pDrvChPrm->scCfg->bypass = FALSE;
        pDrvChPrm->scCfg->nonLinear = FALSE;
        pDrvChPrm->scCfg->stripSize = 0;
        pDrvChPrm->scCfg->vsType = VPS_SC_VST_POLYPHASE;

        pDrvChPrm->deiCropCfg->cropStartX = 0;
        pDrvChPrm->deiCropCfg->cropStartY = 0;
        pDrvChPrm->deiCropCfg->cropWidth = pDrvChPrm->inFmt.width;
        if (bypassDrv)
        {
            pDrvChPrm->deiCropCfg->cropHeight = pDrvChPrm->inFmt.height;
        }
        else
        {
            pDrvChPrm->deiCropCfg->cropHeight = pDrvChPrm->inFmt.height * 2;
        }
    }

    memset(&cbParams, 0, sizeof(cbParams));

    if (bypassDrv)
    {
        cbParams.cbFxn = SwMsLink_drvDeiBypassFvidCb;
    }
    else
    {
        cbParams.cbFxn = SwMsLink_drvDeiFvidCb;
    }
    cbParams.errCbFxn = SwMsLink_drvFvidErrCb;
    cbParams.errList = &pDrvObj->errCbProcessList;
    cbParams.appData = pObj;

    pDrvObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_DEI_DRV,
                                       pDrvObj->drvInstId,
                                       &pDrvObj->deiCreateParams,
                                       &pDrvObj->deiCreateStatus, &cbParams);
    GT_assert(GT_DEFAULT_MASK, pDrvObj->fvidHandle != NULL);

    SwMsLink_drvAllocCtxMem(pDrvObj);

    /* load co-effs only once */
    if (!bypassDrv)
    {
        SwMsLink_drvSetScCoeffs(pDrvObj->fvidHandle, TRUE, TRUE);
    }

    pDrvObj->processList.numInLists = 1;
    pDrvObj->processList.numOutLists = 1;
    pDrvObj->processList.inFrameList[0] = &pDrvObj->inFrameList;
    pDrvObj->processList.outFrameList[0] = &pDrvObj->outFrameList;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvCreateScDrv                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvCreateScDrv(SwMsLink_Obj * pObj)
{
    Semaphore_Params semParams;

    SwMsLink_ScDrvObj *pDrvObj;

    Vps_M2mScChParams *pDrvChPrm;

    UInt32 winId;

    System_LinkChInfo *pChInfo;

    FVID2_CbParams cbParams;

    pDrvObj = &pObj->scDrvObj;
    pDrvObj->drvInstId = VPS_M2M_INST_SEC0_SC5_WB2;

    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pDrvObj->complete = Semaphore_create(0u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pDrvObj->complete != NULL);

    pDrvObj->scCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    pDrvObj->scCreateParams.numChannels = SYSTEM_SW_MS_MAX_WIN;
    pDrvObj->scCreateParams.chParams
        = (Vps_M2mScChParams *) pDrvObj->scChParams;

    for (winId = 0; winId < pDrvObj->scCreateParams.numChannels; winId++)
    {
        pDrvChPrm = &pDrvObj->scChParams[winId];

        /* assume all CHs are of same input size, format, pitch */
        pChInfo = &pObj->inQueInfo.chInfo[0];

        pDrvChPrm->inFmt.channelNum = winId;
        pDrvChPrm->inFmt.width = pChInfo->width;
        pDrvChPrm->inFmt.height = pChInfo->height;
        pDrvChPrm->inFmt.pitch[0] = pChInfo->pitch[0];
        pDrvChPrm->inFmt.pitch[1] = pChInfo->pitch[1];
        pDrvChPrm->inFmt.pitch[2] = pChInfo->pitch[2];
        pDrvChPrm->inFmt.fieldMerged[0] = FALSE;
        pDrvChPrm->inFmt.fieldMerged[1] = FALSE;
        pDrvChPrm->inFmt.fieldMerged[0] = FALSE;
        pDrvChPrm->inFmt.dataFormat = pChInfo->dataFormat;
        pDrvChPrm->inFmt.scanFormat = FVID2_SF_PROGRESSIVE;
        pDrvChPrm->inFmt.bpp = FVID2_BPP_BITS16;

        pDrvChPrm->outFmt.channelNum = winId;
        pDrvChPrm->outFmt.width = pChInfo->width;
        pDrvChPrm->outFmt.height = pChInfo->height;
        pDrvChPrm->outFmt.pitch[0] = pObj->outFrameFormat.pitch[0];
        pDrvChPrm->outFmt.pitch[1] = pObj->outFrameFormat.pitch[1];
        pDrvChPrm->outFmt.pitch[2] = pObj->outFrameFormat.pitch[2];
        pDrvChPrm->outFmt.fieldMerged[0] = FALSE;
        pDrvChPrm->outFmt.fieldMerged[1] = FALSE;
        pDrvChPrm->outFmt.fieldMerged[0] = FALSE;
        pDrvChPrm->outFmt.dataFormat = pObj->outFrameFormat.dataFormat;
        pDrvChPrm->outFmt.scanFormat = FVID2_SF_PROGRESSIVE;
        pDrvChPrm->outFmt.bpp = pObj->outFrameFormat.bpp;

        pDrvChPrm->inMemType = pChInfo->memType;
        pDrvChPrm->outMemType = VPS_VPDMA_MT_NONTILEDMEM;

        pDrvChPrm->scCfg = &pDrvObj->scCfg;
        pDrvChPrm->srcCropCfg = &pDrvObj->scCropCfg[winId];

        pDrvChPrm->scCfg->bypass = FALSE;
        pDrvChPrm->scCfg->nonLinear = FALSE;
        pDrvChPrm->scCfg->stripSize = 0;
        pDrvChPrm->scCfg->vsType = VPS_SC_VST_POLYPHASE;

        pDrvChPrm->srcCropCfg->cropStartX = 0;
        pDrvChPrm->srcCropCfg->cropStartY = 0;
        pDrvChPrm->srcCropCfg->cropWidth = pDrvChPrm->inFmt.width;
        pDrvChPrm->srcCropCfg->cropHeight = pDrvChPrm->inFmt.height;
    }

    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn = SwMsLink_drvScFvidCb;
    cbParams.errCbFxn = SwMsLink_drvFvidErrCb;
    cbParams.errList = &pDrvObj->errCbProcessList;
    cbParams.appData = pObj;

    pDrvObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
                                       pDrvObj->drvInstId,
                                       &pDrvObj->scCreateParams,
                                       &pDrvObj->scCreateStatus, &cbParams);
    GT_assert(GT_DEFAULT_MASK, pDrvObj->fvidHandle != NULL);

    SwMsLink_drvSetScCoeffs(pDrvObj->fvidHandle, FALSE, FALSE); /* only
                                                                 * downscale
                                                                 * for SDTV */

    pDrvObj->processList.numInLists = 1;
    pDrvObj->processList.numOutLists = 1;
    pDrvObj->processList.inFrameList[0] = &pDrvObj->inFrameList;
    pDrvObj->processList.outFrameList[0] = &pDrvObj->outFrameList;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvCreate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvCreate(SwMsLink_Obj * pObj, SwMsLink_CreateParams * pPrm)
{
    Semaphore_Params semParams;

    Clock_Params clockParams;

    UInt32 winId;

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Create in progress !!!\n", Clock_getTicks());
#endif

    pObj->frameCount = 0;
    pObj->totalTime = 0;

    pObj->skipProcessing = 0;
    pObj->switchLayout = FALSE;

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    memset(pObj->winObj, 0, sizeof(pObj->winObj));
    memset(&pObj->deiDrvObj, 0, sizeof(pObj->deiDrvObj));
    memset(&pObj->deiBypassDrvObj, 0, sizeof(pObj->deiBypassDrvObj));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert(GT_DEFAULT_MASK,
              pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    if (pPrm->timerPeriod == 0 || pPrm->timerPeriod > 200)
    {
        pObj->timerPeriod = SW_MS_LINK_TIMER_DEFAULT_PERIOD;
    }
    else
    {
        pObj->timerPeriod = pPrm->timerPeriod;
    }

    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->lock = Semaphore_create(1u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->lock != NULL);

    Clock_Params_init(&clockParams);
    clockParams.period = pObj->timerPeriod;
    clockParams.arg = (UArg) pObj;

    pObj->timer = Clock_create(SwMsLink_drvTimerCb,
                               pObj->timerPeriod, &clockParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->timer != NULL);

    SwMsLink_drvCreateOutInfo(pObj, pPrm->layoutPrm.outRes);

    for (winId = 0; winId < SYSTEM_SW_MS_MAX_WIN; winId++)
    {
        SwMsLink_drvCreateWinObj(pObj, winId);
    }

    if (SYSTEM_LINK_ID_SW_MS_DEI_0 == pObj->linkId ||
        SYSTEM_LINK_ID_SW_MS_DEI_HQ_0 == pObj->linkId)
    {
        pObj->isDei = TRUE;
        SwMsLink_drvCreateDeiDrv(pObj, FALSE);
        SwMsLink_drvCreateDeiDrv(pObj, TRUE);
    }
    else if (SYSTEM_LINK_ID_SW_MS_SC_0 == pObj->linkId)
    {
        pObj->isDei = FALSE;
        SwMsLink_drvCreateScDrv(pObj);
    }
    else
    {
        GT_assert(GT_DEFAULT_MASK, 0);
    }

    pObj->layoutPrm.outLayoutMode = SYSTEM_LAYOUT_MODE_MAX;

    SwMsLink_drvSwitchLayout(pObj, &pPrm->layoutPrm);

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvProcessData                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvProcessData(SwMsLink_Obj * pObj)
{
    UInt32 frameId, winId, chId, freeFrameNum;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    SwMsLink_LayoutWinInfo *pWinInfo;

    SwMsLink_OutWinObj *pWinObj;

    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        SwMsLink_drvLock(pObj);

        freeFrameNum = 0;

        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            if (pFrame == NULL)
            {
                continue;
            }

            /* valid frame */
            chId = pFrame->channelNum;
            if (chId >= SYSTEM_SW_MS_MAX_CH_ID)
            {
                /* invalid ch ID */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }

            /* valid chId */
            winId = pObj->layoutInfo.ch2WinMap[chId];
            if (winId == SYSTEM_SW_MS_INVALID_ID)
            {
                /* ch not mapped to any window */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }

            /* ch mapped to window */
            pWinInfo = &pObj->layoutInfo.winInfo[winId];
            if (pWinInfo->channelNum != chId)
            {
                /* win is not assigned to this ch, normally this condition
                 * wont happen */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }

            pWinObj = &pObj->winObj[winId];

            /* valid win and ch */
            /* [TODO] For the moment, SC drv takes only even fields and make
             * S/W moasic. It throw away odd fields. Later, it can take both
             * fields and make S/W mosaic in merged frame format. */
            if (pWinInfo->bypass || !pObj->isDei)
            {
                /* window shows channel in bypass mode, then drop odd fields, 
                 * i.e always expect even fields */
                pWinObj->expectedFid = 0;
            }

            if (pFrame->fid != pWinObj->expectedFid)
            {
                /* incoming frame fid does not match required fid */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }

            /* queue the frame */
            status = VpsUtils_bufPutFullFrame(&pWinObj->bufInQue, pFrame);
            if (status != FVID2_SOK)
            {
                /* Q full, release frame */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }

            /* toggle next expected fid */
            if (pObj->isDei)
            {
                pWinObj->expectedFid ^= 1;
            }
        }

        SwMsLink_drvUnlock(pObj);

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;

            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &frameList);
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvMakeFrameLists                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvMakeFrameLists(SwMsLink_Obj * pObj, FVID2_Frame * pOutFrame)
{
    UInt32 winId, freeFrameNum;

    SwMsLink_LayoutWinInfo *pWinInfo;

    SwMsLink_OutWinObj *pWinObj;

    SwMsLink_DeiDrvObj *pDeiDrvObj;

    SwMsLink_ScDrvObj *pScDrvObj;

    FVID2_Frame *pInFrame, *pFrame;

    Int32 status;

    Bool repeatFld;

    UInt32 nextFid;

    if (pObj->isDei)
    {
        pObj->deiBypassDrvObj.inFrameList.numFrames = 0;
        pObj->deiBypassDrvObj.outFrameList.numFrames = 0;

        pObj->deiDrvObj.inFrameList.numFrames = 0;
        pObj->deiDrvObj.outFrameList.numFrames = 0;
    }
    else
    {
        pObj->scDrvObj.inFrameList.numFrames = 0;
        pObj->scDrvObj.outFrameList.numFrames = 0;
    }

    freeFrameNum = 0;

    for (winId = 0; winId < SYSTEM_SW_MS_MAX_WIN; winId++)
    {
        pWinObj = &pObj->winObj[winId];
        pWinInfo = &pObj->layoutInfo.winInfo[winId];

        repeatFld = FALSE;

        if (winId >= pObj->layoutInfo.numWin)
        {
            /* empty input queue */
            status = VpsUtils_bufGetFullFrame(&pWinObj->bufInQue,
                                              &pInFrame, BIOS_NO_WAIT);
            if (status == FVID2_SOK)
            {
                pObj->freeFrameList.frames[freeFrameNum] = pInFrame;
                freeFrameNum++;
            }

            /* free any frame which is being held */
            if (pWinObj->pCurInFrame != NULL)
            {
                pObj->freeFrameList.frames[freeFrameNum] = pWinObj->pCurInFrame;
                freeFrameNum++;
                pWinObj->pCurInFrame = NULL;
            }
            continue;
        }

        /* valid window for processing */
        status = VpsUtils_bufGetFullFrame(&pWinObj->bufInQue,
                                          &pInFrame, BIOS_NO_WAIT);

        if (pObj->switchLayout && pInFrame)
        {
            nextFid = pInFrame->fid;

            /* get latest frame */
            while (status == FVID2_SOK)
            {
                /* valid window for processing */
                status = VpsUtils_bufGetFullFrame(&pWinObj->bufInQue,
                                                  &pFrame, BIOS_NO_WAIT);
                if (status == FVID2_SOK)
                {
                    pObj->freeFrameList.frames[freeFrameNum] = pInFrame;
                    freeFrameNum++;
                    pInFrame = pFrame;
                }
            }

            if ((pInFrame->fid != nextFid) && pObj->isDei)
            {
                repeatFld = TRUE;
            }
        }

        if (pInFrame)
        {
            /* got new frame, free any frame which is being held */
            if (pWinObj->pCurInFrame != NULL)
            {
                pObj->freeFrameList.frames[freeFrameNum] = pWinObj->pCurInFrame;
                freeFrameNum++;
            }

            pWinObj->pCurInFrame = pInFrame;
        }
        else
        {
            /* no new frame, repeat previous frame */
            pWinObj->repeatFrameCount++;

            repeatFld = TRUE;
        }

        if (pWinObj->pCurInFrame == NULL)
        {
            /* use blank frame */
            pInFrame = &pWinObj->blankFrame;
        }
        else
        {
            /* use actual frame */
            pInFrame = pWinObj->pCurInFrame;
        }

        if (pObj->isDei)
        {
            if (pWinInfo->bypass)
            {
                pDeiDrvObj = &pObj->deiBypassDrvObj;

                /* [TODO] This is for avoiding bypass DEI driver getting FID 
                 * = 1. Not clear why FID = 1 is passed to bypass DEI
                 * without this workaround. */
                if (FVID2_FID_TOP != pInFrame->fid)
                {
                    pInFrame->fid = FVID2_FID_TOP;
                }
            }
            else
            {
                pDeiDrvObj = &pObj->deiDrvObj;
            }
            pDeiDrvObj->inFrameList.frames[pDeiDrvObj->inFrameList.numFrames]
                = pInFrame;
            pDeiDrvObj->inFrameList.numFrames++;
            pDeiDrvObj->outFrameList.frames[pDeiDrvObj->outFrameList.numFrames]
                = &pWinObj->curOutFrame;
            pDeiDrvObj->outFrameList.numFrames++;
        }
        else
        {
            pScDrvObj = &pObj->scDrvObj;
            pScDrvObj->inFrameList.frames[pScDrvObj->inFrameList.numFrames]
                = pInFrame;
            pScDrvObj->inFrameList.numFrames++;
            pScDrvObj->outFrameList.frames[pScDrvObj->outFrameList.numFrames]
                = &pWinObj->curOutFrame;
            pScDrvObj->outFrameList.numFrames++;
        }

        pInFrame->channelNum = winId;
        pInFrame->perFrameCfg = NULL;
        pWinObj->curOutFrame.perFrameCfg = NULL;

        pWinObj->curOutFrame.addr[0][0] =
            (Ptr) ((UInt32) pOutFrame->addr[0][0] + pWinInfo->bufAddrOffset);

        if (pWinObj->applyRtPrm || repeatFld)
        {
            if (pObj->isDei)
            {
                pInFrame->perFrameCfg = &pWinObj->deiRtPrm;
                pWinObj->curOutFrame.perFrameCfg = &pWinObj->deiRtPrm;
            }
            else
            {
                pInFrame->perFrameCfg = &pWinObj->scRtPrm;
                pWinObj->curOutFrame.perFrameCfg = &pWinObj->scRtPrm;
            }
            pWinObj->applyRtPrm = FALSE;

            if (repeatFld && pObj->isDei)
            {
                pWinObj->deiRtCfg.fldRepeat = TRUE;
            }
        }
    }

    pObj->switchLayout = FALSE;

    pObj->freeFrameList.numFrames = freeFrameNum;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_deiDrvProcessFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_deiDrvProcessFrames(SwMsLink_Obj * pObj,
                                   SwMsLink_DeiDrvObj * pDrvObj)
{
    Int32 status;

    UInt32 curTime;

    if (pDrvObj->inFrameList.numFrames)
    {
#ifdef SYSTEM_DEBUG_SWMS_RT
        Vps_printf(" %d: SWMS: Processing %d frames !!!\n",
                   Clock_getTicks(), pDrvObj->inFrameList.numFrames);
#endif

        curTime = Clock_getTicks();

        status =
            FVID2_processFrames(pDrvObj->fvidHandle, &pDrvObj->processList);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        Semaphore_pend(pDrvObj->complete, BIOS_WAIT_FOREVER);

        status = FVID2_getProcessedFrames(pDrvObj->fvidHandle,
                                          &pDrvObj->processList, BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        curTime = Clock_getTicks() - curTime;
        pObj->totalTime += curTime;

        pObj->frameCount += pDrvObj->inFrameList.numFrames;

#ifdef SYSTEM_DEBUG_SWMS_RT
        Vps_printf(" %d: SWMS: Processing of %d frames done !!!\n",
                   Clock_getTicks(), pDrvObj->inFrameList.numFrames);
#endif
    }

    return status;
}

/* ===================================================================
 *  @func     SwMsLink_scDrvProcessFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_scDrvProcessFrames(SwMsLink_Obj * pObj,
                                  SwMsLink_ScDrvObj * pDrvObj)
{
    Int32 status;

    UInt32 curTime;

    if (pDrvObj->inFrameList.numFrames)
    {
#ifdef SYSTEM_DEBUG_SWMS_RT
        Vps_printf(" %d: SWMS: Processing %d frames !!!\n",
                   Clock_getTicks(), pDrvObj->inFrameList.numFrames);
#endif

        curTime = Clock_getTicks();

        status =
            FVID2_processFrames(pDrvObj->fvidHandle, &pDrvObj->processList);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        Semaphore_pend(pDrvObj->complete, BIOS_WAIT_FOREVER);

        status = FVID2_getProcessedFrames(pDrvObj->fvidHandle,
                                          &pDrvObj->processList, BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        curTime = Clock_getTicks() - curTime;
        pObj->totalTime += curTime;

        pObj->frameCount += pDrvObj->inFrameList.numFrames;

#ifdef SYSTEM_DEBUG_SWMS_RT
        Vps_printf(" %d: SWMS: Processing of %d frames done !!!\n",
                   Clock_getTicks(), pDrvObj->inFrameList.numFrames);
#endif
    }

    return status;
}

/* ===================================================================
 *  @func     SwMsLink_drvDoScaling                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvDoScaling(SwMsLink_Obj * pObj)
{
    FVID2_Frame *pOutFrame;

    Int32 status;

    if (pObj->skipProcessing)
    {
        pObj->skipProcessing--;
        return FVID2_SOK;
    }

    status = VpsUtils_bufGetEmptyFrame(&pObj->bufOutQue,
                                       &pOutFrame, BIOS_NO_WAIT);
    if (status != FVID2_SOK)
    {
        return status;
    }

    SwMsLink_drvLock(pObj);

    SwMsLink_drvMakeFrameLists(pObj, pOutFrame);

    if (pObj->freeFrameList.numFrames)
    {
        System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                                   pObj->createArgs.inQueParams.prevLinkQueId,
                                   &pObj->freeFrameList);
    }

    if (pObj->isDei)
    {
        SwMsLink_deiDrvProcessFrames(pObj, &pObj->deiDrvObj);
        SwMsLink_deiDrvProcessFrames(pObj, &pObj->deiBypassDrvObj);
    }
    else
    {
        SwMsLink_scDrvProcessFrames(pObj, &pObj->scDrvObj);
    }

    SwMsLink_drvUnlock(pObj);

    status = VpsUtils_bufPutFullFrame(&pObj->bufOutQue, pOutFrame);
    if (status != FVID2_SOK)
    {
        // return back frame to empty que
        VpsUtils_bufPutEmptyFrame(&pObj->bufOutQue, pOutFrame);
    }

    System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                       SYSTEM_CMD_NEW_DATA);

    return status;
}

/* ===================================================================
 *  @func     SwMsLink_drvDeleteDrv                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvDeleteDrv(SwMsLink_Obj * pObj, Bool bypassDrv)
{
    SwMsLink_DeiDrvObj *pDeiDrvObj;

    SwMsLink_ScDrvObj *pScDrvObj;

    if (pObj->isDei)
    {
        if (bypassDrv)
        {
            pDeiDrvObj = &pObj->deiBypassDrvObj;
        }
        else
        {
            pDeiDrvObj = &pObj->deiDrvObj;
        }
        SwMsLink_drvFreeCtxMem(pDeiDrvObj);
        FVID2_delete(pDeiDrvObj->fvidHandle, NULL);
        Semaphore_delete(&pDeiDrvObj->complete);
    }
    else
    {
        pScDrvObj = &pObj->scDrvObj;
        FVID2_delete(pScDrvObj->fvidHandle, NULL);
        Semaphore_delete(&pScDrvObj->complete);
    };

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvDelete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvDelete(SwMsLink_Obj * pObj)
{
    UInt32 winId;

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Frames = %d (fps = %d) !!!\n",
               Clock_getTicks(),
               pObj->frameCount,
               pObj->frameCount * 100 / (pObj->totalTime / 10));
#endif

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Delete in progress !!!\n", Clock_getTicks());
#endif

    if (pObj->isDei)
    {
        SwMsLink_drvDeleteDrv(pObj, FALSE);
        SwMsLink_drvDeleteDrv(pObj, TRUE);
    }
    else
    {
        SwMsLink_drvDeleteDrv(pObj, FALSE                  // don't care for
                                                           // SC driver
            );
    }

    VpsUtils_bufDelete(&pObj->bufOutQue);

    for (winId = 0; winId < SYSTEM_SW_MS_MAX_WIN; winId++)
    {
        VpsUtils_bufDelete(&pObj->winObj[winId].bufInQue);
    }

    Semaphore_delete(&pObj->lock);
    Clock_delete(&pObj->timer);

    VpsUtils_memFrameFree(&pObj->bufferFrameFormat,
                          pObj->outFrames, SW_MS_LINK_MAX_OUT_FRAMES);
    VpsUtils_memFrameFree(&pObj->bufferFrameFormat, &pObj->winObj[0].blankFrame,
                          1);

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvStart                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvStart(SwMsLink_Obj * pObj)
{
    Clock_start(pObj->timer);

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Start Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvStop                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvStop(SwMsLink_Obj * pObj)
{
    Clock_stop(pObj->timer);

#ifdef SYSTEM_DEBUG_SWMS
    Vps_printf(" %d: SWMS: Stop Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_drvLock                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvLock(SwMsLink_Obj * pObj)
{
    return Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
}

/* ===================================================================
 *  @func     SwMsLink_drvUnlock                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 SwMsLink_drvUnlock(SwMsLink_Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}
