/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file  ipipe_if.h
 *      This file contains the prototypes and definitions to configure ipipe_if
 *
 *
 * @path        Centaurus\drivers\drv_isp\inc\csl
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *!
 */
 /*========================================================================= */
#ifndef IPIPEIF_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define IPIPEIF_H

/****************************************************************
 *  INCLUDE FILES
 *****************************************************************/
#include "ipipeif_reg.h"

#include "../../isp5_utils/isp5_sys_types.h"
#include "../../common/inc/isp_common.h"

typedef volatile CSL_IpipeifRegs *ipipeif_regs_ovly;

#define IPIPEIF_BASE_OFFSET    0x1200
#define IPIPEIF_BASE_ADDRESS (ISP5_BASE_ADDRESS + IPIPEIF_BASE_OFFSET)

/* ================================================================ */
/**
 *Description:- enum for ctrl start/stop of ipipe-if
 */
 /*================================================================== */
typedef enum {
    IPIPEIF_STOP,                                          // stop/disable
    IPIPEIF_RUN,                                           // run/enable
    IPIPEIF_SINGLE                                         // one shot run
} IPIPEIF_START_T;

/* ================================================================ */
/**
 *Description:- struct to hold device specific data
 */
 /*================================================================== */
typedef struct {

    uint8 opened;

} ipipeif_dev_data_t;

/* ================================================================ */
/**
 *Description:- enum for selecting the data unpacking when read from ipipe-if
 */
 /*================================================================== */

typedef enum {

    UNPACK_NORMAL_16_BITS_PIXEL = 0,
    UNPACK_PACK_8_BITS_PIXEL_LINEAR = 1,
    UNPACK_PACK_8_BITS_PIXEL_ALAW = 2,
    UNPACK_PACK_12_BITS_PIXEL = 3
} IPIPEIF_UNPACK_T;

/* ================================================================ */
/**
 *Description:- enum for ctrl of data shift  whike read from RAM
 */
 /*================================================================== */
typedef enum {

    DATASFT_BITS11_0,
    DATASFT_BITS10_0,
    DATASFT_BITS9_0,
    DATASFT_BITS8_0,
    DATASFT_BITS7_0,
    DATASFT_BITS15_4
} IPIPEIF_DATA_SHIFT_T;

/* ================================================================ */
/**
 *Description:-enum for seleccting the direction of subtraction in case of DFS
 */
 /*================================================================== */
typedef enum {

    DFSDIR_SENSOR_MINUS_SDRAM,
    DFSDIR_SDRAM_MINUS_SENSOR
} IPIPEIF_DFS_DIR_T;

/* ================================================================ */
/**
 *Description:- enum for ctrl of average filter in ipipe path
 */
 /*================================================================== */
typedef enum {

    AVGFILT_OFF,
    AVGFILT_ON
} IPIPEIF_AVGFILT_T;

/* ================================================================ */
/**
 *Description:- enum for ctrl of WEN signal
 */
 /*================================================================== */
typedef enum {

    WENE_NO_EXTERNAL_WEN = 0,
    WENE_EXTERNAL_WEN = 1
} IPIPEIF_WEN_CTRL_T;

/* ================================================================ */
/**
 *Description:-enum for selecting the data format
 */
 /*================================================================== */
typedef enum {
    RAW_DATA,
    YCBCR_16_BIT_DATA
} IPIPEIF_YUV_SEL_T;

/* ================================================================ */
/**
 *Description:-enum for selecting YUV data format
 */
 /*================================================================== */
typedef enum {
    YUV8_YUV16,
    YUV8_YUV8to16
} YCBCR_YUV8_SEL_T;

/* ================================================================ */
/**
 *Description:-enum specifying the pos of Y and C samples
 */
 /*================================================================== */
typedef enum
{
    YUV8P_C_EVEN_Y_ODD,
    YUV8P_Y_EVEN_C_ODD
} IPIPEIF_YUV_YC_POS_T;

/* ================================================================ */
/**
 *Description:- enum for selecting the Defect pixel correction mode
 */
 /*================================================================== */
typedef enum {
    DPCMODE_DPC1,
    DPCMODE_DPC2
} IPIPEIF_DPC_MODE_T;

/* ================================================================ */
/**
 *Description:- General enable/disable enum for diffrerent components in ipipe-if
 */
 /*================================================================== */

typedef enum {
    IPIPEIF_FEATURE_ENABLE = 1,
    IPIPEIF_FEATURE_DISABLE = 0
} IPIPEIF_FEATURE_SELECT_T;

/* ================================================================ */
/**
 *Description:-enum for selecting the dpcm decompression bit size
 */
 /*================================================================== */
typedef enum {
    IPIPEIF_DPCM_BIT_SIZE_8_10 = 0,
    IPIPEIF_DPCM_BIT_SIZE_8_12 = 1
} IPIPEIF_DPCM_BIT_SIZE_T;

/* ================================================================ */
/**
 *Description:-enum for selecting the type of prediction used in decompressing data
 */
 /*================================================================== */
typedef enum {
    IPIPEIF_DPCM_PREDICTION_SIMPLE = 0,
    IPIPEIF_DPCM_PREDICTION_ADVANCED = 1
} IPIPEIF_DPCM_PRED_TYP_T;

/* ================================================================ */
/**
 *Description:-enum for selecting the type of prediction used in decompressing data
 */
 /*================================================================== */
typedef enum {
    IPIPEIF_DFS_DIR_SDRAM,
    IPIPEIF_DFS_DIR_SENSOR
} IPIPEIF_DFS_DIR_TYP_T;

/* ================================================================ */
/**
 *Description:- cfg struct for DPCM
 */
 /*================================================================== */
typedef struct {

    IPIPEIF_FEATURE_SELECT_T dpcm_en;
    IPIPEIF_DPCM_PRED_TYP_T dpcm_predictor;                /* Valid only if
                                                            * DPCM is
                                                            * enabled;
                                                            * dpcm_en=1. */
    IPIPEIF_DPCM_BIT_SIZE_T dpcm_bit_size;                 /* Valid only if
                                                            * DPCM is
                                                            * enabled;
                                                            * dpcm_en=1 */

} ipipeif_dpcm_cfg_t;

/* ================================================================ */
/**
 *Description:-cfg struct for Dark frame subtract
 */
 /*================================================================== */
typedef struct {
    IPIPEIF_FEATURE_SELECT_T dfs_gain_en;
    uint16 dfs_gain_val;                                   /* valid only if
                                                            * dfs_gain_en
                                                            * =IPIPEIF_FEATURE_ENABLE
                                                            */
    uint16 dfs_gain_thr;                                   /* valid only if
                                                            * dfs_gain_en
                                                            * =IPIPEIF_FEATURE_ENABLE
                                                            */
    uint16 oclip;                                          /* valid only if
                                                            * dfs_gain_en
                                                            * =IPIPEIF_FEATURE_ENABLE
                                                            */
    uint8 dfs_dir;                                         /* set to 0 if
                                                            * Sensor Parallel
                                                            * interface data
                                                            * is to be
                                                            * subtracted by
                                                            * DRK frm in
                                                            * SDRAM */
} ipipeif_dfs_cfg_t;

/* ================================================================ */
/**
 *Description:-struct to control defect pixel corrction in ipipeif
 */
 /*================================================================== */
typedef struct {
    IPIPEIF_FEATURE_SELECT_T dpc_en;
    uint16 dpc_thr;
} ipipeif_dpc_cfg_t;

/* ================================================================ */
/**
 *Description:- enum to select ip clk type
 */
 /*================================================================== */
typedef enum {

    IPIPEIF_IP_CLK_VPORT = 0,
    IPIPEIF_IP_CLK_IPIPEIF_CLK_DIV = 1
} IPIPEIF_IP_CLK_SEL;

/* ================================================================ */
/**
 *Description:- enum which controls the operation of INPSRC1
 */
 /*================================================================== */

typedef enum {
    IPIPEIF_ISIF_INPSRC_PP_RAW = 0,                        /* RAW data
                                                            * directly from
                                                            * Parallel port */
    IPIPEIF_ISIF_INPSRC_SDRAM_RAW = 1,                     /* RAW data from
                                                            * SDRAM, DPCM may
                                                            * or may not be
                                                            * enabled */
    IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM = 2,                 /* RAW data with
                                                            * dark frame
                                                            * subtract
                                                            * with/withoutDPCM */
    IPIPEIF_ISIF_INPSRC_SDRAM_YUV = 3                      /* Direct raw
                                                            * data...no DPCM
                                                            * no DRFMS */
} IPIPEIF_ISIF_INPUT_SRC_SEL_T;

typedef enum {
    IPIPEIF_ISIF_MUX = 0,
    IPIPEIF_IPIPE_MUX
} IPIPEIF_SELECT_MUX_T;

/* ================================================================ */
/**
 *Description:-struct for cfg'ing ip dimensions of ipipe-if
 */
 /*================================================================== */

typedef struct {

    uint16 ppln_hs_interval;
    uint16 lpfr_vs_interval;
    uint16 hnum;
    uint16 vnum;
} ipipeif_crop_dims_t;

/* ================================================================ */
/**
 *Description:- struct for cfg relevant to INPSRC1=0
 */
 /*================================================================== */

/* INPSRC1=0 */
typedef struct {

    ipipeif_dpc_cfg_t *dpc1_cfg;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;
    IPIPEIF_WEN_CTRL_T wen;
    IPIPEIF_IP_CLK_SEL ip_clk_sel;

} ipipeif_isif_ip_pp_t;

/* ================================================================ */
/**
 *Description:- struct for cfg relevant to INPSRC1=1
 */
 /*================================================================== */

/* INPSRC1=1 */
typedef struct {

    IPIPEIF_DATA_SHIFT_T data_shift;
    IPIPEIF_UNPACK_T unpack;
    uint8 oneshot;

    ipipeif_crop_dims_t crop_dims;

    // uint16 addru;
    uint32 address;
    uint16 adofs;

    ipipeif_dpcm_cfg_t *dpcm_cfg;

    ipipeif_dpc_cfg_t *dpc2_cfg;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;

    IPIPEIF_IP_CLK_SEL ip_clk_sel;

    uint8 clkdiv_m;
    uint8 clkdiv_n;

} ipipeif_isif_ip_sdram_t;

/* ================================================================ */
/**
 *Description:- struct for cfg relevant to INPSRC1=2
 */
 /*================================================================== */

/* INPSRC1=2 */
typedef struct {

    IPIPEIF_DATA_SHIFT_T data_shift;
    IPIPEIF_UNPACK_T unpack;

    ipipeif_crop_dims_t crop_dims;
    // uint16 addru;
    uint32 address;
    uint16 adofs;

    ipipeif_dpcm_cfg_t *dpcm_cfg;

    ipipeif_dpc_cfg_t *dpc2_cfg;

    ipipeif_dfs_cfg_t *dfs_cfg;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;

    IPIPEIF_DFS_DIR_TYP_T dfs_dir;

} ipipeif_isif_ip_sdram_dfs_t;

typedef struct {
    UInt32 enable;

    IPIPEIF_SELECT_MUX_T muxSel;

    IPIPEIF_DATA_SHIFT_T data_shift;
    IPIPEIF_UNPACK_T unpack;

    ipipeif_crop_dims_t crop_dims;

    uint32 address;
    uint16 adofs;

    IPIPEIF_DFS_DIR_T dfs_dir;
} ipipeif_ip_dfs_t;

/* ================================================================ */
/**
 *Description:- struct for cfg relevant to INPSRC1=3
 */
 /*================================================================== */

/* INPSRC1=3 */
typedef struct {
    uint8 oneshot;

    // uint16 ppln;

    ipipeif_crop_dims_t crop_dims;

    // uint16 addru;
    uint32 address;
    uint16 adofs;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;

    IPIPEIF_IP_CLK_SEL ip_clk_sel;
    uint8 clkdiv_m;
    uint8 clkdiv_n;

} ipipeif_isif_ip_yuv_t;

/* ================================================================ */
/**
 *Description:-
 *   ISIF input path can be configured in one of the 4 ways as enumerated in  IPIPEIF_ISIF_INPUT_SRC_SEL_T
 *   For each input path selected the configurations are embedded within the corresponding
 *   struct for eg if if input is IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM then the field "ipipeif_isif_ip_sdram_dfs_t"
 *   needs to be populated by the user. since only one configuration is possible at point the 4 sucj structures are a
 *  UNION "ipipeif_isif_input_cfg_t"

 ********************************************************************

*/
typedef union {
    ipipeif_isif_ip_pp_t pp_ip;                            /* isif_inpsrc=IPIPEIF_ISIF_INPSRC_PP_RAW
                                                            */
    ipipeif_isif_ip_sdram_t sdram_ip;                      /* isif_inpsrc=IPIPEIF_ISIF_INPSRC_SDRAM_RAW
                                                            */
    ipipeif_isif_ip_sdram_dfs_t sdram_dfs_ip;              /* isif_inpsrc=IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM
                                                            */
    ipipeif_isif_ip_yuv_t yuv_ip;                          /* isif_inpsrc=IPIPEIF_ISIF_INPSRC_SDRAM_YUV
                                                            */

} ipipeif_isif_input_cfg_t;

/* ================================================================ */
/**
 *Description:-enum which controls the operation of INPSRC1
  *                   IPIPE input mux can be configured in one of the following ways.
 */
 /*================================================================== */
typedef enum {
    IPIPEIF_IPIPE_INPSRC_ISIF = 0,                         /* RAW data
                                                            * directly from
                                                            * ISIF port */
    IPIPEIF_IPIPE_INPSRC_SDRAM_RAW = 1,                    /* RAW data from
                                                            * SDRAM, DPCM may
                                                            * or may not be
                                                            * enabled */
    IPIPEIF_IPIPE_INPSRC_ISIF_DARKFM = 2,                  /* RAW data with
                                                            * dark frame
                                                            * subtract
                                                            * with/withoutDPCM */
    IPIPEIF_IPIPE_INPSRC_SDRAM_YUV = 3                     /* Direct raw
                                                            * data...no DPCM
                                                            * no DRFMS */
} IPIPEIF_IPIPE_INPUT_SRC_SEL_T;

/* ================================================================ */
/**
 *Description:-enum to specify start position with respect to PP / ISIF
 */
 /*================================================================== */

typedef enum {
    IPIPEIF_START_POS_PP_VD = 0,
    IPIPEIF_START_POS_ISIF_VD = 1
} IPIPEIF_START_POS_T;

/* ================================================================ */
/**
 *Description:-Description:- struct for cfg relevant to INPSRC2=0
 */
 /*================================================================== */
/* INPSRC2=0 */
typedef struct {

    IPIPEIF_AVGFILT_T avg_filter_enable;
    uint16 gain;

    IPIPEIF_YUV_SEL_T YUV16;                               /* Funny, if you
                                                            * are getting YUV
                                                            * data from ISIF,
                                                            * set YUV16 =1 and
                                                            * YUV8 =1, if
                                                            * output of isif
                                                            * is YUV 8 bit
                                                            * data, set YUV
                                                            * 16=1 and yuv8=0
                                                            * if isif output
                                                            * is yuv16 bit set
                                                            * YUV16 =0 for if
                                                            * isif output is
                                                            * RAW */
    YCBCR_YUV8_SEL_T YUV8;

    IPIPEIF_YUV_YC_POS_T YUV8P;
    /* 0: even pixel = CbCr data, odd pixel = Y data 1: even pixel = Y data,
     * odd pixel = CbCr data */

    ipipeif_dpc_cfg_t *dpc1_cfg;

    uint8 decimation_enable;
    uint8 resize;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;

} ipipeif_ipipe_ip_isif_t;

/* ================================================================ */
/**
 *Description:- struct for cfg relevant to INPSRC2=1
 */
 /*================================================================== */
/* INPSRC2=1 */
typedef struct {

    IPIPEIF_DATA_SHIFT_T data_shift;
    IPIPEIF_UNPACK_T unpack;

    uint8 oneshot;

    ipipeif_crop_dims_t crop_dims;

    /* 32-byte aligned , 27 bit address */
    uint32 address;
    uint16 adofs;

    IPIPEIF_AVGFILT_T avg_filter_enable;
    uint16 gain;

    ipipeif_dpcm_cfg_t *dpcm_cfg;

    ipipeif_dpc_cfg_t *dpc2_cfg;

    uint8 decimation_enable;
    uint8 resize;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;
    IPIPEIF_IP_CLK_SEL ip_clk_sel;
    uint8 clkdiv_m;
    uint8 clkdiv_n;
    IPIPEIF_YUV_SEL_T YUV16;

} ipipeif_ipipe_ip_sdram_t;

/* ================================================================ */
/**
 *Description:- struct for cfg relevant to INPSRC2=2
 */
 /*================================================================== */
/* INPSRC2=2 */
typedef struct {

    IPIPEIF_DATA_SHIFT_T data_shift;
    IPIPEIF_UNPACK_T unpack;

    ipipeif_crop_dims_t crop_dims;

    // uint16 addru;
    uint32 address;
    uint16 adofs;

    IPIPEIF_AVGFILT_T avg_filter_enable;

    uint16 gain;

    ipipeif_dpcm_cfg_t *dpcm_cfg;

    /* DPC params */
    ipipeif_dpc_cfg_t *dpc2_cfg;

    ipipeif_dfs_cfg_t *dfs_cfg;

    uint8 decimation_enable;
    uint8 resize;

    uint8 vd_polarity;
    uint8 hd_polarity;

    uint8 int_sw;

    uint8 align_sync;
    uint16 ini_rsz;

    IPIPEIF_DFS_DIR_TYP_T dsf_dir;

} ipipeif_ipipe_ip_sdram_dfs_t;

/* ================================================================ */
/**
 *Description:-struct for cfg relevant to INPSRC2=3
 */
 /*================================================================== */
typedef struct {

    // uint16 ppln;
    ipipeif_crop_dims_t crop_dims;
    // uint16 addru;
    uint32 address;
    uint16 adofs;

    IPIPEIF_AVGFILT_T avg_filter_enable;

    uint8 decimation_enable;
    uint8 resize;

    uint8 vd_polarity;
    uint8 hd_polarity;

    IPIPEIF_START_POS_T int_sw;

    uint8 align_sync;
    uint16 ini_rsz;
    IPIPEIF_IP_CLK_SEL ip_clk_sel;
    uint8 clkdiv_m;
    uint8 clkdiv_n;
} ipipeif_ipipe_ip_yuv_t;

/*******************************************************************
 * Description :-  ipipe input path can be configured in one of the 4 ways as enumerated in  IPIPEIF_IPIPE_INPUT_SRC_SEL_T
 *                     For each input path selected the configurations are embedded within the corresponding
 *                 struct for eg if if input is IPIPEIF_ipipe_INPSRC_PP_RAW_DARKFM then the field "ipipeif_ipipe_ip_sdram_dfs_t"
 *                 needs to be populated by the user. since only one configuration is possible at point the 4 sucj structures are a
 *  UNION "ipipeif_ipipe_input_cfg_t"

 ********************************************************************

*/

typedef union {
    ipipeif_ipipe_ip_isif_t isif_ip;                       /* ipipe_inpsrc=IPIPEIF_ipipe_INPSRC_PP_RAW
                                                            */
    ipipeif_ipipe_ip_sdram_t sdram_ip;                     /* ipipe_inpsrc=IPIPEIF_ipipe_INPSRC_SDRAM_RAW
                                                            */
    ipipeif_ipipe_ip_sdram_dfs_t sdram_dfs_ip;             /* ipipe_inpsrc=IPIPEIF_ipipe_INPSRC_PP_RAW_DARKFM
                                                            */
    ipipeif_ipipe_ip_yuv_t yuv_ip;                         /* ipipe_inpsrc=IPIPEIF_ipipe_INPSRC_SDRAM_YUV
                                                            */

} ipipeif_ipipe_input_cfg_t;

/******************************************************************/
/**
  Description:-H3A input path configuration, for both ISIF anf IPIPE
  */
 /*********************************************************************/

typedef struct {
    uint8 decimation_enable;
    uint8 resize;
    IPIPEIF_AVGFILT_T avg_filter_enable;
    uint8 align_sync;
    uint16 ini_rsz;
} ipipeif_h3a_input_cfg_t;

/******************************************************************/
/**
  Description:- main IPIPE_IF input path configuration, for both ISIF anf IPIPE
  */

 /********************************************************************/

typedef struct {
    IPIPEIF_ISIF_INPUT_SRC_SEL_T isif_inpsrc;
    ipipeif_isif_input_cfg_t isif_ip_cfg;

    IPIPEIF_IPIPE_INPUT_SRC_SEL_T ipipe_inpsrc;
    ipipeif_ipipe_input_cfg_t ipipe_ip_cfg;

    ipipeif_h3a_input_cfg_t h3a_cfg;

} ipipeif_path_cfg_t;

/******************************************************************************
 *                      FUNCTION PROTOTYPES
 *******************************************************************************/

/* ================================================================ */
/*
 *  Description:- Init should be called before calling any other function
 *
 *
 *  @param   none

 *  @return    ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipeif_init
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
ISP_RETURN ipipeif_init();

/* ================================================================ */
/*
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable ipipe clk
 *
 *
 *  @param   :- none

 *  @return  :- ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_open
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
ISP_RETURN ipipeif_open();

/* ================================================================ */
/*
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler
 *
 *
 *  @param   none

 *  @return   ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_close
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
ISP_RETURN ipipeif_close();

/* ================================================================ */
/*
 *  Description:- This is the main cfg structure of ipipe-if. It configures both INPSRC1 and INPSRC2
 *
 *
 *  @param   ipipeif_path_cfg_t*:- pointer to the configuration structure

 *  @return    :- ISP_RETURN
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ipipeif_config
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
ISP_RETURN ipipeif_config(ipipeif_path_cfg_t * ipipeif_cfg);

/* ===================================================================
 *  @func     ipipeif_h3a_input_config
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
ISP_RETURN ipipeif_h3a_input_config(ipipeif_h3a_input_cfg_t * h3a_cfg);

/* ===================================================================
 *  @func     ipipeif_set_sdram_in_addr
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
ISP_RETURN ipipeif_set_sdram_in_addr(uint32 address);

/* ================================================================ */
/*
 *  Description:- This routine will start /stop the ipipe module,
 *                 start is of one shot type or continuous typt
 *
 *
 *  @param IPIPE_START_T  - enumeration of the start / stop commands

 *  @return         ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_start
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
ISP_RETURN ipipeif_start(IPIPEIF_START_T enable);

ISP_RETURN ipipeif_set_dfs_config(ipipeif_ip_dfs_t *dfsCfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
