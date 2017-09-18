/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

Void Chains_singleChCaptureSii9135Tvp7002(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams capturePrm;
    DisplayLink_CreateParams displayPrm;
    ScalarSwMsLink_CreateParams  scalarSwMsPrm;

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 displayId, captureId, scalarId;
    UInt32 startTime, elaspedTime;
    UInt32 vipInstId;
    Bool   useScSwMsLink;
    UInt32 layoutId;
    UInt32 numOutputs;

    char ch;

    captureId = SYSTEM_LINK_ID_CAPTURE;
    scalarId  = SYSTEM_LINK_ID_SCALAR_SW_MS_0;

    layoutId  = SYSTEM_LAYOUT_MODE_4CH;

    useScSwMsLink = FALSE;

    switch(chainsCfg->displayId[0])
    {
        default:
        case CHAINS_DISPLAY_ID_HDMI_ONCHIP:
            displayId = SYSTEM_LINK_ID_DISPLAY_0;
            break;
        case CHAINS_DISPLAY_ID_HDMI_OFFCHIP:
            displayId = SYSTEM_LINK_ID_DISPLAY_1;
            break;
    }

    numOutputs  = 2;

    capturePrm.numVipInst = 2;

    capturePrm.tilerEnable               = FALSE;
    capturePrm.captureScSwitchEnable = chainsCfg->captureScSwitchEnable;

    if(chainsCfg->captureSingleChOutYuv420)
        useScSwMsLink = TRUE;

    for(vipInstId=0; vipInstId<capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm                     = &capturePrm.vipInst[vipInstId];
        if(vipInstId==0)
        {
            pCaptureInstPrm->vipInstId          = VPS_CAPT_INST_VIP1_PORTA;
            pCaptureInstPrm->videoDecoderId     = FVID2_VPS_VID_DEC_SII9135_DRV;
            pCaptureInstPrm->inDataFormat       = FVID2_DF_YUV422P;
            pCaptureInstPrm->standard           = FVID2_STD_AUTO_DETECT;
            pCaptureInstPrm->frameCaptureMode   = FALSE;
            pCaptureInstPrm->fieldsMerged       = TRUE;
        }
        else
        {
            pCaptureInstPrm->vipInstId          = VPS_CAPT_INST_VIP0_PORTA;
            pCaptureInstPrm->videoDecoderId     = FVID2_VPS_VID_DEC_TVP7002_DRV;
            if(chainsCfg->enableDisSyncMode)
                pCaptureInstPrm->inDataFormat       = FVID2_DF_RGB24_888;
            else
                pCaptureInstPrm->inDataFormat       = FVID2_DF_YUV422P;

            pCaptureInstPrm->standard           = FVID2_STD_1080I_60;
        }

        pCaptureInstPrm->numOutput          = numOutputs;

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[0];

        if(chainsCfg->captureSingleChOutYuv420)
            pCaptureOutPrm->dataFormat          = FVID2_DF_YUV420SP_UV;
        else
            pCaptureOutPrm->dataFormat          = FVID2_DF_YUV422I_YUYV;

        pCaptureOutPrm->scEnable            = FALSE;
        pCaptureOutPrm->scOutWidth          = 1920;
        pCaptureOutPrm->scOutHeight         = 1080;
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;

        if(chainsCfg->captureSingleChOutYuv420
            &&
            pCaptureInstPrm->videoDecoderId == FVID2_VPS_VID_DEC_TVP7002_DRV
        )
        {
            pCaptureOutPrm->scEnable        = TRUE;
            pCaptureOutPrm->scOutWidth      = 1920;
            pCaptureOutPrm->scOutHeight     = 540;

        }

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[1];
        pCaptureOutPrm->dataFormat          = pCaptureInstPrm->outParams[0].dataFormat;
        pCaptureOutPrm->scEnable            = TRUE;
        if(vipInstId==0)
        {
            pCaptureOutPrm->scOutWidth          = 1280;
            pCaptureOutPrm->scOutHeight         = 720;
        }
        else
        {
            pCaptureOutPrm->scOutWidth          = 1920;
            pCaptureOutPrm->scOutHeight         = 540;
        }
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;
    }

    capturePrm.numOutQue = 1;
    if(useScSwMsLink)
        capturePrm.outQueParams[0].nextLink = scalarId;
    else
        capturePrm.outQueParams[0].nextLink = displayId;

    scalarSwMsPrm.inQueParams.prevLinkId    = captureId;
    scalarSwMsPrm.inQueParams.prevLinkQueId = 0;
    scalarSwMsPrm.outQueParams.nextLink     = displayId;
    scalarSwMsPrm.outRes                    = chainsCfg->displayRes;
    scalarSwMsPrm.outLayoutMode             = layoutId;
#ifdef VPS_DEBUG_CHROMA_TEAR
    scalarSwMsPrm.timerPeriod               = 16;
#else
    scalarSwMsPrm.timerPeriod               = 33;
#endif /* VPS_DEBUG_CHROMA_TEAR */

    if(useScSwMsLink)
        displayPrm.inQueParams.prevLinkId    = scalarId;
    else
        displayPrm.inQueParams.prevLinkId    = captureId;

    displayPrm.inQueParams.prevLinkQueId = 0;
    displayPrm.displayRes                = chainsCfg->displayRes;
    displayPrm.fieldsMerged              = TRUE;

    System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    System_linkCreate(captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);

    if(useScSwMsLink)
        System_linkCreate(scalarId, &scalarSwMsPrm);

    System_linkCreate(displayId, &displayPrm);

    System_memPrintHeapStatus();

    if(chainsCfg->exeTimeInSecs)
    {
        System_linkStart(displayId);
        if(useScSwMsLink)
            System_linkStart(scalarId);
        System_linkStart(captureId);

        startTime = Clock_getTicks();

        while(1)
        {
            elaspedTime = Clock_getTicks()-startTime;

            if(elaspedTime>chainsCfg->exeTimeInSecs*1000)
                break;

            Task_sleep(500);

            VpsUtils_getChar(&ch, BIOS_NO_WAIT);
            if(ch=='0')
                break;
            if(ch=='r')
                System_resumeExecution();
            if(ch=='v')
                System_linkControl(captureId, CAPTURE_LINK_CMD_FORCE_RESET, NULL, TRUE);
            if(ch=='p')
                System_linkControl(captureId, CAPTURE_LINK_CMD_PRINT_ADV_STATISTICS, NULL, TRUE);

        }

        System_linkStop(captureId);
        if(useScSwMsLink)
            System_linkStop(scalarId);
        System_linkStop(displayId);
    }

    System_linkDelete(captureId);
    if(useScSwMsLink)
        System_linkDelete(scalarId);
    System_linkDelete(displayId);

    System_displayCtrlDeInit();
}

