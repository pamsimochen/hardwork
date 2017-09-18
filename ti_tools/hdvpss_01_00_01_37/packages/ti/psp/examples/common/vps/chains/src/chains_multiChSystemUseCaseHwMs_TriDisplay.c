/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

#define MAX_SUB_CHAINS 2
#define MAX_DISPLAYS   3


#define ENABLE_SDTV
#define ENABLE_HDTV2 // uncomment this to enable 2nd HDTV display
#define ENABLE_GRPX

Void Chains_multiChSystemUseCaseHwMsTriDisplay(Chains_Ctrl *chainsCfg)
{
    UInt32                          captureId, nsfId, scalarSwMsId;
    UInt32                          deiId[MAX_SUB_CHAINS];
    UInt32                          dispId[MAX_DISPLAYS];
    UInt32                          grpxId[MAX_SUB_CHAINS];

    CaptureLink_CreateParams        capturePrm;
    NsfLink_CreateParams            nsfPrm;
    DeiLink_CreateParams            deiPrm[MAX_SUB_CHAINS];
    DispHwMsLink_CreateParams       dispHwMsPrm[MAX_SUB_CHAINS];
    ScalarSwMsLink_CreateParams     scalarSwMsPrm;
    DisplayLink_CreateParams        dispPrm;

    CaptureLink_VipInstParams      *pCaptureInstPrm;
    CaptureLink_OutParams          *pCaptureOutPrm;

    UInt32                          vipInstId;
    UInt32                          startTime, elaspedTime;
    UInt32                          loop;
    Sys_DeiCfgId                    deiCfgId;
    Sys_DispCfgId                   dispCfgId;

    char ch;

    deiCfgId  = SYS_DEI_CFG_8CH_DUALCHAIN;
    dispCfgId = SYS_DISP_CFG_16CH;

    /* Figure out the configuration based on user settings */
    captureId = SYSTEM_LINK_ID_CAPTURE;
    nsfId     = SYSTEM_LINK_ID_NSF_0;

    scalarSwMsId = SYSTEM_LINK_ID_SCALAR_SW_MS_0;

    deiId[0]  = SYSTEM_LINK_ID_DEI_HQ_0;
    deiId[1]  = SYSTEM_LINK_ID_DEI_0;

    dispId[0] = SYSTEM_LINK_ID_DISPLAY_HW_MS_0;
    grpxId[0] = SYSTEM_LINK_ID_GRPX_0;

    dispId[1] = SYSTEM_LINK_ID_DISPLAY_HW_MS_1;
    grpxId[1] = SYSTEM_LINK_ID_GRPX_1;

    dispId[2] = SYSTEM_LINK_ID_DISPLAY_2;   /* SDTV - non moasiac */

    capturePrm.numOutQue = 1;
    capturePrm.outQueParams[0u].nextLink = nsfId;
    capturePrm.tilerEnable = chainsCfg->tilerEnable;
    capturePrm.numVipInst  = 4;
    for (vipInstId = 0u; vipInstId < capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm = &capturePrm.vipInst[vipInstId];

        pCaptureInstPrm->vipInstId =
            (VPS_CAPT_INST_VIP0_PORTA + vipInstId) % VPS_CAPT_INST_MAX;

        pCaptureInstPrm->videoDecoderId = FVID2_VPS_VID_DEC_TVP5158_DRV;
        pCaptureInstPrm->inDataFormat   = FVID2_DF_YUV422P;
        pCaptureInstPrm->standard       = FVID2_STD_MUX_4CH_D1;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput      = 1u;

        pCaptureOutPrm                  = &pCaptureInstPrm->outParams[0u];
        pCaptureOutPrm->dataFormat      = FVID2_DF_YUV422I_YUYV;
        pCaptureOutPrm->scEnable        = FALSE;
        pCaptureOutPrm->scOutWidth      = 0u;
        pCaptureOutPrm->scOutHeight     = 0u;
        pCaptureOutPrm->outQueId        = 0u;
        pCaptureOutPrm->subFrameEnable  = FALSE;
    }

    /* Update NSF link params */
    nsfPrm.enableEvenFieldOutput     = FALSE;
    nsfPrm.bypassNsf                 = chainsCfg->bypassNsf;
    nsfPrm.tilerEnable               = chainsCfg->tilerEnable;
    nsfPrm.inQueParams.prevLinkId    = captureId;
    nsfPrm.inQueParams.prevLinkQueId = 0u;

    #ifdef ENABLE_SDTV
    nsfPrm.enableEvenFieldOutput     = TRUE;
    #endif

    nsfPrm.enableEvenFieldOutputQueParams.nextLink = scalarSwMsId;

    scalarSwMsPrm.inQueParams.prevLinkId    = nsfId;
    scalarSwMsPrm.inQueParams.prevLinkQueId = 2;
    scalarSwMsPrm.outQueParams.nextLink     = dispId[2];
    scalarSwMsPrm.outRes                    = chainsCfg->sdDisplayRes;
    scalarSwMsPrm.outLayoutMode             = SYSTEM_LAYOUT_MODE_16CH;
    scalarSwMsPrm.timerPeriod               = 33;

    /* Chains get spilt here!! */
    nsfPrm.numOutQue = MAX_SUB_CHAINS;
    for (loop = 0u; loop < MAX_SUB_CHAINS; loop++)
    {
        nsfPrm.outQueParams[loop].nextLink      = deiId[loop];

        deiPrm[loop].deiCfgId                   = deiCfgId;
        deiPrm[loop].inQueParams.prevLinkId     = nsfId;
        deiPrm[loop].inQueParams.prevLinkQueId  = loop;
        deiPrm[loop].outQueParams[0u].nextLink  = dispId[0];
        deiPrm[loop].outQueParams[1u].nextLink  = dispId[1];
        deiPrm[loop].enableOut[0u]              = TRUE;
        deiPrm[loop].enableOut[1u]              = FALSE;

        #ifdef ENABLE_HDTV2
        deiPrm[loop].enableOut[1u]              = TRUE;
        #endif

        deiPrm[loop].tilerEnable                = chainsCfg->tilerEnable;
        deiPrm[loop].comprEnable                = chainsCfg->comprEnable;
        deiPrm[loop].skipAlternateFrames        = FALSE;
        deiPrm[loop].setVipScYuv422Format       = TRUE;

        dispHwMsPrm[loop].dispCfgId                 = dispCfgId;
        dispHwMsPrm[loop].numInQue                  = MAX_SUB_CHAINS;

        if(loop==0)
        {
            dispHwMsPrm[loop].inQueParams[0].prevLinkId = deiId[0];
            dispHwMsPrm[loop].inQueParams[1].prevLinkId = deiId[1];
        }
        else
        {
            dispHwMsPrm[loop].inQueParams[0].prevLinkId = deiId[0];
            dispHwMsPrm[loop].inQueParams[1].prevLinkId = deiId[1];
        }

        dispHwMsPrm[loop].inQueParams[0].prevLinkQueId = loop;
        dispHwMsPrm[loop].inQueParams[1].prevLinkQueId = loop;

        dispHwMsPrm[loop].displayRes = chainsCfg->displayRes;
    }

    dispPrm.inQueParams.prevLinkId    = scalarSwMsId;
    dispPrm.inQueParams.prevLinkQueId = 0;
    dispPrm.displayRes                = scalarSwMsPrm.outRes;

    System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    /* Create links */
    System_linkCreate(captureId, &capturePrm);
    System_linkControl(
        captureId,
        CAPTURE_LINK_CMD_DETECT_VIDEO,
        (Ptr)BIOS_WAIT_FOREVER,
        TRUE);
    System_linkCreate(nsfId, &nsfPrm);

    #ifdef ENABLE_SDTV
    System_linkCreate(scalarSwMsId, &scalarSwMsPrm);
    #endif

    for (loop = 0u; loop < MAX_SUB_CHAINS; loop++)
        System_linkCreate(deiId[loop], &deiPrm[loop]);

    System_linkCreate(dispId[0], &dispHwMsPrm[0]);

    #ifdef ENABLE_HDTV2
    System_linkCreate(dispId[1], &dispHwMsPrm[1]);
    #endif

    #ifdef ENABLE_SDTV
    System_linkCreate(dispId[2], &dispPrm);
    #endif

    System_memPrintHeapStatus();

    if (chainsCfg->exeTimeInSecs)
    {
        /* Start links */
        System_linkStart(dispId[0]);

        #ifdef ENABLE_GRPX
        Grpx_start(grpxId[0]);
        #endif

        #ifdef ENABLE_HDTV2
        System_linkStart(dispId[1]);

        #ifdef ENABLE_GRPX
        Grpx_start(grpxId[1]);
        #endif

        #endif

        #ifdef ENABLE_SDTV
        System_linkStart(dispId[2]);
        #endif

        System_linkStart(nsfId);

        #ifdef ENABLE_SDTV
        System_linkStart(scalarSwMsId);
        #endif

        for (loop = 0u; loop < MAX_SUB_CHAINS; loop++)
            System_linkStart(deiId[loop]);

        /* Start taking CPU load just before starting of links */
        VpsUtils_prfLoadCalcStart();

        System_linkStart(captureId);

        startTime = Clock_getTicks();
        while (1)
        {
            elaspedTime = Clock_getTicks() - startTime;

            if (elaspedTime > (chainsCfg->exeTimeInSecs * 1000u))
            {
                break;
            }

            Task_sleep(500u);

            VpsUtils_getChar(&ch, BIOS_NO_WAIT);
            if(ch=='0')
                break;
            if(ch=='r')
                System_resumeExecution();
            if(ch=='v')
                System_linkControl(captureId, CAPTURE_LINK_CMD_FORCE_RESET, NULL, TRUE);
            if(ch=='p')
                System_linkControl(captureId, CAPTURE_LINK_CMD_PRINT_ADV_STATISTICS, NULL, TRUE);
            if(ch=='b')
                System_linkControl(captureId, CAPTURE_LINK_CMD_CHANGE_BRIGHTNESS, (Ptr)(-4), TRUE);
            if(ch=='n')
                System_linkControl(captureId, CAPTURE_LINK_CMD_CHANGE_BRIGHTNESS, (Ptr)(+4), TRUE);
            if(ch=='g')
                System_linkControl(captureId, CAPTURE_LINK_CMD_CHANGE_CONTRAST, (Ptr)(-4), TRUE);
            if(ch=='h')
                System_linkControl(captureId, CAPTURE_LINK_CMD_CHANGE_CONTRAST, (Ptr)(+4), TRUE);
            if(ch=='t')
                System_linkControl(captureId, CAPTURE_LINK_CMD_CHANGE_SATURATION, (Ptr)(-4), TRUE);
            if(ch=='y')
                System_linkControl(captureId, CAPTURE_LINK_CMD_CHANGE_SATURATION, (Ptr)(+4), TRUE);

        }

        /* Stop taking load just before tear down */
        VpsUtils_prfLoadCalcStop();
        /* Stop links */
        System_linkStop(captureId);
        System_linkStop(nsfId);

        #ifdef ENABLE_SDTV
        System_linkStop(scalarSwMsId);
        #endif

        for (loop = 0u; loop < MAX_SUB_CHAINS; loop++)
        {
            System_linkStop(deiId[loop]);
        }

        #ifdef ENABLE_GRPX
        Grpx_stop(grpxId[0]);
        #endif

        System_linkStop(dispId[0]);

        #ifdef ENABLE_HDTV2

        #ifdef ENABLE_GRPX
        Grpx_stop(grpxId[1]);
        #endif

        System_linkStop(dispId[1]);
        #endif

        #ifdef ENABLE_SDTV
        System_linkStop(dispId[2]);
        #endif
    }

    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(TRUE);
    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();

    /* Delete links */
    System_linkDelete(captureId);
    System_linkDelete(nsfId);

    #ifdef ENABLE_SDTV
    System_linkDelete(scalarSwMsId);
    #endif

    for (loop = 0u; loop < MAX_SUB_CHAINS; loop++)
        System_linkDelete(deiId[loop]);

    System_linkDelete(dispId[0]);

    #ifdef ENABLE_HDTV2
    System_linkDelete(dispId[1]);
    #endif

    #ifdef ENABLE_SDTV
    System_linkDelete(dispId[2]);
    #endif

    System_displayCtrlDeInit();

    return;
}
