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
 *  \file vps_advCfgDei.h
 *
 *  \brief HD-VPSS - De-interlacer Advanced Configurations options.
 */

#ifndef _VPS_ADV_CFG_DEI_H
#define _VPS_ADV_CFG_DEI_H

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

/** \brief MDT configuration related macros. */
#define VPS_DEI_MDT_MAX_SF_SC_THR       (3u)

/** \brief EDI configuration related macros. */
#define VPS_DEI_EDI_MAX_LUT             (16u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_DeiMdtConfig
 *  \brief DEI Motion Dection (MDT) configuration.
 */
typedef struct
{
    UInt32                  spatMaxBypass;
    /**< Bypass spatial maximum filtering. */
    UInt32                  tempMaxBypass;
    /**< Bypass temporal maximum filtering. */
    UInt32                  mvstmMaxCorThr;
    /**< This threshold is for the coring for the spatial-maximum maximum
         output of motion values. It is used for increasing noise
         robustness. Increasing this threshold leads to more robustness
         to noise, but with the potential of introducing ghosting effect.
         Note that this threshold is used for motion values for EDI only,
         and it is in addition mdt_mv_cor_thr. */
    UInt32                  mvCorThr;
    /**< This is the coring threshold for motion value, mv.
         MDT will become more noise robust if this value increases.
         But the picture may be washed out if this value is set to
         high. This threshold can be interpreted as the noise threshold
         for calculating motion values for all blocks. */
    UInt32                  sfScThr[VPS_DEI_MDT_MAX_SF_SC_THR];
    /**< Spatial frequency threshold. It is used for adaptive scaling
         of motion values according to how busy the texture is. If
         the texture is flat, motion values need to be scaled up to
         reflect the sensitivity of motion values with respect to
         the detection error. Increasing the thresholds will make the
         motion value scaling more sensitive to the frequency of the
         texture. Note that 0<= sfScThr[0] <= sfScThr[1] <= sfScThr[2]
         must hold. */
} Vps_DeiMdtConfig;

/**
 *  struct Vps_DeiEdiConfig
 *  \brief Edge Directed Interpolation (EDI) configuration.
 */
typedef struct
{
    UInt32                  inpMode;
    /**< Interpolation mode. For valid values see #Vps_DeiEdiMode. */
    UInt32                  tempInpEnable;
    /**< 3D processing (temporal interpolation). */
    UInt32                  tempInpChromaEnable;
    /**< 3D processing for chroma. */
    UInt32                  corScaleFactor;
    /**< Scaling factor for correlation along detected edge. */
    UInt32                  detCorLowerThr;
    /**< Lower threshold used for correlation along detected edge */
    UInt32                  chroma3DCorThr;
    /**< Correlation threshold used in 3D processing for chroma. Because
         the motion values used for chroma 3D processing are based on
         luma only. Extra protection is needed. Temporal interpolation is
         only performed for chroma, when there is strong spatial or
         temporal correlation for the chroma pixel being processed. When
         the pixel difference is less than this threshold, it is
         assumed that there exists strong correlation between these
         two pixels. Thus, increasing this value leads to more chroma
         pixels being processed in 3D. */
    UInt32                  lut[VPS_DEI_EDI_MAX_LUT];
    /**< The EDI lookup table maps motion values to the coefficient
         of temporal filter. Increasing the values leads to favoring
         2D interpolated results.
         Following must hold: 0<=lut0<=lut1<=lut2<=…<=lut15. */
} Vps_DeiEdiConfig;

/**
 *  struct Vps_DeiRdWrAdvCfg
 *  \brief Structure to read/write to advance DEI registers.
 */
typedef struct
{
    UInt32                  chNum;
    /**< Channel number for which the configuration is to be set/get.
         Used only in M2M driver, for display and capture driver set
         this to 0. */
    UInt32                  bypass;
    /**< DEI should be bypassed or not. */
    UInt32                  width;
    /**< Width of the frame. */
    UInt32                  height;
    /**< Height of the frame. */
    UInt32                  scanFormat;
    /**< Scan format. For valid values see #FVID2_ScanFormat. */
    UInt32                  fieldFlush;
    /**< Flush internal pipe for current output. */

    Vps_DeiMdtConfig        mdtCfg;
    /**< Motion detection configuration. */
    Vps_DeiEdiConfig        ediCfg;
    /**< Edge directed interpolation configuration. */
    Vps_DeiFmdConfig        fmdCfg;
    /**< Film mode detection configuration. */
} Vps_DeiRdWrAdvCfg;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif

/* @} */
