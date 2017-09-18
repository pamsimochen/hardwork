/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_VPS_COMMON
 * \addtogroup VPS_DRV_FVID2_VPS_COMMON_SC  HD-VPSS - Scaler Config API
 *
 * @{
 */

/**
 *  \file vps_cfgSc.h
 *
 *  \brief HD-VPSS - Scalar Config API
 */

#ifndef _VPS_CFG_SC_H
#define _VPS_CFG_SC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Number of Scalar horizontal coefficient. */
#define VPS_SC_HORZ_NUM_COEFF           (224u)
/** \brief Number of Scalar vertical coefficient. */
#define VPS_SC_VERT_NUM_COEFF           (160u)
/** \brief Number of Scalar bilinear coefficient. */
#define VPS_SC_BILINEAR_NUM_COEFF       (224u)
/** \brief Number of peaking coefficients */
#define VPS_SC_PEAKING_MAX_COEFF        (6u)

/**
 *  enum Vps_ScCoeffSet
 *  \brief This defines the standard coefficient sets available for
 *  different scaling ratios.
 */
typedef enum
{
    VPS_SC_DS_SET_3_16,
    /**< Coefficient for the down sampling (3/16) < Factor <= (4/16). */
    VPS_SC_DS_SET_4_16,
    /**< Coefficient for the down sampling (4/16) < Factor <= (5/16). */
    VPS_SC_DS_SET_5_16,
    /**< Coefficient for the down sampling (5/16) < Factor <= (6/16). */
    VPS_SC_DS_SET_6_16,
    /**< Coefficient for the down sampling (6/16) < Factor <= (7/16). */
    VPS_SC_DS_SET_7_16,
    /**< Coefficient for the down sampling (7/16) < Factor <= (8/16). */
    VPS_SC_DS_SET_8_16,
    /**< Coefficient for the down sampling (8/16) < Factor <= (9/16). */
    VPS_SC_DS_SET_9_16,
    /**< Coefficient for the down sampling (9/16) < Factor <= (10/16). */
    VPS_SC_DS_SET_10_16,
    /**< Coefficient for the down sampling (10/16) < Factor <= (11/16). */
    VPS_SC_DS_SET_11_16,
    /**< Coefficient for the down sampling (11/16) < Factor <= (12/16). */
    VPS_SC_DS_SET_12_16,
    /**< Coefficient for the down sampling (12/16) < Factor <= (13/16). */
    VPS_SC_DS_SET_13_16,
    /**< Coefficient for the down sampling (13/16) < Factor <= (14/16). */
    VPS_SC_DS_SET_14_16,
    /**< Coefficient for the down sampling (14/16) < Factor <= (15/16). */
    VPS_SC_DS_SET_15_16,
    /**< Coefficient for the down sampling (15/16) < Factor. */
    VPS_SC_US_SET,
    /**< Coefficient set for the upsampling.  Includes horizontal, vertical
         and both chroma and luma up sampling. */
    VPS_SC_SET_1_1,
    /**< Coefficient set for one-to-one scenario, when scaler is not in
         bypass. */
    VPS_SC_SET_MAX,
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */

    /* Below are the values kept for backward compatibility. */
    VPS_SC_DS_SET_0 = VPS_SC_DS_SET_8_16,
    /**< Coefficient for the down sampling (8/16) < Factor <= (9/16). */
    VPS_SC_DS_SET_1 = VPS_SC_DS_SET_9_16,
    /**< Coefficient for the down sampling (9/16) < Factor <= (10/16). */
    VPS_SC_DS_SET_2 = VPS_SC_DS_SET_10_16,
    /**< Coefficient for the down sampling (10/16) < Factor <= (11/16). */
    VPS_SC_DS_SET_3 = VPS_SC_DS_SET_11_16,
    /**< Coefficient for the down sampling (11/16) < Factor <= (12/16). */
    VPS_SC_DS_SET_4 = VPS_SC_DS_SET_12_16,
    /**< Coefficient for the down sampling (12/16) < Factor <= (13/16). */
    VPS_SC_DS_SET_5 = VPS_SC_DS_SET_13_16,
    /**< Coefficient for the down sampling (13/16) < Factor <= (14/16). */
    VPS_SC_DS_SET_6 = VPS_SC_DS_SET_14_16,
    /**< Coefficient for the down sampling (14/16) < Factor <= (15/16). */
    VPS_SC_DS_SET_7 = VPS_SC_DS_SET_15_16
    /**< Coefficient for the down sampling (15/16) < Factor. */
} Vps_ScCoeffSet;

/**
 *  enum Vps_ScHorzScalarType
 *  \brief This defines the type of horizontal scalar.
 *  Two types of horizontal scalars are there in hardware,
 *  polyphase scalar and decimation scalar. Use decimation scalar for
 *  1/2 and 1/4x scaling. Else use polyphase scalar. If auto is selected
 *  hardware will take care of using appropriate scalar.
 */
typedef enum
{
    VPS_SC_HST_AUTO = 0,
    /**< Automatically selects decimation filter and then polyphase filter if
         required. */
    VPS_SC_HST_DCM_2X,
    /**< Selects 2X decimation Filter. Used for downscaling only. Not used when
         down scaling ratio > 0.5x. */
    VPS_SC_HST_DCM_4X,
    /**< Selects 4X decimation Filter. Used for down scaling only. Not used
         when down scaling ratio > 0.5x. */
    VPS_SC_HST_POLYPHASE,
    /**< Selects polyphase filter. Used for both down as well as up scaling.
         When selected decimation is disabled. Used for all
         scaling ratio > 0.5x. */
    VPS_SC_HST_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_ScHorzScalarType;

/**
 *  enum Vps_ScVertScalarType
 *  \brief This defines type of vertical scaling. Vertical
 *  scaling two types scalars i.e. running average filter and polyphase filter.
 *  The output of the running average filter is based on weighted average
 *  of pixels in the current and previous rows in vertical direction. Weighted
 *  Running Average filter is used only for downscaling by more than 50%. For
 *  other scaling ratios, polyphase scalar is used.
 */
typedef enum
{
    VPS_SC_VST_POLYPHASE = 0,
    /**< Selects polyphase filter. Used for both down as well as up scaling. */
    VPS_SC_VST_RAV,
    /**< Uses Running Average Filter. Used for down scaling only. Vertical
         Scaling ratio <= 0.5x should use RAV. */
    VPS_SC_VST_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_ScVertScalarType;

/**
 *  enum Vps_ScBiLinearIntpType
 *  \brief Enum for specifying Scalar biliear interpolation type.
 */
typedef enum
{
    VPS_SC_BINTP_ORIGINAL = 0,
    /**< Original bilinear interpolation. */
    VPS_SC_BINTP_MODIFIED,
    /**< Modified bilinear interpolation. */
    VPS_SC_BINTP_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_ScBiLinearIntpType;

/**
 *  enum Vps_ScSelfGenFid
 *  \brief Enum for specifying whether Scalar generates FID or not.
 */
typedef enum
{
    VPS_SC_SELF_GEN_FID_DISABLE = 0,
    /**< Disable Self Generation of FID. */
    VPS_SC_SELF_GEN_FID_ENABLE
    /**< Enable Self Generation of FID. */
} Vps_ScSelfGenFid;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_ScCoeff
 *  \brief Structure is used to provide application defined scalar coefficients.
 *  Scalar provides functions to configure standard coefficients as per the
 *  scaling ratio. If application wants to use its own set of coefficients, it
 *  can point the pointers in the structure to the coefficient array and call
 *  Sc_createUserCoeffOvly function to fill up coefficient memory.
 */
typedef struct
{
    UInt16                  hsLuma[VPS_SC_HORZ_NUM_COEFF];
    /**< Array in which horizontal polyphase filter coefficients for Luma
         will be stored. */
    UInt16                  hsChroma[VPS_SC_HORZ_NUM_COEFF];
    /**< Array in which horizontal polyphase filter coefficients for Chroma
         will be stored. */
    UInt16                  vsLuma[VPS_SC_VERT_NUM_COEFF];
    /**< Array in which vertical polyphase filter coefficients for Luma
         will be stored. */
    UInt16                  vsChroma[VPS_SC_VERT_NUM_COEFF];
    /**< Array in which vertical polyphase filter coefficients for Chroma
         will be stored. */
    UInt16                  vsBilinearLuma[VPS_SC_BILINEAR_NUM_COEFF];
    /**< Array in which bilinear polyphase filter coefficients for Luma
         will be stored. */
    UInt16                  vsBilinearChroma[VPS_SC_BILINEAR_NUM_COEFF];
    /**< Array in which bilinear polyphase filter coefficients for Chroma
         will be stored. */
} Vps_ScCoeff;

/**
 *  struct Vps_ScConfig
 *  \brief Scalar parameters.
 */
typedef struct
{
    UInt32                  bypass;
    /**< Scalar should be bypassed or not. */
    UInt32                  nonLinear;
    /**< Flag to enabled the non linear scaling like from 16/9 aspect
         ratio to 4/3 aspect ratio. */
    UInt32                  stripSize;
    /**< Strip size for the non linear scaling. */
    UInt32                  hsType;
    /**< Scalar type to be used for horizontal scaling. Scalar type are
         auto, 2x decimation filter, 4x decimation filter or polyphase filter.
         Recommended type is auto.  If auto is selected driver will select the
         appropriate scalar according to scaling ratio selected.
         For valid values see #Vps_ScHorzScalarType. */
    UInt32                  vsType;
    /**< Scalar type to used for vertical scaling.  Scalers are polyphase
         and running average scalar. Polyphase filters can be used for both
         up as well as down scaling.  Running average is used for downscaling
         ratios <= 0.5x.
         For valid values see #Vps_ScVertScalarType. */
    UInt32                  enablePeaking;
    /**< Enable or disable peaking filter. Peaking filter is a low pass filter
     * This should normally enabled for downscaling and disabled for up scaling
     */
    UInt32                  enableEdgeDetect;
    /**< Enables/disables edge detection. defConfFactor is only used when edge
         detection is disabled. Edge detection is only available in high
         quality scalar. */

} Vps_ScConfig;

/**
 *  struct Vps_ScCoeffParams
 *  \brief Application interface structure for programming the coefficients.
 *  Structure will be used for all drivers involving scalars.
 */
typedef struct
{
    UInt32                  scalarId;
    /**< Scalar ID - Used in case multiple scalars are present in a driver. */
    UInt32                  hScalingSet;
    /**< Horizontal scaling coefficient set.
     *   For valid values see #Vps_ScCoeffSet.
     */
    UInt32                  vScalingSet;
    /**< Vertical scaling coefficeint set.
     *   For valid values see #Vps_ScCoeffSet.
     */
    Ptr                     coeffPtr;
    /**< Pointer to provide user defined coefficients.
     *   This should point to a valid #Vps_ScCoeff structure.
     */
} Vps_ScCoeffParams;

/**
 *  struct Vps_ScLazyLoadingParams
 *  \brief Application interface structure for setting Lazy Loading
 *  configuration for Scalar.
 *  Structure will be used for all drivers involving scalars.
 */
typedef struct
{
    UInt32                  scalarId;
    /**< Scalar ID - Used in case multiple scalars are present in a driver. */
    UInt32                  enableLazyLoading;
    /**< Indicates whether lazy loading is to be enabled.
     *   If TRUE, lazy loading is to be enabled.
     *   If FALSE, lazy loading is to be disabled.
     */
    UInt32                  enableFilterSelect;
    /**< Indicates whether the filter to be used should be automatically
     *   selected based on scaling ratios.
     *   If TRUE, automatic filter selection is to be enabled.
     *   If FALSE, automatic filter selection is to be disabled.
     *   If Lazy Loading is enabled, it is recommended that this is set to TRUE.
     */
} Vps_ScLazyLoadingParams;

/**
 *  struct Vps_ScRtConfig
 *  \brief Struture to configure runtime scalar parameters.
 */
typedef struct
{
    UInt32                  scBypass;
    /**< To bypass the scalar .*/
    UInt32                  scSetPhInfo;
    /**< Flag to indicate whether to program scalar phase information.
         This is used only for subframe based processing to set scalar state for
         required phase values by application.
         If this flag is TRUE, following values are programmed:
               for PolyPh : row_acc_inc, row_acc_offset,row_acc_offset_b
               for RAV    : Sc_factor_rav, Row_acc_init_rav, Row_acc_init_rav_b
         If this flag is FALSE, below values are ignored. */
    UInt32                  rowAccInc;
    /**< Row accumulator increment for vertical polyphase filter. */
    UInt32                  rowAccOffset;
    /**< Scalar phase value for vertical polyphase filter case for
         top field/progressive input. */
    UInt32                  rowAccOffsetB;
    /**< Scalar phase value for vertical polyphase filter case for
         bottom field. */
    UInt32                  ravScFactor;
    /**< Scaling factor for vertical running average filter. */
    UInt32                  ravRowAccInit;
    /**< Scalar phase value for vertical running average filter case for
         top field/progressive input. */
    UInt32                  ravRowAccInitB;
    /**< Scalar phase value for vertical running average filter case for
         bottom field. */
    UInt32                  linAccIncr;
    /**< Row accumulator increment for horizontal polyphase filter. */
    UInt32                  colAccOffset;
    /**< Scalar phase value for horizontal polyphase case. */
} Vps_ScRtConfig;

/**
 *  struct Vps_ScPeakingConfig
 *  \brief This structure defines the parameters required to configure the
 *  peaking block.
 *
 *  Peaking block increases the amplitude of high frequency luminance
 *  information in horizontal direction to increase the sharpness of a video
 *  image before it is scaled.
 *  This configuration is for the advanced users and for debugging.
 *  Normal applications should not use this structure.
 */
typedef struct
{
    UInt8                   hpfCoeff[VPS_SC_PEAKING_MAX_COEFF];
    /**< Array of coefficients for 11 tap high pass filter. */
    UInt8                   hpfNormShift;
    /**< HPF value will be right shifted by this value. */
    UInt16                  nlClipLimit;
    /**< Non linear clipping limit. */
    UInt16                  nlLowThreshold;
    /**< Coring threshold. */
    UInt16                  nlHighThreshold;
    /**< High threshold for nonlinear peaking block. */
    UInt16                  nlLowSlopeGain;
    /**< Slope of the nonlinear peaking function. */
    UInt16                  nlHighSlopeShift;
    /**< Shift of the nonlinear peaking function. */
} Vps_ScPeakingConfig;

/**
 *  struct Vps_ScEdgeDetectConfig
 *  \brief Edge Detection Params.
 *
 *  This block is used to detect edges in image to have edge directed scaling.
 *  This block is available in high quality scalar only.
 *  This configuration is for the advanced users and for debugging.
 *  Normal applications should not use this structure.
 *  confFactor - Defines confidence factor when edge detection is disabled.
 */
typedef struct
{
    UInt16                  gradientThreshold;
    /**< Threshold for the gradient for the edge strength test. */
    UInt8                   gradientRange;
    /**< Define the soft switch range of the edge strength test. The range is
         2^gradient_thr_range. */
    UInt16                  minGyThreshold;
    /**< Threshold for the soft switch of the decay for small Gy. */
    UInt8                   minGyRange;
    /**< Range for minimum Gy threshold. The range is 2^minGyRange. */
    UInt16                  chromaIntpThreshold;
    /**< defines the difference-threshold between chroma pixels. If the
         difference is less than this threshold, the interpolation of chroma
         should be done along edge direction. Otherwise the interpolation
         of chroma should be done vertically. */
    UInt16                  evThreshold;
    /**< If abs(edge-vector) > ev_thr then luma output is based on a soft
         switch between pixels along the edge and vertical. */
    UInt8                   deltaChromaThreshold;
    /**< It defines the range for chroma soft switch based on pixel
         differences. */
    UInt8                   deltaLumaThreshold;
    /**< It defines the range of luma soft switch based on pixel difference. */
    UInt8                   deltaEvThreshold;
    /**< It defines the range for chroma soft switch based on edge vector. */
} Vps_ScEdgeDetectConfig;

/**
 *  struct Vps_ScAdvConfig
 *
 *  \brief This structure defines the parameters required to
 *  configure scalar. It can be used to set the parameters in the scalar
 *  registers as well as in the VPDMA register overlay. It also is used to
 *  configure runtime changes in the scale ratio. For memory to memory and
 *  display driver, all the parameters in the structure will be used and
 *  configured. For the runtime changes, only size and position parameters will
 *  be configured but still upper layer will have to pass all the parameters
 *  because scalar uses these parameters to calculate ratio specific parameters.
 *
 *  WARNING: This for the advanced user for the tuning of the hardware
 *  this strucutre should not be used by normal applications.
 */
typedef struct
{
    UInt32                  tarWidth;
    /**< Target image width.
         Used only for read command. Don't care for write command. */
    UInt32                  tarHeight;
    /**< Target image weight.
         Used only for read command. Don't care for write command. */
    UInt32                  srcWidth;
    /**< Source image width. This is the original non-cropped image width.
         Used only for read command. Don't care for write command. */
    UInt32                  srcHeight;
    /**< Source image height. This is the original non-cropped image height.
         Used only for read command. Don't care for write command. */
    UInt32                  cropStartX;
    /**< Horizontal offset of the cropped image. From this horizontal offset,
         original image will be cropped. If cropping is not required, this
         should be set to zero.
         Used only for read command. Don't care for write command. */
    UInt32                  cropStartY;
    /**< Vertical offset of the cropped image. From this vertical offset,
         original image will be cropped. If cropping is not required, this
         should be set to zero.
         Used only for read command. Don't care for write command. */
    UInt32                  cropWidth;
    /**< Cropped image width. Original image will be cropped to this size. If
         cropping is not required, then this should same as the original image
         size. Used only for read command. Don't care for write command. */
    UInt32                  cropHeight;
    /**< Cropped image Height. Original image will be cropped to this size. If
         cropping is not required, then this should same as the original image
         size. Used only for read command. Don't care for write command. */
    UInt32                  inFrameMode;
    /**< Input frame scan Format i.e interlaced or progressive.
         For valid values see #FVID2_ScanFormat.
         Used only for read command. Don't care for write command. */
    UInt32                  outFrameMode;
    /**< Output Frame scanFormat i.e. interlaced or progressive. Input and
         output frame mode can be different so that scalar can convert
         one format to another. For valid values see #FVID2_ScanFormat.
         Used only for read command. Don't care for write command */
    UInt32                  nonLinear;
    /**< Flag to indicate whether linear or non-linear scaling is used for
         horizontal scalar. Non-scaling is available for polyphase filter
         only.
         Used only for read command. Don't care for write command. */
    UInt32                  stripSize;
    /**< Size of left and right strip for nonlinear horizontal scaling in terms
         of pixel. It must be set to zero for linear horz scaling.
         Used only for read command. Don't care for write command. */
    UInt32                  fidPol;
    /**< This is used to specify whether input FID will be inverted before use
         or not. For valid value see #Vps_FidPol.
         Used only for read command. Don't care for write command. */
    UInt32                  selfGenFid;
    /**< Flag to indicate whether to use self Generate FID feature of the
         scalar. For valid value see #Vps_ScSelfGenFid.
         Used only for read command. Don't care for write command. */
    UInt32                  bypass;
    /**< Flag to indicate whether to bypass scalar or not.
        Used only for read command. Don't care for write command. */

    UInt32                  hsType;
    /**< This is used to specify horizontal scaling algorithm. Typically,
         decimation is used for down scaling and polyphase filters are used
         for up scaling.
         For valid values see #Vps_ScHorzScalarType */
    UInt32                  vsType;
    /**< This is used to specify vertical scaling algorithm. Typically, running
         average filter is used for down scaling and polyphase filters are used
         for up scaling.
         For valid values see #Vps_ScVertScalarType */
    UInt16                  defConfFactor;
    /**< Default confidence factor when edge detection is disabled. This is
         used only for the high quality scalar where edge detection block is
         available. */
    UInt32                  biLinIntpType;
    /**< Variable to specify bilinear interpolation type.
         For valid values see #Vps_ScBiLinearIntpType. */
    UInt32                  enableEdgeDetect;
    /**< Enables/disables edge detection. defConfFactor is only used when edge
         detection is disabled. Edge detection is only available in high
         quality scalar. */
    UInt32                  hPolyBypass;
    /**< Flag to bypass horizontal scalar. When enabled, it bypasses horizontal
         polyphase scalar when tar_w = src_w or tar_w = src_w/2 or
         tar_w = src_w/4. When disabled, polyphase scalar is always used
         regardless of the width configuration. */
    UInt32                  enablePeaking;
    /**< Flag to enable peaking block in the scalar. All the
         coefficients required for the peaking block are already configured
         as part of initialization. */
    UInt32                  rowAccInc;
    /**< Row accumulator increment for vertical polyphase filter. */
    UInt32                  rowAccOffset;
    /**< Scalar phase value for vertical polyphase filter case for
         top field/progressive input. */
    UInt32                  rowAccOffsetB;
    /**< Scalar phase value for vertical polyphase filter case for
         bottom field. */
    UInt32                  ravScFactor;
    /**< Scaling factor for vertical running average filter. */
    UInt32                  ravRowAccInit;
    /**< Scalar phase value for vertical running average filter case for
         top field/progressive input. */
    UInt32                  ravRowAccInitB;
    /**< Scalar phase value for vertical running average filter case for
         bottom field. */
    Vps_ScPeakingConfig    *peakingCfg;
    /**< Pointer to peaking params.
         If this is NULL configuation will not be applied. Else configuration
         for this will be applied.
         Values applied will be applicable to all the channels of the driver.
         This is not per channel configuration. */
    Vps_ScEdgeDetectConfig *edgeDetectCfg;
    /**< Pointer to edege detect parameters.
         If this is NULL configuation will not be applied. Else configuration
         for this will be applied.
         Values applied will be applicable to all the channels of the driver.
         This is not per channel configuration. */
    Ptr                     reserved;
    /**< Currently not used. It should be NULL. */
} Vps_ScAdvConfig;

/**
 *  sturct Vps_ScRdWrAdvCfg
 *  \brief Structure to read/write to advance scalar registers.
 */
typedef struct
{
    UInt32                  chNum;
    /**< Channel number for which the configuration is to be set/get.
         Used only in M2M driver, for display and capture driver set
         this to 0. */
    UInt32                  scalarId;
    /**< Scalar ID - Used in case multiple scalars are present in a driver. */
    Vps_ScAdvConfig         scAdvCfg;
    /**< Advanced scalar configuration to be read/write. */
} Vps_ScRdWrAdvCfg;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif

/* @} */
