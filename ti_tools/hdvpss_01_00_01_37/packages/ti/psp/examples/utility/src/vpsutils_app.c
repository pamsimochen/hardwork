/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsutils_app.c
 *
 *  \brief This file implements application level generic helper functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/devices/vps_sii9022a.h>
#include <ti/psp/devices/vps_thsfilters.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_app.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Display driver instance to be used.
 *  Redefined this as GRPX ID restarts from zero as they belong to a different
 *  driver.
 */
typedef enum
{
    VPSUTILS_APP_DISP_INST_BP0 = 0,
    VPSUTILS_APP_DISP_INST_BP1,
    VPSUTILS_APP_DISP_INST_SEC1,
    VPSUTILS_APP_DISP_INST_MAIN,
    VPSUTILS_APP_DISP_INST_AUX,
    VPSUTILS_APP_DISP_INST_GRPX0,
    VPSUTILS_APP_DISP_INST_GRPX1,
    VPSUTILS_APP_DISP_INST_GRPX2,
    VPSUTILS_APP_DISP_INST_MAX
} VpsUtils_AppDispInst;

#define VPSUTILS_CONFIG_OFFCHIP_HDMI

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure to store app objects.
 */
typedef struct VpsUtils_AppObj_t
{
    Bool                        initDone;
    /**< Initialization done flag. */
    Semaphore_Handle            lockSem;
    /**< Semaphore used to protect globals. */

    Vps_DcConfig                dcCfg;
    /**< DCTRL driver configuration. */
    FVID2_Handle                hdmiHandle;
    /**< HDMI transmitter handle. */

    Vps_PlatformId              platformId;
    /**< Platform identifier. */
    Vps_PlatformBoardId         boardId;
    /**< Platform identifier. */
    Vps_PlatformCpuRev          cpuRev;
    /**< CPU version identifier. */
} VpsUtils_AppObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 VpsUtils_appConfigDctrlPath(FVID2_Handle dcHandle,
                                         VpsUtils_AppDispInst instId,
                                         UInt32 vencId,
                                         Bool useHd1Path,
                                         Bool clear);
static Int32 VpsUtils_appCfgDctrlVencOutput(FVID2_Handle dcHandle,
                                            UInt32 vencId,
                                            UInt32 standard,
                                            Vps_DcAnalogFmt aFmt);
static Int32 VpsUtils_appStartOffChipHdmi(VpsUtils_AppObj *appObj,
                                          UInt32 standard);
static Int32 VpsUtils_appStopOffChipHdmi(VpsUtils_AppObj *appObj);

static VpsUtils_AppDispInst VpsUtils_appGetDispInst(UInt32 driverId,
                                                    UInt32 driverInstId);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief VPS Utils app object used for storing semaphore handle, flags etc. */
static VpsUtils_AppObj gVpsUtilsAppObj;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsUtils_appInit
 *  \brief VPS application utils init function.
 */
Int32 VpsUtils_appInit(void)
{
    Int32               retVal = FVID2_SOK;
    Semaphore_Params    semParams;
    VpsUtils_AppObj    *appObj = &gVpsUtilsAppObj;

    memset(appObj, 0u, sizeof(*appObj));

    /* Get platform type, board and CPU revisions */
    appObj->platformId = Vps_platformGetId();
    if ((appObj->platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (appObj->platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf("Error: Unrecognized platform!!\n");
        retVal = FVID2_EFAIL;
    }
    appObj->cpuRev = Vps_platformGetCpuRev();
    if (appObj->cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        Vps_printf("Error: Unrecognized CPU version!!\n");
        retVal = FVID2_EFAIL;
    }

    appObj->boardId = Vps_platformGetBoardId();
    if (appObj->boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("Error: Unrecognized Board ID!!\n");
        retVal = FVID2_EFAIL;
    }

    if (FVID2_SOK == retVal)
    {
        Semaphore_Params_init(&semParams);
        appObj->lockSem = Semaphore_create(1u, &semParams, NULL);
        if (NULL == appObj->lockSem)
        {
            Vps_printf("Error: Semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        appObj->initDone = TRUE;
    }

    return (retVal);
}

/**
 *  VpsUtils_appDeInit
 *  \brief VPS application utils deinit function.
 */
Int32 VpsUtils_appDeInit(void)
{
    Int32               retVal = FVID2_SOK;
    VpsUtils_AppObj    *appObj = &gVpsUtilsAppObj;

    if (NULL != appObj->hdmiHandle)
    {
        VpsUtils_appStopOffChipHdmi(appObj);
    }

    if (NULL != appObj->lockSem)
    {
        Semaphore_delete(&appObj->lockSem);
        appObj->lockSem = NULL;
    }

    appObj->initDone = FALSE;

    return (retVal);
}

/**
 *  VpsUtils_appSetVencPixClk
 *  \brief Configures display VENC pixel clock as per the provided standard
 *  by programming the video PLL.
 */
Int32 VpsUtils_appSetVencPixClk(UInt32 vencId, UInt32 standard)
{
    Int32               retVal = FVID2_SOK;
    FVID2_Handle        sysDrvHandle = NULL;
    FVID2_ModeInfo      modeInfo;
    Vps_SystemVPllClk   vpllCfg;
    VpsUtils_AppObj    *appObj = &gVpsUtilsAppObj;

    if (appObj->initDone != TRUE)
    {
        Vps_printf("Error: VpsUtils_appInit() is not called!!\n");
        retVal = FVID2_EFAIL;
        return (retVal);
    }

    Semaphore_pend(appObj->lockSem, BIOS_WAIT_FOREVER);

    /* Get the standard information */
    modeInfo.standard = standard;
    retVal = FVID2_getModeInfo(&modeInfo);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("Error: Invalid display mode!!\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Create system driver to set video PLL */
        sysDrvHandle = FVID2_create(
                           FVID2_VPS_VID_SYSTEM_DRV,
                           0u,
                           NULL,
                           NULL,
                           NULL);
        if (NULL == sysDrvHandle)
        {
            Vps_printf("Error: System driver create failed!!\n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (VPS_DC_VENC_SD == vencId)
        {
            vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_RF;
            if (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId)
            {
                vpllCfg.outputClk = 216000u;
            }
            else
            {
                vpllCfg.outputClk = 54000u;
            }
        }
        else if (VPS_DC_VENC_HDCOMP == vencId)
        {
            vpllCfg.outputClk = modeInfo.pixelClock;
            vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;
        }
        else if ((VPS_DC_VENC_HDMI == vencId) ||
                 (VPS_DC_VENC_DVO2 == vencId))
        {
            vpllCfg.outputClk = modeInfo.pixelClock;
            vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;

            /* Revision 1.0 of TI816x require twice the pixel clock, as the
             * divider is not selectable */
            if ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) &&
                (VPS_PLATFORM_CPU_REV_1_0 == appObj->cpuRev))
            {
                vpllCfg.outputClk *= 2u;
            }

            /* Clock Source is not selectable in TI814X. It is fixed to HDMI
             * clock for HDMI and VENCD clock for DVO2. These clocks can
             * only be divided by 2 within HDVPSS */
            if ((appObj->platformId == VPS_PLATFORM_ID_EVM_TI814x) ||
                (appObj->platformId == VPS_PLATFORM_ID_EVM_TI8107))
            {
                if (VPS_DC_VENC_DVO2 == vencId)
                {
                    vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;
                }
                else
                {
                    /* HDMI and DVO1 PLL are same. when HDMI  is used, output of
                     * HDMI is fed as pixel clock to venc, when HDMI is not used
                     * output of the PLL is fed as pixel clock to Venc. Since
                     * we are using HDMI, setting clock source as
                     * VPS_SYSTEM_VPLL_OUTPUT_HDMI, else we need to set
                     * clock source as VPS_SYSTEM_VPLL_OUTPUT_VENC_A
                     */
                    vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_HDMI;
                }
            }
        }
        else
        {
            Vps_printf("Error: Invalid VENC ID %d!!\n", vencId);
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        retVal = FVID2_control(
                     sysDrvHandle,
                     IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                     &vpllCfg,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Set video PLL failed!!\n");
        }
    }

    if (NULL != sysDrvHandle)
    {
        FVID2_delete(sysDrvHandle, NULL);
    }

    Semaphore_post(appObj->lockSem);

    return (retVal);
}

/**
 *  VpsUtils_appStartVenc
 *  \brief Configures display VENC for a particular mode and sets the VENC
 *  output format as per provided parameters.
 */
Int32 VpsUtils_appStartVenc(FVID2_Handle dcHandle,
                            UInt32 vencId,
                            UInt32 standard,
                            Vps_DcAnalogFmt aFmt)
{
    Int32                   retVal = FVID2_SOK;
    VpsUtils_AppObj        *appObj = &gVpsUtilsAppObj;
    Vps_DcConfig           *dcCfg = &appObj->dcCfg;
    FVID2_ModeInfo          modeInfo;
#ifndef PLATFORM_ZEBU
    Vps_Ths7360SfCtrl       thsCtrl;
#endif

    if (appObj->initDone != TRUE)
    {
        Vps_printf("Error: VpsUtils_appInit() is not called!!\n");
        retVal = FVID2_EFAIL;
        return (retVal);
    }

    Semaphore_pend(appObj->lockSem, BIOS_WAIT_FOREVER);

    /* Stop Off-chip HDMI if already started */
    if (NULL != appObj->hdmiHandle)
    {
        VpsUtils_appStopOffChipHdmi(appObj);
    }

#ifndef PLATFORM_ZEBU
    /* Configure THS filter for component output */
    if ((VPS_DC_VENC_HDCOMP == vencId) &&
        ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
         (VPS_PLATFORM_ID_EVM_TI8107 == appObj->platformId)))
    {
        /* Get the standard information */
        modeInfo.standard = standard;
        retVal = FVID2_getModeInfo(&modeInfo);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Invalid display mode!!\n");
        }
        else
        {
            thsCtrl = VPS_THS7360_SF_TRUE_HD_MODE;
            if (modeInfo.pixelClock <= 54000u)
            {
                thsCtrl = VPS_THS7360_SF_ED_MODE;
            }
            else if (modeInfo.pixelClock <= 74250u)
            {
                thsCtrl = VPS_THS7360_SF_HD_MODE;
            }

            retVal = Vps_ths7360SetSfParams(thsCtrl);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("Error: THS7360 set SF params failed!!\n");
            }
        }

        if (FVID2_SOK == retVal)
        {
            /* Set the Pin Mux to output analog sync signals */
            if (FVID2_isStandardVesa(standard) &&
                (appObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_0))
            {
                retVal = Vps_platformSelectHdCompSyncSource(
                             VPS_PLATFORM_HDCOMP_SYNC_SRC_DVO1,
                             TRUE);
                if (FVID2_SOK != retVal)
                {
                    Vps_printf("Error: Select HDCOMP sync source failed!!\n");
                }
            }
        }
    }

    /* Configure THS filter for SD output */
    if ((VPS_DC_VENC_SD == vencId) &&
        ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
         (VPS_PLATFORM_ID_EVM_TI8107 == appObj->platformId)))
    {
        /* Enable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_ENABLE_MODULE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: THS7360 set SD params failed!!\n");
        }
    }
#endif

    if (FVID2_SOK == retVal)
    {
        retVal = VpsUtils_appCfgDctrlVencOutput(
                     dcHandle,
                     vencId,
                     standard,
                     aFmt);
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the display controller VENC configuration */
        dcCfg->useCase = VPS_DC_USERSETTINGS;
        dcCfg->numEdges = 0u;
        dcCfg->vencInfo.modeInfo[0u].vencId = vencId;
        dcCfg->vencInfo.modeInfo[0u].mInfo.standard = standard;
        if (FVID2_STD_WSVGA_70 == standard)
        {
            dcCfg->vencInfo.modeInfo[0u].mInfo.standard = FVID2_STD_CUSTOM;
            dcCfg->vencInfo.modeInfo[0u].mInfo.width = 1024u;
            dcCfg->vencInfo.modeInfo[0u].mInfo.height = 600u;
            dcCfg->vencInfo.modeInfo[0u].mInfo.scanFormat =
                FVID2_SF_PROGRESSIVE;
            dcCfg->vencInfo.modeInfo[0u].mInfo.pixelClock = 50800;
            dcCfg->vencInfo.modeInfo[0u].mInfo.hFrontPorch = 36;
            dcCfg->vencInfo.modeInfo[0u].mInfo.hBackPorch = 60;
            dcCfg->vencInfo.modeInfo[0u].mInfo.hSyncLen = 30;
            dcCfg->vencInfo.modeInfo[0u].mInfo.vFrontPorch = 10;
            dcCfg->vencInfo.modeInfo[0u].mInfo.vBackPorch = 11;
            dcCfg->vencInfo.modeInfo[0u].mInfo.vSyncLen = 10;
            dcCfg->vencInfo.modeInfo[0u].mInfo.fps = 70;
            dcCfg->vencInfo.modeInfo[0u].mode = 1u;
        }

        dcCfg->vencInfo.tiedVencs = 0u;
        dcCfg->vencInfo.numVencs = 1u;
        retVal = FVID2_control(
                     dcHandle,
                     IOCTL_VPS_DCTRL_SET_CONFIG,
                     dcCfg,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: DCTRL set VENC config failed!!\n");
        }
    }

    if ((FVID2_SOK == retVal) &&
        (Vps_platformIsEvm()) &&
        (VPS_DC_VENC_DVO2 == vencId))
    {
        modeInfo.standard = standard;
        retVal = FVID2_getModeInfo(&modeInfo);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Invalid display mode!!\n");
        }
        else
        {
            /* After starting the VENC put one frame delay for VENC timing to
             * stabalize so that off-chip encoder can sync up */
            Task_sleep((2u * 1000u) / (modeInfo.fps));

            if (VPS_PLATFORM_BOARD_NETCAM != appObj->boardId)
            {
                /* Create and config Off-chip HDMI */
                retVal = VpsUtils_appStartOffChipHdmi(appObj, standard);
                if (FVID2_SOK != retVal)
                {
                    Vps_printf("Error: Off-Chip HDMI configuration failed\n");
                }
            }
        }
    }

    Semaphore_post(appObj->lockSem);

    return (retVal);
}

/**
 *  VpsUtils_appStopVenc
 *  \brief Stops the VENC.
 */
Int32 VpsUtils_appStopVenc(FVID2_Handle dcHandle,
                           UInt32 vencId,
                           UInt32 standard)
{
    Int32                   retVal = FVID2_SOK;
    VpsUtils_AppObj        *appObj = &gVpsUtilsAppObj;
    Vps_DcConfig           *dcCfg = &appObj->dcCfg;
#ifndef PLATFORM_ZEBU
    Vps_Ths7360SfCtrl       thsCtrl;
#endif

    if (appObj->initDone != TRUE)
    {
        Vps_printf("Error: VpsUtils_appInit() is not called!!\n");
        retVal = FVID2_EFAIL;
        return (retVal);
    }

    Semaphore_pend(appObj->lockSem, BIOS_WAIT_FOREVER);

    /* Stop Off-chip HDMI if already started */
    if (NULL != appObj->hdmiHandle)
    {
        VpsUtils_appStopOffChipHdmi(appObj);
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the display controller VENC configuration */
        dcCfg->useCase = VPS_DC_USERSETTINGS;
        dcCfg->numEdges = 0u;
        dcCfg->vencInfo.modeInfo[0u].vencId = vencId;
        dcCfg->vencInfo.modeInfo[0u].mInfo.standard = standard;
        if (FVID2_STD_WSVGA_70 == standard)
        {
            dcCfg->vencInfo.modeInfo[0u].mInfo.standard = FVID2_STD_CUSTOM;
        }
        dcCfg->vencInfo.tiedVencs = 0u;
        dcCfg->vencInfo.numVencs = 1u;
        retVal = FVID2_control(
                     dcHandle,
                     IOCTL_VPS_DCTRL_CLEAR_CONFIG,
                     dcCfg,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: DCTRL clear VENC config failed!!\n");
        }
    }

#ifndef PLATFORM_ZEBU
    /* Configure THS filter for component output */
    if ((VPS_DC_VENC_HDCOMP == vencId) &&
        ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
         (VPS_PLATFORM_ID_EVM_TI8107 == appObj->platformId)))
    {
        /* Remove the Pin Mux for output analog sync signals */
        if (FVID2_isStandardVesa(standard) &&
            (appObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_0))
        {
            retVal = Vps_platformSelectHdCompSyncSource(
                         VPS_PLATFORM_HDCOMP_SYNC_SRC_DVO1,
                         FALSE);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("Error: De-select HDCOMP sync source failed!!\n");
            }
        }

        thsCtrl = VPS_THS7360_DISABLE_SF;
        retVal = Vps_ths7360SetSfParams(thsCtrl);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: THS7360 set SF params failed!!\n");
        }
    }

    /* Configure THS filter for SD output */
    if ((VPS_DC_VENC_SD == vencId) &&
        ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
         (VPS_PLATFORM_ID_EVM_TI8107 == appObj->platformId)))
    {
        /* Disable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_DISABLE_MODULE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: THS7360 set SD params failed!!\n");
        }
    }
#endif

    Semaphore_post(appObj->lockSem);

    return (retVal);
}

/**
 *  VpsUtils_appSetDctrlPath
 *  \brief Sets the display controller path as per the given parameters.
 */
Int32 VpsUtils_appSetDctrlPath(FVID2_Handle dcHandle,
                               UInt32 driverId,
                               UInt32 driverInstId,
                               UInt32 vencId,
                               Bool useHd1Path)
{
    Int32                   retVal = FVID2_SOK;
    VpsUtils_AppDispInst    instId;

    instId = VpsUtils_appGetDispInst(driverId, driverInstId);
    if (instId >= VPSUTILS_APP_DISP_INST_MAX)
    {
        retVal = FVID2_EFAIL;
    }

    if (FVID2_SOK == retVal)
    {
        retVal = VpsUtils_appConfigDctrlPath(
                     dcHandle,
                     instId,
                     vencId,
                     useHd1Path,
                     FALSE);
    }

    return (retVal);
}

/**
 *  VpsUtils_appClearDctrlPath
 *  \brief Clear the already set display controller path as per the given
 *  parameters.
 */
Int32 VpsUtils_appClearDctrlPath(FVID2_Handle dcHandle,
                                 UInt32 driverId,
                                 UInt32 driverInstId,
                                 UInt32 vencId,
                                 Bool useHd1Path)
{
    Int32                   retVal = FVID2_SOK;
    VpsUtils_AppDispInst    instId;

    instId = VpsUtils_appGetDispInst(driverId, driverInstId);
    if (instId >= VPSUTILS_APP_DISP_INST_MAX)
    {
        retVal = FVID2_EFAIL;
    }

    if (FVID2_SOK == retVal)
    {
        retVal = VpsUtils_appConfigDctrlPath(
                     dcHandle,
                     instId,
                     vencId,
                     useHd1Path,
                     TRUE);
    }

    return (retVal);
}

/**
 *  VpsUtils_appPrintDispCreateStatus
 *  \brief Prints the display driver create status information.
 */
void VpsUtils_appPrintDispCreateStatus(const Vps_DispCreateStatus *createStatus)
{
    Vps_printf(
        "Display Create Status: VENC Standard: %s, Width: %d, Height: %d\n",
        FVID2_getStandardString(createStatus->standard),
        createStatus->dispWidth, createStatus->dispHeight);
    Vps_printf(
        "Display Create Status: Minimum Number of Buffers to Prime: %d\n",
        createStatus->minNumPrimeBuf);
    Vps_printf(
        "Display Create Status: Maximum Request in Input Queue    : %d\n",
        createStatus->maxReqInQueue);
    Vps_printf(
        "Display Create Status: Mosaic Max Layout: %d, Max Window: %d, "
        "Max Col: %d, Max Row: %d\n",
        createStatus->maxMultiWinLayout, createStatus->maxMultiWin,
        createStatus->maxMultiWinCol, createStatus->maxMultiWinRow);

    return;
}

/**
 *  VpsUtils_appPrintDispInstStatus
 *  \brief Prints the display instance status information.
 */
void VpsUtils_appPrintDispInstStatus(FVID2_Handle fvidHandle)
{
    Int32           retVal;
    Vps_DispStatus  dispStatus;

    retVal = FVID2_control(
                 fvidHandle,
                 IOCTL_VPS_DISP_GET_STATUS,
                 &dispStatus,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("Display Inst Status: Error: Get display status failed!!\n");
        return;
    }

    Vps_printf("Display Inst Status: Number of request queued  : %d\n",
        dispStatus.queueCount);
    Vps_printf("Display Inst Status: Number of request dequeued: %d\n",
        dispStatus.dequeueCount);
    Vps_printf("Display Inst Status: Number of frames displayed: %d\n",
        dispStatus.displayedFrameCount);
    Vps_printf("Display Inst Status: Number of frames repeated : %d\n",
        dispStatus.repeatFrameCount);

    /* Check for errors */
    if (dispStatus.queueCount != dispStatus.dequeueCount)
    {
        Vps_printf(
            "Display Inst Status: Warning: Queue and Dequeue counts mismatch!!\n");
    }
    if (dispStatus.repeatFrameCount != 0u)
    {
        Vps_printf(
            "Display Inst Status: Warning: Frames are repeated in driver!!\n");
    }

    return;
}

/**
 *  VpsUtils_appAllocDeiCtxBuffer
 *  \brief Allocate context buffer according to the DEI driver need and
 *  provide it to the driver.
 */
Int32 VpsUtils_appAllocDeiCtxBuffer(FVID2_Handle fvidHandle, UInt32 numCh)
{
    Int32               retVal = FVID2_SOK;
    Vps_DeiCtxInfo      deiCtxInfo;
    Vps_DeiCtxBuf       deiCtxBuf;
    UInt32              chCnt, bCnt;

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Get the number of buffers to allocate */
        deiCtxInfo.channelNum = chCnt;
        retVal = FVID2_control(
                    fvidHandle,
                    IOCTL_VPS_GET_DEI_CTX_INFO,
                    &deiCtxInfo,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Get DEI Context Info IOCTL failed!!\n");
            return (retVal);
        }

        /* Allocate the buffers as requested by the driver */
        for (bCnt = 0u; bCnt < deiCtxInfo.numFld; bCnt++)
        {
            deiCtxBuf.fldBuf[bCnt] = VpsUtils_memAlloc(
                                         deiCtxInfo.fldBufSize,
                                         VPS_BUFFER_ALIGNMENT);
            if (NULL == deiCtxBuf.fldBuf[bCnt])
            {
                Vps_printf("Error: Mem alloc failed!!\n");
                return (retVal);
            }
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMv; bCnt++)
        {
            deiCtxBuf.mvBuf[bCnt] = VpsUtils_memAlloc(
                                        deiCtxInfo.mvBufSize,
                                        VPS_BUFFER_ALIGNMENT);
            if (NULL == deiCtxBuf.mvBuf[bCnt])
            {
                Vps_printf("Error: Mem alloc failed!!\n");
                return (retVal);
            }
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMvstm; bCnt++)
        {
            deiCtxBuf.mvstmBuf[bCnt] = VpsUtils_memAlloc(
                                           deiCtxInfo.mvstmBufSize,
                                           VPS_BUFFER_ALIGNMENT);
            if (NULL == deiCtxBuf.mvstmBuf[bCnt])
            {
                Vps_printf("Error: Mem alloc failed!!\n");
                return (retVal);
            }
        }

        /* Provided the allocated buffer to driver */
        deiCtxBuf.channelNum = chCnt;
        retVal = FVID2_control(
                    fvidHandle,
                    IOCTL_VPS_SET_DEI_CTX_BUF,
                    &deiCtxBuf,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Set DEI Context Buffer IOCTL failed!!\n");
            return (retVal);
        }
    }

    return (retVal);
}

/**
 *  VpsUtils_appFreeDeiCtxBuffer
 *  \brief Get the context buffers back from the DEI driver and free them.
 */
Int32 VpsUtils_appFreeDeiCtxBuffer(FVID2_Handle fvidHandle, UInt32 numCh)
{
    Int32               retVal = FVID2_SOK;
    Vps_DeiCtxInfo      deiCtxInfo;
    Vps_DeiCtxBuf       deiCtxBuf;
    UInt32              chCnt, bCnt;

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Get the number of buffers to allocate */
        deiCtxInfo.channelNum = chCnt;
        retVal = FVID2_control(
                    fvidHandle,
                    IOCTL_VPS_GET_DEI_CTX_INFO,
                    &deiCtxInfo,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Get DEI Context Info IOCTL failed!!\n");
            return (retVal);
        }

        /* Get the allocated buffer back from the driver */
        deiCtxBuf.channelNum = chCnt;
        retVal = FVID2_control(
                    fvidHandle,
                    IOCTL_VPS_GET_DEI_CTX_BUF,
                    &deiCtxBuf,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Get DEI Context Buffer IOCTL failed!!\n");
            return (retVal);
        }

        /* Free the buffers */
        for (bCnt = 0u; bCnt < deiCtxInfo.numFld; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.fldBuf[bCnt], deiCtxInfo.fldBufSize);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMv; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.mvBuf[bCnt], deiCtxInfo.mvBufSize);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMvstm; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.mvstmBuf[bCnt], deiCtxInfo.mvstmBufSize);
        }
    }

    return (retVal);
}

/**
 *  VpsUtils_appPrintM2mDeiCreateStatus
 *  \brief Prints the M2M DEI driver create status information.
 */
void VpsUtils_appPrintM2mDeiCreateStatus(
                                const Vps_M2mDeiCreateStatus *createStatus)
{
    Vps_printf("M2M DEI Create Status: Maximum Handles                  : %d\n",
        createStatus->maxHandles);
    Vps_printf("M2M DEI Create Status: Maximum Channels per Handle      : %d\n",
        createStatus->maxChPerHandle);
    Vps_printf("M2M DEI Create Status: Maximum same Channels per Request: %d\n",
        createStatus->maxSameChPerRequest);
    Vps_printf("M2M DEI Create Status: Maximum Request in Input Queue   : %d\n",
        createStatus->maxReqInQueue);

    return;
}

/**
 *  VpsUtils_appConfigDctrlPath
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders.
 */
static Int32 VpsUtils_appConfigDctrlPath(FVID2_Handle dcHandle,
                                         VpsUtils_AppDispInst instId,
                                         UInt32 vencId,
                                         Bool useHd1Path,
                                         Bool clear)
{
    Int32               retVal = FVID2_SOK;
    UInt32              edge;
    Bool                useHd0Path = TRUE;
    VpsUtils_AppObj    *appObj = &gVpsUtilsAppObj;
    Vps_DcConfig       *dcCfg = &appObj->dcCfg;

    if (appObj->initDone != TRUE)
    {
        Vps_printf("Error: VpsUtils_appInit() is not called!!\n");
        retVal = FVID2_EFAIL;
        return (retVal);
    }

    Semaphore_pend(appObj->lockSem, BIOS_WAIT_FOREVER);

    /* Update edge info */
    edge = 0u;
    dcCfg->useCase = VPS_DC_USERSETTINGS;
    if (TRUE == useHd1Path)
    {
        useHd0Path = FALSE;
    }
    if (VPS_DC_VENC_SD == vencId)
    {
        useHd0Path = FALSE;
        useHd1Path = FALSE;
    }
    if (VPSUTILS_APP_DISP_INST_BP0 == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_BP0_INPUT_PATH;
    }
    else if (VPSUTILS_APP_DISP_INST_BP1 == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_BP1_INPUT_PATH;
    }
    else if (VPSUTILS_APP_DISP_INST_MAIN == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_MAIN_INPUT_PATH;
        if (TRUE == useHd1Path)
        {
            Vps_printf("Warning: Main path cannot go to HD1 path!!\n");
            Vps_printf("Warning: Instead going through HD0 path!!\n");
            useHd1Path = FALSE;
            useHd0Path = TRUE;
        }
    }
    else if (VPSUTILS_APP_DISP_INST_AUX == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_AUX_INPUT_PATH;
    }
    else if (VPSUTILS_APP_DISP_INST_SEC1 == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_SEC1_INPUT_PATH;
        useHd0Path = FALSE;
        useHd1Path = FALSE;
    }
    else if (VPSUTILS_APP_DISP_INST_GRPX0 == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_GRPX0_INPUT_PATH;
        useHd0Path = FALSE;
        useHd1Path = FALSE;
    }
    else if (VPSUTILS_APP_DISP_INST_GRPX1 == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_GRPX1_INPUT_PATH;
        useHd0Path = FALSE;
        useHd1Path = FALSE;
    }
    else if (VPSUTILS_APP_DISP_INST_GRPX2 == instId)
    {
        dcCfg->edgeInfo[edge].startNode = VPS_DC_GRPX2_INPUT_PATH;
        useHd0Path = FALSE;
        useHd1Path = FALSE;
    }
    else
    {
        Vps_printf("Error: Invalid driver instance %d!!\n", instId);
        retVal = FVID2_EFAIL;
    }

    if (TRUE == useHd0Path)
    {
        if (VPSUTILS_APP_DISP_INST_MAIN == instId)
        {
            dcCfg->edgeInfo[edge].endNode = VPS_DC_VCOMP;
            edge++;
        }
        else
        {
            dcCfg->edgeInfo[edge].endNode = VPS_DC_VCOMP_MUX;
            edge++;
            dcCfg->edgeInfo[edge].startNode = VPS_DC_VCOMP_MUX;
            dcCfg->edgeInfo[edge].endNode = VPS_DC_VCOMP;
            edge++;
        }
    }
    else if (TRUE == useHd1Path)
    {
        dcCfg->edgeInfo[edge].endNode = VPS_DC_HDCOMP_MUX;
        edge++;
        dcCfg->edgeInfo[edge].startNode = VPS_DC_HDCOMP_MUX;
        dcCfg->edgeInfo[edge].endNode = VPS_DC_CIG_PIP_INPUT;
        edge++;
    }

    if (VPS_DC_VENC_HDMI == vencId)
    {
        if (TRUE == useHd0Path)
        {
            dcCfg->edgeInfo[edge].startNode = VPS_DC_CIG_NON_CONSTRAINED_OUTPUT;
        }
        else if (TRUE == useHd1Path)
        {
            dcCfg->edgeInfo[edge].startNode = VPS_DC_CIG_PIP_OUTPUT;
        }
        /* For GRPX planes - it comes directly to SDVENC */
        dcCfg->edgeInfo[edge].endNode = VPS_DC_HDMI_BLEND;
        edge++;
    }
    else if (VPS_DC_VENC_HDCOMP == vencId)
    {
        if (TRUE == useHd0Path)
        {
            /* In TI816x, CIG output goes to HDCOMP and in other platforms CIG
             * output goes to DVO2 */
            if ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
                (VPS_PLATFORM_ID_SIM_TI816x == appObj->platformId))
            {
                dcCfg->edgeInfo[edge].startNode =
                    VPS_DC_CIG_CONSTRAINED_OUTPUT;
            }
            else
            {
                dcCfg->edgeInfo[edge].startNode =
                    VPS_DC_CIG_NON_CONSTRAINED_OUTPUT;
            }
        }
        else if (TRUE == useHd1Path)
        {
            dcCfg->edgeInfo[edge].startNode = VPS_DC_CIG_PIP_OUTPUT;

        }
        /* For GRPX planes - it comes directly to SDVENC */
        dcCfg->edgeInfo[edge].endNode = VPS_DC_HDCOMP_BLEND;
        edge++;
    }
    else if (VPS_DC_VENC_DVO2 == vencId)
    {
        if (TRUE == useHd0Path)
        {
            /* In TI816x, CIG output goes to HDCOMP and in other platforms CIG
             * output goes to DVO2 */
            if ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
                (VPS_PLATFORM_ID_SIM_TI816x == appObj->platformId))
            {
                dcCfg->edgeInfo[edge].startNode =
                    VPS_DC_CIG_NON_CONSTRAINED_OUTPUT;
            }
            else
            {
                dcCfg->edgeInfo[edge].startNode = VPS_DC_CIG_CONSTRAINED_OUTPUT;
            }
        }
        else if (TRUE == useHd1Path)
        {
            dcCfg->edgeInfo[edge].startNode = VPS_DC_CIG_PIP_OUTPUT;
        }
        /* For GRPX planes - it comes directly to SDVENC */
        dcCfg->edgeInfo[edge].endNode = VPS_DC_DVO2_BLEND;
        edge++;
    }
    else if (VPS_DC_VENC_SD == vencId)
    {
        if ((VPSUTILS_APP_DISP_INST_AUX == instId) ||
            (VPSUTILS_APP_DISP_INST_BP0 == instId) ||
            (VPSUTILS_APP_DISP_INST_BP1 == instId) ||
            (VPSUTILS_APP_DISP_INST_SEC1 == instId))
        {
            dcCfg->edgeInfo[edge].endNode = VPS_DC_SDVENC_MUX;
            edge++;
            dcCfg->edgeInfo[edge].startNode = VPS_DC_SDVENC_MUX;
        }
        else if (VPSUTILS_APP_DISP_INST_MAIN == instId)
        {
            Vps_printf("Error: Main path cannot go to SDVENC!!\n");
            retVal = FVID2_EFAIL;
        }
        /* For GRPX planes - it comes directly to SDVENC */
        dcCfg->edgeInfo[edge].endNode = VPS_DC_SDVENC_BLEND;
        edge++;
    }
    else
    {
        Vps_printf("Error: Invalid VENC ID %d!!\n", vencId);
        retVal = FVID2_EFAIL;
    }
    dcCfg->numEdges = edge;

    /* Don't configure VENC through this function */
    dcCfg->vencInfo.tiedVencs = 0u;
    dcCfg->vencInfo.numVencs = 0u;

    if (FVID2_SOK == retVal)
    {
        if (TRUE == clear)
        {
            /* Configure display controller - Remove the settings */
            retVal = FVID2_control(
                         dcHandle,
                         IOCTL_VPS_DCTRL_CLEAR_CONFIG,
                         dcCfg,
                         NULL);
        }
        else
        {
            /* Set the display controller configuration */
            retVal = FVID2_control(
                         dcHandle,
                         IOCTL_VPS_DCTRL_SET_CONFIG,
                         dcCfg,
                         NULL);
        }

        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: DCTRL path configuration failed!!\n");
        }
    }

    Semaphore_post(appObj->lockSem);

    return (retVal);
}

/**
 *  VpsUtils_appCfgDctrlVencOutput
 */
static Int32 VpsUtils_appCfgDctrlVencOutput(FVID2_Handle dcHandle,
                                            UInt32 vencId,
                                            UInt32 standard,
                                            Vps_DcAnalogFmt aFmt)
{
    Int32               retVal = FVID2_SOK;
    VpsUtils_AppObj    *appObj = &gVpsUtilsAppObj;
    Vps_DcOutputInfo    dcOutputInfo;
    Vps_DcVencClkSrc    clkSrc;
    Vps_PlatformBoardId boardId;

    boardId = Vps_platformGetBoardId();
    if (boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("Error: Unrecognized board ID!!\n");
        retVal = FVID2_EFAIL;
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the output format */
        dcOutputInfo.dvoFidPolarity = VPS_DC_POLARITY_ACT_HIGH;
        dcOutputInfo.dvoVsPolarity = VPS_DC_POLARITY_ACT_HIGH;
        dcOutputInfo.dvoHsPolarity = VPS_DC_POLARITY_ACT_HIGH;
        dcOutputInfo.dvoActVidPolarity = VPS_DC_POLARITY_ACT_HIGH;
        if (VPS_DC_VENC_DVO2 == vencId)
        {
            dcOutputInfo.vencNodeNum = VPS_DC_VENC_DVO2;
            dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
            if ((VPS_PLATFORM_BOARD_VC == boardId) ||
                (FVID2_STD_WSVGA_70 == standard))
            {
                dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
                dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
            }
            else
            {
                dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_DOUBLECHAN;
                dcOutputInfo.dataFormat = FVID2_DF_YUV422SP_UV;
            }
        }
        else if (VPS_DC_VENC_HDCOMP == vencId)
        {
            if (FVID2_isStandardVesa(standard))
            {
                dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
                dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPONENT;
                dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
            }
            else
            {
                dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDCOMP;
                dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC;
                dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPONENT;
                dcOutputInfo.dataFormat = FVID2_DF_YUV444P;
            }
        }
        else if (VPS_DC_VENC_HDMI == vencId)
        {
            dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDMI;
            dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
            dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
            dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
        }
        else if (VPS_DC_VENC_SD == vencId)
        {
            dcOutputInfo.vencNodeNum = VPS_DC_VENC_SD;
            dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
            dcOutputInfo.aFmt = aFmt;
            dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
        }
        else
        {
            Vps_printf("Error: Invalid VENC ID %d!!\n", vencId);
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        retVal = FVID2_control(
                     dcHandle,
                     IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                     &dcOutputInfo,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: DCTRL set output failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (VPS_DC_VENC_SD != vencId)
        {
            /* Set the Clock source for DVO2 */
            if (VPS_DC_VENC_HDCOMP == vencId)
            {
                clkSrc.venc = VPS_DC_VENC_HDCOMP;
                clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
            }
            else
            {
                if (VPS_DC_VENC_DVO2 == vencId)
                {
                    clkSrc.venc = VPS_DC_VENC_DVO2;
                }
                else
                {
                    clkSrc.venc = VPS_DC_VENC_HDMI;
                }
                clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
                if ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) &&
                    (VPS_PLATFORM_CPU_REV_1_0 == appObj->cpuRev))
                {
                    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD_DIV2;
                }

                /* Clock Source is not selectable in TI814X. It is fixed to
                   VENCD clock for HDMI and VENCA clock for DVO2. These clocks
                   can only be divided by 2 within HDVPSS */
                if ((VPS_PLATFORM_ID_EVM_TI814x == appObj->platformId) ||
                    (VPS_PLATFORM_ID_EVM_TI8107 == appObj->platformId))
                {
                    if (VPS_DC_VENC_DVO2 == vencId)
                    {
                        clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
                    }
                    else
                    {
                        clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
                    }
                }
            }

            retVal = FVID2_control(
                         dcHandle,
                         IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC,
                         &clkSrc,
                         NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("Error: DCTRL set clk source failed\n");
            }
        }
    }

    return (retVal);
}

/**
 *  VpsUtils_appStartOffChipHdmi
 *  Configures and start the off-chip HDMI transmitter connected to DVO2.
 */
static Int32 VpsUtils_appStartOffChipHdmi(VpsUtils_AppObj *appObj,
                                          UInt32 standard)
{
#ifdef VPSUTILS_CONFIG_OFFCHIP_HDMI
    Int32                           retVal = FVID2_SOK;
    Vps_HdmiChipId                  hdmiId;
    Vps_PlatformBoardId             boardId;
    Vps_SiI9022aHpdPrms             hpdPrms;
    Vps_SiI9022aModeParams          modePrms;
    Vps_VideoEncoderCreateParams    encCreateParams;
    Vps_VideoEncoderCreateStatus    encCreateStatus;

    boardId = Vps_platformGetBoardId();
    if (boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("Error: Unrecognized board ID!!\n");
        retVal = FVID2_EFAIL;
    }

    if (FVID2_SOK == retVal)
    {
        /* Open HDMI Tx */
        encCreateParams.deviceI2cInstId = Vps_platformGetI2cInstId();
        encCreateParams.deviceI2cAddr =
                Vps_platformGetVidEncI2cAddr(FVID2_VPS_VID_ENC_SII9022A_DRV);
        encCreateParams.inpClk = 0u;
        encCreateParams.hdmiHotPlugGpioIntrLine = 0u;
        encCreateParams.clkEdge = FALSE;
        if (VPS_PLATFORM_BOARD_VC == boardId)
        {
            encCreateParams.syncMode = VPS_VIDEO_ENCODER_EXTERNAL_SYNC;
        }
        else
        {
            encCreateParams.syncMode = VPS_VIDEO_ENCODER_EMBEDDED_SYNC;
        }

        appObj->hdmiHandle = FVID2_create(
                                 FVID2_VPS_VID_ENC_SII9022A_DRV,
                                 0u,
                                 &encCreateParams,
                                 &encCreateStatus,
                                 NULL);
        if (NULL == appObj->hdmiHandle)
        {
            Vps_printf("Error: SII9022 create failed!!\n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        retVal = FVID2_control(
                     appObj->hdmiHandle,
                     IOCTL_VPS_SII9022A_GET_DETAILED_CHIP_ID,
                     &hdmiId,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Could not get detailed chip ID!!\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        retVal = FVID2_control(
                     appObj->hdmiHandle,
                     IOCTL_VPS_SII9022A_QUERY_HPD,
                     &hpdPrms,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Could not detect HPD!!\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        modePrms.standard = standard;
        retVal = FVID2_control(
                     appObj->hdmiHandle,
                     IOCTL_VPS_VIDEO_ENCODER_SET_MODE,
                     &modePrms,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Could not set mode!!\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        retVal = FVID2_start(appObj->hdmiHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Off-chip HDMI start failed!!\n");
        }
    }

    if ((FVID2_SOK != retVal) && (NULL != appObj->hdmiHandle))
    {
        /* Close HDMI transmitter */
        retVal |= FVID2_delete(appObj->hdmiHandle, NULL);
        appObj->hdmiHandle = NULL;
    }

    return (retVal);
#else
    return (FVID2_SOK);
#endif
}

/**
 *  VpsUtils_appStopOffChipHdmi
 *  Stops the off-chip HDMI transmitter connected to DVO2.
 */
static Int32 VpsUtils_appStopOffChipHdmi(VpsUtils_AppObj *appObj)
{
#ifdef VPSUTILS_CONFIG_OFFCHIP_HDMI
    Int32       retVal = FVID2_SOK;

    if (NULL != appObj->hdmiHandle)
    {
        retVal = FVID2_stop(appObj->hdmiHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("Error: Off-chip HDMI stop failed!!\n");
        }

        /* Close HDMI transmitter */
        retVal |= FVID2_delete(appObj->hdmiHandle, NULL);
        appObj->hdmiHandle = NULL;
    }

    return (retVal);
#else
    return (FVID2_SOK);
#endif
}

static VpsUtils_AppDispInst VpsUtils_appGetDispInst(UInt32 driverId,
                                                    UInt32 driverInstId)
{
    VpsUtils_AppDispInst    instId = VPSUTILS_APP_DISP_INST_MAX;

    if (FVID2_VPS_DISP_DRV == driverId)
    {
        switch (driverInstId)
        {
            case VPS_DISP_INST_BP0:
                instId = VPSUTILS_APP_DISP_INST_BP0;
                break;

            case VPS_DISP_INST_BP1:
                instId = VPSUTILS_APP_DISP_INST_BP1;
                break;

            case VPS_DISP_INST_SEC1:
                instId = VPSUTILS_APP_DISP_INST_SEC1;
                break;

            case VPS_DISP_INST_MAIN_DEIH_SC1:
            case VPS_DISP_INST_MAIN_DEI_SC1:
                instId = VPSUTILS_APP_DISP_INST_MAIN;
                break;

            case VPS_DISP_INST_AUX_DEI_SC2:
            case VPS_DISP_INST_AUX_SC2:
                instId = VPSUTILS_APP_DISP_INST_AUX;
                break;

            default:
                Vps_printf("Error: Invalid driver instance %d!!\n",
                    driverInstId);
                break;
        }
    }
    else if (FVID2_VPS_DISP_GRPX_DRV == driverId)
    {
        switch (driverInstId)
        {
            case VPS_DISP_INST_GRPX0:
                instId = VPSUTILS_APP_DISP_INST_GRPX0;
                break;

            case VPS_DISP_INST_GRPX1:
                instId = VPSUTILS_APP_DISP_INST_GRPX1;
                break;

            case VPS_DISP_INST_GRPX2:
                instId = VPSUTILS_APP_DISP_INST_GRPX2;
                break;

            default:
                Vps_printf("Error: Invalid driver instance %d!!\n",
                    driverInstId);
                break;
        }
    }
    else
    {
        Vps_printf("Error: Invalid driver ID %d!!\n", driverId);
    }

    return (instId);
}
