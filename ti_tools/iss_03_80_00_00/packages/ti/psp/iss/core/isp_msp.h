/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file isp_msp.h
*
* XXX: Description
*
* @path iss/drivers/drv_isp/drv_isp
*
* @rev nn.mm
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _ISP_MSP_H
#define _ISP_MSP_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
    /* ----- system and platform files ---------------------------- */
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/core/msp_types.h>
/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------macros ----------------------------------------------*/
/** public data **/
    /* AEWB SCM Range Constants */
#define MSP_MAX_SATURATION_LIM  (1023)
#define MSP_MIN_WIN_H           (2)
#define MSP_MAX_WIN_H           (256)
#define MSP_MIN_WIN_W           (6)
#define MSP_MAX_WIN_W           (256)
#define MSP_MAX_WINVC           (32)                       // 128
#define MSP_MAX_WINVC_CAP       (128)
#define MSP_MAX_WINHC           (16)                       // 36
#define MSP_MAX_WINHC_CAP       (36)
#define MSP_MAX_WINSTART        (4095)
#define MSP_MIN_SUB_INC         (2)
#define MSP_MAX_SUB_INC         (32)

#define MSP_MAX_FRAME_COUNT     (0x0FFF)
#define MSP_MAX_FUTURE_FRAMES   (10)

    // One paxel is 32 Bytes + on every 8 paxel 8*2 Bytes for number of
    // unsaturated pixels in previouse 8 paxels
#define MSP_AWWB_H3A_PAXEL_SIZE_BYTES   (32 + 2)
#define MSP_AEWB_H3A_MAX_BUFF_SIZE      (MSP_MAX_WINHC * MSP_MAX_WINVC * MSP_AWWB_H3A_PAXEL_SIZE_BYTES)
#define MSP_AEWB_H3A_MAX_BUFF_SIZE_CAP  (MSP_MAX_WINHC_CAP * MSP_MAX_WINVC_CAP * MSP_AWWB_H3A_PAXEL_SIZE_BYTES)

    /* AF SCM Range Constants */
#define MSP_AF_IIRSH_MIN                    (0)
#define MSP_AF_IIRSH_MAX                    (4094)
#define MSP_AF_PAXEL_HORIZONTAL_COUNT_MIN   (0)
#define MSP_AF_PAXEL_HORIZONTAL_COUNT_MAX   (35)
#define MSP_AF_PAXEL_VERTICAL_COUNT_MIN     (0)
#define MSP_AF_PAXEL_VERTICAL_COUNT_MAX     (127)
#define MSP_AF_PAXEL_INCREMENT_MIN          (0)
#define MSP_AF_PAXEL_INCREMENT_MAX          (14)
#define MSP_AF_PAXEL_HEIGHT_MIN             (0)
#define MSP_AF_PAXEL_HEIGHT_MAX             (127)
#define MSP_AF_PAXEL_WIDTH_MIN              (0)
#define MSP_AF_PAXEL_WIDTH_MAX              (127)
#define MSP_AF_PAXEL_HZSTART_MIN            (2)
#define MSP_AF_PAXEL_HZSTART_MAX            (4094)

#define MSP_AF_PAXEL_VTSTART_MIN            (0)
#define MSP_AF_PAXEL_VTSTART_MAX            (4095)
#define MSP_AF_THRESHOLD_MAX                (255)
#define MSP_AF_COEF_MAX                     (4095)
#define MSP_AF_PAXEL_SIZE                   (48)
#define MSP_AF_NUMBER_OF_COEF               (11)

    // Size of one paxel is 48 Bytes
#define MSP_AF_H3A_MAX_BUFF_SIZE            (MSP_AF_PAXEL_VERTICAL_COUNT_MAX * MSP_AF_PAXEL_HORIZONTAL_COUNT_MAX * 48)

#define MSP_ISS_COMP_ISP_OUT_ALIGN      (32)
#define MSP_ISP_OUT_ROUND_UP(a)         ((a + (MSP_ISS_COMP_ISP_OUT_ALIGN - 1)) & ~(MSP_ISS_COMP_ISP_OUT_ALIGN - 1))

#define MSP_BSC_BUFFER_ELEMENTS         (1920)
#define MSP_BSC_BUFFER_ELEM_SIZE        (2)
#define MSP_BSC_BUFFER_SIZE_BYTES       (MSP_BSC_BUFFER_ELEMENTS * MSP_BSC_BUFFER_ELEM_SIZE)

#define MSP_HIST_MAX_SIZE               (256*4)

#define MSP_ISS_MAX_ZOOM_FACTOR     20
#define MSP_ISS_NO_ZOOM             0x10000
#define MSP_ISS_MAX_ZOOM           (MSP_ISS_NO_ZOOM / MSP_ISS_MAX_ZOOM_FACTOR)

#define MSP_ISS_SPEED_MAX           1000

/*--------data declarations -----------------------------------*/

    typedef enum {
        MSP_IPIPE_IRQ_RSZ,
        MSP_IPIPE_IRQ_ISIF,
        MSP_IPIPE_IRQ_H3A,
        MSP_IPIPE_IRQ_BSC,
        MSP_IPIPE_IRQ_VD,
        MSP_IPIPE_IRQ_2DLSC
    } MSP_IPIPE_IRQ_ID;

    typedef enum {
        MSP_IPIPE_YUV_RANGE_FULL,
        MSP_IPIPE_YUV_RANGE_RESTRICTED
    } MSP_IPIPE_YUV_RANGE;

    typedef enum {
        MSP_IPIPE_OUT_FORMAT_UNUSED,
        MSP_IPIPE_OUT_FORMAT_YUV422,
        MSP_IPIPE_OUT_FORMAT_YUV420,
        MSP_IPIPE_OUT_FORMAT_RGB888,
        MSP_IPIPE_OUT_FORMAT_RGB565,
        MSP_IPIPE_OUT_FORMAT_BAYER
    } MSP_IPIPE_OUT_FORMAT;

    typedef enum {
        MSP_IPIPE_IN_COMP_Y,
        MSP_IPIPE_IN_COMP_C
    } MSP_IPIPE_IN_YUV420_COMP_YC;

    typedef enum {
        MSP_IPIPE_IN_FORMAT_BAYER,
        MSP_IPIPE_IN_FORMAT_YUV422,
        MSP_IPIPE_IN_FORMAT_YUV420
    } MSP_IPIPE_IN_FORMAT;

    typedef enum {
        MSP_IPIPE_BAYER_PATTERN_RGGB,
        MSP_IPIPE_BAYER_PATTERN_GRBG,
        MSP_IPIPE_BAYER_PATTERN_GBRG,
        MSP_IPIPE_BAYER_PATTERN_BGGR
    } MSP_IPIPE_BAYER_PATTERN;

    typedef enum {
        MSP_IPIPE_BAYER_MSB_BIT15,
        MSP_IPIPE_BAYER_MSB_BIT14,
        MSP_IPIPE_BAYER_MSB_BIT13,
        MSP_IPIPE_BAYER_MSB_BIT12,
        MSP_IPIPE_BAYER_MSB_BIT11,
        MSP_IPIPE_BAYER_MSB_BIT10,
        MSP_IPIPE_BAYER_MSB_BIT9,
        MSP_IPIPE_BAYER_MSB_BIT8,
        MSP_IPIPE_BAYER_MSB_BIT7
    } MSP_IPIPE_BAYER_MSB_POS;

    typedef enum {
        MSP_PROC_ISIF_VALID_NO = 0 << 0,                   // No valid
                                                           // parameter
        MSP_PROC_ISIF_VALID_2DLSC = 1 << 0,                // Lens shadding
        MSP_PROC_ISIF_VALID_DC = 1 << 1,                   // Digital clamp
        MSP_PROC_ISIF_VALID_GOFFSET = 1 << 2,              // Gain offset
        MSP_PROC_ISIF_VALID_FLASHT = 1 << 3,               // Flash timming
        MSP_PROC_ISIF_VALID_VLCD = 1 << 4,                 // VLCD
        MSP_PROC_ISIF_VALID_ALL = 0xFFFF                   // All
    } MSP_PROC_ISIF_VALID_ID;

    typedef enum {
        MSP_PROC_IPIPE_VALID_NO = 0 << 0,                  // No valid
                                                           // parameter
        MSP_PROC_IPIPE_VALID_GIC = 1 << 0,
        MSP_PROC_IPIPE_VALID_EE = 1 << 1,
        MSP_PROC_IPIPE_VALID_NF1 = 1 << 2,
        MSP_PROC_IPIPE_VALID_NF2 = 1 << 3,
        MSP_PROC_IPIPE_VALID_CAR = 1 << 4,
        MSP_PROC_IPIPE_VALID_GAMMA = 1 << 5,
        MSP_PROC_IPIPE_VALID_LSC = 1 << 6,
        MSP_PROC_IPIPE_VALID_CFA = 1 << 7,
        MSP_PROC_IPIPE_VALID_RGB2YUV = 1 << 8,
        MSP_PROC_IPIPE_VALID_RGB2RGB1 = 1 << 9,
        MSP_PROC_IPIPE_VALID_RGB2RGB2 = 1 << 10,
        MSP_PROC_IPIPE_VALID_HIST = 1 << 11,
        MSP_PROC_IPIPE_VALID_GBCE = 1 << 12,
        MSP_PROC_IPIPE_VALID_WB = 1 << 13,
        MSP_PROC_IPIPE_VALID_YUV2YU = 1 << 14,
        MSP_PROC_IPIPE_VALID_BSC = 1 << 15,
        MSP_PROC_IPIPE_VALID_BOXCAR = 1 << 16,
        MSP_PROC_IPIPE_VALID_3DCC = 1 << 17,
        MSP_PROC_IPIPE_VALID_DPC_OTF = 1 << 18,
        MSP_PROC_IPIPE_VALID_DPC_LUT = 1 << 19,
        MSP_PROC_IPIPE_VALID_DPC_CGS = 1 << 20,
        MSP_PROC_IPIPE_VALID_ALL = 0xFFFF
    } MSP_PROC_IPIPE_VALID_ID;

    typedef enum {
        MSP_PROC_IPIPE_RSZ_VALID_NO = 0 << 0,              // No valid
                                                           // parameter
        MSP_PROC_IPIPE_RSZ_VALID_H_LPF = 1 << 0,
        MSP_PROC_IPIPE_RSZ_VALID_V_LPF = 1 << 1,
        MSP_PROC_IPIPE_RSZ_VALID_ALL = 0xFFFF
    } MSP_PROC_IPIPE_RSZ_VALID_ID;

    typedef enum {
        MSP_PROC_IPIPEIF_VALID_NO = 0 << 0,                // No valid
                                                           // parameter
        MSP_PROC_IPIPEIF_VALID_DFS = 1 << 0,
        MSP_PROC_IPIPEIF_VALID_DPC1 = 1 << 1,
        MSP_PROC_IPIPEIF_VALID_DPC2 = 1 << 2,
        MSP_PROC_IPIPEIF_VALID_DPCM = 1 << 3,
        MSP_PROC_IPIPEIF_VALID_ALL = 0xFFFF
    } MSP_PROC_IPIPEIF_VALID_ID;

    typedef enum {
        MSP_PROC_H3A_VALID_NO = 0 << 0,                    // No valid
                                                           // parameter
        MSP_PROC_H3A_VALID_AEWB = 1 << 0,
        MSP_PROC_H3A_VALID_AF = 1 << 1,
        MSP_PROC_H3A_VALID_H3A = 1 << 2,
        MSP_PROC_H3A_VALID_HIST = 1 << 3,
        MSP_PROC_H3A_VALID_ALL = 0xFFFF
    } MSP_PROC_H3A_VALID_ID;

    typedef enum {
        MSP_ISIF_LSC_8_PIXEL = 3,
        MSP_ISIF_LSC_16_PIXEL = 4,
        MSP_ISIF_LSC_32_PIXEL = 5,
        MSP_ISIF_LSC_64_PIXEL = 6,
        MSP_ISIF_LSC_128_PIXEL = 7
    } MSP_ISIF_LSC_GAIN_MODE_MN;

    typedef enum {
        MSP_LSC_GAIN_0Q8 = 0,
        MSP_LSC_GAIN_0Q8_PLUS_1 = 1,
        MSP_LSC_GAIN_1Q7 = 2,
        MSP_LSC_GAIN_1Q7_PLUS_1 = 3,
        MSP_LSC_GAIN_2Q6 = 4,
        MSP_LSC_GAIN_2Q6_PLUS_1 = 5,
        MSP_LSC_GAIN_3Q5 = 6,
        MSP_LSC_GAIN_3Q5_PLUS_1 = 7
    } MSP_ISIF_LSC_GAIN_FORMAT;

    typedef enum {
        MSP_ISIF_LSC_OFFSET_NO_SHIFT = 0,
        MSP_ISIF_LSC_OFFSET_1_LEFT_SHIFT = 1,
        MSP_ISIF_LSC_OFFSET_2_LEFT_SHIFT = 2,
        MSP_ISIF_LSC_OFFSET_3_LEFT_SHIFT = 3,
        MSP_ISIF_LSC_OFFSET_4_LEFT_SHIFT = 4,
        MSP_ISIF_LSC_OFFSET_5_LEFT_SHIFT = 5
    } MSP_ISIF_LSC_OFFSET_SHIFT;

    typedef enum {
        MSP_ISIF_LSC_OFFSET_OFF = 0,
        MSP_ISIF_LSC_OFFSET_ON = 1
    } MSP_ISIF_LSC_OFFSET_ENABLE;

    typedef struct {

        MSP_U8 nEnable;                                    // !< DLSCCFG
                                                           // ENABLE-- enable 
                                                           // 2d LSC module
        MSP_U16 nLscHSize;                                 // !< HVAL
                                                           // LSCHVAL--LSC
                                                           // HSIZE
        MSP_U16 nLscVSize;                                 // !< HVAL
                                                           // LSCHVAL--LSC
                                                           // VSIZE
        MSP_U16 nHDirDataOffset;                           // !< HOFST
                                                           // LSCHOFST
                                                           // DATAHOFST
                                                           // 0-16383--
                                                           // H-direction
                                                           // data offset
        MSP_U16 nVDirDataOffset;                           // !< VOFST
                                                           // LSCVOFST
                                                           // DATAHOFST
                                                           // 0-16383--
                                                           // V-direction
                                                           // data offset
        MSP_U8 nHPosInPaxel;                               // !< X DLSCINI
                                                           // 6:0--
                                                           // H-position of
                                                           // the paxel
        MSP_U8 nVPosInPaxel;                               // !< Y DLSCINI
                                                           // 6:0--
                                                           // V-position of
                                                           // the paxel

        MSP_ISIF_LSC_GAIN_MODE_MN ePaxHeight;              // !< GAIN_MODE_M
                                                           // DLSCCFG
        MSP_ISIF_LSC_GAIN_MODE_MN ePaxLength;              // !< GAIN_MODE_N
                                                           // DLSCCFG
        MSP_ISIF_LSC_GAIN_FORMAT eGainFormat;              // !< GAIN_FORMAT
                                                           // DLSCCFG
        MSP_U8 nOffsetScalingFactor;                       // !< offset
                                                           // scaling factor
        MSP_ISIF_LSC_OFFSET_SHIFT eOffsetShiftVal;         // !< OFSTSFT
                                                           // DLSCOFST--offset 
                                                           // shift value
        MSP_ISIF_LSC_OFFSET_ENABLE eOffsetEnable;          // !< OFSTSFT
                                                           // DLSCOFST--offset 
                                                           // enable value
        MSP_U32 nGainTableAddress;                         // !< gain table
                                                           // address--32 bit 
                                                           // aligned
        MSP_U16 nGainTableLength;                          // !< gain table
                                                           // length
        MSP_U32 nOffsetTableAddress;                       // !< offset table 
                                                           // address
        MSP_U16 nOffsetTableLength;                        // !< offset table 
                                                           // length

    } MSP_Isif2dLscCfgT;

    typedef enum {
        MSP_ISIF_HORIZONTAL_CLAMP_DISABLED = 0,
        MSP_ISIF_HORIZONTAL_CLAMP_ENABLED = 1,
        MSP_ISIF_PREVIOUS_HORIZONTAL_CLAMP_ENABLED = 2
    } MSP_ISIF_HORIZONTAL_CLAMP_MODE;

    typedef enum {
        MSP_ISIF_ONE_COLOR_CLAMP = 0,
        MSP_ISIF_FOUR_COLOR_CLAMP = 1
    } MSP_ISIF_BLACK_CLAMP_MODE;

    typedef enum {
        MSP_ISIF_HBLACK_2PIXEL_TALL = 0,
        MSP_ISIF_HBLACK_4PIXEL_TALL = 1,
        MSP_ISIF_HBLACK_8PIXEL_TALL = 2,
        MSP_ISIF_HBLACK_16PIXEL_TALL = 3
    } MSP_ISIF_HBLACK_PIXEL_HEIGHT;

    typedef enum {
        MSP_ISIF_HBLACK_32PIXEL_WIDE = 0,
        MSP_ISIF_HBLACK_64PIXEL_WIDE = 1,
        MSP_ISIF_HBLACK_128PIXEL_WIDE = 2,
        MSP_ISIF_HBLACK_256PIXEL_WIDE = 3
    } MSP_ISIF_HBLACK_PIXEL_WIDTH;

    typedef enum {
        MSP_ISIF_VBLACK_PIXEL_NOT_LIMITED = 0,
        MSP_ISIF_VBLACK_PIXEL_LIMITED = 1
    } MSP_ISIF_HBLACK_PIXEL_VALUE_LIM;

    typedef enum {
        MSP_ISIF_VBLACK_BASE_WINDOW_LEFT = 0,
        MSP_ISIF_VBLACK_BASE_WINDOW_RIGHT = 1
    } MSP_ISIF_HBLACK_BASE_WINDOW;

    typedef struct {
        MSP_U16 nVPos;                                     // !< CLHSV
                                                           // CLHWIN2
                                                           // 12-0--Vertical
                                                           // black clamp.
        // !< Window Start position (V).Range: 0 - 8191
        MSP_ISIF_HBLACK_PIXEL_HEIGHT eVSize;               // !< CLHWN
                                                           // CLHWIN0--[Horizontal 
                                                           // Black clamp]
        // !< Vertical dimension of a Window (2^N).
        MSP_U16 nHPos;                                     // !< CLHSH
                                                           // CLHWIN1
                                                           // 12-0--Horizontal 
                                                           // black clamp.
        // !< Window Start position (H).Range: 0 - 8191
        MSP_ISIF_HBLACK_PIXEL_WIDTH eHSize;                // !< CLHWM
                                                           // CLHWIN0--[Horizontal 
                                                           // Black clamp]
        // !< Horizontal dimension of a Window (2^M).
        MSP_ISIF_HBLACK_PIXEL_VALUE_LIM ePixelValueLimit;  // !< CLHLMT
                                                           // CLHWIN0--Horizontal 
                                                           // Black clamp.
                                                           // Pixel value
        // !< limitation for the Horizontal clamp value calculation
        MSP_ISIF_HBLACK_BASE_WINDOW eRightWindow;          // !< CLHWBS
                                                           // CLHWIN0--[Horizontal 
                                                           // Black clamp]
                                                           // Base Window
                                                           // select
        MSP_U8 nWindowCountPerColor;                       // !< CLHWC
                                                           // CLHWIN0--[Horizontal 
                                                           // Black clamp]
        // !< Window count per color. Window count = CLHWC+1. Range: 1 - 32
    } MSP_IsifHBlackParams;

    typedef enum {
        MSP_ISIF_VBLACK_2PIXEL_WIDE = 0,
        MSP_ISIF_VBLACK_4PIXEL_WIDE = 1,
        MSP_ISIF_VBLACK_8PIXEL_WIDE = 2,
        MSP_ISIF_VBLACK_16PIXEL_WIDE = 3,
        MSP_ISIF_VBLACK_32PIXEL_WIDE = 4,
        MSP_ISIF_VBLACK_64PIXEL_WIDE = 5
    } MSP_ISIF_VBLACK_PIXEL_WIDTH;

    typedef enum {
        MSP_ISIF_VALUE_HORIZONTAL_DIRECTION = 0,
        MSP_ISIF_VALUE_CONFIG_REGISTER = 1,
        MSP_ISIF_VALUE_NOUPDATE = 2
    } MSP_ISIF_VBLACK_RESET_MODE;

    typedef struct {

        MSP_U16 nVPos;                                     // !< CLVSV
                                                           // CLVWIN2
                                                           // 12-0--Vertical
                                                           // black clamp.
        // !< Window Start position (V).Range: 0 - 8191*
        MSP_U16 nVSize;                                    // !< CLVOBV
                                                           // CLVWIN3 12-0
                                                           // range 0-8192--
                                                           // Vertical black
                                                           // clamp.
        // !< Optical black V valid (V).Range: 0 - 8191
        MSP_U16 nHPos;                                     // !< CLVSH
                                                           // CLVWIN1
                                                           // 12-0--Vertical
                                                           // black clamp.
                                                           // Window Start
        // !< position (H).Range: 0 - 8191
        MSP_ISIF_VBLACK_PIXEL_WIDTH HSize;                 // !< CLVOBH
                                                           // CLVWIN0--
                                                           // Vertical Black
                                                           // clamp. Optical
                                                           // Black H valid
                                                           // (2^L).
        MSP_U8 line_avg_coef;                              // !< CLVCOEF
                                                           // CLVWIN0--
                                                           // Vertical Black
                                                           // clamp .Line
                                                           // average
                                                           // coefficient
                                                           // (k).
        MSP_ISIF_VBLACK_RESET_MODE reset_mode;             // !< CLVRVSL
                                                           // CLVWIN0--
                                                           // Vertical Black
                                                           // clamp. Select
                                                           // the reset value
        // !< for the Clamp value of the Previous line
        MSP_U16 reset_value;                               // !< CLVRV reset
                                                           // value
                                                           // U12--Vertical
                                                           // black clamp
                                                           // reset value
        // !< (U12) Range: 0 to 4095

    } MSP_IsifVerticalBlackParams;

    typedef struct {

        MSP_U8 nEnable;                                    // !< CLEN
                                                           // CLAMPCFG--
                                                           // clamp module
                                                           // enablement
        MSP_ISIF_HORIZONTAL_CLAMP_MODE eHClampMode;        // !< CLMD
                                                           // CLAMPCFG--
                                                           // horizontal
                                                           // clamp mode
        MSP_ISIF_BLACK_CLAMP_MODE eBlackClampMode;         // !< CLHMD
                                                           // CLAMPCFG--
                                                           // black clamp
                                                           // mode
        MSP_U16 nDCOffsetClampVal;                         // !< CLDCOFST
                                                           // CLDC s13--
                                                           // clamp dc-offset 
                                                           // value
        MSP_U16 nBlackClampVStartPos;                      // !< CLSV 12-0
                                                           // (range
                                                           // 0-8191)--black
                                                           // clamp v-start
                                                           // position
        MSP_IsifHBlackParams tHorizontalBlack;             // !< CLHWIN0--
                                                           // horizontal
                                                           // black clamp
                                                           // parameters
        MSP_IsifVerticalBlackParams tVerticalBlack;        // !< CLVWIN0--
                                                           // vertical black
                                                           // clamp
                                                           // parameters

    } MSP_IsifClampCfgT;

    typedef struct {

        MSP_U8 nEnable;                                    // !< FLSHEN
                                                           // FLSHCFG0--
                                                           // flash enable
        MSP_U16 nFlashTimingStartLine;                     // !< SFLSH
                                                           // FLSHCFG1--Start 
                                                           // line to set the 
                                                           // FLASH timing
                                                           // signal.
        MSP_U16 nFlashTimingWidth;                         // !< VFLSH
                                                           // FLSHCFG--Valid
                                                           // width of the
                                                           // FLASH timing
                                                           // signal.
        // !< Valid width = Crystal-clock x 2 x (VFLSH + 1)

    } MSP_IsifFlashCfgT;

    typedef struct {
        MSP_U8 gain_offset_featureflag;                    // !< gain offset
                                                           // feature-flag
        MSP_U16 gain_r;                                    // !< CGR CRGAIN-- 
                                                           // gain R
        MSP_U16 gain_gr;                                   // !< CGGR
                                                           // CGRGAIN-- gain
                                                           // GR
        MSP_U16 gain_gb;                                   // !< CGGB
                                                           // CGBGAIN-- gain
                                                           // GB
        MSP_U16 gain_bg;                                   // !< CGB CBGAIN-- 
                                                           // gain B
        MSP_U16 offset;                                    // !< COFT
                                                           // COFSTA--offset
    } MSP_IsifGainOffsetCfgT;

    typedef enum {
        MSP_ISIF_VDLC_WHOLE_LINE = 0,
        MSP_ISIF_VDLC_DISABLE_ABOVE_UPPER_PIXEL = 1
    } MSP_ISIF_VDLC_PIXEL_DEPENDENCY;

    typedef enum {
        MSP_ISIF_VLDC_FED_THRO_ONSATURATION = 0,
        MSP_ISIF_VLDC_HORIZONTAL_INTERPOLATION_ONSATURATION = 1,
        MSP_ISIF_VLDC_HORIZONTAL_INTERPOLATION = 2
    } MSP_ISIF_VLDC_MODE_SELECT;

    typedef struct {

        MSP_U16 nVerticalDefectPosition;                   // !<
                                                           // DFCMEM0--Vertical 
                                                           // Defect position
        MSP_U16 nHorizontalDefectPosition;                 // !< DFCMEM1
                                                           // 12-0--horizontal 
                                                           // defect position
        MSP_U8 nSub1ValueVldc;                             // !<
                                                           // DFCMEM2--Defect 
                                                           // correction
                                                           // Memory 2
        MSP_U8 nSub2LessThanVldc;                          // !<
                                                           // DFCMEM3--Defect 
                                                           // correction
                                                           // Memory 3
        MSP_U8 nSub3GreaterThanVldc;                       // !<
                                                           // DFCMEM4--Defect 
                                                           // correction
                                                           // Memory 4

    } MSP_IsifVldcDefectLineParamsT;

    typedef struct {

        MSP_U8 nEnable;                                    // !< VDFCEN
                                                           // DFCCTL--enable
                                                           // VLDC module
        MSP_ISIF_VDLC_PIXEL_DEPENDENCY eDisableVldcUpperPixels; // !< VDFCUDA 
                                                                // DFCCTL--pixel 
                                                                // dependency
        MSP_U8 nVldcShiftVal;                              // !< VDFLSFT
                                                           // DFCCTL-- VLDC
                                                           // shift values
        MSP_ISIF_VLDC_MODE_SELECT eVldcModeSelect;         // !< VDFCSL
                                                           // DFCCTL--VLDC
                                                           // mode select
        MSP_U16 nVldcSaturationLvl;                        // !< VDFSLV
                                                           // VDFSATLV
                                                           // MSP_U12 range 0 
                                                           // - 4095-- VLDC
                                                           // saturation
                                                           // level
        MSP_U8 nDefectLines;                               // !< number of
                                                           // defect
                                                           // lines-maximum8
        MSP_IsifVldcDefectLineParamsT *tVldcDefectLineParams;   // !< DFCMEM0 
                                                                // -8--
                                                                // defect
                                                                // line
                                                                // paramaters

    } MSP_IsifVldcCfgT;

    typedef enum {
        MSP_NOISE_FILTER_1 = 1,
        MSP_NOISE_FILTER_2 = 2
    } MSP_IPIPE_NOISE_FILTER_T;

    typedef enum {
        MSP_IPIPE_NF_SPR_SINGLE = 0,
        MSP_IPIPE_NF_SPR_LUT = 1
    } MSP_IPIPE_NF_SEL_T;

    typedef enum {
        MSP_IPIPE_NF_LSC_GAIN_OFF = 0,
        MSP_IPIPE_NF_LSC_GAIN_ON = 1
    } MSP_IPIPE_NF_LSC_GAIN_T;

    typedef enum {
        MSP_IPIPE_NF_SAMPLE_BOX = 0,
        MSP_IPIPE_NF_SAMPLE_DIAMOND = 1
    } MSP_IPIPE_NF_SAMPLE_METHOD_T;

    typedef struct {

        MSP_U8 nEnable;
        MSP_IPIPE_NOISE_FILTER_T eNFNum;
        MSP_IPIPE_NF_SEL_T eSel;
        MSP_IPIPE_NF_LSC_GAIN_T eLscGain;
        MSP_IPIPE_NF_SAMPLE_METHOD_T eTyp;
        MSP_U8 nDownShiftVal;
        MSP_U8 nSpread;
        MSP_U16 *pnThr;
        MSP_U8 *pnStr;
        MSP_U8 *pnSpr;
        MSP_U16 nEdgeMin;
        MSP_U16 nEdgeMax;

    } MSP_IpipeNoiseFilterCfgT;

    typedef enum {
        MSP_IPIPE_GIC_LSC_GAIN_OFF = 0,
        MSP_IPIPE_GIC_LSC_GAIN_ON = 1
    } MSP_IPIPE_GIC_LSC_GAIN_T;

    typedef enum {
        MSP_IPIPE_GIC_GICTHR = 0,
        MSP_IPIPE_GIC_NF2THR = 1
    } MSP_IPIPE_GIC_SEL_T;

    typedef enum {
        MSP_IPIPE_GIC_DIFF_INDEX = 0,
        MSP_IPIPE_GIC_HPD_INDEX = 1
    } MSP_IPIPE_GIC_INDEX_T;

    typedef struct {

    /** works only when data format is GR,GB */
        MSP_U16 nEnable;
        MSP_IPIPE_GIC_LSC_GAIN_T eLscGain;
        MSP_IPIPE_GIC_SEL_T eSel;
        MSP_IPIPE_GIC_INDEX_T eTyp;
        MSP_U8 nGicGain;
        MSP_U8 nGicNfGain;
        MSP_U16 nGicThr;
        MSP_U16 nGicSlope;

    } MSP_IpipeGicCfgT;

    typedef struct {

        MSP_U16 *pnOffset;                                 /* offseet after
                                                            * R,GR,GB,B */
        MSP_U16 *pnGain;                                   /* gain for R gr
                                                            * gb B */

    } MSP_IpipeWbCfgT;

    typedef enum {
        MSP_IPIPE_CFA_MODE_2DIR = 0,
        MSP_IPIPE_CFA_MODE_2DIR_DA = 1,
        MSP_IPIPE_CFA_MODE_DAA = 2
    } MSP_IPIPE_CFA_MODE_T;

    typedef struct {
        MSP_U16 nHpfThr;
        MSP_U16 nHpfSlope;
        MSP_U16 nMixThr;
        MSP_U16 nMixSlope;
        MSP_U16 nDirThr;
        MSP_U16 nDirSlope;
        MSP_U16 nDirNdwt;

    } MSP_IpipeCfaDirT;

    typedef struct {
        MSP_U8 nMonoHueFra;
        MSP_U8 nMonoEdgThr;
        MSP_U16 nMonoThrMin;
        MSP_U16 nMonoThrSlope;
        MSP_U16 nMonoSlpMin;
        MSP_U16 nMonoSlpSlp;
        MSP_U16 nMonoLpwt;

    } MSP_IpipeCfaDaaT;

    typedef struct {
        MSP_U8 nEnable;
        MSP_IPIPE_CFA_MODE_T eMode;
        MSP_IpipeCfaDirT tDir;
        MSP_IpipeCfaDaaT tDaa;
    } MSP_IpipeCfaCfgT;

    typedef enum {

        MSP_IPIPE_GAMMA_TBL_64 = 0,
        MSP_IPIPE_GAMMA_TBL_128 = 1,
        MSP_IPIPE_GAMMA_TBL_256 = 2,
        MSP_IPIPE_GAMMA_TBL_512 = 3
    } MSP_IPIPE_GAMMA_TABLE_SIZE_T;

    typedef enum {
        MSP_IPIPE_GAMMA_BYPASS_ENABLE = 1,
        MSP_IPIPE_GAMMA_BYPASS_DISABLE = 0
    } MSP_IPIPE_GAMMA_BYPASS_T;

    typedef struct {

        MSP_IPIPE_GAMMA_TABLE_SIZE_T eGammaTblSize;
        MSP_U8 nTbl;                                       // !< may not be
                                                           // needed.since
                                                           // table is always 
                                                           // in RAM
        MSP_IPIPE_GAMMA_BYPASS_T eBypassB;                 // !< o not
                                                           // bypassed
        MSP_IPIPE_GAMMA_BYPASS_T eBypassG;
        MSP_IPIPE_GAMMA_BYPASS_T eBypassR;
        MSP_S16 *pnRedTable;                               /* poMSP_Ser to
                                                            * red gamma table */
        MSP_S16 *pnBlueTable;
        MSP_S16 *pnGreenTable;

    } MSP_IpipeGammaCfgT;

    typedef struct {
        MSP_U16 *pnMulOff;                                 /* RR,GR,BR,RG,GG,BG,RB,GB,BB 
                                                            * each 11 bits */
        MSP_U16 *pnOft;                                    /* R,G,B each 13
                                                            * bits */

    } MSP_IpipeRgbRgbCfgT;

    typedef struct {
        MSP_U8 nBrightness;
        MSP_U8 nContrast;
        MSP_S16 *pnMulVal;                                 /* RY,GY,BY,RCB,GCB,BCB 
                                                            * ,RCR,GCR,BCR 12 
                                                            * bits */
        MSP_S16 *pnOffset;                                 /* Y,CB,CR -11bits */

    } MSP_IpipeRgbYuvCfgT;

    typedef enum {
        MSP_IPIPE_GBCE_METHOD_Y_VALUE = 0,                 // !< Cr CB
                                                           // unmodified
        MSP_IPIPE_GBCE_METHOD_GAIN_TBL = 1
    } MSP_IPIPE_GBCE_METHOD_T;

    typedef struct {
    /** Defect Correction Enable */
        MSP_U16 nEnable;
        MSP_IPIPE_GBCE_METHOD_T nTyp;
        MSP_U16 *LookupTable;
    } MSP_IpipeGbceCfgT;

    typedef enum {
        MSP_IPIPE_PROC_COMPR_NO,
        MSP_IPIPE_PROC_COMPR_DPCM,
        MSP_IPIPE_PROC_COMPR_ALAW
    } MSP_IPIPE_PROC_COMPRESSION;

    typedef enum {
        MSP_IPIPE_YUV_PHS_POS_COSITED = 0,
        MSP_IPIPE_YUV_PHS_POS_CENTERED = 1
    } MSP_IPIPE_YUV_PHASE_POS_T;

    typedef enum {
        MSP_IPIPE_VP_DEV_CSIA,
        MSP_IPIPE_VP_DEV_CSIB,
        MSP_IPIPE_VP_DEV_CCP,
        MSP_IPIPE_VP_DEV_PI
    } MSP_IPIPE_VP_DEVICE;

    typedef struct {

        MSP_IPIPE_YUV_PHASE_POS_T ePos;
        MSP_U8 nLpfEn;

    } MSP_IpipeYuv444Yuv422CfgT;

    typedef enum {
        MSP_IPIPE_HALO_REDUCTION_ENABLE = 1,
        MSP_IPIPE_HALO_REDUCTION_DISABLE = 0
    } MSP_IPIPE_EE_HALO_CTRL_T;

    typedef struct {
    /** Defect Correction Enable */
        MSP_U16 nEnable;
        MSP_IPIPE_EE_HALO_CTRL_T eHaloReduction;
        MSP_S16 *pnMulVal;                                 /* 9 co
                                                            * -efficients */
        MSP_U8 nSel;
        MSP_U8 nShiftHp;
        MSP_U16 nThreshold;
        MSP_U16 nGain;
        MSP_U16 nHpfLowThr;
        MSP_U8 nHpfHighThr;
        MSP_U8 nHpfGradientGain;
        MSP_U8 nHpfgradientOffset;
        MSP_S16 *pnEeTable;
    } MSP_IpipeEeCfgT;

    typedef struct {

        MSP_U8 nEnable;
        MSP_U8 nTyp;
        MSP_U8 nSw0Thr;
        MSP_U8 nSw1Thr;
        MSP_U8 nHpfType;
        MSP_U8 nHpfShift;
        MSP_U8 nHpfThr;
        MSP_U8 nGn1Gain;
        MSP_U8 nGn1Shift;
        MSP_U16 nGn1Min;
        MSP_U8 nGn2Gain;
        MSP_U8 nGn2Shift;
        MSP_U16 nGn2Min;

    } MSP_IpipeCarCfgT;

    typedef struct {

        MSP_U16 nVOffset;
        MSP_S16 nVLinearCoeff;                             // <! va1
        MSP_S16 nVQuadraticCoeff;                          // <! va2
        MSP_U8 nVLinearShift;                              // <! vs1
        MSP_U8 nVQuadraticShift;                           // <! vs2
        MSP_U16 nHOffset;
        MSP_S16 nHLinearCoeff;                             // <! va1
        MSP_S16 nHQuadraticCoeff;                          // <! va2
        MSP_U8 nHLinearShift;                              // <! vs1
        MSP_U8 nHQuadraticShift;                           // <! vs2
        MSP_U8 nGainR;
        MSP_U8 nGainGR;
        MSP_U8 nGainGB;
        MSP_U8 nGainB;
        MSP_U8 nOffR;
        MSP_U8 nOffGR;
        MSP_U8 nOffGB;
        MSP_U8 nOffB;
        MSP_U8 nShift;                                     // <! LSC_SHIFT
        MSP_U16 nMax;                                      // <! LSC_MAX

    } MSP_IpipeLscCfgT;

    typedef struct {
        MSP_U16 nVPos;
        MSP_U16 nVSize;
        MSP_U16 nHPos;
        MSP_U16 nHSize;
    } MSP_IpipeHistDimT;

    typedef struct {
        MSP_U8 nEnable;
        MSP_U8 nOst;
        MSP_U8 nSel;
        MSP_U8 nType;
        MSP_U8 nBins;
        MSP_U8 nShift;
        MSP_U8 nCol;                                       /* bits [3:0], 0
                                                            * is disable */
        MSP_U8 nRegions;                                   /* [3:0], 0 is
                                                            * disable */
        MSP_IpipeHistDimT *ptHistDim;                      /* pointer to
                                                            * array of 4
                                                            * structs */
        MSP_U8 nClearTable;
        MSP_U8 nTableSel;
        MSP_U8 *pnGainTbl;                                 /* r,gr,gb,b */

    } MSP_IpipeHistCfgT;

    typedef enum {
        MSP_BOXCAR_DISABLED = 0,
        MSP_BOXCAR_ENABLED = 1
    } MSP_BOXCAR_ENABLE_T;

    typedef enum {
        MSP_BOXCAR_FREE_RUN = 0,
        MSP_BOXCAR_ONE_SHOT = 1
    } MSP_BOXCAR_MODE_T;

    typedef enum {
        MSP_BOXCAR_8x8 = 0,
        MSP_BOXCAR_16x16 = 1
    } MSP_BOXCAR_SIZE_T;

    typedef struct {
        MSP_U8 nEnable;
        MSP_U8 nOst;                                       /* one shot or
                                                            * free run */
        MSP_U8 nBoxSize;
        MSP_U8 nShift;
        MSP_U32 pAddr;
    } MSP_IpipeBoxcarCfgT;

    typedef struct {
        MSP_U8 nVectors;
        MSP_U8 nShift;
        MSP_U16 nVPos;
        MSP_U16 nHPos;
        MSP_U16 nVNum;
        MSP_U16 nHNum;
        MSP_U8 nVSkip;
        MSP_U8 nHSkip;

    } MSP_IpipeBscPosParamsT;

    typedef struct {
        MSP_U8 nEnable;
        MSP_U8 nMode;
        MSP_U8 nColSample;
        MSP_U8 nRowSample;
        MSP_U8 nElement;                                   /* Y or CB or CR */
        MSP_IpipeBscPosParamsT nColPos;
        MSP_IpipeBscPosParamsT nRowPos;
    } MSP_IpipeBscCfgT;

    typedef enum {
        MSP_IPIPEIF_FEATURE_ENABLE = 1,
        MSP_IPIPEIF_FEATURE_DISABLE = 0
    } MSP_IPIPEIF_FEATURE_SELECT_T;

    typedef struct {
        MSP_IPIPEIF_FEATURE_SELECT_T eDfsGainEn;
        MSP_U16 nDfsGainVal;                               // !< valid only
                                                           // if dfs_gain_en
                                                           // =IPIPEIF_FEATURE_ENABLE
        MSP_U16 nDfsGainThr;                               // !< valid only
                                                           // if dfs_gain_en
                                                           // =IPIPEIF_FEATURE_ENABLE
        MSP_U16 nOclip;                                    // !< valid only
                                                           // if dfs_gain_en
                                                           // =IPIPEIF_FEATURE_ENABLE
        MSP_U8 nDfsDir;                                    // !< set to 0 if
                                                           // Sensor Parallel 
                                                           // interface data
                                                           // is
        // !< to be subtracted by DRK frm in SDRAM
    } MSP_IpipeifDfsCfgT;

    typedef struct {
        MSP_IPIPEIF_FEATURE_SELECT_T eDpcEn;
        MSP_U16 eDpcThr;
    } MSP_IpipeifDpcCfgT;

    typedef enum {
        MSP_IPIPEIF_DPCM_PREDICTION_SIMPLE = 0,
        MSP_IPIPEIF_DPCM_PREDICTION_ADVANCED = 1
    } MSP_IPIPEIF_DPCM_PRED_TYP_T;

    typedef enum {
        MSP_IPIPEIF_DPCM_BIT_SIZE_8_10 = 0,
        MSP_IPIPEIF_DPCM_BIT_SIZE_8_12 = 1
    } MSP_IPIPEIF_DPCM_BIT_SIZE_T;

    typedef struct {

        MSP_IPIPEIF_FEATURE_SELECT_T nDpcmEn;
        MSP_IPIPEIF_DPCM_PRED_TYP_T nDpcmPredictor;        // !< Valid only
                                                           // if DPCM is
                                                           // enabled;
                                                           // dpcm_en=1
        MSP_IPIPEIF_DPCM_BIT_SIZE_T nDpcmBitSize;          // !< Valid only
                                                           // if DPCM is
                                                           // enabled;
                                                           // dpcm_en=1

    } MSP_IpipeifDpcmCfgT;

    typedef enum {
        MSP_RSZ_IP_IPIPE = 0,
        MSP_RSZ_IP_IPIPEIF = 1
    } MSP_RSZ_IP_PORT_SEL_T;

    typedef struct {

        MSP_U8 nCIntensity;
        MSP_U8 nYIntensity;

    } MSP_RszLpfCfgT;

    typedef enum {
        MSP_H3A_FEATURE_DISABLE = 0,
        MSP_H3A_FEATURE_ENABLE = 1
    } MSP_H3A_FEATURE_ENABLE_T;

    typedef struct {
        MSP_U16 nVPos;                                     // !< AEWINSTART
                                                           // WINSV
                                                           // //AFPAXSTART
                                                           // PAXSV
        MSP_U16 nVSize;                                     // !< AEWWIN1 WINW 
                                                           // //AFPAX1 PAXH
        MSP_U16 nHPos;                                     // !< AEWINSTART
                                                           // WINSH
                                                           // //AFPAXSTART
                                                           // PAXSH
        MSP_U16 nHSize;                                     // !< AEWWIN1 WINH 
                                                           // //AFPAX1 PAXW
        MSP_U8 nVCount;                                    // !< AEWWIN1
                                                           // WINVC //AFPAX2
                                                           // PAXVC
        MSP_U8 nVIncr;                                     // !< AEWSUBWIN
                                                           // AEWINCV
                                                           // //AFPAX2 AFINCV
        MSP_U8 nHCount;                                    // !< AEWWIN1
                                                           // WINHC //AFPAX2
                                                           // PAXHC
        MSP_U8 nHIncr;                                     // !< AEWSUBWIN
                                                           // AEWINCH
                                                           // //AFPAX2 AFINCH
    } MSP_H3aPaxelCfgT;

    typedef struct {
        MSP_U16 nVPos;                                     // !< AEWINBLK
                                                           // WINSV single
                                                           // row of black
                                                           // line vpos
        MSP_U16 nHPos;                                     // !< AEWINBLK
                                                           // WINH win height
    } MSP_H3aAewbBlkDimsT;

    typedef enum {
        MSP_H3A_AEWB_OP_FMT_SUM_OF_SQR = 0,
        MSP_H3A_AEWB_OP_FMT_MINMAX = 1,
        MSP_H3A_AEWB_OP_FMT_SUM_ONLY = 2
    } MSP_H3A_AEWB_OP_FMT_T;

    typedef struct {

        MSP_H3A_FEATURE_ENABLE_T eAewbEnable;              // !< to enable
                                                           // the Aewb engine
        MSP_H3aPaxelCfgT *ptAewbPaxelWin;
        MSP_H3aAewbBlkDimsT *ptBlkWinDims;
        MSP_H3A_AEWB_OP_FMT_T eAeOpFmt;                    // !< AEWCFG AEFMT
        MSP_U8 nShiftValue;                                // !< AEWCFG
                                                           // SUMFST
        MSP_U16 nSaturationLimit;                          // PCR AVE2LMT
        MSP_U32 nAewbOpAddr;                               // AEWBUFST
                                                           // AEWBUFST, 64
                                                           // bit aligned
                                                           // address

    } MSP_H3aAewbParamT;

    typedef enum {
        MSP_H3A_AF_RGBPOS_GR_GB_BAYER = 0,
        MSP_H3A_AF_RGBPOS_RG_GB_BAYER = 1,
        MSP_H3A_AF_RGBPOS_GR_BG_BAYER = 2,
        MSP_H3A_AF_RGBPOS_RG_BG_BAYER = 3,
        MSP_H3A_AF_RGBPOS_GG_RB_CUSTOM = 4,
        MSP_H3A_AF_RGBPOS_RB_GG_CUSTOM = 5
    } MSP_H3A_RGB_POS_T;

    typedef struct {
        MSP_U16 anIirCoef[11];                             // AFCOEF010
                                                           // COEFF1
        MSP_U16 nHfvThres;
    } MSP_H3aAfIirParam;

    typedef struct {
        MSP_U8 anFirCoef[5];
        MSP_U16 nVfvThres;
    } MSP_H3aAfFirParamT;

    typedef struct {

        MSP_H3A_FEATURE_ENABLE_T eAfEnable;                // to enable the
                                                           // AF engine
        MSP_H3A_RGB_POS_T eRgbPos;                         // valid only if
                                                           // vertical focus
                                                           // is enabled

        MSP_H3A_FEATURE_ENABLE_T ePeakModeEn;
        MSP_H3A_FEATURE_ENABLE_T eVerticalFocusEn;

        MSP_U16 nIirStartPos;                              // AFIIRSH IIRSH

        MSP_H3aPaxelCfgT *ptAfPaxelWin;

        MSP_H3aAfIirParam *ptIir1;
        MSP_H3aAfIirParam *ptIir2;

        MSP_H3aAfFirParamT *ptFir1;
        MSP_H3aAfFirParamT *ptFir2;
        MSP_U32 nAfOpAddr;                                 // AEWBUFST
                                                           // AEWBUFST, 64
                                                           // bit aligned
                                                           // address

    } MSP_H3aAfParamT;
	/**
	\brief  AF packet format with vertical AF disabled
	*/
	typedef struct {

		Uint32  hfvSum_0;
		Uint32  hfv1_0;
		Uint32  hfv2_0;
		Uint32  reserved_0;

		Uint32  hfvSum_1;
		Uint32  hfv1_1;
		Uint32  hfv2_1;
		Uint32  reserved_1;

		Uint32  hfvSum_2;
		Uint32  hfv1_2;
		Uint32  hfv2_2;
		Uint32  reserved_2;

	} MSP_H3aAfOutVfDisableOverlay;

	/**
	\brief  AF packet format with vertical AF enabled
	*/
	typedef struct {

		Uint32  hfvSum;
		Uint32  hfv1;
		Uint32  hfv1Square;
		Uint32  hfv1Count;

		Uint32  hfv2;
		Uint32  hfv2Square;
		Uint32  hfv2Count;
		Uint32  reserved_0;

		Uint32  vfv1;
		Uint32  vfv1Square;
		Uint32  vfv1Count;
		Uint32  reserved_1;

		Uint32  vfv2;
		Uint32  vfv2Square;
		Uint32  vfv2Count;
		Uint32  reserved_2;

	} MSP_H3aAfOutVfEnableOverlay;

    typedef struct {
        MSP_H3A_FEATURE_ENABLE_T eAfMedianEn;
        MSP_H3A_FEATURE_ENABLE_T eAewbMedianEn;
        MSP_U8 nMedianFilterThreshold;                     // !< valid only
                                                           // if median_en is 
                                                           // set to
                                                           // H3A_FEATURE_ENABLE
        MSP_H3A_FEATURE_ENABLE_T eAfAlawEn;
        MSP_H3A_FEATURE_ENABLE_T eAewbAlawEn;
        MSP_H3A_FEATURE_ENABLE_T eIpipeifAveFiltEn;
        MSP_H3A_FEATURE_ENABLE_T eH3aDecimEnable;
    } MSP_H3aCommonCfgT;

    typedef struct {
        MSP_U8 enable;
        MSP_PTR pInBiffData;
        // MSP_Ipipe3DLutDataT eDataType;
        // MSP_U32 tblAddress;
    } MSP_Ipipe3DccCfgT;

    typedef enum {
        MSP_IPIPE_DPC_LUT_REPLACE_BLACK = 0,
        MSP_IPIPE_DPC_LUT_REPLACE_WHITE = 1
    } MSP_IPIPE_DPC_LUT_REPLACEMENT_T;

    typedef enum {
        MSP_IPIPE_DPC_LUT_TBL_SIZE_1024 = 0,
        MSP_IPIPE_DPC_LUT_TBL_SIZE_INF = 1
    } MSP_IPIPE_DPC_LUT_TBL_SIZE_T;

    typedef struct {
    /** Defect Correction Enable */
        MSP_U16 nEnable;
        MSP_IPIPE_DPC_LUT_TBL_SIZE_T eTableType;
        MSP_IPIPE_DPC_LUT_REPLACEMENT_T eReplaceType;
        MSP_U16 nLutValidAddr;
        MSP_U16 nLutSize;
        MSP_U32 nLutTable0Addr;                            /* This is
                                                            * mentioned in
                                                            * ISS doc */
        MSP_U32 nLutTable1Addr;

    } MSP_IpipeDpcLutCfgT;

    typedef enum {
        MSP_IPIPE_DPC_OTF_ALG_MINMAX2 = 0,
        MSP_IPIPE_DPC_OTF_ALG_MINMAX3 = 1
    } MSP_IPIPE_DPC_ALGO_T;

    typedef enum {
        MSP_IPIPE_DPC_OTF_MAX1_MIN1 = 0,
        MSP_IPIPE_DPC_OTF_MAX2_MIN2 = 1
    } MSP_IPIPE_DPC_OTF_TYPE_T;

    typedef struct {

        MSP_U16 thr_cor_r;
        MSP_U16 thr_cor_gr;
        MSP_U16 thr_cor_gb;
        MSP_U16 thr_cor_b;

        MSP_U16 thr_det_r;
        MSP_U16 thr_det_gr;
        MSP_U16 thr_det_gb;
        MSP_U16 thr_det_b;

    } MSP_IpipeDpcOtfDpc2T;

    typedef struct {

        MSP_U8 nDThr;
        MSP_U8 nDSlp;
        MSP_U16 nDMin;
        MSP_U16 nDMax;

    } MSP_IpipeDpcOftFilterT;

    typedef struct {
        MSP_U8 eShift;
        MSP_IpipeDpcOftFilterT eOtfCorr;
        MSP_IpipeDpcOftFilterT eOtfDett;

    } MSP_IpipeDpcOtfDpc3T;

    typedef union {
        MSP_IpipeDpcOtfDpc2T tDpc2Params;
        MSP_IpipeDpcOtfDpc3T tDpc3Params;
    } MSP_IpipeDpcOtfFilterParamsT;

    typedef struct {
    /** Defect Correction Enable */
        MSP_U8 nEnable;
        MSP_IPIPE_DPC_OTF_TYPE_T eType;
        MSP_IPIPE_DPC_ALGO_T eAlgo;
        MSP_IpipeDpcOtfFilterParamsT tDpcData;
    } MSP_IpipeDpcOtfCfgT;

    typedef struct {
        MSP_U16 thr;
        MSP_U16 gain;
        MSP_U16 shift;
        MSP_U16 min;

    } MSP_IpipeChromaParamsT;

    typedef struct {
        MSP_U16 enable;
        MSP_IpipeChromaParamsT y_chroma_low;
        MSP_IpipeChromaParamsT y_chroma_high;
        MSP_IpipeChromaParamsT c_chroma;
    } MSP_IpipeCgsCfgT;

    typedef struct {
        MSP_PROC_ISIF_VALID_ID eFProcIsifValidity;
        MSP_PROC_IPIPE_VALID_ID eFProcIpipeValidity;
        MSP_PROC_IPIPEIF_VALID_ID eFProcIpipeifValidity;
        MSP_PROC_IPIPE_RSZ_VALID_ID eFProcRszValidity;
        MSP_PROC_H3A_VALID_ID eFH3aValidity;

        MSP_IPIPE_BAYER_PATTERN eColorPattern;
        MSP_IPIPE_BAYER_MSB_POS eMsbPos;
        MSP_RSZ_IP_PORT_SEL_T eRszIpPortSel;
        MSP_IPIPE_VP_DEVICE nVpDevice;

        MSP_Isif2dLscCfgT *ptLsc2D;
        MSP_IsifClampCfgT *ptClamp;
        MSP_IsifFlashCfgT *ptFlash;
        MSP_IsifGainOffsetCfgT *ptGainOffset;
        MSP_IsifVldcCfgT *ptVlcd;

        MSP_IpipeNoiseFilterCfgT *ptNf1;
        MSP_IpipeNoiseFilterCfgT *ptNf2;
        MSP_IpipeGicCfgT *ptGIC;
        MSP_IpipeWbCfgT *ptWB;
        MSP_IpipeCfaCfgT *ptCFA;
        MSP_IpipeGammaCfgT *ptGamma;
        MSP_IpipeRgbRgbCfgT *ptRgb2Rgb1;
        MSP_IpipeRgbRgbCfgT *ptRgb2Rgb2;
        MSP_IpipeRgbYuvCfgT *ptRgb2Yuv;
        MSP_IpipeGbceCfgT *ptGBCE;
        MSP_IpipeYuv444Yuv422CfgT *ptYuv2Yuv;
        MSP_IpipeEeCfgT *ptEe;
        MSP_IpipeCarCfgT *ptCar;
        MSP_IpipeLscCfgT *ptLsc;
        MSP_IpipeHistCfgT *ptHistogram;
        MSP_IpipeBoxcarCfgT *ptBoxcar;
        MSP_IpipeBscCfgT *ptBsc;
        MSP_Ipipe3DccCfgT *pt3Dcc;
        MSP_IpipeDpcOtfCfgT *ptDpcOtf;
        MSP_IpipeDpcLutCfgT *ptDpcLut;
        MSP_IpipeCgsCfgT *ptCgs;

        MSP_IpipeifDfsCfgT *ptDfs;
        MSP_IpipeifDpcCfgT *ptDpc1;
        MSP_IpipeifDpcCfgT *ptDpc2;
        MSP_IpipeifDpcmCfgT *ptDpcm;

        MSP_RszLpfCfgT *ptHLpf;
        MSP_RszLpfCfgT *ptVLpf;

        MSP_H3aAewbParamT *ptH3aAewbParams;
        MSP_H3aAfParamT *ptH3aAfParams;
        MSP_H3aCommonCfgT *ptH3aCommonParams;

        MSP_U8 *pfIsValid;

    } MSP_IspConfigProcessingT;

    typedef struct {
        MSP_U32 nInSizePpln;
        MSP_U32 nInSizeX;
        MSP_U32 nInSizeY;
        MSP_S32 nInStartX;
        MSP_S32 nInStartY;
        MSP_U32 nOutSizeABpln;
        MSP_U32 nOutSizeAX;
        MSP_U32 nOutSizeAY;
        MSP_U32 nOutSizeBBpln;
        MSP_U32 nOutSizeBX;
        MSP_U32 nOutSizeBY;
        MSP_U32 nCropX;
        MSP_U32 nCropY;
        MSP_U32 nOutSizeABplnC;
        MSP_U32 nOutSizeBBplnC;
        MSP_U32 nClockPercents;

    } MSP_IspConfigSizesT;

    typedef struct {
        MSP_U32 nVidstabEnb;                               // uint32
                                                           // VidstabEnb;
        MSP_IspConfigSizesT tConfigSizes;                  // config_sizes_t
                                                           // config_sizes;
        MSP_IPIPE_IN_FORMAT eInFormat;                     // IPIPE_IN_FORMAT 
                                                           // in_format;
        MSP_IPIPE_OUT_FORMAT eOutFormatA;                  // IPIPE_OUT_FORMAT 
                                                           // out_format;
        MSP_IPIPE_OUT_FORMAT eOutFormatB;                  // IPIPE_OUT_FORMAT 
                                                           // out_formatB;
        MSP_IPIPE_PROC_COMPRESSION eCompresIn;             // IPIPE_PROC_COMPRESSION 
                                                           // f_compres_in;
        MSP_IPIPE_PROC_COMPRESSION eCompresOut;            // IPIPE_PROC_COMPRESSION 
                                                           // f_compres_out;
        MSP_U32 nFFlipA;                                   // uint32 f_flip;
        MSP_U32 nFMirrorA;                                 // uint32
                                                           // f_mirror;
        MSP_U32 nFFlipB;                                   // uint32
                                                           // f_flip_B;
        MSP_U32 nFMirrorB;                                 // uint32
                                                           // f_mirror_B;
        MSP_IspConfigProcessingT *ptIssConfig;             // iss_config_processing_t 
                                                           // *iss_config;
        MSP_PTR *pLsc2DTableBuffer;
        MSP_IPIPE_IN_YUV420_COMP_YC eInComp;
    } MSP_IspConfigParamsT;

    typedef struct {
    } MSP_IspCreateParamT;

    typedef enum {

        MSP_ISP_CMD_H3A_START,
        MSP_ISP_CMD_H3A_STOP,
        MSP_ISP_CMD_STOP_PROCESSING,
        MSP_ISP_CMD_MEM_TO_MEM_START,
        MSP_ISP_CMD_SENSOR_TO_MEM_START,
        MSP_ISP_CMD_SENSOR_SYNC,
        MSP_ISP_CMD_DARK_FRAME_START,
        MSP_ISP_CMD_STOP_ISIF,
        MSP_ISP_CMD_CCP_TO_ISIF_START,
        MSP_ISP_CMD_CCP_TO_ISIF_STOP,
        MSP_ISP_CMD_NUM
    } MSP_IspProcessCmdT;

    typedef struct {

        MSP_PTR pInBuffer;
        MSP_PTR pOutbuffRszA;
        MSP_PTR pOutbuffRszB;
        MSP_PTR pOutbuffRszACr;
        MSP_PTR pOutbuffRszBCr;
        MSP_IPIPE_IN_FORMAT eInFormat;
        MSP_PTR pOutRawBuffer;
    } MSP_IspMemToMemCmdParamT;

    typedef struct {

        MSP_PTR pOutBuffer;
    } MSP_IspSenToMemCmdParamT;

    typedef struct {

        MSP_PTR pInBuffer;
        MSP_PTR pOutBuffer;
    } MSP_IspCcpToIsifCmdParamT;

    typedef struct {

        MSP_PTR pInBuffer;
        MSP_U32 nWidth;
        MSP_U32 nHeight;
        MSP_U32 nFrames;
    } MSP_IspStopIsifCmdParamT;

    typedef struct {

        MSP_PTR pOutBuffer;
        MSP_PTR pDfsAddress;
    } MSP_IspDarkFrameCmdParamT;

    typedef struct {

        MSP_IspProcessCmdT eCmd;
        MSP_PTR pCmdData;
    } MSP_IspProcessParamT;

    typedef enum {
        MSP_IPIPE_DMA_DPC_1,
        MSP_IPIPE_DMA_LAST_PIX,
        MSP_IPIPE_DMA_DPC_0,
        MSP_IPIPE_DMA_HIST,
        MSP_IPIPE_DMA_BSC
    } MSP_DmaReqIdT;

    typedef struct {
        MSP_DmaReqIdT eDmaReqId;
        MSP_BOOL bEnable;
    } MSP_IspDmaReqCfgT;

    typedef enum {

        MSP_ISS_DRV_RSZ_END_ISR,
        MSP_ISS_DRV_ISP_IRQ_START = MSP_ISS_DRV_RSZ_END_ISR,
        MSP_ISS_DRV_ISIF_END_ISR,
        MSP_ISS_DRV_H3A_END_ISR,
        MSP_ISS_DRV_IPIPE_INT_BSC,
        MSP_ISS_DRV_ISIF_VD_ISR,
        MSP_ISS_DRV_ISIF_2DLSC_ISR,
        MSP_ISS_DRV_IPIPE_INT_HST,
        MSP_ISS_DRV_IPIPE_IF,
        MSP_ISS_DRV_CCP_LCM,
        MSP_ISS_DRV_ISS_IRQ_START = MSP_ISS_DRV_CCP_LCM
    } MSP_IssInterruptIdT;

    typedef enum {
        MSP_ISP_CALLBACK_CONTEXT_SWI,
        MSP_ISP_CALLBACK_CONTEXT_HWI,
        MSP_ISP_CALLBACK_CONTEXT_MAX
    } MSP_IspCallbackContextT;

    typedef struct {

        MSP_IssInterruptIdT eInterruptId;
        MSP_IspCallbackContextT nContext;
        MSP_U8 nPriority;
    } MSP_IspCallbackRegCfgT;

    typedef void (*MSP_IssIrqCallbackT) (MSP_ERROR_TYPE status, uint32 arg1,
                                         void *arg2);

    typedef struct {

        MSP_IssInterruptIdT eInterruptId;
    } MSP_IssCallbackRegCfgT;

    typedef struct {

        MSP_IssInterruptIdT eInterruptId;
    } MSP_IspCallbackUnregCfgT;

    typedef struct {

        MSP_IssInterruptIdT eInterruptId;
        MSP_BOOL bEnable;
    } MSP_IspInterruptCfgT;

    typedef struct {

        MSP_U32 nLineNumber;
        MSP_U32 nVdId;
    } MSP_IspVdLineNumberCfgT;

    typedef enum {
        MSP_ISS_YUV_RANGE_FULL,
        MSP_ISS_YUV_RANGE_RESTRICTED
    } MSP_IspYuvRangeT;

    typedef struct {
        MSP_IspYuvRangeT eIspYuvRange;
    } MSP_IspYuvRangeCfgT;

    typedef struct {
        MSP_BOOL bEnable;
    } MSP_Isp2DLscCfgT;
    typedef enum {
        MSP_CALLBACK_CMD_START,
        MSP_CALLBACK_CMD_STOP
    } MSP_IspCallbackCmdT;
    typedef struct {
        MSP_IspConfigProcessingT *ptIssConfig;
        MSP_PTR pAewbBuff;
        MSP_PTR pAfBuff;
    } MSP_IspH3aCfgT;

    typedef struct {
        MSP_IspConfigProcessingT *ptIssConfig;
        MSP_PTR pNew2DLscTable;
    } MSP_Isp2DLscTableCfgT;

    typedef struct {
        MSP_IspConfigProcessingT *ptIssConfig;
    } MSP_IspProcCfgT;

    typedef struct {
        MSP_IspConfigParamsT *ptIssParams;
    } MSP_IspParamCfgT;

    typedef enum {
        MSP_ISP_CFG_INT,
        MSP_ISP_CFG_DMA_REQ,
        MSP_ISP_CFG_REG_CALLBACK,
        MSP_ISP_CFG_UNREG_CALLBACK,
        MSP_ISP_CFG_YUV_RANGE,
        MSP_ISP_CFG_H3A,
        MSP_ISP_CFG_BOXCAR,
        MSP_ISP_CFG_HIST,
        MSP_ISP_CFG_BSC,
        MSP_ISP_CFG_IPIPE,
        MSP_ISP_CFG_PROCESSING,
        MSP_ISP_CFG_PROC_MEM_TO_MEM,
        MSP_ISP_CFG_PROC_SENSOR_TO_MEM,
        MSP_ISP_CFG_PROC_CCP_TO_ISIF,
        MSP_ISP_CFG_PROC_DARK_FRAME,
        MSP_ISP_CFG_VD_ISR,
        MSP_ISP_CFG_2DLSC_EN,
        MSP_ISP_CFG_SET_LSC_TABLE,
        MSP_ISS_CFG_REG_CALLBACK,
        MSP_ISS_CFG_UNREG_CALLBACK,
        MSP_ISS_CFG_IRQ,
        MSP_ISP_CFG_NUM
    } MSP_CfgIndexT;

    typedef struct {
        MSP_PTR pRowsData;
        MSP_PTR pColsData;
    } MSP_IspQueryBscAddrT;

    typedef enum {
        MSP_IPIPE_HIST_R = 0,
        MSP_IPIPE_HIST_B = 1,
        MSP_IPIPE_HIST_G = 2,
        MSP_IPIPE_HIST_Y = 3
    } MSP_IPIPE_HIST_COLOR_T;

    typedef struct {
        // Input parameters
        MSP_PTR pHistBuffer;
        MSP_U32 nHistRegion;
        MSP_IPIPE_HIST_COLOR_T eHistColor;
        // Output parameter
        MSP_U32 nOutHistSize;
    } MSP_IspQueryHistDataT;
    typedef struct {
        MSP_IpipeBscCfgT *ptBscCfg;
        MSP_PTR pRowsBuffer;
        MSP_PTR pColsBuffer;
    } MSP_IspQueryBscDataT;

    typedef struct {
        MSP_IpipeBscCfgT *ptBscCfg;
        MSP_U32 pRowsBufferSize;
        MSP_U32 pColsBufferSize;
    } MSP_IspQueryBscSizesT;

    typedef enum {
        MSP_H3A_STOPPED = 0,
        MSP_H3A_AEWB_ENABLED = (1 << 0),
        MSP_H3A_AEWB_BUSY = (1 << 1),
        MSP_H3A_AF_ENABLED = (1 << 2),
        MSP_H3A_AF_BUSY = (1 << 3)
    } MSP_IspH3aStatesT;

    typedef struct {
        MSP_IspH3aStatesT eH3aState;
    } MSP_IspQueryH3aStateT;

    typedef enum {
        MSP_ISP_QUERY_BSC_ADDR,
        MSP_ISP_QUERY_BSC_BUFF_SIZES,
        MSP_ISP_QUERY_GET_HIST,
        MSP_ISP_QUERY_GET_BSC,
        MSP_ISP_QUERY_H3A_STATE,
        MSP_ISP_QUERY_CROP_SIZES,
        MSP_ISP_QUERY_MAX_WIDTH,
        MSP_ISP_QUERY_CLOCK_DIV,
        MSP_ISP_QUERY_NUM
    } MSP_QueryIndexT;

    typedef struct MSP_IspIntHandleQueueT {
        Interrupt_Handle_T *pHandle;
        MSP_PTR pNext;
    } MSP_IspIntHandleQueueT;

/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
    /* ========================================================================== 
     */
/**
 * @fn  MSP_ISP_init()    MSP ISP device driver initlization function. Allocates
 * memory for the MSP ISP device driver handle and initializes the MSP function
 * pointers
 *
 * @see  isp_msp.h
*/
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE MSP_ISP_init(MSP_COMPONENT_TYPE * hMSP,
                                MSP_PROFILE_TYPE tProfile);

    /* ========================================================================== 
     */
/**
 * @fn  MSP_ISP_control()    MSP ISP device driver control function.
 * Issues commands to ISP device
 *
 * @see  isp_msp.h
*/
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE MSP_ISP_control(MSP_HANDLE handle, MSP_CTRLCMD_TYPE tCmd,
                                   MSP_PTR pCmdParam);

    /* ========================================================================== 
     */
/**
 * @fn  MSP_ISP_config()    MSP ISP device driver config function.
 * Writes different config settings of the ISP device
 *
 * @see  isp_msp.h
*/
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE MSP_ISP_config(MSP_HANDLE handle, MSP_INDEXTYPE tConfigIndex,
                                  MSP_PTR pConfigParam);

    /* ========================================================================== 
     */
/**
 * @fn  MSP_ISP_query()    MSP ISP device driver query function.
 * Reads different config settings of the ISP device
 *
 * @see  isp_msp.h
*/
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE MSP_ISP_query(MSP_HANDLE handle, MSP_INDEXTYPE tQueryIndex,
                                 MSP_PTR pQueryParam);

    /* ========================================================================== 
     */
/**
 * @fn  MSP_ISP_process()    MSP ISP device driver process function.
 * Empty (not used)
 *
 * @see  isp_msp.h
*/
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE MSP_ISP_process(MSP_HANDLE handle, MSP_PTR pArg,
                                   MSP_BUFHEADER_TYPE * ptBufHdr);
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _ISP_MSP_H */
