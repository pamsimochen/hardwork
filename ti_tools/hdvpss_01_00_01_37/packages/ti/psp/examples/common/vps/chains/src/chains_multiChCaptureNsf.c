/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

Void Chains_multiChCaptureNsf(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams capturePrm;
    DisplayLink_CreateParams displayPrm;
    ScalarSwMsLink_CreateParams  scalarSwMsPrm;

    NsfLink_CreateParams     nsfPrm;
    NullLink_CreateParams    nullDisplayPrm;

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 captureId;
    UInt32 displayId;
    UInt32 scalarId;
    UInt32 vipInstId;
    UInt32 nsfId;
    UInt32 startTime, elaspedTime;
    Bool disableDisplay;

    UInt32 layoutId;

    char ch;

    captureId = SYSTEM_LINK_ID_CAPTURE;
    nsfId = SYSTEM_LINK_ID_NSF_0;
    scalarId = SYSTEM_LINK_ID_SCALAR_SW_MS_0;

    disableDisplay = FALSE;

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

    capturePrm.numVipInst                = chainsCfg->numVipInst;
    if(capturePrm.numVipInst>4)
        capturePrm.numVipInst=4;

    layoutId = SYSTEM_LAYOUT_MODE_4CH;
    if(capturePrm.numVipInst==1)
        layoutId = SYSTEM_LAYOUT_MODE_4CH;
    else
    if(capturePrm.numVipInst>1 && capturePrm.numVipInst<=2)
        layoutId = SYSTEM_LAYOUT_MODE_8CH;
    else
        layoutId = SYSTEM_LAYOUT_MODE_16CH;

    capturePrm.numOutQue = 1;
    if(chainsCfg->enableNsfLink)
        capturePrm.outQueParams[0].nextLink     = nsfId;
    else
        capturePrm.outQueParams[0].nextLink     = scalarId;

    capturePrm.outQueParams[1].nextLink     = scalarId;

    if(TRUE == chainsCfg->tilerEnable)
    {
        Vps_printf("\r\n Tiler cannot be supported for this option ");
    }

    capturePrm.tilerEnable               = FALSE;

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

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[1];
        pCaptureOutPrm->dataFormat          = FVID2_DF_RAW_VBI;
        pCaptureOutPrm->scEnable            = FALSE;
        pCaptureOutPrm->scOutWidth          = 0;
        pCaptureOutPrm->scOutHeight         = 0;
        pCaptureOutPrm->outQueId            = 1;
        pCaptureOutPrm->subFrameEnable      = FALSE;
    }

    nsfPrm.enableEvenFieldOutput = FALSE;
    nsfPrm.bypassNsf   = chainsCfg->bypassNsf;
    nsfPrm.tilerEnable = FALSE;

    nsfPrm.inQueParams.prevLinkId = captureId;
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.numOutQue = 1;
    nsfPrm.outQueParams[0].nextLink = scalarId;

    if(chainsCfg->enableNsfLink)
        scalarSwMsPrm.inQueParams.prevLinkId    = nsfId;
    else
        scalarSwMsPrm.inQueParams.prevLinkId    = captureId;

    scalarSwMsPrm.inQueParams.prevLinkQueId = 0;
    scalarSwMsPrm.outQueParams.nextLink     = displayId;
    scalarSwMsPrm.outRes                    = chainsCfg->displayRes;
    scalarSwMsPrm.outLayoutMode             = layoutId;
    scalarSwMsPrm.timerPeriod               = 33;

    displayPrm.inQueParams.prevLinkId    = SYSTEM_LINK_ID_SCALAR_SW_MS_0;
    displayPrm.inQueParams.prevLinkQueId = 0;
    displayPrm.displayRes                = scalarSwMsPrm.outRes;
    displayPrm.fieldsMerged              = TRUE;

    nullDisplayPrm.numInQue = 1;
    nullDisplayPrm.inQueParams[0].prevLinkId = displayPrm.inQueParams.prevLinkId;
    nullDisplayPrm.inQueParams[0].prevLinkQueId = displayPrm.inQueParams.prevLinkQueId;

    System_linkCreate(captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);

    if(!disableDisplay)
        System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    if(chainsCfg->enableNsfLink)
        System_linkCreate(nsfId, &nsfPrm);
    System_linkCreate(SYSTEM_LINK_ID_SCALAR_SW_MS_0, &scalarSwMsPrm);

    if(!disableDisplay)
        System_linkCreate(displayId, &displayPrm);
    else
        System_linkCreate(displayId, &nullDisplayPrm);


    System_memPrintHeapStatus();

    if(chainsCfg->exeTimeInSecs)
    {
        System_linkStart(displayId);

        if(chainsCfg->enableNsfLink)
            System_linkStart(nsfId);
        System_linkStart(scalarId);

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

        /* Stop links */
        System_linkStop(captureId);
        if(chainsCfg->enableNsfLink)
            System_linkStop(nsfId);
        System_linkStop(scalarId);

        System_linkStop(displayId);
    }

    System_linkDelete(captureId);
    if(chainsCfg->enableNsfLink)
        System_linkDelete(nsfId);
    System_linkDelete(SYSTEM_LINK_ID_SCALAR_SW_MS_0);

    System_linkDelete(displayId);

    if(!disableDisplay)
        System_displayCtrlDeInit();

    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(TRUE);

    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();
}

