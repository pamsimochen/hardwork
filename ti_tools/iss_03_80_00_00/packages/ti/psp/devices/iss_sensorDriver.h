/** ==================================================================
 *  @file   iss_sensorDriver.h                                                  
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
 * \ingroup ISS_DRV_FVID2_DEVICE_API
 * \defgroup ISS_DRV_FVID2_DEVICE_SENSOR_API External Video Decoder API
 *
 *  This modules define API to capture video data using external video
 *  decoders like TVP5158, TVP7002. Typically the external video decoders
 *  interface to the host via one of the VIP ports. I2C is used to send
 *  control commands to the video decoder.
 *
 *  User application controls and configures the video decoder
 *  and based on this in turn configures the VIP port.
 *
 *  The following video decoder's are currently supported
 *  - TVP5158 - Multi-CH NTSC/PAL video decoder
 *  - TVP7002 - Component HD video decoder
 *  - SII9135 - HDMI HD video decoder
 *
 *  The API interface used in the FVID2 interface (See also \ref ISS_DRV_FVID2_API)
 *
 *  The following FVID2 APIs are supported by video decoder device drivers,
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
        \ref FVID2_ISS_SENSOR_TVP5158_DRV <br>
        \ref FVID2_ISS_SENSOR_TVP7002_DRV <br>
        \ref FVID2_ISS_SENSOR_SII9135_DRV <br>
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
        Iss_VideoDecoderCreateParams *
        </td>
      </tr>
      <tr>
        <td>createStatusArgs</td>
        <td>
        Iss_VideoDecoderCreateStatus *
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
 See \ref ISS_DRV_FVID2_IOCTL_SENSOR for the list of IOCTLs supported by the driver. <br>
 All supported video decoders implement these IOCTLs. <br> <br>
   - TVP5158 supports further additional specific IOCTLs (See \ref ISS_DRV_FVID2_DEVICE_SENSOR_TVP5158_API)
   - SII9153 and TVP7002 do not support any additional specific IOCTLs.

Typicall API calling sequence for video decoder in context of video capture application would be like below
- FVID2_create() - create the video decoder device handle
- \ref IOCTL_ISS_SENSOR_GET_CHIP_ID - check if device is active
- \ref IOCTL_ISS_SENSOR_RESET  - reset it
- \ref IOCTL_ISS_SENSOR_SET_VIDEO_MODE - setup the device for a particular mode
- \ref IOCTL_ISS_SENSOR_GET_VIDEO_STATUS - check if video source is detected
- Create capture driver based on video decoder setup and video decoder status.
- Start capture driver
- \ref FVID2_start() - enable video decoder to start data output to VIP capture port
- Do video capture using capture driver.
- Stop capture driver
- \ref FVID2_stop() - disable video decoder device
- \ref FVID2_delete() - delete video decoder device
- Delete the capture driver

 *
 * @{
*/

/**
 *  \file iss_sensorDriver.h
 *
 *  \brief External Video Decoder API
*/

#ifndef _ISS_SENSOR_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_SENSOR_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/devices/iss_device.h>
#include <ti/psp/iss/iss_capture.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

/** \brief Max devices that can be associated with a single VIP port  */
#define ISS_SENSOR_DEV_PER_PORT_MAX        (2)

/** \brief Value to use, when it is needed to apply device specific default
  *        setting value
*/
#define ISS_SENSOR_DEFAULT                 ((UInt32)-1)

/** \brief Value to use, when it is needed to NOT change a device specific
  *        setting value
*/
#define ISS_SENSOR_NO_CHANGE               ((UInt32)-2)

/** \brief Value to use, when there no GPIO is used for device reset
*/
#define ISS_SENSOR_GPIO_NONE               ((UInt32)-1)

/**
  * \addtogroup ISS_DRV_FVID2_IOCTL_SENSOR
  * @{
*/

/**
  * \brief Get Chip ID
  *
  * This IOCTL can be used to get video decoder chip information
  * like chip number, revision, firmware/patch revision
  *
  *
  * \param cmdArgs       [IN]  Iss_SensorChipIdParams *
  * \param cmdArgsStatus [OUT] Iss_SensorChipIdStatus *
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_GET_CHIP_ID       \
            (ISS_SENSOR_IOCTL_BASE + 0x00)

/**
  * \brief Reset video decoder chip
  *
  * This IOCTL can be used to reset video decoder chip to power ON reset state
  *
  *
  * \param cmdArgs       [IN]   NULL
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_RESET             \
            (ISS_SENSOR_IOCTL_BASE + 0x01)

/**
  * \brief Configure video decoder video mode
  *
  * This IOCTL can be used to configure video decoder in a specific mode
  * like YUV output, 16-bit output, input standard selection, auto detection
  * of input standard and so on
  *
  * When standard is set as AUTO-DETECT, this API will wait internally until
  * video is detected (upto a timeout limit set by user).
  *
  * Once the API returns IOCTL_ISS_SENSOR_GET_VIDEO_STATUS can be used
  * to get the detected video standard information
  *
  * \param cmdArgs       [IN]  Iss_SensorVideoModeParams *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_SET_VIDEO_MODE    \
            (ISS_SENSOR_IOCTL_BASE + 0x02)

/**
  * \brief Get video status
  *
  * This IOCTL can be used to detect current video status like video standard
  * widthxheight, interlaced or progressive video etc
  *
  * Note, this API will not wait until video is detected. User can call this
  * API repeatedly if they want to wait until video is detected
  *
  * This API can be called independently for every channel in case of
  * multi-channel video decoders
  *
  * This API can be called only after calling
  * IOCTL_ISS_SENSOR_SET_VIDEO_MODE with appropriate parameters.
  *
  * \param cmdArgs       [IN]   Iss_SensorVideoStatusParams *
  * \param cmdArgsStatus [OUT]  Iss_SensorVideoStatus *
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_GET_VIDEO_STATUS  \
            (ISS_SENSOR_IOCTL_BASE + 0x03)

/**
  * \brief Set video color processing related parameters
  *
  * This IOCTL can be called separetly for each channel.
  *
  * \param cmdArgs       [IN]   Iss_VideoDecoderColorParams *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_SET_VIDEO_COLOR   \
            (ISS_SENSOR_IOCTL_BASE + 0x04)

/**
  * \brief Low level I2C register write
  *
  * \param cmdArgs       [IN]   Iss_VideoDecoderRegRdWrParams *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_REG_WRITE         \
            (ISS_SENSOR_IOCTL_BASE + 0x08)

/**
  * \brief Low level I2C register read
  *
  * \param cmdArgs       [IN]   Iss_VideoDecoderRegRdWrParams *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_ISS_SENSOR_REG_READ          \
            (ISS_SENSOR_IOCTL_BASE + 0x09)

#define IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN          \
            (ISS_SENSOR_IOCTL_BASE + 0x0A)

#define IOCTL_ISS_SENSOR_UPDATE_FRAMERATE          \
            (ISS_SENSOR_IOCTL_BASE + 0x0B)

#define IOCTL_ISS_SENSOR_FRAME_RATE_SET          \
            (ISS_SENSOR_IOCTL_BASE + 0x0C)

#define IOCTL_ISS_SENSOR_UPDATE_ITT          \
            (ISS_SENSOR_IOCTL_BASE + 0x0D)

#define IOCTL_ISS_SENSOR_UPDATE_CONFIG          \
            (ISS_SENSOR_IOCTL_BASE + 0x0E)

#define IOCTL_ISS_SENSOR_PWM_CONFIG          \
            (ISS_SENSOR_IOCTL_BASE + 0x0F)
 
#define IOCTL_ISS_SENSOR_WDR_MODE          \
            (ISS_SENSOR_IOCTL_BASE + 0x10)

/* @} */

/**
 *  \brief Video system
*/
typedef enum {
    ISS_SENSOR_VIDEO_SYSTEM_NTSC = 0,
  /**< Interlaced, NTSC system, 720x240 per field */

    ISS_SENSOR_VIDEO_SYSTEM_PAL,
  /**< Interlaced, PAL system, 720x288 per field */

    ISS_SENSOR_VIDEO_SYSTEM_AUTO_DETECT,
  /**< Auto-detect NTSC or PAL system */

    ISS_SENSOR_VIDEO_SYSTEM_NONE,
  /**< Neither NTSC nor PAL system */

    ISS_SENSOR_VIDEO_SYSTEM_MAX
  /**< Max video system */
} Iss_VideoDecoderVideoSystem;

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/**
  * \brief Arguments for Iss_Mt9j003UpdateItt()
*/
typedef struct {
    UInt16 Control;

    UInt16 regAddr;

    UInt16 regValue;

} Itt_RegisterParams;

/**
  * \brief Arguments for FVID2_create()
*/
typedef struct {

    UInt32 deviceI2cInstId;
  /**< I2C device instance ID to use 0 or 1 */

    UInt32 numDevicesAtPort;
  /**< Number of devices connected to a video port  */

    UInt32 deviceI2cAddr[ISS_SENSOR_DEV_PER_PORT_MAX];
  /**< I2C device address for each device */

    UInt32 deviceResetGpio[ISS_SENSOR_DEV_PER_PORT_MAX];
  /**< GPIO number that is used to reset the device
   *
   *   In case HW does not have any such GPIO then set this to
   *   ISS_SENSOR_GPIO_NONE
   */
    UInt32 vnfEnabled;

    UInt32 vstabEnabled;

    UInt32 InputStandard;

} Iss_SensorCreateParams;

/**
  * \brief Status of FVID2_create()
*/
typedef struct {

    Int32 retVal;
  /**< FVID2_SOK on success, else failure */

} Iss_SensorCreateStatus;

/**
 * \brief Arguments for \ref IOCTL_ISS_SENSOR_GET_CHIP_ID
*/
typedef struct {

    UInt32 deviceNum;
  /**< Device number for which chip ID is required
   *
   *   Device number will be 0 in most cases, unless multiple devices
   *   are cascaded together like in TVP5158 in which case device num
   *   can be from 0 .. Iss_VideoDecoderCreateParams.numDevicesAtPort-1
   */

} Iss_SensorChipIdParams;

/**
 * \brief Status for \ref IOCTL_ISS_SENSOR_GET_CHIP_ID
*/
typedef struct {

    UInt32 chipId;
  /**< Chip ID, value is device specific  */

    UInt32 chipRevision;
  /**< Chip revision, value is device specific  */

    UInt32 firmwareVersion;
  /**< Chip internal patch/firmware revision, value is device specific  */

} Iss_SensorChipIdStatus;

/**
  * \brief Arguments for \ref IOCTL_ISS_SENSOR_SET_VIDEO_MODE
*/
typedef struct {

    UInt32 videoIfMode;
  /**< 8 or 16 or 24-bit video interface mode.

     For valid values see #Iss_CaptVideoIfMode
   */

    UInt32 videoDataFormat;
  /**< RGB or YUV data format. valid values are given below \n
      FVID2_DF_YUV422P, ( 'P' is not relavent for input data format) \n
      FVID2_DF_YUV444P, ( 'P' is not relavent for input data format) \n
      FVID2_DF_RGB24_888.

      For valid values see #FVID2_DataFormat.
   */

    UInt32 standard;
  /**< Video decoder video standard
    *
    *  When set to FVID2_STD_AUTO_DETECT the video resolution
    *  will be auto-detected but user will still need to provide other
    *  parameters like videoIfMode, videoDataFormat, videoCaptureMode
    *
    *  For valid values see #FVID2_Standard
    */

    UInt32 videoCaptureMode;
  /**< Multiplexing mode, line mux or pixel mux or split line mode

       For valid values see #Iss_CaptVideoCaptureMode
  */

    UInt32 videoSystem;
  /**< Video system, NTSC or PAL or NONE or Auto-detect
    *
    *  When set to ISS_SENSOR_VIDEO_SYSTEM_AUTO_DETECT the video
    *  system NTSC or PAL will be auto-detected but user will
    *  still need to provide other parameters like videoIfMode,
    *  videoDataFormat, videoCaptureMode, standard
    *
    *  For valid values see #Iss_VideoDecoderVideoSystem
    */

    Int32 videoAutoDetectTimeout;
  /**< Auto-detect timeout, when auto-detecting standard in OS ticks
   *
   * Valid only when
   *
   * Iss_SensorVideoModeParams.standard
   *   = FVID2_STD_AUTO_DETECT
   *
   * OR
   *
   * Iss_SensorVideoModeParams.videoSystem
   *   = ISS_SENSOR_VIDEO_SYSTEM_AUTO_DETECT
   */

} Iss_SensorVideoModeParams;

/**
  * \brief Arguments for \ref IOCTL_ISS_SENSOR_GET_VIDEO_STATUS
*/
typedef struct {

    UInt32 channelNum;
  /**< Channel number for which status is needed
   *
   *   Channel number will be 0 in most cases, unless the device is capable of
   *   multi-channel capture like in TVP5158 in which case channel number
   *   can be from 0 .. Device specific max channels-1
   */

} Iss_SensorVideoStatusParams;

/**
  * \brief Status for \ref IOCTL_ISS_SENSOR_GET_VIDEO_STATUS
*/
typedef struct {

    UInt32 isVideoDetect;
  /**<  TRUE: Video signal is present, FALSE: no video signal */

    UInt32 frameWidth;
  /**< Frame or field width in pixels
   *
   * This is detected video signal frame or field width.
   *
   * Further change in width or height due to additional
   * cropping, scaling like CIF, HALF-D1 is not accoutned for in this field
  */

    UInt32 frameHeight;
  /**< Frame or field height in lines
   *
   * This is detected video signal frame or field height.
   *
   * Further change in width or height due to additional
   * cropping, scaling like CIF, HALF-D1 is not accoutned for in this field
  */

    UInt32 frameInterval;
  /**< Interval between two fields or frames in micro-seconds */

    UInt32 isInterlaced;
  /**< TRUE: Source is Interlaced, FALSE: Source is Progressive */

} Iss_SensorVideoStatus;

/**
 * \brief Arguments for \ref IOCTL_ISS_SENSOR_SET_VIDEO_COLOR
*/
typedef struct {

    UInt32 channelNum;
  /**< Channel number for which these parameters are to be applied
   *
   *   Channel number will be 0 in most cases, unless the device is capable of
   *   multi-channel capture like in TVP5158 in which case channel number
   *   can be from 0 .. Device specific max channels-1
   */

    UInt32 videoBrightness;
  /**< Video brightness, value is device specific
   *
   * This field can be set to \ref ISS_SENSOR_DEFAULT when device specific
   * default needs to be applied in current IOCTL call.
   *
   * This field can be set to \ref ISS_SENSOR_NO_CHANGE when last applied
   * value needs to be retained in current IOCTL call.
   *
   */

    UInt32 videoContrast;
  /**< Video contrast, value is device specific
   *
   * This field can be set to \ref ISS_SENSOR_DEFAULT when device specific
   * default needs to be applied in current IOCTL call.
   *
   * This field can be set to \ref ISS_SENSOR_NO_CHANGE when last applied
   * value needs to be retained in current IOCTL call.
   *
   */

    UInt32 videoSaturation;
  /**< Video saturation, value is device specific
   *
   * This field can be set to \ref ISS_SENSOR_DEFAULT when device specific
   * default needs to be applied in current IOCTL call.
   *
   * This field can be set to \ref ISS_SENSOR_NO_CHANGE when last applied
   * value needs to be retained in current IOCTL call.
   *
   */

    UInt32 videoSharpness;
  /**< Video sharpness, value is device specific
   *
   * This field can be set to \ref ISS_SENSOR_DEFAULT when device specific
   * default needs to be applied in current IOCTL call.
   *
   * This field can be set to \ref ISS_SENSOR_NO_CHANGE when last applied
   * value needs to be retained in current IOCTL call.
   *
   */

    UInt32 videoHue;
  /**< Video hue, value is device specific
   *
   * This field can be set to \ref ISS_SENSOR_DEFAULT when device specific
   * default needs to be applied in current IOCTL call.
   *
   * This field can be set to \ref ISS_SENSOR_NO_CHANGE when last applied
   * value needs to be retained in current IOCTL call.
   *
   */

} Iss_VideoDecoderColorParams;

/**
  * \brief Arguments for \ref IOCTL_ISS_SENSOR_REG_WRITE
  *        and \ref IOCTL_ISS_SENSOR_REG_READ
*/
typedef struct {

    UInt32 deviceNum;
  /**< Device from which to do I2C read/write
   *
   *   Device number will be 0 in most cases, unless multiple devices
   *   are cascaded together like in TVP5158 in which case device num
   *   can be from 0 .. Iss_VideoDecoderCreateParams.numDevicesAtPort-1
   */

    UInt32 numRegs;
  /**< Number of registers to read */

#ifdef CBB_PLATFORM
    UInt8 *regAddr8;
#else
    UInt8 *regAddr;
#endif
  /**< Array of register addresses */

    UInt8 *regValue8;
  /**< Array of register value to write or read back */

    UInt16 regAddr16;
  /**< Array of register addresses */

    UInt16 regValue16;
  /**< Array of register value to write or read back */

} Iss_VideoDecoderRegRdWrParams;

#define ISS_SENSOR_PWM_VIDEO 	(0)
#define ISS_SENSOR_PWM_DC 		(1)
typedef struct {
    UInt32 Id;
    UInt32 period;
    UInt32 duty;
} Iss_SensorPwmParm;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_SENSOR_H_ */

/* @} */
