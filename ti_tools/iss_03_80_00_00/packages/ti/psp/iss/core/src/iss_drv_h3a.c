/** ==================================================================
 *  @file   iss_drv_h3a.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/core/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*******************************************************************
 *
 *    DESCRIPTION:
 *
 *    AUTHOR:
 *
 *    HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <ti/psp/iss/hal/iss/isp/isp.h>
#include "../inc/iss_drv.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

h3a_dims_t h3a_ip_dims_preview;

const H3A_RGB_POS_T h3a_color_pattern[] = {
    H3A_AF_RGBPOS_RG_GB_BAYER,
    H3A_AF_RGBPOS_GR_BG_BAYER,
    H3A_AF_RGBPOS_GR_GB_BAYER,
    H3A_AF_RGBPOS_RG_BG_BAYER
};

/* ===================================================================
 *  @func     h3aGetState                                               
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
ISP_H3A_STATES_T h3aGetState(void)
{
    ISP_H3A_STATES_T result = ISP_H3A_STOPPED;

    if (h3a_aeawb_is_enabled())
    {
        result |= ISP_H3A_AEWB_ENABLED;
    }
    if (h3a_aeawb_is_busy())
    {
        result |= ISP_H3A_AEWB_BUSY;
    }
    if (h3a_af_is_enabled())
    {
        result |= ISP_H3A_AF_ENABLED;
    }
    if (h3a_af_is_busy())
    {
        result |= ISP_H3A_AF_BUSY;
    }

    return (result);
}

/************************* In this H3a params Af  & AWB has been ENabled. AF VERTICAL FOCUS HAS ALSO BEEN ENABLEDother params remain same as in h3a_params_preview_init_1  *********/

/* ===================================================================
 *  @func     h3aParamsInitPreview                                               
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
ISP_RETURN h3aParamsInitPreview(h3a_cfg_t * cfg,
                                iss_config_processing_t * iss_config,
                                void *op_h3a_af_buff, void *op_h3a_aewb_buff)
{

    /* common cfg */
    h3a_ip_dims_preview.hpos = 0;
    h3a_ip_dims_preview.vpos = 0;

    if ((iss_config->f_h3a_validity & PROC_H3A_VALID_AEWB))
    {
        iss_config->f_h3a_validity &= ~PROC_H3A_VALID_AEWB;
        /* aewb setup */
        iss_config->h3a_aewb_params->aewb_op_addr = (uint32) op_h3a_aewb_buff;
        cfg->aewb_cfg = iss_config->h3a_aewb_params;
    }
    else
    {
        cfg->aewb_cfg = NULL;
    }

    if ((iss_config->f_h3a_validity & PROC_H3A_VALID_AF))
    {
        iss_config->f_h3a_validity &= ~PROC_H3A_VALID_AF;
        /* af */
        iss_config->h3a_af_params->af_op_addr = (uint32) op_h3a_af_buff;
        iss_config->h3a_af_params->rgb_pos =
            h3a_color_pattern[iss_config->color_pattern];
        cfg->af_cfg = iss_config->h3a_af_params;
    }
    else
    {
        cfg->af_cfg = NULL;
    }

    if (iss_config->f_h3a_validity & PROC_H3A_VALID_H3A)
    {
        iss_config->f_h3a_validity &= ~PROC_H3A_VALID_H3A;
        cfg->h3a_common_cfg = iss_config->h3a_common_params;
    }
    cfg->h3a_ip_dims = &h3a_ip_dims_preview;

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     updateAewbOpAddr                                               
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
ISP_RETURN updateAewbOpAddr(iss_config_processing_t * iss_config,
                            void *op_h3a_aewb_buff)
{
    iss_config->h3a_aewb_params->aewb_op_addr = (uint32) op_h3a_aewb_buff;
    return ISP_SUCCESS;
}
