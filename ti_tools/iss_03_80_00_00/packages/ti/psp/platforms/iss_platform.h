/** ==================================================================
 *  @file   iss_platform.h                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    \defgroup ISSUTILS_PLATFORM_API Platform Specific API
    @{
*/

/**
 *  \file iss_platform.h
 *
 *  \brief Interface file to the platform specific functions abstraction APIs.
 *
 */

#ifndef _ISS_PLATFORM_H_
#define _ISS_PLATFORM_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/vps/fvid2.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

/**< Platform driver Id */
#define FVID2_ISS_VID_SYSTEM_DRV          (ISS_VID_SYSTEM_DRV_BASE + 0x0000)

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
#define IOCTL_ISS_VID_SYSTEM_GET_PLATFORM_ID                                   \
            (ISS_VID_SYSTEM_IOCTL_BASE + 0x0000)

/**
 *  \brief Platform ID.
 */
    typedef enum {
        ISS_PLATFORM_ID_UNKNOWN,
    /**< Unknown platform. */
        ISS_PLATFORM_ID_EVM_TI814x,
    /**< TI814x EVMs. */
        ISS_PLATFORM_ID_SIM_TI814x,
    /**< TI814x Simulator. */
        ISS_PLATFORM_ID_MAX
    /**< Max Platform ID. */
    } Iss_PlatformId;

/**
 *  \brief EVM Board ID.
 */
    typedef enum {
        ISS_PLATFORM_BOARD_UNKNOWN,
    /**< Unknown board. */
        ISS_PLATFORM_BOARD_VCAM,
    /**< camera based board. */
        ISS_PLATFORM_BOARD_MAX
    /**< Max board ID. */
    } Iss_PlatformBoardId;

/**
 *  \brief CPU revision ID.
 */
    typedef enum {
        ISS_PLATFORM_CPU_REV_1_0,
    /**< CPU revision 1.0. */
        ISS_PLATFORM_CPU_REV_1_1,
    /**< CPU revision 1.1. */
        ISS_PLATFORM_CPU_REV_2_0,
    /**< CPU revision 2.0. */
        ISS_PLATFORM_CPU_REV_2_1,
    /**< CPU revision 2.1. */
        ISS_PLATFORM_CPU_REV_UNKNOWN,
    /**< Unknown/unsupported CPU revision. */
        ISS_PLATFORM_CPU_REV_MAX
    /**< Max CPU revision. */
    } Iss_PlatformCpuRev;

/**
 *  \brief Board revision ID.
 */
    typedef enum {
        ISS_PLATFORM_BOARD_REV_UNKNOWN,
    /**< Unknown/unsupported board revision. */
        ISS_PLATFORM_BOARD_REV_A,
    /**< Board revision A. */
        ISS_PLATFORM_BOARD_REV_B,
    /**< Board revision B. */
        ISS_PLATFORM_BOARD_REV_C,
    /**< Board revision B. */
        ISS_PLATFORM_BOARD_REV_MAX
    /**< Max board revision. */
    } Iss_PlatformBoardRev;

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

/**
 *  \brief Device initialization parameters
 */
    typedef struct {
        UInt32 isI2cInitReq;
    /**< Indicates whether I2C initialization is required */
    } Iss_PlatformDeviceInitParams;

/**
 *  \brief Platform initialization parameters
 */
    typedef struct {
        UInt32 isPinMuxSettingReq;
    /**< Pinumx setting is requried or not. Sometimes pin mux setting
     *   is required to be done from Linux.
     */
    } Iss_PlatformInitParams;

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

/**
 *  \brief Init the underlying platform.
 *
 *  \param platformInitPrms     [IN] Platform Initialization parameters. If
 *                              NULL is passed pin mux setting will be done by
 *                              default.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    /* */
    Int32 Iss_platformInit(Iss_PlatformInitParams * platformInitPrms);

/**
 *  \brief De-init the underlying platform.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    Int32 Iss_platformDeInit(void);

/**
 *  \brief Init EVM related sub-systems like I2C instance.
 *
 *  \param deviceInitPrms     [IN] Device Initialization parameters. If NULL is
 *                            passed I2C will be initialized by default.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    Int32 Iss_platformDeviceInit(Iss_PlatformDeviceInitParams * deviceInitPrms);

/**
 *  \brief De-init EVM related sub-systems.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    Int32 Iss_platformDeviceDeInit(void);

/**
 *  \brief Returns the I2c instance Id.
 *
 *  \return I2C instance ID on success.
 */
    UInt32 Iss_platformGetI2cInstId(void);

/**
 *  \brief Returns EVM specific I2C address for different sensors (like
 *  Mt9j003 etc), connected to different video ports.
 *
 *  \param vidDecId     [IN] Video decoder ID
 *
 *  \param vipInstId    [IN] VIP Instance ID
 *
 *  \return I2C address on success
 */
    UInt8 Iss_platformGetSensorI2cAddr(UInt32 vidDecId, UInt32 vipInstId);

/**
 *  \brief Returns TRUE if the platform is EVM, else returns false.
 *
 *  \return TRUE if the platform is EVM, else FALSE
 */
    UInt32 Iss_platformIsEvm(void);

/**
 *  \brief Returns TRUE if the platform is simulator, else returns false.
 *
 *  \return TRUE if the platform is simulator, else FALSE
 */
    UInt32 Iss_platformIsSim(void);

/**
 *  \brief Probes for video device and return board ID
 *
 *  \return Board ID on success
 */
    Iss_PlatformBoardId Iss_platformGetBoardId(void);

/**
 *  \brief Selects video decoder when video decoder is muxed at board level.
 *
 *  \param vidDecId     [IN] Video decoder ID
 *
 *  \param vipInstId    [IN] VIP instance ID
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    Int32 Iss_platformSelectSensor(UInt32 vidDecId, UInt32 vipInstId);

/**
 *  \brief Returns the CPU revision.
 *
 *  \return CPU revision information on success.
 */
    Iss_PlatformCpuRev Iss_platformGetCpuRev(void);

/**
 *  \brief Returns the base board revision.
 *
 *  \return Base board revision information on success.
 */
    Iss_PlatformBoardRev Iss_platformGetBaseBoardRev(void);

/**
 *  \brief Returns the add-on or daughter card board revision.
 *
 *  \return Add-on or daughter card board revision information on success.
 */
    Iss_PlatformBoardRev Iss_platformGetDcBoardRev(void);

/**
 *  \brief Returns the platform ID.
 *
 *  \return Platform ID on success.
 */
    Iss_PlatformId Iss_platformGetId(void);

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _ISS_PLATFORM_H_ 
 *//* @} */
