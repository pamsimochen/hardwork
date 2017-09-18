/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file rsz.h
 *
 * This file contains the prototypes and definitions to configure 
 *
 * @path   Centaurus\drivers\drv_isp\in\csl
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
*/
/*======================================================================== */
#ifndef RSZ_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define RSZ_H

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/
#include "rsz_reg.h"
#include "../../isp5_utils/isp5_sys_types.h"
#include "../../common/inc/isp_common.h"
/* Create one input and one output channel */

/* ================================================================ */
/* defines */
/*================================================================= */

#define RESIZER_BASE_OFFSET 0x0400
#define RESIZER_BASE_ADDRESS (ISP5_BASE_ADDRESS+RESIZER_BASE_OFFSET)

/* ================================================================ */
/**
 * Descritpion :- typedef's for ease 
*/
/*================================================================= */
typedef Rsz_Regs *rsz_regs_ovly;

typedef Rsz_A_Regs *rsz_A_regs_ovly;

typedef Rsz_B_Regs *rsz_B_regs_ovly;

/* ================================================================ */
/**
 * Description:-Device specific data
*/
/*================================================================= */
typedef struct {

    uint8 opened;

} rsz_dev_data_t;

/* ================================================================ */
/**
 * Description:- enum for selecting one of the 2 resizers
*/
/*================================================================= */
typedef enum {

    RESIZER_A = 0,
    RESIZER_B = 1,
    NUM_RESIZERS = 2
} RESIZER_T;

/* ================================================================ */
/**
 * Description:- resizer clk ctrl enum
*/
/*================================================================= */
typedef enum {
    RSZ_CLK_ENABLE = 1,
    RSZ_CLK_DISABLE = 0
} RSZ_MODULE_CLK_CTRL_T;

/* ================================================================ */
/**
 * Description:- struct to cfg the global crop dimensions at resizer i/p
*/
/*================================================================= */
typedef struct {
    uint32 v_pos;                                          /* Global frame
                                                            * start */
    uint32 v_size;                                         /* Global frame
                                                            * start */
    uint32 h_pos;                                          /* Crop Horizontal 
                                                            * start */
    uint32 h_size;                                         /* Crop vertical
                                                            * start */

} rsz_global_crop_dims_t;

/* ================================================================ */
/**
 * Description:- struct to cfg the input crop of each resizer
*/
/*================================================================= */
typedef struct {
    uint32 vps;                                            /* Resizer input
                                                            * starts after
                                                            * these many
                                                            * lines */
    uint32 hps;                                            /* Resizer input
                                                            * starts after
                                                            * these many
                                                            * pixels */

} rsz_input_crop_dims_t;

/* ================================================================ */
/**
 * Description:- struct to cfg the dimensions of the output of resizer
*/
/*================================================================= */
typedef struct {
    uint32 v_size;                                         /* Resizer output
                                                            * starts after
                                                            * these many
                                                            * lines */
    uint32 h_size;                                         /* Resizer output
                                                            * starts after
                                                            * these many
     * pixels *//* op line size -1 */
} rsz_output_dims_t;

/* ================================================================ */
/**
 * Description:- stryct to cfg the resize ratio of the resizing process
*/
/*================================================================= */
typedef struct {
    uint32 v_rsz_ratio;                                    /* The parameter
                                                            * for vertical
                                                            * resize. */
    uint32 h_rsz_ratio;                                    /* The parameter
                                                            * for horizontal
                                                            * resizing
                                                            * process. */
} rsz_resize_ratio_t;

/* ================================================================ */
/**
 * Description:- enum for selecting what is the op pix fmt, for yuv420 2 addresses are needed,
 								one for Y component, one for C componenet
*/
/*================================================================= */
typedef enum {
    RSZ_YUV422_RAW_RGB_OP = 0,
    RSZ_YUV420_Y_OP = 0,                                   // intentionally
                                                           // made 0 
    RSZ_YUV420_C_OP = 1
} RSZ_OP_ADDRESS_T;

/* ================================================================ */
/**
*  Description:-struct to specify the o/p address of each resizer
*/
/*================================================================== */
typedef struct {
    /* These registers give the address where the data is to be written
     * irrespective of the format of the data and is common for all formats */
    uint32 base_addr;                                      /* OP Base address */
    uint32 start_addr;                                     /* OP start
                                                            * address */
    uint32 start_offset;
    uint32 start_pointer;
    uint32 end_pointer;
} rsz_output_addr_t;

/* ================================================================ */
/**
 * Description:- enumeration to cfg flip at o/p of each resizer
*/
/*================================================================= */
typedef enum {
    RSZ_V_NFLIP_H_NFLIP = 0,                               // NFLIP= flip
                                                           // disabled
    RSZ_V_NFLIP_H_FLIP = 1,
    RSZ_V_FLIP_H_NFLIP = 2,
    RSZ_V_FLIP_H_FLIP = 3
} RSZ_FLIP_CFG_T;

/* ================================================================ */
/**
 * RSZ_START_T is used to start/stop the operartion of ipipe
 *		
 * @param  RSZ_STOP , this is to disable ipipe
 *
 * @param  RSZ_RUN  , this enables continuous mode operation of ipipe
 * 
 *
 * @param   RSZ_SINGLE . this enables the one-shot operation of ipipe
*/
/*================================================================= */
typedef enum {
    RSZ_STOP,                                              // stop/disable
    RSZ_RUN,                                               // run/enable
    RSZ_SINGLE                                             // one shot run
} RSZ_START_T;

/* ================================================================ */
/**
 * Description:- enum specifying one of the 2 possible RESIZE operation
*/
/*================================================================= */
typedef enum {
    RESIZER_NORMAL_OPERATION = 0,
    RESIZER_DOWNSCALE_MODE = 1
} RSZ_RESCALE_MODE_T;

/* ================================================================ */
/**
 * Description:- This enum directly configures the register RZA_420, this is for selecting YUV422 to YUV420 
 *                    conversion and enabling of output of "c" and "Y" samples
*/
/*================================================================= */
typedef enum {

    /* uint32 RZB_420; *//* YUV420 specific */
    RSZ_YUV422_OP = 0,                                     // yuv422 op
    RSZ_YUV420_Y_OP_ONLY = 1,                              // only "y"
                                                           // component of
                                                           // yuv420 is op
    RSZ_YUV420_C_OP_ONLY = 2,                              // only "c"
                                                           // component of
                                                           // yuv420 is op
    RSZ_YUV420_YC_OP = 3                                   // both y and c
                                                           // component of
                                                           // yuv420 is op
} RSZ_YUV420_YC_SEL_T;

/*================================================================ */
/**
 * Description:- strct to encapsulate the phase parameters of both vertical resizing process
*/
/*================================================================= */
typedef struct {
    uint16 phs_y;
    uint16 phs_c;
} rsz_v_phs_cfg_t;

/*================================================================ */
/**
 * Description:-staruct to cfg the phase of Horizontal resizing process
*/
/*================================================================= */
typedef struct {
    uint16 h_phs;                                          // . RZA_H_PHS

    uint16 h_phs_adj;                                      // . RZA_H_PHS_ADJ

} rsz_h_phs_cfg_t;

/* ================================================================ */
/**
 * Description:-enumeration to select the sampling method for C and Y components
*/
/*================================================================= */
typedef enum {
    C_CUBIC_Y_CUBIC = 0,
    C_CUBIC_Y_LINEAR = 1,
    C_LINEAR_Y_CUBIC = 2,
    C_LINEAR_Y_LINEAR = 3
} RSZ_RESIZE_METHOD_T;

/* ================================================================ */
/**
 * Description:-struct to encapsulate the low pass filter params for both Horizontal and Vertical resizing
*/
/*================================================================= */
typedef struct {

    uint8 c_intensity;
    uint8 y_intensity;

} rsz_lpf_cfg_t;

/* ================================================================ */
/**
 * Description:- Enum to ctrl the resizing operation mode
*/
/*================================================================= */
typedef enum {
    RSZ_DOWNSCALE_DISABLE = 0,
    RSZ_DOWNSCALE_ENABLE = 1
} RSZ_DOWNSCALE_CTRL_T;

/* ================================================================ */
/**
 * Description:-struct to encapsulate the parameters required for downscale operation of resizer
*/
/*================================================================= */

typedef struct {
    uint8 v_avg_size;
    uint8 h_avg_size;

} rsz_downscale_cfg_t;

/* ================================================================ */
/**
 * Description:- enum to ctrl the o/p of RGB data
*/
/*================================================================= */

typedef enum {
    RSZ_RGB_OP_DISABLE = 0,
    RSZ_RGB_OP_ENABLE = 1
} RSZ_RGB_OP_CTRL_T;

/* ================================================================ */
/**
 * Description:-This enumeration is used to ctrl the o/p pixel size of RGB data
*/
/*================================================================= */
typedef enum {

    RSZ_RGB_PIX_SIZE_32 = 0,
    RSZ_RGB_PIX_SIZE_16 = 1
} RSZ_RGB_PIX_SIZE_T;

/* ================================================================ */
/**
 * Description:-This enumeration is used to ctrl the masking of first 2 and last 2 pixels affected by yuv422 to yuv444 conversion
  MSK1 is for last 2 pixels,MSK0 is for first 2 pixels . The reg it affects is "RZA_RGB_TYP"
*/
/*================================================================= */
typedef enum {
    RSZ_RGB_MSK1_DISABLE_MSK0_DISABLE = 0,
    RSZ_RGB_MSK1_DISABLE_MSK0_ENABLE = 1,
    RSZ_RGB_MSK1_ENABLE_MSK0_DISABLE = 2,
    RSZ_RGB_MSK1_ENABLE_MSK0_ENABLE = 3
} RSZ_RGB_MASK_CTRL_T;

/* ================================================================ */
/**
 * Description:-This struct encapsulates the parameters required for RGB o/p
*/
/*================================================================= */
typedef struct {
    RSZ_RGB_PIX_SIZE_T rgb_pix_size;
    RSZ_RGB_MASK_CTRL_T mask_ctrl;
    uint8 rgb_alpha_blend;
} rsz_rgb_op_cfg_t;

/* ================================================================ */
/**
 * Description:- enum to specify a operating mode for the resizer module
*/
/*================================================================= */
typedef enum {
    RESIZER_BYPASS = 0,
    RESIZER_PASSTHRU = 1,
    RESIZER_RESIZE = 2
} RSZ_OPERATING_MODE_T;

/* ================================================================ */
/**
 * Description:-this enum is used to select the YUV data types between i/p amd o/p
*/
/*================================================================= */
typedef enum {
    /* No distinction is made between rgb565 and argb, since both do not
     * requiree two passes as in YUV420 case */
    /* YUV422_IP_YUV422_OP and YUV422_IP_YUV420_OP are esentially set the
     * same value in fmt the o/p distinction is made under another register
     * and parameter set */
    YUV420_IP_COL_Y,
    YUV420_IP_COL_C
} RSZ_IP_COL_T;

/* ================================================================ */
/**
 * Description:-this enum is used to select the YUV data types between i/p amd o/p
*/
/*================================================================= */
typedef enum {
    /* No distinction is made between rgb565 and argb, since both do not
     * requiree two passes as in YUV420 case */
    /* YUV422_IP_YUV422_OP and YUV422_IP_YUV420_OP are esentially set the
     * same value in fmt the o/p distinction is made under another register
     * and parameter set */
    YUV420_IP_YUV420_OP = 0,
    YUV422_IP_YUV420_OP = 1,
    YUV422_IP_YUV422_OP = 2,
    YUV422_IP_RGB_OP = 3,
    RAW_IP_RAW_OP = 4                                      // only in bypass
                                                           // and passthru
} RSZ_PIXFMT_MODE_T;

/* ================================================================ */
/**
 * Description:-the enumeration selects the i/p to rszer
*/
/*================================================================= */
typedef enum {
    RSZ_IP_IPIPE = 0,
    RSZ_IP_IPIPEIF = 1
} RSZ_IP_PORT_SEL_T;

/* ================================================================ */
/**
 * Description:-enum to control the write pin
*/
/*================================================================= */
typedef enum {

    RSZ_WRT_ENABLE = 1,
    RSZ_WRT_DISABLE = 0
} RSZ_WRT_ENABLE_T;

/* ================================================================ */
/**
 * Description:-enum that gives the position of Y wrt C samples when yuv444 is converted to yuv422,
 * This should match the cfg in ipipe YUV444 to YUV422 conversion bloc
*/
/*================================================================= */

typedef enum {
    RSZ_YC_COSITED = 0,
    RSZ_YC_CENTERED = 1
} RSZ_IP_YC_PHASE_T;

/* ================================================================ */
/**
 * Description:-struct to encapsulate the common cfg of both the resizers
*/
/*================================================================= */
typedef struct {
    RSZ_IP_PORT_SEL_T ip_port;
    RSZ_WRT_ENABLE_T wrt_enable;
    uint16 rszA_dma_size;                                  // not needed in
                                                           // Bypass and
                                                           // passthru mode
    uint16 rszB_dma_size;                                  // 
    uint16 fifo_thr_low;                                   // IN_FIFO_CTRL;
    uint16 fifo_thr_high;

    RSZ_PIXFMT_MODE_T rsz_pix_fmt_mode;
    RSZ_IP_YC_PHASE_T yuv_phase;                           // y and c phase,
                                                           // co -sited or
                                                           // centered
    uint16 rszA_int_lines;                                 // The interrupt
                                                           // is triggered
                                                           // everytime
                                                           // (RZA+1) lines
                                                           // are written to
                                                           // the circular
                                                           // buffer (Y
                                                           // buffer). 
    uint16 rszB_int_lines;                                 // The interrupt
                                                           // is triggered
                                                           // everytime
                                                           // (RZA+1) lines
                                                           // are written to
                                                           // the circular
                                                           // buffer (Y
                                                           // buffer). 
    uint16 fracdiv;
} rsz_common_cfg_t;

/* ================================================================ */
/**
 * Description:-this struct is for configuring the bypass mode operation
*/
/*================================================================= */
typedef struct {
    rsz_common_cfg_t *rsz_common_params;
    RSZ_FLIP_CFG_T flip_params;
    RSZ_PIXFMT_MODE_T pix_fmt;
    rsz_global_crop_dims_t *crop_dims;
} rsz_bypass_mode_cfg_t;

/* ================================================================ */
/**
 * Description:-this struct encapsulates the params for configuring resizer in oasthru mode
*/
/*================================================================= */
typedef struct {
    rsz_common_cfg_t *rsz_common_params;
    RSZ_FLIP_CFG_T flip_params;
    RSZ_PIXFMT_MODE_T pix_fmt;
    rsz_global_crop_dims_t *crop_dims;

} rsz_passthru_mode_cfg_t;

/* ================================================================ */
/**
 * main cfg structure for individual resizer configuration in case of resizing mode
*/
/*================================================================= */
typedef struct {

    rsz_output_dims_t *op_dims_p;
    rsz_input_crop_dims_t *ip_dims_p;

    RSZ_YUV420_YC_SEL_T yuv420_yc_op;

    rsz_v_phs_cfg_t *v_phs_p;
    rsz_h_phs_cfg_t *h_phs_p;

    rsz_resize_ratio_t *ratio_p;

    rsz_lpf_cfg_t *v_lpf_p;
    rsz_lpf_cfg_t *h_lpf_p;

    RSZ_RESIZE_METHOD_T h_resize_method;
    RSZ_RESIZE_METHOD_T v_resize_method;

    RSZ_DOWNSCALE_CTRL_T downscale_sel;
    rsz_downscale_cfg_t *downscale_p;                      // valid only if
                                                           // downscale_sel=RSZ_DOWNSCALE_ENABLE

    RSZ_RGB_OP_CTRL_T rgb_op_sel;
    rsz_rgb_op_cfg_t *rgb_cfg_p;                           // valid only iff
                                                           // rgb_op_sel=RSZ_RGB_OP_ENABLE

    RSZ_FLIP_CFG_T flip_param;

} rsz_ip_op_cfg_t;

/* ================================================================ */
/**
 * Description:-struct to encapsulate resizer a, resizer B and the common cfg's
*/
/*================================================================= */
typedef struct {

    rsz_ip_op_cfg_t *rsz_A_cfg;

    rsz_ip_op_cfg_t *rsz_B_cfg;

    rsz_common_cfg_t *rsz_common_params;
    rsz_global_crop_dims_t *crop_dims;

} rsz_resize_mode_cfg_t;

/* ================================================================ */
/**
 * Description:-union which abstrats the use of one resizing mode from another
*/
/*================================================================= */
typedef union {

    rsz_bypass_mode_cfg_t *bp_mode_cfgp;
    rsz_passthru_mode_cfg_t *passthru_mode_cfgp;
    rsz_resize_mode_cfg_t *resize_mode_cfgp;
} rsz_mode_cfg_t;

/* ================================================================ */
/**
 * Description:-main struct that needs to be passed as i/p rsz_config()
*/
/*================================================================= */
typedef struct {
    RSZ_OPERATING_MODE_T mode;
    rsz_mode_cfg_t *mode_cfg_p;
} rsz_config_t;

/* ================================================================ */
/**
 * Description:-struct to cfg max.min values for Y/C i/p's
*/
/*================================================================= */
typedef struct {

    uint8 y_min;
    uint8 y_max;
    uint8 c_min;
    uint8 c_max;

} rsz_yc_min_max;

/*****************************************************************************************
 *                                            FUNCTION PROTOTYPES
 *
 *******************************************************************************************/

/* ================================================================ */
/**
 *  Description:-This routine is the inittialisation routine,nit should be called before calling any other function
 *  
 *
 *  @param   none

 *  @return    ISP_RETURN     
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_init                                               
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
ISP_RETURN rsz_init();

/* ================================================================ */
/**
 *  Description:-open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable rsz clk
 *  
 *
 *  @param  none  

 *  @return    ISP_RETURN     
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_open                                               
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
ISP_RETURN rsz_open();

/* ================================================================ */
/**
 *  Description:-close will de-init the CSL reg ptr, cut rsz clk,
 *                removes the int handler 
 *  
 *
 *  @param   none

 *  @return   ISP_RETURN      
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_close                                               
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
ISP_RETURN rsz_close();

/* ================================================================ */
/**
 *  Description:-this routine will cfg the common aspects of resizer A and resizer B
 *  
 *
 *  @param   rsz_common_cfg_t* :- pointer to cfg struct

 *  @return       ISP_RETURN  
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_common_config                                               
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
ISP_RETURN rsz_common_config(rsz_common_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-this routine cfg's the resizer module in bypass mode .
 *  
 *
 *  @param  rsz_bypass_mode_cfg_t *:- ptr to bypass cfg struct 

 *  @return         ISP_RETURN
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_bypass_cfg                                               
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
ISP_RETURN rsz_bypass_cfg(rsz_bypass_mode_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-this routine cfg's the resizer module in passthrough mode
 *  
 *
 *  @param   rsz_passthru_mode_cfg_t* :- ptr to cfg struct

 *  @return         ISP_RETURN
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_passthru_cfg                                               
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
ISP_RETURN rsz_passthru_cfg(rsz_passthru_mode_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-this routine cfg's the resizer in "resize" mode wherein resizing is possible
 *  
 *
 *  @param   rsz_resize_mode_cfg_t* :- pointer to resizing cfg struct

 *  @return    ISP_RETURN     
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_resize_mode_cfg                                               
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
ISP_RETURN rsz_resize_mode_cfg(rsz_resize_mode_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg flip parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_flip                                               
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
ISP_RETURN rsz_cfg_flip(RESIZER_T resizer, RSZ_FLIP_CFG_T flip);

/* ================================================================ */
/**
 *  Description:- This routine will cfg YUV output related, YUV422 or Y/C in case of YUV420 parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
RSZ_YUV420_YC_SEL_T:- enum for selecting different o/p's

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_yuv420_op                                               
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
ISP_RETURN rsz_cfg_yuv420_op(RESIZER_T resizer, RSZ_YUV420_YC_SEL_T yc_sel);

/* ================================================================ */
/**
 *  Description:- This is the main routine which configures the resizer in either bypsass,passthru or resize mode
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_config                                               
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
ISP_RETURN rsz_config(rsz_config_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will enable clocks of the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
RSZ_MODULE_CLK_CTRL_T:- enum to enable /disable clock

 *  @return      ISP_RETURN   
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_clock_enable                                               
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
ISP_RETURN rsz_clock_enable(RESIZER_T resizer, RSZ_MODULE_CLK_CTRL_T enable);

/* ================================================================ */
/**
 *  Description:- This routine will cfg global crop parameters in the resizer module
 *  
 *
 *  @param   rsz_global_crop_dims_t*:- pointer to cfg srct

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_global_crop                                               
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
ISP_RETURN rsz_cfg_global_crop(rsz_global_crop_dims_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg cropping at the i/p's of the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_input_crop_dims_t * :-crop cfg struct

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_ip_dims                                               
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
ISP_RETURN rsz_cfg_ip_dims(RESIZER_T resizer, rsz_input_crop_dims_t * ip_dims);

/* ================================================================ */
/**
 *  Description:- This routine will cfg ouput dimensions of the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_output_dims_t* :- pointer to cfg struct

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_op_dims                                               
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
ISP_RETURN rsz_cfg_op_dims(RESIZER_T resizer, rsz_output_dims_t * op_dims);

/* ================================================================ */
/**
 *  Description:- This routine will cfg output address parameters of the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 			RSZ_OP_ADDRESS_T :- enum for selecting what is the op pix fmt, for yuv420 2 addresses are needed,
 								one for Y component, one for C componenet
			rsz_output_addr_t*:- pointer to cfg struct

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_op_addr                                               
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
ISP_RETURN rsz_cfg_op_addr(RESIZER_T resizer, RSZ_OP_ADDRESS_T op_addr_type,
                           rsz_output_addr_t * address);

/* ================================================================ */
/**
 *  Description:-This routine will cfg the Y and C min/max values
 *  
 *
 *  @param   rsz_yc_min_max*:- pointer to cfg struct

 *  @return    ISP_RETURN     
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_yc_max_min                                               
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
ISP_RETURN rsz_cfg_yc_max_min(rsz_yc_min_max * cfg);

/* ================================================================ */
/**
 *  Description:- This routine cfg's the i/p to rsz
 *  
 *
 *  @param  :- RSZ_IP_PORT_SEL_T:- enum specifying the input to rsz, ipipe/ipipe-if

 *  @return     ISP_RETURN    
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_ip_port                                               
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
void rsz_cfg_ip_port(RSZ_IP_PORT_SEL_T ip_port);

/* ================================================================ */
/**
 *  Description:-This rutine cfg's the registers required for the different inpt oupt pixel format types
 *  
 *
 *  @param   RSZ_PIXFMT_MODE_T:- enumeration of possible i/p and o/p pixel formats

 *  @return    ISP_RETURN     
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_io_pixel_format                                               
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
ISP_RETURN rsz_cfg_io_pixel_format(RSZ_PIXFMT_MODE_T io_pix_fmt);

/* ===================================================================
 *  @func     rsz_cfg_io_pixel_col                                               
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
ISP_RETURN rsz_cfg_io_pixel_col(RSZ_IP_COL_T col);

/* ================================================================ */
/**
 *  Description:- This routine will cfg resize ratio in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_resize_ratio_t* :- pointer to cfg struct of horizontal and vertical resize ratios

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_resize_ratio                                               
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
ISP_RETURN rsz_cfg_resize_ratio(RESIZER_T resizer,
                                rsz_resize_ratio_t * rsz_ratio);

/* ================================================================ */
/**
 *  Description:- This routine will cfg rescaling mode in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
RSZ_RESCALE_MODE_T:- enum for specifying the resizing filter, downscale / normal

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_set_down_scale_mode                                               
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
ISP_RETURN rsz_set_down_scale_mode(RESIZER_T resizer,
                                   RSZ_RESCALE_MODE_T rsz_operation_mode);

/* ================================================================ */
/**
 *  Description:- This routine will cfg the horizontal,low pass filter parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_lpf_cfg_t * :- pointer to lpf cfg

 *  @return      ISP_RETURN   
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_cfg_h_lpf                                               
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
ISP_RETURN rsz_cfg_h_lpf(RESIZER_T resizer, rsz_lpf_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg vertical low pass filterparameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_lpf_cfg_t * :- pointer to lpf cfg

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_v_lpf                                               
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
ISP_RETURN rsz_cfg_v_lpf(RESIZER_T resizer, rsz_lpf_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg horizontal resizing filter parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
RSZ_RESIZE_METHOD_T:- enum specifying the algo cubic convolution/linear 

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_resize_h_method                                               
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
ISP_RETURN rsz_cfg_resize_h_method(RESIZER_T resizer,
                                   RSZ_RESIZE_METHOD_T method);

/* ================================================================ */
/**
 *  Description:- This routine will cfg vertical resizing filter parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
RSZ_RESIZE_METHOD_T:- enum specifying the algo cubic convolution/linear 

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_resize_v_method                                               
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
ISP_RETURN rsz_cfg_resize_v_method(RESIZER_T resizer,
                                   RSZ_RESIZE_METHOD_T method);

/* ================================================================ */
/**
 *  Description:- This routine will cfg downscale mode parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_downscale_cfg_t* :-pointer to cfg struct for downsscale parameters

 *  @return      ISP_RETURN   
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_cfg_downscale_mode                                               
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
ISP_RETURN rsz_cfg_downscale_mode(RESIZER_T resizer, rsz_downscale_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg Horizontal phase values  in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_h_phs_cfg_t *:- pointer to phase settings

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_h_phs                                               
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
ISP_RETURN rsz_cfg_h_phs(RESIZER_T resizer, rsz_h_phs_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg vertical phase values  in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_v_phs_cfg_t *:- pointer to phase settings

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_v_phs                                               
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
ISP_RETURN rsz_cfg_v_phs(RESIZER_T resizer, rsz_v_phs_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:- This routine will cfg YUV 444 to RGB conversion parameters in the specified resizer
 *  
 *
 *  @param   RESIZER_T:- enum for selecting the resizer
 rsz_rgb_op_cfg_t*:- pointer to RGB conversion parameters

 *  @return      ISP_RETURN   
*/
/*================================================================= */

/* ===================================================================
 *  @func     rsz_cfg_rgb_op                                               
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
ISP_RETURN rsz_cfg_rgb_op(RESIZER_T resizer, rsz_rgb_op_cfg_t * cfg);

/* ================================================================ */
/**
 *  Description:-Place holder for updating registers at frame boundaries
 *  NOTE !!!!!!!!!!!This is incomplete
 *
 *  @param   

 *  @return         
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_reg_update_callback                                               
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
void rsz_reg_update_callback(ISP_RETURN status, uint32 arg1, void *arg2);

/* ================================================================ */
/**
 *  Description:- This routine will start /stop the rsz module,
 *                 start is of one shot type or continuous typt
 *  
 *  NOTE:- This routine enables the resizer at the top level , it does not enable individual resizers!!!!
 *  @param RSZ_START_T  - enumeration of the start / stop commands

 *  @return         ISP_RETURN
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_start                                               
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
ISP_RETURN rsz_start(RSZ_START_T enable);

/* ================================================================ */
/**
 *  Description:- This routine will start /stop the rsz module,
 *                 start is of one shot type or continuous typt
 *  
 *  NOTE:- This routine does not enable the resizer at the top level , it enables only the individual resizers, this should 
               be used along with "rsz_start" routine. 
 *  @param RSZ_START_T  - enumeration of the start / stop commands

 *  @return         ISP_RETURN
*/
/*================================================================= */
/* ===================================================================
 *  @func     rsz_submodule_start                                               
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
ISP_RETURN rsz_submodule_start(RSZ_START_T enable, RESIZER_T resizer);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
