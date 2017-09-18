/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"
#include "chains_swMs.h"

Void Chains_multiChSystemUseCaseSwMsTriDisplay2(Chains_Ctrl *chainsCfg)
{
    CaptureLink_CreateParams    capturePrm;
    NsfLink_CreateParams        nsfPrm;
    DupLink_CreateParams        dupPrm;
    SwMsLink_CreateParams       swMsPrm[CHAINS_SW_MS_MAX_DISPLAYS];
    DisplayLink_CreateParams    displayPrm[CHAINS_SW_MS_MAX_DISPLAYS];

    CaptureLink_VipInstParams *pCaptureInstPrm;
    CaptureLink_OutParams     *pCaptureOutPrm;

    UInt32 displayId[CHAINS_SW_MS_MAX_DISPLAYS], captureId, swMsId[CHAINS_SW_MS_MAX_DISPLAYS], nsfId, grpxId[CHAINS_SW_MS_MAX_DISPLAYS], dupId;
    UInt32 vipInstId;
    UInt32 startTime, elaspedTime, i;

    UInt32 numDisplay, displayRes, enableGrpx, enableNsfLink;

    Bool enableDup;
    Bool autoSwitchEnable = TRUE;
    Bool switchCh;
    Bool switchLayout;

    char ch;

    Chains_swMsInit();

    captureId    = SYSTEM_LINK_ID_CAPTURE;
    nsfId        = SYSTEM_LINK_ID_NSF_0;
    dupId        = SYSTEM_LINK_ID_DUP_0;

    swMsId[0] = SYSTEM_LINK_ID_SW_MS_DEI_HQ_0;
    swMsId[1] = SYSTEM_LINK_ID_SW_MS_DEI_0;
    swMsId[2] = SYSTEM_LINK_ID_SW_MS_SC_0;

    displayId[0] = SYSTEM_LINK_ID_DISPLAY_0; // ON CHIP HDMI
    displayId[1] = SYSTEM_LINK_ID_DISPLAY_1; // OFF CHIP HDMI
    displayId[2] = SYSTEM_LINK_ID_DISPLAY_2; // SDTV

    grpxId[0]    = SYSTEM_LINK_ID_GRPX_0;
    grpxId[1]    = SYSTEM_LINK_ID_GRPX_1;
    grpxId[2]    = SYSTEM_LINK_ID_GRPX_2;

    enableGrpx     = FALSE;
    enableNsfLink  = TRUE;
    numDisplay     = 3;
    displayRes     = SYSTEM_DISPLAY_RES_1080P60;

    if (numDisplay > 1)
    {
        enableDup = TRUE;
    }
    else
    {
        enableDup = FALSE;
    }

    capturePrm.numVipInst    = 4;

    capturePrm.numOutQue = 1;
    if (enableNsfLink)
    {
        capturePrm.outQueParams[0].nextLink = nsfId;
    }
    else
    {
        if (enableDup)
        {
            capturePrm.outQueParams[0].nextLink = dupId;
        }
        else
        {
            capturePrm.outQueParams[0].nextLink = swMsId[0];
        }
    }

    if(TRUE == chainsCfg->tilerEnable)
    {
        Vps_printf("\r\n Tiler cannot be supported for this option ");
    }
    capturePrm.tilerEnable = FALSE;

    for (vipInstId = 0; vipInstId < capturePrm.numVipInst; vipInstId++)
    {
        pCaptureInstPrm                 = &capturePrm.vipInst[vipInstId];
        pCaptureInstPrm->vipInstId      = (VPS_CAPT_INST_VIP0_PORTA + vipInstId)
                                        % VPS_CAPT_INST_MAX;
        pCaptureInstPrm->videoDecoderId = FVID2_VPS_VID_DEC_TVP5158_DRV;
        pCaptureInstPrm->inDataFormat   = FVID2_DF_YUV422P;
        pCaptureInstPrm->standard       = FVID2_STD_MUX_4CH_D1;
        pCaptureInstPrm->frameCaptureMode   = FALSE;
        pCaptureInstPrm->fieldsMerged       = TRUE;
        pCaptureInstPrm->numOutput      = 1;

        pCaptureOutPrm                  = &pCaptureInstPrm->outParams[0];
        pCaptureOutPrm->dataFormat      = FVID2_DF_YUV422I_YUYV;
        pCaptureOutPrm->scEnable        = FALSE;
        pCaptureOutPrm->scOutWidth      = 0;
        pCaptureOutPrm->scOutHeight     = 0;
        pCaptureOutPrm->outQueId        = 0;
        pCaptureOutPrm->subFrameEnable  = FALSE;
    }

    nsfPrm.bypassNsf                 = FALSE;
    nsfPrm.tilerEnable               = FALSE;
    nsfPrm.inQueParams.prevLinkId    = captureId;
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.enableEvenFieldOutput     = FALSE;
    nsfPrm.numOutQue                 = 1;
    if (enableDup)
    {
        nsfPrm.outQueParams[0].nextLink  = dupId;
    }
    else
    {
        nsfPrm.outQueParams[0].nextLink  = swMsId[0];
    }

    if (enableNsfLink)
    {
        dupPrm.inQueParams.prevLinkId = nsfId;
    }
    else
    {
        dupPrm.inQueParams.prevLinkId = captureId;
    }
    dupPrm.inQueParams.prevLinkQueId = 0;
    dupPrm.numOutQue                 = numDisplay;
    dupPrm.notifyNextLink            = TRUE;

    for (i = 0; i < numDisplay; i++)
    {
        if (enableDup)
        {
            swMsPrm[i].inQueParams.prevLinkId = dupId;
        }
        else
        {
            if(enableNsfLink)
            {
                swMsPrm[i].inQueParams.prevLinkId = nsfId;
            }
            else
            {
                swMsPrm[i].inQueParams.prevLinkId = captureId;
            }
        }
        swMsPrm[i].inQueParams.prevLinkQueId = i;
        swMsPrm[i].outQueParams.nextLink     = displayId[i];

        if (SYSTEM_LINK_ID_DISPLAY_2 == displayId[i])
        {
            swMsPrm[i].timerPeriod      = 33;    /* SDTV is @ 30fps */
            swMsPrm[i].layoutPrm.outRes = chainsCfg->sdDisplayRes;
        }
        else
        {
            swMsPrm[i].timerPeriod      = 16;
            swMsPrm[i].layoutPrm.outRes = displayRes;
        }

        Chains_swMsGetLayoutPrm(i, FALSE, FALSE, &swMsPrm[i].layoutPrm);

        dupPrm.outQueParams[i].nextLink = swMsId[i];

        displayPrm[i].inQueParams.prevLinkId    = swMsId[i];
        displayPrm[i].inQueParams.prevLinkQueId = 0;
        displayPrm[i].displayRes                = swMsPrm[i].layoutPrm.outRes;
        displayPrm[i].fieldsMerged              = TRUE;
    }

    System_displayCtrlInit(displayRes, chainsCfg->sdDisplayRes,
            chainsCfg->edeEnable);

    System_linkCreate (captureId, &capturePrm);
    System_linkControl(captureId, CAPTURE_LINK_CMD_DETECT_VIDEO, (Ptr)BIOS_WAIT_FOREVER, TRUE);

    if (enableNsfLink)
    {
        System_linkCreate(nsfId, &nsfPrm);
    }
    if(enableDup)
    {
        System_linkCreate(dupId, &dupPrm);
    }

    for (i = 0; i < numDisplay; i++)
    {
        System_linkCreate(swMsId[i], &swMsPrm[i]);
        System_linkCreate(displayId[i], &displayPrm[i]);
    }

    System_memPrintHeapStatus();

    if (chainsCfg->exeTimeInSecs)
    {
        if (enableGrpx)
        {
            for(i=0; i<numDisplay; i++)
                Grpx_start(grpxId[i]);
        }

        for (i = 0; i < numDisplay; i++)
        {
            System_linkStart(displayId[i]);
            System_linkStart(swMsId[i]);
        }

        if (enableDup)
        {
            System_linkStart(dupId);
        }
        if (enableNsfLink)
        {
            System_linkStart(nsfId);
        }

        /* Start taking CPU load just before starting of links */
        VpsUtils_prfLoadCalcStart();

        System_linkStart(captureId);

        startTime = Clock_getTicks();

        while(1)
        {
            switchLayout = Chains_isSwMsAutoSwitchLayout(autoSwitchEnable);
            switchCh = Chains_isSwMsAutoSwitchCh(autoSwitchEnable);

            elaspedTime = Clock_getTicks() - startTime;

            if (elaspedTime > chainsCfg->exeTimeInSecs * 1000)
            {
                break;
            }

            Task_sleep(500);

            VpsUtils_getChar(&ch, BIOS_NO_WAIT);
            if (ch=='0')
            {
                break;
            }
            else if (ch=='p')
            {
                System_linkControl(captureId, CAPTURE_LINK_CMD_PRINT_ADV_STATISTICS, NULL, TRUE);
            }
            else if (ch=='s')
            {
                switchLayout = TRUE;
            }
            else if (ch=='c')
            {
                switchCh = TRUE;
            }
            else if (ch=='a')
            {
                autoSwitchEnable ^= 1;
            }
            Chains_swMsSwitchLayout(swMsId, swMsPrm, switchLayout, switchCh, numDisplay);
        }

        /* Stop taking load just before tear down */
        VpsUtils_prfLoadCalcStop();

        System_linkStop(captureId);
        if (enableNsfLink)
        {
            System_linkStop(nsfId);
        }
        if (enableDup)
        {
            System_linkStop(dupId);
        }
        for (i = 0; i < numDisplay; i++)
        {
            System_linkStop(swMsId[i]);
            System_linkStop(displayId[i]);
        }

        if (enableGrpx)
        {
            for(i=0; i<numDisplay; i++)
                Grpx_stop(grpxId[i]);
        }
    }

    System_linkDelete(captureId);
    if (enableNsfLink)
    {
        System_linkDelete(nsfId);
    }
    if (enableDup)
    {
        System_linkDelete(dupId);
    }

    for (i = 0; i < numDisplay; i++)
    {
        System_linkDelete(swMsId[i]);
        System_linkDelete(displayId[i]);
    }

    System_displayCtrlDeInit();

    /* Print the HWI, SWI and all tasks load */
    VpsUtils_prfLoadPrintAll(FALSE);

    /* Reset the accumulated timer ticks */
    VpsUtils_prfLoadCalcReset();

    Chains_swMsExit();
}


