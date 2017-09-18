/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_VPS_COMMON HD-VPSS - Common API
 *
 * @{
 */

/**
 *  \file vps.h
 *
 *  \brief HD-VPSS - Common API
 */

#ifndef _VPS_H
#define _VPS_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/fvid2.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief HDVPSS driver version string. */
#define VPS_VERSION_STRING              "HDVPSS_01_00_01_37"

/** \brief HDVPSS driver version number. */
#define VPS_VERSION_NUMBER              (0x01000137u)

/** \brief Driver ID base for the display driver class. */
#define VPS_DISP_DRV_BASE               (0x00000000u)

/** \brief Driver ID base for the capture driver class. */
#define VPS_CAPT_DRV_BASE               (0x00000100u)

/** \brief Driver ID base for the M2M driver class. */
#define VPS_M2M_DRV_BASE                (0x00000200u)

/** \brief Driver ID base for the Blender based M2M driver class. */
#define VPS_BLEND_DRV_BASE              (0x00000300u)

/** \brief Driver ID base for video decoder driver class. */
#define VPS_VID_DEC_DRV_BASE            (0x00000400u)

/** \brief Driver ID base for video  encoder driver class. */
#define VPS_VID_ENC_DRV_BASE            (0x00000500u)

/** \brief Driver ID base for platform driver class. */
#define VPS_VID_SYSTEM_DRV_BASE         (0x00000600u)

/** \brief Driver ID base for the custom/user driver class. */
#define VPS_USER_DRV_BASE               (0x00001000u)

/** \brief Display controller driver ID used at the time of FVID2 create. */
#define FVID2_VPS_DCTRL_DRV             (VPS_DISP_DRV_BASE + 0x00000000u)
/** \brief Display driver ID used at the time of FVID2 create. */
#define FVID2_VPS_DISP_DRV              (VPS_DISP_DRV_BASE + 0x00000001u)
/** \brief Graphics display driver ID used at the time of FVID2 create. */
#define FVID2_VPS_DISP_GRPX_DRV         (VPS_DISP_DRV_BASE + 0x00000002u)

/** \brief VIP capture driver ID used at the time of FVID2 create. */
#define FVID2_VPS_CAPT_VIP_DRV          (VPS_CAPT_DRV_BASE + 0x00000000u)

/** \brief Noise Filter M2M driver ID used at the time of FVID2 create. */
#define FVID2_VPS_M2M_NSF_DRV           (VPS_M2M_DRV_BASE + 0x00000000u)
/** \brief Scalar Writeback M2M driver ID used at the time of FVID2 create. */
#define FVID2_VPS_M2M_SC_DRV            (VPS_M2M_DRV_BASE + 0x00000001u)
/** \brief DEI HQ/ DEI M2M driver ID used at the time of FVID2 create. */
#define FVID2_VPS_M2M_DEI_DRV           (VPS_M2M_DRV_BASE + 0x00000002u)

/** \brief COMP/Blender M2M driver ID used at the time of FVID2 create. */
#define FVID2_VPS_M2M_COMP_DRV          (VPS_BLEND_DRV_BASE + 0x00000000u)

/*
 *  IOCTLs Base address.
 */
/** \brief IOCTL base address for the IOCTLs common to display, capture and
 *  mem to mem drivers. */
#define VPS_COMMON_IOCTL_BASE           (FVID2_USER_BASE + 0x00000000u)
/** \brief IOCTL base address for the display driver IOCTLs. */
#define VPS_DISP_IOCTL_BASE             (FVID2_USER_BASE + 0x00010000u)
/** \brief IOCTL base address for the capture driver IOCTLs. */
#define VPS_CAPT_IOCTL_BASE             (FVID2_USER_BASE + 0x00020000u)
/** \brief IOCTL base address for the M2M driver IOCTLs. */
#define VPS_M2M_IOCTL_BASE              (FVID2_USER_BASE + 0x00030000u)
/** \brief IOCTL base address for the display controller driver IOCTLs. */
#define VPS_DCTRL_IOCTL_BASE            (FVID2_USER_BASE + 0x00040000u)
/** \brief IOCTL base address for the noise filter M2M driver IOCTLs. */
#define VPS_M2M_NSF_IOCTL_BASE          (FVID2_USER_BASE + 0x00050000u)
/** \brief IOCTL base address for the video decoder driver IOCTLs. */
#define VPS_VID_DEC_IOCTL_BASE          (FVID2_USER_BASE + 0x00060000u)
/** \brief IOCTL base address for the video  encoder driver IOCTLs. */
#define VPS_VID_ENC_IOCTL_BASE          (FVID2_USER_BASE + 0x00070000u)
/** \brief IOCTL base address for the video  platform driver IOCTLs. */
#define VPS_VID_SYSTEM_IOCTL_BASE       (FVID2_USER_BASE + 0x00080000u)
/** \brief IOCTL base address for the M2M SC driver IOCTLs. */
#define VPS_M2M_SC_IOCTL_BASE           (FVID2_USER_BASE + 0x00090000u)
/** \brief IOCTL base address for the M2M DEI driver IOCTLs. */
#define VPS_M2M_DEI_IOCTL_BASE          (FVID2_USER_BASE + 0x000A0000u)
/** \brief IOCTL base address for the advanced debug capabilities.
 *  Caution: To be used with care. */
#define VPS_ADV_IOCTL_BASE              (FVID2_USER_BASE + 0x000B0000u)

/**
 *  \brief VPS video buffer alignment. All application buffer address and
 *  line pitch should be aligned to this byte boundary.
 */
#ifdef TI_8107_BUILD
#define VPS_BUFFER_ALIGNMENT            (16u)
#else
#define VPS_BUFFER_ALIGNMENT            (16u)
#endif
/** \brief Multi window layout ID base for the user created layouts. */
#define VPS_USER_LAYOUT_ID_BASE         (100u)

/**
 *  \brief Maximum number of horizontal slices supported by the driver.
 */
#define VPS_CFG_MAX_NUM_HORZ_SLICE      (20u)

/*
 *  Common IOCTLS for display, M2M and capture drivers
 */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_COMMON
  @{
*/

/**
 *  \brief Set the scalar coefficients.
 *
 *  \param cmdArgs       [IN]  const Vps_ScCoeffParams *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
*/
#define IOCTL_VPS_SET_COEFFS            (VPS_COMMON_IOCTL_BASE + 0x0000u)

/**
 *  \brief Set the GRPX region parameters.
 *
 *  This IOCTL just passes the GRPX region parameter into the driver to
 *  prepare the necessary memory for GRPX.
 *  This IOCTL could be called only after creating the driver and before the
 *  display or M2M operation is started.
 *
 *  \param cmdArgs       [IN]     Vps_GrpxParamsList *
 *  \param cmdStatusArgs [OUT]    NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_SET_GRPX_PARAMS       (VPS_COMMON_IOCTL_BASE + 0x0002u)

/**
 *  \brief Get the GRPX region parameters.
 *
 *  This IOCTL just Return the region parameters back to caller.
 *  This IOCTL could be called after creating the driver and even if the
 *  display or M2M operation is in progress.
 *
 *  \param cmdArgs       [IN]     Vps_GrpxParamsList *
 *  \param cmdStatusArgs [OUT]    NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_GET_GRPX_PARAMS       (VPS_COMMON_IOCTL_BASE + 0x0003u)

/**
 *  \brief Get DEI context information.
 *
 *  \param cmdArgs       [IN]     Vps_DeiCtxInfo *
 *  \param cmdStatusArgs [OUT]    NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_GET_DEI_CTX_INFO      (VPS_COMMON_IOCTL_BASE + 0x0004u)

/**
 *  \brief Set DEI context buffers to driver.
 *
 *  \param cmdArgs       [IN]     const Vps_DeiCtxBuf *
 *  \param cmdStatusArgs [OUT]    NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_SET_DEI_CTX_BUF       (VPS_COMMON_IOCTL_BASE + 0x0005u)

/**
 *  \brief Get DEI context buffers from driver.
 *
 *  \param cmdArgs       [IN]     Vps_DeiCtxBuf *
 *  \param cmdStatusArgs [OUT]    NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_GET_DEI_CTX_BUF       (VPS_COMMON_IOCTL_BASE + 0x0006u)

/**
 *  \brief Creates a mosaic or region based graphic layout depending on the
 *  multiple window parameter.
 *
 *  This IOCTL creates the necessary infrastructure for the specified layout.
 *  When more than one layout is supported by a driver, the user has to call
 *  select multiple window layout IOCTL to explicitly select a particular
 *  layout before starting the display/M2M operation. In this case, this
 *  IOCTL could be called after creating the driver and even if the
 *  display or M2M operation is in progress.
 *
 *  When a driver supports only one layout, then the driver implicitly selects
 *  the created layout and hence select multiple window layout IOCTL needed
 *  not be called/supported by driver. Hence this IOCTL could not be called
 *  when display or M2M operation is in progress.
 *
 *  Refer to individual driver documentation for other details and specific
 *  information.
 *
 *  Note: This also supports PIP/overlapping windows, in which case
 *  application has to specify the priority for each of the window.
 *
 *  \param cmdArgs       [IN]  const Vps_MultiWinParams *
 *  \param cmdStatusArgs [OUT] Vps_LayoutId *
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_CREATE_LAYOUT         (VPS_COMMON_IOCTL_BASE + 0x0007u)

/**
 *  \brief Deletes a mosaic or region based graphic layout created by
 *  IOCTL_VPS_CREATE_LAYOUT IOCTL.
 *
 *  This IOCTL could be called after creating the driver and even if the
 *  display or M2M operation is in progress. When the layout to delete
 *  is used by the current operation or is not created, this returns error.
 *
 *  \param cmdArgs       [IN]  const Vps_LayoutId *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DELETE_LAYOUT         (VPS_COMMON_IOCTL_BASE + 0x0008u)

/**
 *  \brief Selects the already created layout for display or M2M operation.
 *
 *  This IOCTL should be called before starting the display or M2M operation
 *  to select the default layout to start with.
 *  This IOCTL could not be called once the display starts or request is
 *  queued with the M2M driver. For changing the layout after operation is
 *  started, application could do so by passing the layout ID (which is
 *  returned through create layout IOCTL)as a part of runtime parameter if the
 *  specific driver supports this.
 *
 *  \param cmdArgs       [IN]  const Vps_LayoutId *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_SELECT_LAYOUT         (VPS_COMMON_IOCTL_BASE + 0x0009u)

/**
 *  \brief Deletes all the mosaic or region based graphic layouts created by
 *  IOCTL_VPS_CREATE_LAYOUT IOCTL.
 *
 *  This IOCTL could not be called when display is in progress with one of the
 *  created layout or when requests are pending with the M2M driver.
 *
 *  \param cmdArgs       [IN]  NULL
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DELETE_ALL_LAYOUT     (VPS_COMMON_IOCTL_BASE + 0x000Au)

/**
 *  \brief Enable/disable Lazy Loading for Scalar.
 *
 *  \param cmdArgs       [IN]  const Vps_ScLazyLoadingParams *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
*/
#define IOCTL_VPS_SC_SET_LAZY_LOADING   (VPS_COMMON_IOCTL_BASE + 0x000Bu)

/**
 *  \brief Get the horizontal slice configuration.
 *
 *  This ioctl can be used to get the horizontal slice information.
 *
 *  \param cmdArgs       [IN]  const Vps_SubFrameHorzSlcInfo *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_GET_SC_HORZ_SUB_FRAME_INFO (VPS_COMMON_IOCTL_BASE + 0x000Fu)

/**
 *  \brief Reconfigure the Channel information.
 *
 *  This ioctl can be used to recofigure the channel information.
 *
 *  \param cmdArgs       [IN]  const Vps_M2mScChParams * for the scalar driver
 *                             const Vps_M2mDeiChParams * for DEI driver
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_SET_SC_CHANNEL_INFO       (VPS_COMMON_IOCTL_BASE + 0x0010u)

/* @} */

/*
 *  Advanced IOCTLS for display, M2M and capture drivers.
 */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_ADVANCED
  @{
*/

/**
 *  \brief Read the scalar hardware configuration.
 *
 *  This ioctl can be used to read the actual hardware registers of the
 *  scalar.
 *  This is for the advanced user for the hardware debug capability.
 *  For the multiple channel mode of the driver data is returned from the
 *  overlay memory instead of actual hardware registers.
 *
 *  \param cmdArgs       [OUT] Vps_ScRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_READ_ADV_SC_CFG       (VPS_ADV_IOCTL_BASE + 0x0000u)

/**
 *  \brief Write the scalar hardware configuration.
 *
 *  This ioctl can be used to write the actual hardware registers of the
 *  scalar.
 *  This is for the advanced user for the hardware debug capability.
 *  For the multiple channel mode of the driver data is written to the
 *  overlay memory instead of actual hardware registers.
 *
 *  \param cmdArgs       [IN]  const Vps_ScRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_WRITE_ADV_SC_CFG      (VPS_ADV_IOCTL_BASE + 0x0001u)

/**
 *  \brief Read the High Quality De-Interlacer hardware configuration.
 *
 *  This ioctl can be used to read the actual hardware registers of the
 *  High Quality De-interlacer.
 *  This is for the advanced user for the hardware debug capability.
 *
 *  \param cmdArgs       [OUT] Vps_DeiHqRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_READ_ADV_DEIHQ_CFG       (VPS_ADV_IOCTL_BASE + 0x0010u)

/**
 *  \brief Write the High Quality De-Interlacer hardware configuration.
 *
 *  This ioctl can be used to write the actual hardware registers of the
 *  High Quality De-interlacer.
 *  This is for the advanced user for the hardware debug capability.
 *
 *  \param cmdArgs       [IN]  const Vps_DeiHqRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_WRITE_ADV_DEIHQ_CFG      (VPS_ADV_IOCTL_BASE + 0x0011u)

/**
 *  \brief Read the De-Interlacer hardware configuration.
 *
 *  This ioctl can be used to read the actual hardware registers of the
 *  De-interlacer.
 *  This is for the advanced user for the hardware debug capability.
 *
 *  \param cmdArgs       [OUT] Vps_DeiRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_READ_ADV_DEI_CFG       (VPS_ADV_IOCTL_BASE + 0x0012u)

/**
 *  \brief Write the De-Interlacer hardware configuration.
 *
 *  This ioctl can be used to write the actual hardware registers of the
 *  De-interlacer.
 *  This is for the advanced user for the hardware debug capability.
 *
 *  \param cmdArgs       [IN]  const Vps_DeiRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_WRITE_ADV_DEI_CFG      (VPS_ADV_IOCTL_BASE + 0x0013u)

/* @} */

/** \brief Floor a integer value. */
#define VpsUtils_floor(val, align)  (((val) / (align)) * (align))

/** \brief Align a integer value. */
#define VpsUtils_align(val, align)  VpsUtils_floor(((val) + (align)-1), (align))

/** \brief 8-bit Tiler container pitch in bytes. */
#define VPSUTILS_TILER_CNT_8BIT_PITCH   (16u * 1024u)

/** \brief 16-bit Tiler container pitch in bytes. */
#define VPSUTILS_TILER_CNT_16BIT_PITCH  (32u * 1024u)

/** \brief 32-bit Tiler container pitch in bytes. */
#define VPSUTILS_TILER_CNT_32BIT_PITCH  (32u * 1024u)

/**
 *  enum Vps_VpdmaMemoryType
 *  \brief Enum for buffer memory type.
 */
typedef enum
{
    VPS_VPDMA_MT_NONTILEDMEM = 0,
    /**< 1D non-tiled memory. */
    VPS_VPDMA_MT_TILEDMEM,
    /**< 2D tiled memory. */
    VPS_VPDMA_MT_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_VpdmaMemoryType;

/**
 *  enum Vps_MemRotationType
 *  \brief This enum is used to define the memory data rotation and mirroring
 *  type. The rotation and mirroring is only valid if the memory type is tiler
 *  container mode. For non-tiler and tiler page mode, rotation and mirroring
 *  is not valid
 */
typedef enum
{
    VPS_MEM_0_ROTATION = 0,
    /**< no rotation and mirroring. */
    VPS_MEM_180_ROTATION_MIRRORING,
    /**< 180 degree rotation with mirroring. */
    VPS_MEM_0_ROTATION_MIRRORING,
    /**< 0 degree rotation with mirroring. */
    VPS_MEM_180_ROTATION,
    /**< 180 degree rotation. */
    VPS_MEM_270_ROTATION_MIRRORING,
    /**< 270 degree rotation with mirroring. */
    VPS_MEM_270_ROTATION,
    /**< 270 degree rotation. */
    VPS_MEM_90_ROTATION,
    /**< 90 degree rotation. */
    VPS_MEM_90_ROTATION_MIRRORING,
    /**< 90 degree rotation with mirroring. */
    VPS_MEM_ROTATION_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_MemRotationType;

/**
 *  enum Vps_FidPol
 *  \brief Enumerations for Field ID polarity.
 */
typedef enum
{
    VPS_FIDPOL_NORMAL = 0,
    /**< FID = 0, top field */
    VPS_FIDPOL_INVERT
    /**< FID = 0, bottom field */
} Vps_FidPol;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * struct Vps_FrameParams
 * \brief Structure for setting the frame parameters like frame height, width,
 * pitch and memory type.
 */
typedef struct
{
    UInt32                  width;
    /**< Width of frame in pixels. */
    UInt32                  height;
    /**< Height of frame in lines. */
    UInt32                  pitch[FVID2_MAX_PLANES];
    /**< Pitch for each planes in bytes. Only required planes needs to be
         populated. */
    UInt32                  memType;
    /**< VPDMA memory type - Tiled buffer or normal non-tiled buffer.
         For valid values see #Vps_VpdmaMemoryType. */
    UInt32                  dataFormat;
    /**< Frame data Format. For valid values see #FVID2_DataFormat. */
} Vps_FrameParams;

/**
 *  struct Vps_CropConfig
 *  \brief Structure containing crop configuration - used in Scalar and VCOMP.
 */
typedef struct
{
    UInt32                  cropStartX;
    /**< Horizontal offset from which picture needs to be cropped. */
    UInt32                  cropStartY;
    /**< Vertical offset from which picture needs to be cropped. */
    UInt32                  cropWidth;
    /**< Width of the picture to be cropped. */
    UInt32                  cropHeight;
    /**< Height of the picture to be cropped. */
} Vps_CropConfig;

/**
 *  struct Vps_PosConfig
 *  \brief Structure containing position configuration - used in VCOMP and CIG.
 */
typedef struct
{
    UInt32                  startX;
    /**< Horizontal offset from which picture needs to be positioned. */
    UInt32                  startY;
    /**< Vertical offset from which picture needs to be positioned. */
} Vps_PosConfig;

/**
 *  struct Vps_WinFormat
 *  \brief Structure for setting the mosaic or region based graphic
 *  window for each of the window.
 */
typedef struct
{
    UInt32                  winStartX;
    /**< Horizontal offset in pixel (with respect to frame dimension)
         from which window starts. */
    UInt32                  winStartY;
    /**< Vertical offset in line (with respect to frame dimension)
         from which window starts. */
    UInt32                  winWidth;
    /**< Width of window in pixels. */
    UInt32                  winHeight;
    /**< Number of lines in a window. For interlaced mode, this should
         be set to the frame size and not the field size. */
    UInt32                  pitch[FVID2_MAX_PLANES];
    /**< Pitch in bytes for each of the sub-window buffers. This represents the
         difference between two consecutive line address.
         This is irrespective of whether the video is interlaced or
         progressive and whether the fields are merged or separated for
         interlaced video. */
    UInt32                  dataFormat;
    /**< Data format for each window. For valid values see #FVID2_DataFormat.
         Caution: Check the respective driver user/API guide for the
         data formats supported. Some driver may not support
         separate data formats for each window of the frame. */
    UInt32                  bpp;
    /**< Bits per pixels for each window.
         For valid values see #FVID2_BitsPerPixel. */
    UInt32                  priority;
    /**< In case of overlapping windows (as in PIP), priority could be used
         to choose the window to be displayed in the overlapped region.
         0 is highest priority, 1 is next and so on...
         Note that keeping same priority for all windows specifies that there
         are no overlapping windows. */
} Vps_WinFormat;

/**
 *  struct Vps_MultiWinParams
 *  \brief Structure for setting multiple window (mosaic/region based graphics)
 *  parameters.
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Channel number to which this this config belongs to.
         This is used in case of multiple buffers queuing/deqeuing using a
         single call.
         If only one channel is supported, then this should be set to 0. */
    UInt32                  numWindows;
    /**< Specifies the number windows that would require to be
         displayed/processed, winFmt should point to a array that has at least
         numWindows of entries. */
    const Vps_WinFormat    *winFmt;
    /**< Pointer to an array of multiple window format containing the format
         for each of the mosaic or region based graphics window.
         The format array should be filled from left to right and from top to
         bottom for all the windows.
         The size of the array should be equal to total number of windows
         and the memory should be allocated by the application. */
} Vps_MultiWinParams;

/**
 *  struct Vps_LayoutId
 *  \brief Structure for returning the layout ID in create/select/delete
 *  multiple window (mosaic/region based graphics) layout IOCTLs.
 */
typedef struct
{
    UInt32                  layoutId;
    /**< Layout ID as assigned by driver. This should be used as a reference
         by the application to select/delete the required layout.
         Note: Each driver could have different layout ID for a particular
         layout. */
} Vps_LayoutId;

/**
 *  struct Vps_SubFrameParams
 *  \brief Configuration for sub-frame level processing  at create time.
 */
typedef struct
{
    UInt32                  subFrameModeEnable;
    /**< TRUE : SubFrame level processing is done for this channel
         FALSE: Frame level processing is done for this channel. */
    UInt32                  numLinesPerSubFrame;
    /**< Number of lines in a subframe. */
    UInt32                  numPixelsPerLine;
    /**< Number of pixels per line in a frame. */
} Vps_SubFrameParams;

/**
 *  struct Vps_HorzSubFrmCfg
 *  \brief Individual subframe information.
 */
typedef struct
{
    UInt32          srcW;
    /**< Width of the source slice */
    UInt32          tarW;
    /**< Width of the target slice */
} Vps_HorzSubFrmCfg;

/**
 *  struct Vps_SubFrameHorzSlcInfo
 *  \brief Horizontal slice information.
 */
typedef struct
{
    UInt32              chNum;
    /**< Channel number for which horizontal configuration to be set */
    UInt32              subFrmSize;
    /**< Slice size in terms of number of pixels per line */
    UInt32              numSubFrm;
    /**< Number of subframes, output parameter */
    Vps_HorzSubFrmCfg   subFrmCfg[VPS_CFG_MAX_NUM_HORZ_SLICE];
    /**< horizotal slice configuration */
} Vps_SubFrameHorzSubFrmInfo;

/* ========================================================================== */
/* Include HW specific config constants, structures                           */
/* ========================================================================== */

#include <ti/psp/vps/vps_cfgCsc.h>
#include <ti/psp/vps/vps_cfgDei.h>
#include <ti/psp/vps/vps_cfgRf.h>
#include <ti/psp/vps/vps_cfgSc.h>
#include <ti/psp/vps/vps_cfgVipParser.h>
#include <ti/psp/vps/vps_cfgCproc.h>

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Prints to Shared memory and CCS console
 *
 *  This function prints the provided formatted string to shared memory and CCS
 *  console
 *
 *  \param format       [IN] Formatted string followed by variable arguments
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_printf(char * format, ... );

/**
 *  \brief Prints to Shared memory only
 *
 *  This function prints the provided formatted string to shared memory only
 *
 *  \param format       [IN] Formatted string followed by variable arguments
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
Int32 Vps_rprintf(char * format, ... );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_H */

/* @} */

/**
\mainpage  HD-VPSS Drivers

\par IMPORTANT NOTE
     <b>
     The interfaces defined in this package are bound to change.
     Kindly treat the interfaces as work in progress.
     Release notes/user guide list the additional limitation/restriction
     of this module/interfaces.
     </b> See also \ref TI_DISCLAIMER.

HD-VPSS Drivers allow users to make use of all HD-VPSS hardware features like
1080P capture, HDMI/HDDAC/SDTV display, noise filtering, deinterlacing, scaling.
A user can use the drivers in many different ways like multi-channel capture,
memory to memory processing, mosiac display and so on.
This document has detailed API description that user's can use to make use
of the HD-VPSS drivers.

<b>
  Also refer to HD-VPSS driver user guide for detailed features,
  limitations and usage description.
</b>

The HD-VPSS driver API can be broadly divided into the following categories
  - <b> FVID2 API </b> (See \ref VPS_DRV_FVID2_API) <br>
    API used to create, control and use the different HD-VPSS drivers

  - <b> Capture API </b> (See \ref VPS_DRV_FVID2_CAPTURE_API) <br>
    API for video capture drivers

  - <b> Display API </b> (See \ref VPS_DRV_FVID2_DISPLAY) <br>
    API for video display drivers

  - <b> Display Controller API </b> (See \ref VPS_DRV_FVID2_DISPLAY_CTRL) <br>
    API for controlling and configuring VENCs in the display subsystem

  - <b> Graphics API </b> (See \ref VPS_DRV_FVID2_GRAPHICS) <br>
    API for graphics display drivers

  - <b> Memory to Memory (M2M) API </b> <br>
    API for memory to memory drivers
    - See \ref VPS_DRV_FVID2_DEI_API
    - See \ref VPS_DRV_FVID2_SC_API
    - See \ref VPS_DRV_FVID2_NSF_API

  - <b> External device API </b> (See \ref VPS_DRV_FVID2_DEVICE_API) <br>
    API for controlling external video devices like video decoders, video encoders, video filters
    - See \ref VPS_DRV_FVID2_DEVICE_VID_DEC_API
    - See \ref VPS_DRV_FVID2_DEVICE_VID_ENC_API
    - See \ref VPS_DRV_FVID2_DEVICE_VIDEO_FILTERS_API

  - <b> Platform Specific API </b> (See \ref VPSUTILS_PLATFORM_API) <br>
    API for platform/board specific control, like setting pin muxes, clocks etc

Some other drivers that are included in the HD-VPSS package but are not FVID2 APIs
  - <b> I2C API </b> (See \ref PSP_DRV_I2C_API ) <br>
    API used to read, write data over I2C interface. I2C interface is used
    internally by \ref VPS_DRV_FVID2_DEVICE_API

In addition to above drivers the HD-VPSS package also includes many sample examples
which show how to use the drivers in different ways.
Many of these sample examples use a common set of APIs which a user may find useful to
refer to for use in their final application.
  - <b> Utility library API </b> (See \ref VPSUTILS_API ) <br>
    APIs used by sample application for allocating memory, exchanging buffers,
    measuring performamce and so on

  - <b> Links and chains example API </b> (See \ref VPSEXAMPLE_LINKS_AND_CHAIN_API ) <br>
    APIs used by chains application for connecting drivers to each other in other create
    different application scenarios like multi-chanmel capture + display,
    single channel capture + display and so on
*/

/**
 \page  TI_DISCLAIMER  TI Disclaimer

 \htmlinclude ti_disclaim.htm
*/

/**
  \defgroup VPS_DRV_FVID2_IOCTL           HD-VPSS - All IOCTL's
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_FVID2     FVID2 - Common IOCTL's
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_COMMON    HD-VPSS - Common IOCTL's
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_ADVANCED  HD-VPSS - Advanced IOCTL's

*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_CAPTURE VIP Capture IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver

  - \ref FVID2_START
  - \ref FVID2_STOP
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_M2M_DEI M2M DEI IOCTL's
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_M2M_NSF M2M Noise Filter IOCTL's
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_M2M_DEI M2M DEI IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver

  - \ref IOCTL_VPS_SET_COEFFS
  - \ref IOCTL_VPS_GET_DEI_CTX_INFO
  - \ref IOCTL_VPS_SET_DEI_CTX_BUF
  - \ref IOCTL_VPS_GET_DEI_CTX_BUF
  - \ref IOCTL_VPS_READ_ADV_DEIHQ_CFG
  - \ref IOCTL_VPS_WRITE_ADV_DEIHQ_CFG
  - \ref IOCTL_VPS_READ_ADV_DEI_CFG
  - \ref IOCTL_VPS_WRITE_ADV_DEI_CFG
  - \ref IOCTL_VPS_READ_ADV_SC_CFG
  - \ref IOCTL_VPS_WRITE_ADV_SC_CFG
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_M2M_SC M2M Scalar IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver

  - \ref IOCTL_VPS_SET_COEFFS
  - \ref IOCTL_VPS_READ_ADV_SC_CFG
  - \ref IOCTL_VPS_WRITE_ADV_SC_CFG
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_DISPLAY_CTRL    Display Controller IOCTL's
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_DISPLAY    Display IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver

  - \ref IOCTL_VPS_CREATE_LAYOUT
  - \ref IOCTL_VPS_DELETE_LAYOUT
  - \ref IOCTL_VPS_SELECT_LAYOUT
  - \ref IOCTL_VPS_DELETE_ALL_LAYOUT
  - \ref FVID2_SET_FORMAT
  - \ref FVID2_GET_FORMAT
  - \ref FVID2_START
  - \ref FVID2_STOP
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_GRPX       Graphics IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver.

  - \ref IOCTL_VPS_CREATE_LAYOUT
  - \ref IOCTL_VPS_SET_GRPX_PARAMS
  - \ref IOCTL_VPS_GET_GRPX_PARAMS
  - \ref FVID2_SET_FORMAT
  - \ref FVID2_GET_FORMAT
  - \ref FVID2_START
  - \ref FVID2_STOP
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_VID_DEC    External Video Decoder IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver.

  - \ref FVID2_START
  - \ref FVID2_STOP
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_VID_ENC    External Video Encoder IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver.

  - \ref FVID2_START
  - \ref FVID2_STOP
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_TVP5158    TVP5158 IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver.

  - \ref VPS_DRV_FVID2_IOCTL_VID_DEC
*/

/**
  \ingroup VPS_DRV_FVID2_IOCTL
  \defgroup VPS_DRV_FVID2_IOCTL_SII9022A    SII9022A IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver.

  - \ref VPS_DRV_FVID2_IOCTL_VID_ENC
*/
