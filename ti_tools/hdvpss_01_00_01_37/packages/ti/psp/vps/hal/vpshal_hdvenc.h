/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_hdvenc.h
 *
 * \brief VPS HDVENC HAL header file
 * This file exposes the HAL APIs of the VPS HDVENC.
 *
 */

#ifndef _VPSHAL_HDVENC_H
#define _VPSHAL_HDVENC_H

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
/** \brief Maximum number of the VBI lines supported */
#define VPSHAL_HDVENC_VBI_LINES         (2u)
/** \brief Num of coefficients in each set */
#define VPSHAL_HDVENC_CSC_NUM_COEFF     (3u)

/** \brief HDVENC for HDMI/DVO1 */
#define VPSHAL_HDVENC_HDMI              (0u)
/** \brief HDVENC for HDCOMP */
#define VPSHAL_HDVENC_HDCOMP            (1u)
/** \brief HDVENC for DVO2 */
#define VPSHAL_HDVENC_DVO2              (2u)
/** \brief HDVENC maximum number of instances */
#define VPSHAL_HDVENC_MAX_INST          (3u)

/**
 *  \brief Maximum number of on-chip encoders per venc
 *
 *      Maximum number of on-chip encoders that would be supported for a venc
 *      e.g. If venc 1 supports 4 on-chip encoders and venc 2 supports 1 on-chip
 *      encoder. Then set this macro to 4
 */
#define VPSHAL_HDVENC_MAX_NUM_OF_ONCHIP_ENC (1u)


/**
 * \brief Total number of gamma correction coefficients
 */
#define VPSHAL_HDVENC_NUM_GAMMA_COEFF   (1024u)
/**
 * \brief Total number of the video channels Y, Pb, Pr
 */
#define VPSHAL_HDVENC_MAX_CHANNELS      (3u)

/**
 * \brief Enum for frame mode
 */
typedef enum
{
    VPSHAL_HDVENC_FM_INTERLACED = 0,
    /**< Interlaced Frame Mode */
    VPSHAL_HDVENC_FM_PROGRESSIVE
    /**< Progressive Frame Mode */
} VpsHal_HdVencFrameMode;
#if 0
/**
 * \brief Enum for the HDVENC Mode */
typedef enum
{
    VPSHAL_HDVENC_MODE_480I60 = 0,
    VPSHAL_HDVENC_MODE_480P60,
    VPSHAL_HDVENC_MODE_1080I60,
    VPSHAL_HDVENC_MODE_720P60,
    VPSHAL_HDVENC_MODE_576i60,
    VPSHAL_HDVENC_MODE_576P60,
    VPSHAL_HDVENC_MODE_1080P60,
    VPSHAL_HDVENC_MODE_MANUAL,
    VPSHAL_HDVENC_MODE_INVALID
}VpsHal_HdVencMode;
#endif
/**
 * \brief Display Mode
 */
typedef enum
{
    VPSHAL_HDVENC_DMODE_480I = 0,
    /**< Display mode 480I */
    VPSHAL_HDVENC_DMODE_480P,
    /**< Display mode 480P */
    VPSHAL_HDVENC_DMODE_1080I,
    /**< Display mode 1080I */
    VPSHAL_HDVENC_DMODE_720P,
    /**< Display mode 720P */
    VPSHAL_HDVENC_DMODE_576I,
    /**< Display mode 576I */
    VPSHAL_HDVENC_DMODE_576P,
    /**< Display mode 576P */
    VPSHAL_HDVENC_DMODE_INVALID
    /**< Invalid display mode */
}VpsHal_HdVDispMode;



/**
 * \brief Color Space
 */
typedef enum
{
    VPSHAL_HDVENC_COLORSPACE_RGB = 0,
    /**< RGB color space */
    VPSHAL_HDVENC_COLORSPACE_YUV
    /**< YUV  color space */
}VpsHal_HdVencOutputColorSpace;

/**
 * \brief  Ouput module
 */
typedef enum
{
    VPSHAL_HDVENC_OUTPUTSRC_DATAMGR = 0,
    /**< Output the data from the data manager directly */
    VPSHAL_HDVENC_OUTPUTSRC_GAMMACRT,
    /**< Output the data from the gamma correction block */
    VPSHAL_HDVENC_OUTPUTSRC_COLORSPC
    /**< Output the data from the color space converter block */
}VpsHal_HdVencDataSource;

/**
 * \brief DVO Format
 */
typedef enum
{
    VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN = 0,
    /**< Ouput data format is 10 bit single channel with embedded sync */
    VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN,
    /**< Output data format is 10bit dual channel with embedded sync */
    VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC,
    /**< Output data format is 10 bit tri channel with embedded sync */
    VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC,
    /**< Ouptut data format is 10 bit triple channel with discrete sync */
    VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN_DISCSYNC
    /**< Output data format is 10 bit dual channel with discrete sync */
}VpsHal_HdVencDvoFmt;

/**
 * \brief Enum for the YUV444 to YUV422 conversion
 */
typedef enum
{
    VPSHAL_HDVENC_444TO422_DECIMATION = 0,
    /**< YUV444 to YUV422 done with the help of decimation filter */
    VPSHAL_HDVENC_444TO422_ALTERNATEPIXEL
    /**< YUV444 to Y422 done by dropping the alternate pixel of the CbCr. */
}VpsHal_HdVenc444to422;

/**
 * \brief VS and FID start position
 */
typedef enum
{
    VPSHAL_HDVENC_HFLINE_START_LINE_BEGINNING = 0,
    /**< VS and FID signal will start and end at beginning of the line */
    VPSHAL_HDVENC_HFLINE_START_LINE_MIDDLE
    /**< VS and FID signal will start and end at the middle of the line */
}VpsHal_HdVencHfLine;

/**
 * \brief MV blank level reduction
 */
typedef enum
{
    VPSHAL_HDVENC_MV_BL_REDUCTION_OFF = 0,
    /**< No blank level reduction */
    VPSHAL_HDVENC_MV_BL_REDUCTION_IN_NONE_VBI_PERIOD = 2,
    /**< reduction in non-vbi period */
    VPSHAL_HDVENC_MV_BL_REDUCTION_IN_NONE_VBI_AND_ACTIVE_PERIOD = 3
    /**< reduction in both non-vbi and active video period */
}VpsHal_HdVencMvBlankLevelReduction;

/**
 * \brief MV AGC pulse amplitude pattern
 */
typedef enum
{
    VPSHAL_HDVENC_AGC_PULSE_MAX_AMPLITUDE = 0,
    /**The AGC pulses remain at the maximum amplitude */
    VPSHAL_HDVENC_AGC_PULSE_CYCLIC_AMPLITUDE
    /**< The AGC pulses amplitude will be varied cyclicly */
}VpsHal_HdVencAgcAmpControl;
/**
 * \brief VBI start segment data shift
 */
typedef enum
{
    VPSHAL_HDVENC_MSB_START_SHIFTED_FIRST = 0,
    /**< the MSB of the Start-Segment string will be shifted out first */
    VPSHAL_HDVENC_MSB_START_SHIFTED_LAST
   /**< the LSB of the Start-Segment string will be shifted out first.*/
}VpsHal_HdVencVbiStartSegShift;
/**
 * \brief VBI Header segment data shift
 */
typedef enum
{
    VPSHAL_HDVENC_MSB_HEADER_SHIFTED_FIRST = 0,
    /**< the MSB of the Header-Segment string will be shifted out first. */
    VPSHAL_HDVENC_MSB_HEADER_SHIFTED_LAST
   /**< the LSB of the Header-Segment string will be shifted out first.*/
}VpsHal_HdVencVbiHeaderSegShift;
/**
 * \brief VBI Payload segment data shift
 */
typedef enum
{
    VPSHAL_HDVENC_MSB_PAYLOAD_SHIFTED_FIRST = 0,
    /**< The MSB of the Payload-Segment string will be shifted out first. */
    VPSHAL_HDVENC_MSB_PAYLOAD_SHIFTED_LAST
    /**< The MSB of the Payload-Segment string will be shifted out first. */
}VpsHal_HdVencVbiPayloadSegShift;

/**
 *  enum VpsHal_CscMode
 *  \brief Enum for specifying which sets of coefficientts to be used if user
 *  does not provide coefficients. If coefficients are provided by upper layer,
 *  enum is not used.
 */
typedef enum
{
    VPSHAL_HDVENC_CSCMODE_NONE = -1,
    /**< Used when coefficients are provided */
    VPSHAL_HDVENC_CSCMODE_SDTV_VIDEO_R2Y = 0,
    /**< Select coefficient for SDTV Video */
    VPSHAL_HDVENC_CSCMODE_SDTV_VIDEO_Y2R,
    /**< Select coefficient for SDTV Video */
    VPSHAL_HDVENC_CSCMODE_SDTV_GRAPHICS_R2Y,
    /**< Select coefficient for SDTV Graphics */
    VPSHAL_HDVENC_CSCMODE_SDTV_GRAPHICS_Y2R,
    /**< Select coefficient for SDTV Graphics */
    VPSHAL_HDVENC_CSCMODE_HDTV_VIDEO_R2Y,
    /**< Select coefficient for HDTV Video */
    VPSHAL_HDVENC_CSCMODE_HDTV_VIDEO_Y2R,
    /**< Select coefficient for HDTV Video */
    VPSHAL_HDVENC_CSCMODE_HDTV_GRAPHICS_R2Y,
    /**< Select coefficient for HDTV Graphics */
    VPSHAL_HDVENC_CSCMODE_HDTV_GRAPHICS_Y2R,
    /**< Select coefficient for HDTV Graphics */
    VPSHAL_HDVENC_CSCMODE_R2R,
    /**< CSC coefficients for RGB to RGB mode */
    VPSHAL_HDVENC_CSC_NUM_MODE
} VpsHal_HdVencCscMode;
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/**
 * struct VpsHal_HdVencCscCoeff
 * \brief Set the Coefficients for Color Space Conversion.
 */
typedef struct
{
    UInt16 coeffs[VPSHAL_HDVENC_MAX_CHANNELS][VPSHAL_HDVENC_NUM_GAMMA_COEFF];
    /**< Gamma correction coefficients */
}VpsHal_HdVencGammaCoeffs;

/**
 * struct VpsHal_HdVencAnalogConfig
 * \brief HD Venc Analog configuration structure
 */
typedef struct
{
    VpsHal_HdVencOutputColorSpace           outColorSpace;
    /**< Y_RGBn: Color space setting. This bit defines the color space used by
    the analog video output.
    It will be used to configure the scaling functions on different channels
    for different analog display color spaces. */
    UInt32                                  dac0PwDn;
    /**< PWD_0: DAC0 power down bit. */
    UInt32                                  dac1PwDn;
    /**< PWD_1: DAC1 power down bit. */
    UInt32                                  dac2PwDn;
    /**< PWD_2: DAC2 power down bit. */
    UInt32                                  dacInvertData;
    /**< INVT: When this bit is set, all the data to DAC0, DAC1,
    and DAC2 will be inverted */
    UInt32                                  dac0RfPdn;
    /**< DAC_RF0: This bit will turn on or off the internal voltage
    reference of DAC0/1/2.*/
    UInt32                                  dac1RfPdn;
    /**< DAC_RF1: This bit will turn on or off the internal voltage
    reference of DAC3/4/5.*/
    UInt16                                  scale0Coeff;
    /**< SCALE0: Scaling coefficient of scalier 0 */
    UInt16                                  shift0Coeff;
    /**< SHIFT0: Shifting coefficient of channel 0 */
    UInt16                                  blank0Level;
    /**< BL_0: Defines the blank level of first channel output.
    This level is in the 10-bit resolution domain.*/
    UInt16                                  scale1Coeff;
    /**< SCALE0: Scaling coefficient of scalier 1 */
    UInt16                                  shift1Coeff;
    /**< SHIFT0: Shifting coefficient of channel 1 */
    UInt16                                  blank1Level;
    /**< BL_0: Defines the blank level of second channel output.
    This level is in the 10-bit resolution domain.*/
    UInt16                                  scale2Coeff;
    /**< SCALE0: Scaling coefficient of scalier 2 */
    UInt16                                  shift2Coeff;
    /**< SHIFT0: Shifting coefficient of channel 2 */
    UInt16                                  blank2Level;
    /**< BL_0: Defines the blank level of third channel output.
    This level is in the 10-bit resolution domain*/
    UInt16                                  totalPixelsPerLine;
    /**< PIXELS: Defines the total number of pixels per line */
    UInt16                                  totalLinesPerFrame;
    /**< LINES: Define the total number of lines per frame*/
    UInt16                                  clampPeriod;
    /**< CLAMP: Define the clamp period of the sync pulse in 1080i format. */
    UInt16                                  vbiLineEnd0;
    /**< V_BLA1: Defines a line location in a frame where 1st vertical blank
    period ends.*/
    UInt16                                  vbiLineend1;
    /**< V_BLA2: Defines a line location in a frame where the 2nd vertical
    blank period ends. */
    UInt16                                  equPulseWidth;
    /**< EQ_WTH: Defines the width of pre-equalization and post-equalization
    pulse (active low) */
    UInt16                                  hbiPixelEnd;
    /**< H_BLANK: Defines the location (by pixel number) of the
    last non-active pixel on each line in front of the active video. */
    UInt16                                  numActivePixels;
    /**< ACT_PIX: Defines the number of active pixels in one video line.  */
    UInt16                                  hsWidth;
    /**< HS_WTH: Defines the width of HS (Horizontal Sync) pulse.
    It is also the serration pulse width (active high). */
    UInt16                                  lastActiveLineFirstField;
    /**< END_F1: Define the last active line of the first field.
    This parameter is used only in interlace display format. */
    UInt16                                  pbPrShifting;
    /**< CBCR_S2, CBCR_S1: Defines the shifting of active video of channel
    CB and CR. This shifting is only used when the output is
    in YPbPr color space.*/
    UInt16                                  VencEnableDelayCount;
    /**< DELAY_VENC, DELAY_VENC: The maximum delay from the VENC_EN’s
    activation is 4096 pixel clock periods.*/
    UInt16                                  syncHighLevelAmp;
    /**< SYNC_H: Defines the high level of the sync pulse of a tri-level
    sync system (720p and 1080i format). This is the level in 10-bit
    resolution domain.*/
    UInt16                                  syncLowLevelAmp;
    /**< SYNC_L: Defines the low level of the sync pulse of a tri-level
    sync system (720p and 1080i format). This is the level in 10-bit
    resolution domain.*/
}VpsHal_HdVencAnalogConfig;

/**
 * struct VpsHal_HdVencDvoConfig
 * \brief HD Venc Digtal configuration structure.
 */
typedef struct
{
    VpsHal_HdVencDataSource                 dataSource;
    /**< DVO_CS: These two bits select the video data source for DVO.*/
    VpsHal_HdVencDvoFmt                     fmt;
    /**< DVO_FMT: These three bits define the format of digital video
    output port.*/
    UInt32                                  invtfidPol;
    /**< I_DVO_F: This bit controls the polarity of DVO_FID signal. */
    UInt32                                  invtVsPol;
    /**< I_DVO_V: This bit controls the polarity of DVO_VS signal.*/
    UInt32                                  invtHsPol;
    /**< I_DVO_H: This bit controls the polarity of DVO_HS signal. */
    UInt32                                  invtActsPol;
    /**< I_DVO_A: This bit controls the polarity of DVO_ACTVID signal. */
    VpsHal_HdVenc444to422                   decimationMethod;
    /**< S_422: When the DVO output is in 4:2:2 YCbCr format, this bit
    controls the CbCr format.*/
    UInt32                                  dvoOff;
    /**< DVO_OFF: This bit can force the DVO to 0s */
    VpsHal_HdVencHfLine                     vsFidLineStart;
    /**< HF_LINE: This bit controls the starting point of DVO_VS and
    DVO_FID signals on a video line.*/
    UInt32                                  jeidaFmt;
    /**< JED: This bit can enable the JEIDA output format */
    UInt32                                  firstActivePixelLine;
    /**< DVO_AVST_H: In embedded sync format, this defines the SAV
    location on each line by pixel number. In this mode, normally a line
    starts at EAV, then SAV. Therefore the value in here should be:
    DVO_AVST_H = whole_line – active_line
    In discrete sync format, this defines the location of the
    first active pixel on each line. The pixel number starts as “0”.*/
    UInt32                                  activeVideoLineWidth;
    /**< DVO_AVD_HW: Defines the width of each active video line
    (in number of pixels).*/
    UInt32                                  hsWidth;
    /**< DVO_HS_WD: Defines the width of DVO_HS pulse (in number of pixels).*/
    UInt32                                  hsStartPixel;
    /**< DVO_HS_ST: Defines the starting location of the HS pulse on
    each line */
    UInt32                                  activeLineStart0;
    /**< DVO_AVST_V1: In progressive mode, it defines the first active line
    in a frame. In interlace mode, it defines the first active line of the
    first field in a frame */
    UInt32                                  activeLineStart1;
    /**< DVO_AVST_V2: Defines the first active line of second field in a
    frame.*/
    UInt32                                  activeVideoLines0;
    /**< DVO_AVD_VW1: In progressive mode, it defines the number of active
    video lines in a frame. In interlace mode, it defines the number of active
    lines in the first field.*/
    UInt32                                  activeVideoLines1;
    /**< DVO_AVD_VW2: Defines the number of active lines in the second field.*/
    UInt32                                  vsStartLine0;
    /**< DVO_VS_ST1: In progressive mode, it defines the starting location of
    the VS pulse in a frame. In interlace mode, it defines the starting
    location of VS pulse of the first field. */
    UInt32                                  vsStartLine1;
    /**< DVO_VS_ST2: Defines the starting location of the VS of the
    second field. */
    UInt32                                  vsWidth0;
    /**< DVO_VS_WD1: In progressive mode, it defines the width of the VS pulse
    In interlace mode, it defines the width of the VS pulse of the first
    field. */
    UInt32                                  vsWidth1;
    /**< DVO_VS_WD2: In interlace mode, it defines the width of the
    VS pulse of the second field. */
    UInt32                                  fidToggleLine0;
    /**< DVO_FID_ST1: In progressive mode, the signal “DVO_FID” will
    be toggled on each frame; this parameter defines the location where
    the “DVO_FID” will be toggled. In interlace mode, this parameter defines
    the starting location of the first field. */
    UInt32                                  fidToggleLine1;
    /**< DVO_FID_ST2: In interlace mode, this parameter defines
    the starting location of the second field. */
}VpsHal_HdVencDvoConfig;

/**
 * struct VpsHal_HdVencOsdConfig
 * \brief HD Venc OSD configuration structure
 */
typedef struct
{
    UInt32                                  invtfidPol;
    /**< IVT_FID:This bit controls the polarity of DTV_FID signal. */
    UInt16                                  HbiSignalStartPixel;
    /**< OSD_HBI_ST: Defines the starting location (in pixel number)
    of DTV_HBI signal. */
    UInt16                                  lastNonActivePixelInLine;
    /**< OSD_AVST_H:
    This defines the location of the last non-active pixel
    on each line; the next pixel will be the first active pixel on a line.
    For example, if this parameter is set as “n”; then the active video
    starts at pixel “n+1”.
    The relationship with DVO_AVST_H is:
    OSD_AVST_H =  DVO_AVST_H - 8
    Range is 0 = OSD_AVST_H < “pixels” (“pixels” is defined in CFG10). */
    UInt16                                  activeVideoLineWidth;
    /**< OSD_AVD_HW: Defines the width of each active video line
    (in number of pixels). */
    UInt16                                  hsWidth;
    /**< OSD_HS_WD: Defines the width of DTV_HS pulse. */
    UInt16                                  hsStartPixel;
    /**< OSD_HS_ST: Defines the starting location of the DTV_HS
    pulse on each line */
    UInt16                                  activeLineStart0;
    /**< OSD_AVST_V1: In progressive mode, it defines the first active line
    in a frame. In interlace mode, it defines the first active line of first
    field in a frame. (The line number starts at “0”). */
    UInt16                                  activeLineStart1;
    /**< OSD_AVST_V2: Defines the first active line of second field in a
    frame. This parameter is only used in interlace mode. */
    UInt16                                  activeVideoLines0;
    /**< OSD_AVD_VW1: Defines the number of active lines in a frame;
    In interlace mode, it defines the number of active lines in the first
    field. */
    UInt16                                  activeVideoLines1;
    /**< OSD_AVD_VW2: Defines the number of active lines in the second field.
    This parameter is only used in interlace mode. */
    UInt16                                  vsStartLine0;
    /**< OSD_VS_ST1: Defines the starting location of the DTV_VS.
    In interlace mode, it defines the starting location of the DTV_VS
    of first field. */
    UInt16                                  vsStartLine1;
    /**< OSD_VS_ST2: Defines the starting location of the VS of the second
    field. This parameter is only used in interlace mode.*/
    UInt16                                  vsWidth0;
    /**< OSD_VS_WD1: Defines the width of the first DTV_VS pulse
    (in number of lines) in a frame */
    UInt16                                  vsWidth1;
    /**< OSD_VS_WD2: Defines the width of the second DTV_VS pulse
    (in number of lines) For interlace mode only. */
    UInt32                                  fidToggleLine0;
    /**< OSD_FID_ST1: In progressive mode, the signal “DTV_FID” will
    be toggled on each frame; this parameter defines the location where the
    “DTV_FID” will be toggled. In interlace mode, this parameter defines
    the starting location of the first field. */
    UInt32                                  fidToggleLine1;
    /**< OSD_FID_ST2: In interlace mode, this parameter defines
    the starting location of the first field. */

}VpsHal_HdVencOsdConfig;

/**
 * struct VpsHal_HdVencMvConfig
 * \brief HD Venc Macrovision configuration structure.
 */
typedef struct
{
    UInt32                                  macroVisionEnable;
    /**< CMV: 480p/576p Component video Macro-Vision enable bit*/
    UInt16                                  peakingValue;
    /**< MV_PK_H3, MV_PK_L8:
    The peaking value of MacroVision AGC pulse is programmable by setting the
    MV_PK_H3 and MV_PK_L8 register.*/
    UInt16                                  backPorchPeakValue;
    /**< BP_PK_L, BP_PK_H: This register defines the peak value of BP
    (Back Porch) pulse when MacroVision is enabled.*/
    UInt16                                  mv100mvAgcPulseYSection;
    /**< P_100MV: This parameter is used by the MacroVision in 576i
    display format only. These 8-bits will be used as the 8 MSB to defined
    the 100mv AGC pulse in the Y section of Pulsation period. */
    UInt16                                  mv450mvAgcPulseXSection;
    /**< P_450MV: This parameter is used by the MacroVision in 576i
    display format only. These 8-bits will be used as the 8 MSB to defined
    the 450mv AGC pulse in the X section of Pulsation period.*/
    VpsHal_HdVencMvBlankLevelReduction      blankLevelReductionMethod;
    /**< N0_1_0: These two bits are the corresponding bit1
    and bit0 of N0 of MacroVision spec. They control if the
    blank level reduction is engaged: */
    VpsHal_HdVencAgcAmpControl              agcAmp;
    /**< N0_2: AGC pulse amplitude control: */
    UInt32                                  backPorchPulseControl;
    /**< N0_4: End of field back porch pulse control: */
    UInt32                                  mvEnable480i;
    /**< N0_5: 480i component MacroVision on/off control. */
    UInt16                                  syncPulseDurationFormatB;
    /**< N8_B: Pseudo-sync pulse duration control of format B */
    UInt16                                  syncPulseDurationFormatA;
    /**< N8_A: Pseudo-sync pulse duration control of format A */
    UInt16                                  syncPulseLocationFormatB;
    /**< N9_B: Location of 1st pseudo-sync pulse of format B */
    UInt16                                  syncPulseLocationFormatA;
    /**< N9_A: Location of 1st Pseudo-sync pulse of format A */
    UInt16                                  syncPulseSpacingFormatB;
    /**< N10_B: Pseudo-sync pulse spacing control of format B */
    UInt16                                  syncPulseSpacingFormatA;
    /**< N10_A: Pseudo-sync pulse spacing control of format A */
    UInt16                                  enablePseSyncPulseAndAgc;
    /**< N11: Pseudo-Sync pulse and AGC pulse line enabling
    0: Pseudo-sync and AGC pulse will be disabled on the current line
    1: Pseudo-sync and AGC pulse will be enabled on the current line
    N11[0] controls: line 7/270 of 525i;
                     line 7/319 of 625i;
    N11[1] controls: line 8/271 of 525i;
                     line 8/320 of 625i;
                        .
                        .
                        .
    N11[14] controls: line 21/284 of 525i;
                      line 21/333 of 625i; */
    UInt16                                  PseSyncPulseAndAgcFormat;
    /**< N12: Pseudo-Sync pulse and AGC pulse line format selection: */
    UInt16                                  enableFormatAPulsePair;
    /**< N13: Format A PS/AGC pulse pair enable in the current line:*/
    UInt16                                  enableFormatBPulsePair;
    /**< N14: Format B PS/AGC pulse pair enable in the current line: */
    UInt16                                  beforevSyncPulseInsertion;
    /**< N15_7_4: These are the high four bits of N15[7:4]. They controls the
    BP (Back Porch) pulse insertion before the vertical sync */
    UInt16                                  AftervSyncPulseInsertion;
    /**< N15_3_0: These are the high four bits of N15[7:4]. They controls
    the BP (Back Porch) pulse insertion after the vertical sync */
    UInt16                                  blankLevelReduction;
    /**< MV_BL_REDUC: Blank level reduction control bits.
    The value in this register will be x2 before it is used to
    subtract from Blank-level.
    In other works, if blank-level needs to be shifted down by K steps,
    the K/2 should be set in this register. */
}VpsHal_HdVencMvConfig;

/**
 * struct VpsHal_HdVencVbiConfig
 * \brief HD Venc VBI configuration structure */
typedef struct
{
    UInt16                          VbiLineLoc0[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_L1, VBI_L2: These 12 bit defines the line-location of the
    first VBI data line in a frame; for interlace mode,
    it refer to a line in the first field of a frame.*/
    UInt16                          VbiLineLoc1[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_L1_B, VBI_L2_B: These 12 bit defines the line-location of the
    VBI data line */
    UInt32                          enableVbiLine0[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_L1_EN, VBI_L2_EN: This bit will enable or disable VBI data line.*/
    UInt32                          enableVbiLine1[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_L1_B_EN, VBI_L2_B_EN: This bit will enable or disable the first
    VBI data line in second field. */
    VpsHal_HdVencVbiStartSegShift   startSegShift;
    /**< ST_MSB_1ST:
    1: the MSB of the Start-Segment string will be shifted out first.
    0: the LSB of the Start-Segment string will be shifted out first.
    Note: this bit will control both VBI line 1 and line 2 in both fields.*/
    VpsHal_HdVencVbiHeaderSegShift  headerSegShift;
    /**< HD_MSB_1ST:
    1: the MSB of the Header-Segment string will be shifted out first.
    0: the LSB of the Header-Segment string will be shifted out first.
    Note: this bit will control both VBI line 1 and line 2 in both fields.*/
    VpsHal_HdVencVbiPayloadSegShift pdsSegShift;
    /**< PD_MSB_1ST:
    1: the MSB of the Payload-Segment string will be shifted out first.
    0: the LSB of the Payload-Segment string will be shifted out first.
    Note: this bit will control both VBI line 1 and line 2 in both fields.*/
    UInt16                          VbiPixelLoc[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_L1_ST, VBI_L2_ST: These 12 bit defines the starting pixel-location
    of the first VBI data line. */
    UInt16                          startSymbolWidth[VPSHAL_HDVENC_VBI_LINES];
    /**< ST_WD_L1, ST_WD_L2: This register defines the total width of a start
    symbol (in a number of pixel-clock). For example, for type-A data packet
    of 480p format, the total width of a start-symbol is 26 pixel clocks,
    therefore, 0x1A should be in this register. The maximum width of a start
    symbol is 256 pixel clock wide */
    UInt16                          numSymbolsStartSeg[VPSHAL_HDVENC_VBI_LINES];
    /**< NUM_OF_ST_L1, NUM_OF_ST_L2: These 6-bits define the number of symbol
    in start-segment stream. */
    UInt16                          headerSymbolWidth[VPSHAL_HDVENC_VBI_LINES];
    /**< HD_WD_L1, HD_WD_L2: This register defines the width of a header symbol
    (in a number of pixel clock */
    UInt16                          numSymbolHeaderSeg[VPSHAL_HDVENC_VBI_LINES];
    /**< NUM_OF_HD_L1, NUM_OF_HD_L2: These 4-bits define the number of symbol
    in the header-segment stream. */
    UInt16                          payLoadSymbolWidth[VPSHAL_HDVENC_VBI_LINES];
    /**< PD_WD_L1, PD_WD_L2: This register defines the width of a payload
    symbol (in a number of pixel clock).*/
    UInt16                      numSymbolPayloadSeg[VPSHAL_HDVENC_VBI_LINES];
    /**< NUM_OF_PD_L1, NUM_OF_PD_L2: These 8-bits define the number of symbol
    in the payload-segment stream.*/
    UInt32                          startSegStream0[VPSHAL_HDVENC_VBI_LINES];
    /**< ST_STR0_L1, ST_STR0_L2: These 32-bits defines the start-segment
    stream of the first VBI line, each bit represents one symbol.
    If the start-segment has more than 32 symbol,
    ST_STR1_L1 will be used after this register. */
    UInt32                          startSegStream1[VPSHAL_HDVENC_VBI_LINES];
    /**< ST_STR1_L1, ST_STR1_L2: These 32-bits defines the start-segment
    stream of the first VBI data line. Whenever the start-segment has more than
    32 symbols, this register will be used.*/
    UInt32                          headerSegStream0[VPSHAL_HDVENC_VBI_LINES];
    /**< HD_STR0_L1, HD_STR0_L2: These 32-bits contains the header-segment
    stream of the first VBI line, each bit represents
    one symbol during transmission */
    UInt32                          headerSegStream1[VPSHAL_HDVENC_VBI_LINES];
    /**< HD_STR1_L1, HD_STR1_L2: These 32-bits contains the header-segment
    stream of the first VBI line, each bit represents one symbol during
    transmission. */
    UInt32                          payLoadSegStream0[VPSHAL_HDVENC_VBI_LINES];
    /**< PD_STR0_L1, PD_STR0_L1: These are the first 32 symbols
    in payload-segment of the first VBI line, each bit represents
    one symbol during transmission. Maximum number of symbol
    in payload-segment is 128 */
    UInt32                          payLoadSegStream1[VPSHAL_HDVENC_VBI_LINES];
    /**< PD_STR1_L1, PD_STR1_L2: These are the second 32 symbols in
    payload-segment of the first VBI line, each bit represents one symbol
    during transmission.*/
    UInt32                          payLoadSegStream2[VPSHAL_HDVENC_VBI_LINES];
    /**< PD_STR2_L1, PD_STR2_L2: These are the third 32 symbols in
    payload-segment of the first VBI line, each bit represents one
    symbol during transmission. Maximum number of symbol in
    payload-segment is 128*/
    UInt32                          payLoadSegStream3[VPSHAL_HDVENC_VBI_LINES];
    /**< These are the fourth 32 symbols in payload-segment of the first
    VBI line, each bit represents one symbol during transmission. */
    UInt32                          hstateAmplitude[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_HI_L1: These 8-bits defines the amplitude of the “H” state
    for all the symbols in first VBI data lines.*/
    UInt32                          lstateAmplitude[VPSHAL_HDVENC_VBI_LINES];
    /**< VBI_LO_L1: These 8-bits defines the amplitude of  the “L” state
    for all the symbols in first VBI data lines. */
    UInt32                          startPixelExtVbi;
    /**< EXT_VBI_ST: This register defines the start-pixel location of the
    external VBI data on a VBI line */
    UInt32                          endPixelExtVbi;
    /**< EXT_VBI_END: This register defines the end-pixel location of the
    external VBI data on a VBI line */
    UInt32                          enableExternalVbi;
    /**< VBI_EN: This bit will enable or disable the external VBI data service.
    When it is enabled, the external VBI data from the “EXT_VBI_DATA[7:0]” port
    will be converted to 12 bit data and output to DAC in analog format.*/
}VpsHal_HdVencVbiConfig;


/**
 * struct VpsHal_CscCoeff
 * \brief Set the Coefficients for Color Space Conversion.
 */
typedef struct
{
    UInt16 mulCoeff[VPSHAL_HDVENC_CSC_NUM_COEFF][VPSHAL_HDVENC_CSC_NUM_COEFF];
    /**< Multiplication coefficients in the format A0, B0, C0 in the first row,
         A1, B1, C1 in the second row and A2, B2, C2 in the third row. */
    UInt16 addCoeff[VPSHAL_HDVENC_CSC_NUM_COEFF];
    /**< Addition Coefficients. */
} VpsHal_HdVencCscCoeff;

/**
 *  VpsHal_HdVencConfig
 *  \brief Structure containing the configuration parameters for each venc
 *
 *  instId      - Instance Identifier
 *  baseAddress - Physical register start address for this instance
 */
typedef struct
{
    UInt32                                  hdVencMode;
    /**< Default mode to be set by the Hal */
    VpsHal_HdVDispMode                      dispMode;
    /**< DM_SEL: Display mode selection, 000/480i; 001/480p; 010/1080i; 011/720p
    100/576i; 101/576p */
    UInt32                                  scanFormat;
    /**< I_Pn: Scan format */
    UInt32                                  bypass2XUpSampling;
    /**< BYPS_2X: Bypass 2X upsampling. */
    UInt32                                  bypassCsc;
    /**< BYPS_CS: Bypass color space converter */
    UInt32                                  bypassGammaCor;
    /**< BYPS_GC: Bypass Gamma Correction block */
    UInt32                                  rdGammaCorMem;
    /**< RD_MEM: This bit will enable ARM to read the Gamma-correction
    memories. */
    UInt32                                  selfTest;
    /**< STEST: Self test mode */
    UInt32                                  startEncoder;
    /**< START: This bit will start the operation of encoder. */
    VpsHal_HdVencCscMode                    cscMode;
    /**< Color space conversion mode. Based on this Hal will configure the
    default CSC coefficients */
    VpsHal_HdVencCscCoeff                   *cscCoefficients;
    /**< Pointer to the coefficients.  If this is null Hal will program the
    default coefficients according to the conversion selected. */
    VpsHal_HdVencGammaCoeffs                *gammaCoeffs;
    /** Pointer to the gamma correction coefficients. If the pointer is
    NULL Hal will not program the gamma correction coeffs else the caller
    function has to pass the pointer to the valid coefficient memory.*/
    VpsHal_HdVencAnalogConfig               analogConfig;
    /**< Analog configurations */
    VpsHal_HdVencDvoConfig                  dvoConfig;
    /**<  Digital configuration */
    VpsHal_HdVencOsdConfig                  osdConfig;
    /**< OSd configurations */
    VpsHal_HdVencMvConfig                   mvConfig;
    /**< Configurations for the macrovision */
    VpsHal_HdVencVbiConfig                  vbiConfig;
    /**< Configurations for the VBI */
}VpsHal_HdVencConfig;

/**
 *  VpsHal_HdVencInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 *
 *  instId      - Instance Identifier
 *  baseAddress - Physical register start address for this instance
 */
typedef struct
{
    UInt32              instId;
    /**< HDVENC Instance */
    UInt32              baseAddress;
    /**< Base address of the instance */
    UInt32              pairedEncCnt;
    /**< Number of encoder that are paired with this instance of venc */
    UInt32              encIds[VPSHAL_HDVENC_MAX_NUM_OF_ONCHIP_ENC];
    /**< Paired encoders Identifier */
} VpsHal_HdVencInstParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_hdvencInit
 *  \brief Initializes HDVENC objects, gets the register overlay offsets for
 *  HDVENC registers.
 *  This function should be called before calling any of HDVENC HAL API's.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              Not used currently. Meant for future purpose.
 */
Int VpsHal_hdvencInit(UInt32 numInstances,
                      const VpsHal_HdVencInstParams *initParams,
                      Ptr arg);

/**
 *  VpsHal_hdvencDeInit
 *  \brief Currently this function does not do anything.
 *
 *  \param arg       Not used currently. Meant for future purpose.
 */
Int VpsHal_hdvencDeInit(Ptr arg);


/**
 *  VpsHal_hdvencOpen
 *  \brief This function should be called prior to calling any of the HDVENC HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param chrusInst       Requested HDVENC instance
 */
VpsHal_Handle VpsHal_hdvencOpen(UInt32 chrusInst);

/**
 *  VpsHal_hdvencClose
 *  \brief This functions closes the chrus handle and after call to this
 *  function, chrus handle is not valid. It also decrements the reference
 *  counter.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencClose(VpsHal_Handle handle);

/**
 *  VpsHal_hdvencStartVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencStartVenc(VpsHal_Handle handle);

/**
 *  VpsHal_hdvencStopVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencStopVenc(VpsHal_Handle handle);

/**
 *  VpsHal_hdvencResetVenc
 *  \brief This functions to reset the mode in the HdVenc.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencResetVenc(VpsHal_Handle handle);

/**
 *  VpsHal_hdvencSetConfig
 *  \brief This function is used to configure the HD Venc
 *
 *  \param handle         Requested HDVENC instance
 *
 *  \param vencCfg        Venc configuration parameters to passed for
 *                        configuring the venc
 */
Int VpsHal_hdVencSetConfig(VpsHal_Handle handle, VpsHal_HdVencConfig *vencCfg);
/**
 *  VpsHal_hdVencGetConfig
 *  \brief This function is used to configure the HD Venc
 *
 *  \param handle         Requested HDVENC instance
 *
 *  \param vencCfg        Get the programmed venc configuration parameters. It
 *                        will not copy the csc and gamma correction coeffs.
 */
Void VpsHal_hdVencGetConfig(VpsHal_Handle handle, VpsHal_HdVencConfig *vencCfg);

/**
 *  VpsHal_hdVencSetOutput
 *  \brief This function is used to configure output type for HD Venc
 *
 *  \param handle         Requested HDVENC instance
 *
 *  \param output         Ouput format
 */
Int32 VpsHal_hdVencSetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output);

/**
 *  VpsHal_hdVencGetOutput
 *  \brief This function is used to get output information from HD Venc
 *
 *  \param handle         Requested HDVENC instance
 *
 *  \param output         Ouput format
 */
Void VpsHal_hdVencGetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output);

/**
 * VpsHal_hdVencSetMode
 * \brief This function is used to set the mode on the HDVENC
 *
 * \param handle          Requested HDVENC instance
 *
 * \param mode            Mode information
 */
Int32 VpsHal_hdVencSetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode);

/**
 * VpsHal_hdVencGetMode
 * \brief This function is used to get the mode on the HDVENC
 *
 * \param handle          Requested HDVENC instance
 *
 * \param mode            Mode information
 */
Int32 VpsHal_hdVencGetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode);


/**
 *  VpsHal_hdvencIoctl
 *  \brief This function controls the VENC and other paired encoders. For now
 *         there are no VENC specific control operations that could be performed
 *
 *         If paired encoder (such as HDMI transmitter) for this VENC exists,
 *         the paired encoders control function would be called.
 *
 *  \param VpsHal_Handle    Handle to the venc
 *  \param cmd              Control command
 *  \param cmdArgs          Arguments associated with the command
 *  \param cmdStatusArgs    Result after execution of the command
 */
Int32 VpsHal_hdvencIoctl (  VpsHal_Handle   handle,
                            UInt32          cmd,
                            Ptr             cmdArgs,
                            Ptr             cmdStatusArgs);


/**
 *  VpsHal_hdvencInitEncOps
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
 * \param handle    Handle to HD Venc HAL
 * \param encOps    Capabilities of the encoder
 */
Int32 VpsHal_hdvencInitEncOps(  VpsHal_Handle           handle,
                                Vps_OnChipEncoderOps    *encOps);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPSHAL_HDVENC_H */

