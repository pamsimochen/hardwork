/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_DEVICE_API
 * \defgroup VPS_DRV_FVID2_DEVICE_VID_ENC_API External Video Encoder API
 *
 *  This modules define to configure and control external video encoders like
 *  HDMI transmitters. Typically the external video encoders
 *  interface to the host via one of the DVO ports. I2C is used to send
 *  control commands to the video encoder.
 *
 *  User application controls and configures the video encoder
 *  and based on this in turn configures the DVO ports.
 *
 *  The following video decoder's are currently supported
 *  - SII9022a - HDMI HD transmitter
 *
 *  The API interface used in the FVID2 interface (See also \ref VPS_DRV_FVID2_API)
 *
 *  The following FVID2 APIs are supported by video encoder device drivers,
 *
 *

 - <b> Creating the driver </b> - FVID2_create()
     <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>drvId</td>
        <td>
        \ref FVID2_VPS_VID_ENC_SII9022A_DRV <br>
        </td>
      </tr>
      <tr>
        <td>instanceId</td>
        <td> Set to 0
        </td>
      </tr>
      <tr>
        <td>createArgs</td>
        <td>
        Vps_VideoEncoderCreateParams *
        </td>
      </tr>
      <tr>
        <td>createStatusArgs</td>
        <td>
        Vps_VideoEncoderCreateStatus *
        </td>
      </tr>
      <tr>
        <td>cbParams</td>
        <td>
        NOT USED, Set to NULL.
        </td>
      </tr>
    </table>
    \ref FVID2_Handle returned by FVID2_create() is used in subsequent FVID2 APIs

  - <b> Deleting the driver </b> - FVID2_delete()
    <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>deleteArgs</td>
        <td>NOT USED, set to NULL</td>
      </tr>
    </table>

 - <b> Starting the driver </b> - FVID2_start()
    <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>cmdArgs</td>
        <td>NOT USED, set to NULL</td>
      </tr>
    </table>

 - <b> Stopping the driver </b> - FVID2_stop()
    <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>cmdArgs</td>
        <td>NOT USED, set to NULL</td>
      </tr>
    </table>

 - <b> Controlling the driver </b> - FVID2_control() <br>
 See \ref VPS_DRV_FVID2_IOCTL_VID_ENC for the list of IOCTLs supported by the driver. <br>
 All supported video encoders implement these IOCTLs. <br> <br>
   - SII9022A supports further additional specific IOCTLs (See \ref VPS_DRV_FVID2_DEVICE_VID_ENC_SII9022A_API)

 *
 * @{
*/

/**
 *  \file vps_videoEncoder.h
 *
 *  \brief External Video Encoder API
*/

#ifndef _VPS_VIDEO_ENCODER_H_
#define _VPS_VIDEO_ENCODER_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/devices/vps_device.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/**
  * \addtogroup VPS_DRV_FVID2_IOCTL_VID_ENC
  * @{
*/

/**
  * \brief Get Chip ID
  *
  * This IOCTL can be used to get video  encoder chip information
  * like chip number, revision, firmware/patch revision
  *
  *
  * \param cmdArgs       [IN/OUT]  Vps_VideoEncoderChipId *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_VIDEO_ENCODER_GET_CHIP_ID       \
            (VPS_VID_ENC_IOCTL_BASE + 0x00)

/**
  * \brief Configure HDMI
  *
  * This IOCTL can be used to configure HDMI for mode.
  *
  *
  * \param cmdArgs       [IN]  Vps_VideoEncoderConfigParams *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_VIDEO_ENCODER_SET_MODE       \
            (VPS_VID_ENC_IOCTL_BASE + 0x01)

/* @} */

/**
 * \brief Enum defining ID of the standard Modes.
 *
 *  Standard timinig parameters
 *  will be used if the standard mode id is used for configuring mode
 *  in the hdmi.
 */
typedef enum
{
    VPS_VIDEO_ENCODER_MODE_NTSC = 0,
    /**< Mode Id for NTSC */
    VPS_VIDEO_ENCODER_MODE_PAL,
    /**< Mode Id for PAL */
    VPS_VIDEO_ENCODER_MODE_1080P_60,
    /**< Mode Id for 1080p at 60fps mode */
    VPS_VIDEO_ENCODER_MODE_720P_60,
    /**< Mode Id for 720p at 60fps mode */
    VPS_VIDEO_ENCODER_MODE_1080I_60,
    /**< Mode Id for 1080I at 60fps mode */
    VPS_VIDEO_ENCODER_MODE_1080P_30,
    /**< Mode Id for 1080P at 30fps mode */
    VPS_VIDEO_ENCODER_MAX_MODE
    /**< This should be the last mode id */
} Vps_VideoEncoderOutputModeId;

/**
 * \brief Enum defining external or embedded sync mode.
 */
typedef enum
{
    VPS_VIDEO_ENCODER_EXTERNAL_SYNC,
    /**< HDMI in external sync mode i.e. H-sync and V-sync are external */
    VPS_VIDEO_ENCODER_EMBEDDED_SYNC,
    /**< Embedded sync mode */
    VPS_VIDEO_ENCODER_MAX_SYNC
    /**< This should be the last mode id */
} Vps_VideoEncoderSyncMode;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/**
 * \brief Structure for getting HDMI chip identification Id
 */
typedef struct
{
    UInt32                  chipId;
    /**< Chip ID, value is device specific */

    UInt32                  chipRevision;
    /**< Chip revision, value is device specific  */

    UInt32                  firmwareVersion;
    /**< Chip internal patch/firmware revision, value is device specific */

} Vps_VideoEncoderChipId;


/**
  * \brief Arguments for FVID2_create()
*/
typedef struct
{

    UInt32                          deviceI2cInstId;
    /**< I2C device instance ID to use 0 or 1 */
    UInt32                          deviceI2cAddr;
    /**< I2C device address for each device */
    UInt32                          inpClk;
    /**< input clock*/
    UInt32                          hdmiHotPlugGpioIntrLine;
    /**< HDMI hot plug GPIO interrupt line no */
    UInt32                          syncMode;
    /**< Sync Mode. See #Vps_VideoEncoderSyncMode for valid values */
    UInt32                          clkEdge;
    /**< Specifies the clock edge to be used to latch data on.
         FALSE spacifies to latch on falling edge, raising edge otherwise */
} Vps_VideoEncoderCreateParams;

/**
  * \brief Status of FVID2_create()
*/
typedef struct
{

    Int32   retVal;
    /**< FVID2_SOK on success, else failure */
} Vps_VideoEncoderCreateStatus;


/**
  * \brief configuration paramters for HDMI
*/
typedef struct
{
    Vps_VideoEncoderOutputModeId  ouputMode;
    /**< output mode of hdmi */

    Vps_VideoEncoderSyncMode      syncMode;
    /**< Select either embedded or external sync */

} Vps_VideoEncoderConfigParams;

#endif

/*@}*/

