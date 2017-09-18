/** ==================================================================
 *  @file   system_dctrl_common.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/system/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>
#include <ti/psp/devices/vps_sii9022a.h>
#include <ti/psp/examples/common/iss/chains/src/chains.h>

/* ===================================================================
 *  @func     System_displayCtrlDeInit                                               
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
Int32 System_displayCtrlDeInit()
{
    Int32 retVal;

#ifdef SYSTEM_USE_OFF_CHIP_HDMI
    Vps_PlatformBoardId boardId;
#endif

#ifdef SYSTEM_USE_OFF_CHIP_HDMI
    boardId = Vps_platformGetBoardId();

    if (VPS_PLATFORM_BOARD_VS == boardId || VPS_PLATFORM_BOARD_VC == boardId ||
        VPS_PLATFORM_BOARD_CATALOG == boardId)
    {
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        if (VPS_PLATFORM_BOARD_CATALOG != boardId)
        {
            System_hdmiStop();
        }
#else
        System_hdmiStop();
#endif                                                     /* TI_814X_BUILD */
    }
#endif

    /* Remove and close display controller configuration */
    retVal = FVID2_control(gSystem_obj.fvidDisplayCtrl,
                           IOCTL_VPS_DCTRL_CLEAR_CONFIG,
                           &gSystem_obj.displayCtrlCfg, NULL);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    retVal = FVID2_delete(gSystem_obj.fvidDisplayCtrl, NULL);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    System_displayUnderflowPrint(FALSE, TRUE);

    return retVal;
}

/**
 *  App_dispConfigHdmi
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders and VENCs.
 */
/* ===================================================================
 *  @func     System_hdmiConfig                                               
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
Int32 System_hdmiConfig(UInt32 displayRes)
{
    Int32 retVal;

    Vps_HdmiChipId hdmiId;

    Vps_SiI9022aHpdPrms hpdPrms;

    Vps_SiI9022aModeParams modePrms;

    retVal = FVID2_control(gSystem_obj.hdmiHandle,
                           IOCTL_VPS_SII9022A_GET_DETAILED_CHIP_ID,
                           &hdmiId, NULL);

    if (FVID2_SOK == retVal)
    {
#if 0                                                      // def
                                                           // SYSTEM_DEBUG_DISPLAY
        Vps_printf("DevId %x Prod RevId %x TPI RevId %x HDCP RevId %x\n",
                   hdmiId.deviceId,
                   hdmiId.deviceProdRevId, hdmiId.tpiRevId, hdmiId.hdcpRevTpi);
#endif
    }
    else
    {
        Vps_printf(" HDMI: ERROR: Could not Get Detailed Chip Id !!!\n");
    }

    retVal = FVID2_control(gSystem_obj.hdmiHandle,
                           IOCTL_VPS_SII9022A_QUERY_HPD, &hpdPrms, NULL);

    if (FVID2_SOK == retVal)
    {
#if 0                                                      // def
                                                           // SYSTEM_DEBUG_DISPLAY
        Vps_printf("hpdEvtPending %x busError %x hpdStatus %x\n",
                   hpdPrms.hpdEvtPending, hpdPrms.busError, hpdPrms.hpdStatus);
#endif
    }
    else
    {
        Vps_printf("Could not Get HPD\n");
    }

    modePrms.standard = FVID2_STD_1080P_30;
    if (displayRes == SYSTEM_DISPLAY_RES_1080I60)
        modePrms.standard = FVID2_STD_1080I_60;
    if (displayRes == SYSTEM_DISPLAY_RES_1080P60)
        modePrms.standard = FVID2_STD_1080P_60;
    if (displayRes == SYSTEM_DISPLAY_RES_720P60)
        modePrms.standard = FVID2_STD_720P_60;

    retVal = FVID2_control(gSystem_obj.hdmiHandle,
                           IOCTL_VPS_VIDEO_ENCODER_SET_MODE, &modePrms, NULL);

    if (FVID2_SOK == retVal)
    {
#ifdef SYSTEM_DEBUG_DISPLAY
        Vps_printf(" %d: HDMI: HDMI Config ... DONE !!!\n", Clock_getTicks());
#endif
    }
    else
    {
#ifdef SYSTEM_DEBUG_DISPLAY
        Vps_printf(" %d: HDMI: HDMI Config ... ERROR !!!\n", Clock_getTicks());
#endif
    }

    return (retVal);
}

/* ===================================================================
 *  @func     System_hdmiCreate                                               
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
Int32 System_hdmiCreate(UInt32 displayRes, Vps_PlatformBoardId boardId)
{
    Vps_VideoEncoderCreateParams encCreateParams;

    Vps_VideoEncoderCreateStatus encCreateStatus;

    Int32 retVal;

    /* Open HDMI Tx */
    encCreateParams.deviceI2cInstId = Vps_platformGetI2cInstId();
    encCreateParams.deviceI2cAddr =
        Vps_platformGetVidEncI2cAddr(FVID2_VPS_VID_ENC_SII9022A_DRV);
    encCreateParams.inpClk = 0;
    encCreateParams.hdmiHotPlugGpioIntrLine = 0;

    if (VPS_PLATFORM_BOARD_VS == boardId)
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EMBEDDED_SYNC;
        encCreateParams.clkEdge = FALSE;
    }
    else if (VPS_PLATFORM_BOARD_VC == boardId)
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EXTERNAL_SYNC;
        encCreateParams.clkEdge = TRUE;
    }
    else if (VPS_PLATFORM_BOARD_CATALOG == boardId)
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EXTERNAL_SYNC;
        encCreateParams.clkEdge = TRUE;
    }
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    encCreateParams.clkEdge = FALSE;
#endif                                                     /* TI_814X_BUILD */
    gSystem_obj.hdmiHandle = FVID2_create(FVID2_VPS_VID_ENC_SII9022A_DRV,
                                          0,
                                          &encCreateParams,
                                          &encCreateStatus, NULL);

    if (NULL == gSystem_obj.hdmiHandle)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, 0, __LINE__);
        return FVID2_EFAIL;
    }

    retVal = System_hdmiConfig(displayRes);
    GT_assert(GT_DEFAULT_MASK, retVal == FVID2_SOK);

    return retVal;
}

/* ===================================================================
 *  @func     System_hdmiStart                                               
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
Int32 System_hdmiStart(UInt32 displayRes, Vps_PlatformBoardId boardId)
{
    Int32 retVal;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: HDMI: Starting HDMI Transmitter ... !!!\n",
               Clock_getTicks());
#endif

    System_hdmiCreate(displayRes, boardId);

    retVal = FVID2_start(gSystem_obj.hdmiHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: HDMI: Starting HDMI Transmitter ... DONE !!!\n",
               Clock_getTicks());
#endif

    return retVal;
}

/* ===================================================================
 *  @func     System_hdmiStop                                               
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
Int32 System_hdmiStop()
{
    Int32 retVal;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: HDMI: Stopping HDMI Transmitter ... !!!\n",
               Clock_getTicks());
#endif

    retVal = FVID2_stop(gSystem_obj.hdmiHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    System_hdmiDelete();

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: HDMI: Stopping HDMI Transmitter ... DONE !!!\n",
               Clock_getTicks());
#endif

    return retVal;
}

/* ===================================================================
 *  @func     System_hdmiDelete                                               
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
Int32 System_hdmiDelete()
{
    Int32 retVal;

    retVal = FVID2_delete(gSystem_obj.hdmiHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return retVal;
    }

    return retVal;
}

/* ===================================================================
 *  @func     System_displayUnderflowPrint                                               
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
Int32 System_displayUnderflowPrint(Bool runTimePrint, Bool clearAll)
{
    static char printStr[256];

    sprintf(printStr,
            " %d: DISPLAY: UNDERFLOW COUNT: HDMI(BP0) %d, HDDAC(BP0) %d, DVO2(BP1) %d, SDDAC(SEC1) %d \n",
            Clock_getTicks(), gSystem_obj.displayUnderflowCount[0],
            gSystem_obj.displayUnderflowCount[1],
            gSystem_obj.displayUnderflowCount[2],
            gSystem_obj.displayUnderflowCount[3]);

    if (runTimePrint)
    {
        Vps_rprintf(printStr);
    }
    else
    {
        Vps_printf(printStr);
    }

    if (clearAll)
    {
        System_displayUnderflowCheck(TRUE);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VpsHal_vpsClkcModuleCountUnderFlow                                               
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
Int32 VpsHal_vpsClkcModuleCountUnderFlow(Bool clearAll,
                                         UInt32 * vencUnderflowCounter);

/* ===================================================================
 *  @func     System_displayUnderflowCheck                                               
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
Int32 System_displayUnderflowCheck(Bool clearAll)
{
    if (clearAll)
    {
        gSystem_obj.displayUnderflowCount[0] = 0;
        gSystem_obj.displayUnderflowCount[1] = 0;
        gSystem_obj.displayUnderflowCount[2] = 0;
        gSystem_obj.displayUnderflowCount[3] = 0;
    }
    VpsHal_vpsClkcModuleCountUnderFlow(clearAll,
                                       gSystem_obj.displayUnderflowCount);

    return FVID2_SOK;
}

/**
 *  System_dispSetPixClk
 *  Configure Pixel Clock.
 */
/* ===================================================================
 *  @func     System_dispSetPixClk                                               
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
Int32 System_dispSetPixClk()
{
    Int32 retVal = FVID2_SOK;

    Vps_PlatformCpuRev cpuRev;

    Vps_PlatformId platformId;

    /* Get platform type, board and CPU revisions */
    platformId = Vps_platformGetId();
    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf("%s: Error Unrecognized platform @ line %d\n",
                   __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }

    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        Vps_printf("%s: Error Unrecognized CPU version @ line %d\n",
                   __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }

    gSystem_obj.systemDrvHandle = FVID2_create(FVID2_VPS_VID_SYSTEM_DRV,
                                               0, NULL, NULL, NULL);
    if (NULL == gSystem_obj.systemDrvHandle)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (FVID2_EFAIL);
    }

    /* Multiply the freq by 2 if its TI816x PG1.0 */
    if ((platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
        (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
    {
        gSystem_obj.vpllCfg.outputClk = gSystem_obj.vpllCfg.outputClk * 2;
    }

    gSystem_obj.vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;
    retVal = FVID2_control(gSystem_obj.systemDrvHandle,
                           IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                           &gSystem_obj.vpllCfg, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

#ifdef TI_816X_BUILD
    gSystem_obj.vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;

    if ((platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
        (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
    {
        gSystem_obj.vpllCfg.outputClk = gSystem_obj.vpllCfg.outputClk / 2;
    }
    retVal = FVID2_control(gSystem_obj.systemDrvHandle,
                           IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                           &gSystem_obj.vpllCfg, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
#endif

    FVID2_delete(gSystem_obj.systemDrvHandle, NULL);

    return (FVID2_SOK);
}
