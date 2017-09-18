/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \defgroup VPS_DRV_FVID2_API FVID2 API
 *
 *  FVID2 API is an interface which is used to create, control and use different
 *  HD-VPSS related video drivers. FVID2 API is used not only for on-chip
 *  peripherals drivers like capture, display but also for external off-chip
 *  video device peripherals like video decoders, video encoders.
 *
 *  All HD-VPSS drivers and external video peripherals implement the FVID2 API.
 *  Many drivers also extend the FVID2 interface by defining their own driver
 *  specific IOCTLs.
 *
 *  <b> All FVID2 API must be called from task context </b>. Some examples
 *  in the HD-VPSS package maybe calling FVID2 APIs from callback and/or
 *  interrupt context. Such examples will be modified in subsequent releases
 *  and in order that user application do not have to do special migration it
 *  is strongly recommended to use FVID2 APIs only from task context.
 *
 *  A FVID2 API can be of two kinds,
 *
 *  - <b> Blocking API </b> <br>
 *    Here the FVID2 API call returns only after completing the request or
 *    functionality or if timed out or if some other error occurs.
 *    The below APIs are always blocking, unless specified otherwise by
 *    specific driver.
 *    - FVID2_create()
 *    - FVID2_delete()
 *    - FVID2_start()
 *    - FVID2_stop()
 *    - FVID2_control()
 *    - FVID2_setFormat()
 *    - FVID2_getFormat()
 *
 *  - <b> Non-blocking API </b> <br>
 *    Here FVID2 API call queues the request to the driver and returns
 *    immediately before the request is processed or completed.
 *    Application should use the completion callback to wait for request
 *    completion.
 *    The below APIs are always non-blocking, unless specified otherwise by
 *    specific driver.
 *    - FVID2_queue()
 *    - FVID2_dequeue()
 *    - FVID2_processFrames()
 *    - FVID2_getProcessedFrames()
 *
 *  A FVID2 Driver can belong to one the below categories. The FVID2 APIs
 *  applicable to driver in a category are also listed below. The below is
 *  true for most driver unless specified otherwise by a specific driver.
 *  - <b> Streaming interface drivers </b> <br>
 *  Examples include, capture, display, graphics.
 *  Below FVID APIs are implemented by such drivers.
 *    - FVID2_create()
 *    - FVID2_delete()
 *    - FVID2_start()
 *    - FVID2_stop()
 *    - FVID2_queue()
 *    - FVID2_dequeue()
 *    - FVID2_control()
 *    - FVID2_setFormat() - OPTIONAL
 *    - FVID2_getFormat() - OPTIONAL
 *
 *  - <b> Memory to memory processing interface drivers </b> <br>
 *   Examples include, M2M deinterlacer, M2M scalar, M2M noise filter.
 *   Below FVID APIs are implemented by such drivers.
 *    - FVID2_create()
 *    - FVID2_delete()
 *    - FVID2_processFrames()
 *    - FVID2_getProcessedFrames()
 *    - FVID2_control()
 *    - FVID2_setFormat() - OPTIONAL
 *    - FVID2_getFormat() - OPTIONAL
 *
 *  - <b> Control interface drivers </b> <br>
 *   Examples include, display controller, external video devices.
 *   Below FVID APIs are implemented by such drivers.
 *    - FVID2_create()
 *    - FVID2_delete()
 *    - FVID2_control()
 *    - FVID2_start()
 *    - FVID2_stop()
 *    - FVID2_setFormat() - OPTIONAL
 *    - FVID2_getFormat() - OPTIONAL
 *
 *  Before making any FVID2 API calls, FVID2_init() must be called.
 *  FVID2_deInit() must be called during system shutdown.
 *
 *  FVID2_getVersionString() and FVID2_getVersionNumber() can be used to get
 *  information about current driver version number.
 *
 *  All FVID2 drivers are of type FVID2_Handle, which gets created when
 *  FVID2_create() is called. This handle is used for all subsequent FVID2
 *  API calls. This handle and its associated resources are free'ed when
 *  FVID2_delete() is called.
 *
 *  All FVID2 APIs make use of FVID2_Frame, FVID2_FrameList and/or
 *  FVID2_ProcessList for exchanging and processing video frames via a driver.
 *  Further all drivers use a common user callback mechanism via FVID2_CbParams
 *  to indicate to the user that a frame is ready. Many drivers, but not all,
 *  use the FVID2_Format data structure to describe video input, output data
 *  formats.
 *
 *  All drivers use the constants, enum's, error codes defined in this file to
 *  control and configure a driver.
 *
 *  In addition, most drivers define driver specific create time parameters
 *  and IOCTLs to allow the user to control and configure the driver in driver
 *  specific ways. Further a driver may define driver specific run-time
 *  parameters which are passed by user to the driver via
 *  FVID2_Frame.perFrameCfg and/or FVID2_FrameList.perListCfg.
 *
 *  Also user application can associate user specific app data with a
 *  driver handle via FVID2_CbParams.appData or with every frame via
 *  FIVD2_Frame.appData. The app data set as part of FVID2_CbParams returned
 *  back to user when callback occurs. The app data set as part of FVID2_Frame
 *  is returned back to the user when the FVID2_Frame itself is returned back to
 *  user via FVID2_dequeue() or FVID2_getProcessedFrames(). The driver will not
 *  modify this app data. This could be used by the application to store any
 *  application specific data like application object info and so on.
 *
 *  Many FVID2 APIs define 'reserved' fields. These are meant for future use
 *  and should be set to NULL by user.
 *
 *  @{
 */

/**
 *  \file fvid2.h
 *
 *  \brief FVID2 API
 */

/*
 *  \author  PSP, TI
 *
 *  \version 0.1    Created.
 *           0.2    Changes for BIOS 6 and new FVID2 interface.
 *           0.2.1  Merged color and data format enums to data format enums.
 *                  Added process list structure to support M2M
 *                  (memory to memory) drivers.
 *                  Removed FVID2_Buf structure and directly used address array
 *                  in FVID2_Frame structure.
 *                  Defined proper signature for FVID2 APIs.
 *           0.2.2  Added proper comments.
 *           0.3    Added subframe callback and added reserved fields in all the
 *                  structures.
 *           0.4    Made frames pointer to pointer to array of frame pointer in
 *                  framelist to simplify application allocation and ease of
 *                  use.
 *           0.5    Added FVID2 standard enums.
 *
 */

#ifndef _FVID2_H
#define _FVID2_H

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

/** \brief FVID2 Driver handle. */
typedef Ptr FVID2_Handle;


/**
 *  \brief FVID2 driver callback function prototype.
 *
 *  This callback is used by the drivers which implement non-blocking
 *  FVID2 APIs.
 *
 *  User should assume that callbacks happen in ISR context and hence should
 *  apply rules relevant to ISR context when implementing callback functions,
 *  i.e blocking APIs should not be called from callback. Users should also
 *  assume that interrupts are enabled when in callback context, i.e other
 *  interrupt / callbacks can occur when a callback is under execution.
 *  User callback implementation should not be very long, since callback
 *  execution may block other drivers from executing. Typically it is
 *  recommended to set a semaphore or flag when callback happens and do the
 *  more involved callback handling in task context.
 *
 *  This callback is typically called by the driver as mentioned below,
 *  unless specified otherwise by specific drivers.
 *
 *  In case of display drivers, this callback function will be called
 *  when a frame(s) is/are completed displayed. Then the application can
 *  dequeue the displayed buffer back from the driver.
 *
 *  In case of capture drivers, this callback function will be called
 *  when a frame(s) is/are captured. Then the application can dequeue the
 *  captured buffer back from the driver.
 *
 *  In case of M2M drivers, this callback function will be called
 *  when a request from the application is completed by the driver. Then the
 *  application can dequeue the completed request buffers back from the driver.
 *
 *  In case of control drivers, callbacks are typically not used since mostly
 *  FVID2 APIs implemented by control drivers are of blocking nature.
 *
 *  \param handle   [OUT] FVID2 handle for which the callback has occurred.
 *  \param appData  [OUT] Application data provided to the driver at the
 *                        time of FVID2_create().
 *  \param reserved [OUT] For future use. Set to NULL.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
typedef Int32 (*FVID2_CbFxn) (FVID2_Handle handle, Ptr appData, Ptr reserved);

/**
 *  \brief FVID2 error callback function prototype.
 *
 *  Error callback is called by driver when it encounters a error during
 *  processing of frames in its internal queue.
 *
 *  When user submits frames to the driver it does minimal error checks and
 *  stores the the frames in its internal queues. In case of any error at
 *  this point the FVID2 API will simply return error and error callback
 *  will not be called.
 *
 *  The driver will later (depending on how its request queue is full) process
 *  these frames submitted by the user. If driver finds any error in either
 *  the input frame information or hardware itself then it will call this error
 *  callback to notify the user about this error. As part of the callback
 *  information it also returns the frames or request for which error happened
 *  so that user can take appropriate action.
 *
 *  Users should apply same rules as that of FVID2_CbFxn when implementing
 *  error callbacks.
 *
 *  \param handle   [OUT] FVID2 handle for which the callback has occurred.
 *  \param appData  [OUT] Application data provided to the driver at the time
 *                        of FVID2_create().
 *  \param errList  [OUT] Pointer to a valid framelist (FVID2_FrameList)
 *                        in case of capture and display drivers or a pointer
 *                        to a valid processlist (FVID2_ProcessList) in case of
 *                        M2M drivers. The driver copies the aborted/error
 *                        frames in this frame list or process list.
 *  \param reserved [OUT] For future use. Set to NULL.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
typedef Int32 (*FVID2_ErrCbFxn) (FVID2_Handle handle,
                                 Ptr appData,
                                 Ptr errList,
                                 Ptr reserved);

/**
 *  \name FVID2 Error Codes
 *
 *  Error codes returned by FVID2 APIs
 *
 *  @{
 */

/** \brief FVID2 API call successful. */
#define FVID2_SOK                       ((Int32) 0)

/** \brief FVID2 API call returned with error as failed. Used for generic error.
 *  It may be some hardware failure and/or software failure. */
#define FVID2_EFAIL                     ((Int32) -1)

/** \brief FVID2 API call returned with error as bad arguments.
 *  Typically, NULL pointer passed to the FVID2 API where its not expected. */
#define FVID2_EBADARGS                  ((Int32) -2)

/** \brief FVID2 API call returned with error as invalid parameters. Typically
 *  when parameters passed are not valid or out of range. */
#define FVID2_EINVALID_PARAMS           ((Int32) -3)

/** \brief FVID2 API call returned with error as device already in use. Example,
 *  tried to open the driver maximum + 1 times. Display and Capture driver
 *  supports single open only, while M2M driver supports multiple open. */
#define FVID2_EDEVICE_INUSE             ((Int32) -4)

/** \brief FVID2 API call returned with error as timed out. Typically API is
 *  waiting for some condition and returned as condition not happened
 *  in the timeout period. */
#define FVID2_ETIMEOUT                  ((Int32) -5)

/** \brief FVID2 API call returned with error as allocation failure. Typically
 *  memory or resource allocation failure. */
#define FVID2_EALLOC                    ((Int32) -6)

/** \brief FVID2 API call returned with error as out of range. Typically when
 *  API is called with some argument that is out of range for that API like
 *  array index etc. */
#define FVID2_EOUT_OF_RANGE             ((Int32) -7)

/** \brief FVID2 API call returned with error as try again. Momentarily API is
 *  not able to service request because of queue full or any other temporary
 *  reason. */
#define FVID2_EAGAIN                    ((Int32) -8)

/** \brief FVID2 API call returned with unsupported command. Typically when
 *  command is not supported by control API. */
#define FVID2_EUNSUPPORTED_CMD          ((Int32) -9)

/** \brief FVID2 API call returned with error as no more buffers available.
 *  Typically when no buffers are available. */
#define FVID2_ENO_MORE_BUFFERS          ((Int32) -10)

/** \brief FVID2 API call returned with error as unsupported operation.
 *  Typically when the specific operation is not supported by that API such
 *  as IOCTL not supporting some specific functions. */
#define FVID2_EUNSUPPORTED_OPS          ((Int32) -11)

/** \brief FVID2 API call returned with error as driver already in use. */
#define FVID2_EDRIVER_INUSE             ((Int32) -12)

/* @} */

/**
 *  \name FVID2 Max limits
 *  @{
 */

/**
 *  \brief This macro determines the maximum number of FVID2 frame pointers
 *  that can be passed per frame list.
 */
#define FVID2_MAX_FVID_FRAME_PTR        (64u)

/**
 *  \brief This macro determines the maximum number of planes/address used to
 *  represent a video buffer per field. Currently this is set to 3 to support
 *  the maximum pointers required for YUV planar format - Y, Cb and Cr.
 */
#define FVID2_MAX_PLANES                (3u)

/**
 *  \brief Number of fields - top and bottom. Used for allocating address
 *  pointers for both the fields.
 */
#define FVID2_MAX_FIELDS                (2u)

/** \brief Number of IN/OUT frame list per process list - Used for array
  * allocation in process list structure. */
#define FVID2_MAX_IN_OUT_PROCESS_LISTS  (4u)

/* @} */


/** \brief No Timeout. */
#define FVID2_TIMEOUT_NONE              (0u)

/** \brief Timeout wait forever. */
#define FVID2_TIMEOUT_FOREVER           (~(0u))

/**
 *  \name FVID2 Frame Address Index
 *
 *  FVID2_Frame.addr structure is a 2D array of pointers.
 *  The below indices are used to identify the correct buffer address
 *  corresponding to the field and buffer formats.
 */

/* @{ */

/** \brief Index for top field address in case of interlaced frame mode. */
#define FVID2_FIELD_TOP_ADDR_IDX        (0u)

/** \brief Index for bottom field address in case of interlaced frame mode. */
#define FVID2_FIELD_BOTTOM_ADDR_IDX     (1u)

/** \brief Index for frame address in case of progressive mode. */
#define FVID2_FRAME_ADDR_IDX            (0u)

/** \brief Index for field mode address index. This is used in case of field
 *  mode of operation as in field capture or in deinterlacing mode of
 *  operation. In these cases both the even and odd field index is one and
 *  the same. */
#define FVID2_FIELD_MODE_ADDR_IDX       (0u)

/** \brief Index for even field address in case of interlaced frame mode. */
#define FVID2_FIELD_EVEN_ADDR_IDX       (FVID2_FIELD_TOP_ADDR_IDX)

/** \brief Index for odd field address in case of interlaced frame mode. */
#define FVID2_FIELD_ODD_ADDR_IDX        (FVID2_FIELD_BOTTOM_ADDR_IDX)

/** \brief Index for frame address in case of progressive mode. */
#define FVID2_FIELD_NONE_ADDR_IDX       (FVID2_FRAME_ADDR_IDX)

/** \brief Index for YUV444/YUV422 interleaved formats. */
#define FVID2_YUV_INT_ADDR_IDX          (0u)

/** \brief Y Index for YUV444/YUV422/YUV420 planar formats. */
#define FVID2_YUV_PL_Y_ADDR_IDX         (0u)

/** \brief CB Index for YUV444/YUV422/YUV420 planar formats. */
#define FVID2_YUV_PL_CB_ADDR_IDX        (1u)

/** \brief CR Index for YUV444/YUV422/YUV420 planar formats. */
#define FVID2_YUV_PL_CR_ADDR_IDX        (2u)

/** \brief Y Index for YUV semi planar formats. */
#define FVID2_YUV_SP_Y_ADDR_IDX         (0u)

/** \brief CB Index for semi planar formats. */
#define FVID2_YUV_SP_CBCR_ADDR_IDX      (1u)

/** \brief Index for RGB888/RGB565/ARGB32 formats. */
#define FVID2_RGB_ADDR_IDX              (0u)

/** \brief Index for RAW formats. */
#define FVID2_RAW_ADDR_IDX              (0u)

/** \brief Even Field. */
#define FVID2_FID_EVEN                  (FVID2_FID_TOP)

/** \brief Odd Field. */
#define FVID2_FID_ODD                   (FVID2_FID_BOTTOM)

/** \brief No Field. */
#define FVID2_FID_NONE                  (FVID2_FID_FRAME)

/* @} */

/*
 * =========== Command codes for Submit call =============
 */
/** \brief Control command base address. */
#define FVID2_CTRL_BASE                 (0x00000000u)
/** \brief User command base address. */
#define FVID2_USER_BASE                 (0x10000000u)

/**
 *  \addtogroup VPS_DRV_FVID2_IOCTL_FVID2
 *  @{
 */

/**
 *  \brief Control command used by FVID2_setFormat()
 *
 *  \param cmdArgs       [IN]  const FVID2_Format *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure.
 */
#define FVID2_SET_FORMAT                (FVID2_CTRL_BASE + 3u)

/**
 *  \brief Control command used by FVID2_getFormat()
 *
 *  \param cmdArgs       [IN]  FVID2_Format *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure.
 */
#define FVID2_GET_FORMAT                (FVID2_CTRL_BASE + 4u)

/**
 *  \brief Control command used by FVID2_start()
 *
 *  \param cmdArgs       [IN]  Driver specific
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure.
 */
#define FVID2_START                     (FVID2_CTRL_BASE + 5u)

/**
 *  \brief Control command used by FVID2_stop()
 *
 *  \param cmdArgs       [IN]  Driver specific
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure.
 */
#define FVID2_STOP                      (FVID2_CTRL_BASE + 6u)

/* @} */

/**
 *  \brief Video Data format.
 */
typedef enum
{
    FVID2_DF_YUV422I_UYVY = 0x0000,
    /**< YUV 422 Interleaved format - UYVY. */
    FVID2_DF_YUV422I_YUYV,
    /**< YUV 422 Interleaved format - YUYV. */
    FVID2_DF_YUV422I_YVYU,
    /**< YUV 422 Interleaved format - YVYU. */
    FVID2_DF_YUV422I_VYUY,
    /**< YUV 422 Interleaved format - VYUY. */
    FVID2_DF_YUV422SP_UV,
    /**< YUV 422 Semi-Planar - Y separate, UV interleaved. */
    FVID2_DF_YUV422SP_VU,
    /**< YUV 422 Semi-Planar - Y separate, VU interleaved. */
    FVID2_DF_YUV422P,
    /**< YUV 422 Planar - Y, U and V separate. */
    FVID2_DF_YUV420SP_UV,
    /**< YUV 420 Semi-Planar - Y separate, UV interleaved. */
    FVID2_DF_YUV420SP_VU,
    /**< YUV 420 Semi-Planar - Y separate, VU interleaved. */
    FVID2_DF_YUV420P,
    /**< YUV 420 Planar - Y, U and V separate. */
    FVID2_DF_YUV444P,
    /**< YUV 444 Planar - Y, U and V separate. */
    FVID2_DF_YUV444I,
    /**< YUV 444 interleaved - YUVYUV... */
    FVID2_DF_RGB16_565 = 0x1000,
    /**< RGB565 16-bit - 5-bits R, 6-bits G, 5-bits B. */
    FVID2_DF_ARGB16_1555,
    /**< ARGB1555 16-bit - 5-bits R, 5-bits G, 5-bits B, 1-bit Alpha (MSB). */
    FVID2_DF_RGBA16_5551,
    /**< RGBA5551 16-bit - 5-bits R, 5-bits G, 5-bits B, 1-bit Alpha (LSB). */
    FVID2_DF_ARGB16_4444,
    /**< ARGB4444 16-bit - 4-bits R, 4-bits G, 4-bits B, 4-bit Alpha (MSB). */
    FVID2_DF_RGBA16_4444,
    /**< RGBA4444 16-bit - 4-bits R, 4-bits G, 4-bits B, 4-bit Alpha (LSB). */
    FVID2_DF_ARGB24_6666,
    /**< ARGB6666 24-bit - 6-bits R, 6-bits G, 6-bits B, 6-bit Alpha (MSB). */
    FVID2_DF_RGBA24_6666,
    /**< RGBA6666 24-bit - 6-bits R, 6-bits G, 6-bits B, 6-bit Alpha (LSB). */
    FVID2_DF_RGB24_888,
    /**< RGB24 24-bit - 8-bits R, 8-bits G, 8-bits B. */
    FVID2_DF_ARGB32_8888,
    /**< ARGB32 32-bit - 8-bits R, 8-bits G, 8-bits B, 8-bit Alpha (MSB). */
    FVID2_DF_RGBA32_8888,
    /**< RGBA32 32-bit - 8-bits R, 8-bits G, 8-bits B, 8-bit Alpha (LSB). */
    FVID2_DF_BGR16_565,
    /**< BGR565 16-bit -   5-bits B, 6-bits G, 5-bits R. */
    FVID2_DF_ABGR16_1555,
    /**< ABGR1555 16-bit - 5-bits B, 5-bits G, 5-bits R, 1-bit Alpha (MSB). */
    FVID2_DF_ABGR16_4444,
    /**< ABGR4444 16-bit - 4-bits B, 4-bits G, 4-bits R, 4-bit Alpha (MSB). */
    FVID2_DF_BGRA16_5551,
    /**< BGRA5551 16-bit - 5-bits B, 5-bits G, 5-bits R, 1-bit Alpha (LSB). */
    FVID2_DF_BGRA16_4444,
    /**< BGRA4444 16-bit - 4-bits B, 4-bits G, 4-bits R, 4-bit Alpha (LSB). */
    FVID2_DF_ABGR24_6666,
    /**< ABGR6666 24-bit - 6-bits B, 6-bits G, 6-bits R, 6-bit Alpha (MSB). */
    FVID2_DF_BGR24_888,
    /**< BGR888 24-bit - 8-bits B, 8-bits G, 8-bits R. */
    FVID2_DF_ABGR32_8888,
    /**< ABGR8888 32-bit - 8-bits B, 8-bits G, 8-bits R, 8-bit Alpha (MSB). */
    FVID2_DF_BGRA24_6666,
    /**< BGRA6666 24-bit - 6-bits B, 6-bits G, 6-bits R, 6-bit Alpha (LSB). */
    FVID2_DF_BGRA32_8888,
    /**< BGRA8888 32-bit - 8-bits B, 8-bits G, 8-bits R, 8-bit Alpha (LSB). */
    FVID2_DF_BITMAP8 = 0x2000,
    /**< BITMAP 8bpp. */
    FVID2_DF_BITMAP4_LOWER,
    /**< BITMAP 4bpp lower address in CLUT. */
    FVID2_DF_BITMAP4_UPPER,
    /**< BITMAP 4bpp upper address in CLUT. */
    FVID2_DF_BITMAP2_OFFSET0,
    /**< BITMAP 2bpp offset 0 in CLUT. */
    FVID2_DF_BITMAP2_OFFSET1,
    /**< BITMAP 2bpp offset 1 in CLUT. */
    FVID2_DF_BITMAP2_OFFSET2,
    /**< BITMAP 2bpp offset 2 in CLUT. */
    FVID2_DF_BITMAP2_OFFSET3,
    /**< BITMAP 2bpp offset 3 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET0,
    /**< BITMAP 1bpp offset 0 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET1,
    /**< BITMAP 1bpp offset 1 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET2,
    /**< BITMAP 1bpp offset 2 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET3,
    /**< BITMAP 1bpp offset 3 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET4,
    /**< BITMAP 1bpp offset 4 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET5,
    /**< BITMAP 1bpp offset 5 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET6,
    /**< BITMAP 1bpp offset 6 in CLUT. */
    FVID2_DF_BITMAP1_OFFSET7,
    /**< BITMAP 1bpp offset 7 in CLUT. */
    FVID2_DF_BITMAP8_BGRA32,
    /**< BITMAP 8bpp BGRA32. */
    FVID2_DF_BITMAP4_BGRA32_LOWER,
    /**< BITMAP 4bpp BGRA32 lower address in CLUT. */
    FVID2_DF_BITMAP4_BGRA32_UPPER,
    /**< BITMAP 4bpp BGRA32 upper address in CLUT. */
    FVID2_DF_BITMAP2_BGRA32_OFFSET0,
    /**< BITMAP 2bpp BGRA32 offset 0 in CLUT. */
    FVID2_DF_BITMAP2_BGRA32_OFFSET1,
    /**< BITMAP 2bpp BGRA32 offset 1 in CLUT. */
    FVID2_DF_BITMAP2_BGRA32_OFFSET2,
    /**< BITMAP 2bpp BGRA32 offset 2 in CLUT. */
    FVID2_DF_BITMAP2_BGRA32_OFFSET3,
    /**< BITMAP 2bpp BGRA32 offset 3 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET0,
    /**< BITMAP 1bpp BGRA32 offset 0 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET1,
    /**< BITMAP 1bpp BGRA32 offset 1 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET2,
    /**< BITMAP 1bpp BGRA32 offset 2 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET3,
    /**< BITMAP 1bpp BGRA32 offset 3 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET4,
    /**< BITMAP 1bpp BGRA32 offset 4 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET5,
    /**< BITMAP 1bpp BGRA32 offset 5 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET6,
    /**< BITMAP 1bpp BGRA32 offset 6 in CLUT. */
    FVID2_DF_BITMAP1_BGRA32_OFFSET7,
    /**< BITMAP 1bpp BGRA32 offset 7 in CLUT. */
    FVID2_DF_BAYER_RAW = 0x3000,
    /**< Bayer pattern. */
    FVID2_DF_RAW_VBI,
    /**< Raw VBI data. */
    FVID2_DF_RAW,
    /**< Raw data - Format not interpreted. */
    FVID2_DF_MISC,
    /**< For future purpose. */
    FVID2_DF_INVALID
    /**< Invalid data format. Could be used to initialize variables. */
} FVID2_DataFormat;

/**
 *  \brief Scan format.
 */
typedef enum
{
    FVID2_SF_INTERLACED = 0,
    /**< Interlaced mode. */
    FVID2_SF_PROGRESSIVE,
    /**< Progressive mode. */
    FVID2_SF_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} FVID2_ScanFormat;

/**
 *  \brief Video standards.
 */
typedef enum
{
    FVID2_STD_NTSC = 0u,
    /**< 720x480 30FPS interlaced NTSC standard. */
    FVID2_STD_PAL,
    /**< 720x576 30FPS interlaced PAL standard. */

    FVID2_STD_480I,
    /**< 720x480 30FPS interlaced SD standard. */
    FVID2_STD_576I,
    /**< 720x576 30FPS interlaced SD standard. */

    FVID2_STD_CIF,
    /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
    FVID2_STD_HALF_D1,
    /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
    FVID2_STD_D1,
    /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */

    FVID2_STD_480P,
    /**< 720x480 60FPS progressive ED standard. */
    FVID2_STD_576P,
    /**< 720x576 60FPS progressive ED standard. */

    FVID2_STD_720P_60,
    /**< 1280x720 60FPS progressive HD standard. */
    FVID2_STD_720P_50,
    /**< 1280x720 50FPS progressive HD standard. */

    FVID2_STD_1080I_60,
    /**< 1920x1080 30FPS interlaced HD standard. */
    FVID2_STD_1080I_50,
    /**< 1920x1080 50FPS interlaced HD standard. */

    FVID2_STD_1080P_60,
    /**< 1920x1080 60FPS progressive HD standard. */
    FVID2_STD_1080P_50,
    /**< 1920x1080 50FPS progressive HD standard. */

    FVID2_STD_1080P_24,
    /**< 1920x1080 24FPS progressive HD standard. */
    FVID2_STD_1080P_30,
    /**< 1920x1080 30FPS progressive HD standard. */

    /* Vesa standards from here Please add all SMTPE and CEA standard enums
       above this only. this is to ensure proxy Oses compatibility
     */
    FVID2_STD_VGA_60 = 0x100,
    /**< 640x480 60FPS VESA standard. */
    FVID2_STD_VGA_72,
    /**< 640x480 72FPS VESA standard. */
    FVID2_STD_VGA_75,
    /**< 640x480 75FPS VESA standard. */
    FVID2_STD_VGA_85,
    /**< 640x480 85FPS VESA standard. */

    FVID2_STD_WVGA_60,
    /**< 800x480 60PFS WVGA */

    FVID2_STD_SVGA_60,
    /**< 800x600 60FPS VESA standard. */
    FVID2_STD_SVGA_72,
    /**< 800x600 72FPS VESA standard. */
    FVID2_STD_SVGA_75,
    /**< 800x600 75FPS VESA standard. */
    FVID2_STD_SVGA_85,
    /**< 800x600 85FPS VESA standard. */

    FVID2_STD_WSVGA_70,
    /**< 1024x600 70FPS standard. */

    FVID2_STD_XGA_60,
    /**< 1024x768 60FPS VESA standard. */
    FVID2_STD_XGA_70,
    /**< 1024x768 72FPS VESA standard. */
    FVID2_STD_XGA_75,
    /**< 1024x768 75FPS VESA standard. */
    FVID2_STD_XGA_85,
    /**< 1024x768 85FPS VESA standard. */

    FVID2_STD_1368_768_60,
    /**< 1368x768 60 PFS VESA>*/
    FVID2_STD_1366_768_60,
    /**< 1366x768 60 PFS VESA>*/
    FVID2_STD_1360_768_60,
    /**< 1360x768 60 PFS VESA>*/

    FVID2_STD_WXGA_60,
    /**< 1280x768 60FPS VESA standard. */
    FVID2_STD_WXGA_75,
    /**< 1280x768 75FPS VESA standard. */
    FVID2_STD_WXGA_85,
    /**< 1280x768 85FPS VESA standard. */

    FVID2_STD_1440_900_60,
    /**< 1440x900 60 PFS VESA>*/

    FVID2_STD_SXGA_60,
    /**< 1280x1024 60FPS VESA standard. */
    FVID2_STD_SXGA_75,
    /**< 1280x1024 75FPS VESA standard. */
    FVID2_STD_SXGA_85,
    /**< 1280x1024 85FPS VESA standard. */

    FVID2_STD_WSXGAP_60,
    /**< 1680x1050 60 PFS VESA>*/

    FVID2_STD_SXGAP_60,
    /**< 1400x1050 60FPS VESA standard. */
    FVID2_STD_SXGAP_75,
    /**< 1400x1050 75FPS VESA standard. */

    FVID2_STD_UXGA_60,
    /**< 1600x1200 60FPS VESA standard. */

    /* Multi channel standards from here Please add all VESA standards enums
       above this only. this is to ensure proxy Oses compatibility */
    FVID2_STD_MUX_2CH_D1 = 0x200,
    /**< Interlaced, 2Ch D1, NTSC or PAL. */
    FVID2_STD_MUX_2CH_HALF_D1,
    /**< Interlaced, 2ch half D1, NTSC or PAL. */
    FVID2_STD_MUX_2CH_CIF,
    /**< Interlaced, 2ch CIF, NTSC or PAL. */
    FVID2_STD_MUX_4CH_D1,
    /**< Interlaced, 4Ch D1, NTSC or PAL. */
    FVID2_STD_MUX_4CH_CIF,
    /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    FVID2_STD_MUX_4CH_HALF_D1,
    /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    FVID2_STD_MUX_8CH_CIF,
    /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    FVID2_STD_MUX_8CH_HALF_D1,
    /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

	/* Megapixel resolution in 4:3 */
    FVID2_STD_5MP_2560_1920 = 0x250,
    FVID2_STD_8MP_3264_2448,
    FVID2_STD_10MP_3648_2736,

    /* Auto detect and Custom standards Please add all multi channel standard
       enums above this only. this is to ensure proxy Oses compatibility */
    FVID2_STD_AUTO_DETECT = 0x300,
    /**< Auto-detect standard. Used in capture mode. */
    FVID2_STD_CUSTOM
    /**< Custom standard used when connecting to external LCD etc...
         The video timing is provided by the application.
         Used in display mode. */
} FVID2_Standard;

/**
 *  \brief Field type.
 */
typedef enum
{
    FVID2_FID_TOP = 0,
    /**< Top field. */
    FVID2_FID_BOTTOM,
    /**< Bottom field. */
    FVID2_FID_FRAME,
    /**< Frame mode - Contains both the fields or a progressive frame. */
    FVID2_FID_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} FVID2_Fid;

/**
 * \brief Buffer storage format.
 */
typedef enum
{
    FVID2_BUF_FMT_FIELD = 0,
    /**< Buffers are captured/displayed as fields instead of frames */
    FVID2_BUF_FMT_FRAME
    /**< Buffers are captured/displayed as frames instead of frames */
} FVID2_BufferFormat;

/**
 *  \brief Bits per pixel.
 */
typedef enum
{
    FVID2_BPP_BITS1 = 0,
    /**< 1 Bits per Pixel. */
    FVID2_BPP_BITS2,
    /**< 2 Bits per Pixel. */
    FVID2_BPP_BITS4,
    /**< 4 Bits per Pixel. */
    FVID2_BPP_BITS8,
    /**< 8 Bits per Pixel. */
    FVID2_BPP_BITS12,
    /**< 12 Bits per Pixel - used for YUV420 format. */
    FVID2_BPP_BITS16,
    /**< 16 Bits per Pixel. */
    FVID2_BPP_BITS24,
    /**< 24 Bits per Pixel. */
    FVID2_BPP_BITS32,
    /**< 32 Bits per Pixel. */
    FVID2_BPP_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} FVID2_BitsPerPixel;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief FVID2 callback parameters that are setup during FVID2_create().
 */
typedef struct
{
    FVID2_CbFxn         cbFxn;
    /**< Application callback function used by the driver to intimate any
         operation has completed or not. This is an optional parameter
         in case application decides to use polling method and so could be
         set to NULL. */

    FVID2_ErrCbFxn      errCbFxn;
    /**< Application error callback function used by the driver to intimate
         any error occurs at the time of streaming. This is an optional
         parameter in case application decides not to get any error callback
         and so could be set to NULL. */

    Ptr                 errList;
    /**< Pointer to a valid FVID2_FrameList in case of capture
         and display drivers or a pointer to a valid FVID2_ProcessList
         in case of M2M drivers where the driver copies
         the aborted/error packet. The memory of this list should be
         allocated by the application and provided to the driver at the time
         of driver creation. When the application gets this callback, it has
         to empty this list and taken necessary action like freeing up memories
         etc. The driver will then reuse the same list for future error
         callback.
         This could be NULL if errCbFxn is NULL. Otherwise this should be
         non-NULL. */

    Ptr                 appData;
    /**< Application specific data which is returned in the callback function
         as it is. This could be set to NULL if not used. */
    Ptr                 reserved;
    /**< For future use. Not used currently. Set this to NULL. */
} FVID2_CbParams;

/**
 *  \brief FVID2 video buffer format specification.
 *
 *  Used during FVID2_create() by some drivers. Used as parameter to
 *  FVID2_setFormat(), FVID2_getFormat() by some drivers.
 */
typedef struct
{
    UInt32              channelNum;
    /**< Channel Number to which this format belongs to. */

    UInt32              width;
    /**< Width of the video frame or field in pixels. */

    UInt32              height;
    /**< Height of the video frame or field in lines. */

    UInt32              pitch[FVID2_MAX_PLANES];
    /**< Pitch in bytes for each of the sub-buffers. This represents the
         difference between two consecutive line addresses in bytes.
         This is irrespective of whether the video is interlaced or
         progressive and whether the fields are merged or separated for
         interlaced video. */

    UInt32              fieldMerged[FVID2_MAX_PLANES];
    /**< This field tells whether both the fields have to be merged,
         i.e line interleaved or not.
         Used only for interlaced format. The effective pitch is calculated
         based on this information along with pitch parameter. If fields are
         merged, effective pitch = pitch * 2 else effective pitch = pitch. */

    UInt32              dataFormat;
    /**< Frame data Format. For valid values see #FVID2_DataFormat. */

    UInt32              scanFormat;
    /**< Scan Format. For valid values see #FVID2_ScanFormat. */

    UInt32              bpp;
    /**< Number of bits per pixel. For valid values see #FVID2_BitsPerPixel. */

    Ptr                 reserved;
    /**< For future use. Not used currently. Set this to NULL. */
} FVID2_Format;

/**
 *  \brief Sub-Frame information
 *
 *  This is used in drivers supporting sub-frame level processing,
 *  for application and driver interaction
 */
typedef struct
{
    UInt32              subFrameNum;
    /**< [IN] Current sub-frame number in this frame,
         range is from 0 to (number of sub-frames in frame - 1).
         Set by application and used by driver. */

    UInt32              numInLines;
    /**< [IN] Number of lines available in input frame at the end of this sub-frame. */

    UInt32              numOutLines;
    /**< [OUT] Number of lines generated in output buffer after processing
         current sub-frame. Set by driver and used by application. */
} FVID2_SubFrameInfo;

/**
 *  \brief FVID2 frame buffer structure.
 *
 *  Unless specified otherwise, all fields in this structure are
 *
 *  [IN]  for FVID2_queue(), FVID2_processFrames() operation.
 *  [OUT] for FVID2_dequeue(), FVID2_getProcessedFrames() operation.
 *
 */
typedef struct
{
    Ptr                 addr[FVID2_MAX_FIELDS][FVID2_MAX_PLANES];
    /**<
         FVID2 buffer pointers for supporting multiple addresses like
         Y, U, V etc for a given frame. The interpretation of these pointers
         depend on the format configured for the driver.
         The first dimension represents the field and the second dimension
         represents the color plane.
         Not all pointers are valid for a given format.

         Representation of YUV422 Planar Buffer:
         Field 0 Y -> addr[0][0], Field 1 Y -> addr[1][0]
         Field 0 U -> addr[0][1], Field 1 U -> addr[1][1]
         Field 0 V -> addr[0][2], Field 1 V -> addr[1][2]
         Other pointers are not valid.

         Representation of YUV422 Interleaved Buffer:
         Field 0 YUV -> addr[0][0], Field 1 YUV -> addr[1][0]
         Other pointers are not valid.

         Representation of YUV420SP or YUV422SP Buffer:
         Field 0 Y  -> addr[0][0], Field 1 Y  -> addr[1][0]
         Field 0 UV -> addr[0][1], Field 1 UV -> addr[1][1]
         Other pointers are not valid.

         Representation of RGB888 Buffer
         Field 0 RGB -> addr[0][0], Field 1 RGB -> addr[1][0],
         Other pointers are not valid.

         For progressive mode data formats
         or when fields are processed by driver in frame mode.
         addr[1][x] is not used.

         Instead of using numerical for accessing the buffers, the application
         can use the macros defined for each buffer formats like
         FVID2_YUV_INT_ADDR_IDX, FVID2_RGB_ADDR_IDX, FVID2_FID_TOP etc. */

    UInt32              fid;
    /**< Indicates whether this frame belong to top or bottom field.
         For valid values see #FVID2_Fid. */
    UInt32              channelNum;
    /**< Channel number to which this FVID2 frame belongs to. */

    UInt32              timeStamp;
    /**< Time stamp returned by the driver, in units of msecs.
         Only valid for frames received using FVID2_dequeue(). */
    Ptr                 appData;
    /**< Additional application parameter per frame. This is not modified by
         driver. */

    Ptr                 perFrameCfg;
    /**< Per frame configuration parameters like scaling ratio, positioning,
         cropping etc...
         This should be set to NULL if not used.

         This can be used by application to control driver behaviour on a per
         frame basis, example changing scaling ratio for scalar driver.

         This can be used by application to get per frame status, example
         detected frame width, height from capture driver.

         This could be set to NULL if not used. In this case, the driver will
         use the last supplied configuration.

         The exact structure type that is passed is driver specific. */

    Ptr                 blankData;
    /**< Blanking data associated with this video frame.
         This could be set to NULL if not used. */

    Ptr                 drvData;
    /**< Used by driver. Application should not modify this. */

    FVID2_SubFrameInfo    *subFrameInfo;
    /**< Used for SubFrame level processing information exchange between
         application and driver.
         This could be set to NULL if sub-frame level processing  is not used. */

    Ptr                 reserved;
    /**< For future use. Not used currently. Set this to NULL. */

} FVID2_Frame;

/**
 *  \brief FVID2 Mode information structure.
 */
typedef struct
{
    UInt32              standard;
    /**< [IN] Standard for which to get the info.
         For valid values see #FVID2_Standard. */
    UInt32              width;
    /**< Active video frame width in pixels. */
    UInt32              height;
    /**< Active video frame height in lines. */
    UInt32              scanFormat;
    /**< Scan format of standard. For valid values see #FVID2_ScanFormat. */
    UInt32              pixelClock;
    /**< Pixel clock of standard in KHz. This assumes 8-bit interface for
         NTSC/PAL/480I/576I resolutions and 16/24-bit interface for other
         resolutions including 480P and 576P. */
    UInt32              fps;
    /**< Frames per second. */
    UInt32              hFrontPorch;
    /**< Horizontal front porch. Same for both fields in case of interlaced
         display. */
    UInt32              hBackPorch;
    /**< Horizontal back porch. */
    UInt32              hSyncLen;
    /**< Horizontal sync length. Same for both fields in case of interlaced
         display. */
    UInt32              vFrontPorch;
    /**< Vertical front porch for each field or frame. */
    UInt32              vBackPorch;
    /**< Vertical back porch for each field or frame. */
    UInt32              vSyncLen;
    /**< Vertical sync length for each field. */
    UInt32              reserved[4u];
    /**< For future use. Not used currently. */
} FVID2_ModeInfo;

/**
 *  \brief FVID2 callback that is called by subframe mode Capture driver.
 *
 *  This callback is called for every subframe of frame that
 *  is captured. This function is used by Capture Driver and not meant for M2M
 *  drivers.
 *
 *  Users should apply same rules as that of FVID2_CbFxn when implementing
 *  subframe callbacks.
 *
 *  \param handle       [OUT] FVID2 handle for which this callback happened.
 *  \param subFrameInfo    [OUT] SubFrame information.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
typedef Int32 (*FVID2_SubFrameCbFxn) (FVID2_Handle handle,
                                      FVID2_Frame *subFrame);

/**
 *  \brief FVID2 frame buffer list used to exchange multiple FVID2
 *  frames in a single driver call.
 *
 *  Unless specified otherwise, all fields in this structure are
 *
 *  [IN]  for FVID2_queue(), FVID2_processFrames() operation.
 *  [OUT] for FVID2_dequeue(), FVID2_getProcessedFrames() operation.
 *
 */
typedef struct
{
    FVID2_Frame        *frames[FVID2_MAX_FVID_FRAME_PTR];
    /**< Array of FVID2_Frame pointers that are to given or received from the
         driver. */

    UInt32              numFrames;
    /**< Number of frames that are given or received from the driver
       i.e number of valid pointers in the array containing FVID2_Frame
       pointers. */

    Ptr                 perListCfg;
    /**< Per list configuration parameters like scaling ratio, positioning,
         cropping etc which are applicable for the frames together.

         This could be set to NULL if not used. In this case, the driver will
         use the last supplied configuration.

         The exact structure type that is passed is driver specific. */

    Ptr                 drvData;
    /**< Used by driver. Application should not modify this. */

    Ptr                 reserved;
    /**< For future use. Not used currently. Set this to NULL. */

    Ptr                 appData;
    /**< Additional application parameter per frame. This is not modified by
         driver. */

} FVID2_FrameList;

/**
 *  \brief FVID2 process list containing frame list's used to exchange multiple
 *  input/output buffers in M2M (memory to memory) operation.
 *
 *  Each of the frame list in turn can have multiple frames/request.
 *
 *  This is typically used by FVID2_processFrames(), FVID2_getProcessedFrames().
 */
typedef struct
{
    FVID2_FrameList    *inFrameList[FVID2_MAX_IN_OUT_PROCESS_LISTS];
    /**< Pointer to an array of FVID2 frame list pointers.

         Each frame list points to a seqeuence of input frame pointers
         that are to be processed.

         For drivers that take multiple input there could be more that one
         input frame list. Example, noise filter driver takes two frame as
         inputs, current frame and previous output frame.
         In this case numInList = 2.

         [IN]  for FVID2_processFrames()
         as well as FVID2_getProcessedFrames() operation.

         For FVID2_processFrames(), the frame pointers that the frame list
         points to should be provided by the application.

         For FVID2_getProcessedFrames(), the frame pointers that the frame list
         points to are filled by the driver and returned to the application. */

    FVID2_FrameList    *outFrameList[FVID2_MAX_IN_OUT_PROCESS_LISTS];
    /**< Pointer to an array of FVID2 frame list pointers.

         Each frame list points to a seqeuence of output frame pointers
         that point to where the output should go after processing.

         For drivers that generate multiple outputs there could be more that one
         output frame list. Example, deinterlacer driver outputs two frames as
         for one input frame, VIP-SC output frame and DEI-SC output frame.
         In this case numOutList = 2.

         [IN]  for FVID2_processFrames()
         as well as FVID2_getProcessedFrames() operation.

         For FVID2_processFrames(), the frame pointers that the frame list
         points to should be provided by the application.

         For FVID2_getProcessedFrames(), the frame pointers that the frame
         list points to are filled by the driver and returned to the
         application. */

    UInt32              numInLists;
    /**< Number of input frame lists.

         [IN]  for FVID2_processFrames()
         [OUT]  for FVID2_getProcessedFrames(). */
    UInt32              numOutLists;
    /**< Number of output frame lists.

         [IN]  for FVID2_processFrames()
         [OUT]  for FVID2_getProcessedFrames(). */

    Ptr                 drvData;
    /**< Used by driver. Application should not modify this. */

    Ptr                 reserved;
    /**< For future use. Not used currently. Set this to NULL. */

} FVID2_ProcessList;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief FVID2 init function.
 *
 *  Initializes the drivers and the hardware.
 *  This function should be called before calling any of driver API's and
 *  should be called only once.
 *
 *  \param args         [IN] Not used currently. Set to NULL.
 *
 *  \return FVID2_SOK on success else appropiate FVID2 error code on failure.
 */
Int32 FVID2_init(Ptr args);

/**
 *  \brief FVID2 deinit function.
 *
 *  Uninitializes the drivers and the hardware and should be called during
 *  system shutdown. Should not be called if FVID2_init() is not called.
 *
 *  \param args         [IN] Not used currently. Set to NULL.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_deInit(Ptr args);

/**
 *  \brief Get the version string for the drivers on FVID2 interface.
 *
 *  This is the version number for all the drivers on FVID2 interface. This
 *  function can be called prior to FVID2_init to get the version number.
 *
 *  \return pointer to HDVPSS version string
 */
const Char *FVID2_getVersionString(void);

/**
 *  \brief Same as FVID2_getVersionString() except it returns the version in
 *  UInt32 form.
 *
 *  This is the version number for all the driver on FVID2 interface. This
 *  function can be called prior to FVID2_init to get the version number.
 *
 *  Example, v1.0.1.17 will be 0x01000117
 *
 *  \return HDVPSS version number
 */
UInt32 FVID2_getVersionNumber();

/**
 *  \brief Creates the driver identified by the driver ID.
 *
 *  This will allocate HW and/or SW resources and return a FVID2_Handle
 *  for this driver. This handle will be used for subsequent FVID2 API calls
 *
 *  \param drvId        [IN] Driver to open. Driver ID is driver specific.
 *
 *  \param instanceId   [IN] Instance of the driver to open and is used
 *                      to differentiate multiple instance support on a
 *                      single driver. Instance ID is driver specific.
 *
 *  \param createArgs   [IN] Pointer to the create argument structure. The type
 *                      of the structure is defined by the specific driver.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \param createStatusArgs [OUT] Pointer to status argument structure where the
 *                      driver returns any status information. The type
 *                      of the structure is defined by the specific driver.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \param cbParams     [IN] Application callback parameters.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \return non-NULL FVID2_Handle object pointer on success else returns NULL
 *  on error.
 */
FVID2_Handle FVID2_create(UInt32 drvId,
                          UInt32 instanceId,
                          Ptr createArgs,
                          Ptr createStatusArgs,
                          const FVID2_CbParams *cbParams);

/**
 *  \brief Deletes a previously created FVID2 driver handle.
 *
 *  This free's the HW/SW resources allocated during create
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *
 *  \param deleteArgs   [IN] Pointer to the delete argument structure. The type
 *                      of the structure is defined by the specific driver.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_delete(FVID2_Handle handle, Ptr deleteArgs);

/**
 *  \brief Send control commands (IOCTLs) to the driver.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *
 *  \param cmd          [IN] IOCTL command. The type of command supported
 *                      is defined by the specific driver.
 *
 *  \param cmdArgs      [IN] Pointer to the command argument structure. The type
 *                      of the structure is defined by the specific driver
 *                      for each of the supported IOCTL.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \param cmdStatusArgs [OUT] Pointer to status argument structure where the
 *                      driver returns any status information. The type
 *                      of the structure is defined by the specific driver
 *                      for each of the supported IOCTL.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_control(FVID2_Handle handle,
                    UInt32 cmd,
                    Ptr cmdArgs,
                    Ptr cmdStatusArgs);

/**
 *  \brief An application calls FVID2_queue to submit a video buffer to the
 *  video device driver.
 *
 *  - This is used in capture/display drivers.
 *  - This function could be called from task or ISR context unless the specific
 *  driver restricts from doing so.
 *  - This is a non-blocking API unless the specific driver restricts from
 *  doing so.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param frameList    [IN] Pointer to the frame list structure containing the
 *                      information about the FVID2 frames that has to be
 *                      queued in the driver.
 *  \param streamId     [IN] Stream ID to which the frames should be queued
 *                      This is used in drivers where they could support
 *                      multiple streams for the same handle. Otherwise this
 *                      should be set to zero.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_queue(FVID2_Handle handle,
                  FVID2_FrameList *frameList,
                  UInt32 streamId);

/**
 *  \brief An application calls FVID2_dequeue to request the video device
 *  driver to give ownership of a video buffer.
 *
 *  - This is used in capture/display drivers.
 *  - This is a non-blocking API if timeout is FVID2_TIMEOUT_NONE and could be
 *  called by task and ISR context unless the specific driver restricts from
 *  doing so.
 *  - This is blocking API if timeout is FVID2_TIMEOUT_FOREVER if supported by
 *  specific driver implementation.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param frameList    [OUT] Pointer to the frame list structure where the
 *                      dequeued frame pointer will be stored.
 *  \param streamId     [IN] Stream ID from where frames should be dequeued.
 *                      This is used in drivers where it could support multiple
 *                      streams for the same handle. Otherwise this
 *                      should be set to zero.
 *  \param timeout      [IN] FVID2 timeout in units of OS ticks. This will
 *                      determine the timeout value till the driver will block
 *                      for a free or completed buffer is available.
 *                      For non-blocking drivers this parameter might be
 *                      ignored.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_dequeue(FVID2_Handle handle,
                    FVID2_FrameList *frameList,
                    UInt32 streamId,
                    UInt32 timeout);

/**
 *  \brief An application calls FVID2_processFrames to submit a video buffer
 *  to the video device driver.
 *
 *  This API is very similar to the FVID2_queue API except that this is
 *  used in M2M drivers only.
 *  - This function could be called from task or ISR context unless the specific
 *  driver restricts from doing so.
 *  - This is a non-blocking API unless the specific driver restricts from
 *  doing so.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param processList  [IN] Pointer to the process list structure containing
 *                      the information about the FVID2 frame lists and frames
 *                      that has to be queued to the driver for processing.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_processFrames(FVID2_Handle handle,
                          FVID2_ProcessList *processList);

/**
 *  \brief An application calls FVID2_getProcessedFrames to request the video
 *  device driver to give ownership of a video buffer.
 *
 *  This API is very similar to the FVID2_dequeue API except that this is
 *  used in M2M drivers only.
 *  - This is a non-blocking API if timeout is FVID2_TIMEOUT_NONE and could be
 *  called by task and ISR context unless the specific driver restricts from
 *  doing so.
 *  - This is blocking API if timeout is FVID2_TIMEOUT_FOREVER if supported by
 *  specific driver implementation.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param processList  [OUT] Pointer to the process list structure where the
 *                      driver will copy the references to the dequeued FVID2
 *                      frame lists and frames.
 *  \param timeout      [IN] FVID2 timeout. This will determine the timeout
 *                      value till the driver will block for a free or completed
 *                      buffer is available. For non-blocking drivers this
 *                      parameter might be ignored.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_getProcessedFrames(FVID2_Handle handle,
                               FVID2_ProcessList *processList,
                               UInt32 timeout);

/**
 *  \brief An application calls FVID2_start to request the video device
 *  driver to start the video display or capture operation.
 *  This function should be called from task context only and should not be
 *  called from ISR context.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param cmdArgs      [IN] Pointer to the start argument structure. The type
 *                      of the structure is defined by the specific driver.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
static inline Int32 FVID2_start(FVID2_Handle handle, Ptr cmdArgs)
{
    return FVID2_control(handle, FVID2_START, cmdArgs, NULL);
}

/**
 *  \brief An application calls FVID2_stop to request the video device
 *  driver to stop the video display or capture operation.
 *  This function should be called from task context only and should not be
 *  called from ISR context.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param cmdArgs      [IN] Pointer to the stop argument structure. The type
 *                      of the structure is defined by the specific driver.
 *                      This parameter could be NULL depending on whether the
 *                      actual driver forces it or not.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
static inline Int32 FVID2_stop(FVID2_Handle handle, Ptr cmdArgs)
{
    return FVID2_control(handle, FVID2_STOP, cmdArgs, NULL);
}

/**
 *  \brief An application calls FVID2_setFormat to request the video device
 *  driver to set the format for a given channel.
 *  This function should be called from task context only and should not be
 *  called from ISR context.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param fmt          [IN] Pointer to the FVID2 format structure.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
static inline Int32 FVID2_setFormat(FVID2_Handle handle, FVID2_Format *fmt)
{
    return FVID2_control(handle, FVID2_SET_FORMAT, fmt, NULL);
}

/**
 *  \brief An application calls FVID2_getFormat to request the video device
 *  driver to get the current format for a given channel.
 *  This function should be called from task context only and should not be
 *  called from ISR context.
 *
 *  \param handle       [IN] FVID2 handle returned by create call.
 *  \param fmt          [OUT] Pointer to the FVID2 format structure.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
static inline Int32 FVID2_getFormat(FVID2_Handle handle, FVID2_Format *fmt)
{
    return FVID2_control(handle, FVID2_GET_FORMAT, fmt, NULL);
}

/**
 *  \brief Function to get the information about various FVID2 modes/standards.
 *
 *  \param modeInfo     [OUT] Pointer to #FVID2_ModeInfo structure where
 *                            the information is filled.
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 FVID2_getModeInfo(FVID2_ModeInfo *modeInfo);

/**
 *  \brief Function to get the name of the data format in printable string.
 *
 *  \param dataFmt      [IN] Data format to get the name.
 *                           For valid values see #FVID2_DataFormat.
 *
 *  \return Returns a const pointer to the string. If the data format is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *FVID2_getDataFmtString(UInt32 dataFmt);

/**
 *  \brief Function to get the name of the standard in printable string.
 *

 *  \param standard     [IN] Standard to get the name.
 *                           For valid values see #FVID2_Standard.
 *
 *  \return Returns a const pointer to the string. If the standard is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *FVID2_getStandardString(UInt32 standard);

/**
 *  \brief Function to check whether a data format is YUV422.
 *
 *  \param dataFmt      [IN] Data format to check.
 *                           For valid values see #FVID2_DataFormat.
 *
 *  \return Returns TRUE if data format is YUV422, FALSE otherwise.
 */
static inline Int32 FVID2_isDataFmtYuv422(UInt32 dataFmt)
{
    Int32       retVal = FALSE;

    switch (dataFmt)
    {
        case FVID2_DF_YUV422I_UYVY:
        case FVID2_DF_YUV422I_YUYV:
        case FVID2_DF_YUV422I_YVYU:
        case FVID2_DF_YUV422I_VYUY:
        case FVID2_DF_YUV422SP_UV:
        case FVID2_DF_YUV422SP_VU:
        case FVID2_DF_YUV422P:
            retVal = TRUE;
            break;
    }

    return (retVal);
}

/**
 *  \brief Function to check whether a data format is YUV420.
 *
 *  \param dataFmt      [IN] Data format to check.
 *                           For valid values see #FVID2_DataFormat.
 *
 *  \return Returns TRUE if data format is YUV420, FALSE otherwise.
 */
static inline Int32 FVID2_isDataFmtYuv420(UInt32 dataFmt)
{
    Int32       retVal = FALSE;

    switch (dataFmt)
    {
        case FVID2_DF_YUV420SP_UV:
        case FVID2_DF_YUV420SP_VU:
        case FVID2_DF_YUV420P:
            retVal = TRUE;
            break;
    }

    return (retVal);
}

/**
 *  \brief Function to check whether a data format is semi-planar.
 *
 *  \param dataFmt      [IN] Data format to check.
 *                           For valid values see #FVID2_DataFormat.
 *
 *  \return Returns TRUE if data format is semi-planar, FALSE otherwise.
 */
static inline Int32 FVID2_isDataFmtSemiPlanar(UInt32 dataFmt)
{
    Int32       retVal = FALSE;

    switch (dataFmt)
    {
        case FVID2_DF_YUV422SP_UV:
        case FVID2_DF_YUV422SP_VU:
        case FVID2_DF_YUV420SP_UV:
        case FVID2_DF_YUV420SP_VU:
            retVal = TRUE;
            break;
    }

    return (retVal);
}

/**
 *  \brief Function to check whether a data format is YUV422 interleaved.
 *
 *  \param dataFmt      [IN] Data format to check.
 *                           For valid values see #FVID2_DataFormat.
 *
 *  \return Returns TRUE if data format is YUV422 interleaved, FALSE otherwise.
 */
static inline Int32 FVID2_isDataFmtYuv422I(UInt32 dataFmt)
{
    Int32       retVal = FALSE;

    switch (dataFmt)
    {
        case FVID2_DF_YUV422I_UYVY:
        case FVID2_DF_YUV422I_YUYV:
        case FVID2_DF_YUV422I_YVYU:
        case FVID2_DF_YUV422I_VYUY:
            retVal = TRUE;
            break;
    }

    return (retVal);
}

/**
 *  \brief Function to check whether a standard is VESA mode or not.
 *
 *  \param standard     [IN] Standard to check.
 *                           For valid values see #FVID2_Standard.
 *
 *  \return Returns TRUE if standard is VESA, FALSE otherwise.
 */
static inline Int32 FVID2_isStandardVesa(UInt32 standard)
{
    Int32       retVal = FALSE;

    switch (standard)
    {
        case FVID2_STD_VGA_60:
        case FVID2_STD_VGA_72:
        case FVID2_STD_VGA_75:
        case FVID2_STD_VGA_85:
        case FVID2_STD_SVGA_60:
        case FVID2_STD_SVGA_72:
        case FVID2_STD_SVGA_75:
        case FVID2_STD_SVGA_85:
        case FVID2_STD_WSVGA_70:
        case FVID2_STD_XGA_60:
        case FVID2_STD_XGA_70:
        case FVID2_STD_XGA_75:
        case FVID2_STD_XGA_85:
        case FVID2_STD_WXGA_60:
        case FVID2_STD_WXGA_75:
        case FVID2_STD_WXGA_85:
        case FVID2_STD_SXGA_60:
        case FVID2_STD_SXGA_75:
        case FVID2_STD_SXGA_85:
        case FVID2_STD_SXGAP_60:
        case FVID2_STD_SXGAP_75:
        case FVID2_STD_UXGA_60:
            retVal = TRUE;
            break;
    }

    return (retVal);
}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef  _FVID2_H */

/* @} */
