/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file test.c
 *		This file contains the test cases for testing ISP5 CSL .
 * 
 *
 * @path Centaurus\drivers\drv_isp\in\csl
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 *========================================================================= */

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>

#include <ti/psp/iss/hal/iss/isp/isp.h>

#include <ti/psp/iss/hal/iss/common/csl_utils/csl_types.h>

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

#include <ti/psp/iss/hal/iss/csi2/csi2.h>

#include <ti/psp/iss/hal/iss/ccp2/ccp2.h>
#include <osal/timm_osal_error.h>
#include <osal/timm_osal_memory.h>

#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <ti/psp/iss/hal/iss/cbuff/cbuff.h>
#include <ti/psp/iss/hal/iss/bte/bte_drv.h>

// #include "../../cbuff/inc/Sys_types.h"
#define ZEBU 0
// 1 for Zebu
// 0 for simulator

#define img_height  480
#define img_width  640

#define pix_size 2                                         // in terms of
                                                           // bytes

uint8 *img_buff;                                           // [img_width*img_height*pix_size]; 
                                                           // //ip_img_buffer

uint8 *op_img_buff;                                        // [img_width*img_height*pix_size];//op/of 
                                                           // first resizer-A 
                                                           // ;sccaled down
                                                           // by 2
uint8 *op_img_buff_1;                                      // [img_width*img_height*pix_size*4];//scaled 
                                                           // up by 2, op of
                                                           // resizer 2

int op_h3a_af_buff[60];

int op_h3a_aewb_buff[50];

#pragma DATA_ALIGN(op_h3a_af_buff,128);
#pragma DATA_ALIGN(op_h3a_aewb_buff,128);

// #pragma DATA_SECTION(op_img_buff,"isptest");
// #pragma DATA_SECTION(op_img_buff_1,"isptest");
// #pragma DATA_SECTION(img_buff,"isptest");

/*****************TEST configuration structuresfor other modules**********************************/
ipipe_cfg_t test_ipipe_cfg;

ipipeif_path_cfg_t test_ipipeif_cfg;

rsz_config_t test_rsz_config;

isif_cfg_t test_isif_cfg;

/****************************test cgf structure for rsz************************************************************/

rsz_ip_op_cfg_t default_rszA_ip_op_params;

rsz_ip_op_cfg_t default_rszB_ip_op_params;

rsz_resize_mode_cfg_t default_rsz_resizer_mode_cfg_params;

rsz_output_dims_t rszB_op_dims;

rsz_resize_ratio_t rszB_op_ratio;

rsz_output_addr_t rszB_op_address;                         /* This is used
                                                            * for raw and
                                                            * yuv422/RGB op */
rsz_output_addr_t rszB_c_op_address;                       /* For yuv420 op */

rsz_mode_cfg_t default_rsz_mode_cfg_params;

rsz_output_addr_t rsz_A_op_addr;                           /* This is used
                                                            * for raw and
                                                            * yuv422/RGB op */
rsz_output_addr_t rsz_A_c_op_addr;                         /* For yuv420 op */

rsz_rgb_op_cfg_t rsz_B_rgb_params;

rsz_passthru_mode_cfg_t default_rsz_passthru_params_2;

rsz_mode_cfg_t default_rsz_mode_params_2;

rsz_bypass_mode_cfg_t default_rsz_bp_params;

/* test cffg structs for CSI2 */
// csi2_cfg_t test_csi2_config;

/* test configuration structure for h3a */

h3a_cfg_t test_h3a_cfg;

h3a_common_cfg_t default_h3a_common_params;

h3a_dims_t default_h3a_ip_dims;

h3a_aewb_param_t default_h3a_aewb_params;

h3a_aewb_blk_dims_t default_h3a_aewb_blk_win_params;

h3a_af_param_t default_h3a_af_params;

h3a_paxel_cfg_t default_h3a_aewb_paxel_params;

h3a_paxel_cfg_t default_h3a_af_paxel_params;

/*********************DEFULT CCP2 PARAMS***************************************/

const ccp2_global_ctrl ccp2_global_ctrl_default_params = {
    FALLING,
    1,
    INV,
    0,
    CTRL_BURST_TWO_X_SIXTY_FOUR,
    NONMIPI_CCP2_MODE,
    FRAME_DIS_IMME,
    OUTPUT_PARALLEL,
    DATA_CLOCK_PHY_SEL,
    INTERFACE_DISABLE_IMME,
    POSTED,
    16384,
    VP_CLOCK_DISABLE,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVH,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVL,
    SIXTEEN_CYCLES
};

/* 
 * const ccp2_global_ctrl ccp2_global_ctrl_default_params_12 = { FALLING, 1,
 * INV, 0, CTRL_BURST_TWO_X_SIXTY_FOUR, NONMIPI_CCP2_MODE, FRAME_DIS_IMME,
 * OUTPUT_PARALLEL, DATA_CLOCK_PHY_SEL, INTERFACE_DISABLE_IMME, POSTED,
 * 65536/10, LEVL_LESS_THAN_FIFO_LEV_FOR_LEVH,
   * LEVL_LESS_THAN_FIFO_LEV_FOR_LEVL, SIXTEEN_CYCLES }; *//* santosh */

/* Murielle */
const ccp2_global_ctrl ccp2_global_ctrl_default_params_12 = {
    RISING,
    1,
    NON_INV,
    0,
    CTRL_BURST_TWO_X_SIXTY_FOUR,
    NONMIPI_CCP2_MODE,
    FRAME_DIS_IMME,
    OUTPUT_SERIAL,
    DATA_CLOCK_PHY_SEL,
    INTERFACE_DISABLE_IMME,
    NON_POSTED,
    65536 / 10,
    VP_CLOCK_DISABLE,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVH,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVL,
    SIXTEEN_CYCLES
};

const ccp2_lcm_irqenable_status ccp2_lcm_irqenable_status_default_params = {
    1,
    1
};

const ccp2_lcm_ctrl ccp2_lcm_ctrl_default_params = {
    DST_PACK_ENABLED,
    ADV_PREDICTOR,
    CONTROL_ENABLE,
    LCM_RAW10,
    SRC_PACK_ENABLED,
    ADV_PREDICTOR,
    CONTROL_ENABLE,
    LCM_RAW10,
    CTRL_BURST_FOUR_X_SXITY_FOUR,
    FULL_SPEED,
    SEND_TO_MEMORY,
    3
};

ccp2_lcm_config ccp2_lcm_config_default_params = {
    0x04,
    0x01E,
    13,
    14,
    0x40000,
    16,
    17,
    18
};

const ccp2_lcm_ctrl ccp2_lcm_ctrl_default_dummy_params = {
    DST_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    CONTROL_DISABLE,
    LCM_RAW10,
    SRC_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    CONTROL_DISABLE,
    LCM_RAW10,
    CTRL_BURST_SIXTEEN_X_SIXTY_FOUR,
    FULL_SPEED,
    SEND_TO_VIDEO_NO_COMPRESSION,
    0
};

ccp2_lcm_config ccp2_lcm_config_default_dummy_params = {
    // Extra 16 lines for vertical blanking to flusj teh ISP pipeline
    // SKIP and OFFFSET is used to set horizontal blanking
    // 0x1f0,
    // 0x280,
#if ZEBU
    img_height + 16,

#else
    img_height,
#endif
    img_width,
    0x000,
    0xA0,
    0x0,
    0x0,
    0x00000000,
    0x0
};

const ccp2_lcm_ctrl ccp2_lcm_ctrl_default_dummy_params_12 = {
    DST_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    CONTROL_DISABLE,
    LCM_RAW10,
    SRC_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    CONTROL_DISABLE,
    LCM_RAW10,
    CTRL_BURST_SIXTEEN_X_SIXTY_FOUR,
    FULL_SPEED,
    SEND_TO_VIDEO_NO_COMPRESSION,
    0
};

const ccp2_lcm_ctrl ccp2_lcm_ctrl_default_dummy_params_20 = {
    DST_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    CONTROL_DISABLE,
    LCM_RAW10,
    SRC_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    CONTROL_DISABLE,
    LCM_RAW10,
    CTRL_BURST_SIXTEEN_X_SIXTY_FOUR,
    FULL_SPEED,
    SEND_TO_VIDEO_NO_COMPRESSION,
    1
};

ccp2_lcm_config ccp2_lcm_config_default_dummy_params_12 = {
    // Extra 16 lines for vertical blanking to flusj teh ISP pipeline
    // SKIP and OFFFSET is used to set horizontal blanking
    // 0x1f0,
    // 0x280,
#if ZEBU
    img_height + 16,
#else
    img_height,
#endif
    img_width,
    0x000,
    0xA0,
    0x0,
    0x0,
    // 0x28,
    0x60000000,
    0x0
};

ccp2_lcm_config ccp2_lcm_config_default_dummy_params_20 = {
    // Extra 16 lines for vertical blanking to flusj teh ISP pipeline
    // SKIP and OFFFSET is used to set horizontal blanking
    // 0x1f0,
    // 0x280,
    img_height + 16,
    img_width,
    0x000,
    0xA0,
    0x0,
    0x0,
    // 0x28,
    0x60000000,
    0x0
};

/*********************DEFULT CCP2 PARAMS end***************************************/

/*********************DEFULT IPIPE PARAMS***************************************/
ipipe_src_cfg_t default_ipipe_src_cfg;

ipipe_src_cfg_t default_ipipe_src_cfg_1;

/* Green Imbalance Config */
ipipe_gic_cfg_t gic_default_params = {
    1,                                                     // uint16 enable;
    IPIPE_GIC_LSC_GAIN_ON,                                 // ipipe_gic_lsc_gain 
                                                           // lsc_gain;
    IPIPE_GIC_NF2THR,                                      // ipipe_gic_sel
                                                           // sel;
    IPIPE_GIC_DIFF_INDEX,                                  // ipipe_gic_index 
                                                           // typ;
    128,                                                   // uint8 gic_gain;
    31,                                                    // uint8
                                                           // gic_nfgain;
    512,                                                   // uint8 gic_thr;
    512                                                    // uint8
                                                           // gic_slope;
};

/* LSC GAIN */
uint8 default_ipipe_histogram_mult[4] = { 64, 64, 64, 64 };

const ipipe_hist_dim_t default_hist_dims_t[4] = {
    {
     0,                                                    // uint16 v_pos;
     398,                                                  // uint16 v_size;
     0,                                                    // uint16 h_pos;
     398                                                   // uint16 h_size;
     },
    {
     0,                                                    // uint16 v_pos;
     398,                                                  // uint16 v_size;
     600,                                                  // uint16 h_pos;
     398                                                   // uint16 h_size;
     },
    {
     600,                                                  // uint16 v_pos;
     398,                                                  // uint16 v_size;
     0,                                                    // uint16 h_pos;
     398                                                   // uint16 h_size;
     },
    {
     600,                                                  // uint16 v_pos;
     398,                                                  // uint16 v_size;
     600,                                                  // uint16 h_pos;
     398                                                   // uint16 h_size;
     }

};

ipipe_hist_cfg_t default_histogram_params = {
    1,                                                     // uint8 enable;
    1,                                                     // uint8 ost;
    1,                                                     // uint8 sel;
    // ######## This is not in documentation(IPIPE_CONFIG.RTF) so
    // removed############
    0,                                                     // uint8
                                                           // type;doesnt
                                                           // matter if sel=1
    3,                                                     // uint8 bins;
    4,                                                     // uint8 shift;
    0x0f,                                                  // uint8
                                                           // col;/*bits
                                                           // [3:0], 0 is
                                                           // disable*/
    0x1,                                                   // uint8 regions;
                                                           // /*[3:0], 0 is
                                                           // disable*/
    (ipipe_hist_dim_t *) default_hist_dims_t,              // ipipe_hist_dim_t 
                                                           // *hist_dim;/*pointer 
                                                           // to array of 4
                                                           // structs*/
    0,                                                     // uint8
                                                           // clear_table;
    9,                                                     // uint8
                                                           // table_sel;
    default_ipipe_histogram_mult                           // uint8 mul4;
};

const ipipe_bsc_cfg_t default_bsc_params = {
    1,                                                     // uint8 enable;
    1,                                                     // uint8 mode;
    1,                                                     // uint8
                                                           // col_sample;
    1,                                                     // uint8
                                                           // row_sample;
    0x1,                                                   // uint8
                                                           // element;/*Y or
                                                           // CB or CR*/
    // ipipe_bsc_pos_params_t col_pos;
    {
     0x3,                                                  // uint8 vectors;
     0x0,                                                  // uint8 shift;
     0x128,                                                // uint16 v_pos;
     0x128,                                                // uint16 h_pos;
     0x63,                                                 // uint16 v_num;
     0x31,                                                 // uint16 h_num;
     0x1,                                                  // uint8 v_skip;
     0x7                                                   // uint8 h_skip;
     },
    // ipipe_bsc_pos_params_t row_pos;
    {
     0x3,                                                  // uint8 vectors;
     0x0,                                                  // uint8 shift;
     0x32,                                                 // uint16 v_pos;
     0x32,                                                 // uint16 h_pos;
     0x32,                                                 // uint16 v_num;
     0x31,                                                 // uint16 h_num;
     0x5,                                                  // uint8 v_skip;
     0x7                                                   // uint8 h_skip;
     }

};

const ipipe_lsc_cfg_t lsc_default_params = {

    0,                                                     // uint16
                                                           // v_offset;
    0,                                                     // int16
                                                           // v_linear_coeff;//va1
    0,                                                     // int16
                                                           // v_quadratic_coeff;//va2
    0,                                                     // uint8
                                                           // v_linear_shift;//vs1
    0,                                                     // uint8
                                                           // v_quadratic_shift;//vs2

    0,                                                     // uint16
                                                           // h_offset;
    0,                                                     // int16
                                                           // h_linear_coeff;//va1
    0,                                                     // int16
                                                           // h_quadratic_coeff;//va2
    0,                                                     // uint8
                                                           // h_linear_shift;//vs1
    0,                                                     // uint8
                                                           // h_quadratic_shift;//vs2

    255,                                                   // uint8 gain_r;

    255,                                                   // uint8 gain_gr;

    255,                                                   // uint8 gain_gb;

    255,                                                   // uint8 gain_b;

    32,                                                    // uint8 off_r;
    32,                                                    // uint8 off_gr;
    32,                                                    // uint8 off_gb;
    32,                                                    // uint8 off_b;

    0,                                                     // uint8 shift;
    511                                                    // uint16 max
};

/* Noise Filter */

uint16 nf_thr[8] = { 120, 130, 135, 140, 150, 160, 170, 200 };
uint8 nf_str[8] = { 16, 16, 15, 15, 15, 15, 15, 15 };
uint8 nf_spr[8] = { 3, 3, 3, 3, 3, 3, 3, 3 };

const ipipe_noise_filter_cfg_t nf1_default_params = {
    1,                                                     // uint8 enable;
    NOISE_FILTER_1,                                        // noise_filter_t
                                                           // nf_num;
    IPIPE_NF_SPR_LUT,                                      // ###########TODO############################### 
                                                           // ipipe_nf_sel
                                                           // sel;
    IPIPE_NF_LSC_GAIN_ON,                                  // IPIPE_NF_LSC_GAIN_T 
                                                           // lsc_gain;/*0*/
    IPIPE_NF_SAMPLE_BOX,                                   // IPIPE_NF_SAMPLE_METHOD_T 
                                                           // typ;/*1*/
    0,                                                     // uint8
                                                           // down_shift_val;/*3*/
    3,                                                     // uint8
                                                           // spread;/*4*/
    nf_thr,                                                /* 10 */
    nf_str,                                                /* 5 */
    nf_spr,                                                /* 5 */
    0,                                                     // uint16
                                                           // edge_min;/*10*/
    2048                                                   // uint16
                                                           // edge_max;/*10*/
};

const ipipe_noise_filter_cfg_t nf2_default_params = {
    1,                                                     // uint8 enable;
    NOISE_FILTER_2,                                        // noise_filter_t
                                                           // nf_num;
    IPIPE_NF_SPR_LUT,                                      // ###########TODO############################### 
                                                           // ipipe_nf_sel
                                                           // sel;
    IPIPE_NF_LSC_GAIN_ON,                                  // IPIPE_NF_LSC_GAIN_T 
                                                           // lsc_gain;/*0*/
    IPIPE_NF_SAMPLE_BOX,                                   // IPIPE_NF_SAMPLE_METHOD_T 
                                                           // typ;/*1*/
    0,                                                     // uint8
                                                           // down_shift_val;/*3*/
    3,                                                     // uint8
                                                           // spread;/*4*/
    nf_thr,                                                /* 10 */
    nf_str,                                                /* 5 */
    nf_spr,                                                /* 5 */
    0,                                                     // uint16
                                                           // edge_min;/*10*/
    2048                                                   // uint16
                                                           // edge_max;/*10*/
};

/* WHITE BALANCE PARAMETER */
const uint16 wb_default_config_offset[4] = { 0, 0, 0, 0 };
const uint16 wb_default_config_gain[4] = { 1200, 512, 512, 880 };

ipipe_wb_cfg_t wb_default_params = {

    (uint16 *) wb_default_config_offset,
    (uint16 *) wb_default_config_gain
};

/* CFAI PARMATERS */
const ipipe_cfa_cfg_t cfa_default_params = {
    1,                                                     // uint8 enable;
    IPIPE_CFA_MODE_2DIR,                                   // IPIPE_CFA_MODE_T 
                                                           // mode;
    {
     1024,                                                 // uint16 hpf_thr;
     0,                                                    // uint16
                                                           // hpf_slope;
     30,                                                   // uint16 mix_thr;
     10,                                                   // uint16
                                                           // mix_slope;
     4,                                                    // uint16 dir_thr;
     10,                                                   // uint16
                                                           // dir_slope;
     16                                                    // uint16
                                                           // dir_ndwt;
     },                                                    // ipipe_cfa_dir_t 
                                                           // dir;
    {
     24,                                                   // uint8
                                                           // mono_hue_fra;
     25,                                                   // uint8
                                                           // mono_edg_thr;
     27,                                                   // uint16
                                                           // mono_thr_min;
     20,                                                   // uint16
                                                           // mono_thr_slope;
     50,                                                   // uint16
                                                           // mono_slp_min;
     40,                                                   // uint16
                                                           // mono_slp_slp;
     16                                                    // uint16
                                                           // mono_lpwt;
     }                                                     // ipipe_cfa_daa_t 
                                                           // daa;
};

/* RGB Colour Correction */
const uint16 rgb_rgb1_default_gainparams[9] =
    { 256, 0, 0, 0, 256, 0, 0, 0, 256 };
const uint16 rgb_rgb1_default_offsetparams[3] = { 0, 0, 0 };

ipipe_rgb_rgb_cfg_t rgb_rgb_default_params = {

    (int16 *) rgb_rgb1_default_gainparams,
    (int16 *) rgb_rgb1_default_offsetparams
};

/* Gamma */
const int16 gamma_default_table[] = {
#include "tables/gamma_table.h"
};

ipipe_gamma_cfg_t gamma_default_params = {

    IPIPE_GAMMA_TBL_512,                                   // IPIPE_GAMMA_TABLE_SIZE_T 
                                                           // gamma_tbl_size;
    0,                                                     // uint8 tbl;//may 
                                                           // not be
                                                           // needed.since
                                                           // table is always 
                                                           // in RAM
    IPIPE_GAMMA_BYPASS_DISABLE,                            // IPIPE_GAMMA_BYPASS_T 
                                                           // bypass_b;/*o
                                                           // not bypassed*/
    IPIPE_GAMMA_BYPASS_DISABLE,                            // IPIPE_GAMMA_BYPASS_T 
                                                           // bypass_g;
    IPIPE_GAMMA_BYPASS_DISABLE,                            // IPIPE_GAMMA_BYPASS_T 
                                                           // bypass_r;
    (int16 *) gamma_default_table,                         // uint32 *
                                                           // red_table;
    (int16 *) gamma_default_table,                         // uint32 *
                                                           // blue_table;
    (int16 *) gamma_default_table                          // uint32*
                                                           // green_table;
};

/* RGB to RGB2 */
uint16 rgb_rgb2_default_gainparams[9] = { 256, 0, 0, 0, 256, 0, 0, 0, 256 };
uint16 rgb_rgb2_default_offsetparams[3] = { 0, 0, 0 };

ipipe_rgb_rgb_cfg_t rgb_rgb2_default_params = {

    (int16 *) rgb_rgb2_default_gainparams,
    (int16 *) rgb_rgb2_default_offsetparams
};

/* RGB to YUV */
int16 rgb_yuv_default_mult[9] = {
    77,
    150,                                                   // #
                                                           // IPIPE_YUV_MUL_GY 
                                                           // 
    29,                                                    // #
                                                           // IPIPE_YUV_MUL_BY 
                                                           // 
    -43,                                                   // #
                                                           // IPIPE_YUV_MUL_RCB 
                                                           // 
    -85,                                                   // #
                                                           // IPIPE_YUV_MUL_GCB 
                                                           // 
    128,                                                   // #
                                                           // IPIPE_YUV_MUL_BCB 
                                                           // 
    128,                                                   // #
                                                           // IPIPE_YUV_MUL_RCR 
                                                           // 
    -107,                                                  // #
                                                           // IPIPE_YUV_MUL_GCR
    -21,                                                   // #
                                                           // IPIPE_YUV_MUL_BCR 
                                                           // 
};
int16 rgb_yuv_default_offset[3] = { 0, 0x80, 0x80 };       /* #####changed */

ipipe_rgb_yuv_cfg_t rgb_yuv_default_params = {

    16,                                                    // uint8
                                                           // brightness;
    16,                                                    // uint8 contrast;
    rgb_yuv_default_mult,                                  // int16
                                                           // *mul_val;/*RY,GY,BY,RCB,GCB,BCB 
                                                           // ,RCR,GCR,BCR 12 
                                                           // bits*/

    rgb_yuv_default_offset,                                // int16*
                                                           // offset;/*Y,CB,CR 
                                                           // -11bits*/

};

 /*GBCE*/ const uint16 gbce_default_table[] = {
#include "tables/gbce_table.h"
};

ipipe_gbce_cfg_t gbce_default_params = {
    1,                                                     // uint16 enable;
    IPIPE_GBCE_METHOD_GAIN_TBL,                            // uint16 typ;
    (uint16 *) gbce_default_table                          // uint8 *
                                                           // lookup_table;
};

ipipe_yuv444_yuv422_cfg_t yuv444_yuv_422_default_params = {
    IPIPE_YUV_PHS_POS_COSITED,
    0
};

/* EE */
int16 ee_mul_val_default[9] = {
    84,
    -8,
    -4,
    -8,
    -4,
    -2,
    -4,
    -2,
    -1
};

ipipe_car_cfg_t default_car_params = {
    1,                                                     // uint8 enable;

    1,                                                     // uint8 typ;

    0xff,                                                  // uint8 sw0_thr;
    0xc0,                                                  // uint8 sw1_thr;

    0x3,                                                   // uint8 hpf_type;
    0,                                                     // uint8
                                                           // hpf_shift;
    32,                                                    // uint8 hpf_thr;

    255,                                                   // uint8 gn1_gain;
    5,                                                     // uint8
                                                           // gn1_shift;
    128,                                                   // uint16 gn1_min;

    255,                                                   // uint8 gn2_gain;
    6,                                                     // uint8
                                                           // gn2_shift;
    128                                                    // uint16 gn2_min;
};

const int16 ee_default_table[] = {
#include "tables/edge_enhance_table.h"
};

ipipe_ee_cfg_t edge_enhance_default_params = {
    1,                                                     // uint16 enable;
    IPIPE_HALO_REDUCTION_ENABLE,                           // uint8
                                                           // halo_reduction;
    ee_mul_val_default,                                    // int16
                                                           // *mul_val;/*9 co 
                                                           // -efficients*/
    1,                                                     // uint8 sel;
    10,                                                    // uint8 shift_hp;
    12,                                                    // uint16
                                                           // threshold;
    32,                                                    // uint16 gain;
    128,                                                   // uint8
                                                           // hpf_low_thr;
    768,                                                   // ####################TO 
                                                           // check these
                                                           // values uint8
                                                           // hpf_high_thr;
    32,                                                    // uint8
                                                           // hpf_gradient_gain;
    0,                                                     // uint8
                                                           // hpf_gradient_offset;
    (int16 *) ee_default_table
};

ipipe_boxcar_cfg_t default_boxcar_params = {
    0x1,                                                   // uint8 enable;
    0x1,                                                   // uint8 ost;/*one 
                                                           // shot or free
                                                           // run*/
    0,                                                     // uint8 box_size;
    0,                                                     // uint8 shift;
    NULL                                                   // default_boxcar_op 
                                                           // // uint32 addr;
};

// #pragma DATA_ALIGN(default_baxcar_op,32);
uint8 *default_boxcar_op;                                  // [50000];

ipipe_params_init_1(ipipe_cfg_t * cfg)
{

    cfg->src_cfg = &default_ipipe_src_cfg;
    // cfg->dpc_lut_params=&default_dpc_lut_params;
    // cfg->dpc_otf_params=&default_dpc_otf_params;
    cfg->nf1_params = (ipipe_noise_filter_cfg_t *) & nf1_default_params;
    cfg->nf2_params = (ipipe_noise_filter_cfg_t *) & nf2_default_params;

    cfg->gic_params = &gic_default_params;
    cfg->wb_params = &wb_default_params;
    cfg->cfa_params = (ipipe_cfa_cfg_t *) & cfa_default_params;

    cfg->rgb_rgb1_params = &rgb_rgb_default_params;
    cfg->gamma_params = &gamma_default_params;
    cfg->rgb_rgb2_params = &rgb_rgb2_default_params;
    // cfg->ipipe_3d_lut_params=&default_ipipe_3d_lut_params;

    cfg->rgb_yuv_params = &rgb_yuv_default_params;

    cfg->gbce_params = &gbce_default_params;

    cfg->yuv444_yuv422_params = &yuv444_yuv_422_default_params;

    cfg->ee_param = &edge_enhance_default_params;
    cfg->car_params = &default_car_params;
    cfg->lsc_params = (ipipe_lsc_cfg_t *) & lsc_default_params;
    // cfg->cgs_params=&default_cgs_params;
    default_boxcar_params.addr = (uint32) default_boxcar_op;
    cfg->boxcar_params = &default_boxcar_params;
    cfg->histogram_params = &default_histogram_params;
    cfg->bsc_params = (ipipe_bsc_cfg_t *) & default_bsc_params;

    cfg->filter_flag = 0;

    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = img_width;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = img_height;

    cfg->src_cfg->wrt = 0;
    cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_YCRCB_OUTPUT;

    cfg->src_cfg->Even_line_Even_pixel = IPIPE_PIXEL_COLOUR_B;
    cfg->src_cfg->Even_line_Odd_pixel = IPIPE_PIXEL_COLOUR_GB;
    cfg->src_cfg->Odd_line_Even_pixel = IPIPE_PIXEL_COLOUR_GR;
    cfg->src_cfg->Odd_line_Odd_pixel = IPIPE_PIXEL_COLOUR_R;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_1_FLAG;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_2_FLAG;

    cfg->filter_flag |= IPIPE_GIC_FLAG;

    cfg->filter_flag |= IPIPE_WB_FLAG;

    cfg->filter_flag |= IPIPE_CFA_FLAG;

    cfg->filter_flag |= IPIPE_RGB_RGB_1_FLAG;

    cfg->filter_flag |= IPIPE_RGB_RGB_2_FLAG;

    cfg->filter_flag |= IPIPE_GAMMA_FLAG;

    cfg->filter_flag |= IPIPE_EE_FLAG;

    cfg->filter_flag |= IPIPE_RGB_TO_YUV_FLAG;
    cfg->filter_flag |= IPIPE_CAR_FLAG;
    cfg->filter_flag |= IPIPE_LSC_FLAG;

    // cfg->filter_flag|=IPIPE_GBCE_FLAG;

    cfg->filter_flag |= IPIPE_HST_FLAG;
    cfg->filter_flag |= IPIPE_BSC_FLAG;
#if ZEBU
    cfg->filter_flag |= IPIPE_BOXCAR_FLAG;
#endif
    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Configures IPIPE to not do a RAW to YUV conversion
 *
 *  @param   ipipe_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ipipe_params_init_2(ipipe_cfg_t * cfg)
{

    cfg->src_cfg = &default_ipipe_src_cfg;
    // cfg->dpc_lut_params=&default_dpc_lut_params;
    // cfg->dpc_otf_params=&default_dpc_otf_params;
    cfg->nf1_params = (ipipe_noise_filter_cfg_t *) & nf1_default_params;
    cfg->nf2_params = (ipipe_noise_filter_cfg_t *) & nf2_default_params;

    cfg->gic_params = &gic_default_params;
    cfg->wb_params = &wb_default_params;
    // cfg->cfa_params=(ipipe_cfa_cfg_t*)&cfa_default_params;

    // cfg->rgb_rgb1_params=&rgb_rgb_default_params;
    // cfg->gamma_params=&gamma_default_params;
    // cfg->rgb_rgb2_params=&rgb_rgb2_default_params;
    // cfg->ipipe_3d_lut_params=&default_ipipe_3d_lut_params;

    // cfg->rgb_yuv_params=&rgb_yuv_default_params;

    // cfg->gbce_params=&gbce_default_params;

    // cfg->yuv444_yuv422_params=&yuv444_yuv_422_default_params;

    // cfg->ee_param=&edge_enhance_default_params;
    // cfg->car_params=&default_car_params;
    cfg->lsc_params = (ipipe_lsc_cfg_t *) & lsc_default_params;
    // cfg->cgs_params=&default_cgs_params;
    // cfg->boxcar_params=&default_boxcar_params;
    // cfg->histogram_params=&default_histogram_params; 
    // cfg->bsc_params=(ipipe_bsc_cfg_t *)&default_bsc_params;

    cfg->filter_flag = 0;

    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = img_width;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = img_height;

    cfg->src_cfg->wrt = 1;
    cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_BAYER_OUTPUT;

    cfg->src_cfg->Even_line_Even_pixel = IPIPE_PIXEL_COLOUR_B;
    cfg->src_cfg->Even_line_Odd_pixel = IPIPE_PIXEL_COLOUR_GB;
    cfg->src_cfg->Odd_line_Even_pixel = IPIPE_PIXEL_COLOUR_GR;
    cfg->src_cfg->Odd_line_Odd_pixel = IPIPE_PIXEL_COLOUR_R;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_1_FLAG;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_2_FLAG;

    cfg->filter_flag |= IPIPE_GIC_FLAG;

    cfg->filter_flag |= IPIPE_WB_FLAG;

    // cfg->filter_flag|=IPIPE_CFA_FLAG;

    // cfg->filter_flag|=IPIPE_RGB_RGB_1_FLAG;

    // cfg->filter_flag|=IPIPE_RGB_RGB_2_FLAG;

    // cfg->filter_flag|=IPIPE_GAMMA_FLAG;

    // cfg->filter_flag|=IPIPE_EE_FLAG;

    // cfg->filter_flag|=IPIPE_RGB_TO_YUV_FLAG;
    // cfg->filter_flag|=IPIPE_CAR_FLAG;
    // cfg->filter_flag|=IPIPE_LSC_FLAG;

    // cfg->filter_flag|=IPIPE_GBCE_FLAG;

    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Ipipe does a RAW to YUV conversion and BSC and histogram are enabled
 *
 *  @param   ipipe_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ipipe_params_init_3(ipipe_cfg_t * cfg)
{

    cfg->src_cfg = &default_ipipe_src_cfg;
    // cfg->dpc_lut_params=&default_dpc_lut_params;
    // cfg->dpc_otf_params=&default_dpc_otf_params;
    cfg->nf1_params = (ipipe_noise_filter_cfg_t *) & nf1_default_params;
    cfg->nf2_params = (ipipe_noise_filter_cfg_t *) & nf2_default_params;

    cfg->gic_params = &gic_default_params;
    cfg->wb_params = &wb_default_params;
    cfg->cfa_params = (ipipe_cfa_cfg_t *) & cfa_default_params;

    cfg->rgb_rgb1_params = &rgb_rgb_default_params;
    cfg->gamma_params = &gamma_default_params;
    cfg->rgb_rgb2_params = &rgb_rgb2_default_params;
    // cfg->ipipe_3d_lut_params=&default_ipipe_3d_lut_params;

    cfg->rgb_yuv_params = &rgb_yuv_default_params;

    cfg->gbce_params = &gbce_default_params;

    cfg->yuv444_yuv422_params = &yuv444_yuv_422_default_params;

    cfg->ee_param = &edge_enhance_default_params;
    cfg->car_params = &default_car_params;
    cfg->lsc_params = (ipipe_lsc_cfg_t *) & lsc_default_params;
    // cfg->cgs_params=&default_cgs_params;
    // cfg->boxcar_params=&default_boxcar_params;
    cfg->histogram_params = &default_histogram_params;
    cfg->bsc_params = (ipipe_bsc_cfg_t *) & default_bsc_params;

    cfg->filter_flag = 0;

    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = img_width;
    cfg->src_cfg->dims.vps = 1;
    cfg->src_cfg->dims.v_size = img_height;

    cfg->src_cfg->wrt = 1;
    cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_YCRCB_OUTPUT;

    cfg->src_cfg->Even_line_Even_pixel = IPIPE_PIXEL_COLOUR_B;
    cfg->src_cfg->Even_line_Odd_pixel = IPIPE_PIXEL_COLOUR_GB;
    cfg->src_cfg->Odd_line_Even_pixel = IPIPE_PIXEL_COLOUR_GR;
    cfg->src_cfg->Odd_line_Odd_pixel = IPIPE_PIXEL_COLOUR_R;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_1_FLAG;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_2_FLAG;

    cfg->filter_flag |= IPIPE_GIC_FLAG;

    cfg->filter_flag |= IPIPE_WB_FLAG;

    cfg->filter_flag |= IPIPE_CFA_FLAG;

    cfg->filter_flag |= IPIPE_RGB_RGB_1_FLAG;

    cfg->filter_flag |= IPIPE_RGB_RGB_2_FLAG;

    cfg->filter_flag |= IPIPE_GAMMA_FLAG;

    cfg->filter_flag |= IPIPE_EE_FLAG;

    cfg->filter_flag |= IPIPE_RGB_TO_YUV_FLAG;
    cfg->filter_flag |= IPIPE_CAR_FLAG;
    cfg->filter_flag |= IPIPE_LSC_FLAG;

    // cfg->filter_flag|=IPIPE_GBCE_FLAG;

    cfg->filter_flag |= IPIPE_HST_FLAG;
    cfg->filter_flag |= IPIPE_BSC_FLAG;

    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Ipipe does a RAW to YUV conversion and BSC and Boxcar are enabled
 *
 *  @param   ipipe_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ipipe_params_init_4(ipipe_cfg_t * cfg)
{

    cfg->src_cfg = &default_ipipe_src_cfg;
    // cfg->dpc_lut_params=&default_dpc_lut_params;
    // cfg->dpc_otf_params=&default_dpc_otf_params;
    cfg->nf1_params = (ipipe_noise_filter_cfg_t *) & nf1_default_params;
    cfg->nf2_params = (ipipe_noise_filter_cfg_t *) & nf2_default_params;

    cfg->gic_params = &gic_default_params;
    cfg->wb_params = &wb_default_params;
    cfg->cfa_params = (ipipe_cfa_cfg_t *) & cfa_default_params;

    cfg->rgb_rgb1_params = &rgb_rgb_default_params;
    cfg->gamma_params = &gamma_default_params;
    cfg->rgb_rgb2_params = &rgb_rgb2_default_params;
    // cfg->ipipe_3d_lut_params=&default_ipipe_3d_lut_params;

    cfg->rgb_yuv_params = &rgb_yuv_default_params;

    cfg->gbce_params = &gbce_default_params;

    cfg->yuv444_yuv422_params = &yuv444_yuv_422_default_params;

    cfg->ee_param = &edge_enhance_default_params;
    cfg->car_params = &default_car_params;
    cfg->lsc_params = (ipipe_lsc_cfg_t *) & lsc_default_params;
    // cfg->cgs_params=&default_cgs_params;
    default_boxcar_params.addr = (uint32) default_boxcar_op;
    cfg->boxcar_params = &default_boxcar_params;
    // cfg->histogram_params=&default_histogram_params; 
    cfg->bsc_params = (ipipe_bsc_cfg_t *) & default_bsc_params;

    cfg->filter_flag = 0;

    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = img_width;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = img_height;

    cfg->src_cfg->wrt = 1;
    cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_YCRCB_OUTPUT;

    cfg->src_cfg->Even_line_Even_pixel = IPIPE_PIXEL_COLOUR_B;
    cfg->src_cfg->Even_line_Odd_pixel = IPIPE_PIXEL_COLOUR_GB;
    cfg->src_cfg->Odd_line_Even_pixel = IPIPE_PIXEL_COLOUR_GR;
    cfg->src_cfg->Odd_line_Odd_pixel = IPIPE_PIXEL_COLOUR_R;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_1_FLAG;

    cfg->filter_flag |= IPIPE_NOISE_FILTER_2_FLAG;

    cfg->filter_flag |= IPIPE_GIC_FLAG;

    cfg->filter_flag |= IPIPE_WB_FLAG;

    cfg->filter_flag |= IPIPE_CFA_FLAG;

    cfg->filter_flag |= IPIPE_RGB_RGB_1_FLAG;

    cfg->filter_flag |= IPIPE_RGB_RGB_2_FLAG;

    cfg->filter_flag |= IPIPE_GAMMA_FLAG;

    cfg->filter_flag |= IPIPE_EE_FLAG;

    cfg->filter_flag |= IPIPE_RGB_TO_YUV_FLAG;
    cfg->filter_flag |= IPIPE_CAR_FLAG;
    cfg->filter_flag |= IPIPE_LSC_FLAG;

    cfg->filter_flag |= IPIPE_BOXCAR_FLAG;
    cfg->filter_flag |= IPIPE_BSC_FLAG;

    // cfg->filter_flag|=IPIPE_GBCE_FLAG;

    return ISP_SUCCESS;
}

/**************************IPIPE    PARAMS END *************************************************************/
// ***************************************************************************************************//

/********************************ISIF PARAMS START  *******************************************************/

const isif_ip_src_cfg_t default_isif_src_cfg = {
    ISIF_HD_VD_DISABLE,                                    // ISIF_SYEN_START_T 
                                                           // sync_enable;
                                                           // //SYEN of
                                                           // SYNCEN
    ISIF_DATA_INPUT_MODE_CCD_RAW,                          // ISIF_DATA_INPUT_MODE_T 
                                                           // ip_pix_fmt;
                                                           // //INPMOD of
                                                           // MODESET
    ISIF_DATA_NORMAL,                                      // ISIF_DATA_POLARITY_T 
                                                           // data_polarity;
                                                           // //DPOL of
                                                           // MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // field_pol;
                                                           // //FIPOL of
                                                           // MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // hd_pol; //HDPOL 
                                                           // of MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // vd_pol; //VDPOL 
                                                           // of MODESET
    ISIF_SIGNAL_DIRECTION_OUTPUT,                          // ISIF_SIGNAL_DIR_T 
                                                           // field_dir;
                                                           // //FIDD of
                                                           // MODESET
    ISIF_SIGNAL_DIRECTION_OUTPUT,                          // ISIF_SIGNAL_DIR_T 
                                                           // hd_vd_dir;
                                                           // //HDVDD of
                                                           // MODESET
    0,                                                     // uint16
                                                           // hd_width; //HDW 
                                                           // OF HDW
    0,                                                     // uint16
                                                           // vd_width; //VDW 
                                                           // OF VDW
    0,                                                     // uint16
                                                           // ppln_hs_interval; 
                                                           // //PPLN
    0,                                                     // uint16
                                                           // lpfr_vs_interval; 
                                                           // //LPFR
    ISIF_YCINSWAP_DISABLE,                                 // ISIF_YCINSWAP_T 
                                                           // yc_in_swap;
                                                           // //CCDCFG
                                                           // YCINSWP
    ISIF_MSB_INVERSE_CIN_DISABLE,                          // ISIF_MSB_INVERSE_CIN_T 
                                                           // msb_inverse_cin; 
                                                           // //CCDCFG
                                                           // MSBINVI

    ISIF_GAIN_MSB_BIT11,                                   // ISIF_GAIN_MSB_POS_T 
                                                           // ip_data_msb_pos; 
                                                           // //CGAMMAWD GWDI
    ISIF_COLOR_POSITION_R_YE,                              // ISIF_COLOR_PATTERN_POS_T 
                                                           // field1_pixel_pos[4]; 
                                                           // //CPx_F1 CCOLP
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,

    ISIF_COLOR_POSITION_R_YE,                              // ISIF_COLOR_PATTERN_POS_T 
                                                           // field0_pixel_pos[4]; 
                                                           // //CPx_F0 CCOLP
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,

    ISIF_CFA_PATTERN_MOSAIC,                               // ISIF_CFA_PATTERN_T 
                                                           // cfa_pattern;
                                                           // //CGAMMAWD CFAP

    ISIF_REC656IF_MODE_DISABLE,                            // ISIF_REC656IF_MODE_T 
                                                           // r656_inferface_mode; 
                                                           // //REC656IF
                                                           // R656ON
    ISIF_ERR_CORRECTION_FVC_DISABLE,                       // ISIF_ERR_CORRECTION_FVC_T 
                                                           // err_correction_mode; 
                                                           // //REC656IF
                                                           // ECCFVH
    ISIF_BW656_DISABLE,                                    // ISIF_BW656_T
                                                           // ccir656_bit_width; 
                                                           // //CCDCFG BW656
};

const isif_ip_src_cfg_t default_isif_src_cfg_2 = {
    ISIF_HD_VD_DISABLE,                                    // ISIF_SYEN_START_T 
                                                           // sync_enable;
                                                           // //SYEN of
                                                           // SYNCEN
    ISIF_DATA_INPUT_MODE_CCD_RAW,                          // ISIF_DATA_INPUT_MODE_T 
                                                           // ip_pix_fmt;
                                                           // //INPMOD of
                                                           // MODESET
    ISIF_DATA_NORMAL,                                      // ISIF_DATA_POLARITY_T 
                                                           // data_polarity;
                                                           // //DPOL of
                                                           // MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // field_pol;
                                                           // //FIPOL of
                                                           // MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // hd_pol; //HDPOL 
                                                           // of MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // vd_pol; //VDPOL 
                                                           // of MODESET
    ISIF_SIGNAL_DIRECTION_OUTPUT,                          // ISIF_SIGNAL_DIR_T 
                                                           // field_dir;
                                                           // //FIDD of
                                                           // MODESET
    ISIF_SIGNAL_DIRECTION_OUTPUT,                          // ISIF_SIGNAL_DIR_T 
                                                           // hd_vd_dir;
                                                           // //HDVDD of
                                                           // MODESET
    0,                                                     // uint16
                                                           // hd_width; //HDW 
                                                           // OF HDW
    0,                                                     // uint16
                                                           // vd_width; //VDW 
                                                           // OF VDW
    639,                                                   // uint16
                                                           // ppln_hs_interval; 
                                                           // //PPLN
    479,                                                   // uint16
                                                           // lpfr_vs_interval; 
                                                           // //LPFR
    ISIF_YCINSWAP_DISABLE,                                 // ISIF_YCINSWAP_T 
                                                           // yc_in_swap;
                                                           // //CCDCFG
                                                           // YCINSWP
    ISIF_MSB_INVERSE_CIN_DISABLE,                          // ISIF_MSB_INVERSE_CIN_T 
                                                           // msb_inverse_cin; 
                                                           // //CCDCFG
                                                           // MSBINVI

    ISIF_GAIN_MSB_BIT11,                                   // ISIF_GAIN_MSB_POS_T 
                                                           // ip_data_msb_pos; 
                                                           // //CGAMMAWD GWDI
    ISIF_COLOR_POSITION_R_YE,                              // ISIF_COLOR_PATTERN_POS_T 
                                                           // field1_pixel_pos[4]; 
                                                           // //CPx_F1 CCOLP
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,

    ISIF_COLOR_POSITION_R_YE,                              // ISIF_COLOR_PATTERN_POS_T 
                                                           // field0_pixel_pos[4]; 
                                                           // //CPx_F0 CCOLP
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,
    ISIF_COLOR_POSITION_R_YE,

    ISIF_CFA_PATTERN_MOSAIC,                               // ISIF_CFA_PATTERN_T 
                                                           // cfa_pattern;
                                                           // //CGAMMAWD CFAP

    ISIF_REC656IF_MODE_DISABLE,                            // ISIF_REC656IF_MODE_T 
                                                           // r656_inferface_mode; 
                                                           // //REC656IF
                                                           // R656ON
    ISIF_ERR_CORRECTION_FVC_DISABLE,                       // ISIF_ERR_CORRECTION_FVC_T 
                                                           // err_correction_mode; 
                                                           // //REC656IF
                                                           // ECCFVH
    ISIF_BW656_DISABLE,                                    // ISIF_BW656_T
                                                           // ccir656_bit_width; 
                                                           // //CCDCFG BW656
};

const isif_csc_coef_mat_t default_csc_coef = {
    0xFF,                                                  // uint8
                                                           // csc_coef_m00;
                                                           // //CSCM00 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m01;
                                                           // //CSCM01 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m02;
                                                           // //CSCM02 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m03;
                                                           // //CSCM03 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m10;
                                                           // //CSCM10 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m11;
                                                           // //CSCM11 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m12;
                                                           // //CSCM12 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m13;
                                                           // //CSCM13 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m20;
                                                           // //CSCM20 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m21;
                                                           // //CSCM21 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m22;
                                                           // //CSCM22 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m23;
                                                           // //CSCM23 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m30;
                                                           // //CSCM30 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m31;
                                                           // //CSCM31 CSCM0
    0xFF,                                                  // uint8
                                                           // csc_coef_m32;
                                                           // //CSCM32 CSCM0
    0xFF                                                   // uint8
                                                           // csc_coef_m33;
                                                           // //CSCM33 CSCM0
};

const isif_ipfmt_pgm_addr_ptr_select_t default_pgm_addr_ptr_sel[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15
};

const isif_ipfmt_line_init_t default_line_init[] = {
    0x4, 0x111,
    0x3, 0x222,
    0x2, 0x333,
    0x1, 0x444,

    0x8, 0x555,
    0x7, 0x666,
    0x6, 0x777,
    0x5, 0x888,

    0x5, 0x888,
    0x6, 0x777,
    0x7, 0x666,
    0x8, 0x555,

    0x1, 0x444,
    0x2, 0x333,
    0x3, 0x222,
    0x4, 0x111,
};

/*****************/
const isif_ip_format_cfg_t default_ip_data_format_params = {
    ISIF_CCD_FORMAT_DISABLE,                               // ISIF_CCD_IPFMT_T 
                                                           // enable; //FMTEN 
                                                           // FMTCFG
    ISIF_INPUT_SPLIT,                                      // ISIF_IPFMT_COMBINE_SPLIT_T 
                                                           // combine_enable; 
                                                           // //FMTCBL FMTCFG
    ISIF_NORMAL_SCAN_MODE,                                 // ISIF_SCAN_MODE_T 
                                                           // line_alternate; 
                                                           // //LNALT FMTCFG
    0,                                                     // uint8
                                                           // split_combine_line; 
                                                           // //LNUM FMTCFG
    0,                                                     // uint8
                                                           // address_increment_value; 
                                                           // //FMTAINC
                                                           // FMTCFG
    // 
    0,                                                     // uint8
                                                           // pgm_entry_length_set0; 
                                                           // //FMTPLEN0
                                                           // FMTPLEN
    0,                                                     // uint8
                                                           // pgm_entry_length_set1; 
                                                           // //FMTPLEN1
                                                           // FMTPLEN
    0,                                                     // uint8
                                                           // pgm_entry_length_set2; 
                                                           // //FMTPLEN2
                                                           // FMTPLEN
    0,                                                     // uint8
                                                           // pgm_entry_length_set3; 
                                                           // //FMTPLEN3
                                                           // FMTPLEN
    // 
    0,                                                     // uint16
                                                           // h_pos;//ip_first_pixel; 
                                                           // //FMTSPH
    0,                                                     // uint16
                                                           // h_size;//ip_pixel_length; 
                                                           // //FMTLNH
    0,                                                     // uint16
                                                           // v_pos;//ip_first_line; 
                                                           // //FMTSLV
    0,                                                     // uint16
                                                           // v_size;//ip_line_length; 
                                                           // //FMTLNV
    0,                                                     // uint16
                                                           // op_pixel_length; 
                                                           // //FMTRLEN
    0,                                                     // uint16
                                                           // output_hd_interval_split; 
                                                           // //FMTHCNT
    // 
    0,                                                     // uint8
                                                           // fmt_aptrs; //
                                                           // FMTAPTR used
                                                           // from FMTAPTR 0
                                                           // to fmt_aptrs,
                                                           // max 16 
    (isif_ipfmt_line_init_t *) & default_line_init[0],     // isif_ipfmt_line_init_t* 
                                                           // line_init;
                                                           // //FMTAPTR0 -
                                                           // FMTAPTR15
    // 
    0,                                                     // uint16
                                                           // pgm_addr_enable_bit_map0; 
                                                           // //PGM15EN-PGM0EN 
                                                           // FMTPGMVF0
    0,                                                     // uint16
                                                           // pgm_addr_enable_bit_map1; 
                                                           // //PGM31EN-PGM16EN 
                                                           // FMTPGMVF1
    // 
    0,                                                     // uint16
                                                           // pgm_addr_update_bit_map0; 
                                                           // //PGM15UPDT-PGM0UPDT 
                                                           // FMTPGMAPU0
    0,                                                     // uint16
                                                           // pgm_addr_update_bit_map1; 
                                                           // //PGM31UPDT-PGM0UPDT 
                                                           // FMTPGMAPU1
    // 12, // uint8 pgm_aptrs; //no of pgm address ptrs used 0 to 32
    (isif_ipfmt_pgm_addr_ptr_select_t *) & default_pgm_addr_ptr_sel[0]  // isif_ipfmt_pgm_addr_ptr_select_t* 
                                                                        // pgm_addr_ptr_sel; 
                                                                        // //PGMXXAPTR 
                                                                        // -
                                                                        // FMTPGMAPS7 
                                                                        // FMTPGMAPS0 
                                                                        // must 
                                                                        // be 
                                                                        // array 
                                                                        // of 
                                                                        // 32
};

isif_sdram_op_cfg_t default_sdram_op_params = {
    0,                                                     // uint16
                                                           // sdram_hpos;
                                                           // //first pixel
                                                           // in line SPH
    img_width,                                             // uint16
                                                           // sdram_hsize;
                                                           // //no of pixels
                                                           // in line = LNH+1
    0,                                                     // uint16
                                                           // sdram_vpos0;
                                                           // //wrt to
                                                           // VD,output to
                                                           // SDRAM, SLV0
                                                           // field 0
    0,                                                     // uint16
                                                           // sdram_vpos1;
                                                           // //wrt to
                                                           // VD,SLV1 field 1
    img_height,                                            // uint16
                                                           // sdram_vsize;
                                                           // //no of line =
                                                           // LNV+1

    ISIF_HLINE_ADDR_INCREMENT,                             // ISIF_HLINE_ADDR_T 
                                                           // memory_addr_decrement; 
                                                           // //ADCR, HSIZE 
    0,                                                     // uint16
                                                           // memory_addr_offset; 
                                                           // //memory
                                                           // address offset
                                                           // . line, HSIZE

    ISIF_FID_NO_CHANGE,                                    // ISIF_FID_POLARITY 
                                                           // fid_polarity;
                                                           // //FIINV OF
                                                           // SDOFST
    ISIF_FIELD_LINE_OFFSET_PLUS_3,                         // ISIF_FIELD_LINE_OFFSET_VAL_T 
                                                           // odd_field_line_offset; 
                                                           // //FOFST OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_even_field_offset; 
                                                           // //LOFSTEE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_2,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_even_field_offset; 
                                                           // //LOFSTOE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_3,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_odd_field_offset; 
                                                           // //LOFSTEO OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_4,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_odd_field_offset; 
                                                           // //LOFTOO OF
                                                           // SDOFST
    NULL,                                                  // uint32
                                                           // memory_addr;
                                                           // //(CADU+CADL)<<5 
                                                           // 32byte aligned

    ISIF_LPF_ANTI_ALIAS_DISABLE,                           // ISIF_HPLF
                                                           // alias_filter_enable; 
                                                           // //HLPF of
                                                           // MODESET
    ISIF_DPCM_ENCODE_PREDICTOR2,                           // ISIF_DPCM_ENCODE_PREDICTOR_TYP_T 
                                                           // dpcm_predictor; 
                                                           // //DPCMPRE MISC
    ISIF_DPCM_ENCODER_ENABLE_ON,                           // ISIF_DPCM_ENCODER_ENABLE_TYPE 
                                                           // dpcm_enable;
    ISIF_CCD_DATA_NO_SHIFT,                                // ISIF_CCD_DATA_SHIFT_SDRAM_T 
                                                           // ccd_raw_shift_value; 
                                                           // //CCDW of
                                                           // MODESET
    ISIF_CCD_MODE_NON_INTERLACED,                          // ISIF_CCD_MODE_T 
                                                           // ccd_field_mode; 
                                                           // //CCDMD of
                                                           // MODESET
    ISIF_NO_WEN,                                           // ISIF_SWEN_MODE_T 
                                                           // wen_selection_mode; 
                                                           // //SWEN of
                                                           // MODESET
    ISIF_DATA_WRITE_ENABLE,                                // ISIF_DATA_WRITE_START_T 
                                                           // data_write_enable; 
                                                           // //DWEN of
                                                           // SYNCEN
    ISIF_MSB_INVERSE_COUT_DISABLE,                         // ISIF_MSB_INVERSE_COUT_T 
                                                           // msb_inverse_cout; 
                                                           // //CCDCFG
                                                           // MSBINVO
    ISIF_BYTE_SWAP_DISABLE,                                // ISIF_BYTE_SWAP_T 
                                                           // sdram_byte_swap; 
                                                           // //CCDCFG BSWD
    ISIF_SELECT_Y_POS_EVEN_PX,                             // ISIF_SELECT_Y_POS_T 
                                                           // y_position;
                                                           // //CCDCFG Y8POS
    ISIF_NO_EXTRG,                                         // ISIF_EXTRG_T
                                                           // sdram_ext_trig; 
                                                           // //CCDCFG EXTRG
    ISIF_SDRAM_TRGSEL_DWEN_REG,                            // ISIF_SDRAM_TRGSEL_T 
                                                           // sdram_trig_select; 
                                                           // //CCDCFG TRGSEL
    ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL,               // ISIF_WENLOG_SIGNAL_T 
                                                           // ccd_valid_log;
                                                           // //CCDCFG WENLOG
    ISIF_FID_LATCH_VSYNC,                                  // ISIF_FID_DETECTION_MODE_T 
                                                           // fid_detection_mode; 
                                                           // //CCDCFG FIDMD
    ISIF_YCOUTSWAP_DISABLE,                                // ISIF_YCOUTSWAP_T 
                                                           // yc_out_swap;
                                                           // //CCDCFG
                                                           // YCOUTSWP
    ISIF_SDRAM_PACK_8BITS,                                 // ISIF_SDRAM_PACK_T 
                                                           // sdram_pack_fmt; 
                                                           // //CCDCFG
                                                           // SDRPACK
    ISIF_VLDC_RESYNC_VSYNC_DISABLE,                        // ISIF_VLDC_RESYNC_VSYNC_T 
                                                           // vsync_enable;
                                                           // //CCDCFG VLDC
                                                           // shadow reg or
                                                           // busy update??

    0xFF,                                                  // isif_culling_cfg_t 
                                                           // culling_params; 
                                                           // //Programmable
                                                           // H/V culling
                                                           // pattern
    0xFF,
    0xFF
};

const isif_sdram_op_cfg_t default_sdram_op_params_2 = {
    0,                                                     // uint16
                                                           // sdram_hpos;
                                                           // //first pixel
                                                           // in line SPH
    0x280,                                                 // uint16
                                                           // sdram_hsize;
                                                           // //no of pixels
                                                           // in line = LNH+1
    0,                                                     // uint16
                                                           // sdram_vpos0;
                                                           // //wrt to
                                                           // VD,output to
                                                           // SDRAM, SLV0
                                                           // field 0
    0,                                                     // uint16
                                                           // sdram_vpos1;
                                                           // //wrt to
                                                           // VD,SLV1 field 1
    0x1E0,                                                 // uint16
                                                           // sdram_vsize;
                                                           // //no of line =
                                                           // LNV+1

    ISIF_HLINE_ADDR_INCREMENT,                             // ISIF_HLINE_ADDR_T 
                                                           // memory_addr_decrement; 
                                                           // //ADCR, HSIZE 
    0,                                                     // uint16
                                                           // memory_addr_offset; 
                                                           // //memory
                                                           // address offset
                                                           // . line, HSIZE

    ISIF_FID_NO_CHANGE,                                    // SIF_FID_POLARITY 
                                                           // fid_polarity;
                                                           // //FIINV OF
                                                           // SDOFST
    ISIF_FIELD_LINE_OFFSET_PLUS_3,                         // ISIF_FIELD_LINE_OFFSET_VAL_T 
                                                           // odd_field_line_offset; 
                                                           // //FOFST OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_even_field_offset; 
                                                           // //LOFSTEE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_2,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_even_field_offset; 
                                                           // //LOFSTOE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_3,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_odd_field_offset; 
                                                           // //LOFSTEO OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_4,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_odd_field_offset; 
                                                           // //LOFTOO OF
                                                           // SDOFST
    0x30000000,                                            // uint32
                                                           // memory_addr;
                                                           // //(CADU+CADL)<<5 
                                                           // 32byte aligned

    ISIF_LPF_ANTI_ALIAS_DISABLE,                           // ISIF_HPLF_CTRL_T 
                                                           // alias_filter_enable; 
                                                           // //HLPF of
                                                           // MODESET
    ISIF_DPCM_ENCODE_PREDICTOR2,                           // ISIF_DPCM_ENCODE_PREDICTOR_TYP_T 
                                                           // dpcm_predictor; 
                                                           // //DPCMPRE MISC
    ISIF_DPCM_ENCODER_ENABLE_OFF,                          // ISIF_DPCM_ENCODER_ENABLE_TYPE 
                                                           // dpcm_enable;
    ISIF_CCD_DATA_NO_SHIFT,                                // ISIF_CCD_DATA_SHIFT_SDRAM_T 
                                                           // ccd_raw_shift_value; 
                                                           // //CCDW of
                                                           // MODESET
    ISIF_CCD_MODE_NON_INTERLACED,                          // ISIF_CCD_MODE_T 
                                                           // ccd_field_mode; 
                                                           // //CCDMD of
                                                           // MODESET
    ISIF_NO_WEN,                                           // ISIF_SWEN_MODE_T 
                                                           // wen_selection_mode; 
                                                           // //SWEN of
                                                           // MODESET
    ISIF_DATA_WRITE_DISABLE,                               // ISIF_DATA_WRITE_START_T 
                                                           // data_write_enable; 
                                                           // //DWEN of
                                                           // SYNCEN
    ISIF_MSB_INVERSE_COUT_DISABLE,                         // ISIF_MSB_INVERSE_COUT_T 
                                                           // msb_inverse_cout; 
                                                           // //CCDCFG
                                                           // MSBINVO
    ISIF_BYTE_SWAP_DISABLE,                                // ISIF_BYTE_SWAP_T 
                                                           // sdram_byte_swap; 
                                                           // //CCDCFG BSWD
    ISIF_SELECT_Y_POS_EVEN_PX,                             // ISIF_SELECT_Y_POS_T 
                                                           // y_position;
                                                           // //CCDCFG Y8POS
    ISIF_NO_EXTRG,                                         // ISIF_EXTRG_T
                                                           // sdram_ext_trig; 
                                                           // //CCDCFG EXTRG
    ISIF_SDRAM_TRGSEL_DWEN_REG,                            // ISIF_SDRAM_TRGSEL_T 
                                                           // sdram_trig_select; 
                                                           // //CCDCFG TRGSEL
    ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL,               // ISIF_WENLOG_SIGNAL_T 
                                                           // ccd_valid_log;
                                                           // //CCDCFG WENLOG
    ISIF_FID_LATCH_VSYNC,                                  // ISIF_FID_DETECTION_MODE_T 
                                                           // fid_detection_mode; 
                                                           // //CCDCFG FIDMD
    ISIF_YCOUTSWAP_DISABLE,                                // ISIF_YCOUTSWAP_T 
                                                           // yc_out_swap;
                                                           // //CCDCFG
                                                           // YCOUTSWP
    ISIF_SDRAM_PACK_16BITS,                                // ISIF_SDRAM_PACK_T 
                                                           // sdram_pack_fmt; 
                                                           // //CCDCFG
                                                           // SDRPACK
    ISIF_VLDC_RESYNC_VSYNC_DISABLE,                        // ISIF_VLDC_RESYNC_VSYNC_T 
                                                           // vsync_enable;
                                                           // //CCDCFG VLDC
                                                           // shadow reg or
                                                           // busy update??

    0xFF,                                                  // isif_culling_cfg_t 
                                                           // culling_params; 
                                                           // //Programmable
                                                           // H/V culling
                                                           // pattern
    0xFF,
    0xFF
};

const isif_sdram_op_cfg_t default_sdram_op_params_test_isif_1 = {
    0,                                                     // uint16
                                                           // sdram_hpos;
                                                           // //first pixel
                                                           // in line SPH
    0x280,                                                 // uint16
                                                           // sdram_hsize;
                                                           // //no of pixels
                                                           // in line = LNH+1
    0,                                                     // uint16
                                                           // sdram_vpos0;
                                                           // //wrt to
                                                           // VD,output to
                                                           // SDRAM, SLV0
                                                           // field 0
    0,                                                     // uint16
                                                           // sdram_vpos1;
                                                           // //wrt to
                                                           // VD,SLV1 field 1
    0x1E0,                                                 // uint16
                                                           // sdram_vsize;
                                                           // //no of line =
                                                           // LNV+1

    ISIF_HLINE_ADDR_INCREMENT,                             // ISIF_HLINE_ADDR_T 
                                                           // memory_addr_decrement; 
                                                           // //ADCR, HSIZE 
    0,                                                     // uint16
                                                           // memory_addr_offset; 
                                                           // //memory
                                                           // address offset
                                                           // . line, HSIZE

    ISIF_FID_NO_CHANGE,                                    // SIF_FID_POLARITY 
                                                           // fid_polarity;
                                                           // //FIINV OF
                                                           // SDOFST
    ISIF_FIELD_LINE_OFFSET_PLUS_3,                         // ISIF_FIELD_LINE_OFFSET_VAL_T 
                                                           // odd_field_line_offset; 
                                                           // //FOFST OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_even_field_offset; 
                                                           // //LOFSTEE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_2,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_even_field_offset; 
                                                           // //LOFSTOE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_3,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_odd_field_offset; 
                                                           // //LOFSTEO OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_4,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_odd_field_offset; 
                                                           // //LOFTOO OF
                                                           // SDOFST
    0x80000000,                                            // uint32
                                                           // memory_addr;
                                                           // //(CADU+CADL)<<5 
                                                           // 32byte aligned

    ISIF_LPF_ANTI_ALIAS_DISABLE,                           // ISIF_HPLF_CTRL_T 
                                                           // alias_filter_enable; 
                                                           // //HLPF of
                                                           // MODESET
    ISIF_DPCM_ENCODE_PREDICTOR2,                           // ISIF_DPCM_ENCODE_PREDICTOR_TYP_T 
                                                           // dpcm_predictor; 
                                                           // //DPCMPRE MISC
    ISIF_DPCM_ENCODER_ENABLE_OFF,                          // ISIF_DPCM_ENCODER_ENABLE_TYPE 
                                                           // dpcm_enable;
    ISIF_CCD_DATA_NO_SHIFT,                                // ISIF_CCD_DATA_SHIFT_SDRAM_T 
                                                           // ccd_raw_shift_value; 
                                                           // //CCDW of
                                                           // MODESET
    ISIF_CCD_MODE_NON_INTERLACED,                          // ISIF_CCD_MODE_T 
                                                           // ccd_field_mode; 
                                                           // //CCDMD of
                                                           // MODESET
    ISIF_NO_WEN,                                           // ISIF_SWEN_MODE_T 
                                                           // wen_selection_mode; 
                                                           // //SWEN of
                                                           // MODESET
    ISIF_DATA_WRITE_DISABLE,                               // ISIF_DATA_WRITE_START_T 
                                                           // data_write_enable; 
                                                           // //DWEN of
                                                           // SYNCEN
    ISIF_MSB_INVERSE_COUT_DISABLE,                         // ISIF_MSB_INVERSE_COUT_T 
                                                           // msb_inverse_cout; 
                                                           // //CCDCFG
                                                           // MSBINVO
    ISIF_BYTE_SWAP_DISABLE,                                // ISIF_BYTE_SWAP_T 
                                                           // sdram_byte_swap; 
                                                           // //CCDCFG BSWD
    ISIF_SELECT_Y_POS_EVEN_PX,                             // ISIF_SELECT_Y_POS_T 
                                                           // y_position;
                                                           // //CCDCFG Y8POS
    ISIF_NO_EXTRG,                                         // ISIF_EXTRG_T
                                                           // sdram_ext_trig; 
                                                           // //CCDCFG EXTRG
    ISIF_SDRAM_TRGSEL_DWEN_REG,                            // ISIF_SDRAM_TRGSEL_T 
                                                           // sdram_trig_select; 
                                                           // //CCDCFG TRGSEL
    ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL,               // ISIF_WENLOG_SIGNAL_T 
                                                           // ccd_valid_log;
                                                           // //CCDCFG WENLOG
    ISIF_FID_LATCH_VSYNC,                                  // ISIF_FID_DETECTION_MODE_T 
                                                           // fid_detection_mode; 
                                                           // //CCDCFG FIDMD
    ISIF_YCOUTSWAP_DISABLE,                                // ISIF_YCOUTSWAP_T 
                                                           // yc_out_swap;
                                                           // //CCDCFG
                                                           // YCOUTSWP
    ISIF_SDRAM_PACK_8BITS,                                 // ISIF_SDRAM_PACK_T 
                                                           // sdram_pack_fmt; 
                                                           // //CCDCFG
                                                           // SDRPACK
    ISIF_VLDC_RESYNC_VSYNC_DISABLE,                        // ISIF_VLDC_RESYNC_VSYNC_T 
                                                           // vsync_enable;
                                                           // //CCDCFG VLDC
                                                           // shadow reg or
                                                           // busy update??

    0xFF,                                                  // isif_culling_cfg_t 
                                                           // culling_params; 
                                                           // //Programmable
                                                           // H/V culling
                                                           // pattern
    0xFF,
    0xFF
};

const isif_flash_cfg_t default_flash_timing_params = {
    0,                                                     // uint8 enable;
                                                           // // FLSHEN
                                                           // FLSHCFG0
    3,                                                     // uint16
                                                           // flash_timing_start_line; 
                                                           // // SFLSH
                                                           // FLSHCFG1
    5                                                      // uint16
                                                           // flash_timing_width; 
                                                           // // VFLSH
                                                           // FLSHCFG2
};

const isif_sensor_linear_cfg_t default_linearization_params = {
    0,                                                     // uint8 enable;
                                                           // //LINEN LINCFG0
    ISIF_LINEAR_5_LEFT_SHIFT_VALUE,                        // ISIF_LINEAR_SHIFT_VAL_T 
                                                           // correction_shift; 
                                                           // //CORRSFT
                                                           // LINCFG0
    ISIF_LINEAR_NON_UNIFORM_SAMPLING,                      // ISIF_LINEAR_SAMPLING_T 
                                                           // non_uniform_mode; 
                                                           // //LINMD LINCFG0
    0xff,                                                  // uint16
                                                           // scale_value;
                                                           // //LUTSCL
                                                           // LINCFG1 u11q10
};

isif_gain_offset_cfg_t default_gain_offset_params = {
    0,                                                     // uint8
                                                           // gain_offset_featureflag;
    0,                                                     // uint16 gain_r;
                                                           // //CGR CRGAIN
    0,                                                     // uint16 gain_gr; 
                                                           // //CGGR CGRGAIN
    0,                                                     // uint16 gain_gb; 
                                                           // //CGGB CGBGAIN
    0,                                                     // uint16 gain_bg; 
                                                           // //CGB CBGAIN
    0                                                      // uint16 offset;
                                                           // //COFT COFSTA
};

const isif_vldc_defect_line_params_t default_vldc[] = {
    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position; 
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position; 
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc; 
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc; 
                                                           // //DFCMEM3
    0                                                      // uint8
                                                           // sub3_greater_than_vldc; 
                                                           // //DFCMEM4
};

isif_vldc_cfg_t default_vldc_params = {
    0,                                                     // uint8 enable;
                                                           // //VDFCEN DFCCTL
    ISIF_VDLC_DISABLE_ABOVE_UPPER_PIXEL,                   // ISIF_VDLC_PIXEL_DEPENDENCY 
                                                           // disable_vldc_upper_pixels; 
                                                           // //VDFCUDA
                                                           // DFCCTL
    3,                                                     // uint8
                                                           // vldc_shift_value; 
                                                           // //VDFLSFT
                                                           // DFCCTL
    ISIF_VLDC_HORIZONTAL_INTERPOLATION,                    // ISIF_VLDC_MODE_SEL_T 
                                                           // vldc_mode_select; 
                                                           // //VDFCSL DFCCTL
    0x1ff,                                                 // uint16
                                                           // vldc_saturation_level; 
                                                           // //VDFSLV
                                                           // VDFSATLVuint12
                                                           // range 0 - 4095
    1,                                                     // uint8
                                                           // defect_lines;
                                                           // // no of
                                                           // vdefect lines
                                                           // max 8
    (isif_vldc_defect_line_params_t *) & default_vldc[0]   // isif_vldc_defect_line_params_t* 
                                                           // vldc_defect_line_params; 
                                                           // //DFCMEM0 -8
};

const isif_2dlsc_cfg_t default_lsc_params = {
    0,                                                     // uint8 enable;
                                                           // //DLSCCFG
                                                           // ENABLE
    0,                                                     // uint16
                                                           // lsc_hsize;
                                                           // //HVAL LSCHVAL
    0,                                                     // uint16
                                                           // lsc_vsize;
                                                           // //VVAL LSCVVAL

    0,                                                     // uint16
                                                           // hdirection_data_offset; 
                                                           // //HOFST
                                                           // LSCHOFST
                                                           // DATAHOFST
                                                           // 0-16383
    0,                                                     // uint16
                                                           // vdirection_data_offset; 
                                                           // //VOFST
                                                           // LSCVOFST
                                                           // DATAVOFST

    0,                                                     // uint8
                                                           // hposin_paxel;
                                                           // //X DLSCINI 6:0 
                                                           // 
    0,                                                     // uint8
                                                           // vposin_paxel;
                                                           // //Y DLSCINI 6:0 
                                                           // ypos_distance_from_first_paxel
    // ISIF_LSC
    ISIF_LSC_8_PIXEL,                                      // ISIF_LSC_GAIN_MODE_MN_T 
                                                           // pax_height;
                                                           // //GAIN_MODE_M
                                                           // DLSCCFG
    ISIF_LSC_8_PIXEL,                                      // ISIF_LSC_GAIN_MODE_MN_T 
                                                           // pax_length;
                                                           // //GAIN_MODE_N
                                                           // DLSCCFG
    // busy_field
    ISIF_LSC_GAIN_0Q8,                                     // ISIF_LSC_GAIN_FORMAT 
                                                           // gain_format;
                                                           // //GAIN_FORMAT
                                                           // DLSCCFG

    0,                                                     // uint8
                                                           // offset_scaling_factor; 
                                                           // //OFSTSF
                                                           // DLSCOFST
    ISIF_LSC_OFFSET_NO_SHIFT,                              // ISIF_LSC_OFFSET_SHIFT_T 
                                                           // offset_shift_value; 
                                                           // //OFSTSFT
                                                           // DLSCOFST
    ISIF_LSC_OFFSET_OFF,                                   // ISIF_LSC_OFFSET_CTRL_T 
                                                           // offset_enable;
                                                           // //OFSTEN
                                                           // DLSCOFST

    0xFFFFFF00,                                            // uint32
                                                           // gain_table_address; 
                                                           // //BASE31_16
                                                           // DLSCGRBU+
                                                           // BASE15_0
                                                           // DLSCGRBL 32bit
                                                           // aligned
    0xff,                                                  // uint16
                                                           // gain_table_length; 
                                                           // //OFFSET
                                                           // DLSCGROF

    0xFFFFFF00,                                            // uint32
                                                           // offset_table_address; 
                                                           // //BASE31_16
                                                           // DLSCORBU +
                                                           // BASE31_16
                                                           // DLSCORBU 32bit
                                                           // aligned
    0xff                                                   // uint16
                                                           // offset_table_length; 
                                                           // //OFFSET
                                                           // DLSCOROF
        // interupt enable
        // interupt config
};

const isif_clamp_cfg_t default_digital_clamp_params = {
    0,                                                     // uint8 enable;
                                                           // //CLEN CLAMPCFG
    ISIF_HORIZONTAL_CLAMP_ENABLED,                         // ISIF_HORIZONTAL_CLAMP_MODE_T 
                                                           // hclamp_mode;
                                                           // //CLMD CLAMPCFG
    ISIF_FOUR_COLOR_CLAMP,                                 // ISIF_BLACK_CLAMP_MODE_T 
                                                           // black_clamp_mode; 
                                                           // //CLHMD
                                                           // CLAMPCFG 
    0xfff,                                                 // uint16
                                                           // dcoffset_clamp_value; 
                                                           // //CLDCOFST CLDC 
                                                           // s13
    0xA,                                                   // uint16
                                                           // black_clamp_v_start_pos; 
                                                           // //CLSV 12-0
                                                           // range 0-8191 ?? 
                                                           // 2 vstart pos
    // {
    0xF,                                                   // uint16 v_pos;
                                                           // //CLHSV CLHWIN2 
                                                           // 12-0
    ISIF_HBLACK_16PIXEL_TALL,                              // ISIF_HBLACK_PIXEL_HEIGHT 
                                                           // v_size; //CLHWN 
                                                           // CLHWIN0
    0xD,                                                   // uint16 h_pos;
                                                           // //CLHSH CLHWIN1 
                                                           // 12-0
    ISIF_HBLACK_256PIXEL_WIDE,                             // ISIF_HBLACK_PIXEL_WIDTH 
                                                           // h_size; //CLHWM 
                                                           // CLHWIN0
    ISIF_VBLACK_PIXEL_LIMITED,                             // ISIF_HBLACK_PIXEL_VALUE_LIM 
                                                           // pixel_value_limit; 
                                                           // //CLHLMT
                                                           // CLHWIN0
    ISIF_VBLACK_BASE_WINDOW_RIGHT,                         // ISIF_HBLACK_BASE_WINDOW_T 
                                                           // right_window;
                                                           // //CLHWBS
                                                           // CLHWIN0
    1,                                                     // uint8
                                                           // window_count_per_color; 
                                                           // //CLHWC CLHWIN0
    // } isif_h_black_params_t horizontal_black; //CLHWIN0
    // {
    0xA,                                                   // uint16 v_pos;
                                                           // //CLVSV CLVWIN2 
                                                           // 12-0
    0xB,                                                   // uint16 v_size;
                                                           // //CLVOBV
                                                           // CLVWIN3 12-0
                                                           // range 0-8192
    0xC,                                                   // uint16 h_pos;
                                                           // //CLVSH CLVWIN1 
                                                           // 12-0
    ISIF_VBLACK_64PIXEL_WIDE,                              // ISIF_VBLACK_PXL_WIDTH_T 
                                                           // h_size;
                                                           // //CLVOBH
                                                           // CLVWIN0
    0xE,                                                   // uint8
                                                           // line_avg_coef;
                                                           // //CLVCOEF
                                                           // CLVWIN0
    ISIF_VALUE_NOUPDATE,                                   // ISIF_VBLACK_RESET_MODE_T 
                                                           // reset_mode;
                                                           // //CLVRVSL
                                                           // CLVWIN0
    0xEA,                                                  // uint16
                                                           // reset_value;
                                                           // //CLVRV reset
                                                           // value U12

    // }isif_vertical_black_params_t vertical_black; //CLVWIN0
};

/* ================================================================ */
/* 
 *  Description:- Configures ISIF to read from ipipeif and put data in RAM at the address "img_buff"
 *  contained in the structure field "sdram_op_params", the flags in "feature_flag" enable different features in isif.
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isif_params_init_1(isif_cfg_t * cfg)
{

    cfg->feature_flag = 0;
    // cfg->feature_flag|=ISIF_CSC_FLAG ;
    // cfg->csc_params.enable=1;
    // cfg->csc_params.csc_coef=&default_csc_coef;

    // cfg->feature_flag|=ISIF_LSC_FLAG ;
    cfg->lsc_params = (isif_2dlsc_cfg_t *) & default_lsc_params;

    // cfg->feature_flag|=ISIF_BLACK_CLAMP_FLAG ;
    cfg->digital_clamp_params =
        (isif_clamp_cfg_t *) & default_digital_clamp_params;

    // cfg->feature_flag|=ISIF_FLASH_TIMING_FLAG ;
    cfg->flash_timing_params =
        (isif_flash_cfg_t *) & default_flash_timing_params;

    // cfg->feature_flag|=ISIF_GAIN_OFFSET_FLAG ;
    cfg->gain_offset_params = &default_gain_offset_params;

    // cfg->feature_flag|=ISIF_IP_DATA_FORMATTER_FLAG;
    cfg->ip_data_format_params =
        (isif_ip_format_cfg_t *) & default_ip_data_format_params;

    cfg->feature_flag |= ISIF_SDRAM_OP_FLAG;

    default_sdram_op_params.memory_addr = (uint32) img_buff;
    cfg->sdram_op_params = &default_sdram_op_params;

    // cfg->feature_flag|=ISIF_HLPF_FLAG ;
    // cfg->feature_flag|=ISIF_ALAW_FLAG ;
    cfg->feature_flag |= ISIF_DPCM_FLAG;
    // cfg->feature_flag|=ISIF_HVCULLING_FLAG 
    // cfg->feature_flag|=ISIF_LINEN_FLAG ;
    cfg->linearization_params =
        (isif_sensor_linear_cfg_t *) & default_linearization_params;

    // cfg->feature_flag|=ISIF_VLDC_FLAG ;
    cfg->vldc_params = &default_vldc_params;

    // cfg->feature_flag|=ISIF_VLDC_FLAG ;
    cfg->src_cfg = (isif_ip_src_cfg_t *) & default_isif_src_cfg;

    return ISP_SUCCESS;
    // cfg->feature_flag|=ISIF_VLDC_FL

}

ISP_RETURN isif_params_init_1_1(isif_cfg_t * cfg)
{

    cfg->feature_flag = 0;
    // cfg->feature_flag|=ISIF_CSC_FLAG ;
    // cfg->csc_params.enable=1;
    // cfg->csc_params.csc_coef=&default_csc_coef;

    // cfg->feature_flag|=ISIF_LSC_FLAG ;
    cfg->lsc_params = (isif_2dlsc_cfg_t *) & default_lsc_params;

    // cfg->feature_flag|=ISIF_BLACK_CLAMP_FLAG ;
    cfg->digital_clamp_params =
        (isif_clamp_cfg_t *) & default_digital_clamp_params;

    // cfg->feature_flag|=ISIF_FLASH_TIMING_FLAG ;
    cfg->flash_timing_params =
        (isif_flash_cfg_t *) & default_flash_timing_params;

    // cfg->feature_flag|=ISIF_GAIN_OFFSET_FLAG ;
    cfg->gain_offset_params = &default_gain_offset_params;

    // cfg->feature_flag|=ISIF_IP_DATA_FORMATTER_FLAG;
    cfg->ip_data_format_params =
        (isif_ip_format_cfg_t *) & default_ip_data_format_params;

    cfg->feature_flag |= ISIF_SDRAM_OP_FLAG;

    default_sdram_op_params.memory_addr = (uint32) op_img_buff;
    cfg->sdram_op_params = &default_sdram_op_params;

    // cfg->feature_flag|=ISIF_HLPF_FLAG ;
    // cfg->feature_flag|=ISIF_ALAW_FLAG ;
    // cfg->feature_flag|=ISIF_DPCM_FLAG ;
    // cfg->feature_flag|=ISIF_HVCULLING_FLAG 
    // cfg->feature_flag|=ISIF_LINEN_FLAG ;
    cfg->linearization_params =
        (isif_sensor_linear_cfg_t *) & default_linearization_params;

    // cfg->feature_flag|=ISIF_VLDC_FLAG ;
    cfg->vldc_params = &default_vldc_params;

    // cfg->feature_flag|=ISIF_VLDC_FLAG ;
    cfg->src_cfg = (isif_ip_src_cfg_t *) & default_isif_src_cfg;

    return ISP_SUCCESS;
    // cfg->feature_flag|=ISIF_VLDC_FL

}

/* ================================================================ */
/* 
 *  Description:- Configures ISIF to read from ipipeif and put data in RAM at the address "img_buff"
 *  contained in the structure field "sdram_op_params", the flags in "feature_flag" enable different features in isif.
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isif_params_init_2(isif_cfg_t * cfg)
{

    cfg->feature_flag = 0;
    // cfg->feature_flag|=ISIF_CSC_FLAG ;
    // cfg->csc_params.enable=1;
    // cfg->csc_params.csc_coef=&default_csc_coef;

    // cfg->feature_flag|=ISIF_LSC_FLAG ;
    cfg->lsc_params = (isif_2dlsc_cfg_t *) & default_lsc_params;

    // cfg->feature_flag|=ISIF_BLACK_CLAMP_FLAG ;
    cfg->digital_clamp_params =
        (isif_clamp_cfg_t *) & default_digital_clamp_params;

    // cfg->feature_flag|=ISIF_FLASH_TIMING_FLAG ;
    cfg->flash_timing_params =
        (isif_flash_cfg_t *) & default_flash_timing_params;

    // cfg->feature_flag|=ISIF_GAIN_OFFSET_FLAG ;
    cfg->gain_offset_params = &default_gain_offset_params;

    // cfg->feature_flag|=ISIF_IP_DATA_FORMATTER_FLAG;
    cfg->ip_data_format_params =
        (isif_ip_format_cfg_t *) & default_ip_data_format_params;

    // cfg->feature_flag|=ISIF_SDRAM_OP_FLAG ;
    // cfg->feature_flag|=ISIF_HLPF_FLAG ;
    // cfg->feature_flag|=ISIF_ALAW_FLAG ;
    // cfg->feature_flag|=ISIF_DPCM_FLAG ;
    // cfg->feature_flag|=ISIF_HVCULLING_FLAG ;
    cfg->sdram_op_params = (isif_sdram_op_cfg_t *) & default_sdram_op_params_2;

    // cfg->feature_flag|=ISIF_LINEN_FLAG ;
    cfg->linearization_params =
        (isif_sensor_linear_cfg_t *) & default_linearization_params;

    // cfg->feature_flag|=ISIF_VLDC_FLAG ;
    cfg->vldc_params = &default_vldc_params;

    cfg->feature_flag |= ISIF_INPUT_FLAG;
    cfg->src_cfg = (isif_ip_src_cfg_t *) & default_isif_src_cfg_2;

    return ISP_SUCCESS;

}

/***********************************ISIF PARAMS END***********************************************************/
/**********************************************************************************************************/

/***********************************DEFAULT  H3A  PARAMS START***********************************************************/
/**********************************************************************************************************/

h3a_af_iir_param default_h3a_iir1_params = {
    {                                                      /* fir co eff */
     0x035b, 0x01ff,
     0x0de8, 0x071d,
     0x0f8a, 0x0e2d,
     0x0bd0, 0x0505,

     0x0bb8, 0x0a5c,

     0x0099}
    , 0x0000
};

h3a_af_iir_param default_h3a_iir2_params = {
    {

     0x0bb1, 0x018a,
     0x0e13, 0x0c72,
     0x059b, 0x0bb0,
     0x0dde, 0x0c3c,
     0x0fc0, 0x05d6,
     0x0e76}
    , 0x0000
};

h3a_af_fir_param default_h3a_fir1_params = { {0x01, 0x01, 0x01, 0x01, 0x01}
, 0x01 };

h3a_af_fir_param default_h3a_fir2_params = { {0x01, 0x02, 0x01, 0x01, 0x01}
, 0x01 };

h3a_params_init_1(h3a_cfg_t * cfg)
{

    /* af */

    default_h3a_af_paxel_params.h_count = 1;
    default_h3a_af_paxel_params.h_incr = 0;
    default_h3a_af_paxel_params.h_pos = 2;
    default_h3a_af_paxel_params.h_size = 0x47;

    default_h3a_af_paxel_params.v_count = 0;
    default_h3a_af_paxel_params.v_incr = 0;
    default_h3a_af_paxel_params.v_pos = 1;
    default_h3a_af_paxel_params.v_size = 0x03;

    default_h3a_af_params.af_enable = H3A_FEATURE_ENABLE;
    default_h3a_af_params.af_op_addr = (uint32) & op_h3a_af_buff;
    default_h3a_af_params.af_paxel_win = &default_h3a_af_paxel_params;
    default_h3a_af_params.fir1 = &default_h3a_fir1_params;
    default_h3a_af_params.fir2 = &default_h3a_fir2_params;
    default_h3a_af_params.iir1 = &default_h3a_iir1_params;
    default_h3a_af_params.iir2 = &default_h3a_iir2_params;
    default_h3a_af_params.iir_start_pos = 0;
    default_h3a_af_params.peak_mode_en = H3A_FEATURE_ENABLE;
    default_h3a_af_params.rgb_pos = H3A_AF_RGBPOS_RG_BG_BAYER;
    default_h3a_af_params.vertical_focus_en = H3A_FEATURE_DISABLE;

    /* aewb setup */

    default_h3a_aewb_paxel_params.h_count = 1;
    default_h3a_aewb_paxel_params.h_incr = 0;
    default_h3a_aewb_paxel_params.h_pos = 0;
    default_h3a_aewb_paxel_params.h_size = 71;
    default_h3a_aewb_paxel_params.v_count = 0;
    default_h3a_aewb_paxel_params.v_incr = 0;
    default_h3a_aewb_paxel_params.v_pos = 1;
    default_h3a_aewb_paxel_params.v_size = 0x03;

    default_h3a_aewb_blk_win_params.hpos = 0;
    default_h3a_aewb_blk_win_params.vpos = 0;

    default_h3a_aewb_params.aewb_enable = H3A_FEATURE_ENABLE;
    default_h3a_aewb_params.aewb_op_addr = (uint32) & op_h3a_aewb_buff;
    default_h3a_aewb_params.aewb_paxel_win = &default_h3a_aewb_paxel_params;
    default_h3a_aewb_params.ae_op_fmt = H3A_AEWB_OP_FMT_MINMAX;
    default_h3a_aewb_params.blk_win_dims = &default_h3a_aewb_blk_win_params;
    default_h3a_aewb_params.saturation_limit = 0;
    default_h3a_aewb_params.shift_value = 0;

    /* common cfg */

    default_h3a_common_params.aewb_alaw_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.aewb_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.af_alaw_en = H3A_FEATURE_DISABLE;;
    default_h3a_common_params.af_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.median_filter_threshold = 0;

    default_h3a_ip_dims.hpos = 0;
    default_h3a_ip_dims.vpos = 0;

    cfg->aewb_cfg = &default_h3a_aewb_params;
    cfg->af_cfg = &default_h3a_af_params;

    cfg->h3a_common_cfg = &default_h3a_common_params;
    cfg->h3a_ip_dims = &default_h3a_ip_dims;

    return ISP_SUCCESS;

}

/************************* In this H3a params Af has been disabled. other params remain same as in h3a_params_init_1  *********/

h3a_params_init_2(h3a_cfg_t * cfg)
{

    /* af */

    default_h3a_af_paxel_params.h_count = 1;
    default_h3a_af_paxel_params.h_incr = 0;
    default_h3a_af_paxel_params.h_pos = 2;
    default_h3a_af_paxel_params.h_size = 0x47;

    default_h3a_af_paxel_params.v_count = 0;
    default_h3a_af_paxel_params.v_incr = 0;
    default_h3a_af_paxel_params.v_pos = 1;
    default_h3a_af_paxel_params.v_size = 0x03;

    default_h3a_af_params.af_enable = H3A_FEATURE_DISABLE;
    default_h3a_af_params.af_op_addr = (uint32) & op_h3a_af_buff;
    default_h3a_af_params.af_paxel_win = &default_h3a_af_paxel_params;
    default_h3a_af_params.fir1 = &default_h3a_fir1_params;
    default_h3a_af_params.fir2 = &default_h3a_fir2_params;
    default_h3a_af_params.iir1 = &default_h3a_iir1_params;
    default_h3a_af_params.iir2 = &default_h3a_iir2_params;
    default_h3a_af_params.iir_start_pos = 0;
    default_h3a_af_params.peak_mode_en = H3A_FEATURE_ENABLE;
    default_h3a_af_params.rgb_pos = H3A_AF_RGBPOS_RG_BG_BAYER;
    default_h3a_af_params.vertical_focus_en = H3A_FEATURE_DISABLE;

    /* aewb setup */

    default_h3a_aewb_paxel_params.h_count = 1;
    default_h3a_aewb_paxel_params.h_incr = 0;
    default_h3a_aewb_paxel_params.h_pos = 0;
    default_h3a_aewb_paxel_params.h_size = 71;
    default_h3a_aewb_paxel_params.v_count = 0;
    default_h3a_aewb_paxel_params.v_incr = 0;
    default_h3a_aewb_paxel_params.v_pos = 1;
    default_h3a_aewb_paxel_params.v_size = 0x03;

    default_h3a_aewb_blk_win_params.hpos = 0;
    default_h3a_aewb_blk_win_params.vpos = 0;

    default_h3a_aewb_params.aewb_enable = H3A_FEATURE_ENABLE;
    default_h3a_aewb_params.aewb_op_addr = (uint32) & op_h3a_aewb_buff;
    default_h3a_aewb_params.aewb_paxel_win = &default_h3a_aewb_paxel_params;
    default_h3a_aewb_params.ae_op_fmt = H3A_AEWB_OP_FMT_MINMAX;
    default_h3a_aewb_params.blk_win_dims = &default_h3a_aewb_blk_win_params;
    default_h3a_aewb_params.saturation_limit = 0;
    default_h3a_aewb_params.shift_value = 0;

    /* common cfg */

    default_h3a_common_params.aewb_alaw_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.aewb_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.af_alaw_en = H3A_FEATURE_DISABLE;;
    default_h3a_common_params.af_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.median_filter_threshold = 0;

    default_h3a_ip_dims.hpos = 0;
    default_h3a_ip_dims.vpos = 0;

    cfg->aewb_cfg = &default_h3a_aewb_params;
    cfg->af_cfg = &default_h3a_af_params;

    cfg->h3a_common_cfg = &default_h3a_common_params;
    cfg->h3a_ip_dims = &default_h3a_ip_dims;

    return ISP_SUCCESS;

}

/************************* In this H3a params AWB  has been disabled. other params remain same as in h3a_params_init_1  *********/

h3a_params_init_3(h3a_cfg_t * cfg)
{

    /* af */

    default_h3a_af_paxel_params.h_count = 1;
    default_h3a_af_paxel_params.h_incr = 0;
    default_h3a_af_paxel_params.h_pos = 2;
    default_h3a_af_paxel_params.h_size = 0x47;

    default_h3a_af_paxel_params.v_count = 0;
    default_h3a_af_paxel_params.v_incr = 0;
    default_h3a_af_paxel_params.v_pos = 1;
    default_h3a_af_paxel_params.v_size = 0x03;

    default_h3a_af_params.af_enable = H3A_FEATURE_ENABLE;
    default_h3a_af_params.af_op_addr = (uint32) & op_h3a_af_buff;
    default_h3a_af_params.af_paxel_win = &default_h3a_af_paxel_params;
    default_h3a_af_params.fir1 = &default_h3a_fir1_params;
    default_h3a_af_params.fir2 = &default_h3a_fir2_params;
    default_h3a_af_params.iir1 = &default_h3a_iir1_params;
    default_h3a_af_params.iir2 = &default_h3a_iir2_params;
    default_h3a_af_params.iir_start_pos = 0;
    default_h3a_af_params.peak_mode_en = H3A_FEATURE_ENABLE;
    default_h3a_af_params.rgb_pos = H3A_AF_RGBPOS_RG_BG_BAYER;
    default_h3a_af_params.vertical_focus_en = H3A_FEATURE_DISABLE;

    /* aewb setup */

    default_h3a_aewb_paxel_params.h_count = 1;
    default_h3a_aewb_paxel_params.h_incr = 0;
    default_h3a_aewb_paxel_params.h_pos = 0;
    default_h3a_aewb_paxel_params.h_size = 71;
    default_h3a_aewb_paxel_params.v_count = 0;
    default_h3a_aewb_paxel_params.v_incr = 0;
    default_h3a_aewb_paxel_params.v_pos = 1;
    default_h3a_aewb_paxel_params.v_size = 0x03;

    default_h3a_aewb_blk_win_params.hpos = 0;
    default_h3a_aewb_blk_win_params.vpos = 0;

    default_h3a_aewb_params.aewb_enable = H3A_FEATURE_DISABLE;
    default_h3a_aewb_params.aewb_op_addr = (uint32) & op_h3a_aewb_buff;
    default_h3a_aewb_params.aewb_paxel_win = &default_h3a_aewb_paxel_params;
    default_h3a_aewb_params.ae_op_fmt = H3A_AEWB_OP_FMT_MINMAX;
    default_h3a_aewb_params.blk_win_dims = &default_h3a_aewb_blk_win_params;
    default_h3a_aewb_params.saturation_limit = 0;
    default_h3a_aewb_params.shift_value = 0;

    /* common cfg */

    default_h3a_common_params.aewb_alaw_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.aewb_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.af_alaw_en = H3A_FEATURE_DISABLE;;
    default_h3a_common_params.af_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.median_filter_threshold = 0;

    default_h3a_ip_dims.hpos = 0;
    default_h3a_ip_dims.vpos = 0;

    cfg->aewb_cfg = &default_h3a_aewb_params;
    cfg->af_cfg = &default_h3a_af_params;

    cfg->h3a_common_cfg = &default_h3a_common_params;
    cfg->h3a_ip_dims = &default_h3a_ip_dims;

    return ISP_SUCCESS;

}

/************************* In this H3a params Af  & AWB has been ENabled. AF VERTICAL FOCUS HAS ALSO BEEN ENABLEDother params remain same as in h3a_params_init_1  *********/

h3a_params_init_4(h3a_cfg_t * cfg)
{

    /* af */

    default_h3a_af_paxel_params.h_count = 1;
    default_h3a_af_paxel_params.h_incr = 0;
    default_h3a_af_paxel_params.h_pos = 2;
    default_h3a_af_paxel_params.h_size = 0x47;

    default_h3a_af_paxel_params.v_count = 0;
    default_h3a_af_paxel_params.v_incr = 0;
    default_h3a_af_paxel_params.v_pos = 8;
    default_h3a_af_paxel_params.v_size = 0x03;

    default_h3a_af_params.af_enable = H3A_FEATURE_ENABLE;
    default_h3a_af_params.af_op_addr = (uint32) & op_h3a_af_buff;
    default_h3a_af_params.af_paxel_win = &default_h3a_af_paxel_params;
    default_h3a_af_params.fir1 = &default_h3a_fir1_params;
    default_h3a_af_params.fir2 = &default_h3a_fir2_params;
    default_h3a_af_params.iir1 = &default_h3a_iir1_params;
    default_h3a_af_params.iir2 = &default_h3a_iir2_params;
    default_h3a_af_params.iir_start_pos = 2;
    default_h3a_af_params.peak_mode_en = H3A_FEATURE_ENABLE;
    default_h3a_af_params.rgb_pos = H3A_AF_RGBPOS_RG_BG_BAYER;
    default_h3a_af_params.vertical_focus_en = H3A_FEATURE_ENABLE;

    /* aewb setup */

    default_h3a_aewb_paxel_params.h_count = 1;
    default_h3a_aewb_paxel_params.h_incr = 0;
    default_h3a_aewb_paxel_params.h_pos = 0;
    default_h3a_aewb_paxel_params.h_size = 71;
    default_h3a_aewb_paxel_params.v_count = 0;
    default_h3a_aewb_paxel_params.v_incr = 0;
    default_h3a_aewb_paxel_params.v_pos = 1;
    default_h3a_aewb_paxel_params.v_size = 0x03;

    default_h3a_aewb_blk_win_params.hpos = 0;
    default_h3a_aewb_blk_win_params.vpos = 0;

    default_h3a_aewb_params.aewb_enable = H3A_FEATURE_ENABLE;
    default_h3a_aewb_params.aewb_op_addr = (uint32) & op_h3a_aewb_buff;
    default_h3a_aewb_params.aewb_paxel_win = &default_h3a_aewb_paxel_params;
    default_h3a_aewb_params.ae_op_fmt = H3A_AEWB_OP_FMT_MINMAX;
    default_h3a_aewb_params.blk_win_dims = &default_h3a_aewb_blk_win_params;
    default_h3a_aewb_params.saturation_limit = 0;
    default_h3a_aewb_params.shift_value = 0;

    /* common cfg */

    default_h3a_common_params.aewb_alaw_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.aewb_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.af_alaw_en = H3A_FEATURE_DISABLE;;
    default_h3a_common_params.af_median_en = H3A_FEATURE_DISABLE;
    default_h3a_common_params.median_filter_threshold = 0;

    default_h3a_ip_dims.hpos = 0;
    default_h3a_ip_dims.vpos = 0;

    cfg->aewb_cfg = &default_h3a_aewb_params;
    cfg->af_cfg = &default_h3a_af_params;

    cfg->h3a_common_cfg = &default_h3a_common_params;
    cfg->h3a_ip_dims = &default_h3a_ip_dims;

    return ISP_SUCCESS;

}

/***********************************DEFAULT  H3A  PARAMS END***********************************************************/
/**********************************************************************************************************/

/***********************************RESIZER  PARAMS START***********************************************************/
/**********************************************************************************************************/

rsz_resize_ratio_t default_rszA_resize_ratio = {

    256,                                                   // uint32
                                                           // RZB_V_DIF;/*The 
                                                           // parameter for
                                                           // vertical
                                                           // resize. */
    256                                                    // uint32
                                                           // RZB_H_DIF;/*The 
                                                           // parameter for
                                                           // horizontal
                                                           // resizing
                                                           // process. */
};

rsz_input_crop_dims_t default_rszA_ip_params = {
    0,
    0                                                      // uint32 hps;
};

rsz_input_crop_dims_t default_rszB_ip_params = {
    0,
    0                                                      // uint32 hps;
};

rsz_global_crop_dims_t default_rsz_glb_crop = {
    0,                                                     // uint32
                                                           // v_pos;/*Global
                                                           // frame start*/
    img_height,                                            // uint32
                                                           // v_size;/*Global 
                                                           // frame start*/
    0,                                                     // uint32
                                                           // h_pos;/*Crop
                                                           // Horizontal
                                                           // start */
    img_width,                                             // uint32
                                                           // h_size;/*Crop
                                                           // vertical start
                                                           // */

};
rsz_h_phs_cfg_t default_rsz_h_phs = { 0, 0 };

rsz_v_phs_cfg_t default_rsz_v_phs = { 0, 0 };

rsz_lpf_cfg_t default_rsz_h_lpf_cfg = {
    22,                                                    // c_intensity
    21                                                     // yintensity
};

rsz_lpf_cfg_t default_rsz_v_lpf_cfg = {
    15,                                                    // c_intensity
    14                                                     // yintensity
};

rsz_output_dims_t defult_rszA_op_dims = {
    (img_height / 2),
    (img_width / 2)
};

rsz_output_dims_t defult_rszB_op_dims = {
    (img_height / 2),
    (img_width / 2)
};

rsz_downscale_cfg_t default_rsz_downscale_params = {
    0, 0
};

rsz_common_cfg_t default_rsz_common_params = {
    RSZ_IP_IPIPE,                                          // RSZ_IP_PORT_SEL_T 
                                                           // ip_port;
    RSZ_WRT_DISABLE,                                       // RSZ_WRT_ENABLE_T 
                                                           // wrt_enable;
    0,                                                     // uint16
                                                           // rszA_dma_size;//not 
                                                           // needed in
                                                           // Bypass and
                                                           // passthru mode
    0,                                                     // uint16
                                                           // rszB_dma_size;// 
                                                           // 
    0,                                                     // uint16
                                                           // fifo_thr_low;
                                                           // //IN_FIFO_CTRL;
    0,                                                     // uint16
                                                           // fifo_thr_high;
    YUV422_IP_YUV422_OP,
    RSZ_YC_COSITED,
    0,                                                     // rszA_int_lines
    0,                                                     // rszB_int_lines
    65535
};

rsz_common_cfg_t default_rsz_common_params_1 = {
    RSZ_IP_IPIPEIF,                                        // RSZ_IP_PORT_SEL_T 
                                                           // ip_port;
    RSZ_WRT_DISABLE,                                       // RSZ_WRT_ENABLE_T 
                                                           // wrt_enable;
    0,                                                     // uint16
                                                           // rszA_dma_size;//not 
                                                           // needed in
                                                           // Bypass and
                                                           // passthru mode
    0,                                                     // uint16
                                                           // rszB_dma_size;// 
                                                           // 
    0,                                                     // uint16
                                                           // fifo_thr_low;
                                                           // //IN_FIFO_CTRL;
    0,                                                     // uint16
                                                           // fifo_thr_high;
    YUV422_IP_YUV420_OP,
    RSZ_YC_COSITED,
    0,                                                     // rszA_int_lines
    0,                                                     // rszB_int_lines
    65535
};

rsz_common_cfg_t default_rsz_common_params_2 = {
    RSZ_IP_IPIPEIF,                                        // RSZ_IP_PORT_SEL_T 
                                                           // ip_port;
    RSZ_WRT_DISABLE,                                       // RSZ_WRT_ENABLE_T 
                                                           // wrt_enable;
    0,                                                     // uint16
                                                           // rszA_dma_size;//not 
                                                           // needed in
                                                           // Bypass and
                                                           // passthru mode
    0,                                                     // uint16
                                                           // rszB_dma_size;// 
                                                           // 
    0,                                                     // uint16
                                                           // fifo_thr_low;
                                                           // //IN_FIFO_CTRL;
    0,                                                     // uint16
                                                           // fifo_thr_high;
    RAW_IP_RAW_OP,
    RSZ_YC_COSITED,
    0,                                                     // rszA_int_lines
    0,                                                     // rszB_int_lines
    65535
};

/* RESIZER A is used to down scale the image by a factor of 2 and resizer B
 * is configured to upscale by a factor of 2 */
ISP_RETURN rsz_params_init_1(rsz_config_t * rsz_cfg)
{
    ISP_RETURN retval = ISP_FAILURE;

    default_rszA_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.downscale_sel = RSZ_DOWNSCALE_ENABLE; // RSZ_DOWNSCALE_ENABLE;

    default_rszA_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszA_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszA_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;
    default_rszA_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszA_ip_op_params.h_phs_p = &default_rsz_h_phs;
    default_rszA_ip_op_params.v_phs_p = &default_rsz_v_phs;
    default_rszA_ip_op_params.ip_dims_p = &default_rszA_ip_params;
    default_rszA_ip_op_params.op_dims_p = &defult_rszA_op_dims; // defult_rszA_op_dims;

    default_rszA_ip_op_params.ratio_p = &default_rszA_resize_ratio; // default_rszA_resize_ratio;

    default_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszA_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    default_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;

    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x960;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width / 2) * 2;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer b config

    rszB_op_dims.v_size = (img_height * 2);
    rszB_op_dims.h_size = (img_width * 2);
    rszB_op_ratio.h_rsz_ratio = 128;
    rszB_op_ratio.v_rsz_ratio = 128;

    default_rszB_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;
    default_rszB_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszB_ip_op_params.downscale_sel = RSZ_DOWNSCALE_DISABLE;
    default_rszB_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszB_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszB_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;

    default_rszB_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszB_ip_op_params.h_phs_p = &default_rsz_h_phs;

    default_rszB_ip_op_params.v_phs_p = &default_rsz_v_phs;

    default_rszB_ip_op_params.ip_dims_p = &default_rszB_ip_params;  // crop
                                                                    // is 0

    default_rszB_ip_op_params.op_dims_p = &rszB_op_dims;

    default_rszB_ip_op_params.ratio_p = &rszB_op_ratio;

    default_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszB_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    default_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;

    rszB_op_address.base_addr = (uint32) op_img_buff_1;

    rszB_op_address.start_addr = (uint32) op_img_buff_1;
    rszB_op_address.end_pointer = 0x960;
    rszB_op_address.start_pointer = 0;
    rszB_op_address.start_offset = (img_width * 2) * 2;

    retval =
        rsz_cfg_op_addr(RESIZER_B, RSZ_YUV422_RAW_RGB_OP, &rszB_op_address);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer config
    rsz_cfg->mode = RESIZER_RESIZE;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_cfg_params;

    default_rsz_mode_cfg_params.resize_mode_cfgp =
        &default_rsz_resizer_mode_cfg_params;

    default_rsz_resizer_mode_cfg_params.rsz_A_cfg = &default_rszA_ip_op_params;
    default_rsz_resizer_mode_cfg_params.rsz_B_cfg = &default_rszB_ip_op_params;
    default_rsz_resizer_mode_cfg_params.crop_dims = &default_rsz_glb_crop;
    default_rsz_resizer_mode_cfg_params.rsz_common_params =
        &default_rsz_common_params;

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- Configures resizer in "passthru" mode for  yuv 422 op data, at op_img_buff
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN rsz_params_init_3(rsz_config_t * rsz_cfg)
{

    ISP_RETURN retval = ISP_FAILURE;

    rsz_cfg->mode = RESIZER_PASSTHRU;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_params_2;

    default_rsz_mode_params_2.passthru_mode_cfgp =
        &default_rsz_passthru_params_2;

    default_rsz_passthru_params_2.crop_dims = &default_rsz_glb_crop;
    default_rsz_passthru_params_2.flip_params = RSZ_V_NFLIP_H_NFLIP;

    default_rsz_passthru_params_2.pix_fmt = YUV422_IP_YUV422_OP;
    default_rsz_passthru_params_2.rsz_common_params =
        &default_rsz_common_params;

    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x00960;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width) * 2;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    return retval;

}

/* Resizer is configured in bypass mode,ip is yuv422 op is yuv422 */

ISP_RETURN rsz_params_init_4(rsz_config_t * rsz_cfg)
{

    ISP_RETURN retval = ISP_FAILURE;

    rsz_cfg->mode = RESIZER_BYPASS;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_params_2;

    default_rsz_mode_params_2.bp_mode_cfgp = &default_rsz_bp_params;

    default_rsz_bp_params.crop_dims = &default_rsz_glb_crop;
    default_rsz_bp_params.flip_params = RSZ_V_NFLIP_H_NFLIP;
    default_rsz_bp_params.pix_fmt = YUV422_IP_YUV422_OP;
    default_rsz_bp_params.rsz_common_params = &default_rsz_common_params;

    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x964;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width) * 2;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    return retval;
}

/* Resizer configuresd to give out one RGB frame upscaled by 2 image and one
 * YUV422 frame downscaled */
ISP_RETURN rsz_params_init_5(rsz_config_t * rsz_cfg)
{
    ISP_RETURN retval = ISP_FAILURE;

    default_rszA_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.downscale_sel = RSZ_DOWNSCALE_ENABLE; // RSZ_DOWNSCALE_ENABLE;

    default_rszA_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszA_ip_op_params.flip_param = RSZ_V_NFLIP_H_FLIP;

    default_rszA_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;
    default_rszA_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszA_ip_op_params.h_phs_p = &default_rsz_h_phs;
    default_rszA_ip_op_params.v_phs_p = &default_rsz_v_phs;
    default_rszA_ip_op_params.ip_dims_p = &default_rszA_ip_params;
    default_rszA_ip_op_params.op_dims_p = &defult_rszA_op_dims; // defult_rszA_op_dims;

    default_rszA_ip_op_params.ratio_p = &default_rszA_resize_ratio; // default_rszA_resize_ratio;

    default_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszA_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    default_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;

    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x964;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width / 2) * 2;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer b config

    rszB_op_dims.v_size = (img_height * 2);
    rszB_op_dims.h_size = (img_width * 2);
    rszB_op_ratio.h_rsz_ratio = 128;
    rszB_op_ratio.v_rsz_ratio = 128;

    default_rszB_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;
    default_rszB_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszB_ip_op_params.downscale_sel = RSZ_DOWNSCALE_DISABLE;
    default_rszB_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszB_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszB_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;

    default_rszB_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszB_ip_op_params.h_phs_p = &default_rsz_h_phs;

    default_rszB_ip_op_params.v_phs_p = &default_rsz_v_phs;

    default_rszB_ip_op_params.ip_dims_p = &default_rszB_ip_params;  // crop
                                                                    // is 0

    default_rszB_ip_op_params.op_dims_p = &rszB_op_dims;

    default_rszB_ip_op_params.ratio_p = &rszB_op_ratio;

    /* Enable RGB565 o/p */

    default_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_ENABLE;
    rsz_B_rgb_params.mask_ctrl = RSZ_RGB_MSK1_DISABLE_MSK0_DISABLE;
    rsz_B_rgb_params.rgb_pix_size = RSZ_RGB_PIX_SIZE_16;
    default_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_ENABLE;
    default_rszB_ip_op_params.rgb_cfg_p = &rsz_B_rgb_params;

    default_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;

    rszB_op_address.base_addr = (uint32) op_img_buff_1;

    rszB_op_address.start_addr = (uint32) op_img_buff_1;
    rszB_op_address.end_pointer = 0x00960;
    rszB_op_address.start_pointer = 0;
    rszB_op_address.start_offset = (img_width * 2) * 2;

    retval =
        rsz_cfg_op_addr(RESIZER_B, RSZ_YUV422_RAW_RGB_OP, &rszB_op_address);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer config
    rsz_cfg->mode = RESIZER_RESIZE;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_cfg_params;

    default_rsz_mode_cfg_params.resize_mode_cfgp =
        &default_rsz_resizer_mode_cfg_params;

    default_rsz_resizer_mode_cfg_params.rsz_A_cfg = &default_rszA_ip_op_params;
    default_rsz_resizer_mode_cfg_params.rsz_B_cfg = &default_rszB_ip_op_params;
    default_rsz_resizer_mode_cfg_params.crop_dims = &default_rsz_glb_crop;
    default_rsz_resizer_mode_cfg_params.rsz_common_params =
        &default_rsz_common_params;

    return retval;

}

/* Resizer configuresd to give out one YUV420 frame downscaled image and one
 * YUV420 frame upscaled, ip is from ipipe-if */
ISP_RETURN rsz_params_init_6(rsz_config_t * rsz_cfg)
{
    ISP_RETURN retval = ISP_FAILURE;

    default_rszA_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.downscale_sel = RSZ_DOWNSCALE_ENABLE; // RSZ_DOWNSCALE_ENABLE;

    default_rszA_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszA_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszA_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;
    default_rszA_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszA_ip_op_params.h_phs_p = &default_rsz_h_phs;
    default_rszA_ip_op_params.v_phs_p = &default_rsz_v_phs;
    default_rszA_ip_op_params.ip_dims_p = &default_rszA_ip_params;
    default_rszA_ip_op_params.op_dims_p = &defult_rszA_op_dims; // defult_rszA_op_dims;

    default_rszA_ip_op_params.ratio_p = &default_rszA_resize_ratio; // default_rszA_resize_ratio;

    default_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszA_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    default_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV420_YC_OP;
    /* YUV420 op Y address */
    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x964;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width / 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV420_Y_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    /* YUV420 op C address */

    rsz_A_c_op_addr.base_addr =
        (uint32) ((uint32) (op_img_buff) + (img_height * img_width / 4));
    rsz_A_c_op_addr.start_addr =
        (uint32) ((uint32) (op_img_buff) + (img_height * img_width / 4));
    rsz_A_c_op_addr.end_pointer = 0x964;
    rsz_A_c_op_addr.start_pointer = 0;
    rsz_A_c_op_addr.start_offset = (img_width / 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV420_C_OP, &rsz_A_c_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer b config
    /***************************************************/
    rszB_op_dims.v_size = (img_height * 2);
    rszB_op_dims.h_size = (img_width * 2);
    rszB_op_ratio.h_rsz_ratio = 128;
    rszB_op_ratio.v_rsz_ratio = 128;

    default_rszB_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;
    default_rszB_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszB_ip_op_params.downscale_sel = RSZ_DOWNSCALE_DISABLE;
    default_rszB_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszB_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszB_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;

    default_rszB_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszB_ip_op_params.h_phs_p = &default_rsz_h_phs;

    default_rszB_ip_op_params.v_phs_p = &default_rsz_v_phs;

    default_rszB_ip_op_params.ip_dims_p = &default_rszB_ip_params;  // crop
                                                                    // is 0

    default_rszB_ip_op_params.op_dims_p = &rszB_op_dims;

    default_rszB_ip_op_params.ratio_p = &rszB_op_ratio;

    default_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszB_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    /* YUV420 op */
    default_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV420_YC_OP;

    /* Yuv420 Y address */
    rszB_op_address.base_addr = (uint32) (op_img_buff_1);
    rszB_op_address.start_addr = (uint32) (op_img_buff_1);
    rszB_op_address.end_pointer = 0x00960;
    rszB_op_address.start_pointer = 0;
    rszB_op_address.start_offset = (img_width * 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_B, RSZ_YUV420_Y_OP, &rszB_op_address);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    /* Yuv420 C address */
    rszB_c_op_address.base_addr =
        (uint32) ((uint32) (op_img_buff_1) + (img_height * img_width * 4));
    rszB_c_op_address.start_addr =
        (uint32) ((uint32) (op_img_buff_1) + (img_height * img_width * 4));
    rszB_c_op_address.end_pointer = 0x00960;
    rszB_c_op_address.start_pointer = 0;
    rszB_c_op_address.start_offset = (img_width * 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_B, RSZ_YUV420_C_OP, &rszB_c_op_address);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer config
    rsz_cfg->mode = RESIZER_RESIZE;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_cfg_params;

    default_rsz_mode_cfg_params.resize_mode_cfgp =
        &default_rsz_resizer_mode_cfg_params;

    default_rsz_resizer_mode_cfg_params.rsz_A_cfg = &default_rszA_ip_op_params;
    default_rsz_resizer_mode_cfg_params.rsz_B_cfg = &default_rszB_ip_op_params;
    default_rsz_resizer_mode_cfg_params.crop_dims = &default_rsz_glb_crop;
    default_rsz_resizer_mode_cfg_params.rsz_common_params =
        &default_rsz_common_params_1;

    return retval;

}

/* Resizer configuresd to give out one YUV420 frame downscaled image and one
 * YUV420 frame upscaled, ip is from ipipe */
ISP_RETURN rsz_params_init_6_1(rsz_config_t * rsz_cfg)
{
    ISP_RETURN retval = ISP_FAILURE;

    default_rszA_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszA_ip_op_params.downscale_sel = RSZ_DOWNSCALE_ENABLE; // RSZ_DOWNSCALE_ENABLE;

    default_rszA_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszA_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszA_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;
    default_rszA_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszA_ip_op_params.h_phs_p = &default_rsz_h_phs;
    default_rszA_ip_op_params.v_phs_p = &default_rsz_v_phs;
    default_rszA_ip_op_params.ip_dims_p = &default_rszA_ip_params;
    default_rszA_ip_op_params.op_dims_p = &defult_rszA_op_dims; // defult_rszA_op_dims;

    default_rszA_ip_op_params.ratio_p = &default_rszA_resize_ratio; // default_rszA_resize_ratio;

    default_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszA_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    default_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV420_YC_OP;
    /* YUV420 op Y address */
    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x964;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width / 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV420_Y_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    /* YUV420 op C address */

    rsz_A_c_op_addr.base_addr =
        (uint32) ((uint32) (op_img_buff) + (img_height * img_width / 4));
    rsz_A_c_op_addr.start_addr =
        (uint32) ((uint32) (op_img_buff) + (img_height * img_width / 4));
    rsz_A_c_op_addr.end_pointer = 0x964;
    rsz_A_c_op_addr.start_pointer = 0;
    rsz_A_c_op_addr.start_offset = (img_width / 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV420_C_OP, &rsz_A_c_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer b config
    /***************************************************/
    rszB_op_dims.v_size = (img_height * 2);
    rszB_op_dims.h_size = (img_width * 2);
    rszB_op_ratio.h_rsz_ratio = 128;
    rszB_op_ratio.v_rsz_ratio = 128;

    default_rszB_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;
    default_rszB_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

    default_rszB_ip_op_params.downscale_sel = RSZ_DOWNSCALE_DISABLE;
    default_rszB_ip_op_params.downscale_p = &default_rsz_downscale_params;

    default_rszB_ip_op_params.flip_param = RSZ_V_NFLIP_H_NFLIP;

    default_rszB_ip_op_params.h_lpf_p = &default_rsz_h_lpf_cfg;

    default_rszB_ip_op_params.v_lpf_p = &default_rsz_v_lpf_cfg;

    default_rszB_ip_op_params.h_phs_p = &default_rsz_h_phs;

    default_rszB_ip_op_params.v_phs_p = &default_rsz_v_phs;

    default_rszB_ip_op_params.ip_dims_p = &default_rszB_ip_params;  // crop
                                                                    // is 0

    default_rszB_ip_op_params.op_dims_p = &rszB_op_dims;

    default_rszB_ip_op_params.ratio_p = &rszB_op_ratio;

    default_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
    default_rszB_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

    /* YUV420 op */
    default_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV420_YC_OP;

    /* Yuv420 Y address */
    rszB_op_address.base_addr = (uint32) (op_img_buff_1);
    rszB_op_address.start_addr = (uint32) (op_img_buff_1);
    rszB_op_address.end_pointer = 0x00960;
    rszB_op_address.start_pointer = 0;
    rszB_op_address.start_offset = (img_width * 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_B, RSZ_YUV420_Y_OP, &rszB_op_address);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    /* Yuv420 C address */
    rszB_c_op_address.base_addr =
        (uint32) ((uint32) (op_img_buff_1) + (img_height * img_width * 4));
    rszB_c_op_address.start_addr =
        (uint32) ((uint32) (op_img_buff_1) + (img_height * img_width * 4));
    rszB_c_op_address.end_pointer = 0x00960;
    rszB_c_op_address.start_pointer = 0;
    rszB_c_op_address.start_offset = (img_width * 2) * 1;

    retval = rsz_cfg_op_addr(RESIZER_B, RSZ_YUV420_C_OP, &rszB_c_op_address);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    // resizer config
    rsz_cfg->mode = RESIZER_RESIZE;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_cfg_params;

    default_rsz_mode_cfg_params.resize_mode_cfgp =
        &default_rsz_resizer_mode_cfg_params;

    default_rsz_resizer_mode_cfg_params.rsz_A_cfg = &default_rszA_ip_op_params;
    default_rsz_resizer_mode_cfg_params.rsz_B_cfg = &default_rszB_ip_op_params;
    default_rsz_resizer_mode_cfg_params.crop_dims = &default_rsz_glb_crop;
    default_rsz_resizer_mode_cfg_params.rsz_common_params =
        &default_rsz_common_params;

    return retval;

}

/* Resizer is configured in bypass mode, ip is RAW and op is RAW */

ISP_RETURN rsz_params_init_7(rsz_config_t * rsz_cfg)
{

    ISP_RETURN retval = ISP_FAILURE;

    rsz_cfg->mode = RESIZER_BYPASS;
    rsz_cfg->mode_cfg_p = &default_rsz_mode_params_2;

    default_rsz_mode_params_2.bp_mode_cfgp = &default_rsz_bp_params;

    default_rsz_bp_params.crop_dims = &default_rsz_glb_crop;
    default_rsz_bp_params.flip_params = RSZ_V_NFLIP_H_NFLIP;
    default_rsz_bp_params.pix_fmt = RAW_IP_RAW_OP;
    default_rsz_bp_params.rsz_common_params = &default_rsz_common_params_2;

    rsz_A_op_addr.base_addr = (uint32) op_img_buff;
    rsz_A_op_addr.start_addr = (uint32) op_img_buff;
    rsz_A_op_addr.end_pointer = 0x0960;
    rsz_A_op_addr.start_pointer = 0;
    rsz_A_op_addr.start_offset = (img_width) * 2;

    retval = rsz_cfg_op_addr(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, &rsz_A_op_addr);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_cfg_op_addr failed");

    return retval;
}

/***********************************RESIZER  PARAMS END***********************************************************/
/**********************************************************************************************************/

/* ==================DEFAULT IPIEIF PARAMS
 * START============================================== */

/* ================================================================ */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from dummy PP and ipipe i/p is from 
 SDRAM,     PP-> ipipe-if->RAM  , in 1'st pass
 RAM -> ipipe-if-> ipipe.in 2'nd pass   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */
ipipeif_dpcm_cfg_t dpcm_cfg;

ipipeif_dpc_cfg_t dpc_cfg = {
    IPIPEIF_FEATURE_ENABLE,
    20
};

ISP_RETURN ipipeif_params_init_1(ipipeif_path_cfg_t * cfg)
{
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg = &dpcm_cfg;
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg = &dpc_cfg;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_SDRAM_RAW;
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg->dpcm_en = IPIPEIF_FEATURE_ENABLE;
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg->dpcm_bit_size =
        IPIPEIF_DPCM_BIT_SIZE_8_12;
    // cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg->dpcm_predictor=IPIPEIF_DPCM_PREDICTION_SIMPLE;
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg->dpcm_predictor =
        IPIPEIF_DPCM_PREDICTION_ADVANCED;
    // cfg->ipipe_ip_cfg.sdram_ip.dpc_cfg->dpc2_en=IPIPEIF_FEATURE_ENABLE;
    cfg->ipipe_ip_cfg.sdram_ip.data_shift = DATASFT_BITS9_0;
    // cfg->ipipe_ip_cfg.sdram_ip.unpack=UNPACK_PACK_8_BITS_PIXEL_ALAW;
    cfg->ipipe_ip_cfg.sdram_ip.unpack = UNPACK_PACK_8_BITS_PIXEL_LINEAR;
    cfg->ipipe_ip_cfg.sdram_ip.crop_dims.ppln_hs_interval = img_width;
    cfg->ipipe_ip_cfg.sdram_ip.crop_dims.lpfr_vs_interval = img_height;
    cfg->ipipe_ip_cfg.sdram_ip.crop_dims.hnum = img_width;
    cfg->ipipe_ip_cfg.sdram_ip.crop_dims.vnum = img_height;
    cfg->ipipe_ip_cfg.sdram_ip.address = (uint32) img_buff;
    cfg->ipipe_ip_cfg.sdram_ip.adofs = (img_width * 1) >> 5;    /* because
                                                                 * pixels are 
                                                                 * compressed 
                                                                 * and size
                                                                 * is one
                                                                 * byte */
    /* 32-byte aligned , 27 bit address uint32 address; uint16 adofs; */
    cfg->ipipe_ip_cfg.sdram_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.sdram_ip.gain = 512;
    cfg->ipipe_ip_cfg.sdram_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;
    cfg->ipipe_ip_cfg.sdram_ip.resize = 16;
    cfg->ipipe_ip_cfg.sdram_ip.vd_polarity = 1;
    cfg->ipipe_ip_cfg.sdram_ip.hd_polarity = 1;
    cfg->ipipe_ip_cfg.sdram_ip.int_sw = 1;                 // ########TODO
                                                           // CHECK this
    cfg->ipipe_ip_cfg.sdram_ip.align_sync = 1;             // //########TODO
                                                           // CHECK this
    cfg->ipipe_ip_cfg.sdram_ip.ini_rsz = 1;                // ########TODO
                                                           // CHECK this 
    cfg->ipipe_ip_cfg.sdram_ip.clkdiv_m = 1;
    cfg->ipipe_ip_cfg.sdram_ip.clkdiv_n = 1;
    cfg->ipipe_ip_cfg.sdram_ip.ip_clk_sel = IPIPEIF_IP_CLK_IPIPEIF_CLK_DIV;

    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_PP_RAW;         // IPIPEIF_ISIF_INPSRC_SDRAM_YUV
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg->dpc_thr = 0;
    cfg->isif_ip_cfg.pp_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.int_sw = 1;
    cfg->isif_ip_cfg.pp_ip.align_sync = 0;
    cfg->isif_ip_cfg.pp_ip.ini_rsz = 0;

    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from SDRAM and ipipe i/p is from 
 ISIF,     RAM-> ipipe-if-> ISIF -> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN ipipeif_params_init_2(ipipeif_path_cfg_t * cfg)
{
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg = &dpcm_cfg;
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg = &dpc_cfg;
    cfg->ipipe_ip_cfg.sdram_ip.dpc2_cfg = &dpc_cfg;

    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_SDRAM_RAW;      // IPIPEIF_ISIF_INPSRC_SDRAM_YUV

    cfg->isif_ip_cfg.sdram_ip.data_shift = DATASFT_BITS11_0;    // DATASFT_BITS11_0
    cfg->isif_ip_cfg.sdram_ip.unpack = UNPACK_NORMAL_16_BITS_PIXEL; // UNPACK_NORMAL_16_BITS_PIXEL
    // cfg->isif_ip_cfg.sdram_ip.oneshot=1;

#if ZEBU
    cfg->isif_ip_cfg.sdram_ip.crop_dims.ppln_hs_interval = img_width + 16;  // 
    cfg->isif_ip_cfg.sdram_ip.crop_dims.lpfr_vs_interval = img_height + 16; // 
#else
    cfg->isif_ip_cfg.sdram_ip.crop_dims.ppln_hs_interval = img_width;   // 
    cfg->isif_ip_cfg.sdram_ip.crop_dims.lpfr_vs_interval = img_height;  // 
#endif
    cfg->isif_ip_cfg.sdram_ip.crop_dims.hnum = img_width;  // 
    cfg->isif_ip_cfg.sdram_ip.crop_dims.vnum = img_height; // 
    cfg->isif_ip_cfg.sdram_ip.address = (uint32) img_buff; // 
    cfg->isif_ip_cfg.sdram_ip.adofs = (img_width * 2) >> 5; // needs to be in 
                                                            // terms of
                                                            // 32-bit words
    cfg->isif_ip_cfg.sdram_ip.dpcm_cfg->dpcm_en = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->isif_ip_cfg.sdram_ip.dpcm_cfg->dpcm_predictor = IPIPEIF_DPCM_PREDICTION_SIMPLE;    // 
    cfg->isif_ip_cfg.sdram_ip.dpcm_cfg->dpcm_bit_size = IPIPEIF_DPCM_BIT_SIZE_8_10; // 
    cfg->isif_ip_cfg.sdram_ip.dpc2_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;   // 
    cfg->isif_ip_cfg.sdram_ip.dpc2_cfg->dpc_thr = 0;       // 
    cfg->isif_ip_cfg.sdram_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.sdram_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.sdram_ip.int_sw = 1;
    cfg->isif_ip_cfg.sdram_ip.align_sync = 0;
    cfg->isif_ip_cfg.sdram_ip.ini_rsz = 0;
    cfg->isif_ip_cfg.sdram_ip.clkdiv_m = 0x01;
    cfg->isif_ip_cfg.sdram_ip.clkdiv_n = 0x08;
    cfg->isif_ip_cfg.sdram_ip.ip_clk_sel = IPIPEIF_IP_CLK_IPIPEIF_CLK_DIV;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_ISIF;

    cfg->ipipe_ip_cfg.isif_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.isif_ip.gain = 512;
    cfg->ipipe_ip_cfg.isif_ip.YUV16 = RAW_DATA;            // YCBCR_16_BIT_DATA 
                                                           // RAW_DATA
    cfg->ipipe_ip_cfg.isif_ip.YUV8 = YUV8_YUV16;           // YUV8_YUV16
                                                           // YUV8_YUV8to16
    cfg->ipipe_ip_cfg.isif_ip.YUV8P = YUV8P_C_EVEN_Y_ODD;  // YUV8P_C_EVEN_Y_ODD 
                                                           // YUV8P_Y_EVEN_C_ODD
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;   // IPIPEIF_FEATURE_ENABLE
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg->dpc_thr = 0;       // 
    cfg->ipipe_ip_cfg.isif_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->ipipe_ip_cfg.isif_ip.resize = 16;                 // 
    cfg->ipipe_ip_cfg.isif_ip.vd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.hd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.int_sw = 1;                  // 
    cfg->ipipe_ip_cfg.isif_ip.align_sync = 0;              // 
    cfg->ipipe_ip_cfg.isif_ip.ini_rsz = 0;                 // 

    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from PP RAW and ipipe i/p is from 
 ISIF,     RAM-> ipipe-if-> ISIF -> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN ipipeif_params_init_3(ipipeif_path_cfg_t * cfg)
{
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg = &dpcm_cfg;
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg = &dpc_cfg;
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg = &dpc_cfg;

    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_PP_RAW;         // IPIPEIF_ISIF_INPSRC_PP_RAW
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg->dpc_thr = 0;
    cfg->isif_ip_cfg.pp_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.int_sw = 1;
    cfg->isif_ip_cfg.pp_ip.align_sync = 0;
    cfg->isif_ip_cfg.pp_ip.ini_rsz = 0;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_ISIF;
    cfg->ipipe_ip_cfg.isif_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.isif_ip.gain = 512;
    cfg->ipipe_ip_cfg.isif_ip.YUV16 = RAW_DATA;            // YCBCR_16_BIT_DATA 
                                                           // RAW_DATA
    cfg->ipipe_ip_cfg.isif_ip.YUV8 = YUV8_YUV16;           // YUV8_YUV16
                                                           // YUV8_YUV8to16
    cfg->ipipe_ip_cfg.isif_ip.YUV8P = YUV8P_C_EVEN_Y_ODD;  // YUV8P_C_EVEN_Y_ODD 
                                                           // YUV8P_Y_EVEN_C_ODD
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;   // IPIPEIF_FEATURE_ENABLE
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg->dpc_thr = 0;       // 
    cfg->ipipe_ip_cfg.isif_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->ipipe_ip_cfg.isif_ip.resize = 16;                 // 
    cfg->ipipe_ip_cfg.isif_ip.vd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.hd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.int_sw = 1;                  // 
    cfg->ipipe_ip_cfg.isif_ip.align_sync = 0;              // 
    cfg->ipipe_ip_cfg.isif_ip.ini_rsz = 0;                 // 

    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from SDRAM and ipipe i/p is from 
 ISIF,     RAM-> ipipe-if-> ISIF -> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN ipipeif_params_init_4(ipipeif_path_cfg_t * cfg)
{
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg = &dpcm_cfg;
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg = &dpc_cfg;
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg = &dpc_cfg;

    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_PP_RAW;         // IPIPEIF_ISIF_INPSRC_SDRAM_YUV
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;
    cfg->isif_ip_cfg.pp_ip.dpc1_cfg->dpc_thr = 0;
    cfg->isif_ip_cfg.pp_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;
    cfg->isif_ip_cfg.pp_ip.align_sync = 0;
    cfg->isif_ip_cfg.pp_ip.ini_rsz = 0;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_ISIF;

    cfg->ipipe_ip_cfg.isif_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.isif_ip.gain = 512;
    cfg->ipipe_ip_cfg.isif_ip.YUV16 = RAW_DATA;            // YCBCR_16_BIT_DATA 
                                                           // RAW_DATA
    cfg->ipipe_ip_cfg.isif_ip.YUV8 = YUV8_YUV16;           // YUV8_YUV16
                                                           // YUV8_YUV8to16
    cfg->ipipe_ip_cfg.isif_ip.YUV8P = YUV8P_C_EVEN_Y_ODD;  // YUV8P_C_EVEN_Y_ODD 
                                                           // YUV8P_Y_EVEN_C_ODD
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg->dpc_en = IPIPEIF_FEATURE_DISABLE;   // IPIPEIF_FEATURE_ENABLE
    cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg->dpc_thr = 0;       // 
    cfg->ipipe_ip_cfg.isif_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->ipipe_ip_cfg.isif_ip.resize = 16;                 // 
    cfg->ipipe_ip_cfg.isif_ip.vd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.hd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;   // 
    cfg->ipipe_ip_cfg.isif_ip.align_sync = 0;              // 
    cfg->ipipe_ip_cfg.isif_ip.ini_rsz = 0;                 // 

    return ISP_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Configures Ipipe-if test parameters,  ipipe i/p is from  SDRAM,     PP-> ipipe-if->RAM
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN ipipeif_params_init_5(ipipeif_path_cfg_t * cfg)
{
    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_SDRAM_YUV;

    cfg->ipipe_ip_cfg.yuv_ip.address = (uint32) img_buff;
    cfg->ipipe_ip_cfg.yuv_ip.adofs = (img_width * 2) >> 5;
    cfg->ipipe_ip_cfg.yuv_ip.align_sync = 1;
    cfg->ipipe_ip_cfg.yuv_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.yuv_ip.clkdiv_m = 1;
    cfg->ipipe_ip_cfg.yuv_ip.clkdiv_n = 1;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.ppln_hs_interval = img_width;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.lpfr_vs_interval = img_height;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.hnum = img_width;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.vnum = img_height;

    cfg->ipipe_ip_cfg.yuv_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;
    cfg->ipipe_ip_cfg.yuv_ip.hd_polarity = 1;
    cfg->ipipe_ip_cfg.yuv_ip.ini_rsz = 1;
    cfg->ipipe_ip_cfg.yuv_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;
    cfg->ipipe_ip_cfg.yuv_ip.ip_clk_sel = IPIPEIF_IP_CLK_IPIPEIF_CLK_DIV;

    cfg->ipipe_ip_cfg.yuv_ip.resize = 16;
    cfg->ipipe_ip_cfg.yuv_ip.vd_polarity = 1;

    return ISP_SUCCESS;
}

#if 0
/* ================================================================ */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from SDRAM and ipipe i/p is from 
 ISIF,     RAM-> ipipe-if-> ISIF -> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN ipipeif_params_init_6(ipipeif_path_cfg_t * cfg)
{
    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_PP_RAW;         // IPIPEIF_ISIF_INPSRC_SDRAM_YUV
    cfg->isif_ip_cfg.pp_ip.dpc1_en = IPIPEIF_FEATURE_DISABLE;
    cfg->isif_ip_cfg.pp_ip.dpc1_thr = 0;
    cfg->isif_ip_cfg.pp_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;
    cfg->isif_ip_cfg.pp_ip.align_sync = 0;
    cfg->isif_ip_cfg.pp_ip.ini_rsz = 0;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_ISIF;

    cfg->ipipe_ip_cfg.isif_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.isif_ip.gain = 512;
    cfg->ipipe_ip_cfg.isif_ip.YUV16 = RAW_DATA;            // YCBCR_16_BIT_DATA 
                                                           // RAW_DATA
    cfg->ipipe_ip_cfg.isif_ip.YUV8 = YUV8_YUV16;           // YUV8_YUV16
                                                           // YUV8_YUV8to16
    cfg->ipipe_ip_cfg.isif_ip.YUV8P = YUV8P_C_EVEN_Y_ODD;  // YUV8P_C_EVEN_Y_ODD 
                                                           // YUV8P_Y_EVEN_C_ODD
    cfg->ipipe_ip_cfg.isif_ip.dpc1_en = IPIPEIF_FEATURE_DISABLE;    // IPIPEIF_FEATURE_ENABLE
    cfg->ipipe_ip_cfg.isif_ip.dpc1_thr = 0;                // 
    cfg->ipipe_ip_cfg.isif_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->ipipe_ip_cfg.isif_ip.resize = 16;                 // 
    cfg->ipipe_ip_cfg.isif_ip.vd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.hd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;   // 
    cfg->ipipe_ip_cfg.isif_ip.align_sync = 0;              // 
    cfg->ipipe_ip_cfg.isif_ip.ini_rsz = 0;                 // 

    return ISP_SUCCESS;
}

#endif

/* ==================DEFAULT IPIEIF PARAMS
 * END============================================== */

/*****************TEST configuration structures FOR CCP2**********************************/
// CSL_Ccp2Regs *CCP2_regs;
ccp2_chan_enable_t chan_enable_t_test;

ccp2_path_enable_t path_enable_t_test;

ccp2_lcm_enable_t lcm_enable_t_test;

/*************************CCP2_param_init**********************/

/* ===================================================================
 *  @func     ccp2_test_path_param_init                                               
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
ISP_RETURN ccp2_test_path_param_init(ccp2_path_enable_t * cfg)
{
    memcpy((void *) (&(cfg->ctrl_global)),
           (const void *) (&ccp2_global_ctrl_default_params_12),
           sizeof(ccp2_global_ctrl));

    return ISP_SUCCESS;
}

// This is to configure the memory channel of ccp2
/* ===================================================================
 *  @func     ccp2_test_lcm_enable_init                                               
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
ISP_RETURN ccp2_test_lcm_enable_init(ccp2_lcm_enable_t * lcm_cfg)
{
    ccp2_lcm_config_default_dummy_params_12.LCM_SRC_ADDR = (uint32) img_buff;
    memcpy((void *) (&(lcm_cfg->mem_chan_ctrl)),
           (const void *) (&ccp2_lcm_ctrl_default_dummy_params_12),
           sizeof(ccp2_lcm_ctrl));
    memcpy((void *) (&(lcm_cfg->mem_chan_config)),
           (const void *) (&ccp2_lcm_config_default_dummy_params_12),
           sizeof(ccp2_lcm_config));

    return ISP_SUCCESS;
}

// This is to configure the memory channel of ccp2

/* ===================================================================
 *  @func     ccp2_memory_test_params_init                                               
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
ISP_RETURN ccp2_memory_test_params_init()
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval = ccp2_test_path_param_init(&path_enable_t_test);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_test_path_enable_t failed");

    retval = ccp2_test_lcm_enable_init(&lcm_enable_t_test);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_memory_enable_init failed");
    return ISP_SUCCESS;
}

/* *****************************CCP2_Config********************************* */
/***
  Configues CCP2 to reead from memory
 ****
 */
/* ===================================================================
 *  @func     ccp2_memory_config                                               
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
ISP_RETURN ccp2_memory_config()
{
    CCP2_RETURN retval = CCP2_SUCCESS;

    retval = ccp2_config(&path_enable_t_test);
    if (retval != CCP2_SUCCESS)
        ISP_PRINT("\n ccp2_path_enable_init failed");

    retval = CCP2_Read_from_Memory_Config(&lcm_enable_t_test, MEMORY_READ);
    if (retval != CCP2_SUCCESS)
        ISP_PRINT("\n ccp2_memory_enable_init failed");
    return retval;
}

/********ccp ONLY**************/

ISP_RETURN ccp2_test_path_param_init_20(ccp2_path_enable_t * cfg)
{
    memcpy((void *) (&(cfg->ctrl_global)),
           (const void *) (&ccp2_global_ctrl_default_params_12),
           sizeof(ccp2_global_ctrl));

    return ISP_SUCCESS;
}

// This is to configure the memory channel of ccp2
ISP_RETURN ccp2_test_lcm_enable_init_20(ccp2_lcm_enable_t * lcm_cfg)
{
    ccp2_lcm_config_default_dummy_params_20.LCM_SRC_ADDR = (uint32) img_buff;
    ccp2_lcm_config_default_dummy_params_20.LCM_DST_ADDR = (uint32) op_img_buff;
    memcpy((void *) (&(lcm_cfg->mem_chan_ctrl)),
           (const void *) (&ccp2_lcm_ctrl_default_dummy_params_20),
           sizeof(ccp2_lcm_ctrl));
    memcpy((void *) (&(lcm_cfg->mem_chan_config)),
           (const void *) (&ccp2_lcm_config_default_dummy_params_20),
           sizeof(ccp2_lcm_config));

    return ISP_SUCCESS;
}

// This is to configure the memory channel of ccp2

ISP_RETURN ccp2_memory_test_params_init_20()
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval = ccp2_test_path_param_init_20(&path_enable_t_test);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_test_path_enable_t failed");

    retval = ccp2_test_lcm_enable_init_20(&lcm_enable_t_test);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_memory_enable_init failed");
    return ISP_SUCCESS;
}

/* *****************************CCP2_Config********************************* */
/***
  Configues CCP2 to reead from memory
 ****
 */
ISP_RETURN ccp2_memory_config_20()
{
    CCP2_RETURN retval = CCP2_SUCCESS;

    retval = ccp2_config(&path_enable_t_test);
    if (retval != CCP2_SUCCESS)
        ISP_PRINT("\n ccp2_path_enable_init failed");

    retval = CCP2_Read_from_Memory_Config(&lcm_enable_t_test, MEMORY_READ);
    if (retval != CCP2_SUCCESS)
        ISP_PRINT("\n ccp2_memory_enable_init failed");
    return retval;
}

/***********************CCP2 test configuration structures finished***********************************************************/

/***************************************************/

/**************************CSI2 test configuration structures***********************/
csi2_cfg_t test_csi2_config;

csi2_phyaddress_t csi2_default_ping = 0x40000000;

csi2_phyaddress_t csi2_default_pong = 0x40000000;

csi2_phyaddress_t csi2_default_offset = 0x00000000;

csi2_ctx_transcode_cfg_t csi2_default_transcode_parameters = {

    CSI2_DISABLE_TRANSCODE,                                /* TRANSCODE MODE */
    640,                                                   /* Pixels to
                                                            * output per line 
                                                            * when the values 
                                                            * is between 1
                                                            * and 8191. */
    0,                                                     /* Number of
                                                            * pixels to skip
                                                            * horizontally */
    480,                                                   /* Pixels to
                                                            * output per line 
                                                            * when the values 
                                                            * is between 1
                                                            * and 8191. */
    0                                                      /* NUmber of
                                                            * pixels to skip
                                                            * vertically */
};

csi2_ctx_cfg_t csi2_ctx_default_params = {
    CSI2_DISABLED_BIT,                                     // byteswap
    CSI2_DISABLED_BIT,                                     // generic
    1,                                                     // fec_number 
    0,                                                     // count 
    CSI2_DISABLED_BIT,                                     // EOF_EN
    CSI2_DISABLED_BIT,                                     // EOL_EN
    CSI2_DISABLED_BIT,                                     // CS_EN
    CSI2_DISABLED_BIT,                                     // COUNT_UNLOCK
    CSI2_DISABLED_BIT,                                     // VP_FORCE
    CSI2_ENABLED_BIT,                                      // LINE_MODULO

    CSI2_USER_RAW6,                                        /* pixel format of 
                                                            * USER_DEFINED
                                                            * 0:RAW6; 1:RAW7; 
                                                            * 2:RAW8 */
    CSI2_CHANNEL_0,                                        /* Channel
                                                            * Id:0,1,2,3 */
    CSI2_SIMPLE_DPCM,                                      /* 0:Advanced;
                                                            * 1:Simple */
    CSI2_RAW8,                                             /* Data format
                                                            * selection. */

    0x0,                                                   /* alpha */
    0x0,                                                   /* line number */

    0x00,                                                  /* 8-bit
                                                            * interrupt-bit
                                                            * mask */

};

csi2_ctx_cfg_t csi2_ctx_default_params_1 = {
    CSI2_DISABLED_BIT,                                     // byteswap
    CSI2_DISABLED_BIT,                                     // generic
    1,                                                     // fec_number 
    0,                                                     // count 
    CSI2_DISABLED_BIT,                                     // EOF_EN
    CSI2_DISABLED_BIT,                                     // EOL_EN
    CSI2_DISABLED_BIT,                                     // CS_EN
    CSI2_DISABLED_BIT,                                     // COUNT_UNLOCK
    CSI2_DISABLED_BIT,                                     // VP_FORCE
    CSI2_ENABLED_BIT,                                      // LINE_MODULO

    CSI2_USER_RAW6,                                        /* pixel format of 
                                                            * USER_DEFINED
                                                            * 0:RAW6; 1:RAW7; 
                                                            * 2:RAW8 */
    CSI2_CHANNEL_3,                                        /* Channel
                                                            * Id:0,1,2,3 */
    CSI2_SIMPLE_DPCM,                                      /* 0:Advanced;
                                                            * 1:Simple */
    CSI2_RAW10,                                            /* Data format
                                                            * selection. */

    0x0,                                                   /* alpha */
    0x0,                                                   /* line number */

    0x00,                                                  /* 8-bit
                                                            * interrupt-bit
                                                            * mask */

};

csi2_complexio_cfg_t csi2_complexio_cfg_default_params = {

    CSI2_ENABLED_BIT,                                      /* Controls the
                                                            * reset of the
                                                            * complex IO--
                                                            * 0:Reset active; 
                                                            * 1:reset
                                                            * de-asserted */
    CSI2_ON,                                               /* 0:CSI2_OFF,
                                                            * 1:CSI2_ON,
                                                            * 2:CSI2_ULP */
    CSI2_AUTO,                                             /* Automatic
                                                            * switch between
                                                            * CSI2_ULP and
                                                            * CSI2_ON
                                                            * states--0:disable; 
                                                            * 1:enable */

    CSI2_PLUS_MINUS,                                       /* data lane and
                                                            * clock lane
                                                            * polarity 0:+/-
                                                            * pin order;
                                                            * 1:-/+ pin order 
                                                            */
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,

    CSI2_POSITION_4,                                       /* data lane and
                                                            * clock lane
                                                            * positions:
                                                            * 1,2,3,4,5 */
    CSI2_POSITION_3,
    CSI2_POSITION_2,
    CSI2_POSITION_1,
    CSI2_POSITION_5
};

csi2_phy_cfg_t csi2_physical_default_params = {
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
    0x00000009,
    0x0000000A,
    0x0000000B,
    0x0000000C,
    0x0000000D,
    0x0000000E,
    0x0000000F
};

csi2_ctrl_cfg_t csi2_ctrl_default_params = {
    CSI2_FIFO_8_BY_8,                                      // mflag_level
                                                           // level_h;
    CSI2_FIFO_8_BY_8,                                      // mflag_level
                                                           // level_l;
    CSI2_SIZE_1x64,                                        // burst_size
    CSI2_OCPCLK,                                           // video_clock_mode;
    CSI2_DISABLE_STREAMING,                                // streaming_mode;
    CSI2_DISABLED_BIT,                                     // non_posted_write;
    CSI2_ENABLED_BIT,                                      // vp_only_enable;
    CSI2_DISABLED_BIT,                                     // dbg_enable;
    CSI2_NATIVE_MIPI,                                      // endianness
                                                           // little_big_endian;

    CSI2_DISABLED_BIT,                                     // ecc_enable;
    CSI2_DISABLED_BIT                                      // secure_mode;
};

csi2_ctrl_cfg_t csi2_ctrl_default_params_1 = {

    CSI2_FIFO_8_BY_8,                                      // mflag_level
                                                           // level_h;
    CSI2_FIFO_8_BY_8,                                      // mflag_level
                                                           // level_l;
    CSI2_SIZE_16x64,                                       // burst_size
    CSI2_DISABLE_VP_CLK,                                   // video_clock_mode;
    CSI2_DISABLE_STREAMING,                                // streaming_mode;
    CSI2_DISABLED_BIT,                                     // non_posted_write;
    CSI2_DISABLED_BIT,                                     // vp_only_enable;
    CSI2_DISABLED_BIT,                                     // dbg_enable;
    CSI2_NATIVE_MIPI,                                      // endianness
                                                           // little_big_endian;

    CSI2_DISABLED_BIT,                                     // ecc_enable;
    CSI2_DISABLED_BIT                                      // secure_mode;
};

csi2_timing_cfg_t csi2_timing_default_params = {
    CSI2_ENABLED_BIT,                                      // force_rx_mode;
    CSI2_COUNTER_x1,                                       // multiplier ; 
    0x0000u,                                               // stop_state_counter;

    CSI2_DISABLED_BIT,                                     // force_rx_mode;
    CSI2_COUNTER_x1,                                       // multiplier ; 
    0x0000u                                                // stop_state_counter;
};

/* ================================================================ */
/* 
 *  Description:- Configures CSI2 for the path sensor->CSI2->IPIPEIF
 *  
 *
 *  @param   csi2_cfg_t*
 
 *  @return         CSI2_RETURN
 *================================================================== */

/* ===================================================================
 *  @func     csi2_params_init                                               
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
CSI2_RETURN csi2_params_init(csi2_cfg_t * cfg)
{

    cfg->master_standby_mode = CSI2_SMART_STANDBY;
    cfg->auto_idle_ocp_clock = CSI2_AUTO_IDLE;

    cfg->context_set_flag = 0x01u;                         /* sets only
                                                            * context-0 */

    // for(i=0;i<8;i++)
    // {
    cfg->ctx_cfg_ptr_array[0] = &csi2_ctx_default_params;
    // cfg->ctx_cfg_ptr_array[i] = &csi2_ctx_default_params;
    // }
    cfg->control = &csi2_ctrl_default_params;

    cfg->complexio_cfg_1 = &csi2_complexio_cfg_default_params;
    cfg->complexio_cfg_2 = &csi2_complexio_cfg_default_params;

    cfg->complexio_irq_enable_1 = 0x00000000u;             /* IRQ enable bit
                                                            * mask for
                                                            * complexio
                                                            * interfaces */
    cfg->complexio_irq_enable_2 = 0x00000000u;

    cfg->timing_io = &csi2_timing_default_params;

    cfg->physical = &csi2_physical_default_params;

    return CSI2_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description:- Configures CSI2 for the path sensor->CSI2->RAM. VP clk is disabled in this case
 *  
 *
 *  @param   csi2_cfg_t*
 
 *  @return         CSI2_RETURN
 *================================================================== */

CSI2_RETURN csi2_params_init_1(csi2_cfg_t * cfg)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    cfg->master_standby_mode = CSI2_SMART_STANDBY;
    cfg->auto_idle_ocp_clock = CSI2_AUTO_IDLE;

    cfg->context_set_flag = 0x01u;

    // for(i=0;i<8;i++)
    // {
    cfg->ctx_cfg_ptr_array[0] = &csi2_ctx_default_params_1;
    // cfg->ctx_cfg_ptr_array[i] = &csi2_ctx_default_params;
    // }
    cfg->control = &csi2_ctrl_default_params_1;

    cfg->complexio_cfg_1 = &csi2_complexio_cfg_default_params;
    cfg->complexio_cfg_2 = &csi2_complexio_cfg_default_params;

    cfg->complexio_irq_enable_1 = 0x00000000u;
    cfg->complexio_irq_enable_2 = 0x00000000u;

    cfg->timing_io = &csi2_timing_default_params;

    cfg->physical = &csi2_physical_default_params;

    retval =
        csi2_config_context_buffer(CSI2_DEVICE_A, 0, CSI2_PING,
                                   (uint32) (img_buff), 0);
    retval =
        csi2_config_context_buffer(CSI2_DEVICE_A, 0, CSI2_PONG,
                                   (uint32) (img_buff), 0);

    return retval;
}

/**************************CSI2 test configuration structures finished***********************/

/**************************CBUF test configuration structures starts***********************/

CBUFF_CTX_CTRL ctx_context_read_cfg, ctx_context_write_cfg;

const cbuff_hl_hwinfo cbuff_hl_hwinfo_default_params = {
    EIGHT_CONTEXTS,
    FRAG_ENABLE
};

const CBUFF_CTX_CTRL cbuff_ctx_ctrl_write_default_params = {
    CBUFF_WRITE_MODE,
    // CBUFF_EN,
    TWO_WINDOWS,
    0,                                                     // *tiler*/
    0,                                                     // bcf
    NULL,                                                  // FRAG_ADDR
    0x2000000,                                             // Virt addr Start
    0x2FFFFFF,                                             // Virt addr End
    0X1000,                                                // Window size
    0X1000,                                                // Threshold Full
    0,                                                     // Threshold S
    0X4000000                                              // Physical Addr
};

const CBUFF_CTX_CTRL cbuff_ctx_ctrl_read_default_params = {
    CBUFF_READ_MODE,
    // CBUFF_EN,
    TWO_WINDOWS,
    0,                                                     // *tiler*/
    0,                                                     // bcf
    NULL,                                                  // FRAG_ADDR
    0X3000000,                                             // Virt addr Start
    0X3FFFFFF,                                             // Virt addr End
    0X1000,                                                // Window size
    0,                                                     // Threshold Full
    0,                                                     // Threshold S
    0X2000000                                              // Physical Addr
};

/* ===================================================================
 *  @func     cbuff_ctx_read_param_init                                               
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
CBUFF_RETURN cbuff_ctx_read_param_init(CBUFF_CTX_CTRL * cfg)
{

    memcpy((void *) (cfg), (const void *) (&cbuff_ctx_ctrl_read_default_params),
           sizeof(CBUFF_CTX_CTRL));

    return CBUFF_SUCCESS;
}

/* ===================================================================
 *  @func     cbuff_ctx_write_param_init                                               
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
CBUFF_RETURN cbuff_ctx_write_param_init(CBUFF_CTX_CTRL * cfg)
{

    memcpy((void *) (cfg),
           (const void *) (&cbuff_ctx_ctrl_write_default_params),
           sizeof(CBUFF_CTX_CTRL));

    return CBUFF_SUCCESS;
}

/**************************CBUF test configuration structures finishes***********************/

/*******************************************************************
 *   routine to  start the sensor Test bench and 
//simulate the paraallel port data.
 ********************************************************************/
test_start_sensor()
{

    // bit to select from memoy or from CCP2, this is not there in actual
    // spec This will start the sensor Test bench and 
    // simulate the paraallel port data.
    register_write(0x50002400, 0x1);

}

test_stop_sensor()
{
    // will stop the sensor

    register_write(0x50002400, 0x0);
}

ISP_RETURN isptest1();

ISP_RETURN isptest2();

ISP_RETURN isptest3();

ISP_RETURN isptest4();

ISP_RETURN isptest5();

ISP_RETURN isptest6();

ISP_RETURN isptest7();

ISP_RETURN isptest8();

ISP_RETURN isptest9();

ISP_RETURN isptest10();

ISP_RETURN isptest11();

ISP_RETURN isptest12();

ISP_RETURN isptest13();

ISP_RETURN isptest14();

ISP_RETURN isptest15();

ISP_RETURN isptest16();

ISP_RETURN isptest17();

ISP_RETURN isptest18();

ISP_RETURN isptest19();

ISP_RETURN isptest20();

ISP_RETURN isptest21();

ISP_RETURN isptest22();

/* ================================================================ */
/* 
 *  Description:- Configures ISIF to read from ipipeif and put data in RAM at the address "img_buff"
 *  contained in the structure field "sdram_op_params", the flags in "feature_flag" enable different features in isif.
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *================================================================== */

uint16 test_number = 11;

uint8 isp_test_reset = 1;

/* ===================================================================
 *  @func     isp_test                                               
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
Void isp_test(int argc, char *argv[])
{
    uint32 test_continue = 3;

    ISP_RETURN retval = ISP_FAILURE;

    /* these are ISS top level clk enable registers, right now there is no
     * Frame work to do the same so hardcoding the values here */
    // register_write(0x50000084,0xFFFF);
    // register_write(ISS_REGS_BASE_ADDR + 0x84, 0xFFFF);

    iss_core_init();
    retval = isp_common_init();

    isp_common_enable_clk(ISP_BL_CLK);

    img_buff =
        TIMM_OSAL_Malloc((img_height * img_width * 2), TIMM_OSAL_TRUE, 64,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    if (img_buff == NULL)
    {
        ISP_PRINT("\n MEMORY NOT ENOUGH");
        System_flush();
    }

    op_img_buff =
        TIMM_OSAL_Malloc((img_height * img_width * 2), TIMM_OSAL_TRUE, 64,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    if (img_buff == NULL)
    {
        ISP_PRINT("\n MEMORY NOT ENOUGH");
        System_flush();
    }
    op_img_buff_1 =
        TIMM_OSAL_Malloc((img_height * img_width * 2 * 4), TIMM_OSAL_TRUE, 64,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    if (img_buff == NULL)
    {
        ISP_PRINT("\n MEMORY NOT ENOUGH");
        System_flush();
    }

    default_boxcar_op =
        TIMM_OSAL_Malloc((img_height * img_width), TIMM_OSAL_TRUE, 64,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    if (default_boxcar_op == NULL)
    {
        ISP_PRINT("\n MEMORY NOT ENOUGH for boxcar ");
        System_flush();
    }
    do
    {

        // ISP_PRINT("\n\n ENTER the TEST NUMBER \n");
        // System_flush();
#ifndef SCANF_WORKAROUND
        // scanf("%d", &test_number);
#endif

        ISP_PRINT("\nYou have chosen test number %d\n", test_number);

        switch (test_number)
        {

            case 1:

                retval = isptest1();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test1 failed");
                else
                    ISP_PRINT("\n test 1 passed ");

                break;

            case 2:

                retval = isptest2();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test2 failed");
                else
                    ISP_PRINT("\n test 2 passed ");

                break;
            case 3:

                retval = isptest3();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test3 failed");
                else
                    ISP_PRINT("\n test 3 passed ");

                break;
            case 4:

                retval = isptest4();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test4 failed");
                else
                    ISP_PRINT("\n test 4  passed ");

                break;
            case 5:

                retval = isptest5();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test5 failed");
                else
                    ISP_PRINT("\n test 5 passed ");

                break;
            case 6:

                retval = isptest6();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test6 failed");
                else
                    ISP_PRINT("\n test 6 passed ");

                break;
            case 7:

                retval = isptest7();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test7 failed");
                else
                    ISP_PRINT("\n test 7 passed ");

                break;
            case 8:

                retval = isptest8();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test8 failed");
                else
                    ISP_PRINT("\n test 8 passed ");

                break;

            case 9:

                retval = isptest9();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test 9  failed");
                else
                    ISP_PRINT("\n test 9 passed ");

                break;

            case 10:

                retval = isptest10();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test10  failed");
                else
                    ISP_PRINT("\n test 10 passed ");

                break;
            case 11:
                retval = isptest11();

                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test11 failed");
                else
                    ISP_PRINT("\n test 11 passed ");

                break;
            case 12:
                retval = isptest12();

                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test12 failed");
                else
                    ISP_PRINT("\n test 12 passed ");

                break;
            case 13:
                retval = isptest13();

                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test13 failed");
                else
                    ISP_PRINT("\n test 13 passed ");

                break;
            case 14:
                retval = isptest14();

                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test14 failed");
                else
                    ISP_PRINT("\n test 14 passed ");

                break;
            case 15:
                retval = isptest15();

                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test15 failed");
                else
                    ISP_PRINT("\n test 15 passed ");

                break;
            case 16:
                retval = isptest16();

                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test16 failed");
                else
                    ISP_PRINT("\n test 16 passed ");
                break;
            case 17:
                retval = isptest17();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test17 failed");
                else
                    ISP_PRINT("\n test 17 passed ");

                break;

            case 18:
                retval = isptest18();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test17 failed");
                else
                    ISP_PRINT("\n test 17 passed ");

                break;

            case 19:
                retval = isptest19();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test17 failed");
                else
                    ISP_PRINT("\n test 17 passed ");

                break;

            case 20:
                retval = isptest20();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test20 failed");
                else
                    ISP_PRINT("\n test 20 passed ");

                break;

            case 21:
                retval = isptest21();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test21 failed");
                else
                    ISP_PRINT("\n test 21 passed ");

                break;

            case 22:
                retval = isptest22();
                if (retval != ISP_SUCCESS)
                    ISP_PRINT("\n test21 failed");
                else
                    ISP_PRINT("\n test 21 passed ");

                break;
            default:
                ISP_PRINT("\n invalid input");
                break;

        }
        System_flush();
        System_printf
            ("\n To continue halt the simulator and change the variable isp_test_reset =1 \n");
        System_flush();
        while (isp_test_reset != 1)
        {
            ;
        }

        test_continue = 0;
        System_printf("\r\n Do you want to run another ISP test \n");

        System_printf("\r 1: yes 2: No \n");
        System_flush();
        scanf("%d", &test_continue);
        if ((test_continue < 1) || (test_continue > 2))
        {
            ISP_PRINT("\n invalid input");
            break;
        }

    } while (test_continue == 1);

    TIMM_OSAL_Free(img_buff);
    TIMM_OSAL_Free(op_img_buff);
    TIMM_OSAL_Free(op_img_buff_1);

    // while(1);
    // System_exit(0);
}

uint32 isp_cb_test_flag_1 = 0;

uint32 isp_cb_test_flag_2 = 0;

void test_callback_1(ISP_RETURN status, uint32 arg1, void *arg2)
{
    // ISP_PRINT("\n\nRESIZER CALLBACK CALLED\n\n %d ", isp_test_flag);

    isp_cb_test_flag_1++;

}

void test_callback_2(ISP_RETURN status, uint32 arg1, void *arg2)
{
    // ISP_PRINT("\n\nRESIZER CALLBACK CALLED\n\n %d ", isp_test_flag);

    isp_cb_test_flag_2++;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the dummy Parallel port (from address 0x30000000)and puts it in RAM at the 
 *   address img_buff as RAW data, from there  ipipe-if takes the RAW data and dumps and feeds it to ipipe and then to resizer which scales it down by a factor of 2 and write yuv422 data 
 
 to op_img_buffer address
 *
 *  @param   
 
 *  @return         ISP_RETURN
 
 * NOTE :-   Since timing constraints are not applicable to simulator, and also the simulator processes data in frame  mode rather than line buffer mode
 we enable the sensor before we enable the ipipe_if_module
 
 *================================================================== */

ISP_RETURN isptest1()
{
    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *h1, *h2;

    iss_isp_input_config(ISS_ISP_INPUT_CSI2_A);

    retval = csi2_init();
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_init failed");

    retval = csi2_open();
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_open failed");

    retval = csi2_params_init(&test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_params_init failed");

    retval = csi2_config(CSI2_DEVICE_A, &test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\ncsi2_config  failed");

    h1 = (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                 TIMM_OSAL_TRUE, 64,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(h1, NULL);
    h1->arg1 = 0;
    h1->arg2 = NULL;
    h1->callback = test_callback_1;
    h1->context = ISS_CALLBACK_CONTEXT_HWI;
    h1->priority = 1;

    retval =
        csi2_context_irq_hook_int_handler(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                          CSI2_FS_IRQ, h1);
    csi2_context_irq_enable_interrupt(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                      CSI2_FS_IRQ);
    h2 = (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                 TIMM_OSAL_TRUE, 64,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(h2, NULL);
    h2->arg1 = 0;
    h2->arg2 = NULL;
    h2->callback = test_callback_2;
    h2->context = ISS_CALLBACK_CONTEXT_SWI;
    h2->priority = 1;
    retval =
        csi2_context_irq_hook_int_handler(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                          CSI2_FE_IRQ, h2);
    csi2_context_irq_enable_interrupt(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                      CSI2_FE_IRQ);

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_1(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_1 failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_config failed");

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_openfailed");

    retval = ipipeif_params_init_1(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_1 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_init failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ....ipipe_config failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = csi2_enable_context(CSI2_DEVICE_A, 0);
    retval = csi2_start(CSI2_DEVICE_A);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_start failed");

    test_start_sensor();

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");
    {
        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/
    retval = csi2_disable(CSI2_DEVICE_A);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_disable failed");

    retval = csi2_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_close failed");

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the  array img_buff  and passes it from isif to ipipe thru ipipe-if 
 and then to resizer which scales it down by a factor of 2 along with horizontal flip and write yuv422 data to op_img_buffer address
 *                   only RESIZER A is used
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest2()
{
    ISP_RETURN retval = ISP_SUCCESS;

    /* Interrupt_Handle_T* handle1=(Interrupt_Handle_T*) NULL;
     * Interrupt_Handle_T* handle2=(Interrupt_Handle_T*) NULL; */

    // int i;
    // FILE *ifp,*ofh1,*ofh2;

    // char temp[100];

    /* ISP_PRINT("\n Enter the absolute path of the input, %d * %d RAW
     * file",img_width,img_height); System_flush(); scanf("%s",temp);
     * 
     * ifp=fopen (temp,"r"); if(ifp==NULL) ISP_PRINT("\n FILE not
     * Present");System_flush();
     * 
     * 
     * for(i=0;i<(img_height*img_width*2);i++) fread((img_buff+i),
     * sizeof(uint8),1, ifp);
     * 
     * fclose(ifp);
     * 
     * 
     * ISP_PRINT("\n %d bytes read",i); System_flush(); */

    // isp_register_interrupt_handler(ISP_RSZ_INT_LAST_PIX, test_callback,1,
    // NULL,ISP_CALLBACK_CONTEXT_HWI);
    /* 
     * handle1=(Interrupt_Handle_T*)TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T), 
     * TIMM_OSAL_TRUE, 64, TIMMOSAL_MEM_SEGMENT_EXT);
     * ISP_ASSERT(handle1,NULL); handle1->arg1=1; handle1->arg2=NULL;
     * handle1->callback=test_callback_1;
     * handle1->context=ISS_CALLBACK_CONTEXT_HWI; handle1->priority=3;
     * 
     * 
     * 
     * 
     * handle2=(Interrupt_Handle_T*)TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T), 
     * TIMM_OSAL_TRUE, 64, TIMMOSAL_MEM_SEGMENT_EXT);
     * ISP_ASSERT(handle2,NULL); handle2->arg1=2; handle2->arg2=NULL;
     * handle2->callback=test_callback_2;
     * handle2->context=ISS_CALLBACK_CONTEXT_SWI; handle2->priority=2;
     * 
     * 
     * 
     * 
     * isp_hook_int_handler(ISP_IPIPE_INT_LAST_PIX , handle2);
     * isp_hook_int_handler(ISP_RSZ_INT_LAST_PIX , handle1);
     * 
     * 
     * isp_enable_interrupt(ISP_IPIPE_INT_LAST_PIX);
     * isp_enable_interrupt(ISP_RSZ_INT_LAST_PIX); */
    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

#if ZEBU
    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_2(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");
    register_write(0x5001147C, 0xdc000001);
#endif

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");
#if ZEBU
    /* This is 1 only when ipipe_if i/p is from memory */
    test_ipipe_cfg.src_cfg->dims.vps = 1;
#else
    test_ipipe_cfg.src_cfg->dims.vps = 0;
#endif

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            val = isp_cb_test_flag_2;                      // isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX,&val);

        }

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    /* 
     * isp_disable_interrupt(ISP_RSZ_INT_LAST_PIX);
     * 
     * isp_unhook_int_handler(ISP_RSZ_INT_LAST_PIX , handle2);
     * isp_unhook_int_handler(ISP_RSZ_INT_LAST_PIX , handle1);
     * 
     * 
     * 
     * TIMM_OSAL_Free(handle2); TIMM_OSAL_Free(handle1); */

    // ISP_PRINT("\n Enter the absolute path of the output, %d * %d yuv
    // file",img_width/2,img_height/2);
    // System_flush();
    // scanf("%s",temp);

    // ofh1=fopen(temp,"w");
    // if(ofh1==NULL) {ISP_PRINT("\nOUTPUT FILE CANNOT BE
    // CREATED");System_flush();return ISP_FAILURE;}

    // fwrite(op_img_buff,1, ((img_height/2)*(img_width/2)*2), ofh1);

    // ISP_PRINT("\n Enter the absolute path of the output, %d * %d yuv
    // file",img_width*2,img_height*2);
    // System_flush();
    // scanf("%s",temp);

    // ofh2=fopen(temp,"wb+");
    // if(ofh2==NULL) {ISP_PRINT("\nOUTPUT FILE CANNOT BE
    // CREATED");System_flush();return ISP_FAILURE;}

    // fwrite(op_img_buff_1,4, ((img_height*2)*(img_width*2)*2)/4, ofh1);

    // fclose(ofh1);
    // fclose(ofh2);

    return retval;
}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the array img_buff and passes it from isif to ipipe thru ipipe-if 
 and then to resizer which is operated in passthru mode, yuv422 is i/p to resizer and o/p is yuv422 at op_img_buff
 *
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest3()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);
    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_3(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_3 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the array img_buff and passes it isif to ipipe thru ipipe-if 
 and then to resizer which is operated in bypass mode, yuv422 is i/p to resizer and o/p is yuv422 at op_img_buff
 *                        
 *  @param   none
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest4()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_4(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_4 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    /***********************Wait for status bit of RSZ interrupt to know that the operation is complete********/

    {

        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the dummy Parallel port (from address 0x30000000)and puts it in RAM at the 
 *   address img_buff as RAW data, from there  ipipe-if takes the RAW data and dumps and feeds it to ipipe and then to resizer which scales it down by a factor of 2 and write yuv422 data 
 
 to op_img_buffer address the pixel format is yuv422, the resizerB upscales the image by a factor of 2 and puts it at op_img_buff_1 the pix format is rgb565
 *
 *  @param   
 
 *  @return         ISP_RETURN
 
 * NOTE :-   Since timing constraints are not applicable to simulator, and also the simulator processes data in frame  mode rather than line buffer mode
 we enable the sensor before we enable the ipipe_if_module
 
 *================================================================== */

ISP_RETURN isptest5()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_1 failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_5(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_config failed");

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_openfailed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_1 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_init failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

#if ZEBU
    /* This is 1 only when ipipe_if i/p is from memory */
    test_ipipe_cfg.src_cfg->dims.vps = 1;
#else
    test_ipipe_cfg.src_cfg->dims.vps = 0;
#endif

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ....ipipe_config failed");

    /* Start ISIF first so that it processes the data and puts it in memory,
     * then start ipipe and ipipieif to process the dpcm compressed datat */

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {
        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

    }
    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- YUV422 data is fed thru ipipe_if to resizer and the image is downscaled by a factor of 2 , and converted to yuv420 , 
 *  and resizer B scales up the image and op's yuv420 data .
 *
 *  @param   
 *
 *  @return         ISP_RETURN
 
 * NOTE :-   Since timing constraints are not applicable to simulator, and also the simulator processes data in frame  mode rather than line buffer mode
 we enable the sensor before we enable the ipipe_if_module
 
 *================================================================== */

ISP_RETURN isptest6()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_6(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_openfailed");

    retval = ipipeif_params_init_5(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_1 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    /* Start ISIF first so that it processes the data and puts it in memory,
     * then start ipipe and ipipieif to process the dpcm compressed datat */

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {
        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

    }
    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description: RAW data is taken from SDRAM and fed to ipipe_if to ipipe to resizer and writeen out via resizer as RAW data, resizer in bypass mode. Ipipe doesn’t do a RAW to yuv422 conversion
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest7()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_7(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_7 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_openfailed");

    retval = ipipeif_params_init_5(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_5 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_init failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_2(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_2 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ....ipipe_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_submodule_start failed");
    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {
        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

    }
    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:RAW data from memory is fed to isif thru Ipipeif and then to ipipe to rsz, resizer is configured for horizontal flip and data is
 downscaled by 2 and o/p data fmt is YUV420
 
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest8()
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_7(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:"yuv420 is fed in two passes to ipipe_if to resier to rescale it up by 2, and rescale down by 2"
 
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest9()
{

    return ISP_FAILURE;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the  PP through CSI2 and passes it  from isif to ipipe thru ipipe-if 
 and then to resizer which scales it down by a factor of 2 along with horizontal flip and write yuv422 data to op_img_buffer address
 * only RESIZER A is used
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest10()
{
    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *h1, *h2;

    iss_isp_input_config(ISS_ISP_INPUT_CSI2_A);

    retval = csi2_init();
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_init failed");

    retval = csi2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_open failed");

    retval = csi2_params_init(&test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_params_init failed");

    retval = csi2_config(CSI2_DEVICE_A, &test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\ncsi2_config  failed");

    h1 = (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                 TIMM_OSAL_TRUE, 64,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(h1, NULL);
    h1->arg1 = 0;
    h1->arg2 = NULL;
    h1->callback = test_callback_1;
    h1->context = ISS_CALLBACK_CONTEXT_HWI;
    h1->priority = 1;

    retval =
        csi2_context_irq_hook_int_handler(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                          CSI2_FS_IRQ, h1);
    csi2_context_irq_enable_interrupt(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                      CSI2_FS_IRQ);
    h2 = (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                 TIMM_OSAL_TRUE, 64,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(h2, NULL);
    h2->arg1 = 0;
    h2->arg2 = NULL;
    h2->callback = test_callback_2;
    h2->context = ISS_CALLBACK_CONTEXT_HWI;
    h2->priority = 1;
    retval =
        csi2_context_irq_hook_int_handler(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                          CSI2_FE_IRQ, h2);
    csi2_context_irq_enable_interrupt(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                      CSI2_FE_IRQ);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_3(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_3 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    retval = csi2_enable_context(CSI2_DEVICE_A, 0);
    retval = csi2_start(CSI2_DEVICE_A);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_start failed");
    isp_cb_test_flag_2 = 0x00;
    test_start_sensor();

    {

        uint32 val = 0;

        while (val == 0)
        {

            val = isp_cb_test_flag_2;                      // isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX,&val);

        }

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = csi2_disable(CSI2_DEVICE_A);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_disable failed");

    retval = csi2_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_close failed");

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    return retval;

}

/* ==========================test11====================================== */
/**
 *  Description:-
RAW data from SDRAM (from img_buff ) is taken by CCP2 and given to IPIPEIF. IPIPEIF feeds this data to
ISIF and then to IPIPE to RSZ. RSZ scales it down by factor of 2 and writes YUV422 data 
through resizerA , and resizer B outouts YUV422 data scaled up by factor of 2

 *                        
 *  @param   none

 *  @return         ISP_RETURN
 */
 /*================================================================== */

ISP_RETURN isptest11()
{
    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *handle1 = (Interrupt_Handle_T *) NULL;

    Interrupt_Handle_T *handle2 = (Interrupt_Handle_T *) NULL;

    // isp_register_interrupt_handler(ISP_RSZ_INT_LAST_PIX, test_callback,1,
    // NULL,ISP_CALLBACK_CONTEXT_HWI);

    handle1 =
        (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                TIMM_OSAL_TRUE, 64,
                                                TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(handle1, NULL);
    handle1->arg1 = 1;
    handle1->arg2 = NULL;
    handle1->callback = test_callback_1;
    handle1->context = ISS_CALLBACK_CONTEXT_HWI;
    handle1->priority = 3;

    handle2 =
        (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                TIMM_OSAL_TRUE, 64,
                                                TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(handle2, NULL);
    handle2->arg1 = 2;
    handle2->arg2 = NULL;
    handle2->callback = test_callback_2;
    handle2->context = ISS_CALLBACK_CONTEXT_SWI;
    handle2->priority = 2;

    isp_hook_int_handler(ISP_IPIPE_INT_LAST_PIX, handle2);
    isp_hook_int_handler(ISP_RSZ_INT_LAST_PIX, handle1);

    isp_enable_interrupt(ISP_IPIPE_INT_LAST_PIX);
    isp_enable_interrupt(ISP_RSZ_INT_LAST_PIX);

    isp_common_enable_clk(ISP_BL_CLK);
    iss_isp_input_config(ISS_ISP_INPUT_CCCP2);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = ccp2_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = ccp2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = ccp2_memory_test_params_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory test params init  failed");

    retval = ccp2_memory_config();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory config  failed");

    Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, OCPERROR_IRQ);
    Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, EOF_IRQ);

    retval = h3a_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_4(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

#if ZEBU

    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");
    register_write(0x5001147C, 0xdc000001);
#endif

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_4(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    retval = ccp2_start(MEMORY_READ);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            val = isp_cb_test_flag_2;                      // isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX,&val);

        }

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = ccp2_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_close failed");

    return retval;

}

/* ==========================test11====================================== */
/* 
 *  Description:-RAW data from dummy Parallel Port (from address img_buff) is taken by CSI2 and 
 stored in RAM. This RAW data from RAM is taken by CCP2 and given to IPIPEIF. IPIPEIF feeds this data to
 ISIF and then to IPIPE to RSZ. RSZ scales it down by factor of 2 and writes YUV422 data through resizerA ,
 and resizer B outouts YUV422 data scaled up by factor of 2
 
 *                        
 *  @param   none
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest12()
{
    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *h1, *h2;

    iss_isp_input_config(ISS_ISP_INPUT_CCCP2);
    retval = ccp2_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = ccp2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = ccp2_test_path_param_init(&path_enable_t_test);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_test_path_param_init failed");

    retval = ccp2_config(&path_enable_t_test);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_path_enable_init failed");

    retval = ccp2_test_lcm_enable_init(&lcm_enable_t_test);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_memory_enable_init failed");

    /* CSI2 configuration */

    retval = csi2_init();
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_init failed");

    retval = csi2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_open failed");

    retval = csi2_params_init_1(&test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_params_init failed");

    retval = csi2_config(CSI2_DEVICE_A, &test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\ncsi2_config  failed");

    h1 = (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                 TIMM_OSAL_TRUE, 64,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(h1, NULL);
    h1->arg1 = 0;
    h1->arg2 = NULL;
    h1->callback = test_callback_1;
    h1->context = ISS_CALLBACK_CONTEXT_HWI;
    h1->priority = 1;

    retval =
        csi2_context_irq_hook_int_handler(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                          CSI2_FS_IRQ, h1);
    csi2_context_irq_enable_interrupt(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                      CSI2_FS_IRQ);
    h2 = (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                 TIMM_OSAL_TRUE, 64,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(h2, NULL);
    h2->arg1 = 0;
    h2->arg2 = NULL;
    h2->callback = test_callback_2;
    h2->context = ISS_CALLBACK_CONTEXT_SWI;
    h2->priority = 1;
    retval =
        csi2_context_irq_hook_int_handler(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                          CSI2_FE_IRQ, h2);
    csi2_context_irq_enable_interrupt(CSI2_DEVICE_A, CSI2_CONTEXT_0,
                                      CSI2_FE_IRQ);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_4(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    retval = CCP2_Read_from_Memory_Config(&lcm_enable_t_test, MEMORY_READ);

    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_memory_enable_init failed");

    retval = csi2_enable_context(CSI2_DEVICE_A, 0);
    retval = csi2_start(CSI2_DEVICE_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_enable failed");

    test_start_sensor();

    retval = ccp2_start(MEMORY_READ);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = ccp2_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the  array img_buff  and passes it from isif to ipipe thru ipipe-if 
 and then to resizer which scales it down by a factor of 2 along with horizontal flip and write yuv422 data to op_img_buffer address
 *                   only RESIZER A is used. In H3A  auto focus has been disabled. (only Difference from test12) 
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest13()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);
    retval = h3a_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_2(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

    test_h3a_cfg.h3a_common_cfg->reserved = 0xdc000001;
    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");
    // register_write(0x5001147C,0xdc000001);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = h3a_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the  array img_buff  and passes it from isif to ipipe thru ipipe-if 
 and then to resizer which scales it down by a factor of 2 along with horizontal flip and write yuv422 data to op_img_buffer address
 *                   only RESIZER A is used.In H3A  auto white balance has been disabled. (only Difference from test12) 
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest14()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = h3a_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_3(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

    test_h3a_cfg.h3a_common_cfg->reserved = 0xdc000001;
    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");
    // register_write(0x5001147C,0xdc000001);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = h3a_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_close failed");

    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the  array img_buff  and passes it from isif to ipipe thru ipipe-if 
 and then to resizer which scales it down by a factor of 2 along with horizontal flip and write yuv422 data to op_img_buffer address
 *                   only RESIZER A is used.In H3A  auto focus, AUTO FOCUS VERTICAL  & auto white balance has been ENabled. (only Difference from test12) 
 
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

// To be done.  Update fir filter parsm properly to enable auto focus
// vertical . Other wise it is crashing.

ISP_RETURN isptest15()
{
    ISP_RETURN retval = ISP_SUCCESS;

    iss_isp_input_config(ISS_ISP_INPUT_CCCP2);
    retval = h3a_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_4(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

    test_h3a_cfg.h3a_common_cfg->reserved = 0xdc000001;
    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");

    // register_write(0x5001147C,0xdc000001);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = h3a_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_close failed");
    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This test takes RAW data from the  array img_buff  and passes it from isif to ipipe thru ipipe-if 
 and then to resizer which scales it down by a factor of 2 along with horizontal flip and write yuv422 data to op_img_buffer address
 *                   only RESIZER A is used
 *  @param   
 
 *  @return         ISP_RETURN
 *================================================================== */

ISP_RETURN isptest16()
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = h3a_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_1(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

    test_h3a_cfg.h3a_common_cfg->reserved = 0xdc000001;
    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");
    // register_write(0x5001147C,0xdc000001);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = h3a_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_close failed");

    return retval;

}

/* ==========================test17====================================== */
/**
 *  Description:-

RAW data from SDRAM (from img_buff ) is taken by CCP2 and given to IPIPEIF. IPIPEIF feeds this data to
ISIF and then to IPIPE to RSZ. RSZ scales it down by factor of 2 and writes YUV422 data 
through resizerA , and resizer B outouts YUV422 data scaled up by factor of 2
CBUF is configured in this test case

 *                        
 *  @param   none

 *  @return         ISP_RETURN
 */
 /*================================================================== */

CBUFF_IRQ_Handle_T isp5_test_ctx0_irq_handle;

CBUFF_IRQ_Handle_T isp5_test_ctx1_irq_handle;

/* ===================================================================
 *  @func     isp5_test_ctx0_interrupt                                               
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
void isp5_test_ctx0_interrupt(uint32 ctx, uint32 int_id)
{
    ctx = ctx;
    int_id = int_id;
    ISP_PRINT("\nCBUFF Interrupt: Context = %2d Interrupt ID = %2d", ctx,
              int_id);

}

/* ===================================================================
 *  @func     isp5_test_ctx1_interrupt                                               
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
void isp5_test_ctx1_interrupt(uint32 ctx, uint32 int_id)
{
    ctx = ctx;
    int_id = int_id;
    ISP_PRINT("\nCBUFF Interrupt: Context = %2d Interrupt ID = %2d", ctx,
              int_id);
}

ISP_RETURN isptest17()
{
    ISP_RETURN retval = ISP_SUCCESS;

    CBUFF_RETURN retval1 = CBUFF_SUCCESS;

    iss_isp_input_config(ISS_ISP_INPUT_CCCP2);
    retval = ccp2_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = ccp2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = ccp2_memory_test_params_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory test params init  failed");

    retval = ccp2_memory_config();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory config  failed");

    retval1 = cbuff_Init();
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n cbuf init  failed");

    retval1 = cbuff_Open();
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n cbuf open  failed");

    /* We are using context 0 in write mode & context 1 for read mode */
    retval1 = cbuff_ctx_read_param_init(&ctx_context_read_cfg);
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n cbuf ctx read enable  failed");

    retval1 = cbuff_Ctx_Config(CBUFF_CTX_1, &ctx_context_read_cfg);
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n config cbuf ctx read  enable  failed");

    retval1 = cbuff_ctx_write_param_init(&ctx_context_write_cfg);
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n cbuf ctx write enable  failed");

    retval1 = cbuff_Ctx_Config(CBUFF_CTX_0, &ctx_context_write_cfg);
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n config cbuf ctx write  enable  failed");

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_4(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_1(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    retval = ccp2_start(MEMORY_READ);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 start failed");

    isp5_test_ctx0_irq_handle.list_next = NULL;
    isp5_test_ctx0_irq_handle.callback = isp5_test_ctx0_interrupt;
    isp5_test_ctx0_irq_handle.arg1 = CBUFF_CTX_0;
    isp5_test_ctx0_irq_handle.arg2 = IRQ_CTX_READY;
    isp5_test_ctx0_irq_handle.callback_context = CBUFF_CALLBACK_CONTEXT_SWI;
    isp5_test_ctx0_irq_handle.priority = 0;

    CBUFF_Ctx_Hook_Interrupt(CBUFF_CTX_0, IRQ_CTX_READY,
                             &isp5_test_ctx0_irq_handle);
    CBUFF_Ctx_Enable_Interrupt(CBUFF_CTX_0, IRQ_CTX_READY);
    cbuff_Ctx_Enable(CBUFF_CTX_0);

    isp5_test_ctx1_irq_handle.list_next = NULL;
    isp5_test_ctx1_irq_handle.callback = isp5_test_ctx1_interrupt;
    isp5_test_ctx1_irq_handle.arg1 = CBUFF_CTX_1;
    isp5_test_ctx1_irq_handle.arg2 = IRQ_CTX_READY;
    isp5_test_ctx1_irq_handle.callback_context = CBUFF_CALLBACK_CONTEXT_HWI;
    isp5_test_ctx1_irq_handle.priority = 16;

    CBUFF_Ctx_Hook_Interrupt(CBUFF_CTX_1, IRQ_CTX_READY,
                             &isp5_test_ctx1_irq_handle);
    CBUFF_Ctx_Enable_Interrupt(CBUFF_CTX_1, IRQ_CTX_READY);
    cbuff_Ctx_Enable(CBUFF_CTX_1);

    {

        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = ccp2_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_close failed");

    cbuff_Ctx_Disable(CBUFF_CTX_0);
    cbuff_Ctx_Disable(CBUFF_CTX_1);

    retval1 = cbuff_Close();
    if (retval1 != CBUFF_SUCCESS)
        System_printf("\n cbuf close failed");

    return retval;

}

/* ==========================test17====================================== */
/**
 *  Description:-

RAW data from memory is fed to isif thru Ipipeif and then to ipipe to rsz, resizer scales it down by a factor of 2 
and write yuv422 data thri resizerA , and resizer B puts out yuv422 data scaled up by factor of 2 , 

IPIPE is configured to give out both BSC and Histogram.

 *                        
 *  @param   none

 *  @return         ISP_RETURN
 */
 /*================================================================== */

ISP_RETURN isptest18()
{
    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *handle1 = (Interrupt_Handle_T *) NULL;

    Interrupt_Handle_T *handle2 = (Interrupt_Handle_T *) NULL;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = h3a_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_init failed");

    retval = h3a_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_open failed");

    retval = h3a_params_init_4(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n h3a_params_init_1 failed");

    retval = h3a_config(&test_h3a_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nh3a_config  failed");
    register_write(0x5001147C, 0xdc000001);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_3(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_6_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            val = isp_cb_test_flag_2;                      // isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX,&val);

        }

    }

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    isp_disable_interrupt(ISP_RSZ_INT_LAST_PIX);

    isp_unhook_int_handler(ISP_RSZ_INT_LAST_PIX, handle2);
    isp_unhook_int_handler(ISP_RSZ_INT_LAST_PIX, handle1);

    TIMM_OSAL_Free(handle2);
    TIMM_OSAL_Free(handle1);

    // ISP_PRINT("\n Enter the absolute path of the output, %d * %d yuv
    // file",img_width/2,img_height/2);
    // System_flush();
    // scanf("%s",temp);

    // ofh1=fopen(temp,"w");
    // if(ofh1==NULL) {ISP_PRINT("\nOUTPUT FILE CANNOT BE
    // CREATED");System_flush();return ISP_FAILURE;}

    // fwrite(op_img_buff,1, ((img_height/2)*(img_width/2)*2), ofh1);

    // ISP_PRINT("\n Enter the absolute path of the output, %d * %d yuv
    // file",img_width*2,img_height*2);
    // System_flush();
    // scanf("%s",temp);

    // ofh2=fopen(temp,"wb+");
    // if(ofh2==NULL) {ISP_PRINT("\nOUTPUT FILE CANNOT BE
    // CREATED");System_flush();return ISP_FAILURE;}

    // fwrite(op_img_buff_1,4, ((img_height*2)*(img_width*2)*2)/4, ofh1);

    // fclose(ofh1);
    // fclose(ofh2);

    return retval;
}

/* ==========================test17====================================== */
/**
 *  Description:-
RAW data from memory is fed to isif thru Ipipeif and then to ipipe to rsz, resizer scales it down by a factor of 2 
and write yuv422 data thri resizerA , and resizer B puts out yuv422 data scaled up by factor of 2 , 
IPIPE is configured to give out both BSC and Boxcar data.

 *                        
 *  @param   none

 *  @return         ISP_RETURN
 */
 /*================================================================== */
ISP_RETURN isptest19()
{
    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *handle1 = (Interrupt_Handle_T *) NULL;

    Interrupt_Handle_T *handle2 = (Interrupt_Handle_T *) NULL;

    isp_common_enable_clk(ISP_BL_CLK);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    handle1 =
        (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                TIMM_OSAL_TRUE, 64,
                                                TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(handle1, NULL);
    handle1->arg1 = 1;
    handle1->arg2 = NULL;
    handle1->callback = test_callback_1;
    handle1->context = ISS_CALLBACK_CONTEXT_HWI;
    handle1->priority = 3;

    handle2 =
        (Interrupt_Handle_T *) TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
                                                TIMM_OSAL_TRUE, 64,
                                                TIMMOSAL_MEM_SEGMENT_EXT);
    ISP_ASSERT(handle2, NULL);
    handle2->arg1 = 2;
    handle2->arg2 = NULL;
    handle2->callback = test_callback_2;
    handle2->context = ISS_CALLBACK_CONTEXT_HWI;
    handle2->priority = 2;

    isp_hook_int_handler(ISP_RSZ_INT_LAST_PIX, handle2);
    isp_hook_int_handler(ISP_RSZ_INT_LAST_PIX, handle1);

    isp_enable_interrupt(ISP_RSZ_INT_LAST_PIX);
    // isp_enable_interrupt(ISP_RSZ_INT_LAST_PIX);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_2(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = ipipe_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipe_init  failed");

    retval = ipipe_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_open failed");

    retval = ipipe_params_init_4(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_params_init_1 failed");

    retval = ipipe_config(&test_ipipe_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_config failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_2(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_2 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");

    retval = rsz_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_init failed");

    retval = rsz_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_open failed");

    retval = rsz_params_init_1(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_params_init_1 failed");

    retval = rsz_config(&test_rsz_config);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_config failed");

    retval = rsz_submodule_start(RSZ_SINGLE, RESIZER_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszA_submodule_start failed");

    // retval=rsz_submodule_start(RSZ_SINGLE,RESIZER_B);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rszB_submodule_start failed");

    retval = rsz_start(RSZ_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_start failed");

    retval = ipipe_start(IPIPE_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_start failed");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {

            val = isp_cb_test_flag_1;                      // isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX,&val);

        }

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    {
        uint32 addr = NULL;

        retval = ipipe_get_bsc_address(&addr, IPIPE_BSC_MEMORY_0);

        if (retval == ISP_SUCCESS)
            ISP_PRINT("\r\n BSC ROW SUM table address %x", addr);

        retval = ipipe_get_bsc_address(&addr, IPIPE_BSC_MEMORY_1);
        if (retval == ISP_SUCCESS)
            ISP_PRINT("\r\n BSC ROW SUM table address %x", addr);

    }

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    isp_disable_interrupt(ISP_RSZ_INT_LAST_PIX);

    isp_unhook_int_handler(ISP_RSZ_INT_LAST_PIX, handle2);
    isp_unhook_int_handler(ISP_RSZ_INT_LAST_PIX, handle1);

    TIMM_OSAL_Free(handle2);
    TIMM_OSAL_Free(handle1);

    return retval;
}

ISP_RETURN isptest20()
{
    ISP_RETURN retval = ISP_SUCCESS;

    /* Interrupt_Handle_T* handle1=(Interrupt_Handle_T*) NULL;
     * Interrupt_Handle_T* handle2=(Interrupt_Handle_T*) NULL; */

    isp_common_enable_clk(ISP_BL_CLK);

    retval = ccp2_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = ccp2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = ccp2_memory_test_params_init_20();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory test params init  failed");

    retval = ccp2_memory_config_20();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory config  failed");

    Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, OCPERROR_IRQ);
    Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, EOF_IRQ);

    retval = ccp2_start(MEMORY_READ);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 start failed");

    {

        uint32 val = 0;

        while (val == 0)
        {
            isp_common_get_int_sts(ISP_RSZ_INT_LAST_PIX, &val);

        }

        ISP_PRINT("\nint status= %d ", val);

    }

    /***********************Wait for status bit of RSZ to know that the operation is complete********/

    retval = ipipe_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipe_close failed");

    retval = ipipeif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_close failed");

    retval = rsz_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n rsz_close failed");

    retval = isif_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_close failed");

    retval = ccp2_close();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2_close failed");

    return retval;

}

/* ram->ccp2->ipipe_if->isif->ram */
/* img_buff to op_img_buff */
/* 640*480 to same, raw 10 to raw10 */
ISP_RETURN isptest21()
{

    ISP_RETURN retval = ISP_SUCCESS;

    Interrupt_Handle_T *handle1 = (Interrupt_Handle_T *) NULL;

    Interrupt_Handle_T *handle2 = (Interrupt_Handle_T *) NULL;

    isp_hook_int_handler(ISP_IPIPE_INT_LAST_PIX, handle2);
    isp_hook_int_handler(ISP_RSZ_INT_LAST_PIX, handle1);

    isp_enable_interrupt(ISP_IPIPE_INT_LAST_PIX);
    isp_enable_interrupt(ISP_RSZ_INT_LAST_PIX);

    isp_common_enable_clk(ISP_BL_CLK);
    iss_isp_input_config(ISS_ISP_INPUT_CCCP2);

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);
    isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_DISABLED);

    retval = ccp2_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = ccp2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = ccp2_memory_test_params_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory test params init  failed");

    retval = ccp2_memory_config();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 memory config  failed");

    Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, OCPERROR_IRQ);
    Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, EOF_IRQ);

    retval = ipipeif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_init failed");

    retval = ipipeif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_open failed");

    retval = ipipeif_params_init_4(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_params_init_2 failed");

    retval = ipipeif_config(&test_ipipeif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nipipeif_config  failed");

    retval = isif_init();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_init failed");

    retval = isif_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_open failed");

    retval = isif_params_init_1_1(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_params_init_1 failed");

    retval = isif_config(&test_isif_cfg);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\nisif_config failed");
    retval = isif_config_sdram_address((uint32) op_img_buff);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("isif set add fail");

    retval = isif_start(ISIF_START_WRITE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n isif_start failed");

    retval = ipipeif_start(IPIPEIF_SINGLE);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ipipeif_start failed");

    retval = ccp2_start(MEMORY_READ);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n ccp2 start failed");

    while (1) ;

}

/* sens->csi2->ram(img_buff) */
/* RAw10 to RAW10 */

ISP_RETURN isptest22()
{

    ISP_RETURN retval = ISP_SUCCESS;

    /* Interrupt_Handle_T * h1,*h2; */
    /* CSI2 configuration */

    retval = csi2_init();
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_init failed");

    retval = csi2_open();
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_open failed");

    retval = csi2_params_init_1(&test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\n csi2_params_init failed");

    retval = csi2_config(CSI2_DEVICE_A, &test_csi2_config);
    if (retval != CSI2_SUCCESS)
        ISP_PRINT("\ncsi2_config  failed");

    /* 
     * h1=(Interrupt_Handle_T*)TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
     * TIMM_OSAL_TRUE, 64, TIMMOSAL_MEM_SEGMENT_EXT); ISP_ASSERT(h1,NULL);
     * h1->arg1=0; h1->arg2=NULL; h1->callback=test_callback_1;
     * h1->context=ISS_CALLBACK_CONTEXT_HWI; h1->priority=1;
     * 
     * retval=csi2_context_irq_hook_int_handler(CSI2_DEVICE_A,
     * CSI2_CONTEXT_0, CSI2_FS_IRQ, h1); csi2_context_irq_enable_interrupt(
     * CSI2_DEVICE_A, CSI2_CONTEXT_0 , CSI2_FS_IRQ);
     * h2=(Interrupt_Handle_T*)TIMM_OSAL_Malloc(sizeof(Interrupt_Handle_T),
     * TIMM_OSAL_TRUE, 64, TIMMOSAL_MEM_SEGMENT_EXT); ISP_ASSERT(h2,NULL);
     * h2->arg1=0; h2->arg2=NULL; h2->callback=test_callback_2;
     * h2->context=ISS_CALLBACK_CONTEXT_SWI; h2->priority=1;
     * retval=csi2_context_irq_hook_int_handler(CSI2_DEVICE_A,
     * CSI2_CONTEXT_0, CSI2_FE_IRQ, h2); csi2_context_irq_enable_interrupt(
     * CSI2_DEVICE_A, CSI2_CONTEXT_0 , CSI2_FE_IRQ); */
    retval = csi2_enable_context(CSI2_DEVICE_A, 0);
    retval = csi2_start(CSI2_DEVICE_A);
    if (retval != ISP_SUCCESS)
        ISP_PRINT("\n csi2_enable failed");

    // test_start_sensor();

    while (1) ;

}
