/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

Void Chains_singleChCaptureTvp7002DisSync(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams capturePrm;
    DisplayLink_CreateParams displayPrm;
    ScalarSwMsLink_CreateParams  scalarSwMsPrm;

    ScalarSwMsLink_LayoutParams  scalarSwMsLayoutPrm;

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 displayId, captureId, scalarId;
    UInt32 displayChId;
    UInt32 startTime, elaspedTime;
    UInt32 vipInstId;
    Bool   flipCh, useScSwMsLink;
    UInt32 layoutId;
    UInt32 numFlipCh;
    UInt32 numOutputs;

    char ch;

    captureId = SYSTEM_LINK_ID_CAPTURE;
    scalarId  = SYSTEM_LINK_ID_SCALAR_SW_MS_0;

    layoutId  = SYSTEM_LAYOUT_MODE_1CH;

    useScSwMsLink = FALSE;
    if (chainsCfg->captureSingleChOutYuv422SP)
        numOutputs  = 1;
    else
        numOutputs  = 2;

    displayChId = 0;

    capturePrm.numVipInst = 1;

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

    numFlipCh = capturePrm.numVipInst;
    if(numOutputs>1)
        numFlipCh *= 2;

    flipCh = FALSE;
    if(numFlipCh>1)
        flipCh = TRUE;

    /* In case of 422SP, we would require scaler, as display would not take in
       422SP as input format */
    if((chainsCfg->captureSingleChOutYuv420) ||
       (chainsCfg->captureSingleChOutYuv422SP))
    {
        useScSwMsLink = TRUE;
    }

    capturePrm.tilerEnable               = FALSE;

    for(vipInstId=0; vipInstId<capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm                     = &capturePrm.vipInst[vipInstId];
        pCaptureInstPrm->vipInstId          = VPS_CAPT_INST_VIP0_PORTA;
        pCaptureInstPrm->videoDecoderId     = FVID2_VPS_VID_DEC_TVP7002_DRV;
        pCaptureInstPrm->inDataFormat       = FVID2_DF_RGB24_888;
        pCaptureInstPrm->standard           = FVID2_STD_1080I_60;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput          = numOutputs;

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[0];

        if(chainsCfg->captureSingleChOutYuv420)
            pCaptureOutPrm->dataFormat          = FVID2_DF_YUV420SP_UV;
        else if (chainsCfg->captureSingleChOutYuv422SP)
            pCaptureOutPrm->dataFormat          = FVID2_DF_YUV422SP_UV;
        else
            pCaptureOutPrm->dataFormat          = FVID2_DF_YUV422I_YUYV;

        pCaptureOutPrm->scEnable            = FALSE;
        pCaptureOutPrm->scOutWidth          = 960;
        pCaptureOutPrm->scOutHeight         = 540;
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;

        if(chainsCfg->captureSingleChOutYuv420)
            pCaptureOutPrm->scEnable            = TRUE;
        else
            pCaptureOutPrm->scEnable            = FALSE;


        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[1];
        pCaptureOutPrm->dataFormat          = pCaptureInstPrm->outParams[0].dataFormat;
        pCaptureOutPrm->scEnable            = TRUE;

        pCaptureOutPrm->scOutWidth          = 960;
        pCaptureOutPrm->scOutHeight         = 540;

        pCaptureOutPrm->outQueId            = 0;
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
    if(chainsCfg->displayRes == SYSTEM_DISPLAY_RES_1080P60 ||
       chainsCfg->displayRes == SYSTEM_DISPLAY_RES_720P60)
    {
        scalarSwMsPrm.timerPeriod = 16;
    }
    else
    {
        scalarSwMsPrm.timerPeriod = 33;
    }

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

        scalarSwMsLayoutPrm.outLayoutMode = scalarSwMsPrm.outLayoutMode; /* IGNORED NOT USED */
        scalarSwMsLayoutPrm.startWinChId  = 0;

        startTime = Clock_getTicks();

        while(1)
        {
            elaspedTime = Clock_getTicks()-startTime;

            if(elaspedTime>chainsCfg->exeTimeInSecs*1000)
                break;

            if (Vps_platformIsEvm())
            {
                Task_sleep(11*1000);
            }
            else
            {
                Task_sleep(500);
            }

            if(flipCh)
            {
                {
                    if(!useScSwMsLink)
                    {
                        displayChId = (displayChId+1)%numFlipCh;

                        System_linkControl(displayId, DISPLAY_LINK_CMD_SWITCH_CH, (Ptr)displayChId, FALSE);
                    }
                    else
                    {
                        scalarSwMsLayoutPrm.startWinChId  = (scalarSwMsLayoutPrm.startWinChId+1)%(numFlipCh);

                        System_linkControl(scalarId, SCALAR_SW_MS_LINK_CMD_SWITCH_LAYOUT, &scalarSwMsLayoutPrm, TRUE);
                    }
                }
            }

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

