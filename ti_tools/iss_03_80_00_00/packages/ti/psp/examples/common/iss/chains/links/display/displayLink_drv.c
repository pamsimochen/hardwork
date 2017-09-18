/** ==================================================================
 *  @file   displayLink_drv.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/display/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "displayLink_priv.h"

/* ===================================================================
 *  @func     DisplayLink_drvFvidCb                                               
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
Int32 DisplayLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    DisplayLink_Obj *pObj = (DisplayLink_Obj *) appData;

    UInt32 elaspedTime, curTime;

    VpsUtils_tskSendCmd(&pObj->tsk, DISPLAY_LINK_CMD_DO_DEQUE);

    pObj->cbCount++;

    curTime = Clock_getTicks();

    if (pObj->cbCount > 10)
    {
        elaspedTime = curTime - pObj->lastCbTime;

        if (elaspedTime > pObj->maxCbTime)
            pObj->maxCbTime = elaspedTime;

        if (elaspedTime < pObj->minCbTime)
            pObj->minCbTime = elaspedTime;
    }

    pObj->lastCbTime = curTime;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DisplayLink_drvSwitchCh                                               
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
Int32 DisplayLink_drvSwitchCh(DisplayLink_Obj * pObj, UInt32 channelNum)
{
    DisplayLink_drvLock(pObj);

    pObj->curDisplayChannelNum = channelNum;

    DisplayLink_drvUnlock(pObj);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DisplayLink_drvDisplayAllocAndQueBlankFrame                                               
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
Int32 DisplayLink_drvDisplayAllocAndQueBlankFrame(DisplayLink_Obj * pObj)
{
    FVID2_FrameList frameList;

    UInt32 numQue;

    Int32 status;

    status = VpsUtils_memFrameAlloc(&pObj->displayFormat, &pObj->blankFrame, 1);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    pObj->blankFrame.channelNum = DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM;

    for (numQue = 0; numQue < DISPLAY_LINK_BLANK_FRAME_INIT_QUE; numQue++)
    {
        frameList.numFrames = 1;
        frameList.frames[0] = &pObj->blankFrame;
        frameList.perListCfg = NULL;

        pObj->blankFrame.addr[0][0] =
            (UInt8 *) pObj->blankFrame.addr[0][0] +
            pObj->displayFormat.pitch[0];

        status = FVID2_queue(pObj->displayHndl, &frameList, 0);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     DisplayLink_drvDisplayCreate                                               
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
Int32 DisplayLink_drvDisplayCreate(DisplayLink_Obj * pObj)
{
    FVID2_CbParams cbParams;

    System_LinkChInfo *pInChInfo;

    FVID2_Format *pFormat;

    Int32 status;

    pInChInfo = &pObj->inQueInfo.chInfo[0];

    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn = DisplayLink_drvFvidCb;
    cbParams.appData = pObj;

    pObj->displayCreateArgs.memType = pInChInfo->memType;
    pObj->displayCreateArgs.periodicCallbackEnable = TRUE;

    pFormat = &pObj->displayFormat;

    pFormat->channelNum = 0;
    pFormat->width = pInChInfo->width;
    pFormat->height = pInChInfo->height;
    pFormat->pitch[0] = pInChInfo->pitch[0];
    pFormat->pitch[1] = pInChInfo->pitch[1];
    pFormat->pitch[2] = pInChInfo->pitch[2];
    pFormat->dataFormat = FVID2_DF_YUV422I_UYVY;

    switch (pObj->tskId)
    {
        default:
        case SYSTEM_LINK_ID_DISPLAY_0:
            pObj->displayInstId = VPS_DISP_INST_BP0;
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
            break;
        case SYSTEM_LINK_ID_DISPLAY_1:
            pObj->displayInstId = VPS_DISP_INST_BP1;
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
            break;
        case SYSTEM_LINK_ID_DISPLAY_2:
            pObj->displayInstId = VPS_DISP_INST_SEC1;
            pFormat->scanFormat = FVID2_SF_INTERLACED;
            break;
    }

    pFormat->fieldMerged[0] = FALSE;

    if (pObj->createArgs.displayRes == SYSTEM_DISPLAY_RES_1080I60
        || pObj->createArgs.displayRes == SYSTEM_DISPLAY_RES_NTSC
        || pObj->createArgs.displayRes == SYSTEM_DISPLAY_RES_PAL)
    {
        pFormat->scanFormat = FVID2_SF_INTERLACED;
        pFormat->fieldMerged[0] = TRUE;
    }
    if (pObj->createArgs.displayRes == SYSTEM_DISPLAY_RES_NTSC)
    {
        if (pFormat->width > 720)
            pFormat->width = 720;
        if (pFormat->height > 480)
            pFormat->height = 480;
    }
    if (pObj->createArgs.displayRes == SYSTEM_DISPLAY_RES_PAL)
    {
        if (pFormat->width > 720)
            pFormat->width = 720;
        if (pFormat->height > 576)
            pFormat->height = 576;
    }

    if (pObj->createArgs.displayRes == SYSTEM_DISPLAY_RES_720P60)
    {
        if (pFormat->width > 1280)
            pFormat->width = 1280;
        if (pFormat->height > 720)
            pFormat->height = 720;
    }

    pFormat->fieldMerged[1] = pFormat->fieldMerged[0];
    pFormat->fieldMerged[2] = pFormat->fieldMerged[0];
    pFormat->bpp = FVID2_BPP_BITS16;
    pFormat->reserved = NULL;

#ifdef SYSTEM_VERBOSE_PRINTS
    Vps_printf(" %d: DISPLAY: %d: Window size %dx%d, %dB",
               Clock_getTicks(),
               pObj->displayInstId, pFormat->width, pFormat->height,
               pFormat->pitch[0]);
#endif

    pObj->displayHndl = FVID2_create(FVID2_VPS_DISP_DRV,
                                     pObj->displayInstId,
                                     &pObj->displayCreateArgs,
                                     &pObj->displayCreateStatus, &cbParams);
    GT_assert(GT_DEFAULT_MASK, pObj->displayHndl != NULL);

    status = FVID2_setFormat(pObj->displayHndl, pFormat);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    pObj->queueCount = 0;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DisplayLink_drvCreate                                               
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
Int32 DisplayLink_drvCreate(DisplayLink_Obj * pObj,
                            DisplayLink_CreateParams * pPrm)
{
    Semaphore_Params semParams;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Create in progress !!!\n", Clock_getTicks());
#endif

    pObj->cbCount = 0;
    pObj->dequeCount = 0;
    pObj->totalTime = 0;
    pObj->minCbTime = 0xFFFF;
    pObj->maxCbTime = 0;
    pObj->lastCbTime = 0;

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert(GT_DEFAULT_MASK,
              pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->lock = Semaphore_create(1u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->lock != NULL);

    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->complete = Semaphore_create(0u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->complete != NULL);

    DisplayLink_drvDisplayCreate(pObj);
    DisplayLink_drvDisplayAllocAndQueBlankFrame(pObj);
    DisplayLink_drvSwitchCh(pObj, 0);

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DisplayLink_drvPrintRtStatus                                               
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
Int32 DisplayLink_drvPrintRtStatus(DisplayLink_Obj * pObj, UInt32 elaspedTime)
{
    char *displayName[] = { "HDDAC(BP0) ", "DVO2(BP1)  ", "SDDAC(SEC1)" };

    Vps_rprintf
        (" %d: DISPLAY: %s: %d fps, Callback Interval: Max = %d, Min = %d !!! \r\n",
         Clock_getTicks(), displayName[pObj->displayInstId],
         pObj->cbCount * 1000 / elaspedTime, pObj->maxCbTime, pObj->minCbTime);

#if 0
    /* reset max time */
    pObj->maxCbTime = pObj->minCbTime;
#endif

    return 0;
}

/* ===================================================================
 *  @func     DisplayLink_drvProcessData                                               
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
Int32 DisplayLink_drvProcessData(DisplayLink_Obj * pObj)
{
    FVID2_FrameList frameList;

    FVID2_FrameList freeFrameList;

    FVID2_FrameList displayFrameList;

    UInt32 freeFrameNum, frameId, displayFrameId, elaspedTime;

    System_LinkInQueParams *pInQueParams;

    FVID2_Frame *pFrame;

    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    freeFrameNum = 0;

    elaspedTime = Clock_getTicks() - pObj->startTime;

    if ((elaspedTime - pObj->prevTime) > 30 * 1000)
    {
        DisplayLink_drvPrintRtStatus(pObj, elaspedTime);

        pObj->prevTime = elaspedTime;
    }

    do
    {
        displayFrameList.numFrames = 0;

        /* dequeue all completed frames */
        status = FVID2_dequeue(pObj->displayHndl,
                               &displayFrameList, 0, BIOS_NO_WAIT);

        pObj->dequeCount += displayFrameList.numFrames;

        for (displayFrameId = 0; displayFrameId < displayFrameList.numFrames;
             displayFrameId++)
        {
            pFrame = displayFrameList.frames[displayFrameId];
            if (pFrame->channelNum != DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM)
            {
                freeFrameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        if (displayFrameList.numFrames)
        {
#ifdef SYSTEM_DEBUG_DISPLAY_RT
            Vps_printf(" %d: DISPLAY: Deque %d frames\n", Clock_getTicks(),
                       displayFrameList.numFrames);
#endif
        }

    } while (status == FVID2_SOK);

    /* que frames if any */

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        DisplayLink_drvLock(pObj);

        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            if (pFrame->channelNum == pObj->curDisplayChannelNum)
            {
                /* queue frame for display */
                displayFrameList.numFrames = 0;

                displayFrameList.frames[displayFrameList.numFrames] = pFrame;
                displayFrameList.numFrames++;
                displayFrameList.perListCfg = NULL;

                pFrame->addr[0][0] =
                    (UInt8 *) pFrame->addr[0][0] + pObj->displayFormat.pitch[0];

#ifdef SYSTEM_DEBUG_DISPLAY_RT
                Vps_printf(" %d: DISPLAY: Queue %d frames\n", Clock_getTicks(),
                           displayFrameList.numFrames);
#endif

                status = FVID2_queue(pObj->displayHndl, &displayFrameList, 0);
                GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

                pObj->queueCount++;
            }
            else
            {
                freeFrameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        DisplayLink_drvUnlock(pObj);
    }

    if (freeFrameNum)
    {
        freeFrameList.numFrames = freeFrameNum;

        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &freeFrameList);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DisplayLink_drvDelete                                               
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
Int32 DisplayLink_drvDelete(DisplayLink_Obj * pObj)
{
    FVID2_FrameList frameList;

    Int32 status;

#ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DISPLAY: Frames = %d (fps = %d) !!!\n",
               Clock_getTicks(),
               pObj->dequeCount,
               pObj->dequeCount * 100 / (pObj->totalTime / 10));
#endif

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Delete in progress !!!\n", Clock_getTicks());
#endif

    do
    {
        // dequeue queued buffer's
        status = FVID2_dequeue(pObj->displayHndl, &frameList, 0, BIOS_NO_WAIT);
    } while (status == FVID2_SOK);

    FVID2_delete(pObj->displayHndl, NULL);

    Semaphore_delete(&pObj->lock);
    Semaphore_delete(&pObj->complete);

    status = VpsUtils_memFrameFree(&pObj->displayFormat, &pObj->blankFrame, 1);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DisplayLink_drvStart                                               
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
Int32 DisplayLink_drvStart(DisplayLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Start in progress !!!\n", Clock_getTicks());
#endif

    pObj->lastCbTime = Clock_getTicks();
    pObj->startTime = Clock_getTicks();
    pObj->prevTime = pObj->startTime;

    status = FVID2_start(pObj->displayHndl, NULL);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    pObj->totalTime = Clock_getTicks();

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Start Done !!!\n", Clock_getTicks());
#endif

    return status;
}

/* ===================================================================
 *  @func     DisplayLink_drvStop                                               
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
Int32 DisplayLink_drvStop(DisplayLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Stop in progress !!!\n", Clock_getTicks());
#endif

    pObj->totalTime = Clock_getTicks() - pObj->totalTime;

    status = FVID2_stop(pObj->displayHndl, NULL);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Stop Done !!!\n", Clock_getTicks());
#endif

    return status;
}

/* ===================================================================
 *  @func     DisplayLink_drvLock                                               
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
Int32 DisplayLink_drvLock(DisplayLink_Obj * pObj)
{
    return Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
}

/* ===================================================================
 *  @func     DisplayLink_drvUnlock                                               
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
Int32 DisplayLink_drvUnlock(DisplayLink_Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}
