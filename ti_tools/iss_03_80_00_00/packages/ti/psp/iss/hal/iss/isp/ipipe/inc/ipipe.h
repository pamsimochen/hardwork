/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file ipipe.h
 *           This file contains the prototypes and definitions to configure ipipe
 *
 *
 * @path     Centaurus\drivers\drv_isp\inc\csl
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *!
 */
 /*========================================================================= */
#ifndef IPIPE_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define IPIPE_H

/****************************************************************
 *  INCLUDE FILES
 *****************************************************************/

#include "ipipe_reg.h"
#include "../../isp5_utils/isp5_sys_types.h"
#include "../../common/inc/isp_common.h"

#define IPIPE_BASE_OFFSET       (0x0800)
#define IPIPE_BASE_ADDRESS      (ISP5_BASE_ADDRESS + IPIPE_BASE_OFFSET)

#define IPIPE_HIST_MEM0_OFFSET  (0x2000)
#define IPIPE_HIST_MEM0_ADDRESS (ISP5_BASE_ADDRESS + IPIPE_HIST_MEM0_OFFSET)

#define IPIPE_HIST_MEM1_OFFSET  (0x2800)
#define IPIPE_HIST_MEM1_ADDRESS (ISP5_BASE_ADDRESS + IPIPE_HIST_MEM1_OFFSET)

#define IPIPE_HIST_MEM2_OFFSET  (0x3000)
#define IPIPE_HIST_MEM2_ADDRESS (ISP5_BASE_ADDRESS + IPIPE_HIST_MEM2_OFFSET)

#define IPIPE_HIST_MEM3_OFFSET  (0x3800)
#define IPIPE_HIST_MEM3_ADDRESS (ISP5_BASE_ADDRESS + IPIPE_HIST_MEM3_OFFSET)

#define IPIPE_BSC_MEM0_OFFSET 0x4000
#define IPIPE_BSC_MEM0_ADDRESS (ISP5_BASE_ADDRESS+IPIPE_BSC_MEM0_OFFSET)

#define IPIPE_BSC_MEM1_OFFSET 0x6000
#define IPIPE_BSC_MEM1_ADDRESS (ISP5_BASE_ADDRESS+IPIPE_BSC_MEM1_OFFSET)

/* ================================================================ */
/**
 *Description:-  typedef to CSL_IpipeRegs
 */
 /*================================================================== */

typedef volatile CSL_IpipeRegs *ipipe_regs_ovly;

/* ================================================================ */
/**
 *Description:-
 */
 /*================================================================== */

typedef struct {

    uint8 opened;

} ipipe_dev_data_t;

/* ================================================================ */
/**
 *Description:- The following defines are for the different sub-blocks , filters of ipipe
 */
 /*================================================================== */
#define IPIPE_DPC_LUT 0
#define IPIPE_DPC_OTF 1
#define IPIPE_NOISE_FILTER_1 2
#define IPIPE_NOISE_FILTER_2 3
#define IPIPE_GIC 4
#define IPIPE_WB 5
#define IPIPE_CFA 6
#define IPIPE_RGB_RGB_1 7
#define IPIPE_GAMMA 8
#define IPIPE_RGB_RGB_2 9
#define IPIPE_3D_LUT 10
#define IPIPE_RGB_TO_YUV 11
#define IPIPE_LSC 12

#define IPIPE_GBCE 13
// efine IPIPE_444to422

#define IPIPE_EE 14
#define IPIPE_CAR 15
#define IPIPE_CGS 16
#define IPIPE_BSC 17
#define IPIPE_HST 18
#define IPIPE_BOXCAR 19

#define IPIPE_YUV444_YUV422 20

/* ================================================================ */
/* Description:-The following typedef "ipipe_module" should be populated
 * using one of the define above */
 /*================================================================== */
typedef uint32 ipipe_module;

/* ==========================================================================
 */
/* Description:-The following defines are for bit masks for filters and
 * sub-blocks of ipipie, the field feature flag needs to be populated using
 * these */
/* ==========================================================================
 */

#define IPIPE_LUT_DPC_FLAG  (1<<IPIPE_DPC_LUT)
#define IPIPE_OTF_DPC_FLAG  (1<<IPIPE_DPC_OTF)
#define IPIPE_NOISE_FILTER_1_FLAG  (1<<IPIPE_NOISE_FILTER_1)
#define IPIPE_NOISE_FILTER_2_FLAG (1<<IPIPE_NOISE_FILTER_2)
#define IPIPE_GIC_FLAG (1<<IPIPE_GIC)
#define IPIPE_WB_FLAG (1<<IPIPE_WB)
#define IPIPE_CFA_FLAG (1<<IPIPE_CFA)
#define IPIPE_RGB_RGB_1_FLAG (1<<IPIPE_RGB_RGB_1)
#define IPIPE_GAMMA_FLAG (1<<IPIPE_GAMMA)
#define IPIPE_RGB_RGB_2_FLAG (1<<IPIPE_RGB_RGB_2)
#define IPIPE_3D_LUT_FLAG (1<<IPIPE_3D_LUT )
#define IPIPE_RGB_TO_YUV_FLAG (1<<IPIPE_RGB_TO_YUV )

#define IPIPE_GBCE_FLAG (1<<IPIPE_GBCE )

#define IPIPE_EE_FLAG (1<<IPIPE_EE )
#define IPIPE_CAR_FLAG (1<<IPIPE_CAR )
#define IPIPE_CGS_FLAG (1<<IPIPE_CGS )
#define IPIPE_BSC_FLAG (1<<IPIPE_BSC)
#define IPIPE_HST_FLAG (1<<IPIPE_HST)
#define IPIPE_BOXCAR_FLAG (1<<IPIPE_BOXCAR)
#define IPIPE_LSC_FLAG (1<<IPIPE_LSC)

#define IPIPE_YUV444_YUV422_FLAG (1<<IPIPE_YUV444_YUV422)

typedef uint32 ipipe_filter_flag;

/* ================================================================ */
/* IPIPE_START_T is used to start/stop the operartion of ipipe @param
 * IPIPE_STOP , this is to disable ipipe @param IPIPE_RUN , this enables
 * continuous mode operation of ipipe @param IPIPE_SINGLE . this enables
 * the one-shot operation of ipipe */
 /*================================================================== */
typedef enum {
    IPIPE_STOP,                                            // stop/disable
    IPIPE_RUN,                                             // run/enable
    IPIPE_SINGLE                                           // one shot run
} IPIPE_START_T;

/* ================================================================ */
/**
 *Description:- enum for ctrl of gamma bypass
 */
 /*================================================================== */
typedef enum {
    IPIPE_GAMMA_BYPASS_ENABLE = 1,
    IPIPE_GAMMA_BYPASS_DISABLE = 0
} IPIPE_GAMMA_BYPASS_T;

/* ================================================================ */
/**
 *Description:- enum for sle of gamma table size
 */
 /*================================================================== */

typedef enum {

    IPIPE_GAMMA_TBL_64 = 0,
    IPIPE_GAMMA_TBL_128 = 1,
    IPIPE_GAMMA_TBL_256 = 2,
    IPIPE_GAMMA_TBL_512 = 3
} IPIPE_GAMMA_TABLE_SIZE_T;

/* ================================================================ */
/**
 *Description:- structure for configuring gamma module in isp
 */
 /*================================================================== */

typedef struct {

    IPIPE_GAMMA_TABLE_SIZE_T gamma_tbl_size;
    uint8 tbl;                                             // may not be
                                                           // needed.since
                                                           // table is always
                                                           // in RAM
    IPIPE_GAMMA_BYPASS_T bypass_b;                         /* o not bypassed */
    IPIPE_GAMMA_BYPASS_T bypass_g;
    IPIPE_GAMMA_BYPASS_T bypass_r;
    int16 *red_table;                                      /* pointer to red
                                                            * gamma table */
    int16 *blue_table;
    int16 *green_table;

} ipipe_gamma_cfg_t;

/* ================================================================ */
/**
 *Description:- struct for cfg of histogram dimensions in ipipe
 */
 /*================================================================== */

typedef struct {
    uint16 v_pos;
    uint16 v_size;
    uint16 h_pos;
    uint16 h_size;
} ipipe_hist_dim_t;

/* ================================================================ */
/**
 *Description:- struct for cfg of histogram in ipipe
 */
 /*================================================================== */

typedef struct {
    uint8 enable;
    uint8 ost;
    uint8 sel;
    /* uint8 sel_2; */// ######## This is not in documentation(IPIPE_CONFIG.RTF) so
    // removed############
    uint8 type;
    uint8 bins;
    uint8 shift;
    uint8 col;                                             /* bits [3:0], 0
                                                            * is disable */
    uint8 regions;                                         /* [3:0], 0 is
                                                            * disable */
    ipipe_hist_dim_t *hist_dim;                            /* pointer to
                                                            * array of 4
                                                            * structs */
    uint8 clear_table;
    uint8 table_sel;
    uint8 *gain_tbl;                                       /* r,gr,gb,b */

} ipipe_hist_cfg_t;

/* ================================================================ */
/**
 *Description:- enum for selecting the DPC algorith type in DPC OTF
 */
 /*================================================================== */

typedef enum {
    IPIPE_DPC_OTF_ALG_MINMAX2 = 0,
    IPIPE_DPC_OTF_ALG_MINMAX3 = 1
} IPIPE_DPC_ALGO_T;

/* ================================================================ */
/**
 *Description:-enum for slecting the min/max type in defect pixel correction
 */
 /*================================================================== */
typedef enum {
    IPIPE_DPC_OTF_MAX1_MIN1 = 0,
    IPIPE_DPC_OTF_MAX2_MIN2 = 1
} IPIPE_DPC_OTF_TYPE_T;

/* ================================================================ */
/**
 *Description:- struct for cfg of threshold values in dpc otf in ipipe
 */
 /*================================================================== */
typedef struct {
    // uint16 *thr_c;/*r,gr,gb,b*/
    // uint16 *thr_d;/*r,gr,gb,b*/
    uint16 thr_c[4];                                       /* r,gr,gb,b */
    uint16 thr_d[4];                                       /* r,gr,gb,b */
} ipipe_dpc_otf_dpc2_t;

/* ================================================================ */
/**
 *Description:- struct for selecting thr and slope in OTF DPC
 */
 /*================================================================== */
typedef struct {

    uint16 d_thr;
    uint8  d_slp;
    uint16 d_min;
    uint16 d_max;

} ipipe_dpc_oft_filter_t;

/* ================================================================ */
/**
 *Description:- struct for cfg of 3d and 2d  OTF DPC
 */
 /*================================================================== */
typedef struct {
    uint8 shift;
    ipipe_dpc_oft_filter_t corr;
    ipipe_dpc_oft_filter_t dett;

} ipipe_dpc_otf_dpc3_t;

/* ================================================================ */
/**
 *Description:- Union which helps selec either dpc2/dpc 3 params
 */
 /*================================================================== */
typedef union {
    ipipe_dpc_otf_dpc2_t dpc2_params;
    ipipe_dpc_otf_dpc3_t dpc3_params;
} ipipe_dpc_otf_filter_params_t;

/* ================================================================ */
/**
 *Description:-main struct for cfg of DPC OTF in ipipe
 */
 /*================================================================== */
typedef struct {
    /** Defect Correction Enable */
    uint8 enable;
    IPIPE_DPC_OTF_TYPE_T type;
    IPIPE_DPC_ALGO_T algo;
    ipipe_dpc_otf_filter_params_t dpc_data;
} ipipe_dpc_otf_cfg_t;

/* ================================================================ */
/**
 *Description:-enum for selecting pixel replacement type in DPC LUT
 */
 /*================================================================== */
typedef enum {
    IPIPE_DPC_LUT_REPLACE_BLACK = 0,
    IPIPE_DPC_LUT_REPLACE_WHITE = 1
} IPIPE_DPC_LUT_REPLACEMENT_T;

/* ================================================================ */
/**
 *Description:-enum for selecting DPC LUT table size
 */
 /*================================================================== */

typedef enum {
    IPIPE_DPC_LUT_TBL_SIZE_1024 = 0,
    IPIPE_DPC_LUT_TBL_SIZE_INF = 1
} IPIPE_DPC_LUT_TBL_SIZE_T;

/* ================================================================ */
/**
 *Description:-struct for DPC LUT cfg.
 */
 /*================================================================== */
typedef struct {
    /** Defect Correction Enable */
    uint16 enable;
    IPIPE_DPC_LUT_TBL_SIZE_T table_type;
    IPIPE_DPC_LUT_REPLACEMENT_T replace_type;
    uint16 lut_valid_addr;
    uint16 lut_size;
    uint32 lut_table0_addr;                                /* This is
                                                            * mentioned in
                                                            * ISS doc */
    uint32 lut_table1_addr;

} ipipe_dpc_lut_cfg_t;

/* ================================================================ */
/**
 *Description:- enum for slection of threshold vaue in GIC filter
 */
 /*================================================================== */
typedef enum {
    IPIPE_GIC_GICTHR = 0,
    IPIPE_GIC_NF2THR = 1
} IPIPE_GIC_SEL_T;

/* ================================================================ */
/**
 *Description:-enum for selecting LSC gain in GIC
 */
 /*================================================================== */
typedef enum {
    IPIPE_GIC_LSC_GAIN_OFF = 0,
    IPIPE_GIC_LSC_GAIN_ON = 1
} IPIPE_GIC_LSC_GAIN_T;

/* ================================================================ */
/**
 *Description:- enum for selecting the index in GIC
 */
 /*================================================================== */
typedef enum {
    IPIPE_GIC_DIFF_INDEX = 0,
    IPIPE_GIC_HPD_INDEX = 1
} IPIPE_GIC_INDEX_T;

/* ================================================================ */
/**
 *Description:- main GIC cfg struct
 */
 /*================================================================== */

typedef struct {
    /** works only when data format is GR,GB */

    uint16 enable;
    IPIPE_GIC_LSC_GAIN_T lsc_gain;
    IPIPE_GIC_SEL_T sel;
    IPIPE_GIC_INDEX_T typ;
    uint8 gic_gain;
    uint8 gic_nfgain;
    uint16 gic_thr;
    uint16 gic_slope;

} ipipe_gic_cfg_t;

/* ================================================================ */
/**
 *Description:-enum for selecting the type of GBE method
 */
 /*================================================================== */
typedef enum {
    IPIPE_GBCE_METHOD_Y_VALUE = 0,                         /* Cr CB
                                                            * unmodified */
    IPIPE_GBCE_METHOD_GAIN_TBL = 1
} IPIPE_GBCE_METHOD_T;

/* ================================================================ */
/**
 *  Description:- main GBCE configuration
 */
 /*================================================================== */
typedef struct {
    /** Defect Correction Enable */
    uint16 enable;
    IPIPE_GBCE_METHOD_T typ;
    uint16 *lookup_table;
} ipipe_gbce_cfg_t;

/* ================================================================ */
/**
  * Description:- main cfg struct for CAR module in ipipe
  */
 /*================================================================== */

typedef struct {
    uint8 enable;

    uint8 typ;

    uint8 sw0_thr;
    uint8 sw1_thr;

    uint8 hpf_type;
    uint8 hpf_shift;
    uint8 hpf_thr;

    uint8 gn1_gain;
    uint8 gn1_shift;
    uint16 gn1_min;

    uint8 gn2_gain;
    uint8 gn2_shift;
    uint16 gn2_min;

} ipipe_car_cfg_t;

/* ================================================================ */
/**
  * Description:- enum for halo reduction in Edge enhancement
  */
 /*================================================================== */
typedef enum {
    IPIPE_HALO_REDUCTION_ENABLE = 1,
    IPIPE_HALO_REDUCTION_DISABLE = 0
} IPIPE_EE_HALO_CTRL_T;

/* ================================================================ */
/**
 *Description:- main Edge enhancement struct
 */
 /*================================================================== */
typedef struct {
    /** Defect Correction Enable */
    uint16 enable;
    IPIPE_EE_HALO_CTRL_T halo_reduction;
    int16 *mul_val;                                        /* 9 co
                                                            * -efficients */
    uint8 sel;
    uint8 shift_hp;
    uint16 threshold;
    uint16 gain;
    uint16 hpf_low_thr;
    uint8 hpf_high_thr;
    uint8 hpf_gradient_gain;
    uint8 hpf_gradient_offset;
    int16 *ee_table;
} ipipe_ee_cfg_t;

/* ================================================================ */
/**
 *Description:- struct for cfg of chroma params in CGS
 */
 /*================================================================== */
typedef struct {

    uint16 thr;
    uint16 gain;
    uint16 shift;
    uint16 min;

} ipipe_chroma_params_t;

/* ================================================================ */
/**
 *Description:- main cfg struct for CGS
 */
 /*================================================================== */
typedef struct {
    /** Defect Correction Enable */
    uint16 enable;
    ipipe_chroma_params_t y_chroma_low;                    /* Gain1 low */
    ipipe_chroma_params_t y_chroma_high;                   /* Gain1 high */
    ipipe_chroma_params_t c_chroma;                        /* Gain2 */

} ipipe_cgs_cfg_t;

/* ================================================================ */
/**
 *Description:-struct for BSC dimensions cfg
 */
 /*================================================================== */
typedef struct {
    uint8 vectors;
    uint8 shift;
    uint16 v_pos;
    uint16 h_pos;
    uint16 v_num;
    uint16 h_num;
    uint8 v_skip;
    uint8 h_skip;

} ipipe_bsc_pos_params_t;

/* ================================================================ */
/**
 *Description:-main BSC cfg struct
 */
 /*================================================================== */
typedef struct {
    /** Defect Correction Enable */
    uint8 enable;
    uint8 mode;
    uint8 col_sample;
    uint8 row_sample;
    uint8 element;                                         /* Y or CB or CR */
    ipipe_bsc_pos_params_t col_pos;
    ipipe_bsc_pos_params_t row_pos;
} ipipe_bsc_cfg_t;

typedef enum {

    IPIPE_BSC_MEMORY_0 = 0,                                /* row sum */
    IPIPE_BSC_MEMORY_1 = 1                                 /* column sum */
} IPIPE_BSC_MEMORY_T;

/* ================================================================ */
/**
 *Description:- enum for choosing one of the two noise filters
 */
 /*================================================================== */
typedef enum {
    NOISE_FILTER_1 = 1,
    NOISE_FILTER_2 = 2
} IPIPE_NOISE_FILTER_T;

/* ================================================================ */
/**
 *Description:- enum for selecting the spread in NF
 */
 /*================================================================== */
typedef enum {
    IPIPE_NF_SPR_SINGLE = 0,
    IPIPE_NF_SPR_LUT = 1
} IPIPE_NF_SEL_T;

/* ================================================================ */
/**
 *Description:-enum for ctrling the lsc gain applied in Noise Filter
 */
 /*================================================================== */
typedef enum {
    IPIPE_NF_LSC_GAIN_OFF = 0,
    IPIPE_NF_LSC_GAIN_ON = 1
} IPIPE_NF_LSC_GAIN_T;

/* ================================================================ */
/**
 *Description:- enum for selecting the sampling method
 */
 /*================================================================== */
typedef enum {
    IPIPE_NF_SAMPLE_BOX = 0,
    IPIPE_NF_SAMPLE_DIAMOND = 1
} IPIPE_NF_SAMPLE_METHOD_T;

/* ================================================================ */
/**
 *Description:-main cfg struct for Noise filter .
 */
 /*================================================================== */
typedef struct {
    uint8 enable;
    IPIPE_NOISE_FILTER_T nf_num;
    IPIPE_NF_SEL_T sel;
    IPIPE_NF_LSC_GAIN_T lsc_gain;                          /* 0 */
    IPIPE_NF_SAMPLE_METHOD_T typ;                          /* 1 */
    uint8 down_shift_val;                                  /* 3 */
    uint8 spread;                                          /* 4 */
    uint16 *thr;                                           /* 10 */
    uint8 *str;                                            /* 5 */
    uint8 *spr;                                            /* 5 */
    uint16 edge_min;                                       /* 10 */
    uint16 edge_max;                                       /* 10 */

} ipipe_noise_filter_cfg_t;

/* ================================================================ */
/**
 *Description:-enum for selecting one of the two rgb to rgb blending filters
 */
 /*================================================================== */
typedef enum {
    IPIPE_RGB_RGB_FILTER_1 = 0,
    IPIPE_RGB_RGB_FILTER_2 = 1
} IPIPE_RGB_RGB_FILTER_NUM_T;

/* ================================================================ */
/**
 *Description:- struct with co -efficients and offsets , main cfg struct for rgb to rgb blending
 */
 /*==================================================================*/

typedef struct {
    int16 *mul_off;                                        /* RR,GR,BR,RG,GG,BG,RB,GB,BB
                                                            * each 11 bits */
    int16 *oft;                                            /* R,G,B each 13
                                                            * bits */

} ipipe_rgb_rgb_cfg_t;

/* ================================================================ */
/**
 *Description:- main cfg struct for rgb to yuv conversion
 */
 /*================================================================== */
typedef struct {
    uint8 brightness;
    uint8 contrast;
    int16 *mul_val;                                        /* RY,GY,BY,RCB,GCB,BCB
                                                            * ,RCR,GCR,BCR 12
                                                            * bits */

    int16 *offset;                                         /* Y,CB,CR -11bits */

} ipipe_rgb_yuv_cfg_t;

/* ================================================================ */
/**
 *Description:-enum for selecting the y phase when yuv444 is converted to yuv422
 */
 /*================================================================== */

typedef enum {
    IPIPE_YUV_PHS_POS_COSITED = 0,
    IPIPE_YUV_PHS_POS_CENTERED = 1
} IPIPE_YUV_PHASE_POS_T;

/* ================================================================ */
/**
 *Description:-main cfg struct which controls the operation of yuv444 to yuv 422
 */
 /*================================================================== */

typedef struct {

    IPIPE_YUV_PHASE_POS_T pos;
    uint8 lpf_en;

} ipipe_yuv444_yuv422_cfg_t;

/* ================================================================ */
/**
 *Description:-main cfg ctruct for controlling the White balance filtering
 */
 /*================================================================== */
typedef struct {

    uint16 *offset;                                        /* offseet after
                                                            * R,GR,GB,B */
    uint16 *gain;                                          /* gain for R gr
                                                            * gb B */

} ipipe_wb_cfg_t;

/* ================================================================ */
/**
 *Description:- struct for cfg of DIR filter in CFA
 */
 /*================================================================== */
typedef struct {
    uint16 hpf_thr;
    uint16 hpf_slope;
    uint16 mix_thr;
    uint16 mix_slope;
    uint16 dir_thr;
    uint16 dir_slope;
    uint16 dir_ndwt;

} ipipe_cfa_dir_t;

/* ================================================================ */
/**
 *Description:- struct for cfg of DAA filter in CFA
 */
 /*================================================================== */
typedef struct {
    // uint8 sel;
    uint8 mono_hue_fra;
    uint8 mono_edg_thr;
    uint16 mono_thr_min;
    uint16 mono_thr_slope;
    uint16 mono_slp_min;
    uint16 mono_slp_slp;
    uint16 mono_lpwt;

} ipipe_cfa_daa_t;

/* ================================================================ */
/**
 *Description:-enum for selecting the operation mode of CFA
 */
 /*================================================================== */
typedef enum {
    IPIPE_CFA_MODE_2DIR = 0,
    IPIPE_CFA_MODE_2DIR_DA = 1,
    IPIPE_CFA_MODE_DAA = 2
} IPIPE_CFA_MODE_T;

/* ================================================================ */
/**
 *Description:-main cfg struct for CFA
 */
 /*================================================================== */
typedef struct {
    uint8 enable;
    IPIPE_CFA_MODE_T mode;
    ipipe_cfa_dir_t dir;
    ipipe_cfa_daa_t daa;
} ipipe_cfa_cfg_t;

/* ================================================================ */
/**
 *Description:-main cfg struct for 3D lut
 */
 /*================================================================== */

typedef struct {
    uint8 enable;
    void *table_adr;
    // uint8 datatyp;
    // uint32 tbladdr;
} ipipe_3d_lut_cfg_t;

/* ================================================================ */
/**
 *Description:- enum for selecting inout op data formats
 */
 /*================================================================== */
typedef enum {
    IPIPE_BAYER_INPUT_YCRCB_OUTPUT = 0,
    IPIPE_BAYER_INPUT_BAYER_OUTPUT = 1,
    IPIPE_BAYER_INPUT_DISABLE_OUTPUT = 2,
    IPIPE_YCRCB_INPUT_YCRCB_OUTPUT = 3
} IPIPE_IO_PIX_FMT_T;

/* ================================================================ */
/**
 *Description:-enum for color selection in pix fmt
 */
 /*================================================================== */
typedef enum {

    IPIPE_PIXEL_COLOUR_R = 0,
    IPIPE_PIXEL_COLOUR_GR = 1,
    IPIPE_PIXEL_COLOUR_GB = 2,
    IPIPE_PIXEL_COLOUR_B = 3
} IPIPE_RAW_PIXEL_COLOR_FMT_T;

/* ================================================================ */
/**
 *Description:-struct for cfg'ing the input dimensions of ipipe
 */
 /*================================================================== */
typedef struct {
    uint16 vps;
    uint16 v_size;
    uint16 hps;
    uint16 hsz;
} ipipe_dims_t;

/* ================================================================ */
/**
 *Description:- main cfg struct for configuring the dimensions and type of ip/op pix formats in ipipe
 */
 /*================================================================== */
typedef struct {

    uint8 wrt;
    // uint8 ost;
    IPIPE_IO_PIX_FMT_T io_pixel_fmt;
    IPIPE_RAW_PIXEL_COLOR_FMT_T Odd_line_Odd_pixel;        /* OE OO EE EO,
                                                            * required only
                                                            * if inoput is
                                                            * RGB */
    IPIPE_RAW_PIXEL_COLOR_FMT_T Odd_line_Even_pixel;
    IPIPE_RAW_PIXEL_COLOR_FMT_T Even_line_Odd_pixel;
    IPIPE_RAW_PIXEL_COLOR_FMT_T Even_line_Even_pixel;

    ipipe_dims_t dims;

} ipipe_src_cfg_t;

/* ================================================================ */
/**
 *Description:-main cfg struct for Boxcar module
 */
 /*================================================================== */

typedef struct {
    uint8 enable;
    uint8 ost;                                             /* one shot or
                                                            * free run */
    uint8 box_size;
    uint8 shift;
    uint32 addr;

} ipipe_boxcar_cfg_t;

/* ================================================================ */
/**
 *Description:-main cfg struct for LSC.
 */
 /*================================================================== */

typedef struct {

    uint16 v_offset;
    int16 v_linear_coeff;                                  // va1
    int16 v_quadratic_coeff;                               // va2
    uint8 v_linear_shift;                                  // vs1
    uint8 v_quadratic_shift;                               // vs2

    uint16 h_offset;
    int16 h_linear_coeff;                                  // va1
    int16 h_quadratic_coeff;                               // va2
    uint8 h_linear_shift;                                  // vs1
    uint8 h_quadratic_shift;                               // vs2

    uint8 gain_r;
    uint8 gain_gr;
    uint8 gain_gb;
    uint8 gain_b;

    uint8 off_r;
    uint8 off_gr;
    uint8 off_gb;
    uint8 off_b;

    uint8 shift;                                           // LSC_SHIFT
    uint16 max;                                            // LSC_MAX

} ipipe_lsc_cfg_t;

/* ================================================================ */
/**
 *Description:- defines for clk ctrl
 */
 /*================================================================== */
#define G3 3
#define G2 2
#define G1 1
#define G0 0

#define IPIPE_EE_CLK G3
#define IPIPE_CAR_CLK G3

#define IPIPE_CFA_CLK G2
#define IPIPE_GAMMA_CLK G2
#define IPIPE_RGB_RGB_CLK G2
#define  IPIPE_3D_LUT_CLK G2
#define  IPIPE_RGB_TO_YUV_CLK G2
#define IPIPE_GBCE_CLK G2
#define IPIPE_BSC_CLK G2

/* TODO:-This need to be checked....######### */
#define IPIPE_DPC_LUT_CLK G1
#define IPIPE_DPC_OTF_CLK G1
#define IPIPE_NOISE_FILTER_1_CLK G1
#define IPIPE_NOISE_FILTER_2_CLK  G1
#define IPIPE_GIC_CLK  G1
#define IPIPE_WB_CLK  G1
#define IPIPE_CGS_CLK G1
#define IPIPE_HST_CLK G1

#define IPIPE_BOXCAR_CLK  G0

typedef uint32 ipipe_module_clk;

/* ================================================================ */
/**
 *Description:-enum for clk ctrl
 */
 /*================================================================== */
typedef enum {

    IPIPE_MODULE_CLK_DISABLE = 0,
    IPIPE_MODULE_CLK_ENABLE = 1
} IPIPE_MODULE_CLK_CTRL_T;

/* ==========================================================================
 */
/**
 * IPIPE_HIST_COLOR_T    Histogram color enumeration
 *
 * @param    IPIPE_HIST_R - Red color histogram
 *
 * @param    IPIPE_HIST_G - Green color histogram
 *
 * @param    IPIPE_HIST_B - Blue color histogram
 *
 * @param    IPIPE_HIST_Y - Luminance histogram
*/
/* ==========================================================================
 */
typedef enum {
    IPIPE_HIST_R = 0,
    IPIPE_HIST_B = 1,
    IPIPE_HIST_G = 2,
    IPIPE_HIST_Y = 3
} IPIPE_HIST_COLOR_T;

/* ================================================================ */
/**
 *Description:-main setup struct for ipipe
 */
 /*================================================================== */
typedef struct {

    uint32 filter_flag;

    ipipe_src_cfg_t *src_cfg;
    ipipe_dpc_lut_cfg_t *dpc_lut_params;
    ipipe_dpc_otf_cfg_t *dpc_otf_params;
    ipipe_noise_filter_cfg_t *nf1_params;
    ipipe_noise_filter_cfg_t *nf2_params;
    ipipe_lsc_cfg_t *lsc_params;
    ipipe_gic_cfg_t *gic_params;
    ipipe_wb_cfg_t *wb_params;
    ipipe_cfa_cfg_t *cfa_params;

    ipipe_rgb_rgb_cfg_t *rgb_rgb1_params;
    ipipe_gamma_cfg_t *gamma_params;
    ipipe_rgb_rgb_cfg_t *rgb_rgb2_params;
    ipipe_3d_lut_cfg_t *ipipe_3d_lut_params;

    ipipe_rgb_yuv_cfg_t *rgb_yuv_params;

    ipipe_gbce_cfg_t *gbce_params;

    /* 422 */
    ipipe_yuv444_yuv422_cfg_t *yuv444_yuv422_params;

    ipipe_ee_cfg_t *ee_param;
    ipipe_car_cfg_t *car_params;

    ipipe_cgs_cfg_t *cgs_params;
    ipipe_boxcar_cfg_t *boxcar_params;
    ipipe_hist_cfg_t *histogram_params;
    ipipe_bsc_cfg_t *bsc_params;

} ipipe_cfg_t;

/* ================================================================ */
/* FUNCTION PROTOTYPES */
 /*===============================================================*/

/* ================================================================ */
/**
 *  Description:- Init should be called before calling any other function
 *
 *
 *  @param   none

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_init
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
ISP_RETURN ipipe_init();

/* ================================================================ */
/**
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable ipipe clk
 *
 *
 *  @param   :- none

 *  @return  :- ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_open
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
ISP_RETURN ipipe_open();

/* ================================================================ */
/**
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler
 *
 *
 *  @param   none

 *  @return   ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_close
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
ISP_RETURN ipipe_close();

/* ================================================================ */
/**
 *  Description:- this is the main cfg call, it sets up all the required filters,
 *                ip/op pix types.
 *
 *
 *  @param  ipipe_cfg_t

 *  @return ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_config
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
ISP_RETURN ipipe_config(ipipe_cfg_t * ipipe_config);

/* ===================================================================
 *  @func     ipipe_config_filters
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
ISP_RETURN ipipe_config_filters(ipipe_cfg_t * ipipe_config);

/* ================================================================ */
/**
 *  Description:- This routine will start /stop the ipipe module,
 *                 start is of one shot type or continuous typt
 *
 *
 *  @param IPIPE_START_T  - enumeration of the start / stop commands

 *  @return         ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_start
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
ISP_RETURN ipipe_start(IPIPE_START_T enable);

/* ================================================================ */
/**
 *  Description:-This routine configures the LUT Defect Pixel correction
 *                 block .
 *
 *  @param   ipipe_dpc_lut_cfg_t -pointer to cfg params

 *  @return     return
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_config_DPC_lut
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
ISP_RETURN ipipe_config_DPC_lut(ipipe_dpc_lut_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the ipipe On-the fly Defect pixel correction module
 *
 *
 *  @param   :- ipipe_dpc_otf_cfg_t*  -pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_DPC_otf
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
ISP_RETURN ipipe_config_DPC_otf(ipipe_dpc_otf_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures one of the 2 the noise filters specified by "noise filter",
 *			   with the params specified in "cfg"
 *
 *
 *  @param   IPIPE_NOISE_FILTER_T:- specifies the noise filter
 *			ipipe_noise_filter_cfg_t* :-pointer to  the cfg of the noise filter
 *
 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_noise_filter
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
ISP_RETURN ipipe_config_noise_filter(IPIPE_NOISE_FILTER_T noise_filter,
                                     ipipe_noise_filter_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the Green Imbalance Correction module ipipe
 *
 *
 *  @param   :- ipipe_gic_cfg_t*-pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_gic_filter
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
ISP_RETURN ipipe_config_gic_filter(ipipe_gic_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the White Balance module in ipipe
 *
 *
 *  @param   :- ipipe_wb_cfg_t* - pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_config_wbal
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
ISP_RETURN ipipe_config_wbal(ipipe_wb_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the CFA module in ipipe
 *
 *
 *  @param   :- ipipe_cfa_cfg_t* -pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_cfa
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
ISP_RETURN ipipe_config_cfa(ipipe_cfa_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures one of the two  RGB to RGB blending filters in ipipe,
 *
 *
 *  @param   :-ipipe_rgb_rgb_cfg_t- pointer to cfg params
 IPIPE_RGB_RGB_FILTER_NUM_T - specifies the RGB to RGB blending filters number

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_rgb_to_rgb
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
ISP_RETURN ipipe_config_rgb_to_rgb(IPIPE_RGB_RGB_FILTER_NUM_T filter_num,
                                   ipipe_rgb_rgb_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the Gamma correction filter in ipipe
 *
 *
 *  @param   :- ipipe_gamma_cfg_t *:-pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_gamma
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
ISP_RETURN ipipe_config_gamma(ipipe_gamma_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the 3D Look up Table based correction module in ipipe
 *
 *
 *  @param  ipipe_3d_lut_cfg_t* :- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_config_3d_lut
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
ISP_RETURN ipipe_config_3d_lut(ipipe_3d_lut_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine will configure the RGB yo YUV 444 conversion filter in ipipe
 *
 *
 *  @param   ipipe_3d_lut_cfg_t *:- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_config_rgb2ycbcr
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
ISP_RETURN ipipe_config_rgb2ycbcr(ipipe_rgb_yuv_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine will configure the Global Brightness and contrast enhancement module in ipipe
 *
 *
 *  @param   ipipe_gbce_cfg_t * :- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_GBCE
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
ISP_RETURN ipipe_config_GBCE(ipipe_gbce_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine will configure the Edge Enhancement module in ipipe
 *
 *
 *  @param   ipipe_ee_cfg_t*:- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_edge_enhancement
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
ISP_RETURN ipipe_config_edge_enhancement(ipipe_ee_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routinewill configure the Chroma Artiffact redustion filter in ipipe
 *
 *
 *  @param   ipipe_car_cfg_t* :- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_car
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
ISP_RETURN ipipe_config_car(ipipe_car_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine will configure the Chroma Supression unit in ipipe
 *
 *
 *  @param   ipipe_cgs_cfg_t*:- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_chroma_supression
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
ISP_RETURN ipipe_config_chroma_supression(ipipe_cgs_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine will configure the Lens Shading Component in ipipe
 *
 *
 *  @param  ipipe_lsc_cfg_t*:-  pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_lsc
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
ISP_RETURN ipipe_config_lsc(ipipe_lsc_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the input pixel format , and the position of color components in the RAw pixel data,
 *			    and the dimensions of the input data
 *
 *
 *  @param   ipipe_src_cfg_t* pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_input_src
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
ISP_RETURN ipipe_config_input_src(ipipe_src_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the input dimensions of the data  to ipipe module ,
 *				this can be used at run time during V-blank to change the dimensions , for eg to crop the input data
 *
 *
 *  @param   ipipe_dims_t*:- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_dimensions
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
ISP_RETURN ipipe_config_dimensions(ipipe_dims_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine controls the change of the YUV 444 to YUV 422 conversions it select the co-sited or
 *			centerd phase for the y samples
 *
 *
 *  @param   ipipe_yuv444_yuv422_cfg_t* pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
ISP_RETURN ipipe_config_yuv444_to_yuv422(ipipe_yuv444_yuv422_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configures the BOXCAR filter in ipipe.
 *
 *
 *  @param   ipipe_boxcar_cfg_t* pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_boxcar
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
ISP_RETURN ipipe_config_boxcar(ipipe_boxcar_cfg_t * cfg);
/* ================================================================ */
/**
 *  Description:-This routine configures the BOXCAR filter Address in ipipe.
 *
 *
 *  @param   ipipe_boxcar_cfg_t* pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_boxcar_addr
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
ISP_RETURN ipipe_config_boxcar_addr(uint32 addr);
/* ================================================================ */
/**
 *  Description:-This routine configures the Histogram unit in ipipe
 *
 *
 *  @param  ipipe_hist_cfg_t*:- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_histogram
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
ISP_RETURN ipipe_config_histogram(ipipe_hist_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine configurs the Bondary signal calculator module in ipipe
 *
 *
 *  @param   ipipe_bsc_cfg_t* :- pointer to cfg params

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_config_bsc
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
ISP_RETURN ipipe_config_bsc(ipipe_bsc_cfg_t * cfg);

/* ================================================================ */
/**
 *  ipipe_config_bsc()
 *  Description:-This routine return the address of the bsc region in ISP internal memory map
 *
 *
 *  @param   uint32 *  ptr  ptr:- pointer to the table address
 *
 *  @return    ISP_RETURN
 */
/*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_bsc_address
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
ISP_RETURN ipipe_get_bsc_address(uint32 * ptr, IPIPE_BSC_MEMORY_T table_select);

/* ==========================================================================
 */
/**
 * ipipe_get_histogram()    The function copies the computed histogram into the
 * buffer pointed by the user. The user points which color and region histogram
 * is to be copied
 *
 * @param  uint32 *hst_buff - Pointer to the user histogram buffer where the
 *                            histogram is to be copied (the buffer size should
 *                            be enough to hold the histogram data - the size
 *                            depends on the region size - max is 256x32 bits)
 *
 * @param  uint32 region_num - Number of the region which histogram is to be
 *                             copied (0 to 3)
 *
 * @param  IPIPE_HIST_COLOR_T color - Histogram of which color is to be copied
 *
 * @return  uint32 - Size of the histogram returned (in 32 bit words of which
 *                   only the 20 LSbits are valid, others are 0). Max is 256.
 *                   Returns 0 on error
*/
/* ==========================================================================
 */
/* ===================================================================
 *  @func     ipipe_get_histogram
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
uint32 ipipe_get_histogram(uint32 * hst_buff, uint32 region_num,
                           IPIPE_HIST_COLOR_T color);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of DPC lut module
 *
 *
 *  @param   :- ipipe_dpc_lut_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_DPC_lut
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
ISP_RETURN ipipe_get_config_DPC_lut(ipipe_dpc_lut_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of DPC OTF module
 *
 *
 *
 *  @param   ipipe_dpc_otf_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_DPC_otf
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
ISP_RETURN ipipe_get_config_DPC_otf(ipipe_dpc_otf_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  one of the 2 noise filters specified.
 *
 *
 *
 *  @param   IPIPE_NOISE_FILTER_T:- specifies the noise filter
 *			ipipe_noise_filter_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_noise_filter
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
ISP_RETURN ipipe_get_config_noise_filter(IPIPE_NOISE_FILTER_T noise_filter,
                                         ipipe_noise_filter_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  Green Imbalance correction
 *
 *
 *  @param   ipipe_gic_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_config_gic_filter
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
ISP_RETURN ipipe_get_config_gic_filter(ipipe_gic_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of    White Balance filter
 *
 *
 *  @param   ipipe_wb_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_config_wbal
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
ISP_RETURN ipipe_get_config_wbal(ipipe_wb_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of     CFA filter
 *
 *
 *  @param   ipipe_cfa_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_config_cfa
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
ISP_RETURN ipipe_get_config_cfa(ipipe_cfa_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  one of the 2 RGB to RGB blending modules
 *
 *
 *  @param  IPIPE_RGB_RGB_FILTER_NUM_T:- specifies one of the two filters
 *   			ipipe_rgb_rgb_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_rgb_to_rgb
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
ISP_RETURN ipipe_get_config_rgb_to_rgb(IPIPE_RGB_RGB_FILTER_NUM_T filter_num,
                                       ipipe_rgb_rgb_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  Gamma correction filters
 *
 *
 *  @param   ipipe_gamma_cfg_t*:- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_gamma
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
ISP_RETURN ipipe_get_config_gamma(ipipe_gamma_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  3D LUT module in ipippe
 *
 *
 *  @param   ipipe_3d_lut_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_3d_lut
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
ISP_RETURN ipipe_get_config_3d_lut(ipipe_3d_lut_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of   RGB to YUV444 conversion module
 *
 *
 *  @param   ipipe_rgb_yuv_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_rgb2ycbcr
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
ISP_RETURN ipipe_get_config_rgb2ycbcr(ipipe_rgb_yuv_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  Global brightness and Contrast enhancement module
 *
 *
 *  @param   ipipe_gbce_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_config_GBCE
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
ISP_RETURN ipipe_get_config_GBCE(ipipe_gbce_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of     Edge Enhancement
 *
 *
 *  @param   ipipe_ee_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_edge_enhancement
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
ISP_RETURN ipipe_get_config_edge_enhancement(ipipe_ee_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of    CAR module in ipipe
 *
 *
 *  @param   ipipe_car_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_config_car
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
ISP_RETURN ipipe_get_config_car(ipipe_car_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of     CAS module
 *
 *
 *  @param   ipipe_cgs_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_chroma_supression
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
ISP_RETURN ipipe_get_config_chroma_supression(ipipe_cgs_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  i/p pixel format and the pixel pos of color components
 *
 *
 *  @param   ipipe_src_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipe_get_config_input_src
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
ISP_RETURN ipipe_get_config_input_src(ipipe_src_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the cfg of  YUV444 to YUV422
 *
 *
 *  @param   ipipe_yuv444_yuv422_cfg_t* :- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
 /*================================================================== */
ISP_RETURN ipipe_get_config_yuv444_to_yuv422(ipipe_yuv444_yuv422_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-This routine gets the address of  3D LUT tables in ipippe
 *
 *
 *  @param   tbl_addr :- pointer to table addres
 *  @param   IPIPE_3DLUT_MEMORY_T :- table numbre (0-3)
 *
 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipe_get_3dlut
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
ISP_RETURN ipipe_get_3dlut(uint32 * tbl_addr, uint8 table);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
