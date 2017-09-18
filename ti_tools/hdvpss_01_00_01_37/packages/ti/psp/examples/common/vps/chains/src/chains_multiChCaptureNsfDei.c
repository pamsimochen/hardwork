/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"



Void Chains_multiChCaptureNsfDei(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams    capturePrm;
    DisplayLink_CreateParams    displayPrm;
    DeiLink_CreateParams        deiPrm;
    NullLink_CreateParams       nullPrm;
    ScalarSwMsLink_CreateParams scalarSwMsPrm;

    NsfLink_CreateParams        nsfPrm;
    NullLink_CreateParams       nullDisplayPrm;

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 displayId, captureId, scalarId, deiId, nsfId, nullId;
    UInt32 vipInstId;
    UInt32 startTime, elaspedTime;

    Bool disableDisplay;

    UInt32 layoutId;

    char ch;

    disableDisplay = FALSE;

    captureId   = SYSTEM_LINK_ID_CAPTURE;
    scalarId    = SYSTEM_LINK_ID_SCALAR_SW_MS_0;
    nullId      = SYSTEM_LINK_ID_NULL_1;
    nsfId       = SYSTEM_LINK_ID_NSF_0;

    switch(chainsCfg->displayId[0])
    {
        default:
        case CHAINS_DISPLAY_ID_HDMI_ONCHIP:
            displayId = SYSTEM_LINK_ID_DISPLAY_0;
            break;
        case CHAINS_DISPLAY_ID_HDMI_OFFCHIP:
            displayId = SYSTEM_LINK_ID_DISPLAY_1;
            break;
        case CHAINS_DISPLAY_ID_NONE:
            displayId = SYSTEM_LINK_ID_NULL_0;
            disableDisplay = TRUE;
            break;
    }

    switch(chainsCfg->deiId)
    {
        default:
        case CHAINS_DEI_ID:
            deiId       = SYSTEM_LINK_ID_DEI_0;
            break;
#ifdef TI_816X_BUILD
        case CHAINS_DEI_ID_HQ:
            deiId       = SYSTEM_LINK_ID_DEI_HQ_0;
            break;
#endif /* TI_816X_BUILD */
    }

    capturePrm.numVipInst                = chainsCfg->numVipInst;
    if(capturePrm.numVipInst>2)
        capturePrm.numVipInst=2;

    if(chainsCfg->enableNsfLink==FALSE)
        capturePrm.numVipInst = 1;

    layoutId = SYSTEM_LAYOUT_MODE_4CH;
    if(capturePrm.numVipInst>1)
        layoutId = SYSTEM_LAYOUT_MODE_8CH;

    capturePrm.numOutQue = 1;
    if(chainsCfg->enableNsfLink)
        capturePrm.outQueParams[0].nextLink = nsfId;
    else
        capturePrm.outQueParams[0].nextLink = deiId;

    capturePrm.tilerEnable               = chainsCfg->tilerEnable;

    for(vipInstId=0; vipInstId<capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm                     = &capturePrm.vipInst[vipInstId];
        pCaptureInstPrm->vipInstId          = (VPS_CAPT_INST_VIP0_PORTA+vipInstId)%VPS_CAPT_INST_MAX;
        pCaptureInstPrm->videoDecoderId     = FVID2_VPS_VID_DEC_TVP5158_DRV;
        pCaptureInstPrm->inDataFormat       = FVID2_DF_YUV422P;
        pCaptureInstPrm->standard           = FVID2_STD_MUX_4CH_D1;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput          = 1;

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[0];
        pCaptureOutPrm->dataFormat          = FVID2_DF_YUV422I_YUYV;
        pCaptureOutPrm->scEnable            = FALSE;
        pCaptureOutPrm->scOutWidth          = 0;
        pCaptureOutPrm->scOutHeight         = 0;
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;
    }

    nsfPrm.bypassNsf = chainsCfg->bypassNsf;
    nsfPrm.tilerEnable = chainsCfg->tilerEnable;
    nsfPrm.inQueParams.prevLinkId = SYSTEM_LINK_ID_CAPTURE;
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.enableEvenFieldOutput = FALSE;
    nsfPrm.numOutQue = 1;
    nsfPrm.outQueParams[0].nextLink = deiId;

    deiPrm.setVipScYuv422Format             = FALSE;
    deiPrm.deiCfgId                         = SYS_DEI_CFG_4CH;
    if(capturePrm.numVipInst>1)
        deiPrm.deiCfgId                     = SYS_DEI_CFG_DEFAULT;
    if(chainsCfg->enableNsfLink)
        deiPrm.inQueParams.prevLinkId       = nsfId;
    else
        deiPrm.inQueParams.prevLinkId       = captureId;
    deiPrm.inQueParams.prevLinkQueId    = 0;
    deiPrm.outQueParams[0].nextLink     = scalarId;
    deiPrm.outQueParams[1].nextLink     = nullId;
    deiPrm.enableOut[0]                 = TRUE;
    deiPrm.enableOut[1]                 = TRUE;
    deiPrm.tilerEnable                  = chainsCfg->tilerEnable;
    deiPrm.skipAlternateFrames          = TRUE;
    deiPrm.comprEnable                  = chainsCfg->comprEnable;
    if(chainsCfg->displayRes==SYSTEM_DISPLAY_RES_1080P60)
        deiPrm.skipAlternateFrames = FALSE;

    scalarSwMsPrm.inQueParams.prevLinkId    = deiId;

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
    scalarSwMsPrm.inQueParams.prevLinkQueId = 0;

    nullPrm.numInQue                     = 1;
    nullPrm.inQueParams[0].prevLinkId    = deiId;
    nullPrm.inQueParams[0].prevLinkQueId = 1;

    if(chainsCfg->deiOutDisplayId==DEI_LINK_OUT_QUE_VIP_SC)
    {
        deiPrm.outQueParams[0].nextLink         = nullId;
        deiPrm.outQueParams[1].nextLink         = scalarId;
        scalarSwMsPrm.inQueParams.prevLinkQueId = 1;
        nullPrm.inQueParams[0].prevLinkQueId    = 0;
    }

    displayPrm.inQueParams.prevLinkId    = scalarId;
    displayPrm.inQueParams.prevLinkQueId = 0;
    displayPrm.displayRes                = scalarSwMsPrm.outRes;
    displayPrm.fieldsMerged              = TRUE;

    nullDisplayPrm.numInQue = 1;
    nullDisplayPrm.inQueParams[0].prevLinkId = displayPrm.inQueParams.prevLinkId;
    nullDisplayPrm.inQueParams[0].prevLinkQueId = displayPrm.inQueParams.prevLinkQueId;

    System_linkCreate (captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);

    if(!disableDisplay)
        System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    if(chainsCfg->enableNsfLink)
        System_linkCreate(nsfId     , &nsfPrm);

    System_linkCreate(deiId     , &deiPrm);
    System_linkCreate(scalarId  , &scalarSwMsPrm);
    System_linkCreate(nullId    , &nullPrm);

    if(!disableDisplay)
        System_linkCreate(displayId, &displayPrm);
    else
        System_linkCreate(displayId, &nullDisplayPrm);

    System_memPrintHeapStatus();

    if(chainsCfg->exeTimeInSecs)
    {
        System_linkStart(displayId);
        if(chainsCfg->enableNsfLink)
            System_linkStart(nsfId    );
        System_linkStart(nullId   );
        System_linkStart(scalarId );
        System_linkStart(deiId    );
        /* Start taking CPU load just before starting of links */
        VpsUtils_prfLoadCalcStart();
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

         /* Stop taking load just before tear down */
        VpsUtils_prfLoadCalcStop();

        System_linkStop(captureId);
        if(chainsCfg->enableNsfLink)
            System_linkStop(nsfId    );
        System_linkStop(deiId    );
        System_linkStop(scalarId );
        System_linkStop(nullId   );
        System_linkStop(displayId);
    }
    System_linkDelete(captureId);
    if(chainsCfg->enableNsfLink)
        System_linkDelete(nsfId    );
    System_linkDelete(deiId    );
    System_linkDelete(scalarId );
    System_linkDelete(nullId   );
    System_linkDelete(displayId);

    if(!disableDisplay)
        System_displayCtrlDeInit();
    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(TRUE);
    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();
}

