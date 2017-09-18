/** ==================================================================
 *  @file   scalarSwMsLink_drv.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/scalarSwMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "scalarSwMsLink_priv.h"

// #define SCALAR_SW_MS_LINK_FIELD_MERGED

/* ===================================================================
 *  @func     ScalarSwMsLink_drvTimerCb                                               
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
Void ScalarSwMsLink_drvTimerCb(UArg arg)
{
    ScalarSwMsLink_Obj *pObj = (ScalarSwMsLink_Obj *) arg;

    VpsUtils_tskSendCmd(&pObj->tsk, SCALAR_SW_MS_LINK_CMD_DO_SCALING);
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvFvidCb                                               
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
Int32 ScalarSwMsLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    ScalarSwMsLink_Obj *pObj = (ScalarSwMsLink_Obj *) appData;

    Semaphore_post(pObj->complete);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvFvidErrCb                                               
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
Int32 ScalarSwMsLink_drvFvidErrCb(FVID2_Handle handle,
                                  Ptr appData, Ptr errList, Ptr reserved)
{
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvSwitchLayout                                               
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
Int32 ScalarSwMsLink_drvSwitchLayout(ScalarSwMsLink_Obj * pObj, UInt32 layoutId,
                                     UInt32 startChannelNum)
{
    UInt32 winId;

    ScalarSwMsLink_OutWinObj *pWinObj;

    System_LinkChInfo *pInInfo, *pOutInfo;

    UInt32 inWidth, inHeight, outWidth, outHeight, idx;

    ScalarSwMsLink_drvLock(pObj);

    pObj->layoutId = layoutId;
    pObj->startWinChannelNum = startChannelNum;

    pOutInfo = &pObj->info.queInfo[0].chInfo[0];

    /* assume all CHs have same properties as CH0 */
    pInInfo = &pObj->inQueInfo.chInfo[0];

    outWidth = pOutInfo->width;
    outHeight = pOutInfo->height;

    inWidth = pInInfo->width;
    inHeight = pInInfo->height;

    if (pInInfo->scanFormat == FVID2_SF_INTERLACED)
    {
        inHeight *= 2;
    }

    switch (pObj->layoutId)
    {
        default:
        case SYSTEM_LAYOUT_MODE_1CH:
            pObj->numWin = 1;

            pWinObj = &pObj->winObj[0];

            pWinObj->outWidth = MIN(inWidth, outWidth);
            pWinObj->outHeight = MIN(inHeight, outHeight);
            pWinObj->outStartX = (outWidth - pWinObj->outWidth) / 2;
            pWinObj->outStartY = (outHeight - pWinObj->outHeight) / 2;

            break;

        case SYSTEM_LAYOUT_MODE_4CH:
            pObj->numWin = 4;

            idx = 0;

            pWinObj = &pObj->winObj[idx];

            pWinObj->outWidth = MIN(inWidth * 2, outWidth) / 2;
            pWinObj->outHeight = MIN(inHeight * 2, outHeight) / 2;

            pWinObj->outStartX = (outWidth - pWinObj->outWidth * 2) / 2;
            pWinObj->outStartY = (outHeight - pWinObj->outHeight * 2) / 2;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 0;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 0;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            break;

        case SYSTEM_LAYOUT_MODE_8CH:
            pObj->numWin = 8;

            idx = 0;

            pWinObj = &pObj->winObj[idx];

            pWinObj->outWidth = (MIN((inWidth * 5) / 2, outWidth) * 2) / 5;
            pWinObj->outHeight = MIN(inHeight * 2, outHeight) / 2;

            pWinObj->outStartX = 0;
            pWinObj->outStartY = (outHeight - pWinObj->outHeight * 2) / 2;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 0;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 0;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = outWidth - pWinObj->outWidth * 2;
            pObj->winObj[idx].outHeight = pWinObj->outHeight / 2;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + (pWinObj->outHeight / 2) * 0;

            idx++;
            pObj->winObj[idx].outWidth = outWidth - pWinObj->outWidth * 2;
            pObj->winObj[idx].outHeight = pWinObj->outHeight / 2;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + (pWinObj->outHeight / 2) * 1;

            idx++;
            pObj->winObj[idx].outWidth = outWidth - pWinObj->outWidth * 2;
            pObj->winObj[idx].outHeight = pWinObj->outHeight / 2;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + (pWinObj->outHeight / 2) * 2;

            idx++;
            pObj->winObj[idx].outWidth = outWidth - pWinObj->outWidth * 2;
            pObj->winObj[idx].outHeight = pWinObj->outHeight / 2;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + (pWinObj->outHeight / 2) * 3;
            break;

        case SYSTEM_LAYOUT_MODE_16CH:
            pObj->numWin = 16;

            idx = 0;

            pWinObj = &pObj->winObj[idx];

            pWinObj->outWidth = outWidth / 4;
            pWinObj->outHeight = outHeight / 4;

            pWinObj->outStartX = 0;
            pWinObj->outStartY = 0;

            pWinObj->outStartX =
                VpsUtils_floor(pWinObj->outStartX, VPS_BUFFER_ALIGNMENT);
            pWinObj->outWidth =
                VpsUtils_floor(pWinObj->outWidth, VPS_BUFFER_ALIGNMENT);

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 0;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 0;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 3;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 0;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 0;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 3;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 1;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 0;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 2;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 2;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 2;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 3;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 2;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 0;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 3;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 1;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 3;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 2;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 3;

            idx++;
            pObj->winObj[idx].outWidth = pWinObj->outWidth;
            pObj->winObj[idx].outHeight = pWinObj->outHeight;
            pObj->winObj[idx].outStartX =
                pWinObj->outStartX + pWinObj->outWidth * 3;
            pObj->winObj[idx].outStartY =
                pWinObj->outStartY + pWinObj->outHeight * 3;
    }

    for (winId = 0; winId < pObj->numWin; winId++)
    {
        pWinObj = &pObj->winObj[winId];

        pWinObj->outBufAddrOffset[0] =
            pWinObj->outStartY * pOutInfo->pitch[0] + pWinObj->outStartX * 2;

#ifdef SCALAR_SW_MS_LINK_FIELD_MERGED
        pWinObj->outBufAddrOffset[1] =
            pWinObj->outBufAddrOffset[0] + pOutInfo->pitch[0];
#else
        pWinObj->outBufAddrOffset[1] =
            pWinObj->outBufAddrOffset[0]
            + pOutInfo->pitch[0] * pWinObj->outHeight / 2;
#endif

    }

    ScalarSwMsLink_drvUnlock(pObj);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvSetScCoeffs                                               
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
Int32 ScalarSwMsLink_drvSetScCoeffs(ScalarSwMsLink_Obj * pObj)
{
    Int32 status;

    pObj->scCoeff.hScalingSet = VPS_SC_US_SET;
    pObj->scCoeff.vScalingSet = VPS_SC_US_SET;
    pObj->scCoeff.coeffPtr = NULL;
    pObj->scCoeff.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;

    status = FVID2_control(pObj->fvidHandle,
                           IOCTL_VPS_SET_COEFFS, &pObj->scCoeff, NULL);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    return status;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvCreateOutInfo                                               
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
Int32 ScalarSwMsLink_drvCreateOutInfo(ScalarSwMsLink_Obj * pObj, UInt32 outRes)
{
    Int32 status;

    System_LinkChInfo *pChInfo;

    UInt32 frameId;

    pObj->blankThrehold = SCALAR_SW_MS_LINK_BLANK_THRESHOLD;

    pObj->info.numQue = 1;
    pObj->info.queInfo[0].numCh = 1;

    pChInfo = &pObj->info.queInfo[0].chInfo[0];

    pChInfo->dataFormat = FVID2_DF_YUV422I_YUYV;
    pChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;

    switch (outRes)
    {
        case SYSTEM_DISPLAY_RES_720P60:
            pChInfo->width = 1280;
            pChInfo->height = 720;
            break;
        default:
        case SYSTEM_DISPLAY_RES_1080I60:
        case SYSTEM_DISPLAY_RES_1080P60:
        case SYSTEM_DISPLAY_RES_1080P30:
            pChInfo->width = 1920;
            pChInfo->height = 1080;
            break;
        case SYSTEM_DISPLAY_RES_NTSC:
            pChInfo->width = 720;
            pChInfo->height = 480;
            break;
        case SYSTEM_DISPLAY_RES_PAL:
            pChInfo->width = 720;
            pChInfo->height = 576;
            break;

    }

    pChInfo->pitch[0]
        = VpsUtils_align(pChInfo->width, VPS_BUFFER_ALIGNMENT) * 2;

    pChInfo->pitch[1] = pChInfo->pitch[2] = 0;

    pChInfo->scanFormat = FVID2_SF_PROGRESSIVE;

    pObj->outFrameFormat.channelNum = 0;
    pObj->outFrameFormat.width = pChInfo->width;
    pObj->outFrameFormat.height = pChInfo->height;
    pObj->outFrameFormat.pitch[0] = pChInfo->pitch[0];
    pObj->outFrameFormat.pitch[1] = pChInfo->pitch[1];
    pObj->outFrameFormat.pitch[2] = pChInfo->pitch[2];
    pObj->outFrameFormat.fieldMerged[0] = FALSE;
    pObj->outFrameFormat.fieldMerged[1] = FALSE;
    pObj->outFrameFormat.fieldMerged[2] = FALSE;
    pObj->outFrameFormat.dataFormat = pChInfo->dataFormat;
    pObj->outFrameFormat.scanFormat = pChInfo->scanFormat;
    pObj->outFrameFormat.bpp = FVID2_BPP_BITS16;
    pObj->outFrameFormat.reserved = NULL;

    status = VpsUtils_bufCreate(&pObj->bufOutQue, TRUE, FALSE);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    status =
        VpsUtils_memFrameAlloc(&pObj->outFrameFormat, pObj->outFrames,
                               SCALAR_SW_MS_LINK_MAX_OUT_FRAMES);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    for (frameId = 0; frameId < SCALAR_SW_MS_LINK_MAX_OUT_FRAMES; frameId++)
    {
        status =
            VpsUtils_bufPutEmptyFrame(&pObj->bufOutQue,
                                      &pObj->outFrames[frameId]);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        pObj->outFrames[frameId].appData = &pObj->frameInfo[frameId];

#ifdef SYSTEM_VERBOSE_PRINTS
        Vps_rprintf(" %d: SW_SWMS: %2d: 0x%08x, %d x %d, %08x B\n",
                    Clock_getTicks(),
                    frameId, pObj->outFrames[frameId].addr[0][0],
                    pObj->outFrameFormat.pitch[0] / 2,
                    pObj->outFrameFormat.height,
                    pObj->outFrameFormat.height *
                    pObj->outFrameFormat.pitch[0]);

#endif

    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvCreateWinObj                                               
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
Int32 ScalarSwMsLink_drvCreateWinObj(ScalarSwMsLink_Obj * pObj, UInt32 winId)
{
    ScalarSwMsLink_OutWinObj *pWinObj;

    System_LinkChInfo *pChInfo;

    UInt32 fid;

    Int32 status;

    Vps_M2mScChParams *pScParams;

    FVID2_Format *pFormat;

    UInt32 channelNum;

    pWinObj = &pObj->winObj[winId];

    pWinObj->channelNum = pObj->startWinChannelNum + winId;

    /* assume all CHs have same properties as CH0 */
    pChInfo = &pObj->inQueInfo.chInfo[0];

    if (pChInfo->scanFormat == FVID2_SF_INTERLACED)
        pWinObj->isInterlaced = TRUE;
    else
        pWinObj->isInterlaced = FALSE;

    for (fid = 0; fid < FVID2_MAX_FIELDS; fid++)
    {
        if (winId == 0 && fid == 0)
        {
            status =
                VpsUtils_memFrameAlloc(&pObj->outFrameFormat,
                                       &pWinObj->blankFrame[fid], 1);

            pWinObj->blankFrame[fid].addr[0][1] =
                pWinObj->blankFrame[fid].addr[0][0];

            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
        }
        else
        {
            memcpy(&pWinObj->blankFrame[fid], &pObj->winObj[0].blankFrame[0],
                   sizeof(pWinObj->blankFrame[fid]));
        }

        pWinObj->pCurInFrame[fid] = NULL;

        pWinObj->missFrameCount[fid] = 0;

        status = VpsUtils_bufCreate(&pWinObj->bufInQue[fid], FALSE, FALSE);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        channelNum = ScalarSwMsLink_makeChannelNum(winId, fid);

        pWinObj->curOutFrame[fid].channelNum = channelNum;

        pScParams = &pObj->winScParams[channelNum];

        pFormat = &pScParams->inFmt;
        pFormat->channelNum = channelNum;

        if (pObj->layoutId == SYSTEM_LAYOUT_MODE_4CH &&
            winId < pObj->inQueInfo.numCh)
        {
            pFormat->width = pObj->inQueInfo.chInfo[winId].width;
            pFormat->height = pObj->inQueInfo.chInfo[winId].height;
        }
        else
        {
            pFormat->width = pChInfo->width;
            pFormat->height = pChInfo->height;
        }
        pFormat->pitch[0] = pChInfo->pitch[0];
        pFormat->pitch[1] = pChInfo->pitch[1];
        pFormat->pitch[2] = pChInfo->pitch[2];
        pFormat->fieldMerged[0] = FALSE;
        pFormat->fieldMerged[1] = FALSE;
        pFormat->fieldMerged[2] = FALSE;
        pFormat->dataFormat = pChInfo->dataFormat;
        pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
        pFormat->bpp = FVID2_BPP_BITS16;
        pFormat->reserved = NULL;

        pFormat = &pScParams->outFmt;
        pFormat->channelNum = channelNum;
        pFormat->width = pWinObj->outWidth;
        pFormat->height = pWinObj->outHeight;
        pFormat->pitch[0] = pObj->outFrameFormat.pitch[0];
        pFormat->pitch[1] = pObj->outFrameFormat.pitch[1];
        pFormat->pitch[2] = pObj->outFrameFormat.pitch[2];

        if (pWinObj->isInterlaced)
        {
            pFormat->height /= 2;
#ifdef SCALAR_SW_MS_LINK_FIELD_MERGED
            pFormat->pitch[0] *= 2;
            pFormat->pitch[1] *= 2;
            pFormat->pitch[2] *= 2;
#endif
        }

        pFormat->fieldMerged[0] = FALSE;
        pFormat->fieldMerged[1] = FALSE;
        pFormat->fieldMerged[2] = FALSE;
        pFormat->dataFormat = pObj->outFrameFormat.dataFormat;
        pFormat->scanFormat = pObj->outFrameFormat.scanFormat;
        pFormat->bpp = pObj->outFrameFormat.bpp;
        pFormat->reserved = NULL;

        pScParams->scCfg = NULL;
        pScParams->srcCropCfg = NULL;
        pScParams->inMemType = pChInfo->memType;
        pScParams->outMemType = pObj->info.queInfo[0].chInfo[0].memType;
        pScParams->subFrameParams = NULL;
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvCreate                                               
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
Int32 ScalarSwMsLink_drvCreate(ScalarSwMsLink_Obj * pObj,
                               ScalarSwMsLink_CreateParams * pPrm)
{
    Semaphore_Params semParams;

    Clock_Params clockParams;

    UInt32 winId;

    FVID2_CbParams cbParams;

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: SC_SWMS: Create in progress !!!\n", Clock_getTicks());
#endif

    pObj->frameCount = 0;
    pObj->processFrameCount = 0;
    pObj->getProcessFrameCount = 0;
    pObj->inFrameGetCount = 0;
    pObj->inFramePutCount = 0;
    pObj->totalTime = 0;
    pObj->curTime = 0;

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    memset(pObj->winObj, 0, sizeof(pObj->winObj));
    memset(pObj->winScParams, 0, sizeof(pObj->winScParams));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert(GT_DEFAULT_MASK,
              pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    if (pPrm->timerPeriod == 0 || pPrm->timerPeriod > 100)
        pObj->timerPeriod = SCALAR_SW_MS_LINK_TIMER_DEFAULT_PERIOD;
    else
        pObj->timerPeriod = pPrm->timerPeriod;

    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->lock = Semaphore_create(1u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->lock != NULL);

    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->complete = Semaphore_create(0u, &semParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->complete != NULL);

    Clock_Params_init(&clockParams);
    clockParams.period = pObj->timerPeriod;
    clockParams.arg = (UArg) pObj;

    pObj->timer = Clock_create(ScalarSwMsLink_drvTimerCb,
                               pObj->timerPeriod, &clockParams, NULL);
    GT_assert(GT_DEFAULT_MASK, pObj->timer != NULL);

    ScalarSwMsLink_drvCreateOutInfo(pObj, pPrm->outRes);
    ScalarSwMsLink_drvSwitchLayout(pObj, pPrm->outLayoutMode, 0);

    for (winId = 0; winId < SCALAR_SW_MS_LINK_MAX_WIN; winId++)
        ScalarSwMsLink_drvCreateWinObj(pObj, winId);

    pObj->scCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    pObj->scCreateParams.numChannels =
        SCALAR_SW_MS_LINK_MAX_WIN * FVID2_MAX_FIELDS;
    pObj->scCreateParams.chParams = pObj->winScParams;

    memset(&cbParams, 0, sizeof(cbParams));

    cbParams.cbFxn = ScalarSwMsLink_drvFvidCb;
    cbParams.appData = pObj;
    cbParams.errCbFxn = ScalarSwMsLink_drvFvidErrCb;
    cbParams.errList = &pObj->errCbProcessList;

    switch (pObj->linkId)
    {
        default:
        case SYSTEM_LINK_ID_SCALAR_SW_MS_0:
        case SYSTEM_LINK_ID_SCALAR_SW_MS_1:
            pObj->drvInstId = VPS_M2M_INST_SEC0_SC5_WB2;
            pObj->vipInstId = SYSTEM_VIP_MAX;
            break;
        case SYSTEM_LINK_ID_SCALAR_SW_MS_2:
            pObj->drvInstId = VPS_M2M_INST_SEC0_SC3_VIP0;
            pObj->vipInstId = SYSTEM_VIP_0;
            break;
        case SYSTEM_LINK_ID_SCALAR_SW_MS_3:
            pObj->drvInstId = VPS_M2M_INST_SEC1_SC4_VIP1;
            pObj->vipInstId = SYSTEM_VIP_1;
            break;
    }

    pObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
                                    pObj->drvInstId,
                                    &pObj->scCreateParams,
                                    &pObj->scCreateStatus, &cbParams);

    GT_assert(GT_DEFAULT_MASK, pObj->fvidHandle != NULL);

    ScalarSwMsLink_drvSetScCoeffs(pObj);

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: SC_SWMS: Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvProcessData                                               
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
Int32 ScalarSwMsLink_drvProcessData(ScalarSwMsLink_Obj * pObj)
{
    UInt32 frameId, winId, freeFrameNum;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        pObj->inFrameGetCount += frameList.numFrames;

        freeFrameNum = 0;

        ScalarSwMsLink_drvLock(pObj);

        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            winId = pFrame->channelNum - pObj->startWinChannelNum;

            if (winId < pObj->numWin)
            {
                status =
                    VpsUtils_bufPutFullFrame(&pObj->winObj[winId].
                                             bufInQue[pFrame->fid], pFrame);
                if (status != FVID2_SOK)
                {
                    // Q full, release frame
                    frameList.frames[freeFrameNum] = pFrame;
                    freeFrameNum++;
                }
            }
            else
            {
                // release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        ScalarSwMsLink_drvUnlock(pObj);

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;

            pObj->inFramePutCount += frameList.numFrames;

            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &frameList);
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvMakeFrameLists                                               
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
Int32 ScalarSwMsLink_drvMakeFrameLists(ScalarSwMsLink_Obj * pObj,
                                       FVID2_FrameList * inFrameList,
                                       FVID2_FrameList * outFrameList,
                                       FVID2_FrameList * freeFrameList,
                                       FVID2_Frame * pOutFrame)
{
    UInt32 winId, fid;

    ScalarSwMsLink_OutWinObj *pWinObj;

    FVID2_Frame *pInFrame[FVID2_MAX_FIELDS];

    for (winId = 0; winId < SCALAR_SW_MS_LINK_MAX_WIN; winId++)
    {
        pWinObj = &pObj->winObj[winId];

        if (winId < pObj->numWin)
        {
            for (fid = 0; fid < FVID2_MAX_FIELDS; fid++)
            {
                VpsUtils_bufGetFullFrame(&pWinObj->bufInQue[fid],
                                         &pInFrame[fid], BIOS_NO_WAIT);

                if (!pWinObj->isInterlaced && fid > 0)
                {
                    if (pInFrame[fid])
                    {
                        freeFrameList->frames[freeFrameList->numFrames] =
                            pInFrame[fid];
                        freeFrameList->numFrames++;
                    }
                    continue;
                }

                if (pInFrame[fid])
                {
                    pWinObj->missFrameCount[fid] = 0;

                    if (pWinObj->pCurInFrame[fid])
                    {
                        freeFrameList->frames[freeFrameList->numFrames] =
                            pWinObj->pCurInFrame[fid];
                        freeFrameList->numFrames++;
                    }
                    pWinObj->pCurInFrame[fid] = pInFrame[fid];
                }
                else
                {
                    pWinObj->missFrameCount[fid]++;

                    if (pWinObj->missFrameCount[fid] >= pObj->blankThrehold)
                    {
                        pWinObj->missFrameCount[fid] = 0;
                        if (pWinObj->pCurInFrame[fid])
                        {
                            freeFrameList->frames[freeFrameList->numFrames] =
                                pWinObj->pCurInFrame[fid];
                            freeFrameList->numFrames++;
                        }
                        pWinObj->pCurInFrame[fid] = NULL;
                    }
                }

                if (pWinObj->pCurInFrame[fid])
                {
                    pWinObj->pCurInFrame[fid]->channelNum
                        = ScalarSwMsLink_makeChannelNum(winId, fid);

                    inFrameList->frames[inFrameList->numFrames] =
                        pWinObj->pCurInFrame[fid];
                }
                else
                {
                    pWinObj->blankFrame[fid].channelNum
                        = ScalarSwMsLink_makeChannelNum(winId, fid);

                    inFrameList->frames[inFrameList->numFrames] =
                        &pWinObj->blankFrame[fid];
                }
                inFrameList->numFrames++;

                outFrameList->frames[outFrameList->numFrames] =
                    &pWinObj->curOutFrame[fid];
                outFrameList->numFrames++;

                pWinObj->curOutFrame[fid].addr[0][0]
                    = (Ptr) (pWinObj->outBufAddrOffset[fid]
                             + (UInt32) pOutFrame->addr[0][0]);

            }
        }
        else
        {
            for (fid = 0; fid < FVID2_MAX_FIELDS; fid++)
            {
                VpsUtils_bufGetFullFrame(&pWinObj->bufInQue[fid],
                                         &pInFrame[fid], BIOS_NO_WAIT);

                if (pInFrame[fid])
                {
                    freeFrameList->frames[freeFrameList->numFrames] =
                        pInFrame[fid];
                    freeFrameList->numFrames++;
                }
                if (pWinObj->pCurInFrame[fid])
                {
                    freeFrameList->frames[freeFrameList->numFrames] =
                        pWinObj->pCurInFrame[fid];
                    freeFrameList->numFrames++;
                    pWinObj->pCurInFrame[fid] = NULL;
                }
            }
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvDoScaling                                               
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
Int32 ScalarSwMsLink_drvDoScaling(ScalarSwMsLink_Obj * pObj)
{
    FVID2_Frame *pOutFrame;

    Int32 status;

    pObj->freeFrameList.numFrames = 0;
    pObj->freeFrameList.perListCfg = NULL;
    pObj->freeFrameList.drvData = NULL;
    pObj->freeFrameList.reserved = NULL;

    pObj->inFrameList.numFrames = 0;
    pObj->inFrameList.perListCfg = NULL;
    pObj->inFrameList.drvData = NULL;
    pObj->inFrameList.reserved = NULL;

    pObj->outFrameList.numFrames = 0;
    pObj->outFrameList.perListCfg = NULL;
    pObj->outFrameList.drvData = NULL;
    pObj->outFrameList.reserved = NULL;

    pObj->processList.numInLists = 1;
    pObj->processList.numOutLists = 1;
    pObj->processList.drvData = NULL;
    pObj->processList.reserved = NULL;
    pObj->processList.inFrameList[0] = &pObj->inFrameList;
    pObj->processList.outFrameList[0] = &pObj->outFrameList;

    status =
        VpsUtils_bufGetEmptyFrame(&pObj->bufOutQue, &pOutFrame, BIOS_NO_WAIT);
    if (status != FVID2_SOK)
        return status;

    ScalarSwMsLink_drvLock(pObj);

    ScalarSwMsLink_drvMakeFrameLists(pObj, &pObj->inFrameList,
                                     &pObj->outFrameList, &pObj->freeFrameList,
                                     pOutFrame);

    ScalarSwMsLink_drvUnlock(pObj);

    if (pObj->freeFrameList.numFrames)
    {
        pObj->inFramePutCount += pObj->freeFrameList.numFrames;

        System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                                   pObj->createArgs.inQueParams.prevLinkQueId,
                                   &pObj->freeFrameList);
    }

#ifdef SYSTEM_DEBUG_SCALAR_RT
    Vps_printf(" %d: SC_SWMS: Scaling %d frames !!!\n", Clock_getTicks(),
               pObj->inFrameList.numFrames);
#endif

    if (pObj->inFrameList.numFrames)
    {
        pObj->processFrameCount++;

        pObj->curTime = Clock_getTicks();

        if (pObj->vipInstId != SYSTEM_VIP_MAX)
        {
            System_lockVip(pObj->vipInstId);

            if (System_clearVipResetFlag(pObj->vipInstId))
            {
                // VIP was reset since last frame processing, so we need to
                // reload VIP-SC co-effs
                ScalarSwMsLink_drvSetScCoeffs(pObj);
            }
        }

        status = FVID2_processFrames(pObj->fvidHandle, &pObj->processList);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);

        status =
            FVID2_getProcessedFrames(pObj->fvidHandle, &pObj->processList,
                                     BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        if (pObj->vipInstId != SYSTEM_VIP_MAX)
        {
            System_unlockVip(pObj->vipInstId);
        }

        pObj->curTime = Clock_getTicks() - pObj->curTime;
        pObj->totalTime += pObj->curTime;

        pObj->frameCount += pObj->inFrameList.numFrames;
        pObj->getProcessFrameCount++;
    }

#ifdef SYSTEM_DEBUG_SCALAR_RT
    Vps_printf(" %d: SC_SWMS: Scaling of %d frames Done !!!\n",
               Clock_getTicks(), pObj->inFrameList.numFrames);
#endif

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
 *  @func     ScalarSwMsLink_drvDelete                                               
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
Int32 ScalarSwMsLink_drvDelete(ScalarSwMsLink_Obj * pObj)
{
    UInt32 winId, fid;

#ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: SC_SWMS: Frames = %d (fps = %d) !!!\n",
               Clock_getTicks(),
               pObj->frameCount,
               pObj->frameCount * 100 / (pObj->totalTime / 10));
#endif

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: SC_SWMS: Delete in progress !!!\n", Clock_getTicks());
#endif

    FVID2_delete(pObj->fvidHandle, NULL);

    VpsUtils_bufDelete(&pObj->bufOutQue);

    for (winId = 0; winId < SCALAR_SW_MS_LINK_MAX_WIN; winId++)
    {
        for (fid = 0; fid < FVID2_MAX_FIELDS; fid++)
            VpsUtils_bufDelete(&pObj->winObj[winId].bufInQue[fid]);
    }

    Semaphore_delete(&pObj->lock);
    Semaphore_delete(&pObj->complete);
    Clock_delete(&pObj->timer);

    VpsUtils_memFrameFree(&pObj->outFrameFormat, pObj->outFrames,
                          SCALAR_SW_MS_LINK_MAX_OUT_FRAMES);
    VpsUtils_memFrameFree(&pObj->outFrameFormat, &pObj->winObj[0].blankFrame[0],
                          1);

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: SC_SWMS: Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvStart                                               
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
Int32 ScalarSwMsLink_drvStart(ScalarSwMsLink_Obj * pObj)
{
    Clock_start(pObj->timer);

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: SC_SWMS: Start Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvStop                                               
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
Int32 ScalarSwMsLink_drvStop(ScalarSwMsLink_Obj * pObj)
{
    Clock_stop(pObj->timer);

#ifdef SYSTEM_DEBUG_SCALAR
    Vps_printf(" %d: SC_SWMS: Stop Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvLock                                               
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
Int32 ScalarSwMsLink_drvLock(ScalarSwMsLink_Obj * pObj)
{
    return Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
}

/* ===================================================================
 *  @func     ScalarSwMsLink_drvUnlock                                               
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
Int32 ScalarSwMsLink_drvUnlock(ScalarSwMsLink_Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}
