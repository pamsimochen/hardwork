/** ==================================================================
 *  @file   iss_mt9j003.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \ingroup ISS_DRV_FVID2_DEVICE_VID_ENC_API
 * \defgroup ISS_DRV_FVID2_DEVICE_VID_ENC_MT9J003_API MT9J003 Video Encoder API
 *
 *  This modules define API specific to MT9J003 video encoder device.
 *
 * @{
*/

/**
 *  \file iss_mt9j003.h
 *
 *  \brief MT9J003 Sensor API
*/

#ifndef _ISS_Mt9j003_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_Mt9j003_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/devices/iss_sensorDriver.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */
/**
  * \addtogroup ISS_DRV_FVID2_IOCTL_MT9J003
  * @{
*/

/**
  * \brief Query hot plug detect (HPD) for HDMI cable connect or disconnect.
  *
  *
  * \param cmdArgs       [OUT]  Iss_HdmiChipId *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure. Status= TRUE means cable
  * connected else not connected
  *
*/
#define IOCTL_ISS_MT9J003_GET_DETAILED_CHIP_ID      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1000)

/**
  * \brief Query Hot Plug Detect status
  *
  *
  * \param cmdArgs       [OUT]   Pointer to UInt32 i.e. UInt32 * status
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_ISS_MT9J003_QUERY_HPD      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1001)

/**
  * \brief Set Configuration in HDMI 9022A
  *
  *
  * \param cmdArgs       [IN]   Pointer to Iss_Mt9j003Prms
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_ISS_MT9J003_SET_PARAMS      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1002)

/**
  * \brief Get Configuration in HDMI 9022A
  *
  *
  * \param cmdArgs       [IN]   Pointer to Iss_Mt9j003Prms
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_ISS_MT9J003_GET_PARAMS      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1003)

/**
 * \brief Enum defining output type
 */
typedef enum {
    ISS_MT9J003_HDMI = 0,
    /**< HDMI in external sync mode i.e. H-sync and V-sync are external */
    ISS_MT9J003_DVI,
    /**< Embedded sync mode */
} Iss_Mt9j003OutputType;

/* @} */

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/**
 * \brief Structure for getting HDMI chip identification Id.
 */
typedef struct {
    UInt32 deviceId;
    /**< Device Id TPI */
    UInt32 deviceProdRevId;
    /**< Device Production Revision Id TPI  */
    UInt32 tpiRevId;
    /**< TPI Revision Id TPI */
    UInt32 hdcpRevTpi;
    /**< HDCP revision TPI */
} Iss_HdmiChipId;

/**
 *  \brief Mode Information structure.
 *  This structure is used in setting mode in MT9J003 using
 *  IOCTL_ISS_VIDEO_ENCODER_SET_MODE IOCTL.
 */
typedef struct {
    UInt32 standard;
    /**< Video standard. For valid values see #FVID2_Standard. */
} Iss_Mt9j003ModeParams;

/**
 * \brief Structure for hot plug detection parameters.
 *
 * It is used to get
 * the hpd parameters using IOCTL_ISS_MT9J003_QUERY_HPD ioctl.
 */
typedef struct {
    UInt32 hpdEvtPending;
    /**< Hot Plug Connection Event Pending */
    UInt32 busError;
    /**< Receiver Sense Event Pending or CTRL Bus Error */
    UInt32 hpdStatus;
    /**< Hot Plug Pin Current Status */
} Iss_Mt9j003HpdPrms;

typedef struct {
    UInt32 outputType;
    /**< Type of the OUTPUT i.e. HDMI of DVI #Iss_Mt9j003OutputType */
} Iss_Mt9j003Prms;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
