/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_DISPLAY Display API
 *
 * @{
 */

/**
 *  \file vps_display.h
 *
 *  \brief Display API
 */

#ifndef _VPS_DISPLAY_H
#define _VPS_DISPLAY_H

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

/*
 *  Display driver IOCTLs.
 */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_DISPLAY
  @{
*/

/**
 *  \brief Get display status IOCTL.
 *
 *  This IOCTL can be used to get the display status like number of frames
 *  displayed, number of frames repeated, number of frames queued/dequeued.
 *  Note: These counters will be reset either at the time of driver create or
 *  while starting the display operation. See respective counter comments for
 *  details.
 *
 *  \param cmdArgs       [OUT] Vps_DispStatus *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DISP_GET_STATUS       (VPS_DISP_IOCTL_BASE + 0x0001u)

/**
 *  \brief Set DEI display params IOCTL.
 *
 *  This IOCTL can be used to set the DEI display params.
 *  Note: See respective counter comments for details.
 *
 *  \param cmdArgs       [IN] const Vps_DeiDispParams *
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DEI_DISP_SET_PARAMS   (VPS_DISP_IOCTL_BASE + 0x0002u)

/**
 *  \brief Set the Display Mode.
 *
 *  This IOCTL can be used to set the display. Display can be
 *  either streaming mode or FrameBuffer Mode.
 *  Note: See respective counter comments for details.
 *
 *  \param cmdArgs       [IN] const Vps_DispMode *
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DISP_SET_DISP_MODE    (VPS_DISP_IOCTL_BASE + 0x0003u)

/**
 *  \brief Get the Display Mode.
 *
 *  This IOCTL can be used to get the current display mode.
 *  Note: See respective counter comments for details.
 *
 *  \param cmdArgs       [IN] const Vps_DispMode *
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DISP_GET_DISP_MODE    (VPS_DISP_IOCTL_BASE + 0x0004u)

/* @} */

/*
 *  Macros for different driver instance numbers to be passed as instance ID
 *  at the time of driver create.
 *  Note: These are read only macros. Don't modify the value of these macros.
 */
/** \brief Bypass path 0 display driver instance number. */
#define VPS_DISP_INST_BP0                        (0u)
/** \brief Bypass path 1 display driver instance number. */
#define VPS_DISP_INST_BP1                        (1u)
/** \brief Secondary path SD display driver instance number. */
#define VPS_DISP_INST_SEC1                       (2u)
/** \brief DEI HQ MAIN path display driver instance number.
 *  Valid only for TI816x Platform.
 */
#define VPS_DISP_INST_MAIN_DEIH_SC1              (3u)
/** \brief DEI AUX path display driver instance number.
 *  Valid only for TI816x Platform.
 */
#define VPS_DISP_INST_AUX_DEI_SC2                (4u)
/** \brief DEI MAIN path display driver instance number.
 *  Valid only for TI814x Platform.
 */
#define VPS_DISP_INST_MAIN_DEI_SC1               (5u)
/** \brief SC2 AUX display driver instance number.
 *  Valid only for TI814x Platform.
 */
#define VPS_DISP_INST_AUX_SC2                    (6u)

/**
 *  \brief Maximum number of display driver instance.
 *  2 bypass paths, 1 main path, 1 aux path and 1 SD display through
 *  secondary path 1.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_DISPLAY_INST_MAX            (5u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_DispCreateParams
 *  \brief Display driver create parameter structure to be passed to the
 *  driver at the time of display driver create call.
 */
typedef struct
{
    UInt32                  memType;
    /**< VPDMA Memory type. For valid values see #Vps_VpdmaMemoryType. */
    UInt32                  periodicCallbackEnable;
    /**< TRUE: User callback passed during FVID2 create is called periodically.
         For progressive display, this interval is equal to VSYNC interval.
         For interlaced display, this interval is equal to twice the VSYNC
         interval as frames (two fields) are queued to the driver.
         FALSE: User callback passed during FVID2 create is called only
         if one or more frames (requests) are available in the driver output
         queue for the application to dequeue. */
} Vps_DispCreateParams;

/**
 *  struct Vps_DispMode
 *  \brief Display Driver mode.
 */
typedef struct
{
    UInt32 isFbMode;
    /**< Flag to indicate whether Frame Buffer mode is enabled or not.
         Note, when Frame Buffer mode is enabled, there is not support
         for RT Params change. */
} Vps_DispMode;

/**
 *  struct Vps_DeiDispParams
 *  \brief Dei Display driver  parameter structure to be passed to the
 *  driver at the time of IOCTL IOCTL_VPS_DEI_DISP_SET_PARAMS call .
 */
typedef struct
{
    FVID2_Format            fmt;
    /**< Input FVID frame format. */
    UInt32                  drnEnable;
    /**< Enables/disables the DRN module in the DEI path. DRN is not available
         in TI814X platform, hence this is not used for TI814X platform. */
    Vps_ScConfig            scCfg;
    /**< Scalar parameters like scEnable, peakingEnable etc. for the
         scalar in DEI path. */
    Vps_DeiHqConfig        *deiHqCfg;
    /**< Pointer to the high quality deinterlacer configuration used for
      *  DEI HQ drivers. This parameter should be set to NULL for others. */
    Vps_DeiConfig          *deiCfg;
    /**< Pointer to the deinterlacer configuration used for
      *  DEI drivers. This parameter should be set to NULL for
      *  DEI HQ drivers. */
    Vps_CropConfig         *deiScCropCfg;
    /**< Cropping configuration for the DEI scalar. */
    Vps_PosConfig          *posCfg;
    /**< Position configuration used to position the PIP window.
         Only used if VCOMP is available in the display path,
         Otherwise it is ignored. */
    UInt32                  scTarWidth;
    /**< Target image Width(o/p of scalar).User should provide
         this data even if scalar is in bypass. If scalar is in bypass
         this is used to program the VPDMA, if scalar is enabled scalar IP
         will be programmed to output this resolution */
    UInt32                  scTarHeight;
    /**< Target image Height(o/p of scalar).User should provide
         this data even if scalar is in bypass. If scalar is in bypass
         this is used to program the VPDMA, if scalar is enabled scalar IP
         will be programmed to output this resolution */
    UInt32                  comprEnable;
    /**< Enable compression/decompression for writing back the DEI context
         to and from DDR. COMPR is not available in TI814X/TI813X platform,
         hence this is not used for above platforms. And for TI816X, this
         feature is not supported. Hence this flag should be always set to
         FALSE. */
} Vps_DeiDispParams;

/**
 *  sturct Vps_DispCreateStatus
 *  \brief Create status for the display driver create call. This should be
 *  passed to the create function as a create status argument while creating
 *  the driver.
 */
typedef struct
{
    Int32                   retVal;
    /**< Return value of create call. */
    UInt32                  standard;
    /**< VENC Standard like NTSC, 1080p etc to which the display driver
         path is connected. For valid values see #FVID2_Standard. */
    UInt32                  dispWidth;
    /**< Width of the display at the VENC in pixels to which the display driver
         path is connected. */
    UInt32                  dispHeight;
    /**< Height of the display at the VENC in linesto which the display driver
         path is connected. */
    UInt32                  minNumPrimeBuf;
    /**< Minimum number of buffers to prime before starting display operation.*/
    UInt32                  maxReqInQueue;
    /**< Maximum number of request per driver instance that can be submitted
         for display without having to dequeue the displayed requests. */
    UInt32                  maxMultiWin;
    /**< Maximum number of multiple windows supported for the opened
         instance. If this value is equal to 1, then multiple window display
         is not supported by the driver instance.
         Note: The maximum number of windows/col/row is after the splitting of
         the windows according to the VPDMA requirement. So from the
         application point of view, this could be less than returned value
         depending on the layout selected. */
    UInt32                  maxMultiWinCol;
    /**< Maximum number of columns supported in multiple window mode. */
    UInt32                  maxMultiWinRow;
    /**< Maximum number of rows supported in multiple window mode. */
    UInt32                  maxMultiWinLayout;
    /**< Maximum number of multiple window layout that could be created to
         support dynamic layout change at runtime. */
} Vps_DispCreateStatus;

/**
 *  struct Vps_DispRtParams
 *  \brief Run time configuration structure for the display driver.
 *  This needs to be passed along with frame list to update any supported
 *  run time parameters.
 */
typedef struct
{
    Vps_LayoutId           *layoutId;
    /**< ID of the layout to be selected. This should be a valid layout ID
         as returned by create multi window layout IOCTL. When layout ID is
         NULL, the driver will ignore this runtime parameter
         and continue processing the submitted request. */
    Vps_CropConfig         *vcompCropCfg;
    /**< VCOMP crop configuration to crop the PIP window.
         Pass NULL if no change is required or VCOMP is not present in the
         display path. If application passes non-NULL when VCOMP is not present
         in the display path, then this runtime parameter will be ignored. */
    Vps_PosConfig          *vcompPosCfg;
    /**< VCOMP position configuration used to position the PIP window after
         cropping.
         Pass NULL if no change is required or VCOMP is not present in the
         display path. If application passes non-NULL when VCOMP is not present
         in the display path, then this runtime parameter will be ignored. */
    Vps_FrameParams        *inFrmPrms;
    /**< Frame params for input frame - used to change the frame width and
         height at runtime.
         Note that this is used only in non-mosaic mode when the buffer
         dimension is smaller than the VENC size.
         When changing the size, the application should ensure that the
         startX/startY + the frame size doesn't exceed the display resolution.
         Pass NULL if no change is required. */
    Vps_PosConfig          *vpdmaPosCfg;
    /**< VPDMA position configuration containing startX and startY.
         Note that this is used only in non-mosaic mode when the buffer
         dimension is smaller than the VENC size.
         When changing the position, the application should ensure that the
         startX/startY + the frame size doesn't exceed the display resolution.
         Pass NULL if no change is required. */
} Vps_DispRtParams;

/**
 *  struct Vps_DeiDispRtParams
 *  \brief Run time configuration structure for the dei display driver.
 *  This needs to be passed along with frame list to update any supported
 *  run time parameters.
 */
typedef struct
{
    Vps_LayoutId           *layoutId;
    /**< ID of the layout to be selected. This should be a valid layout ID
         as returned by create multi window layout IOCTL. When layout ID is
         NULL, the driver will ignore this runtime parameter
         and continue processing the submitted request.
         Not supported */
    Vps_CropConfig         *vcompCropCfg;
    /**< VCOMP crop configuration to crop the PIP window.
         Pass NULL if no change is required or VCOMP is not present in the
         display path. If application passes non-NULL when VCOMP is not present
         in the display path, then this runtime parameter will be ignored.
         Not supported */
    Vps_PosConfig          *posCfg;
    /**< Position configuration used to position the PIP window.
         Pass NULL if no change is required or VCOMP is not present in the
         display path. If application passes non-NULL when VCOMP is not present
         in the display path, then this runtime parameter will be ignored. */
    Vps_FrameParams        *inFrmPrms;
    /**< Frame params for input frame - used to change the frame width and
         height at runtime.
         Note that this is used only in non-mosaic mode when the buffer
         dimension is smaller than the VENC size.
         When changing the size, the application should ensure that the
         startX/startY + the frame size doesn't exceed the display resolution.
         Pass NULL if no change is required. */
    Vps_CropConfig         *deiScCropCfg;
    /**< Cropping configuration for the DEI scalar. */
    Vps_FrameParams        *outFrmPrms;
    /**< Target image Size (o/p of scalar).
         Pass NULL if no change is required. */
    Vps_DeiRtConfig        *deiRtCfg;
    /**< DEI Rt Configuration, contains DEI params like DEI/FMD/EDI/MDT mode etc.
         Also has parameter to reset the DEI context.
         Not Supported */
} Vps_DeiDispRtParams;


/**
 *  struct Vps_DispStatus
 *  \brief Display status structure used to get the current status.
 */
typedef struct
{
    UInt32                  queueCount;
    /**< Counter to keep track of how many requests are queued to the driver.
         Note: This counter will be reset at the time of driver create. */
    UInt32                  dequeueCount;
    /**< Counter to keep track of how many requests are dequeued from the
         driver.
         Note: This counter will be reset at the time of driver create. */
    UInt32                  displayedFrameCount;
    /**< Counter to keep track of how many frames are displayed. For interlaced
         display, this is half of the actual field display.
         Note: This counter will be reset at the time of display start. */
    UInt32                  repeatFrameCount;
    /**< Counter to keep track of how many frames are repeated when the
         application fails to queue buffer at the display rate.
         Note: This counter will be reset at the time of display start. */
} Vps_DispStatus;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_DISPLAY_H */

/* @} */
