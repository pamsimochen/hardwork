/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"



Void Chains_multiChSystemUseCaseSwMsTriDisplay(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams    capturePrm;
    DisplayLink_CreateParams    displayPrm[3];
    DeiLink_CreateParams        deiPrm[2];
    ScalarSwMsLink_CreateParams scalarSwMsPrm[3];
    NsfLink_CreateParams        nsfPrm;
    NullLink_CreateParams       nullPrm;

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 displayId[3], captureId, scalarId[3], deiId[2], nsfId, grpxId[3], nullId;
    UInt32 vipInstId;
    UInt32 startTime, elaspedTime, i, queIdSdtv=0;

    UInt32 numSubChains, scalarSwMsTimerPeriod, layoutId, enableDeiVipScOut;
    UInt32 displayRes, skipAlternateFramesInDei, enableGrpx, enableGrpx2, enableNsfLink, displayDeiVipScOut, enableSdtv;

    char ch;

    captureId    = SYSTEM_LINK_ID_CAPTURE;
    nsfId        = SYSTEM_LINK_ID_NSF_0;
#ifdef TI_816X_BUILD
    deiId[0]     = SYSTEM_LINK_ID_DEI_HQ_0;
    deiId[1]     = SYSTEM_LINK_ID_DEI_0;
#endif /* #ifdef TI_816X_BUILD */
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    deiId[0]     = SYSTEM_LINK_ID_DEI_0;
    deiId[1]     = SYSTEM_LINK_ID_DEI_0;
#endif /* #ifdef TI_814X_BUILD || TI_8107_BUILD */
    scalarId[0]  = SYSTEM_LINK_ID_SCALAR_SW_MS_0; // VIP0-SC
    scalarId[1]  = SYSTEM_LINK_ID_SCALAR_SW_MS_1; // VIP1-SC
    scalarId[2]  = SYSTEM_LINK_ID_SCALAR_SW_MS_2;
    displayId[1] = SYSTEM_LINK_ID_DISPLAY_0; // ON CHIP HDMI
    displayId[0] = SYSTEM_LINK_ID_DISPLAY_1; // OFF CHIP HDMI
    displayId[2] = SYSTEM_LINK_ID_DISPLAY_2; /* SDTV - non moasiac */
    grpxId[0]    = SYSTEM_LINK_ID_GRPX_0;
    grpxId[1]    = SYSTEM_LINK_ID_GRPX_1;
    grpxId[2]    = SYSTEM_LINK_ID_GRPX_2;
    nullId       = SYSTEM_LINK_ID_NULL_0;

    enableGrpx               = TRUE;
    enableGrpx2              = FALSE;
    enableSdtv               = TRUE;
    enableDeiVipScOut        = TRUE;
    enableNsfLink            = TRUE;
    displayDeiVipScOut       = TRUE;

    numSubChains             = 2;
    scalarSwMsTimerPeriod    = 33;
    skipAlternateFramesInDei = TRUE;
    displayRes               = SYSTEM_DISPLAY_RES_1080P60;
    layoutId                 = SYSTEM_LAYOUT_MODE_8CH;

    capturePrm.numVipInst    = numSubChains*2;

    if(enableNsfLink)
    {
        capturePrm.numOutQue = 1;
        capturePrm.outQueParams[0].nextLink = nsfId;
    }
    else
    {
        capturePrm.numOutQue = 2;
        capturePrm.outQueParams[0].nextLink = deiId[0];
        capturePrm.outQueParams[1].nextLink = deiId[1];
    }

    capturePrm.tilerEnable              = chainsCfg->tilerEnable;

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

        if(!enableNsfLink)
        {
            if(vipInstId>=2)
                pCaptureOutPrm->outQueId            = 1;
        }
    }

    nsfPrm.bypassNsf                 = TRUE;
    nsfPrm.tilerEnable               = chainsCfg->tilerEnable;
    nsfPrm.inQueParams.prevLinkId    = captureId;
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.enableEvenFieldOutput     = FALSE;
    nsfPrm.numOutQue                 = numSubChains;
    nsfPrm.outQueParams[0].nextLink  = deiId[0];
    nsfPrm.outQueParams[1].nextLink  = deiId[1];

    for(i=0; i<numSubChains; i++)
    {

        deiPrm[i].setVipScYuv422Format         = FALSE;
        deiPrm[i].deiCfgId                     = SYS_DEI_CFG_DEFAULT;

        if(enableNsfLink)
            deiPrm[i].inQueParams.prevLinkId       = nsfId;
        else
            deiPrm[i].inQueParams.prevLinkId       = captureId;

        deiPrm[i].inQueParams.prevLinkQueId    = i;
        deiPrm[i].outQueParams[0].nextLink     = scalarId[i];
        deiPrm[i].outQueParams[1].nextLink     = nullId;
        deiPrm[i].enableOut[0]                 = TRUE;
        deiPrm[i].enableOut[1]                 = enableDeiVipScOut;
        deiPrm[i].tilerEnable                  = chainsCfg->tilerEnable;
        deiPrm[i].skipAlternateFrames          = skipAlternateFramesInDei;
        deiPrm[i].comprEnable                  = chainsCfg->comprEnable;

        scalarSwMsPrm[i].inQueParams.prevLinkId    = deiId[i];
        if(displayDeiVipScOut)
        {
            scalarSwMsPrm[i].inQueParams.prevLinkQueId = DEI_LINK_OUT_QUE_VIP_SC;
            deiPrm[i].outQueParams[DEI_LINK_OUT_QUE_VIP_SC].nextLink     = scalarId[i];
            deiPrm[i].outQueParams[DEI_LINK_OUT_QUE_DEI_SC].nextLink     = nullId;
        }
        else
        {
            scalarSwMsPrm[i].inQueParams.prevLinkQueId = DEI_LINK_OUT_QUE_DEI_SC;
            deiPrm[i].outQueParams[DEI_LINK_OUT_QUE_DEI_SC].nextLink     = scalarId[i];
            deiPrm[i].outQueParams[DEI_LINK_OUT_QUE_VIP_SC].nextLink     = nullId;
        }

        if(enableSdtv && i==0)
        {
            /* if HDTV will display VIP-SC output,
                then SDTV will display DEI-SC output
                and vica-versa
            */
            if(displayDeiVipScOut)
            {
                deiPrm[i].outQueParams[DEI_LINK_OUT_QUE_DEI_SC].nextLink = scalarId[2];
                queIdSdtv = DEI_LINK_OUT_QUE_DEI_SC;
            }
            else
            {
                deiPrm[i].outQueParams[DEI_LINK_OUT_QUE_VIP_SC].nextLink = scalarId[2];
                queIdSdtv = DEI_LINK_OUT_QUE_VIP_SC;
            }
        }

        scalarSwMsPrm[i].outQueParams.nextLink     = displayId[i];
        scalarSwMsPrm[i].outRes                    = displayRes;
        scalarSwMsPrm[i].outLayoutMode             = layoutId;
        scalarSwMsPrm[i].timerPeriod               = scalarSwMsTimerPeriod;

        displayPrm[i].inQueParams.prevLinkId    = scalarId[i];
        displayPrm[i].inQueParams.prevLinkQueId = 0;
        displayPrm[i].displayRes                = scalarSwMsPrm[i].outRes;
        displayPrm[i].fieldsMerged              = TRUE;

        nullPrm.numInQue = numSubChains;
        nullPrm.inQueParams[i].prevLinkId = deiId[i];

        if(displayDeiVipScOut)
            nullPrm.inQueParams[i].prevLinkQueId = DEI_LINK_OUT_QUE_DEI_SC;
        else
            nullPrm.inQueParams[i].prevLinkQueId = DEI_LINK_OUT_QUE_VIP_SC;
    }

    if(enableSdtv == TRUE)
    {
        scalarSwMsPrm[2].inQueParams.prevLinkId    = deiId[0];
        scalarSwMsPrm[2].outQueParams.nextLink     = displayId[2];
        scalarSwMsPrm[2].outRes                    = chainsCfg->sdDisplayRes;
        scalarSwMsPrm[2].outLayoutMode             = SYSTEM_LAYOUT_MODE_1CH;
        scalarSwMsPrm[2].timerPeriod               = scalarSwMsTimerPeriod;
        scalarSwMsPrm[2].inQueParams.prevLinkQueId = queIdSdtv;


        displayPrm[2].inQueParams.prevLinkId       = scalarId[2];
        displayPrm[2].inQueParams.prevLinkQueId    = 0;
        displayPrm[2].displayRes                   = chainsCfg->sdDisplayRes;
        displayPrm[2].fieldsMerged                 = TRUE;
    }

    System_displayCtrlInit(displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    System_linkCreate (captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);

    if(enableNsfLink)
        System_linkCreate(nsfId     , &nsfPrm);

    for(i=0; i<numSubChains; i++)
    {
        System_linkCreate(deiId[i]  , &deiPrm[i]);
        System_linkCreate(scalarId[i]  , &scalarSwMsPrm[i]);
        System_linkCreate(displayId[i], &displayPrm[i]);
    }

    if(enableSdtv)
    {
            System_linkCreate(scalarId[2]  , &scalarSwMsPrm[2]);
            System_linkCreate(displayId[2], &displayPrm[2]);
    }

    System_linkCreate(nullId     , &nullPrm);

    System_memPrintHeapStatus();

    if(chainsCfg->exeTimeInSecs)
    {
        if(enableGrpx)
        {
            for(i=0; i<numSubChains; i++)
                Grpx_start(grpxId[i]);
        }
        if(enableGrpx2)
            Grpx_start(grpxId[2]);

        for(i=0; i<numSubChains; i++)
        {
            System_linkStart(displayId[i]);
            System_linkStart(scalarId[i] );
            System_linkStart(deiId[i] );
        }

        if(enableSdtv)
        {
            System_linkStart(scalarId[2] );
            System_linkStart(displayId[2]);
        }
        if(enableNsfLink)
            System_linkStart(nsfId    );

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
            if(ch=='h')
                System_linkControl(captureId, CAPTURE_LINK_CMD_HALT_EXECUTION, NULL, TRUE);
        }

         /* Stop taking load just before tear down */
        VpsUtils_prfLoadCalcStop();

        System_linkStop(captureId);
        if(enableNsfLink)
            System_linkStop(nsfId    );
        for(i=0; i<numSubChains; i++)
        {
            System_linkStop(deiId[i] );
            System_linkStop(scalarId[i] );
            System_linkStop(displayId[i]);
        }

        if(enableSdtv)
        {
            System_linkStop(scalarId[2] );
            System_linkStop(displayId[2]);
        }

        if(enableGrpx)
        {
            for(i=0; i<numSubChains; i++)
                Grpx_stop(grpxId[i]);
        }
        if(enableGrpx2)
            Grpx_stop(grpxId[2]);
    }

    System_linkDelete(captureId   );
    if(enableNsfLink)
        System_linkDelete(nsfId       );
    for(i=0; i<numSubChains; i++)
    {
        System_linkDelete(deiId[i]    );
        System_linkDelete(scalarId[i] );
        System_linkDelete(displayId[i]);
    }
    if(enableSdtv)
    {
        System_linkDelete(scalarId[2] );
        System_linkDelete(displayId[2]);
    }

    System_linkDelete(nullId);

    System_displayCtrlDeInit();

    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(FALSE);

    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();
}

