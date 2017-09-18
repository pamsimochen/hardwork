/** ==================================================================
 *  @file   iss_config.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/common/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file iss_config.h
 *
 *  \brief ISS header file containing globally used configuration.
 *
 */

#ifndef _ISS_CONFIG_H
#define _ISS_CONFIG_H

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/psp/iss/common/trace.h>

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

    /* ========================================================================== 
     */
    /* The define below could be used to enable logging for each modules */
    /* Set the value to */
    /* (GT_DEBUG | GT_TraceState_Enable) - Enables all log messages */
    /* (GT_INFO | GT_TraceState_Enable) - Enables log messages except debug
     * level */
    /* (GT_ERR | GT_TraceState_Enable) - Enables only error log messages */
    /* 0 - Disables all log messages */
    /* ========================================================================== 
     */
#define ISS_CFG_DEFAULT_TRACE           (GT_ERR | GT_TraceState_Enable)

/** \brief Log enable for display list manager. */
#define DlmTrace                        (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for display controller. */
#define DcTrace                         (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for display driver. */
#define IssDdrvDispTrace                (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for graphics display driver. */
#define IssDdrvGrpxTrace                (ISS_CFG_DEFAULT_TRACE)

/** \brief Log enable for M2M list manager. */
#define MlmTrace                        (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for M2M DEI driver. */
#define IssMdrvDeiTrace                 (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for M2M SC driver. */
#define IssMdrvScwbTrace                (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for M2M COMP driver. */
#define IssMdrvCompTrace                (ISS_CFG_DEFAULT_TRACE)

/** \brief Log enable for DEI core. */
#define DeiCoreTrace                    (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for DEI writeback path core. */
#define DwpCoreTrace                    (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for graphics core. */
#define GrpxCoreTrace                   (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for secondary path core. */
#define SecCoreTrace                    (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for bypass path core. */
#define BpCoreTrace                     (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for scalar writeback path core. */
#define SwpCoreTrace                    (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for VIP core. */
#define VipCoreTrace                    (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for CSC+CIG path core. */
#define CscCigCoreTrace                    (ISS_CFG_DEFAULT_TRACE)

/** \brief Log enable for FVID2 driver manager. */
#define FdmTrace                        (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for ISS event manager. */
#define VemTrace                        (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for ISS resource manager. */
#define VrmTrace                        (ISS_CFG_DEFAULT_TRACE)
/** \brief Log enable for ISS Utils. */
#define IssUtilsTrace                   (ISS_CFG_DEFAULT_TRACE)

/** \brief Log enable for all HAL modules. */
#define IssHalTrace                     (GT_INFO | GT_TraceState_Enable)
/** \brief Log enable for VPDMA HAL debug modules. */
#define IssHalVpdmaDebugTrace           (GT_DEBUG | GT_TraceState_Enable)

/** \brief Log enable for mosaic switch utility routines. */
#define ISS_ENABLE_TRACE_HAL_MOSAIC_SWITCH  (ISS_CFG_DEFAULT_TRACE)

/** \brief Log enable for RF Modulator Hal. */
#define IssHalRfTrace                   (ISS_CFG_DEFAULT_TRACE)

/** \brief Log enable for external decoder/encoder drivers. */
#define IssDeviceTrace                  (ISS_CFG_DEFAULT_TRACE)

/** \brief Define this macro to load custome VPDMA firmware. */
    // #define ISS_CFG_ENABLE_CUSTOM_FW_LOAD

/** \brief Define this macro to enable descriptor placement in OCMC
 */
    // #define ISS_CFG_DESC_IN_OCMC

    /* ========================================================================== 
     *
     *  VPDMA priority is 3bit field having values from 0 to 7.
     *  VPDMA priority is passed to DMM/DDR as master priority.
     *  However bit1 in VPDMA priority is not used by DMM/DDR.
     *  Hence effective priorities map as shown below
     *
     *  VPDMA Desc Prioirty Value       Actual DMM/DDR Master priority
     *  0   (000b)                      0   (00b)
     *  1   (001b)                      1   (01b)
     *  2   (010b)                      0   (00b)
     *  3   (011b)                      1   (01b)
     *  4   (100b)                      2   (10b)
     *  5   (101b)                      3   (11b)
     *  6   (110b)                      2   (10b)
     *  7   (111b)                      3   (11b)
     *
     *  Thus for example, VPDMA Desc priority 0 and 2 map to priority 0 at DMM/DDR
     *
     *  Hence in the below priroties for different drivers we will only use the
     *  below priority values
     *
     *  ISSHAL_VPDMA_DATADESCPRIO_0 (effective DDR/DMM priority 0)
     *  ISSHAL_VPDMA_DATADESCPRIO_1 (effective DDR/DMM priority 1)
     *  ISSHAL_VPDMA_DATADESCPRIO_4 (effective DDR/DMM priority 2)
     *  ISSHAL_VPDMA_DATADESCPRIO_7 (effective DDR/DMM priority 3)
     *
     *  Also
     *  - Capture should be the HIGHEST priority ALWAYS to avoid overflow conditions
     *  - Display should be the 2nd-HIGHEST priority ALWAYS to avoid underflow conditions
     *  - M2M driver should be of lower priority
     *  - NSF is kept higher than other M2M drivers for performance reasons
     * ========================================================================== */

/** \brief VPDMA priority for different capture, display, M2M clients */

#define ISS_CFG_CAPT_VPDMA_PRIORITY         (ISSHAL_VPDMA_DATADESCPRIO_0)
#define ISS_CFG_DISP_VPDMA_PRIORITY         (ISSHAL_VPDMA_DATADESCPRIO_1)
#define ISS_CFG_M2M_VPDMA_PRIORITY_RD_NSF   (ISSHAL_VPDMA_DATADESCPRIO_4)
#define ISS_CFG_M2M_VPDMA_PRIORITY_RD       (ISSHAL_VPDMA_DATADESCPRIO_7)
#define ISS_CFG_M2M_VPDMA_PRIORITY_WR_VIP   (ISSHAL_VPDMA_DATADESCPRIO_7)
#define ISS_CFG_M2M_VPDMA_PRIORITY_WR_NSF   (ISS_CFG_M2M_VPDMA_PRIORITY_RD_NSF)
#define ISS_CFG_M2M_VPDMA_PRIORITY_WR       (ISS_CFG_M2M_VPDMA_PRIORITY_RD)
#define ISS_CFG_CAPT_WB_VPDMA_PRIORITY      (ISS_CFG_DISP_VPDMA_PRIORITY)

/**
 *  \brief Maximum number of windows per row supported in multiple window mode.
 *  This is used to allocate memory and array size statically.
 *  Note: This macro could be changed as and when the requirement changes.
 */
#define ISS_CFG_MAX_MULTI_WIN_COL       (8u)

/**
 *  \brief Maximum number of rows per frame supported in multiple window mode.
 *  This is used to allocate memory and array size statically.
 *  Note: This macro could be changed as and when the requirement changes.
 */
#define ISS_CFG_MAX_MULTI_WIN_ROW       (8u)

/**
 *  \brief Maximum number of frames supported in multiple window mode.
 *  This is used to allocate memory and array size statically.
 *  Note: This macro could be changed as and when the requirement changes.
 */
#define ISS_CFG_MAX_MULTI_WIN           (ISS_CFG_MAX_MULTI_WIN_COL *           \
                                         ISS_CFG_MAX_MULTI_WIN_ROW)

/**
 *  \brief Maximum number of in data descriptors.
 *  First row multi window descriptor * 2u for Y/C separate clients +
 *  considering worst case client - 9 IN channels of DEI HQ path without
 *  SC writback path. (Current field covered in multi window descriptor,
 *  3x2 previous fields, 2 MV in, 1 MVSTM in).
 *  Note: This macro can be changed as and when requirement changes.
 */
#define ISS_CFG_MAX_IN_DESC             (ISS_CFG_MAX_MULTI_WIN_COL * 2u + 9u)

/**
 *  \brief Maximum number of out data descriptors.
 *  Considering worst case client - 6 OUT channels of DEI HQ path without
 *  SC writback path. (4 Current field out, 1 MV out, 1 MVSTM out).
 *  Note: This macro can be changed as and when requirement changes.
 */
#define ISS_CFG_MAX_OUT_DESC            (6u)

/**
 *  \brief Maximum number of multi window data descriptor to allocate per
 *  client to program from 2nd row descriptors.
 *  Max multi window descriptor (- first row) * 2u for Y/C separate clients
 *  1 for "Zero transfer descriptor" * 2u for Y/C separate clients
 *  1 for a SOCH on last window channel (either actual or free channel) and
 *  abort descriptor on actual channel * 2u for Y/C separate clients
 *  Abort descriptor on free channel of last window in all rows * 2u for Y/C
 *  separate clients (/2u since abort desc is 1/2 the size of data desc).
 *  Note: This macro can be changed as and when requirement changes.
 */
#define ISS_CFG_MAX_MULTI_WIN_DESC    ((ISS_CFG_MAX_MULTI_WIN_COL *            \
                                       (ISS_CFG_MAX_MULTI_WIN_ROW - 1u) * 2u)  \
                                     + (1u * 2u)                               \
                                     + (1u * 2u)                               \
                                     + ((ISS_CFG_MAX_MULTI_WIN_ROW * 2u) / 2u))

/**
 *  \brief Define this macro to use Sync on Channel instead of Sync on Client
 *  in all M2M drivers while chaining different channels in a single VPDMA
 *  submission. Uncomment this to use Sync on Client.
 */
#define ISS_CFG_USE_SYNC_ON_CH

    /* Enable or disable all mosaic workarounds */
#define ISS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS

#ifdef ISS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
    /* 
     * Configuration option for VPDMA mosaic
     */
/**
 *  \brief Define this to use different free channels for each of the windows
 *  in a mosaic frame. If this is not defined then the free channels will be
 *  reused for each row.
 */
#define ISS_CFG_VPDMA_MOSAIC_USE_DIFF_FREE_CH

/**
 *  \brief Define this to use abort descriptor on each row's last window
 *  free channel and actual channel.
 *  Note: Use dummy desc instead of abort on actual channel in sim as this
 *  is not modelled in HDISSS simulator. */
#ifndef PLATFORM_SIM
#define ISS_CFG_VPDMA_MOSIAC_USE_ABORT
#endif

/**
 *  \brief Define this to set the mosaic mode bit in the in-bound data
 *  descriptors for the non-first row base channel descriptors.
 */
#define ISS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE

/**
 *  \brief Define this to use Sync On Client (SOC) instead of SOCH for
 *  the video path clients.
 *  Note that this if for experiment only and it will not work when
 *  multiple video paths are running is same list or for DEI/SEC paths having
 *  multiple clients (Y and C).
 */
    // #define ISS_CFG_VPDMA_MOSIAC_USE_SOC

/**
 *  \brief Define this to use Sync on Timer control descriptor between fixed
 *  number of data descriptors for all the non-display lists. This way
 *  the display will get time to parse the 2nd and above row mosaic descriptors
 *  without underflowing the VENC.
 */
    // #define ISS_CFG_VPDMA_MOSIAC_USE_SOT

#ifdef ISS_CFG_VPDMA_MOSIAC_USE_SOT
/**
 *  \brief This defines for how many data descriptors the Sync on Timer
 *  descriptor will be inserted so that the mosaic display doesn't under flows.
 */
#define ISS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT       (4u)
/**
 *  \brief Number of cycles to be used with Sync on Timer descriptor inserted
 *  between data descriptors.
 */
#define ISS_CFG_VPDMA_MOSIAC_SOT_DESC_WAIT_CYCLES   (64u)
#endif                                                     /* ISS_CFG_VPDMA_MOSIAC_USE_SOT 
                                                            */

#endif                                                     /* ISS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS 
                                                            */

/** \brief Blender/Comp BackGround Color in RGB Format is 30-bit packed. */
#define ISS_CFG_COMP_BKCOLOR            (0x04010040u)
/** \brief VCOMP BackGround Color in YUV. Format is 0x0CRCBYY (each 10 bits). */
#define ISS_CFG_VCOMP_BKCOLOR           (0x20080000u)
/** \brief VPDMA BackGround Color in ARGB. Format is 0xAARRGGBB. */
#define ISS_CFG_VPDMA_ARGB_BKCOLOR      (0x00000000u)
/** \brief VPDMA BackGround Color in YUV. Format is 0x00YYCBCR. */
#define ISS_CFG_VPDMA_YUV_BKCOLOR       (0x00008080u)

    /* selectively do memset */
#define ISS_CLEAR_ALL_MEM

    /* so far the below modules are defined VIP capture M2M Noise filter M2M
     * deinterlacer M2M scaler M2M comp/blender Display Grpahics */
#define ISS_MODULE_INCLUDE_CAPTURE_VIP
#define ISS_MODULE_INCLUDE_M2M_NSF
#define ISS_MODULE_INCLUDE_M2M_DEI
#define ISS_MODULE_INCLUDE_M2M_SC
#define ISS_MODULE_INCLUDE_M2M_COMP
#define ISS_MODULE_INCLUDE_DISPLAY
#define ISS_MODULE_INCLUDE_GRPX

#ifdef TI_816X_BUILD
#define ISS_HAL_INCLUDE_DEIH
#define ISS_HAL_INCLUDE_DRN
#define ISS_HAL_INCLUDE_COMPR
#define ISS_HAL_INCLUDE_DCOMPR
#endif                                                     /* TI_816X_BUILD */

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

    /* None */

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

    /* None */

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _ISS_CONFIG_H */
