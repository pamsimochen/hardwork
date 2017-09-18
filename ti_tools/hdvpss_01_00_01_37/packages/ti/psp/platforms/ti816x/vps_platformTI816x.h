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
 *  \file vps_platformTI816x.h
 *
 *  \brief Interface file to the TI816x platform specific functions.
 *
 */

#ifndef _VPS_PLATFORM_TI816X_H_
#define _VPS_PLATFORM_TI816X_H_

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
Int32 Vps_platformTI816xInit(Vps_PlatformInitParams *initParams);

/* De-Init platform related sub-systems */
Int32 Vps_platformTI816xDeInit(void);

/* Init EVM related sub-systems like I2C instance */
Int32 Vps_platformTI816xDeviceInit(Vps_PlatformDeviceInitParams *initPrms);

/* De-Init EVM related sub-systems */
Int32 Vps_platformTI816xDeviceDeInit(void);

/* Get I2C instance associated with HDVPSS M3 video peripherals */
UInt32 Vps_platformTI816xGetI2cInstId(void);

/* Get EVM specific I2C address for different video decoder's connected to
 * different video ports */
UInt8 Vps_platformTI816xGetVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId);

/* Get EVM specific I2C address for different video encoders (like Sii9022) */
UInt8 Vps_platformTI816xGetVidEncI2cAddr(UInt32 vidEncId);

/* Select video port input file and pixel clock for simulator   */
Int32 Vps_platformTI816xSimVideoInputSelect(UInt32 vipInstId,
                                            UInt32 fileId,
                                            UInt32 pixelClk);

/* Function to set pixel clock for the given output */
Int32 Vps_platformTI816xSetVencPixClk(Vps_SystemVPllClk *vpllCfg);

/* Function to select video decoder when video decoder are muxed at board level */
Int32 Vps_platformTI816xSelectVideoDecoder(UInt32 vidDecId, UInt32 vipInstId);

/* reset video devices using GPIO IO expander */
Int32 Vps_platformTI816xResetVideoDevices(void);

/* Get the CPU revision */
Vps_PlatformCpuRev Vps_platformTI816xGetCpuRev(void);

/* Get the base board revision */
Vps_PlatformBoardRev Vps_platformTI816xGetBaseBoardRev(void);

/* Get the add-on or daughter card board revision */
Vps_PlatformBoardRev Vps_platformTI816xGetDcBoardRev(void);

/* Enables and select proper filter in the THS device for the TVP7002 decoder */
Int32 Vps_platformTI816xEnableTvp7002Filter(FVID2_Standard standard);

/* Selects the source signal for the HDCOMP (DAC) sync outputs. */
Int32 Vps_platformTI816xSelectHdCompSyncSource(
        Vps_PlatformHdCompSyncSource syncSrc,
        UInt32 enable);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_PLATFORM_TI816X_H_ */

/*@}*/
