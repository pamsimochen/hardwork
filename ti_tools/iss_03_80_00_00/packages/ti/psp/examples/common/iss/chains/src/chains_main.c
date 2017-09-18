/** ==================================================================
 *  @file   chains_main.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "chains.h"

Chains_Ctrl gChains_ctrl;

Int32 aewbVendor = 0;

/* ===================================================================
 *  @func     Chains_setDefaultCfg                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_setDefaultCfg()
{
    if (Iss_platformIsEvm())
    {
        gChains_ctrl.exeTimeInSecs = (Uint32) 60;
    }
    else
    {
        gChains_ctrl.exeTimeInSecs = 1;
    }
    gChains_ctrl.loopCount = 1;

    gChains_ctrl.numVipInst = 4u;
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    gChains_ctrl.numVipInst = 1u;
#endif

    gChains_ctrl.numSubChains = 1u;
    gChains_ctrl.numDisplays = 1u;
    gChains_ctrl.displayId[0] = CHAINS_DISPLAY_ID_HDMI_ONCHIP;
    gChains_ctrl.displayId[1] = CHAINS_DISPLAY_ID_HDMI_OFFCHIP;
#ifdef TI_816X_BUILD
    gChains_ctrl.deiId = CHAINS_DEI_ID_HQ;
#endif                                                     /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    gChains_ctrl.deiId = NULL;
#endif                                                     /* TI_814X_BUILD */
    gChains_ctrl.deiOutDisplayId = NULL;
    gChains_ctrl.enableNsfLink = FALSE;
    gChains_ctrl.bypassNsf = FALSE;
    gChains_ctrl.grpxEnable = FALSE;
    gChains_ctrl.cpuRev = ISS_PLATFORM_CPU_REV_1_0;
    gChains_ctrl.boardId = ISS_PLATFORM_BOARD_VCAM;
    gChains_ctrl.displayRes = SYSTEM_DISPLAY_RES_1080P60;
    gChains_ctrl.sdDisplayRes = SYSTEM_DISPLAY_RES_NTSC;
    gChains_ctrl.tilerEnable = FALSE;
    gChains_ctrl.comprEnable = FALSE;

#if 0
    VpsUtils_setL3Pri(0, 0x00F0);
#endif
}

/* ===================================================================
 *  @func     Chains_initLinks                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_initLinks()
{
    Vps_printf(" %d: CHAINS  : Initializing Links !!! \r\n", Clock_getTicks());
    System_memPrintHeapStatus();

    CameraLink_init();
    DisplayLink_init();
    NullLink_init();
    DispHwMsLink_init();

    Vps_printf(" %d: CHAINS  : Initializing Links ... DONE !!! \r\n",
               Clock_getTicks());

    VpsUtils_remoteSendChar('s');
}

/* ===================================================================
 *  @func     Chains_deInitLinks                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_deInitLinks()
{
    Vps_printf(" %d: CHAINS  : De-Initializing Links !!! \r\n",
               Clock_getTicks());

    NullLink_deInit();
    DisplayLink_deInit();
    CameraLink_deInit();
    DispHwMsLink_deInit();

    System_memPrintHeapStatus();

    Vps_printf(" %d: CHAINS  : De-Initializing Links ... DONE !!! \r\n",
               Clock_getTicks());

    VpsUtils_remoteSendChar('e');
}

char *gChains_cpuName[ISS_PLATFORM_CPU_REV_MAX] = {
    "ES1.0",
    "ES1.1",
    "ES2.0",
    "ES2.1",
    "UNKNOWN",
};

char *gChains_boardName[ISS_PLATFORM_BOARD_MAX] = {
    "UNKNOWN",
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    "2x MT9003,  VCAM",
#else
    "2x SII9135, 1x TVP7002 VC",
    "2x SIL1161A, 2x TVP7002 Catalog"
#endif                                                     /* TI_814X_BUILD */
};

char *gChains_boardRev[ISS_PLATFORM_BOARD_REV_MAX] = {
    "UNKNOWN",
    "REV A",
    "REV B",
    "REV C",
};

/* ===================================================================
 *  @func     Chains_detectBoard                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Chains_detectBoard()
{
    Iss_PlatformBoardRev boardRev;

    /* Get CPU version */
    gChains_ctrl.cpuRev = Iss_platformGetCpuRev();
    if (gChains_ctrl.cpuRev >= ISS_PLATFORM_CPU_REV_MAX)
    {
        gChains_ctrl.cpuRev = ISS_PLATFORM_CPU_REV_UNKNOWN;
    }
    Vps_printf(" %d: CHAINS  : CPU Revision [%s] !!! \r\n",
               Clock_getTicks(), gChains_cpuName[gChains_ctrl.cpuRev]);

    /* Detect board */
    gChains_ctrl.boardId = Iss_platformGetBoardId();
    if (gChains_ctrl.boardId >= ISS_PLATFORM_BOARD_MAX)
    {
        gChains_ctrl.boardId = ISS_PLATFORM_BOARD_UNKNOWN;
    }
    Vps_printf(" %d: CHAINS  : Detected [%s] Board !!! \r\n",
               Clock_getTicks(), gChains_boardName[gChains_ctrl.boardId]);

    /* Get base board revision */
    boardRev = Iss_platformGetBaseBoardRev();
    if (boardRev >= ISS_PLATFORM_BOARD_REV_MAX)
    {
        boardRev = ISS_PLATFORM_BOARD_REV_UNKNOWN;
    }
    Vps_printf(" %d: CHAINS  : Base Board Revision [%s] !!! \r\n",
               Clock_getTicks(), gChains_boardRev[boardRev]);

    if (gChains_ctrl.boardId != ISS_PLATFORM_BOARD_UNKNOWN)
    {
        /* Get daughter card revision */
        boardRev = Iss_platformGetDcBoardRev();
        if (boardRev >= ISS_PLATFORM_BOARD_REV_MAX)
        {
            boardRev = ISS_PLATFORM_BOARD_REV_UNKNOWN;
        }
        Vps_printf(" %d: CHAINS  : Daughter Card Revision [%s] !!! \r\n",
                   Clock_getTicks(), gChains_boardRev[boardRev]);
    }

    return 0;
}

/* ===================================================================
 *  @func     Chains_checkBoard                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Chains_checkBoard(Iss_PlatformBoardId reqdBoardId)
{
    if (reqdBoardId >= ISS_PLATFORM_BOARD_MAX)
        reqdBoardId = ISS_PLATFORM_BOARD_UNKNOWN;

    if (gChains_ctrl.boardId != reqdBoardId)
    {
        Vps_printf(" %d: CHAINS  : This application needs [%s] Board  !!! \r\n",
                   Clock_getTicks(), gChains_boardName[reqdBoardId]);
        Vps_printf(" %d: CHAINS  : Currently detected [%s] Board  !!! \r\n",
                   Clock_getTicks(), gChains_boardName[gChains_ctrl.boardId]);
        Vps_printf
            (" %d: CHAINS  : Incompatible boards .. CANNOT RUN APPLICATION !!! \r\n",
             Clock_getTicks());

        return FVID2_EFAIL;
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     Chains_run                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_run(Chains_RunFunc chainsRunFunc, Iss_PlatformBoardId reqdBoardId)
{
    UInt32 loopCount;

    Int32 status;

    Chains_Ctrl chainsCtrl;

    if (Iss_platformIsEvm())
    {
        status = Chains_checkBoard(reqdBoardId);
        if (status != FVID2_SOK)
            return;
    }

#ifndef SYSTEM_USE_TILER
    if (gChains_ctrl.tilerEnable)
    {
        Vps_printf
            (" %d: SYSTEM: !!! WARNING: Tiler is DISABLED via compile option, so forcing tiler enable configuration to FALSE\n !!!",
             Clock_getTicks());
        gChains_ctrl.tilerEnable = FALSE;
    }
#endif

    memcpy(&chainsCtrl, &gChains_ctrl, sizeof(gChains_ctrl));

    for (loopCount = 0; loopCount < chainsCtrl.loopCount; loopCount++)
    {
#ifdef SYSTEM_DEBUG
        Vps_printf(" %d: CHAINS  : Starting %d of %d !!! \r\n",
                   Clock_getTicks(), loopCount + 1, chainsCtrl.loopCount);
#endif
        System_memPrintHeapStatus();

        GT_assert(GT_DEFAULT_MASK, chainsRunFunc != NULL);
        chainsRunFunc(&chainsCtrl);

        System_memPrintHeapStatus();
#ifdef SYSTEM_DEBUG
        Vps_printf(" %d: CHAINS  : Stopped %d of %d !!! \r\n",
                   Clock_getTicks(), loopCount + 1, chainsCtrl.loopCount);
#endif
    }

    VpsUtils_remoteSendChar('x');
}

char gChains_menuMain0[] = {
    "\r\n ============" "\r\n Chain Select" "\r\n ============" "\r\n"
};

char gChains_menuMainVcam[] = {
    "\r\n"
        "\r\n Connect a 1080p60 HDMI RGB source to VIP1, this will be input for Camera"
        "\r\n"
        "\r\n 1: Single CH Capture + Display                     (1x Camera , 1080P60, YUV422 )"
        "\r\n "
        "\r\n s: System Settings "
        "\r\n " "\r\n x: Exit " "\r\n " "\r\n Enter Choice: "
};

/* ===================================================================
 *  @func     Chains_menuMainShow                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_menuMainShow(Iss_PlatformBoardId reqdBoardId)
{
    Vps_printf(gChains_menuMain0);

#ifdef PLATFORM_ZEBU
    Chains_menuCurrentSettingsShow();
#endif

    if (reqdBoardId == ISS_PLATFORM_BOARD_VCAM)
        Vps_printf(gChains_menuMainVcam);
    else
        Vps_printf("\n [UNKNOWN BOARD] \n\n");

}

/* ===================================================================
 *  @func     Chains_menuMainRunVcam                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_menuMainRunVcam(char ch)
{
    gChains_ctrl.cameraSingleChOutYuv422SP = FALSE;
    gChains_ctrl.cameraSingleChOutYuv420 = FALSE;
    gChains_ctrl.enableDisSyncMode = FALSE;

    switch (ch)
    {
        case '1':
            Chains_run(Chains_CameraMt9j003, ISS_PLATFORM_BOARD_VCAM);
            break;
    }
}

/* ===================================================================
 *  @func     Chains_main                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_main(UArg arg0, UArg arg1)
{
    char ch;

    Bool done;

    done = FALSE;

    Chains_initLinks();

    Chains_setDefaultCfg();

    while (!done)
    {
        Chains_menuMainShow(gChains_ctrl.boardId);

        VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
        Vps_printf(" \r\n");

        if (gChains_ctrl.boardId == ISS_PLATFORM_BOARD_VCAM)
            Chains_menuMainRunVcam(ch);
        else
            Vps_printf("\n [UNKNOWN BOARD] \n\n");

        switch (ch)
        {
            case 's':
                Chains_menuSettings();
                break;
            case 'x':
                done = TRUE;
                break;
        }

    }

    Chains_deInitLinks();
}

/* ===================================================================
 *  @func     Chains_menuCurrentSettingsShow                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_menuCurrentSettingsShow()
{
    static char *displayName[] = { "HDMI_ONCHIP", "HDMI_OFFCHIP", "NONE" };

    static char *deiName[] = { "DEI-HQ", "DEI" };

    static char *deiOutName[] = { "DEI-SC", "VIP-SC" };

    static char *nsfModeName[] = { "SNF + TNF", "CHR DS ONLY" };

    static char *enableDisableName[] = { "OFF", "ON" };

    static char *displayResName[] =
        { "1080P30", "1080I60", "1080P60", "720P60" };

    static char *sdDisplayResName[] = { "NTSC", "PAL" };

    Vps_printf("\r\n Current System Settings,");
    Vps_printf("\r\n Num Video Ports       : %d", gChains_ctrl.numVipInst);
    Vps_printf("\r\n Number of Displays    : %d", gChains_ctrl.numDisplays);
    Vps_printf("\r\n Display 1             : %s",
               displayName[gChains_ctrl.displayId[0]]);
    Vps_printf("\r\n Display 2             : %s",
               displayName[gChains_ctrl.displayId[1]]);
    Vps_printf("\r\n DEI                   : %s", deiName[gChains_ctrl.deiId]);
    Vps_printf("\r\n DEI Output to Display : %s",
               deiOutName[gChains_ctrl.deiOutDisplayId]);
    Vps_printf("\r\n NSF Mode              : %s",
               nsfModeName[gChains_ctrl.bypassNsf]);
    Vps_printf("\r\n Execution Time        : %d secs",
               gChains_ctrl.exeTimeInSecs);
    Vps_printf("\r\n Loop Count            : %d", gChains_ctrl.loopCount);
    Vps_printf("\r\n GRPX                  : %s",
               enableDisableName[gChains_ctrl.grpxEnable]);
    Vps_printf("\r\n Display Resolution    : %s",
               displayResName[gChains_ctrl.displayRes]);
    Vps_printf("\r\n Display Resolution    : %s",
               sdDisplayResName[gChains_ctrl.sdDisplayRes -
                                SYSTEM_DISPLAY_RES_NTSC]);
    Vps_printf("\r\n Tiler                 : %s",
               enableDisableName[gChains_ctrl.tilerEnable]);
    Vps_printf("\r\n DEI Compression       : %s",
               enableDisableName[gChains_ctrl.comprEnable]);
    Vps_printf("\r\n Number of Sub Chains  : %d", gChains_ctrl.numSubChains);
}

/* ===================================================================
 *  @func     Chains_menuSettingsDisplaySelect                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_menuSettingsDisplaySelect(UInt32 displayNum)
{
    char inputStr[10];

    Int32 value;

    Vps_printf
        (" \r\n Enter display %d ID [1: HDMI_ONCHIP, 2: HDMI_OFFCHIP, 3: NONE] : ",
         displayNum + 1);
    VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
    value = atoi(inputStr);

    if (value == 1)
        gChains_ctrl.displayId[displayNum] = CHAINS_DISPLAY_ID_HDMI_ONCHIP;
    if (value == 2)
        gChains_ctrl.displayId[displayNum] = CHAINS_DISPLAY_ID_HDMI_OFFCHIP;
    if (value == 3)
        gChains_ctrl.displayId[displayNum] = CHAINS_DISPLAY_ID_NONE;

}

char gChains_menuSettings0[] = {
    "\r\n ===============" "\r\n System Settings" "\r\n ===============" "\r\n"
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
        "\r\n c: Enable DEI Compression"
        "\r\n n: Number of Sub Chains" "\r\n " "\r\n x: Exit " "\r\n "
};

/* ===================================================================
 *  @func     Chains_menuSettingsShow                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_menuSettingsShow()
{
    Vps_printf(gChains_menuSettings0);

    Chains_menuCurrentSettingsShow();

    Vps_printf(gChains_menuSettings1);
}

/* ===================================================================
 *  @func     Chains_menuSettings                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void Chains_menuSettings()
{
    char ch;

    Bool done = FALSE;

    char inputStr[10];

    Int32 value;

    Chains_menuSettingsShow();

    while (!done)
    {
        Vps_printf("\r\n Enter Choice: ");

        VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
        Vps_printf(" \r\n");

        switch (ch)
        {
            case '1':
                Vps_printf(" \r\n Enter number of video ports [1..4] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value >= 1 && value <= 4)
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

                if (value == 1)
                    gChains_ctrl.deiId = CHAINS_DEI_ID_HQ;
                if (value == 2)
                    gChains_ctrl.deiId = CHAINS_DEI_ID;
                break;

            case '6':
                Vps_printf
                    (" \r\n Enter DEI Output to Display [1: DEI-SC, 2: VIP-SC] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 1)
                    gChains_ctrl.deiOutDisplayId = DEI_LINK_OUT_QUE_DEI_SC;
                if (value == 2)
                    gChains_ctrl.deiOutDisplayId = DEI_LINK_OUT_QUE_VIP_SC;
                break;

            case '7':
                Vps_printf
                    (" \r\n Enter NSF Mode [1: CHR DS ONLY, 2: SNF + TNF] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 1)
                    gChains_ctrl.bypassNsf = TRUE;
                if (value == 2)
                    gChains_ctrl.bypassNsf = FALSE;

                break;

            case '8':
                Vps_printf
                    (" \r\n Enter execution time in secs [0..10000000] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value >= 0 && value <= 10000000)
                    gChains_ctrl.exeTimeInSecs = value;

                break;

            case '9':
                Vps_printf(" \r\n Enter loop count [1..10000] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value >= 1 && value <= 10000)
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
                Vps_printf
                    (" \r\n Display Resolution [1: 1080P30, 2: 1080I60, 3:1080P60, 4:720P60] : ");
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
#if !defined(TI_814X_BUILD) && !defined(TI_8107_BUILD)
                Vps_printf(" \r\n Tiler Enable [0: Disable, 1: Enable] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 0)
                    gChains_ctrl.tilerEnable = FALSE;
                if (value == 1)
                    gChains_ctrl.tilerEnable = TRUE;
#else
                Vps_printf
                    (" \r\n Tiler memroy not supported by default. Update config_ti814x_largeHeap.bld to include tiler and enable in system_main.c : ");
#endif                                                     /* TI_814X_BUILD */
                break;

            case 'c':
                Vps_printf
                    (" \r\n DEI Compression Enable [0: Disable, 1: Enable] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value == 0)
                    gChains_ctrl.comprEnable = FALSE;
                if (value == 1)
                    gChains_ctrl.comprEnable = TRUE;

                break;

            case 'n':
                Vps_printf(" \r\n Enter number of sub chains [1 or 2] : ");
                VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
                value = atoi(inputStr);

                if (value >= 1 && value <= 2)
                    gChains_ctrl.numSubChains = value;
                break;

            case 'x':
                done = TRUE;
                break;
        }
        fflush(stdin);
    }
}

/* ===================================================================
 *  @func     main                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 main(void)
{
    System_start(Chains_main);
    BIOS_start();

    return (0);
}
