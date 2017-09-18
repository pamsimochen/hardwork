/** ==================================================================
 *  @file   iss_ov2710.h
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
 * \defgroup ISS_DRV_FVID2_DEVICE_VID_ENC_OV2710_API OV2710 Video Encoder API
 *
 *  This modules define API specific to OV2710 video encoder device.
 *
 * @{
*/

/**
 *  \file iss_ov2710.h
 *
 *  \brief OV2710 Sensor API
*/

#ifndef _ISS_OV2710_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_OV2710_H_

/* ========================================================================== 
 */
/*                             Include Files                                  */
/* ========================================================================== 
 */

#include <ti/psp/devices/iss_sensorDriver.h>

/* ========================================================================== 
 */
/*                           Macros & Typedefs                                */
/* ========================================================================== 
 */
/**
  * \addtogroup ISS_DRV_FVID2_IOCTL_OV2710
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
#define IOCTL_ISS_OV2710_GET_DETAILED_CHIP_ID      \
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
#define IOCTL_ISS_OV2710_QUERY_HPD      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1001)

/**
  * \brief Set Configuration in HDMI 9022A
  *
  *
  * \param cmdArgs       [IN]   Pointer to Iss_Ov2710Prms
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_ISS_OV2710_SET_PARAMS      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1002)

/**
  * \brief Get Configuration in HDMI 9022A
  *
  *
  * \param cmdArgs       [IN]   Pointer to Iss_Ov2710Prms
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure.
  *
*/
#define IOCTL_ISS_OV2710_GET_PARAMS      \
            (ISS_VID_ENC_IOCTL_BASE + 0x1003)


/**
 * \brief Enum defining output type
 */
typedef enum
{
    ISS_OV2710_HDMI = 0,
    /**< HDMI in external sync mode i.e. H-sync and V-sync are external */
    ISS_OV2710_DVI,
    /**< Embedded sync mode */
} Iss_Ov2710OutputType;

/* @} */

/* ========================================================================== 
 */
/*                         Structure Declarations                             */
/* ========================================================================== 
 */

/**
 * \brief Structure for getting HDMI chip identification Id.
 */
typedef struct
{
    UInt32                  deviceId;
    /**< Device Id TPI */
    UInt32                  deviceProdRevId;
    /**< Device Production Revision Id TPI  */
    UInt32                  tpiRevId;
    /**< TPI Revision Id TPI */
    UInt32                  hdcpRevTpi;
    /**< HDCP revision TPI */
} Iss_HdmiChipId;

/**
 *  \brief Mode Information structure.
 *  This structure is used in setting mode in OV2710 using
 *  IOCTL_ISS_VIDEO_ENCODER_SET_MODE IOCTL.
 */
typedef struct
{
    UInt32                  standard;
    /**< Video standard. For valid values see #FVID2_Standard. */
} Iss_Ov2710ModeParams;

/**
 * \brief Structure for hot plug detection parameters.
 *
 * It is used to get
 * the hpd parameters using IOCTL_ISS_OV2710_QUERY_HPD ioctl.
 */
typedef struct
{
    UInt32                  hpdEvtPending;
    /**< Hot Plug Connection Event Pending */
    UInt32                  busError;
    /**< Receiver Sense Event Pending or CTRL Bus Error */
    UInt32                  hpdStatus;
    /**< Hot Plug Pin Current Status */
} Iss_Ov2710HpdPrms;

typedef struct
{
    UInt32 outputType;
    /**< Type of the OUTPUT i.e. HDMI of DVI #Iss_Ov2710OutputType */
} Iss_Ov2710Prms;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
