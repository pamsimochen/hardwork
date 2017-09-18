/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

#define MAX_SUB_CHAINS  (2u)

Void Chains_multiChSystemUseCaseHwMsSingleDisplay(Chains_Ctrl *chainsCfg)
{
    UInt32                          captureId, nsfId;
    UInt32                          deiId[MAX_SUB_CHAINS];
    UInt32                          deiNullId[MAX_SUB_CHAINS];
    UInt32                          dispId[MAX_SUB_CHAINS];
    UInt32                          grpxId[MAX_SUB_CHAINS];
    CaptureLink_CreateParams        capturePrm;
    NsfLink_CreateParams            nsfPrm;
    DeiLink_CreateParams            deiPrm[MAX_SUB_CHAINS];
    NullLink_CreateParams           deiNullPrm[MAX_SUB_CHAINS];
    NullLink_CreateParams           dispNullPrm[MAX_SUB_CHAINS];
    DispHwMsLink_CreateParams       dispPrm[MAX_SUB_CHAINS];

    CaptureLink_VipInstParams      *pCaptureInstPrm;
    CaptureLink_OutParams          *pCaptureOutPrm;

    UInt32                          vipInstId;
    UInt32                          startTime, elaspedTime;
    UInt32                          loop, loop1;
    UInt32                          numVipInst;
    UInt32                          numSubChains, numChannels, numDisplays;
    Bool                            disableDctrl = TRUE;
    Bool                            disableDisp[MAX_SUB_CHAINS];
    Sys_DeiCfgId                    deiCfgId;
    Sys_DispCfgId                   dispCfgId;

    char ch;

    numSubChains = chainsCfg->numSubChains;
    numDisplays = chainsCfg->numDisplays;
    numVipInst = chainsCfg->numVipInst;
    GT_assert( GT_DEFAULT_MASK, numSubChains <= MAX_SUB_CHAINS);
    GT_assert( GT_DEFAULT_MASK, numDisplays <= MAX_SUB_CHAINS);
    GT_assert( GT_DEFAULT_MASK, numVipInst <= 4u);

    /* Can't have dual display with one subchain!! */
    if (2u == numDisplays)
    {
        numSubChains = 2u;
    }

    /* Only NSF link can split the channels and DEI link can't handle more than
     * 8 channels. So if more than 2 VIPs are enabled when NSF link is not used,
     * we can't have more than 1 link/display and 8 channels!! */
    if (FALSE == chainsCfg->enableNsfLink)
    {
        /* Use whichever is best possible - ignore user settings */
        numSubChains = 1u;
        numDisplays = 1u;
        if (numVipInst > 2u)
        {
            numVipInst = 2u;
        }
    }

    /* When NSF is enabled and number of displays is one, then 8 channel
     * from DEIHQ and 8 channels from DEI will go to a single display. */
    numChannels = numVipInst * 4u;

    if (numChannels > 8u)
    {
        numSubChains = 2u;
    }

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    /* We have only one instance of DEI */
    if (numSubChains >= 2u)
    {
        return(FVID2_EBADARGS);
    }
#endif /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

    /* Figure out the configuration based on user settings */
    captureId = SYSTEM_LINK_ID_CAPTURE;
    nsfId = SYSTEM_LINK_ID_NSF_0;
    if (2u == numSubChains)
    {
        deiId[0u] = SYSTEM_LINK_ID_DEI_HQ_0;
        deiId[1u] = SYSTEM_LINK_ID_DEI_0;
        deiNullId[0u] = SYSTEM_LINK_ID_NULL_0;
        deiNullId[1u] = SYSTEM_LINK_ID_NULL_1;
        if (2u == numDisplays)
        {
            /* When number of display is 2 and number of chains is 2, then
             * the channels are equally divided and given to both the
             * displays */
            switch (numChannels)
            {
                default:
                case 4u:
                    deiCfgId = SYS_DEI_CFG_2CH;
                    dispCfgId = SYS_DISP_CFG_2CH;
                    break;
                case 2u:
                    deiCfgId = SYS_DEI_CFG_1CH;
                    dispCfgId = SYS_DISP_CFG_1CH;
                    break;
                case 8u:
                    deiCfgId = SYS_DEI_CFG_4CH;
                    dispCfgId = SYS_DISP_CFG_4CH;
                    break;
                case 12u:
                    deiCfgId = SYS_DEI_CFG_6CH;
                    dispCfgId = SYS_DISP_CFG_6CH;
                    break;
                case 16u:
                    deiCfgId = SYS_DEI_CFG_8CH;
                    dispCfgId = SYS_DISP_CFG_8CH;
                    break;
            }
        }
        else
        {
            /* When number of display is 1 and number of chains is 2, then
             * the channels are equally divided at NSF link, goes through
             * both the DEI link and converge back at the display link */
            switch (numChannels)
            {
                default:
                case 4u:
                    deiCfgId = SYS_DEI_CFG_2CH;
                    dispCfgId = SYS_DISP_CFG_4CH;
                    break;
                case 2u:
                    deiCfgId = SYS_DEI_CFG_1CH;
                    dispCfgId = SYS_DISP_CFG_2CH;
                    break;
                case 8u:
                    deiCfgId = SYS_DEI_CFG_4CH_DUALCHAIN;
                    dispCfgId = SYS_DISP_CFG_8CH;
                    break;
                case 12u:
                    deiCfgId = SYS_DEI_CFG_6CH_DUALCHAIN;
                    dispCfgId = SYS_DISP_CFG_12CH;
                    break;
                case 16u:
                    deiCfgId = SYS_DEI_CFG_8CH_DUALCHAIN;
                    dispCfgId = SYS_DISP_CFG_16CH;
                    break;
            }
        }
    }
    else
    {
        /* In single chain mode, select the DEI to use from user option */
        switch (chainsCfg->deiId)
        {
            default:
            case CHAINS_DEI_ID:
                deiId[0u] = SYSTEM_LINK_ID_DEI_0;
                break;
            case CHAINS_DEI_ID_HQ:
                deiId[0u] = SYSTEM_LINK_ID_DEI_HQ_0;
                break;
        }
        deiNullId[0u] = SYSTEM_LINK_ID_NULL_0;

        /* In single channel/single chain mode, DEI can't handle more than
         * 8 channels, hence restrict to 8 channel. */
        switch (numChannels)
        {
            default:
            case 4u:
                deiCfgId = SYS_DEI_CFG_4CH;
                dispCfgId = SYS_DISP_CFG_4CH;
                break;
            case 1u:
                deiCfgId = SYS_DEI_CFG_1CH;
                dispCfgId = SYS_DISP_CFG_1CH;
                break;
            case 2u:
                deiCfgId = SYS_DEI_CFG_2CH;
                dispCfgId = SYS_DISP_CFG_2CH;
                break;
            case 8u:
                deiCfgId = SYS_DEI_CFG_8CH;
                dispCfgId = SYS_DISP_CFG_8CH;
                break;
        }
    }

    /* Decide what display link instance to use depending on user option */
    for (loop = 0u; loop < numDisplays; loop++)
    {
        disableDisp[loop] = FALSE;
        switch (chainsCfg->displayId[loop])
        {
            default:
            case CHAINS_DISPLAY_ID_HDMI_ONCHIP:
                dispId[loop] = SYSTEM_LINK_ID_DISPLAY_HW_MS_0;
                grpxId[loop] = SYSTEM_LINK_ID_GRPX_0;
                /* One display is there, hence need DCTRL settings */
                disableDctrl = FALSE;
                break;
            case CHAINS_DISPLAY_ID_HDMI_OFFCHIP:
                dispId[loop] = SYSTEM_LINK_ID_DISPLAY_HW_MS_1;
                grpxId[loop] = SYSTEM_LINK_ID_GRPX_1;
                /* One display is there, hence need DCTRL settings */
                disableDctrl = FALSE;
                break;
            case CHAINS_DISPLAY_ID_NONE:
                /* With out display, sink the DEI link buffers in NULL link */
                dispId[loop] = SYSTEM_LINK_ID_NULL_2 + loop;
                disableDisp[loop] = TRUE;
                break;
        }
    }

    /* Update capture link params */
    capturePrm.numOutQue = 1;
    if (TRUE == chainsCfg->enableNsfLink)
    {
        capturePrm.outQueParams[0u].nextLink = nsfId;
    }
    else
    {
        capturePrm.outQueParams[0u].nextLink = deiId[0u];
    }
    capturePrm.tilerEnable = chainsCfg->tilerEnable;
    capturePrm.numVipInst = numVipInst;
    for (vipInstId = 0u; vipInstId < capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm = &capturePrm.vipInst[vipInstId];
        pCaptureInstPrm->vipInstId =
            (VPS_CAPT_INST_VIP0_PORTA + vipInstId) % VPS_CAPT_INST_MAX;
        pCaptureInstPrm->videoDecoderId = FVID2_VPS_VID_DEC_TVP5158_DRV;
        pCaptureInstPrm->inDataFormat = FVID2_DF_YUV422P;
        pCaptureInstPrm->standard = FVID2_STD_MUX_4CH_D1;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput = 1u;

        pCaptureOutPrm = &pCaptureInstPrm->outParams[0u];
        pCaptureOutPrm->dataFormat = FVID2_DF_YUV422I_YUYV;
        pCaptureOutPrm->scEnable = FALSE;
        pCaptureOutPrm->scOutWidth = 0u;
        pCaptureOutPrm->scOutHeight = 0u;
        pCaptureOutPrm->outQueId = 0u;
        pCaptureOutPrm->subFrameEnable = FALSE;
    }

    /* Update NSF link params */
    nsfPrm.bypassNsf = chainsCfg->bypassNsf;
    nsfPrm.tilerEnable = chainsCfg->tilerEnable;
    nsfPrm.inQueParams.prevLinkId = captureId;
    nsfPrm.inQueParams.prevLinkQueId = 0u;
    /* Chains get spilt here!! */
    nsfPrm.enableEvenFieldOutput     = FALSE;
    nsfPrm.numOutQue = numSubChains;
    for (loop = 0u; loop < numSubChains; loop++)
    {
        nsfPrm.outQueParams[loop].nextLink = deiId[loop];
    }

    /* Update DEI link and DEI null link params */
    for (loop = 0u; loop < numSubChains; loop++)
    {
        deiPrm[loop].setVipScYuv422Format         = FALSE;
        deiPrm[loop].deiCfgId = deiCfgId;
        /* Link with capture or NSF link depending on presence of NSF link */
        if (TRUE == chainsCfg->enableNsfLink)
        {
            deiPrm[loop].inQueParams.prevLinkId = nsfId;
        }
        else
        {
            deiPrm[loop].inQueParams.prevLinkId = captureId;
        }
        deiPrm[loop].inQueParams.prevLinkQueId = loop;

        /* If single display/double chain mode, link both the DEI's to first
         * display link */
        if (2u == numDisplays)
        {
            deiPrm[loop].outQueParams[0u].nextLink = dispId[loop];
        }
        else
        {
            deiPrm[loop].outQueParams[0u].nextLink = dispId[0u];
        }
        deiPrm[loop].outQueParams[1u].nextLink = deiNullId[loop];
        deiPrm[loop].enableOut[0u] = TRUE;
        deiPrm[loop].enableOut[1u] = FALSE;
        deiPrm[loop].tilerEnable = chainsCfg->tilerEnable;
        deiPrm[loop].comprEnable = chainsCfg->comprEnable;

        /* For 1080p30 and 1080i60, skip alternate frames. 1080p60 is 1 to 1 */
        deiPrm[loop].skipAlternateFrames = TRUE;
        if (SYSTEM_DISPLAY_RES_1080P60 == chainsCfg->displayRes)
        {
            deiPrm[loop].skipAlternateFrames = FALSE;
        }

        deiNullPrm[loop].numInQue = 1u;
        deiNullPrm[loop].inQueParams[0].prevLinkId = deiId[loop];
        deiNullPrm[loop].inQueParams[0].prevLinkQueId = 1u;
    }

    /* Update display link params */
    for (loop = 0u; loop < numDisplays; loop++)
    {
        dispPrm[loop].dispCfgId = dispCfgId;
        /* If single display/double chain mode, both the DEI link come to the
         * same display. Hence number of inQs will be 2. */
        dispPrm[loop].numInQue = (numSubChains / numDisplays);
        dispNullPrm[loop].numInQue = (numSubChains / numDisplays);
        for (loop1 = 0u; loop1 < (numSubChains / numDisplays); loop1++)
        {
            if (1u == numDisplays)
            {
                dispPrm[loop].inQueParams[loop1].prevLinkId = deiId[loop1];
            }
            else
            {
                dispPrm[loop].inQueParams[loop1].prevLinkId = deiId[loop];
            }
            dispPrm[loop].inQueParams[loop1].prevLinkQueId = 0u;

            dispNullPrm[loop].inQueParams[loop1].prevLinkId =
                dispPrm[loop].inQueParams[loop1].prevLinkId;
            dispNullPrm[loop].inQueParams[loop1].prevLinkQueId =
                dispPrm[loop].inQueParams[loop1].prevLinkQueId;
        }
        dispPrm[loop].displayRes = chainsCfg->displayRes;
    }

    /* Init DCTRL only if needed */
    if (!disableDctrl)
    {
        System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);
    }

    /* Create links */
    System_linkCreate(captureId, &capturePrm);
    System_linkControl(
        captureId,
        CAPTURE_LINK_CMD_DETECT_VIDEO,
        (Ptr)BIOS_WAIT_FOREVER,
        TRUE);
    if (TRUE == chainsCfg->enableNsfLink)
    {
        System_linkCreate(nsfId, &nsfPrm);
    }
    for (loop = 0u; loop < numSubChains; loop++)
    {
        System_linkCreate(deiId[loop], &deiPrm[loop]);
        System_linkCreate(deiNullId[loop], &deiNullPrm[loop]);
    }
    for (loop = 0u; loop < numDisplays; loop++)
    {
        if (!disableDisp[loop])
        {
            System_linkCreate(dispId[loop], &dispPrm[loop]);
        }
        else
        {
            System_linkCreate(dispId[loop], &dispNullPrm[loop]);
        }
    }

    System_memPrintHeapStatus();

    if (chainsCfg->exeTimeInSecs)
    {
        /* Start links */
        for (loop = 0u; loop < numDisplays; loop++)
        {
            System_linkStart(dispId[loop]);

            /* Start Grpx if needed */
            if ((!disableDctrl) && chainsCfg->grpxEnable)
            {
                Grpx_start(grpxId[loop]);
            }
        }
        if (TRUE == chainsCfg->enableNsfLink)
        {
            System_linkStart(nsfId);
        }
        for (loop = 0u; loop < numSubChains; loop++)
        {
            System_linkStart(deiNullId[loop]);
            System_linkStart(deiId[loop]);
        }

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

        }

        /* Stop taking load just before tear down */
        VpsUtils_prfLoadCalcStop();
        /* Stop links */
        System_linkStop(captureId);
        if (TRUE == chainsCfg->enableNsfLink)
        {
            System_linkStop(nsfId);
        }
        for (loop = 0u; loop < numSubChains; loop++)
        {
            System_linkStop(deiId[loop]);
            System_linkStop(deiNullId[loop]);
        }
        for (loop = 0u; loop < numDisplays; loop++)
        {
            if ((!disableDctrl) && chainsCfg->grpxEnable)
            {
                Grpx_stop(grpxId[loop]);
            }
            System_linkStop(dispId[loop]);
        }
    }

    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(TRUE);
    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();

    /* Delete links */
    System_linkDelete(captureId);
    if (TRUE == chainsCfg->enableNsfLink)
    {
        System_linkDelete(nsfId);
    }
    for (loop = 0u; loop < numSubChains; loop++)
    {
        System_linkDelete(deiId[loop]);
        System_linkDelete(deiNullId[loop]);
    }
    for (loop = 0u; loop < numDisplays; loop++)
    {
        System_linkDelete(dispId[loop]);
    }

    if (!disableDctrl)
        System_displayCtrlDeInit();

    return;
}
