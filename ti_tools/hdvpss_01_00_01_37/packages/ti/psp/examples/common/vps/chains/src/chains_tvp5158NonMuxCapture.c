/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

/*
    VIP0/A - D1 capture - Dual SC YUV420 output
    VIP1/A - D1 capture - Dual SC YUV420 output

    + SC + 2x2 Display
*/
Void Chains_tvp5158NonMuxCapture(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams capturePrm;
    DisplayLink_CreateParams displayPrm;
    ScalarSwMsLink_CreateParams  scalarSwMsPrm;

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 captureId;
    UInt32 displayId;
    UInt32 scalarId;
    UInt32 vipInstId;
    UInt32 startTime, elaspedTime;

    UInt32 layoutId;

    char ch;

    captureId = SYSTEM_LINK_ID_CAPTURE;
    scalarId = SYSTEM_LINK_ID_SCALAR_SW_MS_0;

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

    capturePrm.numVipInst                = chainsCfg->numVipInst;
    if(capturePrm.numVipInst>2)
        capturePrm.numVipInst=2;

    layoutId = SYSTEM_LAYOUT_MODE_4CH;

    capturePrm.numOutQue = 1;
    capturePrm.outQueParams[0].nextLink     = scalarId;

    if(TRUE == chainsCfg->tilerEnable)
    {
        Vps_printf("\r\n Tiler cannot be supported for this option ");
    }
    capturePrm.tilerEnable               = FALSE;

    for(vipInstId=0; vipInstId<capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm                     = &capturePrm.vipInst[vipInstId];
        pCaptureInstPrm->vipInstId          = (VPS_CAPT_INST_VIP0_PORTA+vipInstId*2)%VPS_CAPT_INST_MAX;
        pCaptureInstPrm->videoDecoderId     = FVID2_VPS_VID_DEC_TVP5158_DRV;
        pCaptureInstPrm->inDataFormat       = FVID2_DF_YUV422P;
        pCaptureInstPrm->standard           = FVID2_STD_D1;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput          = 2;

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[0];
        pCaptureOutPrm->dataFormat          = FVID2_DF_YUV420SP_UV;
        pCaptureOutPrm->scEnable            = TRUE;
        pCaptureOutPrm->scOutWidth          = 720;
        pCaptureOutPrm->scOutHeight         = 240;
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;

        pCaptureOutPrm                      = &pCaptureInstPrm->outParams[1];
        pCaptureOutPrm->dataFormat          = FVID2_DF_YUV420SP_UV;
        pCaptureOutPrm->scEnable            = TRUE;
        pCaptureOutPrm->scOutWidth          = 720;
        pCaptureOutPrm->scOutHeight         = 240;
        pCaptureOutPrm->outQueId            = 0;
        pCaptureOutPrm->subFrameEnable      = FALSE;
    }

    scalarSwMsPrm.inQueParams.prevLinkId    = captureId;
    scalarSwMsPrm.inQueParams.prevLinkQueId = 0;
    scalarSwMsPrm.outQueParams.nextLink     = displayId;
    scalarSwMsPrm.outRes                    = chainsCfg->displayRes;
    scalarSwMsPrm.outLayoutMode             = layoutId;
    scalarSwMsPrm.timerPeriod               = 32;

    displayPrm.inQueParams.prevLinkId    = SYSTEM_LINK_ID_SCALAR_SW_MS_0;
    displayPrm.inQueParams.prevLinkQueId = 0;
    displayPrm.displayRes                = scalarSwMsPrm.outRes;
    displayPrm.fieldsMerged              = TRUE;

    System_linkCreate(captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);
    System_linkCreate(SYSTEM_LINK_ID_SCALAR_SW_MS_0, &scalarSwMsPrm);

    System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    System_linkCreate(displayId, &displayPrm);

    System_memPrintHeapStatus();

    if(chainsCfg->exeTimeInSecs)
    {
        System_linkStart(displayId);
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
        System_linkStop(scalarId);
        System_linkStop(displayId);
    }

    System_linkDelete(captureId);
    System_linkDelete(SYSTEM_LINK_ID_SCALAR_SW_MS_0);
    System_linkDelete(displayId);

    System_displayCtrlDeInit();

    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(TRUE);

    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();
}

