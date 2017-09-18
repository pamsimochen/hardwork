/** ==================================================================
 *  @file   scLink_drv.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/scalar/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "scLink_priv.h"

/* ===================================================================
 *  @func     ScLink_drvFvidCb                                               
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
Int32 ScLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    ScLink_Obj *pObj = (ScLink_Obj *) appData;

    VpsUtils_tskSendCmd(&pObj->tsk, SC_LINK_CMD_GET_PROCESSED_DATA);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvFvidErrCb                                               
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
Int32 ScLink_drvFvidErrCb(FVID2_Handle handle,
                          Ptr appData, Ptr errList, Ptr reserved)
{
    Vps_printf("Error Callback\n");
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvCreateReqObj                                               
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
Int32 ScLink_drvCreateReqObj(ScLink_Obj * pObj)
{
    Int32 status;

    UInt32 reqId;

    memset(pObj->reqObj, 0, sizeof(pObj->reqObj));

    status = VpsUtils_queCreate(&pObj->reqQue,
                                SC_LINK_MAX_REQ,
                                pObj->reqQueMem,
                                VPSUTILS_QUE_FLAG_NO_BLOCK_QUE);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    pObj->reqQueCount = 0;
    pObj->isReqPend = FALSE;

    for (reqId = 0; reqId < SC_LINK_MAX_REQ; reqId++)
    {
        status =
            VpsUtils_quePut(&pObj->reqQue, &pObj->reqObj[reqId], BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    pObj->reqNumOutLists = 1;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvCreateOutObj                                               
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
Int32 ScLink_drvCreateOutObj(ScLink_Obj * pObj)
{
    ScLink_OutObj *pOutObj;

    Int32 status;

    UInt32 frameId, outId, chId;

    FVID2_Format *pFormat;

    UInt32 maxOutWidth = 0u, maxOutHeight = 0u;

    System_LinkChInfo *pInChInfo;

    System_LinkChInfo *pOutChInfo;

    const Sys_ScLinkCfg *pSysScCfg = NULL;

    if (pObj->createArgs.scCfgId != SYS_SC_CFG_DEFAULT)
    {
        pSysScCfg = Sys_getScLinkCfg(pObj->createArgs.scCfgId);
        GT_assert(GT_DEFAULT_MASK, NULL != pSysScCfg);
        GT_assert(GT_DEFAULT_MASK, pObj->inQueInfo.numCh == pSysScCfg->numCh);
    }

    for (outId = 0u; outId < SC_LINK_MAX_OUT_QUE; outId++)
    {
        if (pObj->createArgs.scCfgId != SYS_SC_CFG_DEFAULT)
        {
            for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
            {
                if (pSysScCfg->scOutWidth[chId] > maxOutWidth)
                {
                    maxOutWidth = pSysScCfg->scOutWidth[chId];
                }
                if (pSysScCfg->scOutHeight[chId] > maxOutHeight)
                {
                    maxOutHeight = pSysScCfg->scOutHeight[chId];
                }
            }
        }
        else
        {
            maxOutWidth = pObj->inQueInfo.chInfo[0].width;
            maxOutHeight = pObj->inQueInfo.chInfo[0].height;
        }

        pOutObj = &pObj->outObj[outId];

        pOutObj->outNumFrames =
            pObj->inQueInfo.numCh * SC_LINK_MAX_OUT_FRAMES_PER_CH;

        GT_assert(GT_DEFAULT_MASK, pObj->inQueInfo.numCh <= SC_LINK_MAX_CH);
        GT_assert(GT_DEFAULT_MASK,
                  pOutObj->outNumFrames <= SC_LINK_MAX_OUT_FRAMES);

        status = VpsUtils_bufCreate(&pOutObj->bufOutQue, TRUE, FALSE);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        pFormat = &pOutObj->outFormat;

        pFormat->channelNum = 0;
        pFormat->width = maxOutWidth;
        pFormat->height = maxOutHeight;
        pFormat->fieldMerged[0] = FALSE;
        pFormat->fieldMerged[1] = FALSE;
        pFormat->fieldMerged[2] = FALSE;

        pFormat->pitch[0] =
            VpsUtils_align(pFormat->width, VPS_BUFFER_ALIGNMENT);
        pFormat->dataFormat = FVID2_DF_YUV422I_YUYV;
        pFormat->pitch[0] *= 2;

        pFormat->pitch[1] = pFormat->pitch[0];
        pFormat->pitch[2] = pFormat->pitch[0];

        pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
        pFormat->bpp = FVID2_BPP_BITS16;
        pFormat->reserved = NULL;

        status = VpsUtils_memFrameAlloc(&pOutObj->outFormat,
                                        pOutObj->outFrames,
                                        pOutObj->outNumFrames);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        for (frameId = 0; frameId < pOutObj->outNumFrames; frameId++)
        {
            status =
                VpsUtils_bufPutEmptyFrame(&pOutObj->bufOutQue,
                                          &pOutObj->outFrames[frameId]);
            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
        }
    }

    pObj->info.numQue = SC_LINK_MAX_OUT_QUE;
    for (outId = 0u; outId < SC_LINK_MAX_OUT_QUE; outId++)
    {
        pObj->info.queInfo[outId].numCh = pObj->createArgs.numOutChs;
    }

    for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];

        for (outId = 0u; outId < SC_LINK_MAX_OUT_QUE; outId++)
        {
            pOutChInfo = &pObj->info.queInfo[outId].chInfo[chId];

            pFormat = &pObj->outObj[outId].outFormat;

            pOutChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
            pOutChInfo->dataFormat = (FVID2_DataFormat) pFormat->dataFormat;
            if (pObj->createArgs.scCfgId != SYS_SC_CFG_DEFAULT)
            {
                pOutChInfo->width = pSysScCfg->scOutWidth[chId];
                pOutChInfo->height = pSysScCfg->scOutHeight[chId];
            }
            else
            {
                pOutChInfo->width = pInChInfo->width;
                pOutChInfo->height = pInChInfo->height;
            }
            pOutChInfo->pitch[0] = pFormat->pitch[0];
            pOutChInfo->pitch[1] = pFormat->pitch[1];
            pOutChInfo->pitch[2] = pFormat->pitch[2];
            pOutChInfo->scanFormat = (FVID2_ScanFormat) pFormat->scanFormat;
        }
    }

    return (status);
}

/* ===================================================================
 *  @func     ScLink_drvCreateChObj                                               
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
Int32 ScLink_drvCreateChObj(ScLink_Obj * pObj, UInt32 chId)
{
    ScLink_ChObj *pChObj;

    System_LinkChInfo *pInChInfo;

    System_LinkChInfo *pOutChInfo;

    Vps_M2mScChParams *pDrvChParams;

    FVID2_Format *pFormat;

    Int32 status;

    UInt32 outId;

    pChObj = &pObj->chObj[chId];

    status = VpsUtils_bufCreate(&pChObj->inQue, FALSE, FALSE);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    pInChInfo = &pObj->inQueInfo.chInfo[chId];

    pDrvChParams = &pObj->drvChArgs[chId];

    pFormat = &pDrvChParams->inFmt;

    pFormat->channelNum = chId;
    pFormat->width = pInChInfo->width;
    pFormat->height = pInChInfo->height;
    pFormat->fieldMerged[0] = FALSE;
    if (pInChInfo->scanFormat == FVID2_SF_INTERLACED)
        pFormat->fieldMerged[0] = TRUE;
    pFormat->fieldMerged[1] = pFormat->fieldMerged[0];
    pFormat->fieldMerged[2] = pFormat->fieldMerged[0];
    pFormat->pitch[0] = pInChInfo->pitch[0];
    pFormat->pitch[1] = pInChInfo->pitch[1];
    pFormat->pitch[2] = pInChInfo->pitch[2];
    pFormat->dataFormat = pInChInfo->dataFormat;
    pFormat->scanFormat = pInChInfo->scanFormat;
    pFormat->bpp = FVID2_BPP_BITS16;
    pFormat->reserved = NULL;

    pDrvChParams->inMemType = pInChInfo->memType;
    pDrvChParams->outMemType = VPS_VPDMA_MT_NONTILEDMEM;

    for (outId = 0u; outId < SC_LINK_MAX_OUT_QUE; outId++)
    {

        pOutChInfo = &pObj->info.queInfo[outId].chInfo[chId];

        pFormat = &pChObj->outFormat[outId];

        pFormat->channelNum = chId;
        pFormat->width = pOutChInfo->width;
        pFormat->height = pOutChInfo->height;
        pFormat->fieldMerged[0] = FALSE;
        pFormat->fieldMerged[1] = FALSE;
        pFormat->fieldMerged[2] = FALSE;
        pFormat->pitch[0] = pOutChInfo->pitch[0];
        pFormat->pitch[1] = pOutChInfo->pitch[1];
        pFormat->pitch[2] = pOutChInfo->pitch[2];
        pFormat->dataFormat = pOutChInfo->dataFormat;
        pFormat->scanFormat = pOutChInfo->scanFormat;
        pFormat->bpp = FVID2_BPP_BITS16;
        pFormat->reserved = NULL;
        pDrvChParams->outMemType = pOutChInfo->memType;

        pChObj->scCfg[outId].bypass = FALSE;
        pChObj->scCfg[outId].nonLinear = FALSE;
        pChObj->scCfg[outId].stripSize = 0;
        pChObj->scCfg[outId].vsType = VPS_SC_VST_POLYPHASE;

        pChObj->scCropConfig[outId].cropStartX = 0;
        pChObj->scCropConfig[outId].cropStartY = 0;
        pChObj->scCropConfig[outId].cropWidth = pInChInfo->width;
        pChObj->scCropConfig[outId].cropHeight = pInChInfo->height;
    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvSetScCoeffs                                               
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
Int32 ScLink_drvSetScCoeffs(ScLink_Obj * pObj)
{
    Int32 retVal = FVID2_SOK;

    Vps_ScCoeffParams coeffPrms;

    coeffPrms.hScalingSet = VPS_SC_DS_SET_0;
    coeffPrms.vScalingSet = VPS_SC_DS_SET_0;
    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;

    /* Program scalar coefficient - Always used */
    retVal = FVID2_control(pObj->fvidHandle,
                           IOCTL_VPS_SET_COEFFS, &coeffPrms, NULL);
    GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);

    return (retVal);
}

/* ===================================================================
 *  @func     ScLink_drvCreateFvidObj                                               
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
Int32 ScLink_drvCreateFvidObj(ScLink_Obj * pObj)
{
    Vps_M2mScChParams *pChParams;

    ScLink_ChObj *pChObj;

    UInt32 chId;

    FVID2_CbParams cbParams;

    pObj->drvCreateArgs.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    pObj->drvCreateArgs.numChannels = pObj->inQueInfo.numCh;
    pObj->drvCreateArgs.chParams = (Vps_M2mScChParams *) pObj->drvChArgs;

    for (chId = 0u; chId < pObj->drvCreateArgs.numChannels; chId++)
    {
        pChParams = &pObj->drvChArgs[chId];
        pChObj = &pObj->chObj[chId];

        pChParams->scCfg = NULL;
        pChParams->srcCropCfg = NULL;
        pChParams->subFrameParams = NULL;
        pObj->drvInstId = VPS_M2M_INST_SEC0_SC5_WB2;

        pChParams->scCfg = &pChObj->scCfg[0];
        pChParams->srcCropCfg = &pChObj->scCropConfig[0];
        memcpy(&pChParams->outFmt, &pChObj->outFormat[0], sizeof(FVID2_Format));
    }

    memset(&cbParams, 0, sizeof(cbParams));

    cbParams.cbFxn = ScLink_drvFvidCb;
    cbParams.errCbFxn = ScLink_drvFvidErrCb;
    cbParams.errList = &pObj->errProcessList;
    cbParams.appData = pObj;

    pObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
                                    pObj->drvInstId,
                                    &pObj->drvCreateArgs,
                                    &pObj->drvCreateStatus, &cbParams);
    GT_assert(GT_DEFAULT_MASK, pObj->fvidHandle != NULL);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvCreate                                               
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
Int32 ScLink_drvCreate(ScLink_Obj * pObj, ScLink_CreateParams * pPrm)
{
    UInt32 chId;

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: Scalar : Create in progress !!!\n", Clock_getTicks());
#endif

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert(GT_DEFAULT_MASK,
              pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    pObj->inFrameGetCount = 0;
    pObj->inFrameSkipCount = 0;
    pObj->inFramePutCount = 0;
    pObj->outFrameGetCount[0] = 0;
    pObj->outFrameGetCount[1] = 0;
    pObj->outFramePutCount[0] = 0;
    pObj->outFramePutCount[1] = 0;
    pObj->processFrameReqPendCount = 0;
    pObj->processFrameReqPendSubmitCount = 0;
    pObj->processFrameCount = 0;
    pObj->getProcessFrameCount = 0;
    pObj->processFrameReqCount = 0;
    pObj->getProcessFrameReqCount = 0;

    ScLink_drvCreateOutObj(pObj);

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
        ScLink_drvCreateChObj(pObj, chId);

    ScLink_drvCreateFvidObj(pObj);
    ScLink_drvSetScCoeffs(pObj);
    ScLink_drvCreateReqObj(pObj);

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: Scalar    : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvQueueFramesToChQue                                               
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
Int32 ScLink_drvQueueFramesToChQue(ScLink_Obj * pObj)
{
    UInt32 frameId, freeFrameNum;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    ScLink_ChObj *pChObj;

    UInt32 temp;

    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
#ifdef SYSTEM_DEBUG_SCALAR_RT
        Vps_printf(" %d: Scalar    : Received %d IN frames !!!\n",
                   Clock_getTicks(), frameList.numFrames);
#endif

        pObj->inFrameGetCount++;

        freeFrameNum = 0;

        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];
#ifdef SYSTEM_DEBUG_SCALAR_RT
            Vps_printf
                (" Scalar: Received frame 0x%p from previous link for channel %d\n",
                 pFrame, pFrame->channelNum);
#endif
            pChObj = &pObj->chObj[pFrame->channelNum];

            if ((pFrame->channelNum >= pObj->createArgs.outChStart) &&
                (pFrame->channelNum < (pObj->createArgs.outChStart +
                                       pObj->createArgs.numOutChs)))
            {
                /* Put the required frames in queue */
                status = VpsUtils_bufPutFullFrame(&pChObj->inQue, pFrame);
                if (status != FVID2_SOK)
                {
                    pObj->inFrameSkipCount++;
                    /* Drop the not required frames */
                    frameList.frames[freeFrameNum] = pFrame;
                    freeFrameNum++;
                }
            }
            else
            {
                pObj->inFrameSkipCount++;

                /* Drop the not required frames */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;

#ifdef SYSTEM_DEBUG_SCALAR_RT
            Vps_printf(" %d: Scalar    : Skipped %d IN frames !!!\n",
                       Clock_getTicks(), frameList.numFrames);
#endif

            pObj->inFramePutCount++;

            for (temp = 0; temp < frameList.numFrames; temp++)
            {
#ifdef SYSTEM_DEBUG_SCALAR_RT
                Vps_printf
                    (" Scalar: Giving back frame 0x%p to previous link for channel %d\n",
                     frameList.frames[temp],
                     frameList.frames[temp]->channelNum);
#endif
            }
            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &frameList);
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvMakeFrameLists                                               
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
Int32 ScLink_drvMakeFrameLists(ScLink_Obj * pObj, FVID2_FrameList * inFrameList,
                               FVID2_FrameList
                               outFrameList[SC_LINK_MAX_OUT_QUE])
{
    ScLink_ChObj *pChObj;

    UInt32 chId, outId, frameId;

    FVID2_Frame *pInFrame, *pOutFrame;

    Int32 status;

    frameId = 0;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        VpsUtils_bufGetFullFrame(&pChObj->inQue, &pInFrame, BIOS_NO_WAIT);

        if (pInFrame)
        {
            inFrameList->frames[frameId] = pInFrame;

            for (outId = 0; outId < SC_LINK_MAX_OUT_QUE; outId++)
            {
                pOutFrame = NULL;

                status =
                    VpsUtils_bufGetEmptyFrame(&pObj->outObj[outId].bufOutQue,
                                              &pOutFrame, BIOS_WAIT_FOREVER);
                GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

                pOutFrame->channelNum = pInFrame->channelNum;

                pObj->outFrameGetCount[outId]++;

                outFrameList[outId].frames[frameId] = pOutFrame;
            }
            frameId++;
        }
    }

    inFrameList->numFrames = frameId;
    for (outId = 0; outId < SC_LINK_MAX_OUT_QUE; outId++)
        outFrameList[outId].numFrames = frameId;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvReleaseFrames                                               
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
Int32 ScLink_drvReleaseFrames(ScLink_Obj * pObj, FVID2_FrameList * inFrameList,
                              FVID2_FrameList outFrameList[SC_LINK_MAX_OUT_QUE])
{
    UInt32 frameId, outId, sendCmd[SC_LINK_MAX_OUT_QUE], temp;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    for (outId = 0; outId < SC_LINK_MAX_OUT_QUE; outId++)
        sendCmd[outId] = FALSE;

    for (frameId = 0; frameId < inFrameList->numFrames; frameId++)
    {
        pFrame = inFrameList->frames[frameId];

        for (outId = 0; outId < SC_LINK_MAX_OUT_QUE; outId++)
        {
            pFrame = outFrameList[outId].frames[frameId];

            if (pFrame)
            {
                if (pFrame->channelNum >= pObj->createArgs.outChStart)
                {
                    pFrame->channelNum -= pObj->createArgs.outChStart;
                }
                status =
                    VpsUtils_bufPutFullFrame(&pObj->outObj[outId].bufOutQue,
                                             pFrame);
                GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

                sendCmd[outId] = TRUE;

                pObj->outFramePutCount[outId]++;
            }

        }
    }

#ifdef SYSTEM_DEBUG_SCALAR_RT
    Vps_printf(" %d: Scalar    : Released %d IN frames !!!\n", Clock_getTicks(),
               inFrameList->numFrames);
#endif

    pObj->inFramePutCount++;
    for (temp = 0; temp < inFrameList->numFrames; temp++)
    {
#ifdef SYSTEM_DEBUG_SCALAR_RT
        Vps_printf
            (" Scalar: Giving back frame 0x%p to previous link for channel %d\n",
             inFrameList->frames[temp], inFrameList->frames[temp]->channelNum);
#endif
    }
    System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                               pInQueParams->prevLinkQueId, inFrameList);

    for (outId = 0; outId < SC_LINK_MAX_OUT_QUE; outId++)
    {
        if (sendCmd[outId])
        {
            System_sendLinkCmd(pObj->createArgs.outQueParams[outId].nextLink,
                               SYSTEM_CMD_NEW_DATA);
        }
    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvSubmitData                                               
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
Int32 ScLink_drvSubmitData(ScLink_Obj * pObj)
{
    ScLink_ReqObj *pReqObj;

    Int32 status;

    status = VpsUtils_queGet(&pObj->reqQue, (Ptr *) & pReqObj, 1, BIOS_NO_WAIT);
    if (status != FVID2_SOK)
    {
#ifdef SYSTEM_DEBUG_SCALAR_RT
        Vps_printf(" %d: Scalar    : Pending request !!!\n", Clock_getTicks());
#endif

        pObj->processFrameReqPendCount++;

        pObj->isReqPend = TRUE;
        return FVID2_SOK;
    }

    pObj->isReqPend = FALSE;

    pReqObj->processList.inFrameList[0] = &pReqObj->inFrameList;
    pReqObj->processList.outFrameList[0] = &pReqObj->outFrameList[0];
    pReqObj->processList.outFrameList[1] = &pReqObj->outFrameList[1];
    pReqObj->processList.numInLists = 1;
    pReqObj->processList.numOutLists = pObj->reqNumOutLists;

    pReqObj->inFrameList.appData = pReqObj;

    ScLink_drvMakeFrameLists(pObj, &pReqObj->inFrameList,
                             pReqObj->outFrameList);

#ifdef SYSTEM_DEBUG_SCALAR_RT
    Vps_printf(" %d: Scalar    : Submitting %d frames !!!\n", Clock_getTicks(),
               pReqObj->inFrameList.numFrames);
#endif

    if (pReqObj->inFrameList.numFrames)
    {
        pObj->reqQueCount++;

        pObj->processFrameCount += pReqObj->inFrameList.numFrames;
        pObj->processFrameReqCount++;

        status = FVID2_processFrames(pObj->fvidHandle, &pReqObj->processList);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }
    else
    {

        status = VpsUtils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     ScLink_drvProcessData                                               
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
Int32 ScLink_drvProcessData(ScLink_Obj * pObj)
{
    ScLink_drvQueueFramesToChQue(pObj);
    ScLink_drvSubmitData(pObj);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvGetProcessedData                                               
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
Int32 ScLink_drvGetProcessedData(ScLink_Obj * pObj)
{
    ScLink_ReqObj *pReqObj;

    Int32 status;

    FVID2_ProcessList processList;

    status =
        FVID2_getProcessedFrames(pObj->fvidHandle, &processList, BIOS_NO_WAIT);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_SCALAR_RT
    Vps_printf(" %d: Scalar    : Completed %d frames !!!\n", Clock_getTicks(),
               processList.inFrameList[0]->numFrames);
#endif

    pObj->getProcessFrameCount += processList.inFrameList[0]->numFrames;
    pObj->getProcessFrameReqCount++;

    pReqObj = (ScLink_ReqObj *) processList.inFrameList[0]->appData;

    ScLink_drvReleaseFrames(pObj, &pReqObj->inFrameList, pReqObj->outFrameList);

    status = VpsUtils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    pObj->reqQueCount--;

    if (pObj->isReqPend)
    {
#ifdef SYSTEM_DEBUG_SCALAR_RT
        Vps_printf(" %d: Scalar   : Submitting pending request !!!\n",
                   Clock_getTicks());
#endif

        pObj->processFrameReqPendSubmitCount++;

        ScLink_drvSubmitData(pObj);
    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvStop                                               
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
Int32 ScLink_drvStop(ScLink_Obj * pObj)
{
#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: Scalar    : Stop in progress, %d requests pending !!!\n",
               Clock_getTicks(), pObj->reqQueCount);
#endif

    while (pObj->reqQueCount)
    {
        VpsUtils_tskWaitCmd(&pObj->tsk, NULL, SC_LINK_CMD_GET_PROCESSED_DATA);
        ScLink_drvGetProcessedData(pObj);
    }

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: Scalar    : Stop Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScLink_drvDelete                                               
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
Int32 ScLink_drvDelete(ScLink_Obj * pObj)
{
    UInt32 outId, chId;

    ScLink_ChObj *pChObj;

    ScLink_OutObj *pOutObj;

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: Scalar    : Delete in progress !!!\n", Clock_getTicks());
#endif

    FVID2_delete(pObj->fvidHandle, NULL);

    for (outId = 0; outId < SC_LINK_MAX_OUT_QUE; outId++)
    {

        pOutObj = &pObj->outObj[outId];

        VpsUtils_bufDelete(&pOutObj->bufOutQue);

        VpsUtils_memFrameFree(&pOutObj->outFormat, pOutObj->outFrames,
                              pOutObj->outNumFrames);

    }

    for (chId = 0; chId < SC_LINK_MAX_CH; chId++)
    {
        pChObj = &pObj->chObj[chId];

        VpsUtils_bufDelete(&pChObj->inQue);
    }

    VpsUtils_queDelete(&pObj->reqQue);

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: Scalar    : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
