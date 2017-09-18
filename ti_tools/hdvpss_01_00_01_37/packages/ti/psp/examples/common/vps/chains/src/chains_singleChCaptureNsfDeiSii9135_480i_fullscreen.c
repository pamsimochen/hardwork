/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

Void Chains_singleChCaptureNsfDeiSii9135_480i_fullscreen(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams    capturePrm;
    NsfLink_CreateParams        nsfPrm;
    DeiLink_CreateParams        deiPrm;
    DisplayLink_CreateParams    displayPrm;

    CaptureLink_VipInstParams  *pCaptureInstPrm;
    CaptureLink_OutParams      *pCaptureOutPrm;

    UInt32 captureId, deiId, nsfId, displayId;

    UInt32 startTime, elaspedTime;
    UInt32 vipInstId;

    Bool enableNsf;

    char ch;

    captureId = SYSTEM_LINK_ID_CAPTURE;
    nsfId = SYSTEM_LINK_ID_NSF_0;
    switch(chainsCfg->deiId)
    {
        default:
        case CHAINS_DEI_ID:
            deiId = SYSTEM_LINK_ID_DEI_0;
            break;
#ifdef TI_816X_BUILD
        case CHAINS_DEI_ID_HQ:
            deiId = SYSTEM_LINK_ID_DEI_HQ_0;
            break;
#endif
    }
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

    capturePrm.numVipInst = 1;

    enableNsf = TRUE;
    if(chainsCfg->captureSingleChOutYuv422SP)
        enableNsf = FALSE;

    capturePrm.tilerEnable               = chainsCfg->tilerEnable;
    for(vipInstId=0; vipInstId<capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm                     = &capturePrm.vipInst[vipInstId];
        pCaptureInstPrm->vipInstId          = VPS_CAPT_INST_VIP1_PORTA;
        pCaptureInstPrm->videoDecoderId     = FVID2_VPS_VID_DEC_SII9135_DRV;
        pCaptureInstPrm->standard           = FVID2_STD_480I;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput          = 1;

        if(chainsCfg->enableDisSyncMode)
            pCaptureInstPrm->inDataFormat   = FVID2_DF_RGB24_888;
        else
            pCaptureInstPrm->inDataFormat   = FVID2_DF_YUV422P;

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[0];
        pCaptureOutPrm->dataFormat          = FVID2_DF_YUV422I_YUYV;
        pCaptureOutPrm->scEnable            = FALSE;
        pCaptureOutPrm->scOutWidth          = 720;
        pCaptureOutPrm->scOutHeight         = 240;
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;

        if(chainsCfg->captureSingleChOutYuv422SP)
        {
            pCaptureOutPrm->dataFormat          = FVID2_DF_YUV422SP_UV;
        }
    }
    capturePrm.numOutQue = 1;
    if(enableNsf)
        capturePrm.outQueParams[0].nextLink = nsfId;
    else
        capturePrm.outQueParams[0].nextLink = deiId;

    nsfPrm.bypassNsf = chainsCfg->bypassNsf;
    nsfPrm.tilerEnable = chainsCfg->tilerEnable;
    nsfPrm.inQueParams.prevLinkId = captureId;
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.enableEvenFieldOutput = FALSE;
    nsfPrm.numOutQue = 1;
    nsfPrm.outQueParams[0].nextLink = deiId;

    deiPrm.setVipScYuv422Format         = FALSE;
    deiPrm.deiCfgId                     = SYS_DEI_CFG_1CH;
    if(enableNsf)
        deiPrm.inQueParams.prevLinkId       = nsfId;
    else
        deiPrm.inQueParams.prevLinkId       = captureId;
    deiPrm.inQueParams.prevLinkQueId    = 0;
    deiPrm.outQueParams[0].nextLink     = displayId;
    deiPrm.enableOut[0]                 = TRUE;
    deiPrm.enableOut[1]                 = FALSE;
    deiPrm.tilerEnable                  = chainsCfg->tilerEnable;
    deiPrm.skipAlternateFrames          = TRUE;
    deiPrm.comprEnable                  = chainsCfg->comprEnable;
    if(chainsCfg->displayRes==SYSTEM_DISPLAY_RES_1080P60)
        deiPrm.skipAlternateFrames = FALSE;

    displayPrm.inQueParams.prevLinkId    = deiId;
    displayPrm.inQueParams.prevLinkQueId = 0;
    displayPrm.displayRes                = chainsCfg->displayRes;
    displayPrm.fieldsMerged              = TRUE;

    System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    System_linkCreate(captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);
    if(enableNsf)
        System_linkCreate(nsfId, &nsfPrm);
    System_linkCreate(deiId, &deiPrm);
    System_linkCreate(displayId, &displayPrm);

    System_memPrintHeapStatus();

    if(chainsCfg->exeTimeInSecs)
    {
        System_linkStart(displayId);
        if(enableNsf)
            System_linkStart(nsfId);
        System_linkStart(deiId);
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
        if(enableNsf)
            System_linkStop(nsfId);
        System_linkStop(deiId);
        System_linkStop(displayId);
    }

    System_linkDelete(captureId);
    if(enableNsf)
        System_linkDelete(nsfId);
    System_linkDelete(deiId);
    System_linkDelete(displayId);

    System_displayCtrlDeInit();
}

