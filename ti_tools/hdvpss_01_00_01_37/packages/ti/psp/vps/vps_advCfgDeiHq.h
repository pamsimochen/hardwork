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
 *  \file vps_advCfgDeiHq.h
 *
 *  \brief HD-VPSS - High Quality De-interlacer Advanced Config API
 */

#ifndef _VPS_ADV_CFG_DEIHQ_H
#define _VPS_ADV_CFG_DEIHQ_H

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

/* MDT configuration related macros. */
/** \brief Maximum number of spatial frequency coring threshold values */
#define VPS_DEIHQ_MDT_MAX_SF_COR_THR        (3u)
/** \brief Maximum number of spatial-temporal frequency threshold values */
#define VPS_DEIHQ_MDT_MAX_STF_FD_THR        (3u)
/** \brief Maximum number of right shift values applied to field difference */
#define VPS_DEIHQ_MDT_MAX_FIKM_RIGHT_SHIFT  (8u)
/** \brief Maximum number of activity detect threshold values */
#define VPS_DEIHQ_MDT_MAX_ACT_DETECT_THR    (3u)

/* EDI configuration related macros. */
/** \brief Maximum number of SAD threshold values used for coarse-resolution
           edge detection */
#define VPS_DEIHQ_EDI_MAX_SAD_THR           (6u)
/** \brief Maximum number of EDI lookup tables */
#define VPS_DEIHQ_EDI_MAX_LUT               (16u)

/* TNR configuration related macros. */
/** \brief Maximum number of TNR lookup tables */
#define VPS_DEIHQ_TNR_MAX_LUT               (16u)

/* SNR configuration related macros. */
/** \brief Maximum number of thresholds in signa filters */
#define VPS_DEIHQ_SNR_MAX_THR               (2u)
/** \brief Maximum number of values of shift used in determining INR mode */
#define VPS_DEIHQ_SNR_MAX_INR_SHIFT         (3u)

/**
 *  enum Vps_DeiHqTnrAdvMode
 *  \brief Enumerations for Advanced Temporal Noise Reduction (TNR) mode.
 */
typedef enum
{
    VPS_DEIHQ_TNRADVMODE_REGULAR = 0,
    /**< Regular TNR mode. */
    VPS_DEIHQ_TNRADVMODE_DITHERING
    /**< Dithering scheme used. */
} Vps_DeiHqTnrAdvMode;

/**
 *  enum Vps_DeiHqSnrInrMode
 *  \brief Enumerations for SNR Impulse Noise Reduction (INR) mode.
 */
typedef enum
{
    VPS_DEIHQ_SNRINRMODE_NONE = 0,
    /**< No INR. */
    VPS_DEIHQ_SNRINRMODE_LEAST_AGGR = 1,
    /**< Least aggressive INR mode. */
    VPS_DEIHQ_SNRINRMODE_MORE_AGGR = 2,
    /**< More aggressive INR mode. */
    VPS_DEIHQ_SNRINRMODE_MOST_AGGR = 3,
    /**< Most aggressive INR mode. */
    VPS_DEIHQ_SNRINRMODE_ADAPTIVE = 4
    /**< Adaptive INR mode. */
} Vps_DeiHqSnrInrMode;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_DeiHqMdtConfig
 *  \brief High quality DEI MDT configuration.
 */
typedef struct
{
    UInt32                  fldMode;
    /**< Motion detection mode (4/5 field mode).
         For valid values see #Vps_DeiHqFldMode. */
    UInt32                  spatMaxBypass;
    /**< Bypass spatial maximum filtering. */
    UInt32                  tempMaxBypass;
    /**< Bypass temporal maximum filtering. */
    UInt32                  mvIIR;
    /**< Enable Motion IIR filtering. */
    UInt32                  lcModeEnable;
    /**< Low cost mode disables logics that reduces flicker and produces smooth
         motion in areas of detail. */
    UInt32                  adaptiveCoring;
    /**< Adaptive coring. */
    UInt32                  fldComp;
    /**< Field comp. */
    UInt32                  edgeKd;
    /**< Edge detect for mixing luma diffs. */
    UInt32                  edgeVarThrld;
    /**< Edge detection for varying thrld. */
    UInt32                  forceSlomoDiff;
    /**< Force luma differences to include immediate vicinity. */
    UInt32                  forceF02Diff;
    /**< Force luma differences to frame difference. */
    UInt32                  forceMv3D;
    /**< Force advanced mode to produce 3D. */
    UInt32                  forceMv2D;
    /**< Force advanced mode to produce 2D. */
    UInt32                  useDynGain;
    /**< Use dynamic gain. */
    UInt32                  enable3PixelFilt;
    /**< Enable 3 pixel linear filter. */
    UInt32                  mvCorThrMax;
    /**< Maximum value of coring threshold for mv. */
    UInt32                  mvCorThr;
    /**< This is the coring threshold for motion value, mv.
         MDT will become more noise robust if this value increases.
         But the picture may be washed out if this value is set to
         high. This threshold can be interpreted as the noise threshold
         for calculating motion values for all blocks. */
    UInt32                  sfCorThr[VPS_DEIHQ_MDT_MAX_SF_COR_THR];
    /**< Spatial frequency threshold. It is used for adaptive
         scaling of motion values according to how busy the texture is. */
    UInt32                  sceneChangeThr;
    /**< Index to scene change threshold. This index should be within
         the range of [0,6]. Increasing it leads to higher threshold
         for detecting scene change between frames. */
    UInt32                  mvstmCorThr;
    /**< This is the coring threshold for the spatio-temporal
         maximum output of motion values. It is used for increasing
         noise robustness. */
    UInt32                  stfFdThr[VPS_DEIHQ_MDT_MAX_STF_FD_THR];
    /**< Spatial-temporal frequency threshold. It is used for
         controlling the coefficient of the IIR filter coefficient
         in MDT, which is adaptively adjusted based on the
         detected spatio-temporal frequency. */
    UInt32                  edgeDetectThr;
    /**< Edge detect threshold. */
    UInt32                  activityThr;
    /**< Activity threshold. */
    UInt32                  edgeRtShift;
    /**< Edge detect gain right shift. */
    UInt32                  vfGain;
    /**< Vertical frequency identification gain. Real number in 1.3 format. */
    UInt32                  vfThr;
    /**< Vertical frequency identification threshold. */
    UInt32                  thrVarLimit;
    /**< Limit on threshold variablility. */
    UInt32                  kdetVarThrGain;
    /**< Gain on threshold control parameter. Real number in 2.2 format. */
    UInt32                  edgeRtShiftVarth;
    /**< Gain on threshold control parameter. Real number in 2.2 format. */
    UInt32                  yDetectThr;
    /**< Immediate vicinity detail threshold. */
    UInt32                  yDetectGain;
    /**< Immediate vicinity detail gain. Real number in 2.2 format. */
    UInt32                  actDetectThr[VPS_DEIHQ_MDT_MAX_ACT_DETECT_THR];
    /**< Negative bias of edge on activity for threshold. */
    UInt32                  fld20Thr1;
    /**< Field 0 & 2 diff threshold 1 for motion parameter. */
    UInt32                  fld20Thr2;
    /**< Field 0 & 2 diff threshold 2 for motion parameter. */
    UInt32                  fld20Gain1;
    /**< Field 0 & 2 diff gain 1 for motion parameter. Real number
         in 2.6 format. */
    UInt32                  fld20Gain2;
    /**< Field 0 & 2 diff gain 2 for motion parameter. Real number
         in 2.6 format. */
    UInt32                  fikmRtShift[VPS_DEIHQ_MDT_MAX_FIKM_RIGHT_SHIFT];
    /**< Amount of right shift applied to field difference, which
         affect time constant of going to 3D. */
    UInt32                  pixFltHfCore;
    /**< High Frequency coring for differences to reduce impact of
         noise on motion detection. Setting 0 gives pixel frame
         differences, setting max gives 3x3 filtered frame differences. */
    UInt32                  fld012Thr1;
    /**< Field 0, 1, 2 diff threshold 1. */
    UInt32                  fld012Thr2;
    /**< Field 0, 1, 2 diff threshold 2. */
    UInt32                  fld012Gain1;
    /**< Field 0, 1, 2 diff gain 1. */
    UInt32                  fld012Gain2;
    /**< Field 0, 1, 2 diff gain 2. */
} Vps_DeiHqMdtConfig;

/**
 *  struct Vps_DeiHqEdiConfig
 *  \brief High quality DEI EDI configuration.
 */
typedef struct
{
    UInt32                  inpMode;
    /**< Interpolation mode. For valid values see #Vps_DeiHqEdiMode. */
    UInt32                  chromaEdiEnable;
    /**< Edge adaptive interpolation for chroma. */
    UInt32                  flagAreaThr;
    /**< Flat area threshold. If the luma difference within a small area is
         less than this threshold, this area is marked as flat area.
         Edge detection favors vertical vector in flat area. */
    UInt32                  spaIntThr;
    /**< Spatial correlation threshold used in luma spatial interpolation.
         By default 4-tap filter is used for vertical/diagonal interpolation.
         However, if the luma difference between the top line or bottom line
         and the middle lines are greater than this threshold, it means
         there is no strong correlation among the four lines. Thus 2-tap
         vertical/diagonal filter will be used instead. If the user prefers
         more conservative vertical interpolation, decrease this value. */
    UInt32                  chroma3DCorThr;
    /**< Chroma 3D confidence threshold. */
    UInt32                  ceiThr;
    /**< Chroma edge adaptive interpolation threshold. When the difference of
         the two pixels along the detected edge is greater than this threshold,
         edge-adaptive interpolation is disabled. This is because the edge is
         obtained based on luma only. Increasing this threshold leads to more
         chroma pixels being processed using edge adaptive interpolation. */
    UInt32                  sadThr[VPS_DEIHQ_EDI_MAX_SAD_THR];
    /**< SAD thresholds are used in coarse-resolution edge detection. Used in
         the block of coarse edge vector calculation. This is the threshold
         used to determine distinct minimum. If the difference of one
         minimums from the global minimum is greater the threshold, it should
         be a distinct minimum. */
    UInt32                  lut[VPS_DEIHQ_EDI_MAX_LUT];
    /**< EDI lookup table, which maps motion value to the blending coefficient
         of 2D/3D interpolation results. Increasing the values leads to
         favoring 2D interpolated results.
         Following must hold: 0<=lut0<=lut1<=lut2<=…<=lut15. */
} Vps_DeiHqEdiConfig;

/**
 *  struct Vps_DeiHqTnrConfig
 *  \brief High quality DEI TNR configuration.
 */
typedef struct
{
    UInt32                  chroma;
    /**< TNR for chroma. */
    UInt32                  luma;
    /**< TNR for luma. */
    UInt32                  adaptive;
    /**< TNR using adaptive threshold. */
    UInt32                  advMode;
    /**< Advanced TNR mode. For valid values see #Vps_DeiHqTnrAdvMode. */
    UInt8                   advScaleFactor;
    /**< Scale factor in advanced TNR. */
    UInt32                  skt;
    /**< Skin tone detection. */
    UInt8                   maxSktGain;
    /**< Maximum gain for detected skintone. */
    UInt8                   minChromaSktThrld;
    /**< Minimum diff of chroma value. */
    UInt32                  lumaMvLowThr;
    /**< Lower bound in converting luma difference to motion. */
    UInt32                  chromaMvLowThr;
    /**< Lower bound in converting chroma difference to motion. */
    UInt32                  lumaMvLowThrMax;
    /**< The max value of the lower bound in converting luma difference
         to motion. */
    UInt32                  chromaMvLowThrMax;
    /**< The max value of the lower bound in converting chroma difference
         to motion. */
    UInt32                  lumaScaleFactor;
    /**< Motion scaling factor for luma. */
    UInt32                  chromaScaleFoctor;
    /**< Motion scaling factor for chroma. */
    UInt32                  lut[VPS_DEIHQ_TNR_MAX_LUT];
    /**< TNR lookup table, which maps motion value to the coefficient of the
         temporal IIR filter. Decreasing the values leads to strong
         IIR filtering. 16 is equivalent to reset the IIR filter.
         Following must hold: 0<=lut0<=lut1<=lut2<=…<=lut15. */
} Vps_DeiHqTnrConfig;

/**
 *  struct Vps_DeiHqSnrConfig
 *  \brief High quality DEI SNR configuration.
 */
typedef struct
{
    UInt32                  gnrChroma;
    /**< GNR for chroma. */
    UInt32                  gnrLuma;
    /**< GNR for luma. */
    UInt32                  gnrAdaptive;
    /**< Adaptive GNR. */
    UInt32                  inrMode;
    /**< INR mode. For valid values see #Vps_DeiHqSnrInrMode. */
    UInt8                   gnmDevStableThrld;
    /**< GNR deviation stable threshold. */
    UInt32                  sigmaThr[VPS_DEIHQ_SNR_MAX_THR];
    /**< Threshold in sigma filter. */
    UInt32                  sigmaThrMax[VPS_DEIHQ_SNR_MAX_THR];
    /**< Max value of snr_sigma_thr used in adaptive GNR. */
    UInt32                  inrShift[VPS_DEIHQ_SNR_MAX_INR_SHIFT];
    /**< Number of shift used in determining INR mode. This value relates to
         the threshold for number of detected impulse noise in determining the
         mode of INR. Increasing this value leads to more likelihood to perform
         INR. Note that inr_shift1 >= inr_shift2>= inr_shift3 must hold. */
    UInt32                  inrTsmDeltaThr;
    /**< Impulse noise threshold in tri-state median algorithm.
         If the pixel difference is less than this threshold, no impulse
         will be assumed. */
    UInt32                  intMedDeltaThr;
    /**< Impulse noise threshold in median filter algorithm. If the pixel
         difference is less than this threshold, no impulse will be assumed. */
} Vps_DeiHqSnrConfig;

/**
 *  struct Vps_DeiHqGnmConfig
 *  \brief High quality DEI GNM configuration.
 */
typedef struct
{
    UInt32                  lumaNoiseDev;
    /**< Measured standard deviation of luma channel noise. */
    UInt32                  chromaNoiseDev;
    /**< Measured standard deviation of chroma channel noise. */
    UInt32                  lumaDelta0;
    /**< Pixel difference upper bound that determines histogram
         samples hist_0 for luma channel. */
    UInt32                  chromaDelta0;
    /**< Pixel difference upper bound that determines histogram
         samples hist_0 for chroma channel. */
    UInt32                  lumaDelta1;
    /**< Pixel difference upper bound that determines histogram
         samples hist_1 for luma channel. */
    UInt32                  chromaDelta1;
    /**< Pixel difference upper bound that determines histogram
         samples hist_1 for chroma channel. */
    UInt32                  lumaDeltam;
    /**< Pixel difference upper bound that determines histogram
         samples hist_m for luma channel. */
    UInt32                  chromaDeltam;
    /**< Pixel difference upper bound that determines histogram
         samples hist_m for chroma channel. */
    UInt32                  yHistz;
    /**< Histogram (number of pix differences with value equal to 0)
         for luma.
         Read only parameter. */
    UInt32                  yHist0;
    /**< Histogram (number of pix differences with value less than yDelta0)
         for luma.
         Read only parameter. */
    UInt32                  yHist1;
    /**< Histogram (number of pix differences with value less than yDelta1)
         for luma.
         Read only parameter. */
    UInt32                  yHistm;
    /**< Histogram (number of pix differences with value less than yDeltam)
         for luma.
         Read only parameter. */
    UInt32                  cHistz;
    /**< Histogram (number of pix differences with value equal to 0)
         for chroma.
         Read only parameter. */
    UInt32                  cHist0;
    /**< Histogram (number of pix differences with value less than cDelta0)
         for luma.
         Read only parameter. */
    UInt32                  cHist1;
    /**< Histogram (number of pix differences with value less than cDelta1)
         for luma.
         Read only parameter. */
    UInt32                  cHistm;
    /**< Histogram (number of pix differences with value less than cDeltam)
         for luma.
         Read only parameter. */
} Vps_DeiHqGnmConfig;

/**
 *  struct Vps_DeiHqRdWrAdvCfg
 *  \brief Structure to read/write to advance DEIHQ registers.
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
    UInt32                  tempInpEnable;
    /**< 3D processing (temporal interpolation). */
    UInt32                  tempInpChromaEnable;
    /**< 3D processing for chroma. */

    Vps_DeiHqMdtConfig      mdtCfg;
    /**< Motion detection configuration. */
    Vps_DeiHqEdiConfig      ediCfg;
    /**< Edge directed interpolation configuration. */
    Vps_DeiHqTnrConfig      tnrCfg;
    /**< Temporal noise filter configuration. */
    Vps_DeiFmdConfig        fmdCfg;
    /**< Film mode detection configuration. */
    Vps_DeiHqSnrConfig      snrCfg;
    /**< Spatial noise filter configuration. */
    Vps_DeiHqGnmConfig      gnmCfg;
    /**< Guassian noise filter configuration. */
} Vps_DeiHqRdWrAdvCfg;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif

/* @} */
