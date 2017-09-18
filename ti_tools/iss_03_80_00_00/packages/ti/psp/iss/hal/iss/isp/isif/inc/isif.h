/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file isif.h
 *
 * This file contains structures and definitions for ISIF component of Ducati subsystem in OMAP4/Monica
 *
 * @path Centaurus\drivers\drv_isp\in\csl
 *
 * @rev 1.0
 *
 * 
 */
/*========================================================================*/

#ifndef ISIF_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define ISIF_H

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/
#include "isif_reg.h"
#include "../../isp5_utils/isp5_csl_utils.h"

/* Linearization */
#define ISIF_LINEN_FLAG                  (1<<0)

/* Data formatter flag */
#define ISIF_IP_DATA_FORMATTER_FLAG      (1<<1)

/* color space converter flag */
#define ISIF_CSC_FLAG                    (1<<2)

/* vertical line defect corection flag */
#define ISIF_VLDC_FLAG                   (1<<3)

/* lens shading correction flag */
#define ISIF_LSC_FLAG                    (1<<4)

/* LPF Anti-aliasing flag */
#define ISIF_HLPF_FLAG                   (1<<5)

/* A-law flag */
#define ISIF_ALAW_FLAG                   (1<<6)

/* DPCM encoder flag */
#define ISIF_DPCM_FLAG                   (1<<7)

/* Black clamp flag */
#define ISIF_BLACK_CLAMP_FLAG            (1<<8)

/* flash timing flag */
#define ISIF_FLASH_TIMING_FLAG           (1<<9)

/* horizontal-vertical culling flag */
#define ISIF_HVCULLING_FLAG              (1<<10)

/* gain-offset flag */
#define ISIF_GAIN_OFFSET_FLAG            (1<<11)

/* SDRAM flag */
#define ISIF_SDRAM_OP_FLAG               (1<<12)

/* Input source flag */
#define ISIF_INPUT_FLAG                  (1<<13)

/* VD flag */
#define ISIF_VD_FLAG                     (1<<14)

typedef volatile CSL_IsifRegs *isif_regs_ovly;

/* : WARNING: ISIF_BASE_ADDRESS correct value needs to be entered based on
 * the specification */
#define ISIF_BASE_OFFSET    0x1000
#define ISIF_BASE_ADDRESS (ISP5_BASE_ADDRESS + ISIF_BASE_OFFSET)

/* ======================================================================= */
/* isif_start_t enumeration for the mode of operation of ISIF module @param
 * ISIF_STOP stop/disable @param ISIF_RUN run/enable @param ISIF_SINGLE one
 * shot run */
/* ======================================================================= */
typedef enum {
    ISIF_STOP,
    ISIF_STOP_WRITE,
    ISIF_START_SYNC,
    ISIF_START_WRITE
        /* ISIF_SINGLE */
} isif_start_t;

/*******************************************************************/
/**
 ISIF Device-data data-structure
 */
/******************************************************************/
typedef struct {

    uint8 opened;

} isif_dev_data_t;

/* ======================================================================= */
/**
 * ISIF_WHITE_BALANCE enumeration for White-balance mode of ISIF
 *
 * @param ISIF_WHITE_BALANCE_DISABLE    Disabled
 * @param ISIF_WHITE_BALANCE_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_WHITE_BALANCE_DISABLE = 0,
    ISIF_WHITE_BALANCE_ENABLE = 1
} ISIF_WHITE_BALANCE;

/* ======================================================================= */
/**
 * ISIF_OFFSET_CTRL enumeration for Offset-Control in ISIF
 *
 * @param ISIF_OFFSET_CTRL_DISABLE    Disabled
 * @param ISIF_OFFSET_CTRL_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_OFFSET_CTRL_DISABLE = 0,
    ISIF_OFFSET_CTRL_ENABLE = 1
} ISIF_OFFSET_CTRL;

/* ======================================================================= */
/**
 * ISIF_CFA_PATTERN enumeration for type of CFA pattern in ISIF
 *
 * @param ISIF_CFA_PATTERN_MOSAIC    Mosaic pattern
 * @param ISIF_CFA_PATTERN_STRIP      Strip pattern
 */
/* ======================================================================= */
typedef enum {
    ISIF_CFA_PATTERN_MOSAIC = 0,
    ISIF_CFA_PATTERN_STRIP = 1
} ISIF_CFA_PATTERN;

/* ======================================================================= */
/**
 * ISIF_CCDTBL_ALAW_ONOFF enumeration for ALAW compression enablement
 *
 * @param ISIF_CCDTBL_ALAW_DISABLE    Disabled
 * @param ISIF_CCDTBL_ALAW_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_CCDTBL_ALAW_DISABLE = 0,
    ISIF_CCDTBL_ALAW_ENABLE = 1
} ISIF_CCDTBL_ALAW_ONOFF;

/* ======================================================================= */
/**
 * ISIF_REC656IF_MODE enumeration for REC656 mode enablement
 *
 * @param ISIF_REC656IF_MODE_DISABLE    Disabled
 * @param ISIF_REC656IF_MODE_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_REC656IF_MODE_DISABLE = 0,
    ISIF_REC656IF_MODE_ENABLE = 1
} ISIF_REC656IF_MODE;

/* ======================================================================= */
/**
 * ISIF_ERR_CORRECTION_FVC enumeration for Error correction of FVH codes enablement
 *
 * @param ISIF_ERR_CORRECTION_FVC_DISABLE    Disabled
 * @param ISIF_ERR_CORRECTION_FVC_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_ERR_CORRECTION_FVC_DISABLE = 0,
    ISIF_ERR_CORRECTION_FVC_ENABLE = 1
} ISIF_ERR_CORRECTION_FVC;

/* ======================================================================= */
/**
 * ISIF_VLDC_RESYNC_VSYNC enumeration for control of CPU registers re-synchronize function by VSYNC
 *
 * @param ISIF_VLDC_RESYNC_VSYNC_DISABLE    Disabled
 * @param ISIF_VLDC_RESYNC_VSYNC_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_VLDC_RESYNC_VSYNC_DISABLE = 0,
    ISIF_VLDC_RESYNC_VSYNC_ENABLE = 1
} ISIF_VLDC_RESYNC_VSYNC;

/* ======================================================================= */
/**
 * ISIF_MSB_INVERSE_COUT enumeration for MSB inverse of COUT port enablement
 *
 * @param ISIF_MSB_INVERSE_COUT_DISABLE    Disabled
 * @param ISIF_MSB_INVERSE_COUT_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_MSB_INVERSE_COUT_DISABLE = 0,
    ISIF_MSB_INVERSE_COUT_ENABLE = 1
} ISIF_MSB_INVERSE_COUT;

/* ======================================================================= */
/**
 * ISIF_MSB_INVERSE_CIN enumeration for MSB inverse of CIN port
 *
 * @param ISIF_MSB_INVERSE_CIN_DISABLE    Disabled
 * @param ISIF_MSB_INVERSE_CIN_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_MSB_INVERSE_CIN_DISABLE = 0,
    ISIF_MSB_INVERSE_CIN_ENABLE = 1
} ISIF_MSB_INVERSE_CIN;

/* ======================================================================= */
/**
 * ISIF_BYTE_SWAP enumeration for control of  Byte SWAP function when SDRAM capturing
 *
 * @param ISIF_BYTE_SWAP_DISABLE    Disabled
 * @param ISIF_BYTE_SWAP_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_BYTE_SWAP_DISABLE = 0,
    ISIF_BYTE_SWAP_ENABLE = 1
} ISIF_BYTE_SWAP;

/* ======================================================================= */
/**
 * ISIF_SELECT_Y_POS enumeration for selecting Y signal position when in 8bit input mode
 *
 * @param ISIF_SELECT_Y_POS_EVEN_PX    Even pixel 
 * @param ISIF_SELECT_Y_POS_ODD_PX     Odd pixel 
 */
/* ======================================================================= */
typedef enum {
    ISIF_SELECT_Y_POS_EVEN_PX = 0,
    ISIF_SELECT_Y_POS_ODD_PX = 1
} ISIF_SELECT_Y_POS;

/* ======================================================================= */
/**
 * ISIF_EXTRG enumeration for enablement of trigger on rising edge. 
 *   Setting '1' to this register, the SDRAM address is initialized at the rising edge of FID input signal or DWEN register.
 *
 * @param ISIF_NO_EXTRG   		     Disabled
 * @param ISIF_EXTRG_RISING_EDGE      Enabled for rising edge on FID input/ DWEN register
 */
/* ======================================================================= */
typedef enum {
    ISIF_NO_EXTRG = 0,
    ISIF_EXTRG_RISING_EDGE = 1
} ISIF_EXTRG;

/* ======================================================================= */
/**
 * ISIF_SDRAM_TRGSEL enumeration for selecting trigger source signal of SDRAM address initializing in case EXTRG=1
 *
 * @param ISIF_SDRAM_TRGSEL_DWEN_REG    DWEN register selected
 * @param ISIF_SDRAM_TRGSEL_FID_IN     	FID input signal selected
 */
/* ======================================================================= */
typedef enum {
    ISIF_SDRAM_TRGSEL_DWEN_REG = 0,
    ISIF_SDRAM_TRGSEL_FID_IN = 1
} ISIF_SDRAM_TRGSEL;

/* ======================================================================= */
/**
 * ISIF_WENLOG_SIGNAL enumeration for defining CCD valid area
 *
 * @param ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL    internal valid signal and WEN signal is ANDed logically
 * @param ISIF_WENLOG_SIGNAL_LOGICAL_OR_INTERNAL     internal valid signal and WEN signal is ORed logically. 
 */
/* ======================================================================= */
typedef enum {
    ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL = 0,
    ISIF_WENLOG_SIGNAL_LOGICAL_OR_INTERNAL = 1
} ISIF_WENLOG_SIGNAL;

/* ======================================================================= */
/**
 * ISIF_FID_DETECTION_MODE enumeration for selecting FID detection mode
 *
 * @param ISIF_FID_LATCH_VSYNC    latch the FID at the VSYNC timing 
 * @param ISIF_FID_NO_LATCH          no latch the FID 
 */
/* ======================================================================= */
typedef enum {
    ISIF_FID_LATCH_VSYNC = 0,
    ISIF_FID_NO_LATCH = 1
} ISIF_FID_DETECTION_MODE;

/* ======================================================================= */
/**
 * ISIF_BW656 enumeration for selecting bit width of CCIR656
 *
 * @param ISIF_BW656_DISABLE    Disabled
 * @param ISIF_BW656_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_BW656_DISABLE = 0,
    ISIF_BW656_ENABLE = 1
} ISIF_BW656;

/* ======================================================================= */
/**
 * ISIF_YCINSWAP enumeration for enabling the swapping of Y-IN signal and C-IN  signal
 *
 * @param ISIF_YCINSWAP_DISABLE    Disabled
 * @param ISIF_YCINSWAP_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_YCINSWAP_DISABLE = 0,
    ISIF_YCINSWAP_ENABLE = 1
} ISIF_YCINSWAP;

/* ======================================================================= */
/**
 * ISIF_YCOUTSWAP enumeration for enabling the swapping of Y-OUT signal and C-OUT signal
 *
 * @param ISIF_YCOUTSWAP_DISABLE    Disabled
 * @param ISIF_YCOUTSWAP_ENABLE      Enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_YCOUTSWAP_DISABLE = 0,
    ISIF_YCOUTSWAP_ENABLE = 1
} ISIF_YCOUTSWAP;

/* ======================================================================= */
/**
 * ISIF_SDRAM_PACK enumeration for packing modes for SDRAM storage
 *
 * @param ISIF_SDRAM_PACK_16BITS    16-bit packing
 * @param ISIF_SDRAM_PACK_12BITS    12-bit packing
 * @param ISIF_SDRAM_PACK_8BITS      8-bit packing
 */
/* ======================================================================= */
typedef enum {
    ISIF_SDRAM_PACK_16BITS = 0,
    ISIF_SDRAM_PACK_12BITS = 1,
    ISIF_SDRAM_PACK_8BITS = 2
} ISIF_SDRAM_PACK;

/*******************************************************************/
/**
 * ISIF Color-space-conversion matrix (4X4) data-structure
 */
/*******************************************************************/
typedef struct {
    uint8 csc_coef_m00;                                    // CSCM00 CSCM0
    uint8 csc_coef_m01;                                    // CSCM01 CSCM0
    uint8 csc_coef_m02;                                    // CSCM02 CSCM0
    uint8 csc_coef_m03;                                    // CSCM03 CSCM0
    uint8 csc_coef_m10;                                    // CSCM10 CSCM0
    uint8 csc_coef_m11;                                    // CSCM11 CSCM0
    uint8 csc_coef_m12;                                    // CSCM12 CSCM0
    uint8 csc_coef_m13;                                    // CSCM13 CSCM0
    uint8 csc_coef_m20;                                    // CSCM20 CSCM0
    uint8 csc_coef_m21;                                    // CSCM21 CSCM0
    uint8 csc_coef_m22;                                    // CSCM22 CSCM0
    uint8 csc_coef_m23;                                    // CSCM23 CSCM0
    uint8 csc_coef_m30;                                    // CSCM30 CSCM0
    uint8 csc_coef_m31;                                    // CSCM31 CSCM0
    uint8 csc_coef_m32;                                    // CSCM32 CSCM0
    uint8 csc_coef_m33;                                    // CSCM33 CSCM0
} isif_csc_coef_mat_t;

/*******************************************************************/
/**
 * ISIF Color-space-conversion configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* Enable the CSC mode */
    uint8 enable;

    /* CSC coefficient matrix */
    isif_csc_coef_mat_t *csc_coef;
} isif_csc_cfg_t;

typedef uint8 isif_ipfmt_pgm_addr_ptr_select_t;

/* ======================================================================= */
/**
 * ISIF_SCAN_MODE enumeration for scan mode of the ISIF (continuous/alternate  lines)
 *
 * @param ISIF_NORMAL_SCAN_MODE    Normal scan mode selected
 * @param ISIF_LINE_ALTERNATIVE_SCAN_MODE    Line-alternate scan mode selected
 */
/* ======================================================================= */
typedef enum {
    ISIF_NORMAL_SCAN_MODE = 0,
    ISIF_LINE_ALTERNATIVE_SCAN_MODE = 1
} ISIF_SCAN_MODE;

/* ======================================================================= */
/* ISIF_IPFMT_COMBINE_SPLIT enumeration for split/combine mode of the input
 * data lines @param ISIF_INPUT_SPLIT split the input line into multiple
 * output lines @param ISIF_INPUT_COMBINE combine multiple input lines into a 
 * single output line */
/* ======================================================================= */
typedef enum {
    ISIF_INPUT_SPLIT = 0,
    ISIF_INPUT_COMBINE = 1
} ISIF_IPFMT_COMBINE_SPLIT;

/* ======================================================================= */
/* ISIF_CCD_IPFMT enumeration for input format of the CCD @param
 * ISIF_CCD_FORMAT_DISABLE disabled @param ISIF_CCD_FORMAT_ENABLE enabled */
/* ======================================================================= */
typedef enum {
    ISIF_CCD_FORMAT_DISABLE = 0,
    ISIF_CCD_FORMAT_ENABLE = 1
} ISIF_CCD_IPFMT;

/*******************************************************************/
/**
 * ISIF INput format Line-initialization data-structure
 */
/*******************************************************************/
typedef struct {

    /* Output Line address */
    uint8 output_line_addr;

    /* initialization address */
    uint16 init_addr;
} isif_ipfmt_line_init_t;

/*******************************************************************/
/**
 * ISIF Input format configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* FMTEN FMTCFG--input format of the CCD */
    ISIF_CCD_IPFMT enable;

    /* FMTCBL FMTCFG--split/combine mode of the input data line */
    ISIF_IPFMT_COMBINE_SPLIT combine_enable;

    /* LNALT FMTCFG--scan mode of the ISIF */
    ISIF_SCAN_MODE line_alternate;

    /* LNUM FMTCFG--split/combine the input lines to generate output lines */
    uint8 split_combine_line;

    /* FMTAINC FMTCFG-- Address increment value */
    uint8 address_increment_value;

    /* FMTPLEN0 FMTPLEN--Number of Set0 program entries */
    uint8 pgm_entry_length_set0;

    /* FMTPLEN1 FMTPLEN--Number of Set1 program entries */
    uint8 pgm_entry_length_set1;

    /* FMTPLEN2 FMTPLEN--Number of Set2 program entries */
    uint8 pgm_entry_length_set2;

    /* FMTPLEN3 FMTPLEN--Number of Set3 program entries */
    uint8 pgm_entry_length_set3;

    /* H-position of the first pixel in an input line fed into the formatter */
    uint16 h_pos;

    /* Number of pixels in a line fed to the formatter Number of pixels =
     * FMTLNH + 1 */
    uint16 h_size;

    /* Start Line vertical */
    uint16 v_pos;

    /* Number of lines in vertical; Number of lines = FMTLNV + 1 */
    uint16 v_size;

    /* Number of pixels in an output line */
    uint16 op_pixel_length;

    /* HD interval for output lines */
    uint16 output_hd_interval_split;

    /* FMTAPTR used from FMTAPTR 0 to fmt_aptrs, max 16 */
    uint8 fmt_aptrs;

    /* Input Data Formatter Register pointer :FMTAPTR0 - FMTAPTR15 */
    isif_ipfmt_line_init_t *line_init;

    /* PGM15EN-PGM0EN FMTPGMVF0 */
    uint16 pgm_addr_enable_bit_map0;

    /* PGM31EN-PGM16EN FMTPGMVF1 */
    uint16 pgm_addr_enable_bit_map1;

    /* PGM15UPDT-PGM0UPDT FMTPGMAPU0 */
    uint16 pgm_addr_update_bit_map0;

    /* PGM31UPDT-PGM0UPDT FMTPGMAPU1 */
    uint16 pgm_addr_update_bit_map1;
    /* uint8 pgm_aptrs; no of pgm address ptrs used 0 to 32 */

    /* Program address pointer select pointer: PGMXXAPTR - FMTPGMAPS7
     * FMTPGMAPS0 must be array of 32 */
    isif_ipfmt_pgm_addr_ptr_select_t *pgm_addr_ptr_sel;

} isif_ip_format_cfg_t;

/* ======================================================================= */
/**
 * ISIF_DATA_INPUT_MODE enumeration for input data format of the ISIF
 *
 * @param ISIF_DATA_INPUT_MODE_CCD_RAW   RAW format
 * @param ISIF_DATA_INPUT_MODE_YUV16    YUV16 format
 * @param ISIF_DATA_INPUT_MODE_YUV8    YUV8 format
 */
/* ======================================================================= */
typedef enum {
    ISIF_DATA_INPUT_MODE_CCD_RAW = 0,
    ISIF_DATA_INPUT_MODE_YUV16 = 1,
    ISIF_DATA_INPUT_MODE_YUV8 = 2
} ISIF_DATA_INPUT_MODE;

/* ======================================================================= */
/**
 * ISIF_DATA_POLARITY enumeration for data polarity
 *
 * @param ISIF_DATA_NORMAL   normal format of data 
 * @param ISIF_DATA_ONES_COMPLIMENT    one's complement of data
 */
/* ======================================================================= */
typedef enum {
    ISIF_DATA_NORMAL = 0,
    ISIF_DATA_ONES_COMPLIMENT = 1
} ISIF_DATA_POLARITY;

/* ======================================================================= */
/**
 * ISIF_CCD_MODE enumeration for selecting CCD mode
 *
 * @param ISIF_CCD_MODE_NON_INTERLACED   non-interlaced format of data from CCD
 * @param ISIF_CCD_MODE_INTERLACED           interlaced format of data from CCD
 */
/* ======================================================================= */
typedef enum {
    ISIF_CCD_MODE_NON_INTERLACED = 0,
    ISIF_CCD_MODE_INTERLACED = 1
} ISIF_CCD_MODE;

/* ======================================================================= */
/**
 * ISIF_WEN enumeration for enablement of the Write-enable signal
 *
 * @param ISIF_WEN_DISABLE  disabled
 * @param ISIF_WEN_ENABLE    enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_WEN_ENABLE = 1,
    ISIF_WEN_DISABLE = 0
} ISIF_WEN;

/* ======================================================================= */
/**
 * ISIF_POLARITY enumeration for polarity of the different signals within ISIF such as HD/VD/FI.
 *
 * @param ISIF_POSITIVE_POLARITY  Positive polarity
 * @param ISIF_NEGATIVE_POLARITY  Negative polarity
 */
/* ======================================================================= */
typedef enum {
    ISIF_POSITIVE_POLARITY = 0,
    ISIF_NEGATIVE_POLARITY = 1
} ISIF_POLARITY;

/**
 * ======================================================================= */
/* ISIF_SIGNAL_DIRECTION enumeration for direction of different signals
 * within ISIF. @param ISIF_SIGNAL_DIRECTION_INPUT signal is input @param
 * ISIF_SIGNAL_DIRECTION_OUTPUT signal is output */
/* ======================================================================= */
typedef enum {
    ISIF_SIGNAL_DIRECTION_INPUT = 0,
    ISIF_SIGNAL_DIRECTION_OUTPUT = 1
} ISIF_SIGNAL_DIRECTION;

/* ======================================================================= */
/**
 * ISIF_HLINE_ADDR enumeration for hline address increment/decrement
 *
 * @param ISIF_HLINE_ADDR_INCREMENT  increment
 * @param ISIF_HLINE_ADDR_DECREMENT  decrement
 */
/* ======================================================================= */
typedef enum {
    ISIF_HLINE_ADDR_INCREMENT = 0,
    ISIF_HLINE_ADDR_DECREMENT = 1
} ISIF_HLINE_ADDR;

/* ======================================================================= */
/**
 * ISIF_LINE_OFFSET_VALUE enumeration for defining line offsets
 *
 * @param ISIF_LINE_OFFSET_PLUS_1  increment by 1 
 * @param ISIF_LINE_OFFSET_PLUS_2  increment by 2 
 * @param ISIF_LINE_OFFSET_PLUS_3  increment by 3 
 * @param ISIF_LINE_OFFSET_PLUS_4  increment by 4 
 * @param ISIF_LINE_OFFSET_MINUS_1  decrement  by 1 
 * @param ISIF_LINE_OFFSET_MINUS_2  decrement  by 2 
 * @param ISIF_LINE_OFFSET_MINUS_3  decrement  by 3 
 * @param ISIF_LINE_OFFSET_MINUS_4  decrement  by 4 
 */
/* ======================================================================= */
typedef enum {
    ISIF_LINE_OFFSET_PLUS_1 = 0,
    ISIF_LINE_OFFSET_PLUS_2 = 1,
    ISIF_LINE_OFFSET_PLUS_3 = 2,
    ISIF_LINE_OFFSET_PLUS_4 = 3,
    ISIF_LINE_OFFSET_MINUS_1 = 4,
    ISIF_LINE_OFFSET_MINUS_2 = 5,
    ISIF_LINE_OFFSET_MINUS_3 = 6,
    ISIF_LINE_OFFSET_MINUS_4 = 7
} ISIF_LINE_OFFSET_VALUE;

/* ======================================================================= */
/**
 * ISIF_FIELD_LINE_OFFSET_VALUE enumeration for field line address increment
 *
 * @param ISIF_FIELD_LINE_OFFSET_PLUS_1  increment by 1 
 * @param ISIF_FIELD_LINE_OFFSET_PLUS_2  increment by 2
 * @param ISIF_FIELD_LINE_OFFSET_PLUS_3  increment by 3
 * @param ISIF_FIELD_LINE_OFFSET_PLUS_4  increment by 4
 */
/* ======================================================================= */
typedef enum {
    ISIF_FIELD_LINE_OFFSET_PLUS_1 = 0,
    ISIF_FIELD_LINE_OFFSET_PLUS_2 = 1,
    ISIF_FIELD_LINE_OFFSET_PLUS_3 = 2,
    ISIF_FIELD_LINE_OFFSET_PLUS_4 = 3
} ISIF_FIELD_LINE_OFFSET_VALUE;

/* ======================================================================= */
/**
 * ISIF_FID_POLARITY enumeration for FID polarity 
 *
 * @param ISIF_FID_NO_CHANGE  No change of polarity
 * @param ISIF_FID_INVERSE_FID  Polarity is reversed
 */
/* ======================================================================= */
typedef enum {
    ISIF_FID_NO_CHANGE = 0,
    ISIF_FID_INVERSE_FID = 1
} ISIF_FID_POLARITY;

/*******************************************************************/
/**
 * ISIF Field ID Line-offset configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* Field ID polarity */
    ISIF_FID_POLARITY fid_polarity;

    /* Field line offset value in odd(FID = 1) field */
    ISIF_FIELD_LINE_OFFSET_VALUE odd_field_line_offset;

    /* Field line offset value in even line, even field */
    ISIF_LINE_OFFSET_VALUE even_line_even_field_offset;

    /* Field line offset value in odd line, even field */
    ISIF_LINE_OFFSET_VALUE odd_line_even_field_offset;

    /* Field line offset value in even line, odd field */
    ISIF_LINE_OFFSET_VALUE even_line_odd_field_offset;

    /* Field line offset value in odd line, odd field */
    ISIF_LINE_OFFSET_VALUE odd_line_odd_field_offset;
} isif_fid_line_offset_ctg_t;

/* ======================================================================= */
/**
 * ISIF_GAIN_MSB_POS enumeration for defining the MSB bit in the input data
 *
 * @param ISIF_GAIN_MSB_BIT15  position 15
 * @param ISIF_GAIN_MSB_BIT14  position 14
 * @param ISIF_GAIN_MSB_BIT13  position 13
 * @param ISIF_GAIN_MSB_BIT12  position 12
 * @param ISIF_GAIN_MSB_BIT11  position 11
 * @param ISIF_GAIN_MSB_BIT10  position 10
 * @param ISIF_GAIN_MSB_BIT9  position 9
 * @param ISIF_GAIN_MSB_BIT8  position 8
 * @param ISIF_GAIN_MSB_BIT7  position 7
 */
/* ======================================================================= */
typedef enum {
    ISIF_GAIN_MSB_BIT15 = 0,
    ISIF_GAIN_MSB_BIT14 = 1,
    ISIF_GAIN_MSB_BIT13 = 2,
    ISIF_GAIN_MSB_BIT12 = 3,
    ISIF_GAIN_MSB_BIT11 = 4,
    ISIF_GAIN_MSB_BIT10 = 5,
    ISIF_GAIN_MSB_BIT9 = 6,
    ISIF_GAIN_MSB_BIT8 = 7,
    ISIF_GAIN_MSB_BIT7 = 8
} ISIF_GAIN_MSB_POS;

/* ======================================================================= */
/**
 * ISIF_COLOR_PATTERN_POSITION enumeration for color pattern position
 *
 * @param ISIF_COLOR_POSITION_R_YE  R / Ye 
 * @param ISIF_COLOR_POSITION_GR_CY  Gr / Cy 
 * @param ISIF_COLOR_POSITION_GR_G  Gb / G 
 * @param ISIF_COLOR_POSITION_B_MG  B / Mg 
 */
/* ======================================================================= */
typedef enum {
    ISIF_COLOR_POSITION_R_YE = 0,
    ISIF_COLOR_POSITION_GR_CY = 1,
    ISIF_COLOR_POSITION_GR_G = 2,
    ISIF_COLOR_POSITION_B_MG = 3
} ISIF_COLOR_PATTERN_POSITION;

/* ======================================================================= */
/**
 * ISIF_DPCM_ENCODE_PREDICTOR_TYPE enumeration for DPCM predictor 
 *
 * @param ISIF_DPCM_ENCODE_PREDICTOR1  type-1 predictor
 * @param ISIF_DPCM_ENCODE_PREDICTOR2  type-1 predictor
 */
/* ======================================================================= */
typedef enum {
    ISIF_DPCM_ENCODE_PREDICTOR1 = 0,
    ISIF_DPCM_ENCODE_PREDICTOR2 = 1
} ISIF_DPCM_ENCODE_PREDICTOR_TYPE;

typedef enum {
    ISIF_DPCM_ENCODER_ENABLE_OFF,
    ISIF_DPCM_ENCODER_ENABLE_ON
} ISIF_DPCM_ENCODER_ENABLE_TYPE;

/* ======================================================================= */
/**
 * ISIF_HPLF enumeration for anti-aliasing enablement
 *
 * @param ISIF_LPF_ANTI_ALIAS_DISABLE  disabled
 * @param ISIF_LPF_ANTI_ALIAS_ENABLE  enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_LPF_ANTI_ALIAS_DISABLE = 0,
    ISIF_LPF_ANTI_ALIAS_ENABLE = 1
} ISIF_HPLF;

/* ======================================================================= */
/**
 * ISIF_CCD_DATA_SHIFT_SDRAM enumeration for SDRAM data storage
 *
 * @param ISIF_CCD_DATA_NO_SHIFT  no shift 
 * @param ISIF_CCD_DATA_RIGHT_SHIFT_1  shift 1 towards right
 * @param ISIF_CCD_DATA_RIGHT_SHIFT_2  shift 2 towards right
 * @param ISIF_CCD_DATA_RIGHT_SHIFT_3  shift 3 towards right
 * @param ISIF_CCD_DATA_RIGHT_SHIFT_4  shift 4 towards right
 */
/* ======================================================================= */
typedef enum {
    ISIF_CCD_DATA_NO_SHIFT = 0,
    ISIF_CCD_DATA_RIGHT_SHIFT_1 = 1,
    ISIF_CCD_DATA_RIGHT_SHIFT_2 = 2,
    ISIF_CCD_DATA_RIGHT_SHIFT_3 = 3,
    ISIF_CCD_DATA_RIGHT_SHIFT_4 = 4
} ISIF_CCD_DATA_SHIFT_SDRAM;

/* ======================================================================= */
/**
 * ISIF_DATA_WRITE_START enumeration enablement of data write
 *
 * @param ISIF_DATA_WRITE_DISABLE disabled   
 * @param ISIF_DATA_WRITE_ENABLE  enabled 
 */
/* ======================================================================= */
typedef enum {
    ISIF_DATA_WRITE_DISABLE = 0,
    ISIF_DATA_WRITE_ENABLE = 1
} ISIF_DATA_WRITE_START;

/* ======================================================================= */
/**
 * ISIF_SWEN_MODE enumeration for selection of external WEN signal 
 *
 * @param ISIF_NO_WEN  WEN not used 
 * @param ISIF_EXTERNAL_WEN  external WEN selected
 */
/* ======================================================================= */
typedef enum {
    ISIF_NO_WEN = 0,
    ISIF_EXTERNAL_WEN = 1
} ISIF_SWEN_MODE;

/* ======================================================================= */
/**
 * ISIF_SYEN_START enumeration for Controls ON/OFF of VD/HD output
 *
 * @param ISIF_HD_VD_DISABLE  disabled
 * @param ISIF_HD_VD_ENABLE  enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_HD_VD_DISABLE = 0,
    ISIF_HD_VD_ENABLE = 1
} ISIF_SYEN_START;

typedef struct {
    uint16 hd_width;
    uint16 vd_width;
    uint16 ppln_hs_interval;
    uint16 lpfr_vs_interval;

} isif_input_sizes_t;

/*******************************************************************/
/**
 * ISIF Input source configuration data-structure
 */
 /*******************************************************************/
typedef struct {
    /* SYEN of SYNCEN--Controls ON/OFF of VD/HD output */
    ISIF_SYEN_START sync_enable;

    /* INPMOD of MODESET--Data input format */
    ISIF_DATA_INPUT_MODE ip_pix_fmt;

    /* DPOL of MODESET */
    ISIF_DATA_POLARITY data_polarity;

    /* FIPOL of MODESET */
    ISIF_POLARITY field_pol;

    /* HDPOL of MODESET */
    ISIF_POLARITY hd_pol;

    /* VDPOL of MODESET */
    ISIF_POLARITY vd_pol;

    /* FIDD of MODESET */
    ISIF_SIGNAL_DIRECTION field_dir;

    /* HDVDD of MODESET */
    ISIF_SIGNAL_DIRECTION hd_vd_dir;

    /* HD Width */
    uint16 hd_width;

    /* VD Width */
    uint16 vd_width;

    /* PPLN--Pixels per Line */
    uint16 ppln_hs_interval;

    /* LPFR--lines per frame */
    uint16 lpfr_vs_interval;

    /* YCINSWP-- Swapping of YIN and CIN */
    ISIF_YCINSWAP yc_in_swap;

    /* MSBINVI--MSB inverse of CIN port */
    ISIF_MSB_INVERSE_CIN msb_inverse_cin;

    /* GWDI--MSB position of input data */
    ISIF_GAIN_MSB_POS ip_data_msb_pos;

    /* Color pattern position array for field-0 */
    ISIF_COLOR_PATTERN_POSITION field1_pixel_pos[4];

    /* Color pattern position array for field-1 */
    ISIF_COLOR_PATTERN_POSITION field0_pixel_pos[4];

    /* CFAP--CFA pattern */
    ISIF_CFA_PATTERN cfa_pattern;

    /* R656ON--r656_inferface_mode */
    ISIF_REC656IF_MODE r656_inferface_mode;

    /* ECCFVH--FVH Error Correction Code */
    ISIF_ERR_CORRECTION_FVC err_correction_mode;

    /* BW656--bit width */
    ISIF_BW656 ccir656_bit_width;
} isif_ip_src_cfg_t;

/*******************************************************************/
/**
 * ISIF Culling configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* CLHO OF CULH --horizontal odd culling */
    uint8 horizontal_odd_culling;

    /* CLHE OF CULH--horizontal even culling */
    uint8 horizontal_even_culling;

    /* CULV--vertical culling */
    uint8 vertical_culling;
} isif_culling_cfg_t;

/*******************************************************************/
/**
 * ISIF SDRAM output configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* first pixel in line SPH */
    uint16 sdram_hpos;

    /* no of pixels in line = LNH+1 */
    uint16 sdram_hsize;

    /* wrt to VD,output to SDRAM, SLV0 field 0 */
    uint16 sdram_vpos0;

    /* wrt to VD,SLV1 field 1 */
    uint16 sdram_vpos1;

    /* no of line = LNV+1 */
    uint16 sdram_vsize;

    /* ADCR--memory address decrement/increment */
    ISIF_HLINE_ADDR memory_addr_decrement;

    /* HSIZE--address offset for increment/decrement */
    uint16 memory_addr_offset;

    /* SDOFST--field offset parameters */
    isif_fid_line_offset_ctg_t field_offset_params;

    /* (CADU+CADL)<<5 32byte aligned */
    uint32 memory_addr;

    /* HLPF of MODESET */
    ISIF_HPLF alias_filter_enable;

    /* DPCMPRE MISC */
    ISIF_DPCM_ENCODE_PREDICTOR_TYPE dpcm_predictor;

    /* DPCM ENABLE MISC */
    ISIF_DPCM_ENCODER_ENABLE_TYPE dpcm_enable;

    /* CCDW of MODESET */
    ISIF_CCD_DATA_SHIFT_SDRAM ccd_raw_shift_value;

    /* CCDMD of MODESET */
    ISIF_CCD_MODE ccd_field_mode;

    /* SWEN of MODESET */
    ISIF_SWEN_MODE wen_selection_mode;

    /* DWEN of SYNCEN */
    ISIF_DATA_WRITE_START data_write_enable;

    /* CCDCFG MSBINVO */
    ISIF_MSB_INVERSE_COUT msb_inverse_cout;

    /* CCDCFG BSWD */
    ISIF_BYTE_SWAP sdram_byte_swap;

    /* CCDCFG Y8POS */
    ISIF_SELECT_Y_POS y_position;

    /* CCDCFG EXTRG */
    ISIF_EXTRG sdram_ext_trig;

    /* CCDCFG TRGSEL */
    ISIF_SDRAM_TRGSEL sdram_trig_select;

    /* CCDCFG WENLOG */
    ISIF_WENLOG_SIGNAL ccd_valid_log;

    /* CCDCFG FIDMD */
    ISIF_FID_DETECTION_MODE fid_detection_mode;

    /* CCDCFG YCOUTSWP */
    ISIF_YCOUTSWAP yc_out_swap;

    /* CCDCFG SDRPACK */
    ISIF_SDRAM_PACK sdram_pack_fmt;

    /* CCDCFG VLDC shadow reg or busy update?? */
    ISIF_VLDC_RESYNC_VSYNC vsync_enable;

    /* Programmable H/V culling pattern */
    isif_culling_cfg_t culling_params;
} isif_sdram_op_cfg_t;

/*******************************************************************/
/**
 * ISIF crop dimensions data-structure
 */
/*******************************************************************/
typedef struct {
    /* horizontal number of pixels */
    uint16 hnum;

    /* number of lines */
    uint16 vnum;
} isif_crop_dims;

/* ======================================================================= */
/**
 * ISIF_CSC enumeration for color-space-conversion enablement
 *
 * @param ISIF_CSC_DISABLE  disabled
 * @param ISIF_CSC_ENABLE  enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_CSC_ENABLE = 1,
    ISIF_CSC_DISABLE = 0
} ISIF_CSC;

/* ======================================================================= */
/**
 * ISIF_LINEAR_SHIFT_VALUE enumeration for Linearization left shift values
 *
 * @param ISIF_LINEAR_NO_SHIFT_VALUE  no shift
 * @param ISIF_LINEAR_1_LEFT_SHIFT_VALUE  shift by 1
 * @param ISIF_LINEAR_2_LEFT_SHIFT_VALUE  shift by 2
 * @param ISIF_LINEAR_3_LEFT_SHIFT_VALUE  shift by 3
 * @param ISIF_LINEAR_4_LEFT_SHIFT_VALUE  shift by 4
 * @param ISIF_LINEAR_5_LEFT_SHIFT_VALUE  shift by 5
 * @param ISIF_LINEAR_6_LEFT_SHIFT_VALUE  shift by 6
 */
/* ======================================================================= */
typedef enum {
    ISIF_LINEAR_NO_SHIFT_VALUE = 0,
    ISIF_LINEAR_1_LEFT_SHIFT_VALUE = 1,
    ISIF_LINEAR_2_LEFT_SHIFT_VALUE = 2,
    ISIF_LINEAR_3_LEFT_SHIFT_VALUE = 3,
    ISIF_LINEAR_4_LEFT_SHIFT_VALUE = 4,
    ISIF_LINEAR_5_LEFT_SHIFT_VALUE = 5,
    ISIF_LINEAR_6_LEFT_SHIFT_VALUE = 6
} ISIF_LINEAR_SHIFT_VALUE;

/* ======================================================================= */
/**
 * ISIF_LINEAR_SAMPLING enumeration for linear sampling format
 *
 * @param ISIF_LINEAR_UNIFORM_SAMPLING  Uniform sampling
 * @param ISIF_LINEAR_NON_UNIFORM_SAMPLING  Non-uniform sampling
 */
/* ======================================================================= */
typedef enum {
    ISIF_LINEAR_UNIFORM_SAMPLING = 0,
    ISIF_LINEAR_NON_UNIFORM_SAMPLING = 1
} ISIF_LINEAR_SAMPLING;

/*******************************************************************/
/**
 * ISIF Sensor Linear configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* Linearization enabled */
    uint8 enable;

    /* COrrection shift values */
    ISIF_LINEAR_SHIFT_VALUE correction_shift;

    /* Linear Sampling method */
    ISIF_LINEAR_SAMPLING non_uniform_mode;

    /* Scale factor (U11Q10) for LUT input */
    uint16 scale_value;

} isif_sensor_linear_cfg_t;

/*******************************************************************/
/**
 * ISIF Gain-offset configuration data-structure
 */
/********************************************************************/
typedef struct {
    /* gain offset feature-flag */
    uint8 gain_offset_featureflag;

    /* CGR CRGAIN-- gain R */
    uint16 gain_r;

    /* CGGR CGRGAIN-- gain GR */
    uint16 gain_gr;

    /* CGGB CGBGAIN-- gain GB */
    uint16 gain_gb;

    /* CGB CBGAIN-- gain B */
    uint16 gain_bg;

    /* COFT COFSTA--offset */
    uint16 offset;
} isif_gain_offset_cfg_t;

#define ISIF_H3A_WHITE_BALANCE_FLAG     (1<<0)
#define ISIF_IPIPE_WHITE_BALANCE_FLAG   (1<<1)
#define ISIF_SDRAM_WHITE_BALANCE_FLAG   (1<<2)
#define ISIF_H3A_OFFSET_CTRL_FLAG       (1<<3)
#define ISIF_IPIPE_OFFSET_CTRL_FLAG     (1<<4)
#define ISIF_SDRAM_OFFSET_CTRL_FLAG     (1<<5)

/* 
 * ISIF_WHITE_BALANCE h3a_white_balance_enable; //CGAMMAWD WBEN2
 * ISIF_WHITE_BALANCE ipipe_white_balance_enable; //CGAMMAWD WBEN1
 * ISIF_WHITE_BALANCE sdram_white_balance_enable; //CGAMMAWD WBEN0
 * ISIF_OFFSET_CTRL h3a_offset_enable; //CGAMMAWD OFSTEN2 ISIF_OFFSET_CTRL
 * ipipe_offset_enable; //CGAMMAWD OFSTEN1 ISIF_OFFSET_CTRL
 * sdram_offset_enable; //CGAMMAWD OFSTEN0 */

/* ======================================================================= */
/**
 * ISIF_DPCM enumeration for enablement of DPCM
 *
 * @param ISIF_DPCM_DISABLE  disabled 
 * @param ISIF_DPCM_ENABLE  enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_DPCM_DISABLE = 0,
    ISIF_DPCM_ENABLE = 1
} ISIF_DPCM;

/* ======================================================================= */
/**
 * ISIF_VDLC_PIXEL_DEPENDENCY enumeration for pixel dependency
 *
 * @param ISIF_VDLC_WHOLE_LINE  The whole line is corrected.  
 * @param ISIF_VDLC_DISABLE_ABOVE_UPPER_PIXEL  Pixels upper than the defect are not corrected. 
 */
/* ======================================================================= */
typedef enum {
    ISIF_VDLC_WHOLE_LINE = 0,
    ISIF_VDLC_DISABLE_ABOVE_UPPER_PIXEL = 1
} ISIF_VDLC_PIXEL_DEPENDENCY;

/* ======================================================================= */
/**
 * ISIF_VLDC_ONOFF enumeration for enablement of VLDC
 *
 * @param ISIF_VLDC_DISABLE  disabled 
 * @param ISIF_VLDC_ENABLE  enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_VLDC_DISABLE = 0,
    ISIF_VLDC_ENABLE = 1
} ISIF_VLDC_ONOFF;

/* ======================================================================= */
/**
 * ISIF_VLDC_MODE_SELECT enumeration for mode select of VLDC
 *
 * @param ISIF_VLDC_FED_THRO_ONSATURATION  Defect level subtraction. Just fed through if data are saturating. 
 * @param ISIF_VLDC_HORIZONTAL_INTERPOLATION_ONSATURATION  Defect level subtraction. Horizontal interpolation ((i-2)+(i+2))/2 if data are saturating.  
 * @param ISIF_VLDC_HORIZONTAL_INTERPOLATION  Horizontal interpolation ((i-2)+(i+2))/2.  
 */
/* ======================================================================= */
typedef enum {
    ISIF_VLDC_FED_THRO_ONSATURATION = 0,
    ISIF_VLDC_HORIZONTAL_INTERPOLATION_ONSATURATION = 1,
    ISIF_VLDC_HORIZONTAL_INTERPOLATION = 2
} ISIF_VLDC_MODE_SELECT;

/*******************************************************************/
/**
 * ISIF Vertical Line Defect parameters data-structure
 */
/*******************************************************************/
typedef struct {

    /* DFCMEM0--Vertical Defect position */
    uint16 vertical_defect_position;

    /* DFCMEM1 12-0--horizontal defect position */
    uint16 horizontal_defect_position;

    /* DFCMEM2--Defect correction Memory 2 */
    uint8 sub1_value_vldc;

    /* DFCMEM3--Defect correction Memory 3 */
    uint8 sub2_less_than_vldc;

    /* DFCMEM4--Defect correction Memory 4 */
    uint8 sub3_greater_than_vldc;
} isif_vldc_defect_line_params_t;

/*******************************************************************/
/**
 * ISIF Vertical Line Defect Correction configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* VDFCEN DFCCTL--enable VLDC module */
    uint8 enable;

    /* VDFCUDA DFCCTL--pixel dependency */
    ISIF_VDLC_PIXEL_DEPENDENCY disable_vldc_upper_pixels;

    /* VDFLSFT DFCCTL-- VLDC shift values */
    uint8 vldc_shift_value;

    /* VDFCSL DFCCTL--VLDC mode select */
    ISIF_VLDC_MODE_SELECT vldc_mode_select;

    /* VDFSLV VDFSATLV uint12 range 0 - 4095-- VLDC saturation level */
    uint16 vldc_saturation_level;

    /* number of defect lines-maximum8 */
    uint8 defect_lines;

    /* DFCMEM0 -8-- defect line paramaters */
    isif_vldc_defect_line_params_t *vldc_defect_line_params;

} isif_vldc_cfg_t;

/* ======================================================================= */
/**
 * ISIF_LSC_OFFSET_SHIFT enumeration for LSC offset shift
 *
 * @param ISIF_LSC_OFFSET_NO_SHIFT   no shift
 * @param ISIF_LSC_OFFSET_1_LEFT_SHIFT  shift left by 1 
 * @param ISIF_LSC_OFFSET_2_LEFT_SHIFT  shift left by 2
 * @param ISIF_LSC_OFFSET_3_LEFT_SHIFT  shift left by 3
 * @param ISIF_LSC_OFFSET_4_LEFT_SHIFT  shift left by 4
 * @param ISIF_LSC_OFFSET_5_LEFT_SHIFT  shift left by 5
 */
/* ======================================================================= */
typedef enum {
    ISIF_LSC_OFFSET_NO_SHIFT = 0,
    ISIF_LSC_OFFSET_1_LEFT_SHIFT = 1,
    ISIF_LSC_OFFSET_2_LEFT_SHIFT = 2,
    ISIF_LSC_OFFSET_3_LEFT_SHIFT = 3,
    ISIF_LSC_OFFSET_4_LEFT_SHIFT = 4,
    ISIF_LSC_OFFSET_5_LEFT_SHIFT = 5
} ISIF_LSC_OFFSET_SHIFT;

/* ======================================================================= */
/**
 * ISIF_LSC_OFFSET_ENABLE enumeration for enablement of LSC offset
 *
 * @param ISIF_LSC_OFFSET_OFF  disabled 
 * @param ISIF_LSC_OFFSET_ON  enabled
 */
/* ======================================================================= */
typedef enum {
    ISIF_LSC_OFFSET_OFF = 0,
    ISIF_LSC_OFFSET_ON = 1
} ISIF_LSC_OFFSET_ENABLE;

/* ======================================================================= */
/**
 * ISIF_LSC_GAIN_MODE_MN enumeration for LSC gain mode(Paxel size)
 *
 * @param ISIF_LSC_8_PIXEL  8-pixel tall paxel
 * @param ISIF_LSC_16_PIXEL  16-pixel tall paxel
 * @param ISIF_LSC_32_PIXEL  32-pixel tall paxel
 * @param ISIF_LSC_64_PIXEL  64-pixel tall paxel
 * @param ISIF_LSC_128_PIXEL  128-pixel tall paxel
 */
/* ======================================================================= */
typedef enum {
    ISIF_LSC_8_PIXEL = 3,
    ISIF_LSC_16_PIXEL = 4,
    ISIF_LSC_32_PIXEL = 5,
    ISIF_LSC_64_PIXEL = 6,
    ISIF_LSC_128_PIXEL = 7
} ISIF_LSC_GAIN_MODE_MN;

/* ======================================================================= */
/**
 * ISIF_LSC_GAIN_FORMAT enumeration for LSC gain formats
 *
 * @param ISIF_LSC_GAIN_0Q8  0Q8 format
 * @param ISIF_LSC_GAIN_0Q8_PLUS_1  0Q8 PLUS 1  format
 * @param ISIF_LSC_GAIN_1Q7  1Q7 format
 * @param ISIF_LSC_GAIN_1Q7_PLUS_1 1Q7 PLUS 1 format
 * @param ISIF_LSC_GAIN_2Q6  2Q6 format
 * @param ISIF_LSC_GAIN_2Q6_PLUS_1 2Q6 PLUS 1 format
 * @param ISIF_LSC_GAIN_3Q5  3Q5 format
 * @param ISIF_LSC_GAIN_3Q5_PLUS_1  3Q5 PLUS 1 format
 */
/* ======================================================================= */
typedef enum {
    ISIF_LSC_GAIN_0Q8 = 0,
    ISIF_LSC_GAIN_0Q8_PLUS_1 = 1,
    ISIF_LSC_GAIN_1Q7 = 2,
    ISIF_LSC_GAIN_1Q7_PLUS_1 = 3,
    ISIF_LSC_GAIN_2Q6 = 4,
    ISIF_LSC_GAIN_2Q6_PLUS_1 = 5,
    ISIF_LSC_GAIN_3Q5 = 6,
    ISIF_LSC_GAIN_3Q5_PLUS_1 = 7
} ISIF_LSC_GAIN_FORMAT;

/*******************************************************************/
/**
 * ISIF 2d LSC configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* DLSCCFG ENABLE-- enable 2d LSC module */
    uint8 enable;

    /* HVAL LSCHVAL--LSC HSIZE */
    uint16 lsc_hsize;

    /* HVAL LSCHVAL--LSC VSIZE */
    uint16 lsc_vsize;

    /* HOFST LSCHOFST DATAHOFST 0-16383-- H-direction data offset */
    uint16 hdirection_data_offset;

    /* VOFST LSCVOFST DATAHOFST 0-16383-- V-direction data offset */
    uint16 vdirection_data_offset;

    /* X DLSCINI 6:0-- H-position of the paxel */
    uint8 hposin_paxel;

    /* Y DLSCINI 6:0-- V-position of the paxel */
    uint8 vposin_paxel;

    // ISIF_LSC
    /* GAIN_MODE_M DLSCCFG */
    ISIF_LSC_GAIN_MODE_MN pax_height;

    /* GAIN_MODE_N DLSCCFG */
    ISIF_LSC_GAIN_MODE_MN pax_length;

    /* GAIN_FORMAT DLSCCFG */
    ISIF_LSC_GAIN_FORMAT gain_format;

    /* offset scaling factor */
    uint8 offset_scaling_factor;

    /* OFSTSFT DLSCOFST--offset shift value */
    ISIF_LSC_OFFSET_SHIFT offset_shift_value;

    /* OFSTSFT DLSCOFST--offset enable value */
    ISIF_LSC_OFFSET_ENABLE offset_enable;

    /* gain table address--32 bit aligned */
    uint32 gain_table_address;

    /* gain table length */
    uint16 gain_table_length;

    /* offset table address */
    uint32 offset_table_address;

    /* offset table length */
    uint16 offset_table_length;

    // interupt enable
    // interupt config
} isif_2dlsc_cfg_t;

/* ======================================================================= */
/**
 * ISIF_BLACK_CLAMP_MODE enumeration for selection of black clamp mode
 *
 * @param ISIF_ONE_COLOR_CLAMP  
 * @param ISIF_FOUR_COLOR_CLAMP 
 */
/* ======================================================================= */
typedef enum {
    ISIF_ONE_COLOR_CLAMP = 0,
    ISIF_FOUR_COLOR_CLAMP = 1
} ISIF_BLACK_CLAMP_MODE;

/* ======================================================================= */
/**
 * ISIF_HORIZONTAL_CLAMP_MODE enumeration for selection of horizontal clamp mode
 *
 * @param ISIF_HORIZONTAL_CLAMP_DISABLED  disabled 
 * @param ISIF_HORIZONTAL_CLAMP_ENABLED enabled
 * @param ISIF_PREVIOUS_HORIZONTAL_CLAMP_ENABLED previous image used for subtraction
 */
/*=======================================================================*/
typedef enum {
    ISIF_HORIZONTAL_CLAMP_DISABLED = 0,
    ISIF_HORIZONTAL_CLAMP_ENABLED = 1,
    ISIF_PREVIOUS_HORIZONTAL_CLAMP_ENABLED = 2
} ISIF_HORIZONTAL_CLAMP_MODE;

/* ======================================================================= */
/**
 * ISIF_HBLACK_PIXEL_HEIGHT enumeration for selection of horizontal black clamp window height 
 *
 * @param ISIF_HBLACK_2PIXEL_TALL  2-pixel height
 * @param ISIF_HBLACK_4PIXEL_TALL 4-pixel height 
 * @param ISIF_HBLACK_8PIXEL_TALL 8-pixel height
 * @param ISIF_HBLACK_16PIXEL_TALL 16-pixel height
 */
/*=======================================================================*/

typedef enum {
    ISIF_HBLACK_2PIXEL_TALL = 0,
    ISIF_HBLACK_4PIXEL_TALL = 1,
    ISIF_HBLACK_8PIXEL_TALL = 2,
    ISIF_HBLACK_16PIXEL_TALL = 3
} ISIF_HBLACK_PIXEL_HEIGHT;

/* ======================================================================= */
/**
 * ISIF_HBLACK_PIXEL_WIDTH enumeration for selection of horizontal black clamp window width 
 *
 * @param ISIF_HBLACK_32PIXEL_WIDE  32-pixel width
 * @param ISIF_HBLACK_64PIXEL_WIDE 64-pixel width 
 * @param ISIF_HBLACK_128PIXEL_WIDE 128-pixel width
 * @param ISIF_HBLACK_256PIXEL_WIDE 256-pixel width
 */
/*=======================================================================*/

typedef enum {
    ISIF_HBLACK_32PIXEL_WIDE = 0,
    ISIF_HBLACK_64PIXEL_WIDE = 1,
    ISIF_HBLACK_128PIXEL_WIDE = 2,
    ISIF_HBLACK_256PIXEL_WIDE = 3
} ISIF_HBLACK_PIXEL_WIDTH;

/* ======================================================================= */
/**
 * ISIF_VBLACK_RESET_MODE enumeration for selection of reset mode of Vertical Black clamp
 *
 * @param ISIF_VALUE_HORIZONTAL_DIRECTION  The base value calculated for Horizontal direction 
 * @param ISIF_VALUE_CONFIG_REGISTER Value set via the configuration register 
 * @param ISIF_VALUE_NOUPDATE No update (same as the previous image) 
 */
/*=======================================================================*/
typedef enum {
    ISIF_VALUE_HORIZONTAL_DIRECTION = 0,
    ISIF_VALUE_CONFIG_REGISTER = 1,
    ISIF_VALUE_NOUPDATE = 2
} ISIF_VBLACK_RESET_MODE;

/* ======================================================================= */
/**
 * ISIF_VBLACK_PIXEL_WIDTH enumeration for selection of window width of Vertical Black clamp
 *
 * @param ISIF_VBLACK_2PIXEL_WIDE  2-pixel wide  
 * @param ISIF_VBLACK_4PIXEL_WIDE  4-pixel wide  
 * @param ISIF_VBLACK_8PIXEL_WIDE  8-pixel wide  
 * @param ISIF_VBLACK_16PIXEL_WIDE  16-pixel wide  
 * @param ISIF_VBLACK_32PIXEL_WIDE  32-pixel wide  
 * @param ISIF_VBLACK_64PIXEL_WIDE  64-pixel wide  
 */
/*=======================================================================*/
typedef enum {
    ISIF_VBLACK_2PIXEL_WIDE = 0,
    ISIF_VBLACK_4PIXEL_WIDE = 1,
    ISIF_VBLACK_8PIXEL_WIDE = 2,
    ISIF_VBLACK_16PIXEL_WIDE = 3,
    ISIF_VBLACK_32PIXEL_WIDE = 4,
    ISIF_VBLACK_64PIXEL_WIDE = 5
} ISIF_VBLACK_PIXEL_WIDTH;

/* ======================================================================= */
/**
 * ISIF_HBLACK_PIXEL_VALUE_LIM enumeration for selection of Horizantal pixel value limit
 *
 * @param ISIF_VBLACK_PIXEL_NOT_LIMITED no upper limit
 * @param ISIF_VBLACK_PIXEL_LIMITED  upper limit specified
 */
/*=======================================================================*/
typedef enum {
    ISIF_VBLACK_PIXEL_NOT_LIMITED = 0,
    ISIF_VBLACK_PIXEL_LIMITED = 1
} ISIF_HBLACK_PIXEL_VALUE_LIM;

/* ======================================================================= */
/**
 * ISIF_HBLACK_BASE_WINDOW enumeration for selection of Horizantal black clamp window 
 *
 * @param ISIF_VBLACK_BASE_WINDOW_LEFT most left window
 * @param ISIF_VBLACK_BASE_WINDOW_RIGHT  most right window
 */
/*=======================================================================*/
typedef enum {
    ISIF_VBLACK_BASE_WINDOW_LEFT = 0,
    ISIF_VBLACK_BASE_WINDOW_RIGHT = 1
} ISIF_HBLACK_BASE_WINDOW;

/*******************************************************************/
/**
 * ISIF Vertical Black Clamp parameter configuration data-structure
 *
 */
/*******************************************************************/
typedef struct {

    /* CLVSV CLVWIN2 12-0--Vertical black clamp. Window Start position
     * (V).Range: 0 - 8191 */
    uint16 v_pos;

    /* CLVOBV CLVWIN3 12-0 range 0-8192-- Vertical black clamp. Optical black 
     * V valid (V).Range: 0 - 8191 */
    uint16 v_size;

    /* CLVSH CLVWIN1 12-0--Vertical black clamp. Window Start position
     * (H).Range: 0 - 8191 */
    uint16 h_pos;

    /* CLVOBH CLVWIN0-- Vertical Black clamp. Optical Black H valid (2^L). */
    ISIF_VBLACK_PIXEL_WIDTH h_size;

    /* CLVCOEF CLVWIN0-- Vertical Black clamp .Line average coefficient (k). */
    uint8 line_avg_coef;

    /* CLVRVSL CLVWIN0-- Vertical Black clamp. Select the reset value for the 
     * Clamp value of the Previous line */
    ISIF_VBLACK_RESET_MODE reset_mode;

    /* CLVRV reset value U12--Vertical black clamp reset value (U12) Range: 0 
     * to 4095 */
    uint16 reset_value;

} isif_vertical_black_params;

/*******************************************************************/
 /**
 * ISIF horizontal black clamp parameter configuration data-structure
 */
/*******************************************************************/
typedef struct {
    /* CLHSV CLHWIN2 12-0--Vertical black clamp. Window Start position
     * (V).Range: 0 - 8191 */
    uint16 v_pos;

    /* CLHWN CLHWIN0--[Horizontal Black clamp] Vertical dimension of a Window 
     * (2^N). */
    ISIF_HBLACK_PIXEL_HEIGHT v_size;

    /* CLHSH CLHWIN1 12-0--Horizontal black clamp. Window Start position
     * (H).Range: 0 - 8191 */
    uint16 h_pos;

    /* CLHWM CLHWIN0--[Horizontal Black clamp] Horizontal dimension of a
     * Window (2^M). */
    ISIF_HBLACK_PIXEL_WIDTH h_size;

    /* CLHLMT CLHWIN0--Horizontal Black clamp. Pixel value limitation for the 
     * Horizontal clamp value calculation */
    ISIF_HBLACK_PIXEL_VALUE_LIM pixel_value_limit;

    /* CLHWBS CLHWIN0--[Horizontal Black clamp] Base Window select */
    ISIF_HBLACK_BASE_WINDOW right_window;

    /* CLHWC CLHWIN0--[Horizontal Black clamp] Window count per color. Window 
     * count = CLHWC+1. Range: 1 - 32 */
    uint8 window_count_per_color;
} isif_h_black_params;

/*******************************************************************/
/**
 * ISIF black clamp configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* CLEN CLAMPCFG-- clamp module enablement */
    uint8 enable;

    /* CLMD CLAMPCFG-- horizontal clamp mode */
    ISIF_HORIZONTAL_CLAMP_MODE hclamp_mode;

    /* CLHMD CLAMPCFG-- black clamp mode */
    ISIF_BLACK_CLAMP_MODE black_clamp_mode;

    /* CLDCOFST CLDC s13-- clamp dc-offset value */
    uint16 dcoffset_clamp_value;

    /* CLSV 12-0 (range 0-8191)--black clamp v-start position */
    uint16 black_clamp_v_start_pos;

    /* CLHWIN0-- horizontal black clamp parameters */
    isif_h_black_params horizontal_black;

    /* CLVWIN0-- vertical black clamp parameters */
    isif_vertical_black_params vertical_black;

} isif_clamp_cfg_t;

/*******************************************************************/
/**
 * ISIF flash configuration data-structure
 */
/*******************************************************************/
typedef struct {
    /* FLSHEN FLSHCFG0-- flash enable */
    uint8 enable;

    /* SFLSH FLSHCFG1--Start line to set the FLASH timing signal. */
    uint16 flash_timing_start_line;

    /* VFLSH FLSHCFG--Valid width of the FLASH timing signal. Valid width =
     * Crystal-clock x 2 x (VFLSH + 1) */
    uint16 flash_timing_width;

} isif_flash_cfg_t;

typedef struct {
    /* CVD0 VDINT0--VD0 Interrupt timing in a field (line number). */
    uint16 vd0_intr_timing_lineno;

    /* CVD1 VDINT1--VD1 Interrupt timing in a field (line number). */
    uint16 vd1_intr_timing_lineno;

    /* CVD2 VDINT2--VD2 Interrupt timing in a field (line number).  */
    uint16 vd2_intr_timing_lineno;

} isif_vd_interrupt_t;

/*******************************************************************/
/**
 * ISIF top-level configuration data-structure
 */
/*******************************************************************/
typedef struct {

    /* feature-flag for ISIF configuration */
    uint32 feature_flag;

    /* source configuration parameters */
    isif_ip_src_cfg_t *src_cfg;

    /* sensor data linearization parameters */
    isif_sensor_linear_cfg_t *linearization_params;

    /* input data format parameters */
    isif_ip_format_cfg_t *ip_data_format_params;

    /* Color space conversion parameters */
    isif_csc_cfg_t *csc_params;

    /* digital clamp with Horizontal/Vertical offset drift compensation
     * parameters */
    isif_clamp_cfg_t *digital_clamp_params;

    /* vertical line defect correction parameters */
    isif_vldc_cfg_t *vldc_params;

    /* Programmable 2D-matrix Lens Shading Correction parameters */
    isif_2dlsc_cfg_t *lsc_params;

    /* Gain and offset parameters */
    isif_gain_offset_cfg_t *gain_offset_params;

    /* flash_timing_params */
    isif_flash_cfg_t *flash_timing_params;

    /* SDRAM parameters */
    isif_sdram_op_cfg_t *sdram_op_params;

    isif_vd_interrupt_t vd;

} isif_cfg_t;

/* ================================================================ */
/* FUNCTION PROTOTYPES */
 /*===============================================================*/

/* ===================================================================
 *  @func     isif_2dlsc_is_prefetched                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
uint8 isif_2dlsc_is_prefetched(void);

/* ================================================================ */
/**
 *  isif_config_timing_generator() configures the timing generator of the ISIF module. 
 *
 *  @param  cfg      isif_flash_cfg_t * cfg  is a pointer to the data-structure of isif_flash_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_timing_generator                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_timing_generator(isif_flash_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_vd_line_numbers() configures the VD0, VD1, VD2 line number for the ISIF module. 
 *
 *  @param  cfg      isif_vd_interrupt_t * cfg  is a pointer to the data-structure of isif_vd_interrupt_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_vd_line_numbers                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_vd_line_numbers(isif_vd_interrupt_t * cfg);

/* ===================================================================
 *  @func     isif_config_vd0_line_number                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_vd0_line_number(uint32 line_number);

/* ===================================================================
 *  @func     isif_config_vd1_line_number                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_vd1_line_number(uint32 line_number);

/* ===================================================================
 *  @func     isif_config_vd2_line_number                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_vd2_line_number(uint32 line_number);

/* ================================================================ */
/**
 *  isif_config_sensor_linearization() configures the sensor linearization component of  the ISIF module. 
 *
 *  @param  cfg  isif_sensor_linear_cfg_t * cfg       is a pointer to the data-structure of isif_sensor_linear_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_sensor_linearization                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_sensor_linearization(isif_sensor_linear_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_gain_offset() configures the gain-offset component of  the ISIF module. 
 *
 *  @param  cfg  isif_gain_offset_cfg_t * cfg:-  is a pointer to the data-structure of isif_gain_offset_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_gain_offset                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_gain_offset(isif_gain_offset_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_input_src() configures the input source formatting settings of the ISIF module. 
 *
 *  @param  cfg  isif_ip_src_cfg_t * cfg     is a pointer to the data-structure of isif_ip_src_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_input_src                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_input_src(isif_ip_src_cfg_t * cfg);

/* ===================================================================
 *  @func     isif_config_in_sizes                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_in_sizes(isif_input_sizes_t * sizes);

/* ================================================================ */
/**
 *  isif_config_sdram_format() configures the sdram output formatting of  the ISIF module. 
 *
 *  @param  cfg isif_sdram_op_cfg_t * cfg       is a pointer to the data-structure of isif_sdram_op_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_sdram_format                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_sdram_format(isif_sdram_op_cfg_t * cfg);

/* ===================================================================
 *  @func     isif_config_sdram_address                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_sdram_address(uint32 address);

/* ================================================================ */
/**
 *  isif_config_lpf() configures the LPF component of  the ISIF module to be either ON or OFF. 
 *
 *  @param  lpf_onoff        is an unsigned integer which indicates the status of LPF to be configured
 *  @return  status   	   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_lpf                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_lpf(uint8 lpf_onoff);

/* ================================================================ */
/**
 *  isif_config_alaw_compression() configures the ALAW  component of the ISIF module to be either ON or OFF. 
 *
 *  @param  alaw_onoff  uint8 alaw_onoff :-    is an unsigned integer which indicates the status of ALAW compression for sdram
 *  @return  status              Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_alaw_compression                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_alaw_compression(uint8 alaw_onoff);

/* ================================================================ */
/**
 *  isif_config_dpcm_encoder() configures the DPCM component of the ISIF module to be using either type1 or type-2. 
 *
 *  @param  dpcm_predictor  uint8 dpcm_predictor is an unsigned integer which indicates the type of DPCM compression to be used
 *  @return  status  			      Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_dpcm_encoder                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_dpcm_encoder(uint8 dpcm_predictor, uint8 dpcm_enable);

/* ================================================================ */
/**
 *  isif_config_hv_culling() configures the horizontal-vertical culling of the input in  the ISIF module. 
 *
 *  @param  cfg   isif_culling_cfg_t * cfg is a pointer to the data-structure of isif_culling_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_hv_culling                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_hv_culling(isif_culling_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_hv_culling_disable() disables the culling mechanism in the ISIF module. 
 *
 *  @param   none
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_hv_culling_disable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_hv_culling_disable();

/* ===================================================================
 *  @func     isif_config_pol                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_pol(isif_ip_src_cfg_t * src_cfg);

/* ================================================================ */
/**
 *  isif_config_vertical_line_defect_correction() configures the VLDC component  of  the ISIF module. 
 *
 *  @param  cfg     isif_vldc_cfg_t * cfg   is a pointer to the data-structure of isif_vldc_cfg_t type which encapsulates the configuration settings
 *  @return  status    Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_vertical_line_defect_correction                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_vertical_line_defect_correction(isif_vldc_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_2d_lsc() configures the 2d-Lens Shading Correction component  of  the ISIF module. 
 *
 *  @param  cfg   isif_2dlsc_cfg_t * cfg  is a pointer to the data-structure of isif_2dlsc_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_2d_lsc                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_2d_lsc(isif_2dlsc_cfg_t * cfg);

/* ===================================================================
 *  @func     isif_2dlsc_enable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_2dlsc_enable(uint8 enable);

/* ===================================================================
 *  @func     isif_2dlsc_sof_int_enable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_2dlsc_sof_int_enable(uint8 enable);

/* ================================================================ */
/**
 *  isif_config_ip_data_formatter() configures the input data formatting settings of  the ISIF module. 
 *
 *  @param  cfg  isif_ip_format_cfg_t * cfg      is a pointer to the data-structure of isif_ip_format_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_ip_data_formatter                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_ip_data_formatter(isif_ip_format_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_color_space_converter() configures the Color-Space-Converter component of  the ISIF module. 
 *
 *  @param  cfg   isif_csc_cfg_t * cfg     is a pointer to the data-structure of isif_csc_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_color_space_converter                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_color_space_converter(isif_csc_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_config_black_clamp() configures the black-clamp  component settings  of  the ISIF module. 
 *
 *  @param  cfg  isif_clamp_cfg_t * cfg      is a pointer to the data-structure of isif_clamp_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config_black_clamp                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config_black_clamp(isif_clamp_cfg_t * cfg);

/* ================================================================ */
/**
 *  isif_init() initializes the ISIF module. It also sets  the CSL structure to the ISIF base address. 
 *
 *  @param  
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_init();

/* ================================================================ */
/**
 *  isif_start() configures the running mode of  the ISIF module. 
 *
 *  @param  enable  is the start mode of ISIF, specified through data-structure isif_start_t
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_start                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_start(isif_start_t enable);

/* ================================================================ */
/**
 *  isif_close() stops the operation of ISIF module by gating the clock. 
 *
 *  @param  
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */
/* ===================================================================
 *  @func     isif_close                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_close();

/* ================================================================ */
/**
 *  isif_open() enables the clock for the ISIF module.
 *
 *  @param  cfg        is a pointer to the data-structure of isif_gain_offset_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_open                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_open();

/* ================================================================ */
/**
 *  isif_config() is the top-level configuration function to configure the ISIF module. 
 *
 *  @param  isif_config  isif_cfg_t * isif_config      is a pointer to the data-structure of isif_cfg_t type which encapsulates the configuration settings
 *  @return  status              Success/failure of the operation
 */
 /*================================================================== */

/* ===================================================================
 *  @func     isif_config                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
ISP_RETURN isif_config(isif_cfg_t * isif_config);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
