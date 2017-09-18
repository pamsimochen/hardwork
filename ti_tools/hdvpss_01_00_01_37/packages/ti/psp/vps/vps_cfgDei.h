/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_VPS_COMMON
 * \addtogroup VPS_DRV_FVID2_VPS_COMMON_DEI HD-VPSS - De-interlacer Config API
 *
 * @{
 */

/**
 *  \file vps_cfgDei.h
 *
 *  \brief HD-VPSS - De-interlacer Configurations options interface
 */

#ifndef _VPS_CFG_DEI_H
#define _VPS_CFG_DEI_H

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

/** \brief Maximum number of field buffers requried by DEI. */
#define VPS_DEI_MAX_CTX_FLD_BUF         (4u)
/** \brief Maximum number of MV buffers requried by DEI. */
#define VPS_DEI_MAX_CTX_MV_BUF          (3u)
/** \brief Maximum number of MVSTM buffers requried by DEI. */
#define VPS_DEI_MAX_CTX_MVSTM_BUF       (3u)

/**
 *  enum Vps_DeiFmdJamDir
 *  \brief Enumerations for field jamming direction, used in
 *         Film Mode Detection (FMD)
 */
typedef enum
{
    VPS_DEI_FMDJAMDIR_PREV_FLD = 0,
    /**< Curr fld jammed with previous fld. */
    VPS_DEI_FMDJAMDIR_NEXT_FLD
    /**< Curr fld jammed with next fld. */
} Vps_DeiFmdJamDir;

/**
 *  enum Vps_DeiHqEdiMode
 *  \brief Enumerations for DEIHQ (High quality De-Interlacer Engine)
 *  Edge Directed Interpolation (EDI) mode.
 */
typedef enum
{
    VPS_DEIHQ_EDIMODE_LINE_AVG = 0,
    /**< Line average. */
    VPS_DEIHQ_EDIMODE_FLD_AVG,
    /**< Field average. */
    VPS_DEIHQ_EDIMODE_EDI_SMALL_WINDOW,
    /**< EDI using small [-1, 1] window. */
    VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW,
    /**< EDI using large [-7, 7] window. */
    VPS_DEIHQ_EDIMODE_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_DeiHqEdiMode;

/**
 *  enum Vps_DeiHqFldMode
 *  \brief Enumerations for Motion Detection (MDT) mode.
 */
typedef enum
{
    VPS_DEIHQ_FLDMODE_4FLD = 1,
    /**< 4-field operation. */
    VPS_DEIHQ_FLDMODE_5FLD = 2,
    /**< 5-field operation. */
    VPS_DEIHQ_FLDMODE_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_DeiHqFldMode;

/**
 *  enum Vps_DeiHqCtxMode
 *  \brief Enumerations for the DEI HQ context buffer modes.
 */
typedef enum
{
    VPS_DEIHQ_CTXMODE_DRIVER_ALL = 0,
    /**< All context buffers are maintained by driver. */
    VPS_DEIHQ_CTXMODE_APP_N_1,
    /**< All context buffers, except N-1 field, are maintained by driver.
         N-1 field buffer is provided by application which is from DEI
         output. */
    VPS_DEIHQ_CTXMODE_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_DeiHqCtxMode;

/**
 *  enum Vps_DeiEdiMode
 *  \brief Enumerations for DEI Edge Directed Interpolation (EDI) mode.
 */
typedef enum
{
    VPS_DEI_EDIMODE_LINE_AVG = 0,
    /**< Line average. */
    VPS_DEI_EDIMODE_FLD_AVG,
    /**< Field average. */
    VPS_DEI_EDIMODE_LUMA_ONLY,
    /**< EDI for Luma only. */
    VPS_DEI_EDIMODE_LUMA_CHROMA,
    /**< EDI for Luma and Chroma. */
    VPS_DEI_EDIMODE_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_DeiEdiMode;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_DeiFmdConfig
 *  \brief DEI FMD configuration.
 */
typedef struct
{
    UInt32                  filmMode;
    /**< Enable Film mode. */
    UInt32                  bed;
    /**< Enable Bad edit detection. */
    UInt32                  window;
    /**< Enable FMD operation window. */
    UInt32                  lock;
    /**< Lock Deinterlacer to film mode. */
    UInt32                  jamDir;
    /**< Field jamming direction. For valid values see #Vps_DeiFmdJamDir. */
    UInt16                  windowMinx;
    /**< Left boundary of FMD window */
    UInt16                  windowMiny;
    /**< Top boundary of FMD window */
    UInt16                  windowMaxx;
    /**< Right boundary of FMD window */
    UInt16                  windowMaxy;
    /**< Bottom boundary of FMD window */
    UInt32                  cafThr;
    /**< CAF threshold used for leaving film mode:  If the
         combing artifacts is greater than this threshold, CAF
         is detected and thus the state machine will be forced
         to leave the film mode.  If the user prefers to be more
         conservative in using film mode, decrease this threshold. */
    UInt32                  cafLineThr;
    /**< CAF threshold used for the pixels from two lines in one
         field. This is the threshold used for combing artifacts
         detection. The difference of two consecutive lines from
         the same field (so there is one line in between if two
         fields are merged into one progressive frame) is compared
         with this threshold. Decreasing this threshold leads to
         be more conservative in detecting CAF.  Both
         fmd_caf_field_thr and fmd_caf_line_thr are close the values
         that two pixels differed by this value is observable. */
    UInt32                  cafFieldThr;
    /**< CAF threshold used for the pixels from two fields.
         This is the threshold used for combing artifacts detection.
         The difference of two consecutive lines (when merging two
         fields into one progressive frame) is used to compare with
         this threshold. Increasing this threshold leads to be more
         conservative in detecting CAF. */
    UInt32                  frameDiff;
    /**< Frame difference (difference between two top or two bottom fields).
         Read only parameter. */
    UInt32                  fldDiff;
    /**< Field difference (difference between two neighboring fields, one
         top and one bottom).
         Read only parameter. */
    UInt32                  reset;
    /**< When 1, the film mode detection module needs to be reset by
         the software. This bit needs to be checked at each occurrence
         of the film mode detection interrupt.
         Read only parameter. */
    UInt32                  caf;
    /**< Detected combing artifacts.
         Read only parameter. */
} Vps_DeiFmdConfig;

/**
 *  struct Vps_DeiRtConfig
 *  \brief Struture to configure runtime DEI parameters.
 */
typedef struct
{
   UInt32                   resetDei;
   /**< Resets the DEI context. */
   UInt32                   fldRepeat;
   /**< Field repeat flag. Application should set this flag to TRUE if
        it repeats the input field. This is needed to disable
        context buffer roatation to generate output as same as previous field
        without any field artifacts. */
} Vps_DeiRtConfig;

/**
 *  struct Vps_DeiCtxInfo
 *  \brief DEI context information containing the number of buffers and size of
 *  the buffers needed for deinterlacing operation.
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Channel number from which to get the DEI context information.
         If only one channel is supported, then this should be set to 0. */
    UInt32                  numFld;
    /**< Number of field buffers needed for deinterlacing operation.
         This is not applicable for platform TI814X. Please refer the
         user guide of TI814X, that came with this release. */
    UInt32                  numMv;
    /**< Number of MV buffers needed for deinterlacing operation. */
    UInt32                  numMvstm;
    /**< Number of MVSTM buffers needed for deinterlacing operation.
         This is not applicable for platform TI814X. Please refer the
         user guide for TI814X, that came with this release. */
    UInt32                  fldBufSize;
    /**< Size of one field buffer in bytes.

         In case application wants to allocate the context buffer based on the
         maximum input resolution it operates with, then below equation could
         be used for calculating the context buffer sizes,

         Size = Align(width, 16) * field height * 2
         If compressor is enabled, then the size could be half of above value.

         This is not applicable for platform TI814X. Please refer the
         user guide for TI814X, that came with this release. */
    UInt32                  mvBufSize;
    /**< Size of one MV buffer in bytes.

         In case application wants to allocate the context buffer based on the
         maximum input resolution it operates with, then below equation could
         be used for calculating the context buffer sizes,

         Size = Align(width/2, 16) * field height */
    UInt32                  mvstmBufSize;
    /**< Size of one MVSTM buffer in bytes.

         In case application wants to allocate the context buffer based on the
         maximum input resolution it operates with, then below equation could
         be used for calculating the context buffer sizes,

         Size = Align(width/2, 16) * field height

         This is not applicable for platform TI814X. Please refer the
         user guide for TI814X, that came with this release. */
} Vps_DeiCtxInfo;

/**
 *  struct Vps_DeiCtxBuf
 *  \brief DEI context buffers needed for deinterlacing operation.
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Channel number to which the DEI context buffers be given.
         If only one channel is supported, then this should be set to 0. */
    Void                   *fldBuf[VPS_DEI_MAX_CTX_FLD_BUF];
    /**< Array of DEI context field buffer pointers.
         This is not applicable for platform TI814X. Please refer the
         user guide for TI814X, that came with this release. */
    Void                   *mvBuf[VPS_DEI_MAX_CTX_MV_BUF];
    /**< Array of DEI context MV buffer pointers. */
    Void                   *mvstmBuf[VPS_DEI_MAX_CTX_MVSTM_BUF];
    /**< Array of DEI context MVSTM buffer pointers.
         This is not applicable for platform TI814X. Please refer the
         user guide for TI814X, that came with this release. */
} Vps_DeiCtxBuf;

/**
 *  struct Vps_DeiHqConfig
 *  \brief High quality DEI configuration.
 */
typedef struct
{
    UInt32                  bypass;
    /**< DEI should be bypassed or not.
     *   1. For interlaced input and interlaced output from DEI/VIP Scalar,
     *   the DEI could be operated in two modes.
     *      a. DEI in bypass mode and the respective scalar in non-interlacing
     *      mode (interlaced input DEI, interlaced output from Scalar).
     *      In this case,
     *      DEI bypass = TRUE
     *      Scalar bypass = TRUE/FALSE depending up on whether scaling is
     *      required or not.
     *      b. DEI in deinterlacing mode - converts interlaced input to
     *      progressive output to the scalar. Scalar in interlacing mode -
     *      converts the progressive input from DEI to interlaced output.
     *      In this case,
     *      DEI bypass = FALSE
     *      Scalar bypass = FALSE
     *   2. For progressive input and interlaced output, Scalar will be in
     *      interlacing mode.
     *      DEI bypass = TRUE
     *      Scalar bypass = FALSE
     *   3. For progressive input and progressive output,
     *      DEI bypass = TRUE
     *      Scalar bypass = TRUE/FALSE depending up on whether scaling is
     *      required or not.
     *   4. For interlaced input and progressive output,
     *      DEI bypass = FALSE
     *      Scalar bypass = TRUE/FALSE depending up on whether scaling is
     *      required or not.
     */
    UInt32                  inpMode;
    /**< Interpolation mode. For valid values see #Vps_DeiHqEdiMode. */
    UInt32                  tempInpEnable;
    /**< 3D processing (temporal interpolation). */
    UInt32                  tempInpChromaEnable;
    /**< 3D processing for chroma. */
    UInt32                  spatMaxBypass;
    /**< Bypass spatial maximum filtering. */
    UInt32                  tempMaxBypass;
    /**< Bypass temporal maximum filtering. */
    UInt32                  fldMode;
    /**< Motion Detection (MDT) mode. Determines how many previous fields
         are used in motion detection.
         For valid values see #Vps_DeiHqFldMode. */
    UInt32                  lcModeEnable;
    /**< Low cost mode disables logics that reduces flicker and produces
         smooth motion in areas of detail. It is recommended to use
         Advanced mode, so should be set to false. */
    UInt32                  mvstmEnable;
    /**< Enable MVSTM loop. MVSTM should be enabled for the Advanced
         mode but it is optional for the low cost mode. */
    UInt32                  tnrEnable;
    /**< Enable TNR for both Luma as well as for Chroma.
         If the DEI bypass flag is TRUE (as in the case of progressive inputs),
         the driver will internally set the DEI in non-bypass mode when TNR
         needs to be enabled. */
    UInt32                  snrEnable;
    /**< Enable Snr for both Luma as well as for Chroma. */
    UInt32                  sktEnable;
    /**< Enable Skin Tone Detection. */
    UInt32                  chromaEdiEnable;
    /**< Edge adaptive interpolation for chroma. */
} Vps_DeiHqConfig;

/**
 *  struct Vps_DeiConfig
 *  \brief DEI configuration.
 */
typedef struct
{
    UInt32                  bypass;
    /**< DEI should be bypassed or not.
     *   1. For interlaced input and interlaced output from DEI/VIP Scalar,
     *   the DEI could be operated in two modes.
     *      a. DEI in bypass mode and the respective scalar in non-interlacing
     *      mode (interlaced input DEI, interlaced output from Scalar).
     *      In this case,
     *      DEI bypass = TRUE
     *      Scalar bypass = TRUE/FALSE depending up on whether scaling is
     *      required or not.
     *      b. DEI in deinterlacing mode - converts interlaced input to
     *      progressive output to the scalar. Scalar in interlacing mode -
     *      converts the progressive input from DEI to interlaced output.
     *      In this case,
     *      DEI bypass = FALSE
     *      Scalar bypass = FALSE
     *   2. For progressive input and interlaced output, Scalar will be in
     *      interlacing mode.
     *      DEI bypass = TRUE
     *      Scalar bypass = FALSE
     *   3. For progressive input and progressive output,
     *      DEI bypass = TRUE
     *      Scalar bypass = TRUE/FALSE depending up on whether scaling is
     *      required or not.
     *   4. For interlaced input and progressive output,
     *      DEI bypass = FALSE
     *      Scalar bypass = TRUE/FALSE depending up on whether scaling is
     *      required or not.
     */
    UInt32                  inpMode;
    /**< Interpolation mode. For valid values see #Vps_DeiEdiMode. */
    UInt32                  tempInpEnable;
    /**< 3D processing (temporal interpolation). */
    UInt32                  tempInpChromaEnable;
    /**< 3D processing for chroma. */
    UInt32                  spatMaxBypass;
    /**< Bypass spatial maximum filtering. */
    UInt32                  tempMaxBypass;
    /**< Bypass temporal maximum filtering. */
} Vps_DeiConfig;


/* ========================================================================== */
/* Include HW specific advance config constants, structures                   */
/* ========================================================================== */

#include <ti/psp/vps/vps_advCfgDeiHq.h>
#include <ti/psp/vps/vps_advCfgDei.h>


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif

/* @} */
