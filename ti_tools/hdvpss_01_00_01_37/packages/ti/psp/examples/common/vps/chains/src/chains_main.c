/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "chains.h"

Chains_Ctrl gChains_ctrl;

Void Chains_setDefaultCfg()
{
    if (Vps_platformIsEvm())
    {
        gChains_ctrl.exeTimeInSecs = (Uint32)60;
    }
    else
    {
        gChains_ctrl.exeTimeInSecs = 1;
    }
    gChains_ctrl.loopCount = 1;

    gChains_ctrl.numVipInst = 4u;
#ifdef TI_814X_BUILD
    gChains_ctrl.numVipInst = 2u;
#endif
#ifdef TI_8107_BUILD
    gChains_ctrl.numVipInst = 1u;
#endif

    gChains_ctrl.numSubChains = 1u;
    gChains_ctrl.numDisplays = 1u;
    gChains_ctrl.displayId[0] = CHAINS_DISPLAY_ID_HDMI_OFFCHIP;
    gChains_ctrl.displayId[1] = CHAINS_DISPLAY_ID_HDMI_ONCHIP;
#ifdef TI_816X_BUILD
    gChains_ctrl.deiId = CHAINS_DEI_ID_HQ;
#endif /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    gChains_ctrl.deiId = CHAINS_DEI_ID;
#endif /* TI_814X_BUILD || TI_8107_BUILD */
    gChains_ctrl.deiOutDisplayId = DEI_LINK_OUT_QUE_DEI_SC;
    gChains_ctrl.enableNsfLink   = FALSE;
    gChains_ctrl.bypassNsf       = FALSE;
    gChains_ctrl.grpxEnable      = TRUE;
    gChains_ctrl.cpuRev          = VPS_PLATFORM_CPU_REV_1_0;
    gChains_ctrl.boardId         = VPS_PLATFORM_BOARD_VS;
    gChains_ctrl.displayRes      = SYSTEM_DISPLAY_RES_1080P60;
    gChains_ctrl.sdDisplayRes    = SYSTEM_DISPLAY_RES_NTSC;
    gChains_ctrl.tilerEnable     = FALSE;
    gChains_ctrl.comprEnable     = FALSE;
    gChains_ctrl.autoRunEnable   = FALSE;
    gChains_ctrl.captureScSwitchEnable = FALSE;
    gChains_ctrl.edeEnable = TRUE;

    #ifndef PLATFORM_ZEBU
    if (Vps_platformIsEvm())
    {
        Chains_detectBoard();

        if((gChains_ctrl.boardId==VPS_PLATFORM_BOARD_VC) ||
           (gChains_ctrl.boardId==VPS_PLATFORM_BOARD_CATALOG))
        {
            gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080P60;
        }
    }
    #endif

    #if 0
    VpsUtils_setL3Pri(0, 0x00F0);
    #endif
}



Void Chains_initLinks()
{
    Vps_printf(" %d: CHAINS  : Initializing Links !!! \r\n",
        Clock_getTicks()
    );
    System_memPrintHeapStatus();

    CaptureLink_init();
    NsfLink_init();

    DeiLink_init();

    ScalarSwMsLink_init();
    DisplayLink_init();
    NullLink_init();
    DispHwMsLink_init();
    Grpx_init();
    ScLink_init();
    DupLink_init();
    SwMsLink_init();

#ifndef PLATFORM_ZEBU
    Vps_platformVideoResetVideoDevices();
#endif

    Vps_printf(" %d: CHAINS  : Initializing Links ... DONE !!! \r\n",
        Clock_getTicks()
    );

    VpsUtils_remoteSendChar('s');
}

Void Chains_deInitLinks()
{
    Vps_printf(" %d: CHAINS  : De-Initializing Links !!! \r\n",
        Clock_getTicks()
    );

    NullLink_deInit();
    DisplayLink_deInit();
    ScalarSwMsLink_deInit();

    DeiLink_deInit();

    NsfLink_deInit();
    CaptureLink_deInit();
    DispHwMsLink_deInit();
    Grpx_deInit();
    ScLink_deInit();
    DupLink_deInit();
    SwMsLink_deInit();

    System_memPrintHeapStatus();

    Vps_printf(" %d: CHAINS  : De-Initializing Links ... DONE !!! \r\n",
        Clock_getTicks()
    );

    VpsUtils_remoteSendChar('e');
}

Int32 Chains_detectBoard()
{
    /* Get CPU version */
    gChains_ctrl.cpuRev = Vps_platformGetCpuRev();
    if (gChains_ctrl.cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        gChains_ctrl.cpuRev = VPS_PLATFORM_CPU_REV_UNKNOWN;
    }

    /* Detect board */
    gChains_ctrl.boardId = Vps_platformGetBoardId();
    if (gChains_ctrl.boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        gChains_ctrl.boardId = VPS_PLATFORM_BOARD_UNKNOWN;
    }

    /* Print platform information */
    Vps_platformPrintInfo();

    return 0;
}

Int32 Chains_checkBoard(Vps_PlatformBoardId reqdBoardId)
{
    if(reqdBoardId>=VPS_PLATFORM_BOARD_MAX)
        reqdBoardId = VPS_PLATFORM_BOARD_UNKNOWN;

    if(gChains_ctrl.boardId!=reqdBoardId)
    {
        Vps_printf(" %d: CHAINS  : This application needs [%s] Board  !!! \r\n",
            Clock_getTicks(), Vps_platformGetBoardString());
        Vps_printf(" %d: CHAINS  : Currently detected [%s] Board  !!! \r\n",
            Clock_getTicks(), Vps_platformGetBoardString());
        Vps_printf(" %d: CHAINS  : Incompatible boards .. CANNOT RUN APPLICATION !!! \r\n",
            Clock_getTicks());

        return FVID2_EFAIL;
    }

    return FVID2_SOK;
}

Void Chains_run(Chains_RunFunc chainsRunFunc, Vps_PlatformBoardId reqdBoardId)
{
    UInt32 loopCount;
    Int32 status;
    Chains_Ctrl chainsCtrl;

#ifndef PLATFORM_ZEBU
    Vps_platformVideoResetVideoDevices();
#endif

    if (Vps_platformIsEvm())
    {
        status = Chains_checkBoard(reqdBoardId);
        if(status!=FVID2_SOK)
            return;
    }

    #ifndef SYSTEM_USE_TILER
    if(gChains_ctrl.tilerEnable)
    {
        Vps_printf(" %d: SYSTEM: !!! WARNING: Tiler is DISABLED via compile option, so forcing tiler enable configuration to FALSE\n !!!", Clock_getTicks());
        gChains_ctrl.tilerEnable = FALSE;
    }
    #endif

    memcpy(&chainsCtrl, &gChains_ctrl, sizeof(gChains_ctrl));

    for(loopCount=0; loopCount<chainsCtrl.loopCount; loopCount++)
    {
        #ifdef SYSTEM_DEBUG
        Vps_printf(" %d: CHAINS  : Starting %d of %d !!! \r\n",
            Clock_getTicks(),
            loopCount+1,
            chainsCtrl.loopCount
        );
        #endif
        System_memPrintHeapStatus();

        GT_assert( GT_DEFAULT_MASK, chainsRunFunc!=NULL);
        chainsRunFunc(&chainsCtrl);

        System_memPrintHeapStatus();
        #ifdef SYSTEM_DEBUG
        Vps_printf(" %d: CHAINS  : Stopped %d of %d !!! \r\n",
            Clock_getTicks(),
            loopCount+1,
            chainsCtrl.loopCount
        );
        #endif
    }

    VpsUtils_remoteSendChar('x');
}

char gChains_menuMain0[] = {
    "\r\n ============"
    "\r\n Chain Select"
    "\r\n ============"
    "\r\n"
};

char gChains_menuMainVs[] = {
    "\r\n"
    "\r\n 1: Single CH Capture + Scale   + Display                     ( 2CH 2x TVP5158, NTSC, YUV420SP)"
    "\r\n 2: Multi  CH Capture + Scale   + Display                     (        TVP5158, NTSC, YUV422I )"
    "\r\n 3: Multi  CH Capture + NSF     + Scale   + Display           (        TVP5158, NTSC, YUV422I )"
    "\r\n 4: Multi  CH Capture + DEI     + Scale   + Display           ( 4CH 1x TVP5158, NTSC, YUV422I )"
    "\r\n 5: Multi  CH Capture + NSF     + DEI     + Scale   + Display ( 8CH 2x TVP5158, NTSC, YUV422I )"
#ifdef TI_816X_BUILD
    "\r\n 6: Multi  CH Capture + DEI     + Mosaic Display              ( 8CH 2x TVP5158, NTSC, YUV422I )"
    "\r\n 7: Multi  CH System Use Case - HW Mosaic Triple Display      (16CH 4x TVP5158, NTSC, YUV422I )"
    "\r\n 8: Multi  CH System Use Case - SW Mosaic Triple Display      (16CH 4x TVP5158, NTSC, YUV422I )"
#endif /* TI_816X_BUILD */
    "\r\n 9: Single CH Capture + NSF     + DEI     + Display (Full screen DEI) ( 1CH 1x TVP5158, NTSC, YUV422I )"
#ifdef TI_816X_BUILD
    "\r\n a: Multi  CH System Use Case - SW Mosaic (v2) Triple Display (16CH 4x TVP5158, NTSC, YUV422I )"
#endif
    "\r\n "
    "\r\n s: System Settings "
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
};

#ifdef TI_816X_BUILD
char gChains_autoRunVs[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'x'};
#else
char gChains_autoRunVs[] = {'1', '2', '3', '4', '5', '9', 'x'};
#endif

char gChains_menuMainVc[] = {
    "\r\n"
    "\r\n Connect a 1080p60 HDMI RGB source to VIP1, this will be input for SII9135"
    "\r\n Connect a 1080i60 YPbPr component source to VIP0, this will be input for TVP7002"
    "\r\n"
    "\r\n 1: Single CH Capture + Display                     (1x SII9135 16b, 1080P60, YUV422I )"
    "\r\n 2: Single CH Capture + SC      + Display           (1x SII9135 16b, 1080P60, YUV420SP)"
    "\r\n 3: Single CH Capture + NSF     + DEI     + Display (1x TVP7002 16b, 1080i60, YUV422I )"
    "\r\n 4: Single CH Capture + DEI     + Display           (1x TVP7002 16b, 1080i60, YUV420SP)"
    "\r\n 5: Single CH Capture + SC      + Display           (1x TVP7002 24b, 1080i60, YUV420SP)"
    "\r\n 6: Single CH Capture + SC      + Display           (1x TVP7002 24b, 1080i60, YUV422SP)"
    "\r\n 7: Single CH Capture + SC      + Display           (1x TVP7002 16b + 1x SII9135 16b, 1080i60+1080P60, YUV420SP) "
    "\r\n 8: Single CH Capture + SC      + Display           (1x TVP7002 24b + 1x SII9135 16b, 1080i60+1080P60, YUV420SP) "
    "\r\n 9: Single CH Capture + SC      + Display           (1x SII9135 16b, 1080P60, YUV422SP)"
    "\r\n a: Single CH Capture + NSF     + DEI     + Display (1x SII9135 16b, 480i60  -> 480p60 , YUV422I )"
    "\r\n b: Single CH Capture + NSF     + DEI     + Display (1x SII9135 16b, 1080i60 -> 1080p60, YUV422I )"
    "\r\n c: Single CH Capture + NSF     + DEI     + Display (1x SII9135 16b, 480i60  -> 1080p60, YUV422I )"
    "\r\n d: Single CH Capture + DEI     + Display (1x SII9135 16b, 480i60  -> 480p60 , YUV422I )"
    "\r\n e: Single CH Capture + DEI     + Display (1x SII9135 16b, 1080i60 -> 1080p60, YUV422I )"
    "\r\n f: Single CH Capture + DEI     + Display (1x SII9135 16b, 480i60  -> 1080p60, YUV422I )"
    "\r\n g: Single CH Capture + SC      + Display (1x TVP7002 16b, 1080i60, YUV420SP, FieldsMerged -> 1080p30, YUV422I)"
    "\r\n h: Single CH Capture (Run-time Sc Coeff Load) + SC + Display (1x TVP7002 16b + 1x SII9135 16b, 1080i60+1080P60, YUV420SP) "

    "\r\n "
    "\r\n s: System Settings "
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
};

char gChains_autoRunVc[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'g', 'h', 'x'};

char gChains_menuMainCatalog[] = {
    "\r\n"
    "\r\n Connect a right component source to VIP port, this will be input for TVP7002"
    "\r\n"
    "\r\n 1: Single CH Capture + NSF     + DEI     + Display (VIP0: 1x TVP7002 16b, 1080i60, YUV422I )"
    "\r\n 2: Single CH Capture + DEI     + Display           (VIP0: 1x TVP7002 16b, 1080i60, YUV420SP)"
    "\r\n 3: Single CH Capture + SC      + Display           (VIP0: 1x TVP7002 16b, 1080P60, YUV422SP)"
    "\r\n 4: Single CH Capture + SC      + Display           (VIP1: 1x TVP7002 16b, 1080P60, YUV422SP)"
#ifdef TI_814X_BUILD
    "\r\n 5: Single CH Capture + Display                     (VIP0: 1x TVP7002 24b, 1080i60, YUV422I )"
    "\r\n 6: Single CH Capture + SC      + Display           (VIP0: 1x TVP7002 24b, 1080i60, YUV422SP)"
#endif /* TI_814X_BUILD */

    "\r\n "
    "\r\n s: System Settings "
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
    "\r\n Enter Choice: "
};

#ifdef TI_814X_BUILD
char gChains_autoRunCatalog[] = {'1', '2', '3', '4', '5', '6', 'x'};
#else
char gChains_autoRunCatalog[] = {'1', '2', '3', '4', 'x'};
#endif

Void Chains_menuMainShow(Vps_PlatformBoardId reqdBoardId)
{
    Vps_printf(gChains_menuMain0);

    #ifdef PLATFORM_ZEBU
    Chains_menuCurrentSettingsShow();
    #endif

    if(reqdBoardId==VPS_PLATFORM_BOARD_VC)
        Vps_printf(gChains_menuMainVc);
    else
    if(reqdBoardId==VPS_PLATFORM_BOARD_VS)
        Vps_printf(gChains_menuMainVs);
    else
    if(reqdBoardId==VPS_PLATFORM_BOARD_CATALOG)
        Vps_printf(gChains_menuMainCatalog);
    else
        Vps_printf("\n [UNKNOWN BOARD] \n\n");

}

Void Chains_menuMainRunVs(char ch)
{
    gChains_ctrl.captureSingleChOutYuv422SP = FALSE;
    gChains_ctrl.captureSingleChOutYuv420 = FALSE;
    gChains_ctrl.enableDisSyncMode = FALSE;
    gChains_ctrl.enableNsfLink = FALSE;

    switch(ch)
    {
        case '1':
            Chains_run(Chains_tvp5158NonMuxCapture, VPS_PLATFORM_BOARD_VS);
            break;
        case '2':
            Chains_run(Chains_multiChCaptureNsf, VPS_PLATFORM_BOARD_VS);
            break;
        case '3':
            gChains_ctrl.enableNsfLink = TRUE;
            Chains_run(Chains_multiChCaptureNsf, VPS_PLATFORM_BOARD_VS);
            break;
        case '4':
            Chains_run(Chains_multiChCaptureNsfDei, VPS_PLATFORM_BOARD_VS);
            break;
        case '5':
            gChains_ctrl.enableNsfLink = TRUE;
            Chains_run(Chains_multiChCaptureNsfDei, VPS_PLATFORM_BOARD_VS);
            break;
#ifdef TI_816X_BUILD
        case '6':
            Chains_run(Chains_multiChSystemUseCaseHwMsSingleDisplay, VPS_PLATFORM_BOARD_VS);
            break;
        case '7':
            gChains_ctrl.enableNsfLink = TRUE;
            Chains_run(Chains_multiChSystemUseCaseHwMsTriDisplay, VPS_PLATFORM_BOARD_VS);
            break;
        case '8':
            Chains_run(Chains_multiChSystemUseCaseSwMsTriDisplay, VPS_PLATFORM_BOARD_VS);
            break;
        case 'a':
            Chains_run(Chains_multiChSystemUseCaseSwMsTriDisplay2, VPS_PLATFORM_BOARD_VS);
            break;
#endif /* TI_816X_BUILD */
        case '9':
            //gChains_ctrl.enableNsfLink = TRUE;
            Chains_run(Chains_singleChCaptureNsfDeiTvp5158, VPS_PLATFORM_BOARD_VS);
            break;

    }
}

Void Chains_menuMainRunVc(char ch)
{
    gChains_ctrl.captureSingleChOutYuv422SP = FALSE;
    gChains_ctrl.captureSingleChOutYuv420 = FALSE;
    gChains_ctrl.enableDisSyncMode = FALSE;
    gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080P60;
    gChains_ctrl.captureScSwitchEnable = FALSE;

    switch(ch)
    {
        case '1':
            Chains_run(Chains_singleChCaptureSii9135, VPS_PLATFORM_BOARD_VC);
            break;
        case '2':
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            Chains_run(Chains_singleChCaptureSii9135, VPS_PLATFORM_BOARD_VC);
            break;

        case '3':
            Chains_run(Chains_singleChCaptureNsfDeiTvp7002, VPS_PLATFORM_BOARD_VC);
            break;
        case '4':
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            Chains_run(Chains_singleChCaptureNsfDeiTvp7002, VPS_PLATFORM_BOARD_VC);
            break;
        case '5':
            gChains_ctrl.enableDisSyncMode = TRUE;
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            gChains_ctrl.captureSingleChOutYuv422SP = FALSE;
            Chains_run(Chains_singleChCaptureTvp7002DisSync, VPS_PLATFORM_BOARD_VC);
            break;
        case '6':
            gChains_ctrl.enableDisSyncMode = TRUE;
            gChains_ctrl.captureSingleChOutYuv420 = FALSE;
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            Chains_run(Chains_singleChCaptureTvp7002DisSync, VPS_PLATFORM_BOARD_VC);
            break;

        case '7':
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            Chains_run(Chains_singleChCaptureSii9135Tvp7002, VPS_PLATFORM_BOARD_VC);
            break;
        case '8':
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            gChains_ctrl.enableDisSyncMode = TRUE;
            Chains_run(Chains_singleChCaptureSii9135Tvp7002, VPS_PLATFORM_BOARD_VC);
            break;

        case '9':
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            Chains_run(Chains_singleChCaptureSii9135, VPS_PLATFORM_BOARD_VC);
            break;

        case 'a':
            Chains_run(Chains_singleChCaptureNsfDeiSii9135_480i, VPS_PLATFORM_BOARD_VC);
            break;

        case 'b':
            Chains_run(Chains_singleChCaptureNsfDeiSii9135_1080i, VPS_PLATFORM_BOARD_VC);
            break;

        case 'c':
            Chains_run(Chains_singleChCaptureNsfDeiSii9135_480i_fullscreen, VPS_PLATFORM_BOARD_VC);
            break;

        case 'd':
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            Chains_run(Chains_singleChCaptureNsfDeiSii9135_480i, VPS_PLATFORM_BOARD_VC);
            break;

        case 'e':
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            Chains_run(Chains_singleChCaptureNsfDeiSii9135_1080i, VPS_PLATFORM_BOARD_VC);
            break;

        case 'f':
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            Chains_run(Chains_singleChCaptureNsfDeiSii9135_480i_fullscreen, VPS_PLATFORM_BOARD_VC);
            break;

        case 'g':
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080P30;
            Chains_run(Chains_singleChCaptureTvp7002FieldMerged, VPS_PLATFORM_BOARD_VC);
            break;

        case 'h':
            gChains_ctrl.captureScSwitchEnable = TRUE;
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            Chains_run(Chains_singleChCaptureSii9135Tvp7002, VPS_PLATFORM_BOARD_VC);
            break;
    }

    return;
}



Void Chains_menuMainRunCatalog(char ch)
{
    gChains_ctrl.captureSingleChOutYuv422SP = FALSE;
    gChains_ctrl.captureSingleChOutYuv420 = FALSE;
    gChains_ctrl.enableDisSyncMode = FALSE;
    gChains_ctrl.tvp7002InProgressiveMode = FALSE;

#ifdef TI_814X_BUILD
    gChains_ctrl.displayId[0] = CHAINS_DISPLAY_ID_HDMI_ONCHIP;
    gChains_ctrl.displayId[1] = CHAINS_DISPLAY_ID_HDMI_ONCHIP;
#endif /* TI_814X_BUILD */

    switch(ch)
    {
        case '1':
            Chains_run(Chains_singleChCaptureNsfDeiTvp7002, VPS_PLATFORM_BOARD_CATALOG);
            break;
        case '2':
            gChains_ctrl.captureSingleChOutYuv420 = TRUE;
            Chains_run(Chains_singleChCaptureNsfDeiTvp7002, VPS_PLATFORM_BOARD_CATALOG);

            break;

        case '3':
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            gChains_ctrl.VipPort = VPS_CAPT_INST_VIP0_PORTA;
            gChains_ctrl.tvp7002InProgressiveMode = TRUE;

            Chains_run(Chains_singleChCaptureTvp7002, VPS_PLATFORM_BOARD_CATALOG);
            break;

        case '4':
#ifdef TI_814X_BUILD
            Vps_printf("\r\n Cannot be supported, as we do not have any ");
            Vps_printf("\r\n Decoders (such as TVP7002) connected to VIP 1");
            Vps_printf("\r\n ");
#else
            gChains_ctrl.VipPort = VPS_CAPT_INST_VIP1_PORTA;
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            gChains_ctrl.tvp7002InProgressiveMode = TRUE;
            Chains_run(Chains_singleChCaptureTvp7002, VPS_PLATFORM_BOARD_CATALOG);
#endif
            break;
#ifdef TI_814X_BUILD
        case '5':
            gChains_ctrl.enableDisSyncMode = TRUE;
            gChains_ctrl.captureSingleChOutYuv420 = FALSE;
            gChains_ctrl.captureSingleChOutYuv422SP = FALSE;
            Chains_run(Chains_singleChCaptureTvp7002DisSync, VPS_PLATFORM_BOARD_CATALOG);
            break;
        case '6':
            gChains_ctrl.captureSingleChOutYuv420 = FALSE;
            gChains_ctrl.captureSingleChOutYuv422SP = TRUE;
            gChains_ctrl.enableDisSyncMode = TRUE;
            Chains_run(Chains_singleChCaptureTvp7002DisSync, VPS_PLATFORM_BOARD_CATALOG);
            break;
#endif /* TI_814X_BUILD */
    }
}

Void Chains_main(UArg arg0, UArg arg1)
{
    char ch = '0';
    Bool done;
    UInt32 index = 0;

    done = FALSE;

    Chains_initLinks();

    Chains_setDefaultCfg();

    #ifdef PLATFORM_ZEBU
    Chains_menuMainRunVs('8');
    #else
    while(!done)
    {
        Chains_menuMainShow(gChains_ctrl.boardId);
        if (FALSE == gChains_ctrl.autoRunEnable)
        {
            VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
            Vps_printf(" \r\n");
        }

        if (gChains_ctrl.boardId==VPS_PLATFORM_BOARD_VC)
        {
            if (TRUE == gChains_ctrl.autoRunEnable)
            {
                if (index >= (sizeof (gChains_autoRunVc) / sizeof (char)))
                {
                    index = 0;
                }
                ch = gChains_autoRunVc[index];
                Vps_printf("%c \r\n", ch);
                index++;
            }
            Chains_menuMainRunVc(ch);
        }
        else if( gChains_ctrl.boardId==VPS_PLATFORM_BOARD_VS)
        {
            if (TRUE == gChains_ctrl.autoRunEnable)
            {
                if (index >= (sizeof (gChains_autoRunVs) / sizeof (char)))
                {
                    index = 0;
                }
                ch = gChains_autoRunVs[index];
                Vps_printf("%c \r\n", ch);
                index++;
            }
            Chains_menuMainRunVs(ch);
        }
        else if (gChains_ctrl.boardId == VPS_PLATFORM_BOARD_CATALOG)
        {
            if (TRUE == gChains_ctrl.autoRunEnable)
            {
                if (index >= (sizeof (gChains_autoRunCatalog) / sizeof (char)))
                {
                    index = 0;
                }
                ch = gChains_autoRunCatalog[index];
                Vps_printf("%c \r\n", ch);
                index++;
            }
            Chains_menuMainRunCatalog(ch);
        }
        else
        {
            Vps_printf("\n [UNKNOWN BOARD] \n\n");
        }

        switch(ch)
        {
            case 's':
                Chains_menuSettings();
                break;
            case 'x':
                done = TRUE;
                break;
        }
    }
    #endif

    Chains_deInitLinks();
}

Void Chains_menuCurrentSettingsShow()
{
    static char *displayName[] =
        { "HDMI_ONCHIP", "HDMI_OFFCHIP", "NONE" };

    static char *deiName[] =
        { "DEI-HQ", "DEI" };

    static char *deiOutName[] =
        { "DEI-SC", "VIP-SC" };

    static char *nsfModeName[] =
        { "SNF + TNF", "CHR DS ONLY" };

    static char *enableDisableName[] =
        { "OFF", "ON" };

    static char *displayResName[] =
        { "1080P30", "1080I60", "1080P60", "720P60" };

    static char *sdDisplayResName[] =
        { "NTSC", "PAL" };

    Vps_printf("\r\n Current System Settings,");
    Vps_printf("\r\n Num Video Ports       : %d", gChains_ctrl.numVipInst);
    Vps_printf("\r\n Number of Displays    : %d", gChains_ctrl.numDisplays);
    Vps_printf("\r\n Display 1             : %s", displayName[gChains_ctrl.displayId[0]]);
    Vps_printf("\r\n Display 2             : %s", displayName[gChains_ctrl.displayId[1]]);
    Vps_printf("\r\n DEI                   : %s", deiName[gChains_ctrl.deiId]);
    Vps_printf("\r\n DEI Output to Display : %s", deiOutName[gChains_ctrl.deiOutDisplayId]);
    Vps_printf("\r\n NSF Mode              : %s", nsfModeName[gChains_ctrl.bypassNsf]);
    Vps_printf("\r\n Execution Time        : %d secs", gChains_ctrl.exeTimeInSecs);
    Vps_printf("\r\n Loop Count            : %d", gChains_ctrl.loopCount);
    Vps_printf("\r\n GRPX                  : %s", enableDisableName[gChains_ctrl.grpxEnable]);
    Vps_printf("\r\n Display Resolution    : %s", displayResName[gChains_ctrl.displayRes]);
    Vps_printf("\r\n Display Resolution    : %s", sdDisplayResName[gChains_ctrl.sdDisplayRes - SYSTEM_DISPLAY_RES_NTSC]);
    Vps_printf("\r\n Tiler                 : %s", enableDisableName[gChains_ctrl.tilerEnable]);
    Vps_printf("\r\n Number of Sub Chains  : %d", gChains_ctrl.numSubChains);
    Vps_printf("\r\n Auto Run              : %s", enableDisableName[gChains_ctrl.autoRunEnable]);
}

Void Chains_menuSettingsDisplaySelect(UInt32 displayNum)
{
    char inputStr[10];
    Int32 value;

    Vps_printf(" \r\n Enter display %d ID [1: HDMI_ONCHIP, 2: HDMI_OFFCHIP, 3: NONE] : ", displayNum+1);
    VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
    value = atoi(inputStr);

    if(value==1)
        gChains_ctrl.displayId[displayNum] = CHAINS_DISPLAY_ID_HDMI_ONCHIP;
    if(value==2)
        gChains_ctrl.displayId[displayNum] = CHAINS_DISPLAY_ID_HDMI_OFFCHIP;
    if(value==3)
        gChains_ctrl.displayId[displayNum] = CHAINS_DISPLAY_ID_NONE;

}

char gChains_menuSettings0[] = {
    "\r\n ==============="
    "\r\n System Settings"
    "\r\n ==============="
    "\r\n"
};

char gChains_menuSettings1[] = {
    "\r\n"
    "\r\n 1: Number of video ports "
    "\r\n 2: Number of Displays"
    "\r\n 3: Display 1 Select "
    "\r\n 4: Display 2 Select"
    "\r\n 5: DEI Select"
    "\r\n 6: DEI Output to Display Select"
    "\r\n 7: NSF Bypass Mode"
    "\r\n 8: Execution time"
    "\r\n 9: Loop Count"
    "\r\n g: Enable Grpx"
    "\r\n d: Display Resolution"
    "\r\n s: SD Display Resolution"
    "\r\n t: Enable Tiler"
    "\r\n n: Number of Sub Chains"
    "\r\n a: Enable Auto Run"
    "\r\n "
    "\r\n x: Exit "
    "\r\n "
};

Void Chains_menuSettingsShow()
{
    Vps_printf(gChains_menuSettings0);

    Chains_menuCurrentSettingsShow();

    Vps_printf(gChains_menuSettings1);
}

Void Chains_menuSettings()
{
    char ch;
    Bool done = FALSE;
    char inputStr[10];
    Int32 value;

    Chains_menuSettingsShow();

    while(!done)
    {
        Vps_printf("\r\n Enter Choice: ");

        VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
        Vps_printf(" \r\n");

        switch(ch)
        {
            case '1':
                Vps_printf(" \r\n Enter number of video ports [1..4] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if(value>=1&&value<=4)
                    gChains_ctrl.numVipInst = value;

                break;

            case '2':
                Vps_printf(" \r\n Enter number of displays [1 or 2] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value >= 1 && value <= 2)
                    gChains_ctrl.numDisplays = value;
                break;

            case '3':
                Chains_menuSettingsDisplaySelect(0);
                break;

            case '4':
                Chains_menuSettingsDisplaySelect(1);
                break;

            case '5':
                Vps_printf(" \r\n Enter DEI ID [1: DEI HQ, 2: DEI] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if(value==1)
                    gChains_ctrl.deiId = CHAINS_DEI_ID_HQ;
                if(value==2)
                    gChains_ctrl.deiId = CHAINS_DEI_ID;
                break;

            case '6':
                Vps_printf(" \r\n Enter DEI Output to Display [1: DEI-SC, 2: VIP-SC] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if(value==1)
                    gChains_ctrl.deiOutDisplayId = DEI_LINK_OUT_QUE_DEI_SC;
                if(value==2)
                    gChains_ctrl.deiOutDisplayId = DEI_LINK_OUT_QUE_VIP_SC;
                break;

            case '7':
                Vps_printf(" \r\n Enter NSF Mode [1: CHR DS ONLY, 2: SNF + TNF] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if(value==1)
                    gChains_ctrl.bypassNsf = TRUE;
                if(value==2)
                    gChains_ctrl.bypassNsf = FALSE;

                break;

            case '8':
                Vps_printf(" \r\n Enter execution time in secs [0..10000000] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if(value>=0 && value<=10000000)
                    gChains_ctrl.exeTimeInSecs = value;

                break;

            case '9':
                Vps_printf(" \r\n Enter loop count [1..10000] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if(value>=1 && value<=10000)
                    gChains_ctrl.loopCount = value;
                break;

            case 'g':
                Vps_printf(" \r\n GRPX Enable [0: Disable, 1: Enable] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                gChains_ctrl.grpxEnable = FALSE;
                if (value == 1)
                {
                    gChains_ctrl.grpxEnable = TRUE;
                }
                break;

            case 'd':
                Vps_printf(" \r\n Display Resolution [1: 1080P30, 2: 1080I60, 3:1080P60, 4:720P60] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 1)
                    gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080P30;
                if (value == 2)
                    gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080I60;
                if (value == 3)
                    gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080P60;
                if (value == 4)
                    gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_720P60;
                break;

            case 's':
                Vps_printf(" \r\n SD Display Resolution [1: NTSC, 2: PAL] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 1)
                    gChains_ctrl.sdDisplayRes = SYSTEM_DISPLAY_RES_NTSC;
                if (value == 2)
                    gChains_ctrl.sdDisplayRes = SYSTEM_DISPLAY_RES_PAL;
                break;

            case 't':
                #ifndef TI_814X_BUILD
                Vps_printf(" \r\n Tiler Enable [0: Disable, 1: Enable] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 0)
                    gChains_ctrl.tilerEnable = FALSE;
                if (value == 1)
                    gChains_ctrl.tilerEnable = TRUE;
                #else
                Vps_printf(" \r\n Tiler memroy not supported by default. Update config_ti814x_largeHeap.bld to include tiler and enable in system_main.c : ");
                #endif /* TI_814X_BUILD */
                break;

            case 'n':
                Vps_printf(" \r\n Enter number of sub chains [1 or 2] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value >= 1 && value <= 2)
                    gChains_ctrl.numSubChains = value;
                break;

            case 'a':
                Vps_printf(" \r\n Auto Run Enable [0: Disable, 1: Enable] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 0)
                    gChains_ctrl.autoRunEnable = FALSE;
                if (value == 1)
                    gChains_ctrl.autoRunEnable = TRUE;
                break;

            case 'x':
                done = TRUE;
                break;
        }
        fflush(stdin);
    }
}

Int32 main ( void )
{
    System_start ( Chains_main );
    BIOS_start (  );

    return (0);
}
