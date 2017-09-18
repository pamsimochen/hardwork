/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_CAPTURE_API Capture API
 *
 *  This modules define APIs to capture video data using VIP ports in HD-VPSS.
 *  This module can be used for single channel capture as well as multi-channel capture.
 *
 *  Capture driver supports the following FVID2 APIs (see also \ref VPS_DRV_FVID2_API)
 *
 - <b> Creating the driver </b> - FVID2_create()
     <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>drvId</td>
        <td>\ref FVID2_VPS_CAPT_VIP_DRV</td>
      </tr>
      <tr>
        <td>instanceId</td>
        <td>
            \ref VPS_CAPT_INST_VIP0_PORTA <br>
            \ref VPS_CAPT_INST_VIP0_PORTB <br>
            \ref VPS_CAPT_INST_VIP1_PORTA <br>
            \ref VPS_CAPT_INST_VIP1_PORTB <br>
            \ref VPS_CAPT_INST_VIP_ALL  <br>
        </td>
      </tr>
      <tr>
        <td>createArgs</td>
        <td>
        Vps_CaptCreateParams *
        </td>
      </tr>
      <tr>
        <td>createStatusArgs</td>
        <td>
        Vps_CaptCreateStatus *
        </td>
      </tr>
      <tr>
        <td>cbParams</td>
        <td>
        FVID2_CbParams *

        When FVID2_CbParams.cbFxn is set, FVID2_CbParams.cbFxn
        gets called based on value of Vps_CaptCreateParams.periodicCallbackEnable

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
 See \ref VPS_DRV_FVID2_IOCTL_CAPTURE for the list of IOCTLs supported by the driver

 - <b> Getting captured frames from the driver </b> - FVID2_dequeue()
    <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>frameList</td>
        <td>
        [OUT] FVID2_FrameList.numFrames returns the number of captured frames returned in this FVID2_dequeue() invocation. <br>
        [OUT] FVID2_FrameList.frames[0..FVID2_FrameList.numFrames-1] are the captured FVID2_Frame pointers to the captured frames. <br>
        [OUT] For each FVID2_Frame, FVID2_Frame.perFrameCfg points to Vps_CaptRtParams that was set during FVID2_queue()  <br>
        </td>
      </tr>
      <tr>
        <td>streamId</td>
        <td>
        Value can be from 0 .. Vps_CaptCreateParams.numStream-1
        </td>
      </tr>
      <tr>
        <td>timeout</td>
        <td>
        Must be BIOS_NO_WAIT
        </td>
      </tr>
    </table>

 - <b> Releasing used frames back to the driver </b> - FVID2_queue() <br>
 Also used to queue initial frame buffers to the driver, before calling FVID2_start().
   <table border="1">
      <tr>
        <th>Parameter</th>
        <th>Value</th>
      </tr>
      <tr>
        <td>frameList</td>
        <td>
        [IN] FVID2_FrameList.numFrames sets the number of captured frames given back to driver in this FVID2_queue() invocation. <br>
        [IN] FVID2_FrameList.frames[0..FVID2_FrameList.numFrames-1] are the captured FVID2_Frame pointers that are being returned. <br>
        [IN] For each FVID2_Frame, FVID2_Frame.perFrameCfg points to Vps_CaptRtParams.
              This structure is filled and returned when FVID2_dequeue() is called  <br>
        </td>
      </tr>
      <tr>
        <td>streamId</td>
        <td>
        Value can be from 0 .. Vps_CaptCreateParams.numStream-1 OR \ref VPS_CAPT_STREAM_ID_ANY
        </td>
      </tr>
    </table>

 * @{
*/

/**
 *  \file vps_capture.h
 *
 *  \brief VIP Capture API
*/

#ifndef _VPS_CAPTURE_H
#define _VPS_CAPTURE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
    \name Capture Instance ID's

    @{
*/

/** \brief Capture instance - VIP0 - Port A  */
#define VPS_CAPT_INST_VIP0_PORTA (0u)

/** \brief Capture instance - VIP0 - Port B  */
#define VPS_CAPT_INST_VIP0_PORTB (1u)

/** \brief Capture instance - VIP1 - Port A  */
#define VPS_CAPT_INST_VIP1_PORTA (2u)

/** \brief Capture instance - VIP1 - Port B  */
#define VPS_CAPT_INST_VIP1_PORTB (3u)

/** \brief Capture instance - VIP ALL  */
#define VPS_CAPT_INST_VIP_ALL    (0xFFu)

/* @} */

/**
    \name Capture Max Limits

    @{
*/

/** \brief Capture instance - MAX instances  */
#define VPS_CAPT_INST_MAX        (4u)

/** \brief Maximun channels that can be captured per port */
#define VPS_CAPT_CH_PER_PORT_MAX       (16u)

/** \brief Maximun channels that can be captured in the system */
#define VPS_CAPT_CH_MAX       (VPS_CAPT_CH_PER_PORT_MAX*VPS_CAPT_INST_MAX)

/** \brief Maximum frames that can be queued per channel */
#define VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX  (16u)

/** \brief Maximum stream's that can be captured per channel

  Stream's corresponds to different types of output that are possible
  for a given capture input source.

  The stream output type is setup by user during FVID2_create()

  Example streams are,

  For a 1080P input source,
  Stream 0: YUV420 non-scaled output
  Stream 1: YUV422 scaled output
  Stream 3: RAW VBI output

  Refer to user guide for more details about what valid stream
  combinations are possible.
*/
#define VPS_CAPT_STREAM_ID_MAX (4u)

/* @} */

/** \brief Value to use for 'streamId' when required to do FVID2_queue
 to any stream

 Note, this value cannot be used as 'streamId' with FVID2_dequeue
*/
#define VPS_CAPT_STREAM_ID_ANY (0xFFFFu)

/**
 *  \brief Scalar ID to be used in the set scalar coefficient and set/get
 *  advance scalar params IOCTLS.
 *  Since this driver supports only one scalar per driver instance, the scalar
 *  could be identified by the instance handle. The below macro is used for
 *  initializing the scalar ID to a default value.
 */
#define VPS_CAPT_SCALAR_ID_DEFAULT      (0u)

/**
 *  \brief Function prototype, to determine the time stamp.
 *
 *  \param args  [IN]  Pointer to FVID2_Frame, for which the capture has just
 *                          completed.
 *                     CAUTION : Driver also use this function to determine
 *                      time, when debug of TS is enabled. In which case 
 *                      applications should ignore such case.
 *
 *  \return An unsigned int 32 value, that represents the current time.
 */
typedef UInt32 (*Vps_CaptFrameTimeStamp) (Ptr args);


/* Capture IOCTL's  */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_CAPTURE
  @{
*/

/**
  * \brief Set scaler frame info
  *
  * This IOCTL is valid only when Scaler is enabled during create time
  * This IOCTL can be used for features like Down scaling, Pan, Crop
  *
  * Note, upscaling is not supported for capture path
  *
  * \param cmdArgs       [IN]  Vps_CaptScParams *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_CAPT_SET_SC_PARAMS       (VPS_CAPT_IOCTL_BASE + 0x0000u)

/**
  * \brief Set frame skip info
  *
  * This IOCTL can be used to control capture frame drop sequence,
  * Example, this IOCTL can be used to convert 60fps input source to 30fps
  * by dropping frames.
  *
  * This control can be done independently for each stream, channel.
  *
  * \param cmdArgs       [IN]  Vps_CaptFrameSkip *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_CAPT_SET_FRAME_SKIP      (VPS_CAPT_IOCTL_BASE + 0x0001u)

/**
  * \brief Get channel status
  *
  * This IOCTL can be used to get status about a specific channel, stream
  *
  * This control can be done independently for each stream, channel.
  *
  * \param cmdArgs       [IN]  Vps_CaptChGetStatusArgs *
  * \param cmdArgsStatus [OUT] Vps_CaptChStatus *
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_CAPT_GET_CH_STATUS       (VPS_CAPT_IOCTL_BASE + 0x0002u)


/**
 * \brief Reset VIP0
 *
 * IMPORTANT: Only applicable to \ref VPS_CAPT_INST_VIP_ALL handle
 *
 * This IOCTL executes the VIP reset sequence for VIP0.
 *
 * This will reset both Port A as well as Port B for that VIP instance.
 *
 * This will reset all the VIP blocks including CSC, SC, CHR_DS
 * for that VIP instance.
 *
 * Make sure no module is being accessed in that VIP instance,
 * either in M2M path or capture path when this API is called.
 *
 * This API MUST be called in the following scenarios
 * - when in non-mux capture mode AND
 * - CHR_DS OR SC or CSC is enabled in capture driver path.
 *
 * If this API is not called in above scenarios then capture driver
 * may get hung up.
 *
 * This API must be called each time before starting capture in above
 * scenario's.
 *
 * This API need not be called in the following scenarios
 * - when in non-mux capture mode or multi-CH capture mode AND
 * - SC is disabled, CSC is disabled, CHR_DS is disabled in capture driver path.
 *
 * \param cmdArgs       [IN]  NULL
 * \param cmdArgsStatus [OUT] NULL
 *
 * \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_RESET_VIP0          (VPS_CAPT_IOCTL_BASE + 0x0003u)

/**
 * \brief Reset VIP1
 *
 * Same as \ref IOCTL_VPS_CAPT_RESET_VIP0 except that it operates on VIP1
 *
 * \param cmdArgs       [IN]  NULL
 * \param cmdArgsStatus [OUT] NULL
 *
 * \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_RESET_VIP1          (VPS_CAPT_IOCTL_BASE + 0x0004u)

/**
 * \brief Print detailed capture information
 *
 * IMPORTANT: Only applicable to \ref VPS_CAPT_INST_VIP_ALL handle
 *
 * This is meant to be used by driver developer for internal debugging purposes
 * It should not be used by user.
 *
 * \param cmdArgs    [IN] UInt32 Execution time in msecs
 *
 * \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_PRINT_ADV_STATISTICS (VPS_CAPT_IOCTL_BASE + 0x0005u)

/**
 *  \brief Check VIP0, VIP1 overflow status and return the status
 *
 *  \param cmdArgs       [IN]  NULL
 *  \param cmdArgsStatus [OUT] Vps_CaptOverFlowStatus *
 *
 *  \return FVID2_SOK
 */
#define IOCTL_VPS_CAPT_CHECK_OVERFLOW       (VPS_CAPT_IOCTL_BASE + 0x0006u)

/**
 *  \brief If the overflow flag is set, resets VIP
 *
 *  This API does the following
 *  If VIP output FIFO is overflowed
 *    - then its stops the driver on PortA, PortB
 *    - reset the VIP instance
 *    - starts driver of PortA, PortB
 *
 *  \param cmdArgs       [IN]  const Vps_CaptOverFlowStatus *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID2_SOK
 */
#define IOCTL_VPS_CAPT_RESET_AND_RESTART    (VPS_CAPT_IOCTL_BASE + 0x0007u)

/**
 *  \brief Flush capture driver and dequeue all frames including those that are not captured
 *
 *  This API can be called only when driver is in stopped state.
 *  Driver is in stopped state when,
 *  - FIVD2_stop() is called
 *  - FVID2_create() is called and FVID2_start() is not called.
 *
 *  This will return frames from all streams for a given capture handle.
 *  If this IOCTL is called with global handle then this will return
 *  frames from all capture handles.
 *
 *  Since maximum FVID2_MAX_FVID_FRAME_PTR frames can be returned at a time.
 *  This function should be called in a loop until FVID2_FrameList.numFrames = 0
 *  is returned in order to get back all the frames from the capture driver.
 *
 *  In case capture is in running state this function will return error.
 *
 *  This IOCTL will return capture frames, non-captured frames, as well as all frames
 *  held inside the hardware.
 *
 *  \param cmdArgs       [IN]  NULL
 *  \param cmdArgsStatus [OUT] FVID2_FrameList
 *
 *  \return FVID2_SOK
 */
#define IOCTL_VPS_CAPT_FLUSH                 (VPS_CAPT_IOCTL_BASE + 0x0008u)

/**
 * \brief User provided buffer to be used by driver when dropping data
 *
 * WARNING: Experimental IOCTL for test. Users should NOT call this IOCTL.
 *
 *  \param cmdArgs       [IN]  Void * pointer to data buffer
 *
 *  \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_DROP_DATA_BUFFER      (VPS_CAPT_IOCTL_BASE + 0x0009u)

/**
 * \brief Reset VIP Port
 *
 * IMPORTANT: Cannot be used with ref \ref VPS_CAPT_INST_VIP_ALL handle
 *
 * This IOCTL executes the VIP reset sequence for VIP Port associated
 * with the given handle.
 *
 * This will reset all the VIP blocks including CSC, SC, CHR_DS used
 * for that VIP instance.
 *
 * \param cmdArgs       [IN]  NULL
 * \param cmdArgsStatus [OUT] NULL
 *
 * \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_RESET_VIP           (VPS_CAPT_IOCTL_BASE + 0x000Au)

/**
 * \brief Field or Frame capture for interlaced input.
 * This IOCTL allows user to capture fields or frames for interlaced input.
 * For frame capture fields can be line interleaved or it can be
 * separate. This IOCTL is only valid
 *  \param cmdArgs          [IN]  const Vps_CaptStorageParams *
 *  \param cmdArgsStatus    [OUT] NULL
 *
 *  \return FVID_SOK        return FVID_SOK on success else a proper failure
 *                          FVID2 failure code.
 */
#define IOCTL_VPS_CAPT_SET_STORAGE_FMT      (VPS_CAPT_IOCTL_BASE + 0x000Bu)

/**
 * \brief Field or Frame capture for interlaced input.
 * This IOCTL allows user to capture fields or frames for interlaced input.
 * For frame capture fields can be line interleaved or it can be
 * separate. This IOCTL is only valid
 *  \param cmdArgs          [IN]  Vps_CaptStorageParams *
 *  \param cmdArgsStatus    [OUT] NULL
 *
 *  \return FVID_SOK        return FVID_SOK on success else a proper failure
 *                          FVID2 failure code.
 */
#define IOCTL_VPS_CAPT_GET_STORAGE_FMT      (VPS_CAPT_IOCTL_BASE + 0x000Cu)

/**
 * \brief Capture driver queues back error frame to driver empty queue and
 * does not return err frame to application. With this IOCTL capture driver
 * will return err frame to application with error status flagged. Instead of
 * driver queing it back to empty queue
 *  \param cmdArgs          [IN]  const enable *
 *  \param cmdArgsStatus    [OUT] NULL
 *
 *  \return FVID_SOK        return FVID_SOK on success else a proper failure
 *                          FVID2 failure code.
 */
#define IOCTL_VPS_CAPT_RETURN_ERR_FRM      (VPS_CAPT_IOCTL_BASE + 0x000Du)

/**
 * \brief Capture driver gives call back continously till the frame is dequeued
 * from the output queue. This ioctl allows application to have callback once
 * per completed frame. This will ensure that application will receive N call
 * backs for N queued frames and not more than that.  Application will have
 * to call N time dequeue for N call backs This functionality is
 * required by high level operation system like V4L2 capture.
 *  \param cmdArgs          [IN]  const Vps_CaptOneCallBackPerFrm *
 *  \param cmdArgsStatus    [OUT] NULL
 *
 *  \return FVID_SOK        return FVID_SOK on success else a proper FVID2
 *                          failure code.
 */
#define IOCTL_VPS_CAPT_ONE_CB_PER_FRM      (VPS_CAPT_IOCTL_BASE + 0x000Eu)

/**
 *  \brief Sets the VIP parser crop configuration for a channel.
 *
 *  This IOCTL allows the user to configure the VIP parser crop feature for a
 *  port.
 *  This is only available for TI816x ES 2.0 and TI814x ES2.1 versions or above.
 *
 *  \param cmdArgs          [IN]  const Vps_CaptVipCropParams *
 *  \param cmdArgsStatus    [OUT] NULL
 *
 *  \return Returns FVID_SOK on success else a proper failure FVID2
 *  failure code.
 */
#define IOCTL_VPS_CAPT_SET_VIP_CROP_CFG     (VPS_CAPT_IOCTL_BASE + 0x000Fu)

/**
 *  \brief Gets the VIP parser crop configuration for a channel.
 *
 *  This IOCTL allows the user to get the already set VIP parser crop
 *  parameters of a port.
 *  This is only available for TI816x ES 2.0 and TI814x ES2.1 versions or above.
 *
 *  \param cmdArgs          [IN]  Vps_CaptVipCropParams *
 *  \param cmdArgsStatus    [OUT] NULL
 *
 *  \return Returns FVID_SOK on success else a proper failure FVID2
 *  failure code.
 */
#define IOCTL_VPS_CAPT_GET_VIP_CROP_CFG     (VPS_CAPT_IOCTL_BASE + 0x0010u)

 
/**
 * \brief Disables VIP Ports on Overflow
 *
 * This IOCTL disable or not to disable VIP Ports based on the cmdArgs
 * parameter whenever Overflow occurs
 * 
 * By default capture driver disables the VIP Ports whenever overflow occurs
 *
 * So Application is supposed to call reset and restart IOCTL for 
 * all platforms (TI816x ,TI814x and TI813x) if not capture driver
 * may get hung up.
 *
 *  Note: This IOCTL applies to the global handle as this is used across
 *  all handles.
 *
 * \param cmdArgs       [IN]  UInt32 Flag to disable or not to disable VIP ports
 *                                   on Overflow
 * \param cmdArgsStatus [OUT] NULL
 *
 * \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_DISABLE_PORT_ON_OVERFLOW  (VPS_CAPT_IOCTL_BASE + 0x0014u)

/**
 * \brief Configure the time stamping of captured frames.
 *
 * This IOCTL could be used to configure the method to be used for time stamping
 *  of captured frames.
 *
 * By default capture driver uses approximation to time stamp the captured frame
 *  if desired, applicaitons could re-configure capture driver to use besteffort
 *  method for time stamping.
 *
 * Best Effort method uses interrupts to determine end of captured frame and
 *  time stamps the frame in the ISR.
 *
 * \param cmdArgs       [IN]  A pointer of type Vps_CaptFrameTimeStampParms.
 *
 * \return FVID2_SOK
*/
#define IOCTL_VPS_CAPT_CFG_TIME_STAMPING_FRAMES (VPS_CAPT_IOCTL_BASE + 0x0015u)

/* @} */

/**
 * \brief Video capture operation mode
*/
typedef enum
{
    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC = 0,
    /**< Single Channel non multiplexed mode */
    VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC,
    /**< Multi-channel line-multiplexed mode */
    VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC,
    /**< Multi-channel pixel muxed */
    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK,
    /**< Single Channel non multiplexed discrete sync mode with HSYNC and
        VBLK as control signals. */
    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC,
    /**< Single Channel non multiplexed discrete sync mode with HSYNC and
        VSYNC as control signals. */
    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK,
    /**< Single Channel non multiplexed discrete sync mode with ACTVID and
        VBLK as control signals. */
    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC,
    /**< Single Channel non multiplexed discrete sync mode with ACTVID and
        VBLK as control signals. */
    VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_SPLIT_LINE_EMBEDDED_SYNC,
    /**< Multi-channel line-multiplexed mode - split line mode */
    VPS_CAPT_VIDEO_CAPTURE_MODE_MAX
    /**< Maximum modes */
} Vps_CaptVideoCaptureMode;

/**
 * \brief Video interface mode
 */
typedef enum
{
    VPS_CAPT_VIDEO_IF_MODE_8BIT = 0,
    /**< Embedded sync mode:  8bit - BT656 standard  */
    VPS_CAPT_VIDEO_IF_MODE_16BIT,
    /**< Embedded sync mode:  16bit - BT1120 standard  */
    VPS_CAPT_VIDEO_IF_MODE_24BIT,
    /**< Embedded sync mode:  24bit */
    VPS_CAPT_VIDEO_IF_MODE_MAX
    /**< Maximum modes */
} Vps_CaptVideoIfMode;

/**
    \brief Output data max width

    This enum can be used to limit the width of the output data that is captured in to the
    user buffer.

    Any additional pixels coming from the source are discarded.
*/
typedef enum
{
    VPS_CAPT_MAX_OUT_WIDTH_UNLIMITED = 0,
    /**< Do not limit the output width, captured whatever is coming from the source */
    VPS_CAPT_MAX_OUT_WIDTH_352_PIXELS = 4,
    /**< Limit height to 352 pixels */
    VPS_CAPT_MAX_OUT_WIDTH_768_PIXELS = 5,
    /**< Limit height to 768 pixels */
    VPS_CAPT_MAX_OUT_WIDTH_1280_PIXELS = 6,
    /**< Limit height to 1280 pixels */
    VPS_CAPT_MAX_OUT_WIDTH_1920_PIXELS = 7
    /**< Limit height to 1920 pixels */
} Vps_CaptMaxOutWidth;

/**
    \brief Output data max height

    This enum can be used to limit the height of the output data that is captured in to the
    user buffer.

    By default HW will capture whatever active data comes from the external video source.
    The width typically remains fixed from the external source. However, in some cases
    external video sources may generate spurious frames of active data which have random height.
    This can cause user buffers to overflow if the height of such a frame is more that
    the allocated buffer height.

    To avoid such buffer overflow, users should limit the maximum output data height.
    This a HW feature and limits that can be set are a few fixed values as shown below.

    Any additional lines coming from the source are discarded.
*/
typedef enum
{
    VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED = 0,
    /**< Do not limit the output width, captured whatever is coming from the source */
    VPS_CAPT_MAX_OUT_HEIGHT_288_LINES = 4,
    /**< Limit height to 288 lines */
    VPS_CAPT_MAX_OUT_HEIGHT_576_LINES = 5,
    /**< Limit height to 576 lines */
    VPS_CAPT_MAX_OUT_HEIGHT_720_LINES = 6,
    /**< Limit height to 720 lines */
    VPS_CAPT_MAX_OUT_HEIGHT_1080_LINES = 7
    /**< Limit height to 1080 lines */
} Vps_CaptMaxOutHeight;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Capture window information
 *
 *  Only valid when Vps_CaptOutInfo.scEnable is or can be TRUE
 *
 *  Note, upscaling is not supported for capture path
 */
typedef struct
{
    UInt32                  inWidth;
    /**< Input source width, MUST be >= actual or expected
         video source input width */
    UInt32                  inHeight;
    /**< Input source height, MUST be >= actual or expected
         video source input width

         Height should be field height in case source is interlaced \n
         Height should be frame height in case source is progressive \n
     */
    UInt32                  outWidth;
    /**< Scaler output width, in pixels */
    UInt32                  outHeight;
    /**< Scaler output height, in lines */
    Vps_CropConfig          inCropCfg;
    /**< Scalar input crop config */
    UInt32                  inScanFormat;
    /**< Input source scan format - interlaced or progressive.
         For valid values see #FVID2_ScanFormat. */
    Vps_ScConfig           *scConfig;
    /**< Scaler config, set NULL to setup default parameters */
    Vps_ScCoeffParams      *scCoeffConfig;
    /**< Scaler co-eff config, set NULL to setup default co-effs */
    UInt32                  enableCoeffLoad;
    /**< Enable scaler coefficient load during IOCTL_VPS_CAPT_SET_SC_PARAMS
         If this is set to TRUE, it may result in the VIP instance getting
         stopped, reset, and restarted to load new coefficients as per the
         provided new scaling factor. This may result in some frame loss.
         The scaler coefficients are loaded only if there is a change in either
         the horizontal or vertical scaling set. The best scaler coefficients to
         be used are determined internally when scCoeffConfig in this structure
         is set to NULL.
         If the user has provided scaler coefficients or coefficient sets to be
         used, these are used instead of internally calculating the best scaler
         coefficients. */
} Vps_CaptScParams;

/**
 *  \brief Capture output format information
 */
typedef struct
{
    UInt32                  dataFormat;
    /**< Output Data format, valid options are
         FVID2_DF_YUV422I_YUYV,
         FVID2_DF_YUV420SP_UV,
         FVID2_DF_YUV422SP_UV,
         FVID2_DF_RGB24_888,
         FVID2_DF_RAW_VBI.
         For valid values see #FVID2_DataFormat.

         If FVID2_DF_YUV422SP_UV is used as output format, it must
         be the first output format (output format at the
         index 0 in outStreamInfo in #Vps_CaptCreateParams). */
    UInt32                  memType;
    /**< Tiled or non-tiled memory selection for output.
         For valid values see #Vps_VpdmaMemoryType.

         For FVID2_DF_YUV422I_YUYV, FVID2_DF_RGB24_888, FVID2_DF_RAW_VBI
         output, always VPS_VPDMA_MT_NONTILEDMEM will be used internally.

         User can control tiled or non-tiled for FVID2_DF_YUV420SP_UV,
         FVID2_DF_YUV422SP_UV */
    UInt32                  pitch[FVID2_MAX_PLANES];
    /**< Pitch in bytes between two lines.
         Pitch can be specified separately for every plane. */
    UInt32                  maxOutWidth;
    /**< Set limit on the max possible width of the output frame
         For valid values see #Vps_CaptMaxOutWidth */
    UInt32                  maxOutHeight;
    /**< Set limit on the max possible height of the output frame
         For valid values see #Vps_CaptMaxOutHeight */
    UInt32                  scEnable;
    /**< TRUE: Use scaler before writing video data to memory,
         FALSE: Scaler is not used in capture path
         MUST be FALSE for line-multiplexed, pixel multiplexed modes  */
    UInt32                  subFrameModeEnable;
    /**< TRUE: subframe based capture is enabled
         FALSE: subframe based capture is disabled
         Must be FALSE for multi-channel modes.
         Sub frame based capture is not YET supported. This flag has to be set
         to FALSE for normal operations */
    UInt32                  numLinesInSubFrame;
    /**< Number of lines per subframes for this output stream

         MUST be multiple of the stream output size.
         Not valid, ignored for ancillary data stream

         SubFrame callback gets called after every numLinesInSubFrame
         for every output stream, except ancillary data stream

         Ignored when subFrameModeEnable = FALSE */
    FVID2_SubFrameCbFxn     subFrameCb;
    /**< SubFrame callback, is called for every subframe of this
         output stream
         Ignored when subFrameModeEnable = FALSE */
} Vps_CaptOutInfo;

/**
 *  \brief Capture driver create arguments, used when calling FVID2_create()
 */
typedef struct
{

    UInt32                  videoCaptureMode;
    /**< Video capture mode. For valid values see #Vps_CaptVideoCaptureMode */

    UInt32                  videoIfMode;
    /**< Video interface mode. For valid values see #Vps_CaptVideoIfMode */

    UInt32                  inDataFormat;
    /**< Input source color data format, valid values are given below \n
         FVID2_DF_YUV422P, ( 'P' is not relavent for input data format) \n
         FVID2_DF_YUV444P, ( 'P' is not relavent for input data format) \n
         FVID2_DF_RGB24_888.
         For valid values see #FVID2_DataFormat. */
    UInt32                  periodicCallbackEnable;
    /**< TRUE: User callback passed during FVID2 create is called periodically
               at a fixed duration of about 8msecs
         FALSE: User callback passed during FVID2 create is called only
                if one or more frames are captured in any of the streams,
                channels belonging to this handle */
    UInt32                  numCh;
    /**< Number of channel for multi-channel modes,
         Must be 1 for
         VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC */
    UInt32                  numStream;
    /**< Number of streams to capture, MUST be <= VPS_CAPT_STREAM_ID_MAX  */
    Vps_CaptOutInfo         outStreamInfo[VPS_CAPT_STREAM_ID_MAX];
    /**< Output format for each stream */
    Vps_CaptScParams        scParams;
    /**< Scaler parameters to use when
         Vps_CaptCreateParams.outStreamInfo[x].scEnable = TRUE

        Parameters are ignored when outStreamInfo[x].scEnable = FALSE */
    Vps_VipConfig          *vipParserInstConfig;
    /**< VIP Parser instance config, set NULL to ignore these parameters */
    Vps_VipPortConfig      *vipParserPortConfig;
    /**< VIP Parser port config, set NULL to ignore these parameters */
    Vps_CscConfig          *cscConfig;
    /**< CSC config, set NULL to ignore these parameters */
    UInt32                  channelNumMap[VPS_CAPT_STREAM_ID_MAX]
                                         [VPS_CAPT_CH_PER_PORT_MAX];
    /**< Channel Number to assign to each channel and stream of this handle.
         This is used during FVID2_queue(), FVID2_dequeue().
         Channel number must be unique across the whole system.
         Users can use Vps_captMakeChannelNum() to generate a system unique
         channel number. */
    UInt32                  inScanFormat;
    /**< Input source scan format - interlaced or progressive.
         For valid values see #FVID2_ScanFormat. */
    UInt32                  muxModeStartChId;
    /**< Start channel ID in pixel or line mux mode. Used to add an offset to
         start channel mapping. This will be used when the decoder start channel
         ID is other than 0.
         For example some decoder's channel 0 CHID starts from 4 instead of 0.
         This is valid only in multi-channel mode and is ignored in single
         channel or other modes. */
} Vps_CaptCreateParams;

/**
 *  \brief Capture driver create status. Returned after calling FVID2_create()
 */
typedef struct
{
    Int32                   retVal;
    /**< Create status, FVID2_SOK on success, else failure */
} Vps_CaptCreateStatus;

/**
  \brief Capture driver run-time parameters

  - This structure is returned by capture driver when FVID2_dequeue()
    is called by application
  - NOTE:this structure is NOT filled by driver when TILED mode is used
  - This structure is returned as part of FVID2_Frame.perFrameCfg
  - Application should make sure FVID2_Frame.perFrameCfg is set to a valid
    Vps_CaptRtParams pointer when queue-ing the frame back to capture driver
    \code
    Vps_CaptRtParams rtParams;

    pFrame->perFrameCfg = &rtParams;
    \endcode
  - Alternatively, user can pass back the same FVID2_Frame pointer without
    modifying FVID2_Frame.perFrameCfg
*/
typedef struct
{
    UInt32                  captureOutWidth;
    /**< Capture data width in pixels */
    UInt32                  captureOutHeight;
    /**< Capture data height in lines */
} Vps_CaptRtParams;

/**
 *  \brief Capture Channel Get Status IOCTL input arguments
 */
typedef struct
{
    UInt32                  channelNum;
    /**< [IN] Channel number for which status is requested */

    UInt32                  frameInterval;
    /**< [IN] Expected interval in units of timer ticks between frames.
         Vps_CaptChStatus.isVideoDetected is FALSE if no frame is captured
         for a duration of 'frameInterval x 2', else
         Vps_CaptChStatus.isVideoDetected is TRUE */
} Vps_CaptChGetStatusArgs;

/**
 *  \brief Capture Channel Get Status IOCTL result
 */
typedef struct
{
    UInt32                  isVideoDetected;
    /**< [OUT] TRUE: Video detected at this channel,
               FALSE: Video not detected at this channel */
    UInt32                  outWidth;
    /**< [OUT] Capture data output width in pixels */
    UInt32                  outHeight;
    /**< [OUT] Capture data output height in lines */
    UInt32                  droppedFrameCount;
    /**< [OUT] Number of frame's dropped by driver due to unavailability
         of buffer from application  */
    UInt32                  captureFrameCount;
    /**< [OUT] Number of frame's captured by driver into the buffer
               provided by application */
} Vps_CaptChStatus;

/**
 *  \brief Frame Skip parameters
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Channel number of which frame skip will be applied */
    UInt32                  frameSkipMask;
    /**< Frame Skip Mask (bit0..bit29) bitN = 1 Skip frame,
         bitN = 0 DO NOT skip frame
         Example, 0x0 for no frame skip, 0x2AAAAAAA for skipping
         alternate frames */
} Vps_CaptFrameSkip;

/**
 *  \brief VIP overflow status structure.
 */
typedef struct
{
    UInt32                  isPortOverFlowed[VPS_CAPT_INST_MAX];
    /**< Flag to indicate whether a port overflowed or not. */
} Vps_CaptOverFlowStatus;

/**
 *  \brief Structure for VIP Frame based capture.
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Channel number for which this configuration holds good. ChNum is
     *   ignored for now. Configuration will take effect for all the channels
     *   of the object for now
     */
    UInt32                  bufferFmt;
    /**< Captured interlaced input as either fields or frame. For valid values
     *   see #FVID2_BufferFormat
     */
    UInt32                  fieldMerged;
    /**< Whether to merged the captured fields or to be in seperate planes.
     *   If this variable is True both the fields of the captured frames will
     *   be merged. If its false EvenField will be in top plane and odd
     *   field will be bottom plane. User has to populate address for both the
     *   fields in #FVID2_Frame structure for frame based capture.
     */
} Vps_CaptStorageParams;

/**
 *  \brief Structure for VIP crop configuration.
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Channel number for which this configuration is be applied. This is
      *  not used in the current implementation and should be set to zero by
      *  the application. */
    UInt32                  vipCropEnable;
    /**< Enable or disable VIP crop feature. */
    Vps_CropConfig          vipCropCfg;
    /**< VIP crop configuration - crop width, crop height, startX and startY. */
} Vps_CaptVipCropParams;


typedef struct
{
    UInt32          enable;
    /**< Enable/disable returning of error frames to application. */
}Vps_CaptReturnErrFrms;


/**
 *  \brief Structure for VIP Capture frames callback configuration.
 */
typedef struct
{
    UInt32          enable;
    /**< Enable single callback for the completed frames. */
}Vps_CaptOneCallBackPerFrm;


/**
 *  \brief Structure used to configure time stamping of captured frames.
 */
typedef struct
{
    UInt32              enableBestEffort;
    /**< Enable accurate time stamping of captured frames. This method uses
         interrupt to detemine when capture of a frame completes and time stamps
         the frame. TRUE enables this method, 

         FALSE, enables time stamping of captured frames, without relying on 
         interrupt. This meathod gurantees a max latency/offset period of 16 
         msecs. This meathod has least CPU load and will not use interrupts. */

    Vps_CaptFrameTimeStamp  timeStamper;
    /**< By default, DSP/BIOS API Clock_getTicks () is used to time stamp
         captured frames, applications could over-ride the same, by providing a
         function that would return an unsigned int 32 value.
         The driver would call this function and update the timeStamp member of
         FVID2_Frame with the return value of this function.
         
         The above functionality is applicable only when, enableBestEffort is 
         set to TRUE. */
    UInt32 expectedFps;
    /**< As the name suggests, specify the expected FPS. This value will be used
         to calculate statistics */
}Vps_CaptFrameTimeStampParms;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief Get capture instance ID from channelNum
 *
 * channelNum is value which is a combination of
 *  - instance ID
 *  - stream ID for that instance
 *  - channel ID for that stream, instance
 *
 * For details refer to VIP capture section in User Guide
 *
 * \param channelNum [IN] channelNum
 *
 * \return instance ID ( 0 .. VPS_CAPT_INST_MAX-1 )
*/
static inline UInt32 Vps_captGetInstId ( UInt32 channelNum )
{
    return channelNum / ( VPS_CAPT_CH_PER_PORT_MAX * VPS_CAPT_STREAM_ID_MAX );
}

/**
 * \brief Get capture stream ID from channelNum
 *
 * channelNum is value which is a combination of
 *  - instance ID
 *  - stream ID for that instance
 *  - channel ID for that stream, instance
 *
 * For details refer to VIP capture section in User Guide
 *
 * \param channelNum [IN] channelNum
 *
 * \return stream ID ( 0 .. VPS_CAPT_STREAM_ID_MAX-1 )
*/
static inline UInt32 Vps_captGetStreamId ( UInt32 channelNum )
{
    UInt32 value;

    value = channelNum % ( VPS_CAPT_CH_PER_PORT_MAX * VPS_CAPT_STREAM_ID_MAX );
    value /= VPS_CAPT_CH_PER_PORT_MAX;

    return value;
}

/**
 * \brief Get capture channel ID from channelNum
 *
 * channelNum is value which is a combination of
 *  - instance ID
 *  - stream ID for that instance
 *  - channel ID for that stream, instance
 *
 * For details refer to VIP capture section in User Guide
 *
* \param channelNum [IN] channelNum
 *
 * \return channel ID ( 0 .. VPS_CAPT_CH_PER_PORT_MAX-1 )
*/
static inline UInt32 Vps_captGetChId ( UInt32 channelNum )
{
    return channelNum % ( VPS_CAPT_CH_PER_PORT_MAX );
}

/**
 * \brief Make a system unique channelNum
 *
 * channelNum is value which is a combination of
 *  - instance ID
 *  - stream ID for that instance
 *  - channel ID for that stream, instance
 *
 * For details refer to VIP capture section in User Guide
 *
 * \param instId    [IN] VIP Instance ID
 * \param streamId  [IN] Stream ID
 * \param chId      [IN] Channel ID
 *
 * \return channelNum
*/
static inline UInt32 Vps_captMakeChannelNum ( UInt32 instId, UInt32 streamId,
                                              UInt32 chId )
{
    return instId * VPS_CAPT_CH_PER_PORT_MAX * VPS_CAPT_STREAM_ID_MAX
        + streamId * VPS_CAPT_CH_PER_PORT_MAX + chId;
}

#endif /* #ifndef _VPS_CAPTURE_H */

/* @} */
