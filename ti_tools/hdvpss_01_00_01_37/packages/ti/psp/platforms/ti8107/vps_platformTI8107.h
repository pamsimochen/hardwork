/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSUTILS_API
    \defgroup VPSUTILS_PLATFORM_API Platform Specific API
    @{
*/

/**
 *  \file vps_platformTI8107.h
 *
 *  \brief Interface file to the TI8107 platform specific functions.
 *
 */

#ifndef _VPS_PLATFORM_TI8107_H_
#define _VPS_PLATFORM_TI8107_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

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

/* Init the underlying platform */
Int32 Vps_platformTI8107Init(Vps_PlatformInitParams *initParams);

/* De-Init platform related sub-systems */
Int32 Vps_platformTI8107DeInit(void);

/* Init EVM related sub-systems like I2C instance */
Int32 Vps_platformTI8107DeviceInit(Vps_PlatformDeviceInitParams *initPrms);

/* De-Init EVM related sub-systems */
Int32 Vps_platformTI8107DeviceDeInit(void);

/* Get I2C instance associated with HDVPSS M3 video peripherals */
UInt32 Vps_platformTI8107GetI2cInstId(void);

/* Get EVM specific I2C address for different video decoder's connected to
 * different video ports */
UInt8 Vps_platformTI8107GetVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId);

/* Get EVM specific I2C address for different video encoders (like Sii9022) */
UInt8 Vps_platformTI8107GetVidEncI2cAddr(UInt32 vidEncId);

/* Select video port input file and pixel clock for simulator   */
Int32 Vps_platformTI8107SimVideoInputSelect(UInt32 vipInstId,
                                            UInt32 fileId,
                                            UInt32 pixelClk);

/* Function to set pixel clock for the given output */
Int32 Vps_platformTI8107SetVencPixClk(Vps_SystemVPllClk *vpllCfg);

/* Function to select video decoder when video decoder are muxed at board level */
Int32 Vps_platformTI8107SelectVideoDecoder(UInt32 vidDecId, UInt32 vipInstId);

/* reset video devices using GPIO IO expander */
Int32 Vps_platformTI8107ResetVideoDevices(void);

/* Get the CPU revision */
Vps_PlatformCpuRev Vps_platformTI8107GetCpuRev(void);

/* Get the base board revision */
Vps_PlatformBoardRev Vps_platformTI8107GetBaseBoardRev(void);

/* Get the add-on or daughter card board revision */
Vps_PlatformBoardRev Vps_platformTI8107GetDcBoardRev(void);

/* Enables and select proper filter in the THS device for the TVP7002 decoder */
Int32 Vps_platformTI8107EnableTvp7002Filter(FVID2_Standard standard);

/* Enable LCD by setting GPIO output */
Int32 Vps_platformTI8107EnableLcd(UInt32 isEnable);

/* Enable LCD backlight */
Int32 Vps_platformTI8107EnableLcdBackLight(UInt32 isEnable);

/* Set LCD backlight */
Int32 Vps_platformTI8107SetLcdBackLight(UInt32 value);

Int32 Vps_platformTI816xSelectHdCompClkSrc(Vps_VPllOutputClk clkSrc);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_PLATFORM_TI8107_H_ */

/*@}*/
