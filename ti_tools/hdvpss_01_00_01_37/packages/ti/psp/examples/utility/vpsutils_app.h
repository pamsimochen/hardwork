/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSUTILS_API
    \defgroup VPSUTILS_APP_API Application helper APIs
    @{
*/

/**
 *  \file vpsutils_app.h
 *
 *  \brief Header file to use application level helper functions.
 */

#ifndef _VPSUTILS_APP_H_
#define _VPSUTILS_APP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief VPS application utils init function.
 *
 *  Init function which allocates semaphore and initializes
 *  global objects.
 *
 *  This function should be called before calling any of the below API's and
 *  should be called only once after doing all other module initialization
 *  like FIVD2 init, device init and platform init.
 *
 *  \return FVID2_SOK on success else appropiate FVID2 error code on failure.
 */
Int32 VpsUtils_appInit(void);

/**
 *  \brief VPS application utils deinit function.
 *
 *  De-init function which frees already allocated objects.
 *
 *  \return FVID2_SOK on success else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appDeInit(void);


/* ========================================================================== */
/*                  Display Driver Related Helper functions                   */
/* ========================================================================== */

/**
 *  \brief Configures display VENC pixel clock as per the provided standard
 *  by programming the video PLL.
 *
 *  \param vencId       [IN] ID of VENC to set the pixel clock.
 *                           Supports on-Chip HDMI, DVO2, HDDAC and SD VENCs.
 *  \param standard     [IN] Required VENC standard.
 *                           For valid values see #FVID2_Standard.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appSetVencPixClk(UInt32 vencId, UInt32 standard);

/**
 *  \brief Configures and start display VENC for a particular mode and sets
 *  the VENC output format as per provided parameters.
 *
 *  This will also configure and start the off-chip HDMI if required.
 *  This configures the THS filter present in the EVM for HDCOMP and SD VENC.
 *
 *  Note: This function doesn't configure the VENC PLL (pixel clock).
 *
 *  \param dcHandle     [IN] Valid display controller driver handle.
 *  \param vencId       [IN] ID of VENC to configure and start.
 *                           Supports on-Chip HDMI, DVO2, HDDAC and SD VENCs.
 *  \param standard     [IN] Required VENC standard.
 *                           For valid values see #FVID2_Standard.
 *  \param aFmt         [IN] Analog standard for SD VENC - Composite/Svideo.
 *                           Not use for other VENCs and application should
 *                           initialize this to VPS_DC_A_OUTPUT_COMPONENT.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appStartVenc(FVID2_Handle dcHandle,
                            UInt32 vencId,
                            UInt32 standard,
                            Vps_DcAnalogFmt aFmt);

/**
 *  \brief Stops the VENC.
 *
 *  This will also stops the off-chip HDMI if required.
 *  And disables the THS filter present in the EVM for HDCOMP and SD VENC.
 *
 *  \param dcHandle     [IN] Valid display controller driver handle.
 *  \param vencId       [IN] ID of VENC to stop.
 *                           Supports on-Chip HDMI, DVO2, HDDAC and SD VENCs.
 *  \param standard     [IN] Required VENC standard.
 *                           For valid values see #FVID2_Standard.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appStopVenc(FVID2_Handle dcHandle,
                           UInt32 vencId,
                           UInt32 standard);

/**
 *  \brief Sets the display controller path as per the given parameters.
 *
 *  Notes:
 *  1. This currently doesn't support tied VENC configurations and
 *  display path connected to multiple VENCs.
 *  2. This function doesn't configure the VENC. Configuration of VENC for a
 *  particular mode should be done prior to calling this function.
 *
 *  \param dcHandle     [IN] Valid display controller driver handle.
 *  \param driverId     [IN] Driver ID. This should be one of FVID2_VPS_DISP_DRV
 *                           or FVID2_VPS_DISP_GRPX_DRV.
 *  \param driverInstId [IN] Should be a valid driver instance ID - display
 *                           or GRPX as defined in the respective header files.
 *                           Supports BP0, BP1, SEC1, MAIN and AUX display
 *                           paths and GRPX0, GRPX1 and GRPX2 graphics paths.
 *  \param vencId       [IN] VENC ID to which to connect driver instance.
 *                           Supports on-Chip HDMI, DVO2, HDDAC and SD VENCs.
 *  \param useHd1Path   [IN] This flag will determine whether to use the
 *                           HD0 path or HDCOMP (HD1) path to connect to the
 *                           VENC. This parameter should be TRUE only for
 *                           BP0, BP1 and AUX paths as only these paths could
 *                           be connected to HDCOMP (HD1) path. For all other
 *                           instances this should be set to FALSE.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appSetDctrlPath(FVID2_Handle dcHandle,
                               UInt32 driverId,
                               UInt32 driverInstId,
                               UInt32 vencId,
                               Bool useHd1Path);

/**
 *  \brief Clear the already set display controller path as per the given
 *  parameters.
 *
 *  Notes:
 *  1. This currently doesn't support tied VENC configurations and
 *  display path connected to multiple VENCs.
 *  2. This function doesn't configure the VENC. Configuration of VENC for a
 *  particular mode should be done prior to calling this function.
 *
 *  \param dcHandle     [IN] Valid display controller driver handle.
 *  \param driverId     [IN] Driver ID. This should be one of FVID2_VPS_DISP_DRV
 *                           or FVID2_VPS_DISP_GRPX_DRV.
 *  \param driverInstId [IN] Should be a valid driver instance ID - display
 *                           or GRPX as defined in the respective header files.
 *                           Supports BP0, BP1, SEC1, MAIN and AUX display
 *                           paths and GRPX0, GRPX1 and GRPX2 graphics paths.
 *  \param vencId       [IN] VENC ID to which to disconnect the driver instance.
 *                           Supports on-Chip HDMI, DVO2, HDDAC and SD VENCs.
 *  \param useHd1Path   [IN] This flag will determine whether to use the
 *                           HD0 path or HDCOMP (HD1) path to disconnect to the
 *                           VENC. This parameter should be TRUE only for
 *                           BP0, BP1 and AUX paths as only these paths could
 *                           be connected to HDCOMP (HD1) path. For all other
 *                           instances this should be set to FALSE.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appClearDctrlPath(FVID2_Handle dcHandle,
                                 UInt32 driverId,
                                 UInt32 driverInstId,
                                 UInt32 vencId,
                                 Bool useHd1Path);

/**
 *  \brief Prints the display driver create status information.
 *
 *  \param createStatus [IN] Pointer to display driver create status.
 */
void VpsUtils_appPrintDispCreateStatus(
                                const Vps_DispCreateStatus *createStatus);

/**
 *  \brief Prints the display instance status information by calling the
 *  display driver get status IOCTL.
 *
 *  \param fvidHandle   [IN] Valid display driver handle.
 */
void VpsUtils_appPrintDispInstStatus(FVID2_Handle fvidHandle);


/* ========================================================================== */
/*                  M2M DEI Driver Related Helper functions                   */
/* ========================================================================== */

/**
 *  \brief Allocate context buffer according to the DEI driver need and
 *  provide it to the driver.
 *
 *  This function internally uses VpsUtils_memAlloc() to allocate buffer memory.
 *
 *  \param fvidHandle   [IN] Valid M2M DEI driver handle.
 *  \param numCh        [IN] Number of channel to configure. This is typically
 *                      the number of channels provided at create time.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appAllocDeiCtxBuffer(FVID2_Handle fvidHandle, UInt32 numCh);

/**
 *  \brief Get the context buffers back from the DEI driver and free them.
 *
 *  This function internally uses VpsUtils_memFree() to free buffer memory.
 *
 *  \param fvidHandle   [IN] Valid M2M DEI driver handle.
 *  \param numCh        [IN] Number of channel to configure. This is typically
 *                      the number of channels provided at create time.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 VpsUtils_appFreeDeiCtxBuffer(FVID2_Handle fvidHandle, UInt32 numCh);

/**
 *  \brief Checks if the M2M DEI driver instance is a DEI_HQ instance.
 *
 *  \param drvInst      [IN] Valid M2M DEI driver instance ID.
 *
 *  \return TRUE if the instance is DEI_HQ path else returns FALSE.
 */
static Int32 VpsUtils_appM2mDeiIsHqInst(UInt32 drvInst)
{
    Int32       result = FALSE;

#ifdef TI_816X_BUILD
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == drvInst))
    {
        result = TRUE;
    }
#endif

    return (result);
}

/**
 *  \brief Checks if the M2M DEI driver instance has write-back path.
 *
 *  \param drvInst      [IN] Valid M2M DEI driver instance ID.
 *
 *  \return TRUE if the instance has write-back path else returns FALSE.
 */
static Int32 VpsUtils_appM2mDeiIsWbInst(UInt32 drvInst)
{
    Int32       result = FALSE;

#ifdef TI_816X_BUILD
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == drvInst) ||
        (VPS_M2M_INST_AUX_DEI_SC2_WB1 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1 == drvInst))
    {
        result = TRUE;
    }
#else
    if ((VPS_M2M_INST_MAIN_DEI_SC1_WB0 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEI_SC1_SC3_WB0_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_SC2_WB1 == drvInst) ||
        (VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1 == drvInst))
    {
        result = TRUE;
    }
#endif

    return (result);
}

/**
 *  \brief Checks if the M2M DEI driver instance has VIP path.
 *
 *  \param drvInst      [IN] Valid M2M DEI driver instance ID.
 *
 *  \return TRUE if the instance has VIP path else returns FALSE.
 */
static Int32 VpsUtils_appM2mDeiIsVipInst(UInt32 drvInst)
{
    Int32       result = FALSE;

#ifdef TI_816X_BUILD
    if ((VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_DEI_SC4_VIP1 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1 == drvInst))
    {
        result = TRUE;
    }
#else
    if ((VPS_M2M_INST_MAIN_DEI_SC3_VIP0 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEI_SC1_SC3_WB0_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_SC4_VIP1 == drvInst) ||
        (VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1 == drvInst))
    {
        result = TRUE;
    }
#endif

    return (result);
}

/**
 *  \brief Checks if the M2M DEI driver instance is a dual out instance.
 *
 *  \param drvInst      [IN] Valid M2M DEI driver instance ID.
 *
 *  \return TRUE if the instance is dual output else returns FALSE.
 */
static Int32 VpsUtils_appM2mDeiIsDualOutInst(UInt32 drvInst)
{
    Int32       isDualOut = FALSE;

#ifdef TI_816X_BUILD
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1 == drvInst))
    {
        isDualOut = TRUE;
    }
#else
    if ((VPS_M2M_INST_MAIN_DEI_SC1_SC3_WB0_VIP0 == drvInst) ||
        (VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1 == drvInst))
    {
        isDualOut = TRUE;
    }
#endif

    return (isDualOut);
}

/**
 *  \brief Checks if the M2M DEI driver instance has DEIH or DEI in it.
 *
 *  \param drvInst      [IN] Valid M2M DEI driver instance ID.
 *
 *  \return TRUE if the instance has DEIH/DEI else returns FALSE.
 */
static Int32 VpsUtils_appM2mDeiIsDeiPresent(UInt32 drvInst)
{
    Int32       isDeiPresent = TRUE;

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    if ((VPS_M2M_INST_AUX_SC2_WB1 == drvInst) ||
        (VPS_M2M_INST_AUX_SC4_VIP1 == drvInst) ||
        (VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1 == drvInst))
    {
        isDeiPresent = FALSE;
    }
#endif

    return (isDeiPresent);
}

/**
 *  \brief Prints the M2M DEI driver create status information.
 *
 *  \param createStatus [IN] Pointer to M2M DEI driver create status.
 */
void VpsUtils_appPrintM2mDeiCreateStatus(
                                const Vps_M2mDeiCreateStatus *createStatus);

#ifdef __cplusplus
}
#endif

#endif  /* #define _VPSUTILS_APP_H_ */

/*@}*/
