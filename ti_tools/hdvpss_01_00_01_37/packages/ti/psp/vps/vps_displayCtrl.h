/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_DISPLAY_CTRL Display Controller API
 *
 * @{
 */


/**
 *  \file vps_displayCtrl.h
 *
 *  \brief Display Controller API
 *
 *         This file implements display controller APIs, which are
 *         used to control VCOMP, EDE, CIG, CSC, COMP and all vencs.
 *
 *         Display controller handles all the central part of the VPS,
 *         which are used for the display. These central part of VPS
 *         includes all the modules starting from multplexers to vencs.
 *         Display Controller controls configuration for VCOMP, EDE,
 *         CIG, CSC, COMP and all the vencs. It also configures the
 *         multiplexers to enable/disable different path to a perticular venc.
 *
 *         This file has been divided in multiple parts
 *         Part-1 contains Basic configuration: The basic configuration
 *                includes setting up display paths and vencs as per
 *                the pre-defined configurations. Display Controller
 *                supports few pre-defined configurations. Pre-defined
 *                configurations configures all vps modules, including
 *                vencs, handled by display controller.
 *         Part-2 contains Runtime configuration: These configurations can
 *                be applied even while streaming is on on the perticular
 *                path. This configuration includes setting alpha value for
 *                the video display, changing priority of video and graphics
 *                blending etc.
 *         Part-3 contains Advanced configuration: This configuration
 *                is mainly used for enumerating and setting properties
 *                of the individual HDVPSS modules. Even output and mode of
 *                the vencs can be changed using this configurations. This
 *                part also contains create time parameters, which are used
 *                to configure different HDVPSS modules.
 *         Part-4 Control of on-chip encoders such as RF, etc...
 *                Each of the on-chip is tied with one of the vencs, that would
 *                typically drive the encoders.
 *                Such venc/encoder is termed a paired venc/encoder.
 *
 */

#ifndef _VPS_DISPLAYCTRL_H
#define _VPS_DISPLAYCTRL_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* Including VPS.h file here since display controller needs declaration
   for the csc Config. */
#include <ti/psp/vps/vps.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/* ========================================================================== */
/*                      Part-1 Basic Configuration                            */
/* ========================================================================== */
/* ========================================================================== */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*
 *  Macros for display controller instance numbers to be passed as instance ID
 *  at the time of driver create.
 *  Note: These are read only macros. Don't modify the value of these macros.
 */
/** \brief Display controller instance 0. */
#define VPS_DCTRL_INST_0                            (0u)

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_DISPLAY_CTRL
  @{
*/

/** \brief Command to set the entire VPS display path configuration in
 *  one shot.
 *
 *  All the VPS modules handled by the Display Controller can be
 *  represented by a graph, where node represents a module like blender,
 *  mux etc. and edge is present between two nodes if they are connected. All
 *  VPS paths can be configured by this IOCTL in one shot. Use macro
 *  defined in this file for input path, muxes, vcomp, cig input, cig
 *  output and blender as the node numbers.
 *
 *  This IOCTL takes either name of the pre-defined configuration or
 *  list of edges
 *  connecting nodes and configures display paths.
 *  It first validates these paths and then configures VPS for the display
 *  paths. It configures all the center modules, except blender. Blender will
 *  only be enabled when venc is configured with the given mode.
 *
 * \par This IOCTL cannot be used for clearing configuration on a path
 *      streaming path.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_CONFIG              (VPS_DCTRL_IOCTL_BASE + 0x1u)

/** \brief Command to clear the VPS display path configuration in
 *  one shot.
 *
 *  This IOCTL takes either name of the pre-defined configuration or
 *  list of edges
 *  connecting nodes and clears the configuration. It also stops vencs.
 *
 *  It does not validates the edge list. It simply disables the edge
 *  connecting nodes. For the vencs, it checks for the validity and then
 *  disables the venc if there are not errors.
 *
 *  Use macro defined in this file for input path, muxes, vcomp, cig input, cig
 *  output and blender as the node numbers.
 *
 * \par This IOCTL cannot be used for clearing configuration on a path
 *      streaming path.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_CLEAR_CONFIG            (VPS_DCTRL_IOCTL_BASE + 0x2u)

/** \brief Command to set output in the given Venc.
 *
 *  This IOCTL is used to set the output i.e. composite, s-video etc. on
 *  the given venc if venc is not on. For the tied vencs, venc has to
 *  be stopped first, then output can can be changed.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOutputInfo
 *
 * \return  VPS_SOK if successful, else suitable error code
 *
 */
#define IOCTL_VPS_DCTRL_SET_VENC_OUTPUT         (VPS_DCTRL_IOCTL_BASE + 0x3u)

/** \brief Command to get output in the given Venc.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOutputInfo
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_VENC_OUTPUT         (VPS_DCTRL_IOCTL_BASE + 0x4u)

/**
 *  \brief VENC IOCTL for setting controls like brightness
 *
 *  \param   cmdArgs [IN] Pointer to Vps_DcVencControl
 *
 *  \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_VENC_CONTROL                                        \
                                    (VPS_DCTRL_IOCTL_BASE + 0x5u)

/**
 *  \brief VENC IOCTL for getting current values of current control like brightness
 *
 *  \param   cmdArgs [IN] Pointer to Vps_DcVencControl
 *
 *  \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_VENC_CONTROL                                        \
                                    (VPS_DCTRL_IOCTL_BASE + 0x6u)

/** \brief Maximum number of basic IOCTL commands
 *
 *  Marker used to denote the maximum number of BASIC IOCTLs supported
 *
 *  \par CAUTION Ensure that basic IOCTL value does not execeed this value
 */
#define VPS_DCTRL_IOCTL_BASIC_MAX               (VPS_DCTRL_IOCTL_BASE + 0x9u)

/* @} */

/** \brief Maximum number of characters in the string for specifying
 *  output Name */
#define VPS_DC_MAX_OUTPUT_NAME              (20u)

/** \brief Maximum number of edges in the list of edges in Dc_config
 *  structure */
#define VPS_DC_MAX_EDGES                    (46u)

/* Following macros define bitmasks for the Vencs. Here, bitmasks are
   used for identifying Vencs so that tied vencs can be easily specified
   and configured. Two vencs, which uses same pixel clock and whose
   vsync are synchronized, can be tied together. SD Venc cannot
   be tied with any other venc since it supports only SD modes and HD
   vencs supports only HD Modes. Even if it cannot be tied, same
   mechanism of configuring VENC is used for the SD VENC. */

/** \brief Bitmask for HDMI VENC */
#define VPS_DC_VENC_HDMI                    (0x1u)
/** \brief Bitmask for HDCOMP VENC */
#define VPS_DC_VENC_HDCOMP                  (0x2u)
/** \brief Bitmask for DVO2 VENC */
#define VPS_DC_VENC_DVO2                    (0x4u)
/** \brief Bitmask for SD VENC. */
#define VPS_DC_VENC_SD                      (0x8u)

/** \brief Defines maximum number of vencs supported */
#define VPS_DC_MAX_VENC                     (4u)

/** \brief On-Chip encoder identifier - RF */
#define VPS_DC_ENCODER_RF                   (0x1u)

/** \brief On-Chip encoder identifier - Max guard */
#define VPS_DC_MAX_ENCODER                  (0x2u)

/**
 *  enum Vps_DcUseCase
 *  \brief Enum for selecting VPS configuration for the specific use
 *  case or user defined use case.
 *  Display Controller supports few pre-defined configurations. Pre-defined
 *  configurations configures all vps modules, including vencs, handled by
 *  display controller. Once a
 *  pre-defined configuration is used, all other parameters will be ignored
 *  in the Vps_DcConfig structure and display controller will be
 *  configured as per the pre-defined configuration.
 */
typedef enum
{
    VPS_DC_TRIDISPLAY = 0,
    /**< TRIDISPLAY configuration: Pre-defined configuration in which
         HDMI, HDCOMP and SD VENCs are used to provide three outputs. Both
         the HDVENCs are running 1080p mode at 60 fps and SDVENC is
         running NTSC mode. Private
         path0 is connected to HDMI output through VCOMP, Private path1
         is connected to HDCOMP through CIG and secondary path is connected
         to SDVENC. */
    VPS_DC_DUALHDDISPLAY,
    /**< DUALDISPLAY configuration: Pre-defined configuration in which
         HDMI and DVO2 are used to provide two HD outputs. Both
         the HDVENCs are running 1080p mode at 60 fps. Private
         path0 is connected to HDMI output through VCOMP and Private path1
         is connected to HDCOMP throug CIG. */
    VPS_DC_DUALHDSDDISPLAY,
    /**< DUALDISPLAY configuration: Pre-defined configuration in which
         HDMI and SDVENC are used to provide one HD and one SD output. HDVENC
         is running 1080p mode at 60 fps and SD VENC is running PAL mode. Private
         path0 is connected to HDMI output through VCOMP and Private path1
         is connected to SDVENC. */
    VPS_DC_USERSETTINGS,
    /**< User Defined configuration */
    VPS_DC_NUM_USECASE
    /**< This must be the last Enum */
} Vps_DcUseCase;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief DVO Format
 */
typedef enum
{
    VPS_DC_DVOFMT_SINGLECHAN = 0,
    /**< Ouput data format is single channel with embedded sync */
    VPS_DC_DVOFMT_DOUBLECHAN,
    /**< Output data format is dual channel with embedded sync */
    VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC,
    /**< Output data format is tripple channel with embedded sync */
    VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC,
    /**< Ouptut data format is triple channel with discrete sync */
    VPS_DC_DVOFMT_DOUBLECHAN_DISCSYNC,
    /**< Output data format is dual channel with discrete sync */
    VPS_DC_DVOFMT_MAX
    /**< This should be the last Enum */
} Vps_DcDigitalFmt;

/**
 * \brief Analog Format
 */
typedef enum
{
    VPS_DC_A_OUTPUT_COMPOSITE = 0,
    /**< Analog output format composite */
    VPS_DC_A_OUTPUT_SVIDEO,
    /**< Analog output format svideo */
    VPS_DC_A_OUTPUT_COMPONENT,
    /**< Analog output format component */
    VPS_DC_A_OUTPUT_MAX
} Vps_DcAnalogFmt;

/**
 * \brief Signal polarity
 */
typedef enum
{
    VPS_DC_POLARITY_ACT_HIGH = 0,
    /**< Signal polarity Active high */
    VPS_DC_POLARITY_ACT_LOW = 1,
    /**< Signal polarity Active low */
    VPS_DC_POLARITY_MAX = 2
    /**< Signal polarity Active low */
}vps_DcSignalPolarity;

/**
 * \brief Enum for control likes brightness, contrast
 */
typedef enum
{
    VPS_DC_CTRL_BRIGHTNESS = 0,
    /**< Brightness control */
    VPS_DC_CTRL_CONTRAST,
    /**< Contrast control */
    VPS_DC_CTRL_SATURATION,
    /**< Saturation control */
    VPS_DC_CTRL_HUE,
    /**< Hue control */
    VPS_DC_CTRL_MAX
    /**< Control Max */
} Vps_DcCtrl;

/**
 * \brief Structure containing output information. This structure is used
 *  to set output in the output node. Outputs are like Composite,
 *  Component etc.. This structure is used as an argument to
 *  IOCTL_VPS_DCTRL_SET_VENC_OUTPUT ioctl.
 */
typedef struct
{
    UInt32 vencNodeNum;
    /**< Node Number of the Venc */
    UInt32 dvoFmt;
    /**< digital output. See #Vps_DcDigitalFmt for the possible Values */
    UInt32 aFmt;
    /**< Analog output. See #Vps_DcAnalogFmt for the possible Values */
    UInt32 dataFormat;
    /**< Output Data format from Venc. Currently, valid values are
         FVID2_DF_RGB24_888, FVID2_DF_YUV444P, FVID2_DF_YUV422SP_UV */
    UInt32 dvoFidPolarity;
    /**< Polarity for the field id signal for the digital output only
         valid values see #vps_DcSignalPolarity */
    UInt32 dvoVsPolarity;
    /**< Polarity for the vertical sync signal for the digital output only
         valid values see #vps_DcSignalPolarity */
    UInt32 dvoHsPolarity;
    /**< Polarity for the horizontal sync signal for the digital output only
         valid values see #vps_DcSignalPolarity */
    UInt32 dvoActVidPolarity;
    /**< Polarity for the active video signal for the digital output only
         valid values see #vps_DcSignalPolarity */

} Vps_DcOutputInfo;

/**
 *  \brief Brightness control IOCTL command arguments.
 */
typedef struct
{
    UInt32  vencNodeNum;
    /**< Node Number of the Venc. */
    UInt32  control;
    /**< Control to be set For the valid values, se #Vps_DcCtrl */
    Int32   level;
    /**< Brightness level, varies from -128 to +127.
         Contrast and Saturation level varies from 0 to 200 */
} Vps_DcVencControl;

/**
 * \brief Structure containing edge information. Edge is a connection
 *  between two nodes i.e. two modules (like CIG and Blend) in VPS.
 *  VPS can be represented by a graph, where each
 *  module is node and edge is present between two nodes if they are connected.
 *  All VPS paths can be configured in one shot by IOCTL
 *  IOCTL_VPS_DCTRL_SET_CONFIG.
 *  This IOCTL takes the array of edges connected between nodes. This structure
 *  is used to specify individual edge information.
 */
typedef struct
{
    UInt32 startNode;
    /**< Starting node (VPS Module) of the edge */
    UInt32 endNode;
    /**< End node (VPS Module) of the edge */
} Vps_DcEdgeInfo;

/**
 *  struct Vps_DcTimingInfo
 *  \brief Structure for configuring the HDVenc timing parameters.
 */
typedef struct
{
    UInt32 hFrontPorch;
    /**< Horizontal front porch. Same for both fields in case of interlaced
    display */
    UInt32 hBackPorch;
    /**< Horizontal back porch */
    UInt32 hSyncLen;
    /**< Horizontal sync length. Same for both fields in case of interlaced
    display */
    UInt32 vFrontPorch[2];
    /**< Vertical front porch for each field or frame */
    UInt32 vBackPorch[2];
    /**< Vertical back porch for each field or frame */
    UInt32 vSyncLen[2];
    /**< Vertical sync length for each field */
    UInt32 width;
    /**< Active width for each frame. Same for both fields in case of interlaced
    display */
    UInt32 height;
    /**< Active height of each field or frame */
    UInt32 scanFormat;
    /**< Scan format. For valid values see #FVID2_ScanFormat. */
    UInt32 mode;
    /**< mode */

} Vps_DcTimingInfo;


/**
 * \brief Structure containing mode information.
 */
typedef struct
{
    UInt32              vencId;
    /**< Identifies the VENC on which mode parameters is to be applied.
         Use one of VPS_DC_VENC_HDMI, VPS_DC_VENC_HDCOMP,
         VPS_DC_VENC_DVO2, VPS_DC_VENC_SD macro for this variable. */
    FVID2_ModeInfo      mInfo;
    /**< Mode Information to be configured in VENC */
    UInt32              mode;
    /**< VENC mode */
    UInt32              isVencRunning;
    /**< Flag to indicate whether VENC is running or not. This is
         read only parameter returned from the display controller to
         indicated whether given venc is running or not. */
    UInt32              numInPath;
    /**< This is read only parameter returned from the display
         controller to inform number of input paths connected to
         the this vencs. */
} Vps_DcModeInfo;

/**
 * struct Vps_DcVencInfo
 * \brief Structure containing venc information. This structure is used is
 *  IOCTL_VPS_DCTRL_SET_CONFIG API to configure mode in the Vencs.
 *  It also is used to inform
 *  which vencs are tied. Two vencs are tied when both are running on the
 *  same pixel clock and vsync signal for both the vencs are
 *  synchronized. If two vencs are tied, they will be enabled at the same
 *  time in order to have both synchronized. Note that if each of tied venc
 *  is configured with the different modes, display controller does not check
 *  whether venc synchronization is possible or not with the given modes.
 */
typedef struct
{
    Vps_DcModeInfo  modeInfo[VPS_DC_MAX_VENC];
    /**< Mode Information to be set the Venc. */
    UInt32          tiedVencs;
    /**< Bitmask of tied vencs. Two vencs, which uses same pixel clock and whose
         vsync are synchronized, can be tied together. */
    UInt32          numVencs;
    /**< Number of valid entries in modeInfo array */
} Vps_DcVencInfo;

/**
 * struct Vps_DcConfig
 * \brief Structure contaning set of edges and VENC information. Here,
 *  edge represents connection between two VPS modules. When an edge is
 *  enabled, output from a VPS modules is connected as an input to another
 *  VPS module.
 *  This structure is used in
 *  IOCTL_VPS_DCTRL_SET_CONFIG API to configure Complete VPS
 *  connections statically in one shot. It
 *  has set of edges, which describes how individual modules are connected to
 *  each other and finally to the VENC. It also configures the mode in the
 *  VENCs and tells which vencs are tied.
 */
typedef struct
{
    UInt32           useCase;
    /**< Indicates which use case is to be configured for. Display
         Controller provides set of pre-defined configuration
         for some standard use cases. Application can
         directly specify the usecase here. If it is standard use case,
         there is no need
         to specify next arguments. Application can also specify
         user defined path configuration by specifying
         VPS_DC_USERSETTINGS in this argument and providing list of
         edges. */
    Vps_DcEdgeInfo   edgeInfo[VPS_DC_MAX_EDGES];
    /**< List of edges connecting vps modules. Display controller parse these
         edges and enables/disables input/output path in the appropriate VPS
         module. This edge tells which module is connected to which module
         enabling output in edge start module and input in edge end module.*/
    UInt32           numEdges;
    /**< Number edge in the edgeInfo array */
    Vps_DcVencInfo   vencInfo;
    /**< Structure containing Venc Information like mode to be configured and
         which are tied. */
} Vps_DcConfig;

/**
 *  struct Vps_DcYuvColor
 * \brief Structure containing color information in YCrCb. This could
 *  be used for specifying background and
 *  alternate color in VCOMP.
 */
typedef struct
{
    UInt16 yLuma;
    /**< Luma/Y Value */
    UInt16 crChroma;
    /**< Chroma/Cr Value */
    UInt16 cbChroma;
    /**< Chroma/Cb Value */
} Vps_DcYuvColor;

/**
 *  struct Vps_DcRgbColor
 * \brief Structure containing color information in RGB. This could be used for
 *  specifying background color in COMP.
 */
typedef struct
{
    UInt16 r;
    /**< Red Value */
    UInt16 g;
    /**< Green Value */
    UInt16 b;
    /**< Blue Value */
} Vps_DcRgbColor;



/* ========================================================================== */
/* ========================================================================== */
/*                       Part-2 RunTime Configuration                         */
/* ========================================================================== */
/* ========================================================================== */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_DISPLAY_CTRL
  @{
*/

/** \brief Start of Runtime control
 *
 *  Marker used to denote the begining of runtime control IOCTLs
 *
 *  \par CAUTION Ensure that basic IOCTL value does not execeed this value
 */
#define VPS_DCTRL_IOCTL_RT_BASE             (VPS_DCTRL_IOCTL_BASIC_MAX + 0x1u)

/** \brief Command for setting Vcomp runtime configuration.
 *
 *  This IOCTL is used to set the runtime configuration in VCOMP. This
 *  configuration includes setting the priority of the input
 *  video window. This can be even changed when streaming is on on
 *  both the video windows.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcVcompRtConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_VCOMP_RTCONFIG  (VPS_DCTRL_IOCTL_RT_BASE + 0x1u)

/** \brief Command for getting Vcomp runtime configuration.
 *
 *  This IOCTL is used to get the runtime configuration from VCOMP. This
 *  configuration includes the priority of the input
 *  video window.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcVcompRtConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_VCOMP_RTCONFIG  (VPS_DCTRL_IOCTL_RT_BASE + 0x2u)

/** \brief Command for setting Comp runtime configuration.
 *
 *  This IOCTL is used to set the runtime configuration in COMP. This
 *  configuration includes setting the priority of the input video
 *  window and graphics windows. It also specifies whether to use
 *  global or pixel base alpha blending. This can be even changed
 *  when streaming is on. Specify nodeId of the comp in the
 *  Vps_DcCompRtConfig structure to set configuration in specific comp.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcCompRtConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_COMP_RTCONFIG   (VPS_DCTRL_IOCTL_RT_BASE + 0x3u)

/** \brief Command for getting Comp runtime configuration.
 *
 *  This IOCTL is used to get the runtime configuration from COMP.
 *  Specify nodeId of the comp in the Vps_DcCompRtConfig structure to
 *  get configuration in specific comp.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcCompRtConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_COMP_RTCONFIG   (VPS_DCTRL_IOCTL_RT_BASE + 0x4u)

/** \brief Command for setting CIG runtime configuration.
 *
 *  This IOCTL is used to set the runtime configuration in CIG. This
 *  configuration includes alpha value
 *  for the blending and transparency color value for all three output video
 *  windwos. Specify nodeId of the CIG in the
 *  Vps_DcCigRtConfig structure to set configuration in specific CIG output.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcCigRtConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_CIG_RTCONFIG    (VPS_DCTRL_IOCTL_RT_BASE + 0x5u)

/** \brief Command for getting CIG runtime configuration.
 *
 *  This IOCTL is used to get the runtime configuration in CIG. Specify
 *  nodeId of the CIG in the Vps_DcCigRtConfig structure to set
 *  configuration in specific CIG output.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcCigRtConfig
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_CIG_RTCONFIG    (VPS_DCTRL_IOCTL_RT_BASE + 0x6u)

/** \brief Maximum number of runtime IOCTL commands
 *
 *  Marker used to denote the maximum number of runtime IOCTLs supported
 *
 *  \par CAUTION Ensure that runtime IOCTL value does not execeed this value
 */
#define VPS_DCTRL_IOCTL_RT_MAX              (VPS_DCTRL_IOCTL_RT_BASE + 0x9u)

/* @} */

/**
 *  enum Vps_DcCigTransMask
 *  \brief Enumerations for transparency color mask bit (Number of LSB bits to
 *  mask when checking for pixel transparency).
 */
typedef enum
{
    VPS_DC_CIG_TM_NO_MASK = 0,
    /**< Disable Masking */
    VPS_DC_CIG_TM_MASK_1_LSB,
    /**< Mask 1 LSB before checking */
    VPS_DC_CIG_TM_MASK_2_LSB,
    /**< Mask 2 LSB before checking */
    VPS_DC_CIG_TM_MASK_3_LSB
    /**< Mask 3 LSB before checking */
} Vps_DcCigTransMask;

/**
 * \brief enum Vps_DcCompDisplayOrder
 *  Enum for display order selection. PLease note alpha blending is done from
 *  bottom to top layer
 */
typedef enum
{
    VPS_DC_COMP_DISPLAY_VID_ORDER = 0,
    /* Video layer display order.  From low to high: 00, 01, 10, and 11 */
    VPS_DC_COMP_DISPLAY_G0_ORDER,
    /* Graphic0 layer display order when g_reorder = 1.  From low to high: 00,
     01, 10, and 11 */
    VPS_DC_COMP_DISPLAY_G1_ORDER,
    /* Graphic1 layer display order when g_reorder = 1.  From low to high: 00,
     01, 10, and 11 */
    VPS_DC_COMP_DISPLAY_G2_ORDER,
    /* Graphic2 layer display order when g_reorder = 1.  From low to high: 00,
     01, 10, and 11 */
    VPS_DC_COMP_DISPLAY_ORDER_MAX
    /* Defined to find out maximum*/
} Vps_DcCompDisplayOrder;

/**
 * \brief enum Vps_DcCompFeedbkPathSelect
 *  Enum for selection of Feedback data. The feedback video data can
 *  be selected from video alpha blending or final alpha blending. This
 *  enum is used for selecting source of the feedback path.
 */
typedef enum
{
    VPS_DC_COMP_OUT_VIDEO_ALPHA_BLENDING = 0,
    /* select data from video alpha blending */
    VPS_DC_COMP_OUT_FINAL_ALPHA_BLENDING
    /* select data from final alpha blending */
} Vps_DcCompFeedbkPathSelect;



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * struct Vps_DcVcompRtConfig
 * \brief Structure containing runtime configurable parameters for VCOMP
 *  from the display controller. This just includes which of the input video
 *  window is on the top when compositing in the VCOMP. This structure is
 *  passed as an argument to the IOCTL IOCTL_VPS_DCTRL_SET_VCOMP_RTCONFIG.
 */
typedef struct
{
    UInt32  isPrimaryVideoOnTop;
    /**< DEI_HQ input video path is considered as the privide video path.
         This flag indicates whether primary video window is on top or not.
         If it is false, Aux video window will be on the top. */
} Vps_DcVcompRtConfig;

/**
 * struct Vps_DcCigRtConfig
 * \brief Structure containing runtime configurable parameters for CIG
 *  from the display controller. This includes Blending and transparency value
 *  for the output video windows. This structure is passed as an
 *  argument to IOCTL IOCTL_VPS_DCTRL_SET_CIG_RTCONFIG.
 */
typedef struct
{
    UInt32                 nodeId;
    /**< Id of the node. Use one of VPS_DC_CIG_CONSTRAINED_OUTPUT,
         VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_CIG_PIP_OUTPUT as
         node id for configuring CIG runtime configuration. */
    UInt32                 transparency;
    /**< Enable Transparency */
    UInt32                 mask;
    /**< Transparency color mask bit For valid values see #Vps_DcCigTransMask.*/
    UInt32                 alphaBlending;
    /**< Enable alpha blending */
    UInt8                  alphaValue;
    /**< Alpha blending value */
    Vps_DcRgbColor         transColor;
    /**< Transparency color in RGB */
} Vps_DcCigRtConfig;

/**
 * struct Vps_DcCompRtConfig
 * \brief Structure containing runtime configurable parameters for COMP
 *  from the display controller. These parameters includes priority of the
 *  input layers and whether to use global reordering or not. For each of
 *  the comp this runtime configurable parameters can be set separately. For
 *  setting runtime parameters for all blenders, this ioctl should be called
 *  multiple times. This structure is passed as an
 *  argument to IOCTL IOCTL_VPS_DCTRL_SET_COMP_RTCONFIG
 */
typedef struct
{
    UInt32                      nodeId;
    /**< Id of the node. Use one of VPS_DC_HDMI_BLEND,
         VPS_DC_HDCOMP_BLEND, VPS_DC_DVO2_BLEND,
         VPS_DC_SDVENC_BLEND as node id for runtime configuration of COMP */
    UInt32                      isPipBaseVideo;
    /**< CIG PIP output goes to all threee HD Blenders. This flag indicates
         whether this pip video is base video or not. */
    UInt32                      fbPath;
    /**< Selects Feedback data path. These selects the source of the feedback
         path. Feedback path from the blender is
         supported only for the DVO1 and DVO2 output in the hardware. There
         is not feedback path from the other blenders, so these parameter
         will not be used for these blender
         For valid values see #Vps_DcCompFeedbkPathSelect.*/
    UInt32                      isGlobalReorderEnable;
    /**< Enables/Disables global reordering. If global reordering is enabled,
         Display order/priority for the input paths are specified in the
         displayOrder member of this structure. Otherwise, only display
         order of the video window is used from the displayOrder array.
         Graphics will bring priority per pixel. */
    UInt32                     displayOrder[VPS_DC_COMP_DISPLAY_ORDER_MAX];
    /**< decides order of priority between three graphic and video
         if isGlobalReorderEnable is set TRUE. It is array of size 4 and
         user has to configure priority.  In the case,if
         isGlobalReorderEnable is
         set 0, user has to provide priority for vidoe layer only and for
         graphic layer, it will be taken from settings or bits [35:32]
         in the data bus. For valid values see #Vps_DcCompDisplayOrder.*/
} Vps_DcCompRtConfig;


/* ========================================================================== */
/* ========================================================================== */
/*                      Part-3 Advanced Configuration                         */
/* ========================================================================== */
/* ========================================================================== */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_DISPLAY_CTRL
  @{
*/

/** \brief Advanced configurations
 *
 *  Marker used to denote the begining of IOCTLs that would be required for
 *  for advanced control
 *
 *  \par CAUTION Ensure that basic IOCTL value does not execeed this value
 */
#define VPS_DCTRL_IOCTL_ADV_BASE    (VPS_DCTRL_IOCTL_RT_MAX + 0x1u)

/** \brief Command to enumerate VPS nodes and its information for the
 *  node available at the given index.
 *
 *  This IOCTL is used for enumerating and then dynamically connecting different
 *  VPS modules. Enumeration starts from the node number 0 and
 *  continues untill it returns negative error code. On each index, this IOCTL
 *  returns node and its information like type of node number, name, number of
 *  inputs and number output available at given index. Only nodes, in
 *  which inputs can be enabled/disabled, can be enumerated with this
 *  ioctl. It returns node number from macros defined for input path,
 *  muxes, vcomp, cig input, cig output and blender.
 *
 * \param   cmdArgs [IN/OUT] Pointer of type Vps_DcEnumNode, which would be
 *                           updated with input node information. Provided that
 *                           node index and input are valid.
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_ENUM_NODES          (VPS_DCTRL_IOCTL_ADV_BASE + 0x1u)

/** \brief Command to enumerate nodes connected as a inputs to the given node.
 *  It enumerates inputs nodes of this node.
 *
 *  To enumerate all inputs, applications shall begin with input
 *  index zero, get the information,
 *  increment by one until the driver returns -1.
 *
 * \param   cmdArgs [IN/OUT] Pointer of type Vps_DcEnumNodeInput, which would be
 *                           updated with input node information. Provided that
 *                           node index and input are valid.
 *
 * \return  VPS_SOK if successful, else suitable error code
 *
 */
#define IOCTL_VPS_DCTRL_ENUM_NODE_INPUTS    (VPS_DCTRL_IOCTL_ADV_BASE + 0x2u)

/** \brief Command to enable/disable the given input on the given node
 *
 *  This IOCTL enables or disables input at the given index
 *  on the given node. It enables/disables given input as inputs
 *  node to this node and enables/disables given node
 *  as output node to the parent node i.e. it enables/disables edge connecting
 *  given node and input node. For example,
 *  to enable/disable CIG PIP output on HDCOMP Blender, use nodeid as
 *  VPS_DC_HDCOMP_BLEND and VPS_DC_CIG_PIP_OUTPUT as inputnodeid. This
 *  will enable/disable CIG PIP output in the CIG and CIG PIP input in the
 *  HDCOMP Blender.
 *
 * \par CAUTION This IOCTL can not be used to enable node input
 *       if streaming on that input.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcNodeInput
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_NODE_INPUT          (VPS_DCTRL_IOCTL_ADV_BASE + 0x3u)

/** \brief Command to get the status of the given input on the given node.
 *
 *  This command is used to get the status i.e. whether it is
 *  enabled or not, of the input on the given node.
 *
 *  For example, to get the status of CIG PIP on HDCOMP Blender, use nodeid as
 *  VPS_DC_HDCOMP_BLEND and VPS_DC_CIG_PIP_OUTPUT as inputnodeid. It
 *  will return the status of this input on the blender node.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcNodeInput
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_NODE_INPUT_STATUS  (VPS_DCTRL_IOCTL_ADV_BASE + 0x4u)

/** \brief Command to set mode/standard in the given Venc.
 *
 *  This IOCTL is used to set the mode/standard in the given vencs
 *  if its inputs are not running. If the multiple venc is tied, then
 *  it sets mode in all the vencs if their inputs are not running.
 *  Otherwise it returns error. The IOCTL first stops venc, changes
 *  mode and restarts venc. Note that this IOCTL can break tying of
 *  vencs if used incorrectly.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcVencInfo
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_VENC_MODE   (VPS_DCTRL_IOCTL_ADV_BASE + 0x5u)

/** \brief Command to get current mode/standard set in the given Venc.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcVencInfo
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_VENC_MODE   (VPS_DCTRL_IOCTL_ADV_BASE + 0x6u)

/** \brief Command to stop streaming on the VENC.
 *
 *  It stops streaming on the given venc or set of vencs.
 *  Application should pass the bit mask of all the vencs, which
 *  needs to be stopped.
 *
 * \param   cmdArgs [IN] Pointer of type UInt32, that contains bitmask of all
 *          the vencs to be disabled.
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_DISABLE_VENC    (VPS_DCTRL_IOCTL_ADV_BASE + 0x7u)

/** \brief Command for Setting Venc Clock Source
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcVencClkSrc
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC (VPS_DCTRL_IOCTL_ADV_BASE + 0x8u)

/** \brief Command for Getting Venc Clock Source
 *
 * \param   cmdArgs [IN/OUT] Pointer of type Vps_DcVencClkSrc
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_GET_VENC_CLK_SRC (VPS_DCTRL_IOCTL_ADV_BASE + 0x9u)


/** \brief Maximum number of advanced IOCTL commands
 *
 *  Marker used to denote the maximum number of advance IOCTLs supported
 *
 *  \par CAUTION Ensure that advanced IOCTL value does not execeed this value
 */
#define VPS_DCTRL_IOCTL_ADV_MAX         (VPS_DCTRL_IOCTL_ADV_BASE + 0x20u)

/* @} */

/* Macros, Which can be used in the setconfig API to connect different
 * modules */

/** \brief Macro defining Main input path */
#define VPS_DC_MAIN_INPUT_PATH              (0u)
/** \brief Macro defining Auxiliary input path */
#define VPS_DC_AUX_INPUT_PATH               (4u)
/** \brief Macro defining Bypass 0 input path */
#define VPS_DC_BP0_INPUT_PATH               (5u)
/** \brief Macro defining Bypass 1 input path */
#define VPS_DC_BP1_INPUT_PATH               (6u)
/** \brief Macro defining secondary 1 input path */
#define VPS_DC_SEC1_INPUT_PATH              (10u)
/** \brief Macro defining Graphics 0 input path */
#define VPS_DC_GRPX0_INPUT_PATH             (19u)
/** \brief Macro defining Graphics 1 input path */
#define VPS_DC_GRPX1_INPUT_PATH             (20u)
/** \brief Macro defining Graphics 2 input path */
#define VPS_DC_GRPX2_INPUT_PATH             (21u)

/** \brief Macro defining VCOMP Multiplexer */
#define VPS_DC_VCOMP_MUX                    (1u)
/** \brief Macro defining HDCOMP Multiplexer */
#define VPS_DC_HDCOMP_MUX                   (2u)
/** \brief Macro defining SDVENC Multiplexer */
#define VPS_DC_SDVENC_MUX                   (3u)


/* Macros for the other VPS modules */

/** \brief Macro defining VCOMP */
#define VPS_DC_VCOMP                        (14u)

/** \brief Macro defining CIG PIP input. In CIG this is the only path
  * which can be disabled. Main input path cannot be disabled. */
#define VPS_DC_CIG_PIP_INPUT                (16u)

/** \brief Macros defining CIG Constrained output. CIG is used for
  * constraning input video. There are three video outputs
  *  from CIG, i.e. non-constrained video output (HDMI), which
  *  provides original video stream, constrained
  *  video output (HDCOMP), which provides video with the reduced quality
  *  and PIP video output, which can be used for PIP. */
#define VPS_DC_CIG_CONSTRAINED_OUTPUT        (15u)

/** \brief Macros defining CIG Non-Contrained Output. */
#define VPS_DC_CIG_NON_CONSTRAINED_OUTPUT    (17u)

/** \brief Macro defining CIG PIP output. This output can also be used
  * as pip in the display. Input to the pip is aux path, which can be
  * smaller than the actual display size. In this case, CIG fills rest of the
  * display area with the zero. */
#define VPS_DC_CIG_PIP_OUTPUT               (18u)


/* Macros for the VPS Blenders */
/** \brief Macro for the HDMI Blender */
#define VPS_DC_HDMI_BLEND                   (22u)

/** \brief Macro for the HDCOMP Blender */
#define VPS_DC_HDCOMP_BLEND                 (23u)

/** \brief Macro for the DVO2 Blender */
#define VPS_DC_DVO2_BLEND                   (24u)

/** \brief Macro for the SDVENC Blender */
#define VPS_DC_SDVENC_BLEND                 (25u)

/** \brief Maximum number of characters in the string for specifying
 *  node name */
#define VPS_DC_MAX_NODE_NAME                (20u)

/**
 *  enum Vps_DcNodeType
 *  \brief Enum defining node types available in VPS. There are several
 *  modules available in VPS, which controls how and which display goes
 *  to perticulcar VENC. These path controller modules are Multiplexers,
 *  VCOMP, CIG and Blenders. There modules are known as nodes in the display
 *  controller. This enum defines the type of the node.
 */
typedef enum
{
    VPS_DC_NODETYPE_MUX = 0,
    /**< Multiplexer or Switch, takes N selectable inputs and
         provides one output */
    VPS_DC_NODETYPE_SPLITTER,
    /**< Splitter, takes one input and provides M identical outputs */
    VPS_DC_NODETYPE_COMP,
    /**< Compositor, takes N inputs and provides one composited output */
    VPS_DC_NODETYPE_INPUT,
    /**< Input Node, there is no node connected as input to this node */
    VPS_DC_NODETYPE_OUTPUT,
    /**< Output Node,  there is no node connected as output from this node  */
    VPS_DC_NODETYPE_MAX
    /**< This must be last enum */
} Vps_DcNodeType;

/**
 *  enum Vps_DcVencClkSrcSel
 *  \brief Enum defining clock source selection options for the VENCs.
 */
typedef enum
{
    VPS_DC_CLKSRC_VENCD = 0,
    /**< Clk1X input and output clock from VENC are same as VENCD Clock */
    VPS_DC_CLKSRC_VENCD_DIV2,
    /**< Clk1X input clock and venc output clock are sourced from VEND/2 clock */
    VPS_DC_CLKSRC_VENCD_DIV2_DIFF,
    /**< Clk1X input is sourced from VENCD/2 clock and VENC output clock
         is from VENCD DIV2 Clock */
    VPS_DC_CLKSRC_VENCA,
    /**< Clk1X input and output clock from VENC are same as VENCA Clock */
    VPS_DC_CLKSRC_VENCA_DIV2,
    /**< Clk1X input clock and venc output clock are sourced from VENA/2 clock */
    VPS_DC_CLKSRC_VENCA_DIV2_DIFF,
    /**< Clk1X input is sourced from VENCA/2 clock and VENC output clock
         is from VENCA DIV2 Clock */
    VPS_DC_CLKSRC_MAX
} Vps_DcVencClkSrcSel;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_DcEnumNode
 * \brief Structure containing the properties of a the processing node. Node
 *  represents a VPS module which controls how and which display goes
 *  to perticulcar VENC. This structure is for enumerating this processing
 *  modules one by one and get the properties of it.
 */
typedef struct Vps_DcEnumNode
{
    UInt32          nodeIdx;
    /**< Index of the node. Node Index starting from 0 to maximum
         number of nodes. Used at the time of enumerating nodes.
         After max number of nodes, this function returns error. */
    UInt32          nodeId;
    /**< ID of the node. This ID should be used when enabling input
         of this node */
    UInt32          nodeType;
    /**< Type of the node, This type indicates whether this node can
         accept multiple input or output or not
         For valid values see #Vps_DcNodeType*/
    Char            nodeName[VPS_DC_MAX_NODE_NAME];
    /**< Name of the processing node */
    UInt32          numInputs;
    /**< Number of all possible inputs for this node */
    UInt32          numOutputs;
    /**< Number of all possible outputs for this node */
} Vps_DcEnumNode;


/**
 *  struct Vps_DcEnumNodeInput3
 * \brief Structure containing the properties of a input of a
 *  the processing node. This structure is for enumerating input of
 *  modules one by one and get the properties of it.
 */
typedef struct
{
    UInt32          nodeId;
    /**< Id of the node. This is the ID of the node for which input
         is to be enumerated. */
    UInt32          inputIdx;
    /**< Index of the input. Input Index starting from 0 to maximum
         number of inputs of this node. Used at the time of enumerating
         inputs of the given node. After max number of inputs,
         this function returns errro. */
    UInt32          inputId;
    /**< Input ID of the given node */
    Char            inputName[VPS_DC_MAX_NODE_NAME];
    /**< Name of the input */
} Vps_DcEnumNodeInput;

/**
 *  struct Vps_DcNodeInput
 * \brief This structure will be used at the time of enabling/disabling
 *  input of the given node. It takes id of the node and id of the
 *  input for this node.
 */
typedef struct
{
    UInt32          nodeId;
    /**< Id of the node. Use macros defined above for this id. */
    UInt32          inputId;
    /**< Input id of the node to be enabled. Use macros defined
         above for this id. */
    UInt32          isEnable;
    /**< Flag to indicate whether given input on the given node
         is enabled or not. 0: Disabled, 1: Enabled */
} Vps_DcNodeInput;

/**
 *  struct Vps_DcVcompConfig
 * \brief structure containing vcomp static configuration.
 */
typedef struct
{
    Vps_DcYuvColor        bckGrndColor;
    /**< background colcor to be displayed in YCrCb */
    Vps_DcYuvColor        mainAltColor;
    /**< alternate colcor to be displayed in YCrCb. When the main source is
         enabled, send the alternate main Y/Cb/Cr values instead of the
         true source picture.  This bit allows the datapath to flush through
         without outputing the actual picture. */
    Vps_DcYuvColor        auxAltColor;
    /**< alternate colcor to be displayed in YCrCb. When the aux source is
         enabled, send the alternate main Y/Cb/Cr values instead of the
         true source picture.  This bit allows the datapath to flush through
         without outputing the actual picture. */
    /* Precedence of the Video */
} Vps_DcVcompConfig;

/**
 * struct Vps_DcEdeConfig
 * structure containing EDE static configuration
 */
typedef struct
{
    UInt32  ltiEnable;
    /**< Enables/Disables Luminance Transition Improvement Block */
    UInt32  horzPeaking;
    /**< Enables/Disables Horizontal Peaking */
    UInt32  ctiEnable;
    /**< Enables/Disables Chrominance Transition Improvement Block */
    UInt32  transAdjustEnable;
    /**< Enables/Disables Transition Adjustment for Chroma Block */
    UInt32  lumaPeaking;
    /**< Enables/Disables Luminance Peaking */
    UInt32  chromaPeaking;
    /**< Enables/Disables Chrominance Peaking */
    UInt16  minClipLuma;
    /**< Minimum value of the C clipping in the clipping block */
    UInt16  maxClipLuma;
    /**< Maximum value of the Y clipping in the clipping block */
    UInt16  minClipChroma;
    /**< Minimum value of the C clipping in the clipping block */
    UInt16  maxClipChroma;
    /**< Maximum value of the C clipping in the clipping block */
    UInt32  bypass;
    /**< Bypass complete EDE processing */
} Vps_DcEdeConfig;

/**
 * struct Vps_DcCigMainConfig
 * \brief Structure containing static configuration for CIG Main Path
 */
typedef struct
{
    UInt32 enableContraining;
    /**< This enables contraining in the CIG module. Once enabled, it provides
         controained video on the CIT video output. */
    UInt32 nonCtrInterlace;
    /**< Enables/Disables interlacing on the non-contraining video output
         of the CIG. Enabling this will require input video of type
         progressive and CIG will interlace it and provide interlaced video
         to the blender */
    UInt32 ctrInterlace;
    /**< Enables/Disables interlacing on the contraining video output
         of the CIG. Enabling this will require input video of type
         progressive and CIG will interlace it and provide interlaced video
         to the blender */
} Vps_DcCigMainConfig;

/**
 * struct Vps_DcCigPipConfig
 * \brief Structure containing static configuration for CIG PIP Path
 */
typedef struct
{
    UInt32 pipInterlace;
    /**< Enables/Disables interlacing on the pip video output
         of the CIG. Enabling this will require input video of type
         progressive and CIG will interlace it and provide interlaced video
         to the blender */
} Vps_DcCigPipConfig;

/**
 * struct Vps_DcCompConfig
 * \brief Structure containing COMP static configuration
 */
typedef struct
{
    Vps_DcRgbColor     bckGrndColor;
    /**< Background Color in RGB format.his backround color is common for
         all the compositors/blenders(like HDMI, HDCOMP, DVO2, SD). This color
         will replace any pixel with RGB value of 000.*/
} Vps_DcCompConfig;

typedef struct
{
    UInt32              venc;
    /**< Venc Id. VPS_DC_VENC_HDMI, VPS_DC_VENC_DVO2 or
         VPS_DC_VENC_HDCOMP */
    UInt32              clkSrc;
    /**< Clock source for the given venc. HDMI can be sourced only from
         VEND clock wheread other two vencs, HDCOMP and DVO2, can be
         sourced either from VENCD clock or VENCA clock.
         See #Vps_DcVencClkSrcSel for possible values */
} Vps_DcVencClkSrc;

/**
 * struct Vps_DcCreateConfig
 * \brief Structure for static configuration. This structure is used
 *  for statically configuring display controller modules. It will be
 *  passed at the time of opening the display controller. It
 *  contains pointers to configuration structures of display
 *  controller modules.
 */
typedef struct
{
    Vps_DcVcompConfig      *vcompConfig;
    /**< Vcomp Configuration */
    Vps_DcEdeConfig        *edeConfig;
    /**< Ede Configuration */
    Vps_DcCigMainConfig    *cigMainConfig;
    /**< Cig Configuration for the Main Path */
    Vps_DcCigPipConfig     *cigPipConfig;
    /**< Cig Configuration for the PIP Path */
    Vps_CprocConfig        *cprocConfig;
    /**< Cproc Configuration */
    Vps_DcCompConfig       *compConfig;
    /**< Comp Configuration */
    Vps_CscConfig          *sdCscConfig;
    /**< CSC Configuraton for the CSC on SD path */
    Vps_CscConfig          *hdcompCscConfig;
    /**< CSC Configuraton for the CSC on HDComp */
    Vps_CscConfig          *vcompCscConfig;
    /**< CSC Configuraton for the CSC on VComp */
} Vps_DcCreateConfig;


/* ========================================================================== */
/* ========================================================================== */
/*                      Part-4 On-Chip encoder Configuration                  */
/* ========================================================================== */
/* ========================================================================== */

/* ========================================================================== */
/*                           Control Command                                  */
/* ========================================================================== */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_DISPLAY_CTRL
  @{
*/

/** \brief On-Chip encoder control base
 *
 *  Marker used to denote the begining of IOCTLs that would be required to
 *  configure/control on-chip encoders
 *
 *  \par CAUTION Ensure that basic IOCTL value does not exceed this value
 */
#define VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE (VPS_DCTRL_IOCTL_ADV_MAX + 0x1u)

/**
 * \brief This control command retrieves the current (basic) RF configuration.
 *
 *         When any of the configurable parameters of the RF Modulator is
 *         required to be modified, it is expected to retrieve the current
 *         configuration, modify the required parameter(s) and apply the
 *         configuration via IOCTL_VPS_DCTRL_RF_SET_CONFIG.
 *
 * \par CAUTION By default, the configurable parameters may be turned OFF.
 *      Caller is expected to turn ON the required parameter(s) and
 *      update the config with IOCTL_VPS_DCTRL_RF_SET_CONFIG command.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOnchipEncoderCmd, which
 *                       initializes following members
 *                       .vencId    = Paired VENC ,VPS_DC_VENC_SD in this case.
 *                       .encoderId = VPS_DC_ENCODER_RF
 *                       .cmd       = This macro
 *                       .argument  = Pointer to structure of type
 *                                     #Vps_RfConfig
 *                       .additionalArgs = NULL
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_RF_GET_CONFIG                                        \
                                    (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x1u)

/**
 * \brief This control command applies the supplied (basic) RF configuration.
 *
 *        Provided the values supplied are valid and encoder is not enabled.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOnchipEncoderCmd, which
 *                       initializes following members
 *                       .vencId    = Paired VENC ,VPS_DC_VENC_SD in this case.
 *                       .encoderId = VPS_DC_ENCODER_RF
 *                       .cmd       = This macro
 *                       .argument  = Pointer to structure of type
 *                                     #Vps_RfConfig
 *                       .additionalArgs = NULL
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_RF_SET_CONFIG                                        \
                                    (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x2u)

/**
 * \brief This control command retrieves the current expert RF configuration.
 *
 *         When any of the configurable parameters of the RF Modulator is
 *         required to be modified, it is expected to retrieve the current
 *         configuration, modify the required parameter(s) and apply the
 *         configuration via IOCTL_VPS_DCTRL_RF_SET_EXPERT_CONFIG.
 *
 * \par CAUTION By default, the configurable parameters may be turned OFF.
 *      Caller is expected to turn ON the required parameter(s) and
 *      update the config with IOCTL_VPS_DCTRL_RF_SET_EXPERT_CONFIG command.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOnchipEncoderCmd, which
 *                       initializes following members
 *                       .vencId    = Paired VENC ,VPS_DC_VENC_SD in this case.
 *                       .encoderId = VPS_DC_ENCODER_RF
 *                       .cmd       = This macro
 *                       .argument  = Pointer to structure of type
 *                                    VpsHal_RfExpertConfig
 *                       .additionalArgs = NULL
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_RF_GET_EXPERT_CONFIG                                 \
                                    (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x3u)

/**
 * \brief This control command applies the supplied expert RF configuration.
 *
 *        Provided the values supplied are valid and encoder is not enabled.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOnchipEncoderCmd, which
 *                       initializes following members
 *                       .vencId    = Paired VENC ,VPS_DC_VENC_SD in this case.
 *                       .encoderId = VPS_DC_ENCODER_RF
 *                       .cmd       = This macro
 *                       .argument  = Pointer to structure of type
 *                                    VpsHal_RfExpertConfig
 *                       .additionalArgs = NULL
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_RF_SET_EXPERT_CONFIG                                 \
                                    (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x4u)

/**
 *
 * \brief Switch ON the RF Modulator DAC.
 *
 *  This control command could be used to switch ON the RF Modulator DAC.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOnchipEncoderCmd, which
 *                       initializes following members
 *                       .vencId    = Paired VENC ,VPS_DC_VENC_SD in this case.
 *                       .encoderId = VPS_DC_ENCODER_RF
 *                       .cmd       = This macro
 *                       .argument  = NULL
 *                       .additionalArgs = NULL
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_RF_START                                             \
                                    (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x5u)

/**
 *
 * \brief Switch OFF the RF Modulator DAC.
 *
 *  This control command could be used to switch OFF the RF Modulator DAC.
 *
 * \param   cmdArgs [IN] Pointer of type Vps_DcOnchipEncoderCmd, which
 *                       initializes following members
 *                       .vencId    = Paired VENC ,VPS_DC_VENC_SD in this case.
 *                       .encoderId = VPS_DC_ENCODER_RF
 *                       .cmd       = This macro
 *                       .argument  = NULL
 *                       .additionalArgs = NULL
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
#define IOCTL_VPS_DCTRL_RF_STOP                                              \
                                    (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x6u)

/** \brief Maximum number of on-chip encoders control/configure IOCTL commands
 *
 *  Marker used to denote the maximum number of on-chip IOCTLs supported
 *
 *  \par CAUTION Ensure that on-chip encoders IOCTL value does not execeed this
 *       value
 */
#define VPS_DCTRL_IOCTL_ONCHIP_ENC_MAX (VPS_DCTRL_IOCTL_ONCHIP_ENC_BASE + 0x20u)

/* @} */


/* ========================================================================== */
/*                          Callback Funtion type                             */
/* ========================================================================== */

/**
 *
 * \brief Callback type, that would be called when a sink is attached/dected and
 *        removed.
 *
 * \param   vencId      [IN] Identifier that identifies the paired VENC.
 * \param   encoderId   [IN] Identifier allocated for this instance of encoder.
 * \param   sinkState   [IN] TRUE/Positive Value indicates presence of a sink,
 *                           FALSE/0x0 indicates sink is not dected.
 * \param   appData     [IN] Application supplied data, while attaching the
 *                           callback.
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
typedef Int32 (*Vps_DcOnChipEnc_CbFxn) (UInt32 vencId,
                                        UInt32 encoderId,
                                        UInt32 sinkState,
                                        Ptr    appData);

/* ========================================================================== */
/*             Structure Required for On-Chip Encoder control                 */
/* ========================================================================== */

/**
 * \brief
 *  On-Chip encoders control parameters. Place holder for arguments thats would
 *  be required by the on-chip encoder/its paired venc.
 *
 * \par CAUTION - Refer the encoder specific HALs header files for the types
 *                of arguments required by the encoders.
 */
typedef struct
{
    UInt32      vencId;
    /**< Venc identifier - Venc that would drive encoder of intreset */
    UInt32      encoderId;
    /**< Encoder identifier - Selects the encoder that would require perform the
         requested control */
    UInt32      cmd;
    /**< Command for the encoder */
    Ptr         argument;
    /**< Command arguments that would be required. */
    Ptr         additionalArgs;
    /**< Additional arguments if any. */
    Vps_DcOnChipEnc_CbFxn cbFxn;
    /**< Application provided callback function - used only when the encoder
         is HDMI. */
} Vps_DcOnchipEncoderCmd;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPS_DISPLAYCTRL_H */

/* @} */
