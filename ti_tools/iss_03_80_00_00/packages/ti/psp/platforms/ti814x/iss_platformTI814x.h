/** ==================================================================
 *  @file   iss_platformTI814x.h                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/ti814x/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    \ingroup ISSUTILS_API
    \defgroup ISSUTILS_PLATFORM_API Platform Specific API
    @{
*/

/**
 *  \file iss_platformTI814x.h
 *
 *  \brief Interface file to the TI814x platform specific functions.
 *
 */

#ifndef _ISS_PLATFORM_TI814X_H_
#define _ISS_PLATFORM_TI814X_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

    /* None */

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

    /* None */

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

    /* Init the underlying platform */
    Int32 Iss_platformTI814xInit(Iss_PlatformInitParams * initParams);

    /* De-Init platform related sub-systems */
    Int32 Iss_platformTI814xDeInit(void);

    /* Init EVM related sub-systems like I2C instance */
    Int32 Iss_platformTI814xDeviceInit(Iss_PlatformDeviceInitParams * initPrms);

    /* De-Init EVM related sub-systems */
    Int32 Iss_platformTI814xDeviceDeInit(void);

    /* Get I2C instance associated with HDISSS M3 video peripherals */
    UInt32 Iss_platformTI814xGetI2cInstId(void);

    /* Get EVM specific I2C address for different video decoder's connected
     * to different video ports */
    UInt8 Iss_platformTI814xGetSensorI2cAddr(UInt32 vidDecId, UInt32 vipInstId);

    /* Function to set pixel clock for the given output */
    // Int32 Iss_platformTI814xSetVencPixClk(Iss_SystemVPllClk *vpllCfg);

    /* Function to select video decoder when video decoder are muxed at board 
     * level */
    Int32 Iss_platformTI814xSelectSensor(UInt32 vidDecId,
                                               UInt32 vipInstId);

    /* reset video devices using GPIO IO expander */
    Int32 Iss_platformTI814xResetVideoDevices(void);

    /* Get the CPU revision */
    Iss_PlatformCpuRev Iss_platformTI814xGetCpuRev(void);

    /* Get the base board revision */
    Iss_PlatformBoardRev Iss_platformTI814xGetBaseBoardRev(void);

    /* Get the add-on or daughter card board revision */
    Iss_PlatformBoardRev Iss_platformTI814xGetDcBoardRev(void);


#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _ISS_PLATFORM_TI814X_H_ 
 *//* @} */
