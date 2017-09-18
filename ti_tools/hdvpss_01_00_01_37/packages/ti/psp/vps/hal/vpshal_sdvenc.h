/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_sdvenc.h
 *
 * \brief VPS SDVENC HAL header file
 * This file exposes the HAL APIs of the VPS SDVENC.
 *
 */

#ifndef _VPSHAL_SDVENC_H
#define _VPSHAL_SDVENC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/hal/vpshal_onChipEncOps.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief SDVENC on the high quality display path */
#define VPSHAL_SDVENC_INST_0                    (0u)
/** \brief SDVENC maximum number of instances */
#define VPSHAL_SDVENC_MAX_INST                  (1u)

/**
 *  \brief Maximum number of on-chip encoders per venc
 *
 * Maximum number of on-chip encoders that would be supported for a venc.
 * For e.g. if venc 1 supports 4 on-chip encoders and venc 2 supports 1 on-chip
 * encoder, then set this macro to 4.
 */
#define VPSHAL_SDVENC_MAX_NUM_OF_ONCHIP_ENC     (1u)

/** \brief Number of coefficients for the Low Pass filter */
#define VPSHAL_SDVENC_LPF_COEFFS                (6u)

/** \brief Number of coefficients for the 2x upsampling */
#define VPSHAL_SDVENC_2X_COEFFS                 (7u)

/** \brief Number of color space conversion coefficients */
#define VPSHAL_SDVENC_CSC_NUM_COEFF             (3u)
/** \brief Number of coefficients for the Dac 2x Oversampling */
#define VPSHAL_SDVENC_DAC2X_COEFFS              (7u)

/**
 * \brief Enum for frame mode
 */
typedef enum
{
    VPSHAL_SDVENC_FM_INTERLACED = 0,
    /**< Interlaced Frame Mode */
    VPSHAL_SDVENC_FM_PROGRESSIVE
    /**< Progressive Frame Mode */
} VpsHal_SdVencFrameMode;

/**
 * \brief Enum for the SDVENC Mode */
typedef enum
{
    VPSHAL_SDVENC_MODE_480I60 = 0,
    VPSHAL_SDVENC_MODE_480P60,
    VPSHAL_SDVENC_MODE_1080I60,
    VPSHAL_SDVENC_MODE_720P60,
    VPSHAL_SDVENC_MODE_576i60,
    VPSHAL_SDVENC_MODE_576P60,
    VPSHAL_SDVENC_MODE_1080P60,
    VPSHAL_SDVENC_MODE_NTSC_M,
    VPSHAL_SDVENC_MODE_PAL_BDGHI,
    VPSHAL_SDVENC_MODE_MANUAL,
    VPSHAL_SDVENC_MODE_INVALID
}VpsHal_SdVencMode;
typedef enum
{
    VPSHAL_SDVENC_FMT_525I = 0,
    /**< 525I picture format */
    VPSHAL_SDVENC_FMT_625I,
    /**< 625I picture format */
    VPSHAL_SDVENC_FMT_525P,
    /**< 525P picture format */
    VPSHAL_SDVENC_FMT_625P,
    /**< 625P picture format */
    VPSHAL_SDVENC_FMT_1080I,
    /**< 1080I picture format */
    VPSHAL_SDVENC_FMT_720P,
    /**< 720P picture format */
    VPSHAL_SDVENC_FMT_1080P,
    /**< 1080P picture format */
    VPSHAL_SDVENC_FMT_MAX
    /**< Maximum number of formats supported */
}VpsHal_SdVencFmt;

/** \brief SDVenc Color modulation type */
typedef enum
{
    VPSHAL_SDVENC_COL_MODULATION_MODE_NTSC = 0,
    /**< color modulation mode NTSC */
    VPSHAL_SDVENC_COL_MODULATION_MODE_PAL,
    /**< color modulation mode PAL */
    VPSHAL_SDVENC_COL_MODULATION_MODE_SECAM,
    /**< color modulation mode SECAM */
    VPSHAL_SDVENC_COL_MODULATION_MODE_MAX
    /**< Maximum number of color modulation supported */
}VpsHal_SdVencColorModulation;

/** \brief SDVenc closed caption type */
typedef enum
{
    VPSHAL_SDVENC_CC_NO_DATA = 0,
    /**< Closed caption on none of the fields */
    VPSHAL_SDVENC_CC_ODD_FIELD,
    /**< Closed caption on odd field */
    VPSHAL_SDVENC_CC_EVEN_FIELD,
    /**< Closed caption on even field */
    VPSHAL_SDVENC_CC_BOTH_FIELDS,
    /**< Closed caption on both fields */
    VPSHAL_SDVENC_CC_MAX
    /**< Maximum number of closed captioning supported */
}VpsHal_SdVencClosedCaptionSelection;

/** \brief DAC Selection for various outputs */
typedef enum
{
    VPSHAL_SDVENC_DAC_SEL_CVBS = 0,
    /**< CVBS output on DAC */
    VPSHAL_SDVENC_DAC_SEL_SVIDEO_Y,
    /**< Y component output from the SVideo on DAC */
    VPSHAL_SDVENC_DAC_SEL_SVIDEO_C,
    /**< C component output from the SVideo on DAC */
    VPSHAL_SDVENC_DAC_SEL_COM_YG,
    /**< Y/G component output from the Component on DAC */
    VPSHAL_SDVENC_DAC_SEL_COM_UB,
    /**< U/B component output from the Component on DAC */
    VPSHAL_SDVENC_DAC_SEL_COM_VR,
    /**< V/R component output from the Component on DAC */
    VPSHAL_SDVENC_DAC_SEL_MAX
    /**< Maximum number of DAC selection supported */
}VpsHal_SdVencDacSelection;

/** \brief Lines per field, equal lines per field or un-equal lines per field */
typedef enum
{
    VPSHAL_SDVENC_EQUAL_LINES = 0,
    /**< Equal lines per field */
    VPSHAL_SDVENC_UNEQUAL_LINES
    /**< Unequal lines per field */
}VpsHal_SdVencLinePerField;

/** \brief FID detection method */
typedef enum
{
    VPSHAL_SDVENC_FD_DET_VSYNC_RISE = 0u,
    /**< latch external field at external vsync rise edge. */
    VPSHAL_SDVENC_FD_DET_EXT_FIELD,
    /**< use raw external field */
    VPSHAL_SDVENC_FD_DET_VSYNC_FID,
    /**< use external vsync as field ID */
    VPSHAL_SDVENC_FD_DET_VSYNC_PHASE
    /**< detect external vsync phase */
}VpsHal_SdVencFieldDetectMethod;

/** \brief Signal Polarity */
typedef enum
{
    VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH = 0,
    /**< Polarity is active high */
    VPSHAL_SDVENC_POLARITY_ACTIVE_LOW
    /**< Polarity is active low */
}VpsHal_SdVencPolarity;

/** \brief Vertical Sync width */
typedef enum
{
    VPSHAL_SDVENC_VERT_SYNC_WID_30 = 0,
    /** Vertical sync width is 3 lines */
    VPSHAL_SDVENC_VERT_SYNC_WID_25
    /**< Vertical sync width is 2.5 lines */
}VpsHal_SdVencVertSyncWidth;

/** \brief  */
typedef enum
{
    VPSHAL_SDVENC_PIX_CLK_1X = 0,
    VPSHAL_SDVENC_PIX_CLK_2X
}VpsHal_SdVencPixelClkMul;

/** \brief Picture to Sync ratio */
typedef enum
{
    VPSHAL_SDVENC_PIC_SYNC_RAT_10_4 = 0u,
    /**< Picture to sync ratio is 10:4 */
    VPSHAL_SDVENC_PIC_SYNC_RAT_7_3
    /**< Picture to sync ratio is 7:3 */
}VpsHal_SdVencPicToSyncRatio;

/** \brief Output data format */
typedef enum
{
    VPSHAL_SDVENC_OUTPUT_RGB = 0u,
    /**< Otuput data format is RGB */
    VPSHAL_SDVENC_OUTPUT_YUV
    /**< Otutput data foramt is YUV */
}VpsHal_OuputDataType;

/** \brief  Wide screen signaling mode */
typedef enum
{
    VPSHAL_SDVENC_WSS_MODE0 = 0,
    /**< Wide screen signaling mdoe 0 for 525I, 525P, 1080i and 720P formats */
    VPSHAL_SDVENC_WSS_MODE1
    /**< Wide screen signaling mdoe 0 for 625I, 625P formats */
}VpsHal_SdVencWssMode;

/** \brief DAC Mode */
typedef enum
{
    VPSHAL_SDVENC_DC_MODE_NORMAL = 0,
    VPSHAL_SDVENC_DC_MODE_DC
}VpsHal_SdVencDacOutMode;
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_SdVencInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 *
 *  instId      - Instance Identifier
 *  baseAddress - Physical register start address for this instance
 */
typedef struct
{
    UInt32              instId;
    /**< SDVENC Instance */

    UInt32              baseAddress;
    /**< Base address of the instance */

    UInt32              pairedEncCnt;
    /**< Number of encoder that are paired with this instance of venc */

    UInt32              encIds[VPSHAL_SDVENC_MAX_NUM_OF_ONCHIP_ENC];
    /**< Paired encoders Identifier */
} VpsHal_SdVencInstParams;

/**
 *  vpsHal_SdVencDtvConfig
 *  \brief Structure for configuring the DTV parameters
 *
 */
typedef struct
{
    VpsHal_SdVencPolarity                    hsPolarity;
    /**< DTV_HS_POL: Horizontal sync polarity */
    VpsHal_SdVencPolarity                    vsPolarity;
    /**< DTV_VS_POL: Vertical sync polarity */
    VpsHal_SdVencPolarity                    fidPolarity;
    /** DTV_FID_POL: Field id line polarity */
    VpsHal_SdVencPolarity                    activVidPolarity;
    /** DTV_AVID_POL: Active video id polarity */
    UInt32                  hSyncStartPixel;
    /**< DTV_HS_H_STA: Horizontal sync start pixel */
    UInt32                  hSyncStopPixel;
    /**< DTV_HS_H_STP: Horizontal sync stop pixel */
    UInt32                  vSyncStartPixel;
    /**< DTV_VS_H_STA: Vsync start pixel */
    UInt32                  vSyncStopPixel;
    /**< DTV_VS_H_STP: Vsync stop pixel */
    UInt32                  vSyncStartLine;
    /**< DTV_VS_V_STA: Vysnc output start line */
    UInt32                  vSyncStopLine;
    /**< DTV_VS_V_STP: Vysnc output stop line */
    UInt32                  fidTogglePixelPosition;
    /**< DTV_FID_H_STA: Fid toggle pixel position */
    UInt32                  fidStartLine0;
    /**< DTV_FID_V_STA0: Start line  for field 0 line */
    UInt32                  fidId0;
    /**< DTV_FID_F_STA0: FID for the field 0 */
    UInt32                  fidStartLine1;
    /**< DTV_FID_V_STA1: Start line for the field1 line */
    UInt32                  fidId1;
    /**< DTV_FID_F_STA1: FID for the field 1 */
    UInt32                  avidStartPixel;
    /**< DTV_AVID_H_STA: Active video line start pixel */
    UInt32                  avidStopPixel;
    /**< DTV_AVID_H_STP: Active video line stop pixel */
    UInt32                  avidStartLine0;
    /**< DTV_AVID_V_STA0: Start line for the active video pulse for field 0 */
    UInt32                  avidStopLine0;
    /**< DTV_AVID_V_STA0: Stop line for the active video pulse for field 0 */
    UInt32                  avidStartLine1;
    /**< DTV_AVID_V_STA1: Start line for the active video pulse for field 1 */
    UInt32                  avidStopLine1;
    /**< DTV_AVID_V_STA1: Stop line for the active video pulse for field 1 */
}vpsHal_SdVencDtvConfig;

/**
 *  vpsHal_SdVencDvoConfig
 *  \brief Structure for configuring the DVO parameters
 *
 */
typedef struct
{
    VpsHal_SdVencPolarity   hsPolarity;
    /**< DVO_HS_POL: Horizontal sync polarity */
    VpsHal_SdVencPolarity   vsPolarity;
    /**< DVO_VS_POL: Vertical sync polarity */
    VpsHal_SdVencPolarity   fidPolarity;
    /** DVO_FID_POL: Field id line polarity */
    VpsHal_SdVencPolarity   activVidPolarity;
    /** DVO_AVID_POL: Active video id polarity */
    UInt32                  hSyncStartPixel;
    /**< DVO_HS_H_STA: Horizontal sync start pixel */
    UInt32                  hSyncStopPixel;
    /**< DVO_HS_H_STP: Horizontal sync stop pixel */
    UInt32                  vSyncStartPixel;
    /**< DVO_VS_H_STA: Vsync start pixel */
    UInt32                  vSyncStopPixel;
    /**< DVO_VS_H_STP: Vsync stop pixel */
    UInt32                  vSyncStartLine;
    /**< DVO_VS_V_STA: Vysnc output start line */
    UInt32                  vSyncStopLine;
    /**< DVO_VS_V_STP: Vysnc output stop line */
    UInt32                  fidTogglePixelPosition;
    /**< DVO_FID_H_STA: Fid toggle pixel position */
    UInt32                  fidStartLine0;
    /**< DVO_FID_V_STA0: Start line  for field 0 line */
    UInt32                  fidId0;
    /**< DVO_FID_F_STA0: FID for the field 0 */
    UInt32                  fidStartLine1;
    /**< DVO_FID_V_STA1: Start line for the field1 line */
    UInt32                  fidId1;
    /**< DVO_FID_F_STA1: FID for the field 1 */
    UInt32                  avidStartPixel;
    /**< DVO_AVID_H_STA: Active video line start pixel */
    UInt32                  avidStopPixel;
    /**< DVO_AVID_H_STP: Active video line stop pixel */
    UInt32                  avidStartLine0;
    /**< DVO_AVID_V_STA0: Start line for the active video pulse for field 0 */
    UInt32                  avidStopLine0;
    /**< DVO_AVID_V_STA0: Stop line for the active video pulse for field 0 */
    UInt32                  avidStartLine1;
    /**< DVO_AVID_V_STA1: Start line for the active video pulse for field 1 */
    UInt32                  avidStopLine1;
    /**< DVO_AVID_V_STA1: Stop line for the active video pulse for field 1 */
    UInt32                  horzDelay;
    /**< DVO_DELAY: DVO signal horizontal delay. Maximum allowed number is
    HITV/2 for interlaced and HITV for progressive. */
}vpsHal_SdVencDvoConfig;

/**
 *  VpsHal_SdVencTvDetConfig
 *  \brief Structure for configuring the TV Detection parameters
 *
 */
typedef struct
{
    VpsHal_SdVencPolarity   tvDetePolarity;
    /**< TVDETGP_POL: Tv detection signal polarity */
    UInt32                  tvDeteStartPixel;
    /**< TVDETGP_H_STA: Tv detection signal start pixel position */
    UInt32                  tvDeteStopPixel;
    /**< TVDETGP_H_STP: Tv detection signal stop pixel position */
    UInt32                  tvDeteStartLine;
    /**< TVDETGP_V_STA: Tv detection signal start line */
    UInt32                  teDeteStopLine;
    /**< TVDETGP_V_STP: Tv detection signal stop line */

}VpsHal_SdVencTvDetConfig;

/**
 *  vpsHal_SdVencSalveConfig
 *  \brief Structure for configuring the slave parameters
 *
 */
typedef struct
{
    VpsHal_SdVencPolarity                    hsPolarity;
    /**< HIP: 0=Active high, 1=Active Low */
    VpsHal_SdVencPolarity                    vsPolarity;
    /**< VIP: 0=Active high, 1=Active Low */
    VpsHal_SdVencPolarity                    fidPolarity;
    /**< FIP: 0=Active high, 1=Active Low */
    UInt32                   fieldDetectionMode;
    /**<  FMD: Field id detection method
    0: latch external field at external vsync rise edge.
    1: use raw external field
    2: use external vsync as field ID
    3: detect external vsync phase */

}vpsHal_SdVencSalveConfig;

/**
 *  vpsHal_SdVencStatus
 *  \brief SDVenc status parameters
 *
 */
typedef struct
{
    UInt32                  closedCaption0;
    /**< CAOST: Closed caption status (odd field). This bit shows 0
    when caption data register (L21DO) is ready to be input..
    and changes to 1 when data is written to L21DO. This bit is
    automatically cleared to 0 when a caption data transmission
    is completed on the line 21 in odd field.*/
    UInt32                  closedCaption1;
    /**< CAOST: Closed caption status (even field). This bit shows 0
    when caption data register (L21DE) is ready to be input..
    and changes to 1 when data is written to L21DE. This bit is
    automatically cleared to 0 when a caption data transmission
    is completed on the line 284 in even field.*/
    UInt32                  fidMonitor;
    /** FIDST: Field id monitor */

}vpsHal_SdVencStatus;

/**
 *  vpsHal_SdVencEtmgConfig
 *  \brief Encoder Timing parameters
 *
 */
typedef struct
{
    UInt32                  activeVidHorzStartPos;
    /**< AV_H_STA: Active video horizontal start position. */
    UInt32                  activeVidHorzStopPos;
    /**< AV_H_STP: Active video horizontal stop position. */
    UInt32                  activeVidVertStarPos0;
    /**< AV_V_STA0: Active video vertical start position for fid=0. */
    UInt32                  activeVidVertStopPos0;
    /**< AV_V_STP0: Active video vertical stop position for fid=0. */
    UInt32                  activeVidVertStarPos1;
    /**< AV_V_STA1: Active video vertical start position for fid=1. */
    UInt32                  activeVidVertStopPos1;
    /**< AV_V_STP1: Active video vertical stop position for fid=1. */
    UInt32                  colorBurstStartPos;
    /**< BST_H_STA: Color burst start position. */
    UInt32                  colorBurstStopPos;
    /**< BST_H_STP: Color burst stop position. */
    UInt32                  vbiReqStartPos;
    /**< VBI_H_STA: VBI request start position. */
    UInt32                  vbiReqStopPos;
    /**< VBI_H_STP: VBI request stop position. */
}vpsHal_SdVencEtmgConfig;

/**
 *  vpsHal_SdVencCvbsConfig
 *  \brief Composite video signal parameters.
 */
typedef struct
{
    UInt32                  synctipAmp;
    /**< CTLVL: CVBS sync-tip amplitude.*/
    UInt32                  syncAmp;
    /**< CSLVL: CVBS sync amplitude. */
    VpsHal_SdVencPicToSyncRatio pictSynRatio;
    /**< CPSR: CVBS picture sync ratio
               0: 10:4
               1: 7:3 */
    UInt32                  lumaLpfEnable;
    /**< YLPF: CVBS luma LPF enable */
    UInt32                  chromaLpfEnable;
    /**< CLPF: CVBS chroma LPF enable */
    VpsHal_SdVencColorModulation colorModulation;
    /**< CCM: CVBS color modulation mode */
    UInt32                  yDelayAdjustment;
    /**< CYDLY: CVBS Y delay adjustment. s3.0 */
    UInt32                  burstAmplitude;
    /**< CBLVL: CVBS burst amplitude.*/
    Int32                  yClipUpperLimit;
    /**< CYUCLP: CVBS Y Upper Limit. s12.0 */
    Int32                  yClipLowerLimit;
    /**< CYLCLP: CVBS Y Lower Limit. s12.0 */
    Int32                  uClipUpperLimit;
    /**< CUUCLP: CVBS U Upper Limit. s12.0 */
    Int32                  uClipLowerLimit;
    /**< CULCLP: CVBS U Lower Limit. s12.0 */
    Int32                  vClipUpperLimit;
    /**< CVUCLP: CVBS V Upper Limit. s12.0 */
    Int32                  vClipLowerLimit;
    /**< CVLCLP: CVBS V Lower Limit. s12.0 */
}vpsHal_SdVencCvbsConfig;

/**
 *  vpsHal_SdVencComponentConfig
 *  \brief Component video signal parameters.
 */
typedef struct
{
    UInt32                  synctipAmp;
    /**< MTLVL: CVBS sync-tip amplitude.*/
    UInt32                  syncAmp;
    /**< MSLVL: CVBS sync amplitude. */
    VpsHal_SdVencPicToSyncRatio  pictSynRatio;
    /**< MPSR: CVBS picture sync ratio
               0: 10:4
               1: 7:3 */
    VpsHal_OuputDataType    outColorSpace;
    /**< MRGB: RGB select for component output.
                0: YPbPr
                1: RGB */
    UInt32                  syncOnYOrG;
    /**< MSOYG: Sync on Y or G */
    UInt32                  syncOnPbOrB;
    /**< MSOUB: Sync on Pb or Pr */
    UInt32                  syncOnPrOrR;
    /**< MSOVR: Sync on Pr or R */
    Int32                  yGUpperLimit;
    /**< MYGUCLP: Component Y/G upper limit */
    Int32                  yGLowerLimit;
    /**< MYGUCLP: Component Y/G Lower limit */
    Int32                  uBUpperLimit;
    /**< MYGUCLP: Component U/B upper limit */
    Int32                  uBLowerLimit;
    /**< MYGUCLP: Component U/B Lower limit */
    Int32                  vRUpperLimit;
    /**< MYGUCLP: Component V/R upper limit */
    Int32                  vRLowerLimit;
    /**< MYGUCLP: Component V/R Lower limit */
}vpsHal_SdVencComponentConfig;

/**
 *  VpsHal_SdVencCscCoeff
 *  \brief Color space conversion coefficients.
 */
typedef struct
{
    Int16  mulCoeff[VPSHAL_SDVENC_CSC_NUM_COEFF][VPSHAL_SDVENC_CSC_NUM_COEFF];
     /**< Multiplication coefficients in the format A0, B0, C0 in the first row,
         A1, B1, C1 in the second row and A2, B2, C2 in the third row. */
    Int16 inOff[VPSHAL_SDVENC_CSC_NUM_COEFF];
    /**< Input offsets for the Coefficients. */
    Int16  outOff[VPSHAL_SDVENC_CSC_NUM_COEFF];
    /**< Ouput offsets for the coefficients*/
}VpsHal_SdVencCscCoeff;

/**
 *  VpsHal_SdVencDacConfig
 *  \brief Dac Selection for different outputs.
 */
typedef struct
{
    VpsHal_SdVencDacSelection dac0;
    /**< DA0S: Dac0 output */
    VpsHal_SdVencDacSelection dac1;
    /**< DA1S: Dac1 output */
    VpsHal_SdVencDacSelection dac2;
    /**< DA2S: Dac2 output */
    VpsHal_SdVencDacSelection dac3;
    /**< DA3S: Dac3 output */
    UInt32                    enableDac0;
    /**< DA0E: Enable DAC0 */
    UInt32                    enableDac1;
    /**< DA1E: Enable DAC1 */
    UInt32                    enableDac2;
    /**< DA2E: Enable DAC2 */
    UInt32                    enableDac3;
    /**< DA2E: Enable DAC3 */
    UInt32                    dclevelControl;
    /**< DALVL: DAC DC level control */
    VpsHal_SdVencDacOutMode   dcOutputMode;
    /**< DADC: DAC DC output mode. Setting 1 converts the value written in the
         DALVL register to DAC and directly outputs from DAOUT.
            0: Normal
            1: DC output mode */
    UInt32                    invert;
    /**< DAIV: DAC output invert mode. Setting 1 inverts the DAC output code.
         0: Non-Inverse
         1: Inverse */
}VpsHal_SdVencDacConfig;

/**
 *  VpsHal_SdVencLpfCoeff
 *  \brief Low pass filter coefficients
 */
typedef struct
{
    Int16 coeffs[VPSHAL_SDVENC_LPF_COEFFS];
    /**< Low pass filter coefficients */
}VpsHal_SdVencLpfCoeff;

/**
 *  VpsHal_SdVenc2xUpSampCoeff
 *  \brief 2X upsampling coefficients
 */
typedef struct
{
    Int16 coeffs[VPSHAL_SDVENC_2X_COEFFS];
    /**< 2X upsampling coefficients */
}VpsHal_SdVenc2xUpSampCoeff;

/**
 *  VpsHal_SdVencDac2xOverSampCoeff
 *  \brief 2X DAC oversampling coefficients
 */
typedef struct
{
    Int16 coeffs[VPSHAL_SDVENC_DAC2X_COEFFS];
    /**< X DAC oversampling coefficients  */
}VpsHal_SdVencDac2xOverSampCoeff;

/**
 *  VpsHal_SdVencClosedCapConfig
 *  \brief Closed caption parameters
 */
typedef struct
{
    VpsHal_SdVencClosedCaptionSelection ccSelection;
    /**< L21EN: Closed caption enable/disable on the individual fields; */
    UInt32                              ccDefaultData;
    /**<L21DF: Closed caption default data register. When the caption data
                register (L21DO or L21DE) is not updated before the caption data
                transmission timing for the corresponding field, the ASCII code
                specified by this register is automatically transmitted for
                closed caption data. */
    UInt32                              ccData0[2];
    /**< L21DO1, L21DO0:
            Closed caption data1 (odd field). Specify the ASCII code of the 2nd
            byte to be transmitted in closed captioning for odd field.
            Closed caption data0 (odd field). Specify the ASCII code of the 1st
            byte to be transmitted in closed captioning for odd field. */
    UInt32                              ccData1[2];
    /**< L21DE1, L21DE0:
            Closed caption data1 (even field). Specify the ASCII code of the 2nd
            byte to be transmitted in closed captioning for odd field.
            Closed caption data0 (even field). Specify the ASCII code of the 1st
            byte to be transmitted in closed captioning for odd field. */
}VpsHal_SdVencClosedCapConfig;

/**
 *  VpsHal_SdVencWssConfig
 *  \brief Wide screen signaling parameters
 */
typedef struct
{
    UInt32                          word0;
    /**< WSS_DATA: (525i/525p/1080i/720p) word0 */
    UInt32                          word1;
    /**< (525i/525p/1080i/720p) word1 */
    UInt32                          word2;
    /**< (525i/525p/1080i/720p) word2 */
    UInt32                          crc;
    /**< (525i/525p/1080i/720p) crc */
    UInt32                          group1;
    /**< (625i/625p) group1*/
    UInt32                          group2;
    /**< (625i/625p) group2 */
    UInt32                          group3;
    /**< (625i/625p) group3*/
    UInt32                          group4;
    /**< (625i/625p) group4 */
    UInt32                          enable;
    /**<  WSS_EN: Enable WSS enable */
    VpsHal_SdVencWssMode            wssMode;
    /**< WSS Mode for 525i/525p/1080i/720p or
         625i/625p */
}VpsHal_SdVencWssConfig;

/**
 *  VpsHal_SdVencCgmsConfig
 *  \brief CGMS parameters
 */
typedef struct
{
    UInt32                          typeBPacketHeader;
    /**< VIDBH: Type B packet header. Specifies h0-h5. */
    UInt32                          enable;
    /**< VIDB_EN:  Enable the CGMS service */
    UInt32                          typeBPayLoad0;
    /**< VIDBP0: Type B packet payload data. Specifies p0-p31. */
    UInt32                          typeBPayLoad1;
    /**< VIDBP1: Type B packet payload data. Specifies p32-p63. */
    UInt32                          typeBPayLoad2;
    /**< VIDBP2: Type B packet payload data. Specifies p64-p95. */
    UInt32                          typeBPayLoad3;
    /**< VIDBP3: Type B packet payload data. Specifies p96-p127. */
}VpsHal_SdVencCgmsConfig;

/**
 *  VpsHal_SdVencConfig
 *  \brief SDVenc configuration parameters including whole SDVenc
 *  congfiguration.
 */
typedef struct
{
    UInt32                  sdVencMode;
    /**< Default mode to be set by HAL */
    UInt32                  enable;
    /**< VIEN: Enable the venc */
    Vps_ScanFormat          scanFormat;
    /**< ITLC: Scan Format */
    UInt32                  FieldLineSym;
    /**< UEL: Lines in both field are same or un-equal */
    UInt32                  TestColorBar;
    /**< CBAR:Normal output or the Color bar test pattern */
    UInt32                  inputDataInv;
    /**< DIIV:Input data inversion */
    UInt32                  slave;
    /**< SLV: Encoder in slave mode */
    UInt32                  totalPixels;
    /**< HITV:Total Number of  */
    UInt32                  totalLines;
    /**< VITV: Total lines in frame */
    UInt32                  irqOutputStartPixel;
    /**< IRQ_H_STA:Pixel at which the irq signal will be generated */
    UInt32                  irqOutputStartLine;
    /**< IRQ_H_STP: Line at which the irq signal will be generated */
    UInt32                  gammaCorrEnable;
    /**< Gamma correction OFF or ON */
    UInt32                  Upsampling2XEnable;
    /**< 2x upsampling ON or OFF */
    UInt32                  Dac2xOverSamplingEnable;
    /**< DAC 2x oversampling ON or OFF */
    VpsHal_SdVencVertSyncWidth        vertSyncWidth;
    /**< SD Venc Vertical  sync widht*/
    UInt32                  blankingShapeEnable;
    /**< Disable blanking shape feature */
    UInt32                  vbiEnable;
    /**< VBI service ON or OFF */
    VpsHal_SdVencPixelClkMul pixelRateMultiplier;
    /**< Set 0 when the pixel rate is half of the clk2x.
        It is used to determine the internal pipeline delay alignment.*/
    VpsHal_SdVencFmt        fmt;
    /**< Set the SD Venc format */
    vpsHal_SdVencDtvConfig  dtvConfig;
    /**< SDVenc internal configuration */
    vpsHal_SdVencDvoConfig  dvoConfig;
    /**< SDVenc digital config */
    VpsHal_SdVencTvDetConfig      tvDetectConfig;
    /**< SDVenc Tv detection config */
    vpsHal_SdVencSalveConfig slaveConfig;
    /**< SDVenc slave config */
    vpsHal_SdVencEtmgConfig encoderTiming;
    /**< Encoder timing manager setting;*/
    vpsHal_SdVencCvbsConfig cvbsConfig;
    /**< SDVenc Composite signal config */
    vpsHal_SdVencComponentConfig    componentConfig;
    /**< SDVenc component config */
    VpsHal_SdVencDacConfig  dacConfig;
    /**< SDVenc DAC configuration */
    VpsHal_SdVencClosedCapConfig    ccConfig;
    /**< SDVenc closed caption configuration */
    VpsHal_SdVencWssConfig      wssConfig;
    /**< Wide screen signaling configuration */
    VpsHal_SdVencCgmsConfig     cgmsConfig;
    /**< Copy Generation Management System configuration */
    VpsHal_SdVencCscCoeff   *cvbsCoeffs;
    /**< CSC Coefficients for the Composite output */
    VpsHal_SdVencCscCoeff   *componentCoeffs;
    /**< CSC Coefficients for the Composite output */
    VpsHal_SdVencLpfCoeff   *lumaLpfCoeffs;
    /**<Luma Low pass filter coefficients */
    VpsHal_SdVencLpfCoeff   *chromaLpfCoeffs;
    /**<Chroma Low pass filter coefficients */
    VpsHal_SdVenc2xUpSampCoeff *upSamplingCoeffs;
    /**< Coefficeint for the 2x upsampling enabled */
    VpsHal_SdVencDac2xOverSampCoeff *dacOverSamplingCoeffs;
    /**< Coefficients for the DAC 2x over sampling */
    UInt32                  subCarrierIniPhase;
    /**< SCSD: Sub-carrier initial phase value. The degree can be specified by
               SCSD/1024*360. */
    UInt32                  subCarrierFreqPar0;
    /**< SCP0: Sub-carrier frequency parameter 0.*/
    UInt32                  subCarrierFreqPar1;
    /**< SCP1: Sub-carrier frequency parameter 1. */
    UInt32                  subCarrierFreqPar2;
    /**< SCP2: Sub-carrier frequency parameter 2. */

}VpsHal_SdVencConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_sdvencInit
 *  \brief Initializes SDVENC objects, gets the register overlay offsets for
 *  SDVENC registers.
 *  This function should be called before calling any of SDVENC HAL API's.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              Not used currently. Meant for future purpose.
 */
Int VpsHal_sdvencInit(UInt32 numInstances,
                     const VpsHal_SdVencInstParams *initParams,
                     Ptr arg);

/**
 *  VpsHal_sdvencDeInit
 *  \brief Currently this function does not do anything.
 *
 *  \param arg       Not used currently. Meant for future purpose.
 */
Int VpsHal_sdvencDeInit(Ptr arg);


/**
 *  VpsHal_sdvencOpen
 *  \brief This function should be called prior to calling any of the SDVENC HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param chrusInst       Requested SDVENC instance
 */
VpsHal_Handle VpsHal_sdvencOpen(UInt32 chrusInst);

/**
 *  VpsHal_sdvencClose
 *  \brief This functions closes the chrus handle and after call to this
 *  function, chrus handle is not valid. It also decrements the reference
 *  counter.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencClose(VpsHal_Handle handle);

/**
 *  VpsHal_sdvencGetModeInfo
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int32 VpsHal_sdVencGetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode);

/**
 *  VpsHal_sdvencSetModeInfo
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int32 VpsHal_sdVencSetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode);


/**
 *  VpsHal_sdvencStartVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencStartVenc(VpsHal_Handle handle);

/**
 *  VpsHal_sdvencStopVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencStopVenc(VpsHal_Handle handle);

/**
 *  VpsHal_sdvencResetVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencResetVenc(VpsHal_Handle handle);

/**
  *  \brief This function controls the VENC and other paired encoders. For now
 *         there are no VENC specific control operations that could be performed
 *
 *         If paired encoder (such as RF transmitter) for this VENC exists,
 *         the paired encoders control function would be called.
 *
 *  \param VpsHal_Handle    Handle to the venc
 *  \param cmd              Control command
 *  \param cmdArgs          Arguments associated with the command
 *  \param cmdStatusArgs    Result after execution of the command
 */
Int32 VpsHal_sdvencIoctl(VpsHal_Handle handle,
                            UInt32          cmd,
                            Ptr             cmdArgs,
                            Ptr             cmdStatusArgs);


/**
 *  \brief Configures VENC with on-chip encoders operations
 *
 *         This function is expected to be used the on-chip encoder HAL function
 *         to let the venc know about the abilites of on-chip encoders
 *         capabilities.
 *         Its expected the on-chip encoder call this function after venc hal
 *         has been initialized. Typically in the open of the on-chip encoders
 *         hal.
 *
 *  \par  CAUTION Its expected that venc is initialized first and then on-chip
 *        encoders hals are initialized.
 *
 * \param handle    Handle to SD Venc HAL
 * \param encOps    Capabilities of the encoder
 */
Int32 VpsHal_sdvencInitEncOps(VpsHal_Handle handle,
                                Vps_OnChipEncoderOps *encOps);

Int32 VpsHal_sdvencSetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output);
Void VpsHal_sdvencGetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output);
#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPSHAL_SDVENC_H */


