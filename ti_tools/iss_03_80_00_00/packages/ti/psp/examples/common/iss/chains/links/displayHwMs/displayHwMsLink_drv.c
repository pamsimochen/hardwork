/** ==================================================================
 *  @file   displayHwMsLink_drv.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/displayHwMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file displayHwMsLink_drv.c
 *
 *  \brief Display mosaic link implementation file.
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include "displayHwMsLink_priv.h"

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Function Declarations */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     DispHwMsLink_drvDqFrames                                               
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
static Int32 DispHwMsLink_drvDqFrames(DispHwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     DispHwMsLink_drvQFrames                                               
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
static Int32 DispHwMsLink_drvQFrames(DispHwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     DispHwMsLink_createLayouts                                               
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
static Int32 DispHwMsLink_createLayouts(DispHwMsLink_Obj * pObj);

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

/* 
 *  Display driver callback function.
 *  Periodic callback is enabled and this will be called by the driver every
 *  VSYNC interval.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvFvidCb                                               
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
static Int32 DispHwMsLink_drvFvidCb(FVID2_Handle handle,
                                    Ptr appData, Ptr reserved)
{
    DispHwMsLink_Obj *pObj = (DispHwMsLink_Obj *) appData;

    UInt32 elaspedTime, curTime;

    /* Send command to dequeue frames from driver */
    VpsUtils_tskSendCmd(&pObj->taskHndl, DISP_HWMS_LINK_CMD_DO_DEQUE);

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

    return (FVID2_SOK);
}

/* 
 *  Allocates blank frame and assign it to each sub window and prime them to
 *  driver.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvAllocAndQueBlankFrm                                               
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
static Int32 DispHwMsLink_drvAllocAndQueBlankFrm(DispHwMsLink_Obj * pObj)
{
    Int32 retVal;

    UInt32 primeCnt, winCnt;

    FVID2_FrameList frmList;

    System_FrameInfo *pFrameInfo;

    /* Allocate one frame as blank frame */
    retVal = VpsUtils_memFrameAlloc(&pObj->dispFmt, &pObj->blankFrm, 1u);
    GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);
    pFrameInfo = &pObj->frameInfo;
    pFrameInfo->displayHwMsRepeatCount = 1u;
    pObj->blankFrm.appData = (Void *) pFrameInfo;
    /* Use a unused channel number to identify as blank frame while dequeuing 
     */
    pObj->blankFrm.channelNum = DISP_HWMS_LINK_BLK_FRM_CH_NUM;

    /* Assign odd field address - used in interlaced mode */
    pObj->blankFrm.addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
        ((UInt8 *) pObj->blankFrm.addr[FVID2_FIELD_EVEN_ADDR_IDX]
         [FVID2_YUV_INT_ADDR_IDX]) + pObj->dispFmt.pitch[0u];

    /* Assign blank frame for all the sub-windows and queue it to driver */
    frmList.perListCfg = NULL;
    for (primeCnt = 0u; primeCnt < DISP_HWMS_LINK_NUM_PRIMING; primeCnt++)
    {
        frmList.numFrames = pObj->totalSubWin;
        for (winCnt = 0u; winCnt < pObj->totalSubWin; winCnt++)
        {
            frmList.frames[winCnt] = &pObj->blankFrm;
            pObj->pLastQueuedFrm[winCnt] = &pObj->blankFrm;
        }

        retVal = FVID2_queue(pObj->dispHandle, &frmList, 0u);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);
        pObj->queueCount++;
    }

    return (retVal);
}

/* 
 *  Creates the display driver and configure with the required mosaic mode.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvDisplayCreate                                               
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
static Int32 DispHwMsLink_drvDisplayCreate(DispHwMsLink_Obj * pObj)
{
    Int32 retVal = FVID2_SOK;

    FVID2_Format *pFmt;

    FVID2_CbParams cbParams;

    System_LinkChInfo *pInChInfo;

    Vps_DispCreateParams dispCreatePrms;

    /* Setting the format with the 0th channel should be good enough. Rest
     * everything is taken care while creating layouts */
    pInChInfo = &pObj->inQueInfo[0u].chInfo[0u];

    pFmt = &pObj->dispFmt;
    pFmt->channelNum = 0u;
    pFmt->width = pInChInfo->width;
    pFmt->height = pInChInfo->height;
    pFmt->pitch[0u] = pInChInfo->pitch[0u];
    pFmt->pitch[1u] = pInChInfo->pitch[1u];
    pFmt->pitch[2u] = pInChInfo->pitch[2u];
    pFmt->dataFormat = pInChInfo->dataFormat;

    pFmt->scanFormat = FVID2_SF_PROGRESSIVE;
    if (SYSTEM_DISPLAY_RES_1080I60 == pObj->createPrms.displayRes)
    {
        pFmt->scanFormat = FVID2_SF_INTERLACED;
    }

    switch (pObj->taskId)
    {
        case SYSTEM_LINK_ID_DISPLAY_HW_MS_0:
        {
            pObj->dispInstId = VPS_DISP_INST_BP0;
            break;
        }

        case SYSTEM_LINK_ID_DISPLAY_HW_MS_1:
        {
            pObj->dispInstId = VPS_DISP_INST_BP1;
            break;
        }

        default:
        {
            retVal = FVID2_EUNSUPPORTED_CMD;
            break;
        }
    }

    if (retVal != FVID2_SOK)
    {
        GT_assert(GT_DEFAULT_MASK, 0);

    }

    if (FVID2_SF_INTERLACED == pFmt->scanFormat)
    {
        pFmt->fieldMerged[0u] = TRUE;
    }
    else
    {
        pFmt->fieldMerged[0u] = FALSE;
    }
    pFmt->fieldMerged[1u] = pFmt->fieldMerged[0u];
    pFmt->fieldMerged[2u] = pFmt->fieldMerged[0u];
    if (FVID2_DF_YUV420SP_UV == pInChInfo->dataFormat)
    {
        pFmt->bpp = FVID2_BPP_BITS12;
    }
    else
    {
        pFmt->bpp = FVID2_BPP_BITS16;
    }
    pFmt->reserved = NULL;

    /* Create the driver */
    pObj->queueCount = 0;
    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn = DispHwMsLink_drvFvidCb;
    cbParams.appData = pObj;
    dispCreatePrms.memType = pInChInfo->memType;
    dispCreatePrms.periodicCallbackEnable = TRUE;
    pObj->dispHandle = FVID2_create(FVID2_VPS_DISP_DRV,
                                    pObj->dispInstId,
                                    &dispCreatePrms, NULL, &cbParams);
    GT_assert(GT_DEFAULT_MASK, pObj->dispHandle != NULL);

    /* Set the format */
    retVal = FVID2_setFormat(pObj->dispHandle, pFmt);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    /* Create mosaic layout */
    retVal = DispHwMsLink_createLayouts(pObj);

    return (retVal);
}

/* 
 *  Mosaic layout is done with the help of system configuration table.
 */
/* ===================================================================
 *  @func     DispHwMsLink_createLayouts                                               
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
static Int32 DispHwMsLink_createLayouts(DispHwMsLink_Obj * pObj)
{
    Int32 retVal = FVID2_SOK;

    UInt32 index, qIndex, winCnt;

    Vps_WinFormat *winFmt;

    Vps_MultiWinParams winPrms;

    const Sys_DispLinkCfg *pSysDispCfg;

    /* Get the display mosaic configuration from sysytem table based on
     * display configuration ID */
    pSysDispCfg = Sys_getDispLinkCfg(pObj->createPrms.dispCfgId);
    GT_assert(GT_DEFAULT_MASK, NULL != pSysDispCfg);
    GT_assert(GT_DEFAULT_MASK, pObj->totalSubWin == pSysDispCfg->numCh);

    /* Set the window position and size for all the windows. Start from the
     * first input Queue, then second and so on... */
    index = 0u;
    for (qIndex = 0u; qIndex < pObj->createPrms.numInQue; qIndex++)
    {
        for (winCnt = 0u; winCnt < pObj->inQueInfo[qIndex].numCh; winCnt++)
        {
            winFmt = &pObj->winFmt[index];
            /* Get the widnow position from system config */
            winFmt->winStartX = pSysDispCfg->winStartX[index];
            winFmt->winStartY = pSysDispCfg->winStartY[index];
            /* Get the window dimension from previous link channel info. It
             * is assumed that this and position are done properly by system
             * configuration table. */
            winFmt->winWidth = pObj->inQueInfo[qIndex].chInfo[winCnt].width;
            winFmt->winHeight = pObj->inQueInfo[qIndex].chInfo[winCnt].height;
            winFmt->dataFormat =
                pObj->inQueInfo[qIndex].chInfo[winCnt].dataFormat;
            winFmt->priority = 0u;
            memcpy(&(winFmt->pitch[0u]),
                   &(pObj->inQueInfo[qIndex].chInfo[winCnt].pitch[0u]),
                   sizeof(winFmt->pitch));
            winFmt->bpp = FVID2_BPP_BITS16;
            index++;

#if 0
            if (qIndex == 0 || (qIndex == 1 && winCnt < 4))
            {
                winFmt->winHeight -= 100;
            }
#endif

#ifdef SYSTEM_VERBOSE_PRINTS
            Vps_rprintf(" %d: DISPLAY: WIN%d: (%d , %d) (%d x %d) %dB\n",
                        Clock_getTicks(), index, winFmt->winStartX,
                        winFmt->winStartY, winFmt->winWidth, winFmt->winHeight,
                        winFmt->pitch[0u]);
#endif
        }
    }

    /* Create the layout */
    winPrms.channelNum = 0u;
    winPrms.numWindows = pObj->totalSubWin;
    winPrms.winFmt = &pObj->winFmt[0u];
    retVal = FVID2_control(pObj->dispHandle,
                           IOCTL_VPS_CREATE_LAYOUT,
                           &winPrms, &(pObj->layoutId));
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    /* Select the created layout */
    retVal = FVID2_control(pObj->dispHandle,
                           IOCTL_VPS_SELECT_LAYOUT, &(pObj->layoutId), NULL);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    return (retVal);
}

/* 
 *  Link create main function.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvCreate                                               
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
Int32 DispHwMsLink_drvCreate(DispHwMsLink_Obj * pObj,
                             DispHwMsLink_CreateParams * pPrm)
{
    Int32 retVal;

    UInt32 winCnt, qIndex;

    System_LinkInfo *pInTskInfo;

    Semaphore_Params semPrms;

    pObj->cbCount = 0;
    pObj->minCbTime = 0xFFFF;
    pObj->maxCbTime = 0;
    pObj->lastCbTime = 0;

    /* Copy create args */
    memcpy(&pObj->createPrms, pPrm, sizeof(pObj->createPrms));
    GT_assert(GT_DEFAULT_MASK,
              pObj->createPrms.numInQue <= DISP_HWMS_LINK_MAX_IN_QUE);

    /* Copy the previous queue information for all the required queues */
    pObj->totalSubWin = 0u;
    for (qIndex = 0u; qIndex < pObj->createPrms.numInQue; qIndex++)
    {
        pInTskInfo = System_getLinkInfo(pPrm->inQueParams[qIndex].prevLinkId);
        GT_assert(GT_DEFAULT_MASK,
                  pPrm->inQueParams[qIndex].prevLinkQueId < pInTskInfo->numQue);
        memcpy(&pObj->inQueInfo[qIndex],
               &pInTskInfo->queInfo[pPrm->inQueParams[qIndex].prevLinkQueId],
               sizeof(pObj->inQueInfo[qIndex]));

        pObj->totalSubWin += pObj->inQueInfo[qIndex].numCh;
    }
    GT_assert(GT_DEFAULT_MASK, pObj->totalSubWin <= DISP_HWMS_LINK_MAX_SUB_WIN);

    /* Create the required semahores */
    Semaphore_Params_init(&semPrms);
    semPrms.mode = Semaphore_Mode_BINARY;
    pObj->lock = Semaphore_create(1u, &semPrms, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->lock != NULL);

    DispHwMsLink_drvDisplayCreate(pObj);
    DispHwMsLink_drvAllocAndQueBlankFrm(pObj);

    /* Initialize sub-window Qs and stats */
    for (winCnt = 0u; winCnt < DISP_HWMS_LINK_MAX_SUB_WIN; winCnt++)
    {
        retVal = VpsUtils_queCreate(&(pObj->qHandles[winCnt]),
                                    DISP_HWMS_LINK_MAX_QOBJ_PER_WIN,
                                    &(pObj->subWinQuePool[winCnt][0u]),
                                    VPSUTILS_QUE_FLAG_NO_BLOCK_QUE);
        GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);
    }

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Create Done!!!\n",
               Clock_getTicks(), pObj->taskId);
#endif

    return (FVID2_SOK);
}

/* ===================================================================
 *  @func     DispHwMsLink_drvPrintRtStatus                                               
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
Int32 DispHwMsLink_drvPrintRtStatus(DispHwMsLink_Obj * pObj, UInt32 elaspedTime)
{
    char *displayName[] = { "HDDAC(BP0) ", "DVO2(BP1)  ", "SDDAC(SEC1)" };

    Vps_rprintf
        (" %d: DISPLAY: %s: %d fps, Callback Interval: Max = %d, Min = %d !!! \r\n",
         Clock_getTicks(), displayName[pObj->dispInstId],
         pObj->cbCount * 1000 / elaspedTime, pObj->maxCbTime, pObj->minCbTime);

#if 0
    /* reset max time */
    pObj->maxCbTime = pObj->minCbTime;
#endif

    return 0;
}

/* ===================================================================
 *  @func     DispHwMsLink_drvProcessData                                               
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
Int32 DispHwMsLink_drvProcessData(DispHwMsLink_Obj * pObj)
{
    Int32 retVal = FVID2_SOK;

    UInt32 elaspedTime;

    UInt32 dqCount;

    elaspedTime = Clock_getTicks() - pObj->startTime;

    if ((elaspedTime - pObj->prevTime) > 5 * 1000)
    {
        DispHwMsLink_drvPrintRtStatus(pObj, elaspedTime);

        pObj->prevTime = elaspedTime;
    }

    dqCount = 0;
    do
    {
        retVal = DispHwMsLink_drvDqFrames(pObj);
        if (retVal == FVID2_SOK)
            dqCount++;
        if (retVal != FVID2_SOK)
        {
            /* Dequeue could return error if no buffers are to be dequeued.
             * Hence should not escalate this error!! */
            retVal = FVID2_SOK;
            break;
        }
    } while (1u);

    while (dqCount > 0u)
    {
        /* only if a frame is dequed should we queue something back */

        retVal = DispHwMsLink_drvRcvFrames(pObj);
        if (retVal != FVID2_SOK)
        {
#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
            Vps_printf(" %d: DISPLAY: Error receiving frames !!!\n",
                       Clock_getTicks());
#endif
        }

        retVal = DispHwMsLink_drvQFrames(pObj);
        if (retVal != FVID2_SOK)
        {
#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
            Vps_printf(" %d: DISPLAY: Error queuing frames !!!\n",
                       Clock_getTicks());
#endif
        }
        dqCount--;
    }

    return (retVal);
}

/* 
 *  Dequeues frames from the driver and give back the frames to the previous
 *  link. Blank frames are not given back to the previous links.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvDqFrames                                               
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
static Int32 DispHwMsLink_drvDqFrames(DispHwMsLink_Obj * pObj)
{
    Int32 retVal;

    UInt32 dispFrmCnt, qIndex, tempCnt, effQIndex;

    FVID2_Frame *pFrm;

    UInt32 outFrmCnt[DISP_HWMS_LINK_MAX_IN_QUE];

    FVID2_FrameList outFrmList[DISP_HWMS_LINK_MAX_IN_QUE];

    FVID2_FrameList dispFrmList;

    System_FrameInfo *pFrameInfo;

    System_LinkInQueParams *pInQuePrms;

    for (qIndex = 0u; qIndex < pObj->createPrms.numInQue; qIndex++)
    {
        outFrmList[qIndex].perListCfg = NULL;
        outFrmCnt[qIndex] = 0u;
    }
    dispFrmList.perListCfg = NULL;
    dispFrmList.numFrames = 0u;

    /* Dequeue completed frames from driver */
    retVal = FVID2_dequeue(pObj->dispHandle, &dispFrmList, 0u, BIOS_NO_WAIT);
    if (retVal != FVID2_SOK)
    {
        return (retVal);
    }
    pObj->dequeueCount++;

    for (dispFrmCnt = 0u; dispFrmCnt < dispFrmList.numFrames; dispFrmCnt++)
    {
        pFrm = dispFrmList.frames[dispFrmCnt];

        if (DISP_HWMS_LINK_BLK_FRM_CH_NUM != pFrm->channelNum)
        {
            GT_assert(GT_DEFAULT_MASK, pFrm->appData != NULL);

            pFrameInfo = (System_FrameInfo *) pFrm->appData;
            GT_assert(GT_DEFAULT_MASK,
                      pFrameInfo->displayHwMsRepeatCount != 0u);
            pFrameInfo->displayHwMsRepeatCount--;
            if (0u == pFrameInfo->displayHwMsRepeatCount)
            {
                /* Frame is not repeated, so give the frame to previous link. 
                 * Before that change back the channel number to the original 
                 * one as needed by each input links. */
                tempCnt = 0u;
                effQIndex = 0u;
                for (qIndex = 0u; qIndex < pObj->createPrms.numInQue; qIndex++)
                {
                    /* Figure out the input queue to which this frame belongs 
                     */
                    if (pFrm->channelNum <
                        (tempCnt + pObj->inQueInfo[qIndex].numCh))
                    {
                        effQIndex = qIndex;
                        pFrm->channelNum -= tempCnt;
                        break;
                    }
                    tempCnt += pObj->inQueInfo[qIndex].numCh;
                }

                outFrmList[effQIndex].frames[outFrmCnt[effQIndex]] = pFrm;
                outFrmCnt[effQIndex]++;
            }
        }
    }

    /* Give back the separated frames to individual input links */
    for (qIndex = 0u; qIndex < pObj->createPrms.numInQue; qIndex++)
    {
        if (outFrmCnt[qIndex] != 0u)
        {
            UInt32 temp;

            outFrmList[qIndex].numFrames = outFrmCnt[qIndex];

            /* Check if we are giving back blank frame or NULL frame. This *
             * could be removed later on. */
            temp = outFrmCnt[qIndex];
            while (temp)
            {
                pFrm = outFrmList[qIndex].frames[(temp - 1)];
                if ((outFrmList[qIndex].frames[(temp - 1)] == NULL) ||
                    (outFrmList[qIndex].frames[(temp - 1)] == &pObj->blankFrm))
                {
                    GT_assert(GT_DEFAULT_MASK, FALSE);
                }
                temp--;
            }

            /* Give back the frames to previous link */
            pInQuePrms = &pObj->createPrms.inQueParams[qIndex];
            System_putLinksEmptyFrames(pInQuePrms->prevLinkId,
                                       pInQuePrms->prevLinkQueId,
                                       &outFrmList[qIndex]);
        }
    }

    return (FVID2_SOK);
}

/* 
 *  Gets new frames from previous links and puts them in proper queues.
 *  When more than one input queue is used, the channel number of the received
 *  frame is changed to match the display window number and put in appropriate
 *  window queue.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvRcvFrames                                               
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
Int32 DispHwMsLink_drvRcvFrames(DispHwMsLink_Obj * pObj)
{
    Int32 retVal = FVID2_SOK;

    UInt32 qIndex, tempCnt;

    UInt32 frmCnt;

    FVID2_Frame *pFrm;

    FVID2_FrameList inFrmList;

    System_LinkInQueParams *pInQuePrms;

    System_FrameInfo *pFrameInfo;

    UInt32 outFrmCnt[DISP_HWMS_LINK_MAX_IN_QUE];

    FVID2_FrameList outFrmList[DISP_HWMS_LINK_MAX_IN_QUE];

    GT_assert(GT_DEFAULT_MASK, pObj != NULL);

    tempCnt = 0u;
    inFrmList.perListCfg = NULL;
    for (qIndex = 0u; qIndex < pObj->createPrms.numInQue; qIndex++)
    {
        outFrmList[qIndex].perListCfg = NULL;
        outFrmCnt[qIndex] = 0u;

        /* Get frames from previous link */
        pInQuePrms = &pObj->createPrms.inQueParams[qIndex];
        System_getLinksFullFrames(pInQuePrms->prevLinkId,
                                  pInQuePrms->prevLinkQueId, &inFrmList);

        if (inFrmList.numFrames > 0u)
        {
            DispHwMsLink_drvLock(pObj);
            for (frmCnt = 0u; frmCnt < inFrmList.numFrames; frmCnt++)
            {
                pFrm = inFrmList.frames[frmCnt];

                /* Assign channel number based on display needs and queue it
                 * to the respective queue */
                GT_assert(GT_DEFAULT_MASK,
                          pFrm->channelNum < pObj->inQueInfo[qIndex].numCh);

                /* Assign odd field address - used in interlaced mode */
                pFrm->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                    ((UInt8 *) pFrm->addr[FVID2_FIELD_EVEN_ADDR_IDX]
                     [FVID2_YUV_INT_ADDR_IDX])
                    +
                    pObj->inQueInfo[qIndex].chInfo[pFrm->channelNum].pitch[0u];

                pFrm->channelNum += tempCnt;
                pFrameInfo = (System_FrameInfo *) pFrm->appData;
                GT_assert(GT_DEFAULT_MASK, pFrameInfo != NULL);
                pFrameInfo->displayHwMsRepeatCount = 0;

                retVal = VpsUtils_quePut(&(pObj->qHandles[pFrm->channelNum]),
                                         (Void *) pFrm, BIOS_WAIT_FOREVER);
                if (retVal != FVID2_SOK)
                {
                    /* 
                     * Window queue is full, give back the frame to previous
                     * link. This could happen when a particular channel comes
                     * in at a faster FPS (momentarily) than display FPS
                     */

                    /* Revert the changes done */
                    pFrm->channelNum -= tempCnt;

                    /* Put the frame in the particular out queue framelist */
                    outFrmList[qIndex].frames[outFrmCnt[qIndex]] = pFrm;
                    outFrmCnt[qIndex]++;

                    /* Reset the error value as we have handled this */
                    retVal = FVID2_SOK;
                }
            }
            DispHwMsLink_drvUnlock(pObj);
        }

        /* Give back the separated frames to individual input links */
        if (outFrmCnt[qIndex] != 0u)
        {
            outFrmList[qIndex].numFrames = outFrmCnt[qIndex];

            /* Give back the frames to previous link */
            System_putLinksEmptyFrames(pInQuePrms->prevLinkId,
                                       pInQuePrms->prevLinkQueId,
                                       &outFrmList[qIndex]);
        }

        tempCnt += pObj->inQueInfo[qIndex].numCh;
    }

    return (retVal);
}

/* 
 *
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvQFrames                                               
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
static Int32 DispHwMsLink_drvQFrames(DispHwMsLink_Obj * pObj)
{
    Int32 retVal = FVID2_SOK;

    UInt32 frmCnt;

    FVID2_Frame *pFrm;

    FVID2_FrameList dispFrmList;

    System_FrameInfo *pFrameInfo;

    dispFrmList.perListCfg = NULL;
    dispFrmList.numFrames = 0u;
    DispHwMsLink_drvLock(pObj);
    for (frmCnt = 0u; frmCnt < pObj->totalSubWin; frmCnt++)
    {
        /* Get frames for each sub-window from input queue */
        retVal = VpsUtils_queGet(&(pObj->qHandles[frmCnt]),
                                 (Void **) (&pFrm), 0u, BIOS_NO_WAIT);
        if (retVal != FVID2_SOK)
        {
            retVal = FVID2_SOK;
            pFrm = NULL;
        }

        /* Check if we have a new frame for each window */
        if (NULL == pFrm)
        {
            /* If we do not have new frame, repeat the last queued frame */
            pFrm = pObj->pLastQueuedFrm[frmCnt];
            GT_assert(GT_DEFAULT_MASK, pFrm != NULL);

            pObj->repeatCount[frmCnt]++;
        }

        pFrameInfo = (System_FrameInfo *) pFrm->appData;
        GT_assert(GT_DEFAULT_MASK, pFrameInfo != NULL);
        pFrameInfo->displayHwMsRepeatCount++;

        /* Should be the last operation */
        pObj->pLastQueuedFrm[frmCnt] = pFrm;
        dispFrmList.frames[frmCnt] = pFrm;
        dispFrmList.numFrames++;
    }
    pObj->queueCount++;
    DispHwMsLink_drvUnlock(pObj);

    /* Queue the frames to driver */
    retVal = FVID2_queue(pObj->dispHandle, &dispFrmList, 0u);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    return (FVID2_SOK);
}

/* 
 *  Delete link function.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvDelete                                               
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
Int32 DispHwMsLink_drvDelete(DispHwMsLink_Obj * pObj)
{
    Int32 retVal;

    UInt32 winCnt;

    FVID2_FrameList frmList;

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Delete in progress ...\n",
               Clock_getTicks(), pObj->taskId);
#endif

    /* Dequeue all the frames from driver */
    frmList.perListCfg = NULL;
    do
    {
        retVal = FVID2_dequeue(pObj->dispHandle, &frmList, 0u, BIOS_NO_WAIT);
        if (retVal == FVID2_SOK)
        {
            pObj->dequeueCount++;
        }
    } while (retVal == FVID2_SOK);

    FVID2_delete(pObj->dispHandle, NULL);
    Semaphore_delete(&pObj->lock);

    retVal = VpsUtils_memFrameFree(&pObj->dispFmt, &pObj->blankFrm, 1u);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    for (winCnt = 0u; winCnt < DISP_HWMS_LINK_MAX_SUB_WIN; winCnt++)
    {
        retVal = VpsUtils_queDelete(&(pObj->qHandles[winCnt]));
        GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);
    }

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Delete Done !!!\n",
               Clock_getTicks(), pObj->taskId);
#endif

    return (FVID2_SOK);
}

/* 
 *  Display link start function.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvStart                                               
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
Int32 DispHwMsLink_drvStart(DispHwMsLink_Obj * pObj)
{
    Int32 retVal;

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Starting...\n",
               Clock_getTicks(), pObj->taskId);
#endif

    pObj->lastCbTime = Clock_getTicks();
    pObj->startTime = Clock_getTicks();
    pObj->prevTime = pObj->startTime;

    retVal = FVID2_start(pObj->dispHandle, NULL);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Start done!!!\n",
               Clock_getTicks(), pObj->taskId);
#endif

    return (retVal);
}

/* 
 *  Display link stop function.
 */
/* ===================================================================
 *  @func     DispHwMsLink_drvStop                                               
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
Int32 DispHwMsLink_drvStop(DispHwMsLink_Obj * pObj)
{
    Int32 retVal;

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Stopping...\n",
               Clock_getTicks(), pObj->taskId);
#endif

    retVal = FVID2_stop(pObj->dispHandle, NULL);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

#ifdef SYSTEM_DEBUG_DISPLAY_HWMS
    Vps_printf(" %d: DISPLAY (%d): Stopped!!!\n",
               Clock_getTicks(), pObj->taskId);
#endif

    return (retVal);
}

/* ===================================================================
 *  @func     DispHwMsLink_drvLock                                               
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
Int32 DispHwMsLink_drvLock(DispHwMsLink_Obj * pObj)
{
    return Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
}

/* ===================================================================
 *  @func     DispHwMsLink_drvUnlock                                               
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
Int32 DispHwMsLink_drvUnlock(DispHwMsLink_Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return (FVID2_SOK);
}
