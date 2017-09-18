/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file h3a.h
*
* This file contains the prototypes and definitions to configure h3a 
*
* @path Centaurus\drivers\drv_isp\inc\csl
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! 
*========================================================================= */
#ifndef h3a_HEADER
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define h3a_HEADER

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "h3a_reg.h"

#define H3A_BASE_OFFSET    (0x1400)
#define H3A_BASE_ADDRESS (ISP5_BASE_ADDRESS + H3A_BASE_OFFSET)

typedef volatile CSL_H3aRegs *h3a_regs_ovly;

typedef struct {

    uint8 opened;

} h3a_dev_data_t;

/* ================================================================ */
/**
*Description:-enum to select the RGB positions in the input stream
*/
/*================================================================== */
typedef enum {
    H3A_AF_RGBPOS_GR_GB_BAYER = 0,
    H3A_AF_RGBPOS_RG_GB_BAYER = 1,
    H3A_AF_RGBPOS_GR_BG_BAYER = 2,
    H3A_AF_RGBPOS_RG_BG_BAYER = 3,
    H3A_AF_RGBPOS_GG_RB_CUSTOM = 4,
    H3A_AF_RGBPOS_RB_GG_CUSTOM = 5
} H3A_RGB_POS_T;

/* ================================================================ */
/**
*Description:-enum to enable/disable different features within h3a
*/
/*================================================================== */
typedef enum {
    H3A_FEATURE_DISABLE = 0,
    H3A_FEATURE_ENABLE = 1
} H3A_FEATURE_ENABLE_T;

/* ================================================================ */
/**
*Description:-structure to encapsulate the paxel dimensions and number of paxels in both AF anf AEWB engine
*/
/*================================================================== */
typedef struct {
    uint16 v_pos;                                          // AEWINSTART
                                                           // WINSV
                                                           // //AFPAXSTART
                                                           // PAXSV
    uint16 v_size;                                          // AEWWIN1 WINW
                                                           // //AFPAX1 PAXH
    uint16 h_pos;                                          // AEWINSTART
                                                           // WINSH
                                                           // //AFPAXSTART
                                                           // PAXSH
    uint16 h_size;                                          // AEWWIN1 WINH
                                                           // //AFPAX1 PAXW
    uint8 v_count;                                         // AEWWIN1 WINVC
                                                           // //AFPAX2 PAXVC
    uint8 v_incr;                                          // AEWSUBWIN
                                                           // AEWINCV
                                                           // //AFPAX2 AFINCV
    uint8 h_count;                                         // AEWWIN1 WINHC
                                                           // //AFPAX2 PAXHC
    uint8 h_incr;                                          // AEWSUBWIN
                                                           // AEWINCH
                                                           // //AFPAX2 AFINCH
} h3a_paxel_cfg_t;

/* ================================================================ */
/**
*Description:-enum to select one of the 2 IIR filters for Horizontal Auto Focus
*/
/*================================================================== */
typedef enum {

    H3A_AF_IIR_1 = 0,
    H3A_AF_IIR_2 = 1
} H3A_AF_IIR_T;

/* ================================================================ */
/**
*Description:-enum for selecting the op format of AEWB
*/
/*================================================================== */
typedef enum {
    H3A_AEWB_OP_FMT_SUM_OF_SQR = 0,
    H3A_AEWB_OP_FMT_MINMAX = 1,
    H3A_AEWB_OP_FMT_SUM_ONLY = 2
} H3A_AEWB_OP_FMT_T;

/* ================================================================ */
/**
*Description:- enum to select the FIR filter used in Vertical Focus 
*/
/*================================================================== */
typedef enum {

    H3A_AF_FIR_1 = 0,
    H3A_AF_FIR_2 = 1
} H3A_AF_FIR_T;

/* ================================================================ */
/**
*Description:-structure for encapsulating IIR parameters
*/
/*================================================================== */
typedef struct {
    uint16 iir_coef[11];                                   // AFCOEF010
                                                           // COEFF1
    uint16 hfv_thres;
} h3a_af_iir_param;

/* ================================================================ */
/**
*Description:- structure for encapsulating FIR parameters
*/
/*================================================================== */
typedef struct {
    uint8 fir_coef[5];
    uint16 vfv_thres;
} h3a_af_fir_param;

/* ================================================================ */
/**
*Description:-the Auto Focus Configuration struture
*/
/*================================================================== */
typedef struct {

    H3A_FEATURE_ENABLE_T af_enable;                        // to enable the
                                                           // AF engine
    H3A_RGB_POS_T rgb_pos;                                 // valid only if
                                                           // vertical focus
                                                           // is enabled

    H3A_FEATURE_ENABLE_T peak_mode_en;
    H3A_FEATURE_ENABLE_T vertical_focus_en;

    uint16 iir_start_pos;                                  // AFIIRSH IIRSH

    h3a_paxel_cfg_t *af_paxel_win;

    h3a_af_iir_param *iir1;
    h3a_af_iir_param *iir2;

    h3a_af_fir_param *fir1;
    h3a_af_fir_param *fir2;
    uint32 af_op_addr;                                     // AEWBUFST
                                                           // AEWBUFST, 64
                                                           // bit aligned
                                                           // address

} h3a_af_param_t;

/* ================================================================ */
/**
*Description:-Structure to encapsulate the AEWB Black Window Dimensions
*/
/*================================================================== */
typedef struct {
    uint16 vpos;                                           // AEWINBLK WINSV
                                                           // single row of
                                                           // black line vpos
    uint16 hpos;                                           // AEWINBLK WINH
                                                           // win height
} h3a_aewb_blk_dims_t;

/* ================================================================ */
/**
*Description:-configuration struct for AEWB
*/
/*================================================================== */
typedef struct {

    H3A_FEATURE_ENABLE_T aewb_enable;                      // to enable the
                                                           // Aewb engine

    h3a_paxel_cfg_t *aewb_paxel_win;
    h3a_aewb_blk_dims_t *blk_win_dims;

    H3A_AEWB_OP_FMT_T ae_op_fmt;                           // AEWCFG AEFMT
    uint8 shift_value;                                     // AEWCFG SUMFST
    uint16 saturation_limit;                               // PCR AVE2LMT
    uint32 aewb_op_addr;                                   // AEWBUFST
                                                           // AEWBUFST, 64
                                                           // bit aligned
                                                           // address

} h3a_aewb_param_t;

/* ================================================================ */
/**
*Description:-configuration struct for t he h3a preprocesor block
*/
/*================================================================== */

typedef struct {
    H3A_FEATURE_ENABLE_T af_median_en;
    H3A_FEATURE_ENABLE_T aewb_median_en;

    uint8 median_filter_threshold;                         // valid only if
                                                           // median_en is
                                                           // set to
                                                           // H3A_FEATURE_ENABLE

    H3A_FEATURE_ENABLE_T af_alaw_en;
    H3A_FEATURE_ENABLE_T aewb_alaw_en;
    H3A_FEATURE_ENABLE_T ipiprif_ave_filt_en;
    H3A_FEATURE_ENABLE_T h3a_decim_enable;
    uint32 reserved;
} h3a_common_cfg_t;

/* ================================================================ */
/**
*Description:-Structure to encapsulate the input dimensions to h3a
*/
/*================================================================== */
typedef struct {
    uint16 vpos;
    uint16 hpos;

} h3a_dims_t;

/* ================================================================ */
/**
*Description:-configuration struc for the H3a block , includes common cfg, AF cfg, AEWB cfg
*/
/*================================================================== */
typedef struct {

    h3a_common_cfg_t *h3a_common_cfg;

    h3a_dims_t *h3a_ip_dims;

    h3a_aewb_param_t *aewb_cfg;

    h3a_af_param_t *af_cfg;

} h3a_cfg_t;

/******************************************************************************
*						FUNCTION PROTOTYPES
*******************************************************************************/

/* ================================================================ */
/**
*  Description:-Init should be called before calling any other function 
*  
*
*  @param   none

*  @return     ISP_RETURN    
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_init                                               
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
ISP_RETURN h3a_init();

/* ================================================================ */
/**
*  Description:-open call will set up the CSL register pointers to
*                 appropriate values, register the int handler, enable h3a clk
*  
*
*  @param   none

*  @return      ISP_RETURN   
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_open                                               
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
ISP_RETURN h3a_open();

/* ================================================================ */
/**
*  Description:-close call will de-initialise the CSL register pointers,disable h3a clk
*  
*
*  @param   none

*  @return      ISP_RETURN   
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_close                                               
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
ISP_RETURN h3a_close();

/* ================================================================ */
/**
*  Description:- This routine configures the AEWB black window dimensions
*  
*
*  @param   pointer to h3a_aewb_blk_dims_t struct

*  @return    ISP_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_AEWB_blk_win                                               
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
ISP_RETURN h3a_config_AEWB_blk_win(h3a_aewb_blk_dims_t * cfg);

/* ================================================================ */
/**
*  Description:-This routine configuresthe AE output format
*  
*
*  @param   H3A_AEWB_OP_FMT_T

*  @return    ISP_RETURN     
*/
/*================================================================== */

/* ===================================================================
 *  @func     h3a_config_ae_fmt                                               
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
ISP_RETURN h3a_config_ae_fmt(H3A_AEWB_OP_FMT_T ae_op_fmt);

/* ================================================================ */
/**
*  Description:-this is the main AEWB configuration routine
*  
*
*  @param   pointer to h3a_aewb_param_t struct

*  @return         
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_AEWB                                               
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
ISP_RETURN h3a_config_AEWB(h3a_aewb_param_t * cfg);

/* ================================================================ */
/**
*  Description:-This is the main AF configuration routine
*  
*
*  @param   pointer to h3a_af_param_t struct

*  @return      ISP_RETURN   
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_AF                                               
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
ISP_RETURN h3a_config_AF(h3a_af_param_t * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the input dimensions of H3a 
*  
*
*  @param   :- pointer to "h3a_dims_t" struct

*  @return     :- ISP_RETURN    
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_ip_dims                                               
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
ISP_RETURN h3a_config_ip_dims(h3a_dims_t * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the common "preprocessor" block in H3A
*  
*
*  @param   pointer to "h3a_common_cfg_t" struct

*  @return     ISP_RETURN    
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_common                                               
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
ISP_RETURN h3a_config_common(h3a_common_cfg_t * cfg);

/* ================================================================ */
/**
*  Description:- This is the main configuration call.
*  
*
*  @param   h3a_cfg_t* 

*  @return        ISP_RETURN 
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config                                               
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
ISP_RETURN h3a_config(h3a_cfg_t * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the AF FIR filters
*  
*
*  @param   H3A_AF_FIR_T, specifies the filter number
pointer to "h3a_af_fir_param" struct

*  @return        ISP_RETURN 
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_af_fir                                               
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
ISP_RETURN h3a_config_af_fir(H3A_AF_FIR_T fir_num, h3a_af_fir_param * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the AF IIR filters used in Horizontal focus
*  
*
*  @param   H3A_AF_IIR_T, gives the filter number
h3a_af_iir_param , pointer to cfg struct

*  @return      ISP_RETURN   
*/
/*================================================================== */

/* ===================================================================
 *  @func     h3a_config_af_iir                                               
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
ISP_RETURN h3a_config_af_iir(H3A_AF_IIR_T iir_num, h3a_af_iir_param * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the dimensions and number of AF paxels 
*  
*
*  @param   h3a_paxel_cfg_t, pointer to the configuration structures.

*  @return    ISP_RETURN     
*/
/*================================================================== */

/* ===================================================================
 *  @func     h3a_config_af_paxels                                               
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
ISP_RETURN h3a_config_af_paxels(h3a_paxel_cfg_t * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the AEWB paxel dimensions
*  
*
*  @param   pointer to "h3a_paxel_cfg_t"

*  @return     ISP_RETURN    
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_aewb_paxels                                               
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
ISP_RETURN h3a_config_aewb_paxels(h3a_paxel_cfg_t * cfg);

/* ================================================================ */
/**
*  Description:-This routine configures the AF o.p address
*  
*
*  @param   :- address, is the 64-bit aligned address

*  @return      ISP_RETURN   
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_af_op_addr                                               
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
ISP_RETURN h3a_config_af_op_addr(uint32 address);

/* ================================================================ */
/**
*  Description:-This routine configures theAEWB op address
*  
*
*  @param   :- address,is the 64-bit aligned address

*  @return   ISP_RETURN      
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_config_aewb_op_addr                                               
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
ISP_RETURN h3a_config_aewb_op_addr(uint32 address);

/* ================================================================ */
/**
*  Description:-this Enable/Disable AF H3A
*  
*
*  @param  H3A_FEATURE_ENABLE_T enable :-H3A_FEATURE_DISABLE/H3A_FEATURE_ENABLE

*  @return         
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_af_enable                                               
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
ISP_RETURN h3a_af_enable(H3A_FEATURE_ENABLE_T enable);
/* ================================================================ */
/**
*  Description:-this Enable/Disable AEWB H3A
*  
*
*  @param  H3A_FEATURE_ENABLE_T enable :-H3A_FEATURE_DISABLE/H3A_FEATURE_ENABLE

*  @return         
*/
/*================================================================== */
/* ===================================================================
 *  @func     h3a_aeawb_enable                                               
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
ISP_RETURN h3a_aeawb_enable(H3A_FEATURE_ENABLE_T enable);

/* ================================================================ */
/**
 *  Description:-this checks if AEWB H3A is enabled.
 *  
 *
 *  @param  H3A_FEATURE_ENABLE_T enable :-H3A_FEATURE_DISABLE/H3A_FEATURE_ENABLE

 *  @return         
 */
/*================================================================== */

/* ===================================================================
 *  @func     h3a_aeawb_is_enabled                                               
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
H3A_FEATURE_ENABLE_T h3a_aeawb_is_enabled(void);

/* ================================================================ */
/**
 *  Description:-this checks if AF H3A is enabled.
 *  
 *
 *  @param  H3A_FEATURE_ENABLE_T enable :-H3A_FEATURE_DISABLE/H3A_FEATURE_ENABLE

 *  @return         
 */
/*================================================================== */

/* ===================================================================
 *  @func     h3a_af_is_enabled                                               
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
H3A_FEATURE_ENABLE_T h3a_af_is_enabled(void);

/* ================================================================ */
/**
 *  Description:-this checks if AEWB H3A is busy.
 *  
 *
 *  @param  H3A_FEATURE_ENABLE_T enable :-H3A_FEATURE_DISABLE/H3A_FEATURE_ENABLE

 *  @return         
 */
/*================================================================== */

/* ===================================================================
 *  @func     h3a_aeawb_is_busy                                               
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
H3A_FEATURE_ENABLE_T h3a_aeawb_is_busy(void);

/* ================================================================ */
/**
 *  Description:-this checks if AF H3A is busy.
 *  
 *
 *  @param  H3A_FEATURE_ENABLE_T enable :-H3A_FEATURE_DISABLE/H3A_FEATURE_ENABLE

 *  @return         
 */
/*================================================================== */

/* ===================================================================
 *  @func     h3a_af_is_busy                                               
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
H3A_FEATURE_ENABLE_T h3a_af_is_busy(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
