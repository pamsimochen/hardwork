/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \defgroup VPSUTILS_PLATFORM_API Platform Specific API
    @{
*/

/**
 *  \file vps_platform.h
 *
 *  \brief Interface file to the platform specific functions abstraction APIs.
 *
 */

#ifndef _VPS_PLATFORM_H_
#define _VPS_PLATFORM_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/fvid2.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**< Platform driver Id */
#define FVID2_VPS_VID_SYSTEM_DRV          (VPS_VID_SYSTEM_DRV_BASE + 0x0000)

/**
  * \brief ioclt for setting video pll.
  *
  *
  * \param cmdArgs       [IN]   Vps_SystemVPllClk *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL                                     \
            (VPS_VID_SYSTEM_IOCTL_BASE + 0x0000)

/**
  * \brief ioclt for getting current video settings.
  *
  *
  * \param cmdArgs       [OUT]  Vps_SystemVPllClk *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_VPS_VID_SYSTEM_GET_VIDEO_PLL                                     \
            (VPS_VID_SYSTEM_IOCTL_BASE + 0x0001)

/**
  * \brief ioclt for getting platform Id.
  *
  *
  * \param cmdArgs       [IN]   UInt32 *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_VPS_VID_SYSTEM_GET_PLATFORM_ID                                   \
            (VPS_VID_SYSTEM_IOCTL_BASE + 0x0002)

/**
 *  \brief Enum for the output clock modules
 *  Caution: Do not change the enum values.
 */
typedef enum
{
    VPS_SYSTEM_VPLL_OUTPUT_VENC_RF = 0,
    /**< Pixel clock frequency for the RF,
         Note: SD Pixel frequency will RF Clock Freq/4.
         This is Video0 PLL for DM385 */
    VPS_SYSTEM_VPLL_OUTPUT_VENC_D,
    /**< VencD output clock.
         This is Video1 PLL for DM385 */
    VPS_SYSTEM_VPLL_OUTPUT_VENC_A,
    /**< VencA output clock. For the TI814X, this is for DVO1.
         This is HDMI PLL for DM385 */
    VPS_SYSTEM_VPLL_OUTPUT_HDMI,
    /**< HDMI output clock, this is used for HDMI display TI814X only. */
    VPS_SYSTEM_VPLL_OUTPUT_MAX_VENC
    /**< This should be last Enum. */
} Vps_VPllOutputClk;

/**
 *  \brief Platform ID.
 */
typedef enum
{
    VPS_PLATFORM_ID_UNKNOWN,
    /**< Unknown platform. */
    VPS_PLATFORM_ID_EVM_TI816x,
    /**< TI816x EVMs. */
    VPS_PLATFORM_ID_SIM_TI816x,
    /**< TI816x Simulator. */
    VPS_PLATFORM_ID_EVM_TI814x,
    /**< TI814x EVMs. */
    VPS_PLATFORM_ID_SIM_TI814x,
    /**< TI814x Simulator. */
    VPS_PLATFORM_ID_EVM_TI8107,
    /**< TI8107 EVMs. */
    VPS_PLATFORM_ID_MAX
    /**< Max Platform ID. */
} Vps_PlatformId;

/**
 *  \brief EVM Board ID.
 */
typedef enum
{
    VPS_PLATFORM_BOARD_UNKNOWN,
    /**< Unknown board. */
    VPS_PLATFORM_BOARD_VS,
    /**< TVP5158 based board. */
    VPS_PLATFORM_BOARD_VC,
    /**< TVP7002/SII9135 based board. */
    VPS_PLATFORM_BOARD_CATALOG,
    /**< TVP7002/SIL1161A. */
    VPS_PLATFORM_BOARD_CUSTOM,
    /**< Any other custom board. */
    VPS_PLATFORM_BOARD_NETCAM,
    /**< Netcam/VCAM board. */
    VPS_PLATFORM_BOARD_MAX
    /**< Max board ID. */
} Vps_PlatformBoardId;

/**
 *  \brief CPU revision ID.
 */
typedef enum
{
    VPS_PLATFORM_CPU_REV_1_0,
    /**< CPU revision 1.0. */
    VPS_PLATFORM_CPU_REV_1_1,
    /**< CPU revision 1.1. */
    VPS_PLATFORM_CPU_REV_2_0,
    /**< CPU revision 2.0. */
    VPS_PLATFORM_CPU_REV_2_1,
    /**< CPU revision 2.1. */
    VPS_PLATFORM_CPU_REV_UNKNOWN,
    /**< Unknown/unsupported CPU revision. */
    VPS_PLATFORM_CPU_REV_MAX
    /**< Max CPU revision. */
} Vps_PlatformCpuRev;

/**
 *  \brief Board revision ID.
 */
typedef enum
{
    VPS_PLATFORM_BOARD_REV_UNKNOWN,
    /**< Unknown/unsupported board revision. */
    VPS_PLATFORM_BOARD_REV_A,
    /**< Board revision A. */
    VPS_PLATFORM_BOARD_REV_B,
    /**< Board revision B. */
    VPS_PLATFORM_BOARD_REV_C,
    /**< Board revision B. */
    VPS_PLATFORM_BOARD_REV_MAX
    /**< Max board revision. */
} Vps_PlatformBoardRev;

/**
 *  \brief Enums for HDCOMP DAC sync output selection.
 */
typedef enum
{
    VPS_PLATFORM_HDCOMP_SYNC_SRC_DVO1,
    /**< HDCOMP(DAC) HSYNC/VSYNC analog outputs are selected instead of
         VOUT1_HSYNC/VSYNC (DVO1). */
    VPS_PLATFORM_HDCOMP_SYNC_SRC_DVO2
    /**< HDCOMP(DAC) HSYNC/VSYNC analog outputs are selected instead of
         VOUT0_AVID/FID (DVO2). */
} Vps_PlatformHdCompSyncSource;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief System VPLL Clock
 */
typedef struct
{
    UInt32 outputVenc;
    /**< Select output venc for which video pll is configured.
         See #Vps_VPllOutputClk  See for all possible values */
    UInt32 outputClk;
    /**< Pixel clock frequency for vencs. Expected to specify in KHz */
} Vps_SystemVPllClk;

/**
 *  \brief Device initialization parameters
 */
typedef struct
{
    UInt32  isI2cInitReq;
    /**< Indicates whether I2C initialization is required */
    UInt32  isI2cProbingReq;
    /**< If this is TRUE, Vps_platformDeviceInit will try to probe all the I2C
     * devices connected on a specific I2C bus. This should be FALSE for
     * all production  system since this is a time consuming function.
     * For debugging this should be kept TRUE to probe all on-board I2C devices.
     * This field is dont care if #isI2cInitReq=FALSE.
     */
} Vps_PlatformDeviceInitParams;

/**
 *  \brief Platform initialization parameters
 */
typedef struct
{
    UInt32  isPinMuxSettingReq;
    /**< Pinumx setting is requried or not. Sometimes pin mux setting
     *   is required to be done from Linux.
     */
} Vps_PlatformInitParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Init the underlying platform.
 *
 *  \param platformInitPrms     [IN] Platform Initialization parameters. If
 *                              NULL is passed pin mux setting will be done by
 *                              default.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
/*  */
Int32 Vps_platformInit(Vps_PlatformInitParams *platformInitPrms);

/**
 *  \brief De-init the underlying platform.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformDeInit(void);

/**
 *  \brief Init EVM related sub-systems like I2C instance.
 *
 *  \param deviceInitPrms     [IN] Device Initialization parameters. If NULL is
 *                            passed I2C will be initialized by default.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformDeviceInit(Vps_PlatformDeviceInitParams *deviceInitPrms);

/**
 *  \brief De-init EVM related sub-systems.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformDeviceDeInit(void);

/**
 *  \brief Returns the I2c instance Id.
 *
 *  \return I2C instance ID on success.
 */
UInt32 Vps_platformGetI2cInstId(void);

/**
 *  \brief Returns EVM specific I2C address for different video decoders (like
 *  TVP5158, TVP7002, Sii9135 etc), connected to different video ports.
 *
 *  \param vidDecId     [IN] Video decoder ID
 *
 *  \param vipInstId    [IN] VIP Instance ID
 *
 *  \return I2C address on success
 */
UInt8 Vps_platformGetVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId);

/**
 *  \brief Returns EVM specific I2C address for different video encoders
 *  (like Sii9022).
 *
 *  \param vidEncId     [IN] Video encoder ID
 *
 *  \return I2C address on success
 */
UInt8 Vps_platformGetVidEncI2cAddr(UInt32 vidEncId);

/**
 *  \brief Selects video port input file and pixel clock for simulator.
 *
 *  \param vipInstId    [IN] VIP instance ID
 *
 *  \param fileId       [IN] File ID
 *
 *  \param pixelClk     [IN] Pixel clock
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformSimVideoInputSelect(UInt32 vipInstId,
                                      UInt32 fileId,
                                      UInt32 pixelClk);

/**
 *  \brief Returns TRUE if the platform is EVM, else returns false.
 *
 *  \return TRUE if the platform is EVM, else FALSE
 */
UInt32 Vps_platformIsEvm(void);

/**
 *  \brief Returns TRUE if the platform is simulator, else returns false.
 *
 *  \return TRUE if the platform is simulator, else FALSE
 */
UInt32 Vps_platformIsSim(void);

/**
 *  \brief Returns the platform ID.
 *
 *  \return Platform ID on success.
 */
Vps_PlatformId Vps_platformGetId(void);

/**
 *  \brief Probes for video device and return board ID
 *
 *  \return Board ID on success
 */
Vps_PlatformBoardId Vps_platformGetBoardId(void);

/**
 *  \brief Returns the CPU revision.
 *
 *  \return CPU revision information on success.
 */
Vps_PlatformCpuRev Vps_platformGetCpuRev(void);

/**
 *  \brief Returns the base board revision.
 *
 *  \return Base board revision information on success.
 */
Vps_PlatformBoardRev Vps_platformGetBaseBoardRev(void);

/**
 *  \brief Returns the add-on or daughter card board revision.
 *
 *  \return Add-on or daughter card board revision information on success.
 */
Vps_PlatformBoardRev Vps_platformGetDcBoardRev(void);

/**
 *  \brief Selects video decoder when video decoder is muxed at board level.
 *
 *  \param vidDecId     [IN] Video decoder ID
 *
 *  \param vipInstId    [IN] VIP instance ID
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformSelectVideoDecoder(UInt32 vidDecId, UInt32 vipInstId);

/**
 *  \brief Resets video related devices on EVM.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformVideoResetVideoDevices();

/**
 *  \brief Enables and select proper filter in the THS device for the TVP7002
 *         decoder.
 *
 *  \param standard     [IN] FVID2 standard to be selected.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformEnableTvp7002Filter(FVID2_Standard standard);

/**
 *  \brief Enable THS7353.
 *
 *  \param standard     [IN] FVID2 standard to be selected.
 *
 *  \param i2cInstId    [IN] I2C instance ID
 *
 *  \param i2cDevAddr   [IN] I2C device address
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformEnableThs7353(FVID2_Standard standard,
                                UInt32 i2cInstId,
                                UInt32 i2cDevAddr);

/**
 *  \brief Enable LCD Output.
 *
 *  \param isEnable     [IN] Flag to enable/disable LCD
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformEnableLcd(UInt32 isEnable);

/**
 *  \brief Enable LCD Backlight.
 *
 *  \param isEnable     [IN] Flag to enable/disable LCD
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformEnableLcdBackLight(UInt32 isEnable);

/**
 *  \brief Set the LCD back light to make LCD light or Dim.
 *
 *  \param value     [IN] LCD back Light balue
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformSetLcdBackLight(UInt32 value);

/**
 *  \brief Selects the source signal for the HDCOMP (DAC) sync outputs.
 *
 *  This is applicable only for TI816x PG2.0 or above versions.
 *
 *  Caution: Application should disable the selection by setting enable to
 *  FALSE in case the pin mux needs to be reverted to the original state.
 *
 *  \param syncSrc      [IN] Selects the HDCOMP sync source as DVO1/DVO2 pins.
 *
 *  \param enable       [IN] TRUE - Enables HDCOMP syncs instead of DVO1/DVO2.
 *                           FALSE - Enables DVO1/DVO2 instead of
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformSelectHdCompSyncSource(Vps_PlatformHdCompSyncSource syncSrc,
                                         UInt32 enable);


/**
 *  \brief Selects the clock source for HDCOMP VENC
 *
 *  This is applicable only for DM385/TI8107 platform.
 *
 *  \param clkSrc       [IN] Selects the HDCOMP sync source as DVO1/DVO2 pins.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_platformSelectHdCompClkSrc(Vps_VPllOutputClk clkSrc);


/**
 *  \brief Function to get the name of the platform in printable string.
 *
 *  \return Returns a const pointer to the string. If the platform is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *Vps_platformGetString(void);

/**
 *  \brief Function to get the name of the board in printable string.
 *
 *  \return Returns a const pointer to the string. If the board is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *Vps_platformGetBoardString(void);

/**
 *  \brief Function to get the name of the CPU revision in printable string.
 *
 *  \return Returns a const pointer to the string. If the CPU revision is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *Vps_platformGetCpuRevString(void);

/**
 *  \brief Function to get the name of the base board revision in printable
 *  string.
 *
 *  \return Returns a const pointer to the string. If the base board revision is
 *  not known, then it return the string as "UNKNOWN".
 */
const Char *Vps_platformGetBaseBoardRevString(void);

/**
 *  \brief Function to get the name of the daughter card revision in printable
 *  string.
 *
 *  \return Returns a const pointer to the string. If the daughter card revision
 *  is not known, then it return the string as "UNKNOWN".
 */
const Char *Vps_platformGetDcRevString(void);

/**
 *  \brief Funtion to print all the platform information like CPU revision,
 *  board type, board revision etc...
 *
 */
Void Vps_platformPrintInfo(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_PLATFORM_H_ */

/*@}*/
