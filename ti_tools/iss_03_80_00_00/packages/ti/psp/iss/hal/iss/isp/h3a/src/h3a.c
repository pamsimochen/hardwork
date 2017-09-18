/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file h3a.c
 * this file implements the routines required to configure ipipe.
 * 
 *
 * @path Centaurus\drivers\drv_isp\src\csl
 *
 * @rev 
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 *========================================================================= */

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/

#include "../inc/h3a.h"
#include "../../isp5_utils/isp5_sys_types.h"

#include "../../isp5_utils/isp5_csl_utils.h"
#include "../../common/inc/isp_common.h"

h3a_regs_ovly h3a_reg;

h3a_dev_data_t h3a_dev;

h3a_dev_data_t *h3a_devp;

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
ISP_RETURN h3a_init()
{
    /* This memory can be malloc'ed */
    h3a_devp = &h3a_dev;
    h3a_devp->opened = FALSE;
    return ISP_SUCCESS;
}

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
ISP_RETURN h3a_open()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (h3a_devp->opened == TRUE)
    {

        return ISP_FAILURE;

    }
    h3a_devp->opened = TRUE;

    h3a_reg = (h3a_regs_ovly) H3A_BASE_ADDRESS;

    retval = isp_common_enable_clk(ISP_H3A_CLK);

    if (retval == ISP_SUCCESS)
    {
        // retval=isp_common_set_interrupt_handler(ISP_IPIPE_INT_REG ,
        // ipipe_reg_update_callback,

        // 0,0 );
    }

    return retval;

}

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
ISP_RETURN h3a_close()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (h3a_devp->opened != TRUE)
    {

        return ISP_FAILURE;

    }

    retval = isp_common_disable_clk(ISP_H3A_CLK);

#if 0
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_unset_interrupt_handler(ISP_IPIPE_INT_REG);
    }
#endif

    h3a_reg = NULL;
    h3a_devp->opened = FALSE;

    /* Release the malloced memory ipipe_dev */
    return retval;

}

/* ================================================================ */
/**
 *  Description:- This routine configures the AEWB black window dimensions
 *  
 *
 *  @param   cfg  h3a_aewb_blk_dims_t * cfg :- pointer to h3a_aewb_blk_dims_t struct

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
ISP_RETURN h3a_config_AEWB_blk_win(h3a_aewb_blk_dims_t * cfg)
{

    ISP_SET32(h3a_reg->AEWINBLK, cfg->vpos, CSL_H3A_AEWINBLK_WINSV_SHIFT, CSL_H3A_AEWINBLK_WINSV_MASK); // AEWINBLK 
                                                                                                        // WINSV
    ISP_SET32(h3a_reg->AEWINBLK, cfg->hpos, CSL_H3A_AEWINBLK_WINH_SHIFT, CSL_H3A_AEWINBLK_WINH_MASK);   // AEWINBLK 
                                                                                                        // WINH

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:-This routine configuresthe AE output format
 *  
 *
 *  @param  ae_op_fmt  H3A_AEWB_OP_FMT_T ae_op_fmt

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
ISP_RETURN h3a_config_ae_fmt(H3A_AEWB_OP_FMT_T ae_op_fmt)
{

    ISP_SET32(h3a_reg->AEWCFG, ae_op_fmt, CSL_H3A_AEWCFG_AEFMT_SHIFT,
              CSL_H3A_AEWCFG_AEFMT_MASK);
    return ISP_SUCCESS;
}

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
ISP_RETURN h3a_aeawb_enable(H3A_FEATURE_ENABLE_T enable)
{
    /* Enable / Disable */
    ISP_SET32(h3a_reg->PCR, enable, CSL_H3A_PCR_AEW_EN_SHIFT, CSL_H3A_PCR_AEW_EN_MASK); // PCR 
                                                                                        // AEW_EN
    return ISP_SUCCESS;
}

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
H3A_FEATURE_ENABLE_T h3a_aeawb_is_enabled(void)
{
    return ((H3A_FEATURE_ENABLE_T)
            ISP_FEXT(h3a_reg->PCR, CSL_H3A_PCR_AEW_EN_SHIFT,
                     CSL_H3A_PCR_AEW_EN_MASK));
}

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
H3A_FEATURE_ENABLE_T h3a_aeawb_is_busy(void)
{
    return ((H3A_FEATURE_ENABLE_T)
            ISP_FEXT(h3a_reg->PCR, CSL_H3A_PCR_BUSYAEAWB_SHIFT,
                     CSL_H3A_PCR_BUSYAEAWB_MASK));
}

/* ================================================================ */
/**
 *  Description:-this is the main AEWB configuration routine
 *  
 *
 *  @param  cfg h3a_aewb_param_t * cfg :-pointer to h3a_aewb_param_t struct

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
ISP_RETURN h3a_config_AEWB(h3a_aewb_param_t * cfg)
{

    ISP_RETURN retval = ISP_FAILURE;

    ISP_ASSERT(cfg, NULL);
    retval = h3a_config_aewb_op_addr(cfg->aewb_op_addr);

    if (retval != ISP_FAILURE)
        retval = h3a_config_AEWB_blk_win(cfg->blk_win_dims);

    if (retval != ISP_FAILURE)
        retval = h3a_config_ae_fmt(cfg->ae_op_fmt);

    if (retval != ISP_FAILURE)
        retval = h3a_config_aewb_paxels(cfg->aewb_paxel_win);

    if (retval != ISP_FAILURE)
        ISP_SET32(h3a_reg->PCR, cfg->saturation_limit, CSL_H3A_PCR_AVE2LMT_SHIFT, CSL_H3A_PCR_AVE2LMT_MASK);    // PCR 
                                                                                                                // AVE2LMT

    if (retval != ISP_FAILURE)
        ISP_SET32(h3a_reg->AEWCFG, cfg->shift_value, CSL_H3A_AEWCFG_SUMSFT_SHIFT, CSL_H3A_AEWCFG_SUMSFT_MASK);  // AEWCFG 
                                                                                                                // SUMSFT

    if (retval != ISP_FAILURE)
        h3a_aeawb_enable(cfg->aewb_enable);

    return retval;
}

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
ISP_RETURN h3a_af_enable(H3A_FEATURE_ENABLE_T enable)
{
    /* Enable / Disable */
    ISP_SET32(h3a_reg->PCR, enable, CSL_H3A_PCR_AF_EN_SHIFT, CSL_H3A_PCR_AF_EN_MASK);   // PCR 
                                                                                        // AEW_EN
    return ISP_SUCCESS;

}

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
H3A_FEATURE_ENABLE_T h3a_af_is_enabled(void)
{

    return ((H3A_FEATURE_ENABLE_T)
            ISP_FEXT(h3a_reg->PCR, CSL_H3A_PCR_AF_EN_SHIFT,
                     CSL_H3A_PCR_AF_EN_MASK));

}

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
H3A_FEATURE_ENABLE_T h3a_af_is_busy(void)
{

    return ((H3A_FEATURE_ENABLE_T)
            ISP_FEXT(h3a_reg->PCR, CSL_H3A_PCR_BUSYAF_SHIFT,
                     CSL_H3A_PCR_BUSYAF_MASK));

}

/* ================================================================ */
/**
 *  Description:-This is the main AF configuration routine
 *  
 *
 *  @param   cfg h3a_af_param_t * cfg :- pointer to h3a_af_param_t struct

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
ISP_RETURN h3a_config_AF(h3a_af_param_t * cfg)
{

    ISP_RETURN retval = ISP_SUCCESS;

    ISP_ASSERT(cfg, NULL);

    if (H3A_FEATURE_ENABLE == cfg->af_enable)
    {
        retval = h3a_config_af_paxels(cfg->af_paxel_win);
        if (retval != ISP_FAILURE)
            retval = h3a_config_af_op_addr(cfg->af_op_addr);
        if (retval != ISP_FAILURE)
        {
            ISP_SET32(h3a_reg->PCR, cfg->peak_mode_en, CSL_H3A_PCR_FVMODE_SHIFT, CSL_H3A_PCR_FVMODE_MASK);  // PCR 
                                                                                                            // FVMODE
            ISP_SET32(h3a_reg->PCR, cfg->vertical_focus_en,
                      CSL_H3A_PCR_AF_VF_EN_SHIFT, CSL_H3A_PCR_AF_VF_EN_MASK);
            ISP_SET32(h3a_reg->PCR, cfg->rgb_pos, CSL_H3A_PCR_RGBPOS_SHIFT,
                      CSL_H3A_PCR_RGBPOS_MASK);
        }
        if (retval != ISP_FAILURE)
            retval = h3a_config_af_fir(H3A_AF_FIR_1, cfg->fir1);
        if (retval != ISP_FAILURE)
            retval = h3a_config_af_fir(H3A_AF_FIR_2, cfg->fir2);
        if (retval != ISP_FAILURE)
            retval = h3a_config_af_iir(H3A_AF_IIR_1, cfg->iir1);
        if (retval != ISP_FAILURE)
            retval = h3a_config_af_iir(H3A_AF_IIR_2, cfg->iir2);

        if (retval != ISP_FAILURE)
        {
            ISP_WRITE32(h3a_reg->AFIIRSH, cfg->iir_start_pos);  // CSL_H3A_AFIIRSH_IIRSH_SHIFT, 
                                                                // CSL_H3A_AFIIRSH_IIRSH_MASK); 
                                                                // // AFIIRSH 
                                                                // IIRSH
            // ISP_SET32(h3a_reg->PCR, cfg->af_enable,
            // CSL_H3A_PCR_AF_EN_SHIFT, CSL_H3A_PCR_AF_EN_MASK);
        }
    }
    retval = h3a_af_enable(cfg->af_enable);

    return retval;
}

/* ================================================================ */
/**
 *  Description:-This routine configures the input dimensions of H3a 
 *  
 *
 *  @param   cfg h3a_dims_t * cfg:- pointer to "h3a_dims_t" struct

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
ISP_RETURN h3a_config_ip_dims(h3a_dims_t * cfg)
{

    ISP_ASSERT(cfg, NULL);
    ISP_SET32(h3a_reg->LINE_START, cfg->vpos, CSL_H3A_LINE_START_SLV_SHIFT, CSL_H3A_LINE_START_SLV_MASK);   // LINE_START 
                                                                                                            // SLV
    ISP_SET32(h3a_reg->LINE_START, cfg->hpos, CSL_H3A_LINE_START_LINE_START_SHIFT, CSL_H3A_LINE_START_LINE_START_MASK); // LINE_START 
                                                                                                                        // LINE_START

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:-This routine configures the common "preprocessor" block in H3A
 *  
 *
 *  @param   cfg h3a_common_cfg_t * cfg :- pointer to "h3a_common_cfg_t" struct

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
ISP_RETURN h3a_config_common(h3a_common_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);

    ISP_SET32(h3a_reg->PCR, cfg->af_median_en, CSL_H3A_PCR_AF_MED_EN_SHIFT, CSL_H3A_PCR_AF_MED_EN_MASK);    // PCR 
                                                                                                            // MED_TH

    ISP_SET32(h3a_reg->PCR, cfg->aewb_median_en, CSL_H3A_PCR_AEW_MED_EN_SHIFT, CSL_H3A_PCR_AEW_MED_EN_MASK);    // PCR 
                                                                                                                // MED_TH

    ISP_SET32(h3a_reg->PCR, cfg->af_alaw_en, CSL_H3A_PCR_AF_ALAW_EN_SHIFT, CSL_H3A_PCR_AF_ALAW_EN_MASK);    // PCR 
                                                                                                            // MED_TH

    ISP_SET32(h3a_reg->PCR, cfg->aewb_alaw_en, CSL_H3A_PCR_AEW_ALAW_EN_SHIFT, CSL_H3A_PCR_AEW_ALAW_EN_MASK);    // PCR 
                                                                                                                // MED_TH

    ISP_SET32(h3a_reg->PCR, cfg->median_filter_threshold, CSL_H3A_PCR_MED_TH_SHIFT, CSL_H3A_PCR_MED_TH_MASK);   // PCR 
                                                                                                                // MED_TH

    ISP_WRITE32(h3a_reg->RESERVED, cfg->reserved);         // reserved

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This is the main configuration call.
 *  
 *
 *  @param   cfg h3a_cfg_t * cfg

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
ISP_RETURN h3a_config(h3a_cfg_t * cfg)
{
    ISP_RETURN retval = ISP_SUCCESS;

    ISP_ASSERT(cfg, NULL);

    retval = h3a_config_common(cfg->h3a_common_cfg);

    if (retval == ISP_SUCCESS)
    {
        if (cfg->af_cfg == NULL)
        {
            // ISP_SET32(h3a_reg->PCR, CSL_H3A_PCR_AF_EN_DISABLE,
            // CSL_H3A_PCR_AF_EN_SHIFT, CSL_H3A_PCR_AF_EN_MASK);
        }
        else
        {

            retval = h3a_config_AF(cfg->af_cfg);
        }

        // readonly = ISP_FEXT(h3a_reg->PCR, CSL_H3A_PCR_BUSYAF_SHIFT,
        // CSL_H3A_PCR_BUSYAF_MASK);// PCR BUSYAF

    }

    if (retval == ISP_SUCCESS)
    {
        if (cfg->aewb_cfg == NULL)
        {
            // ISP_SET32(h3a_reg->PCR, CSL_H3A_PCR_AEW_EN_DISABLE,
            // CSL_H3A_PCR_AEW_EN_SHIFT, CSL_H3A_PCR_AEW_EN_MASK);// PCR
            // AEW_EN
        }
        else
        {

            retval = h3a_config_AEWB(cfg->aewb_cfg);
        }

        // readonly = ISP_FEXT(h3a_reg->PCR, CSL_H3A_PCR_BUSYAF_SHIFT,
        // CSL_H3A_PCR_BUSYAF_MASK);// PCR BUSYAF

    }

    if (cfg->h3a_ip_dims != NULL)
    {
        h3a_config_ip_dims(cfg->h3a_ip_dims);
    }

    return retval;
}

/* ================================================================ */
/**
 *  Description:-This routine configures the AF FIR filters
 *  
 *
 *  @param   fir_num  H3A_AF_FIR_T fir_num, specifies the filter number
 *  @param   cfg  h3a_af_fir_param * cfg pointer to "h3a_af_fir_param" struct

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
ISP_RETURN h3a_config_af_fir(H3A_AF_FIR_T fir_num, h3a_af_fir_param * cfg)
{
    uint32 filter_coef = 0;

    if (fir_num == H3A_AF_FIR_1)
    {
        filter_coef =
            ((cfg->fir_coef[0]) + (cfg->fir_coef[1] << 8) +
             (cfg->fir_coef[2] << 16) + (cfg->fir_coef[3] << 24));
        ISP_WRITE32(h3a_reg->VFV_CFG1, filter_coef);

        ISP_SET32(h3a_reg->VFV_CFG2, cfg->fir_coef[4], CSL_H3A_VFV_CFG2_VCOEF1_4_SHIFT, CSL_H3A_VFV_CFG2_VCOEF1_4_MASK);    // VFV_CFG2 
                                                                                                                            // VCOEF1_4
        ISP_SET32(h3a_reg->VFV_CFG2, cfg->vfv_thres, CSL_H3A_VFV_CFG2_VTHR1_SHIFT, CSL_H3A_VFV_CFG2_VTHR1_MASK);    // VFV_CFG2 
                                                                                                                    // VTHR1
    }
    else
    {
        filter_coef =
            ((cfg->fir_coef[0]) + (cfg->fir_coef[1] << 8) +
             (cfg->fir_coef[2] << 16) + (cfg->fir_coef[3] << 24));

        ISP_WRITE32(h3a_reg->VFV_CFG3, filter_coef);
        ISP_SET32(h3a_reg->VFV_CFG4, cfg->fir_coef[4], CSL_H3A_VFV_CFG4_VCOEF2_4_SHIFT, CSL_H3A_VFV_CFG4_VCOEF2_4_MASK);    // VFV_CFG2 
                                                                                                                            // VCOEF2_4
        ISP_SET32(h3a_reg->VFV_CFG4, cfg->vfv_thres, CSL_H3A_VFV_CFG4_VTHR2_SHIFT, CSL_H3A_VFV_CFG4_VTHR2_MASK);    // VFV_CFG2 
                                                                                                                    // VTHR2
    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:-This routine configures the AF IIR filters used in Horizontal focus
 *  
 *
 *  @param   iir_num H3A_AF_IIR_T iir_num, gives the filter number
 *  @param	 cfg h3a_af_iir_param * cfg	, pointer to cfg struct

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
ISP_RETURN h3a_config_af_iir(H3A_AF_IIR_T iir_num, h3a_af_iir_param * cfg)
{

    uint32 filter_coef = 0;

    volatile uint32 *reg_ptr;

    uint8 i = 0;

    ISP_ASSERT(cfg, NULL);
    if (iir_num == H3A_AF_IIR_1)
    {
        reg_ptr = &(h3a_reg->AFCOEF010);

        for (i = 0; i < 5; i++)
        {
            filter_coef =
                ((cfg->iir_coef[2 * i + 1] << 16) + (cfg->iir_coef[2 * i]));

            ISP_WRITE32(*(reg_ptr), filter_coef);
            reg_ptr++;
        }
        /* update the last filter co -efficient */
        ISP_WRITE32(h3a_reg->AFCOEFF0010, cfg->iir_coef[10]);   // CSL_H3A_AFCOEF0010_COEFF10_SHIFT, 
                                                                // CSL_H3A_AFCOEF0010_COEFF10_MASK); 
                                                                // //
                                                                // AFCOEF0010 
                                                                // COEFF10

        /* update the threshold value */
        ISP_SET32(h3a_reg->HFV_THR, cfg->hfv_thres, CSL_H3A_HFV_THR_HTHR1_SHIFT, CSL_H3A_HFV_THR_HTHR1_MASK);   // HFV_THR 
                                                                                                                // HTHR1
    }

    else
    {
        reg_ptr = &(h3a_reg->AFCOEF110);
        for (i = 0; i < 5; i++)
        {
            filter_coef =
                ((cfg->iir_coef[2 * i + 1] << 16) + (cfg->iir_coef[2 * i]));

            ISP_WRITE32(*(reg_ptr), filter_coef);
            reg_ptr++;
        }
        /* update the last filter co -efficient */
        ISP_WRITE32(h3a_reg->AFCOEFF1010, cfg->iir_coef[10]);   // CSL_H3A_AFCOEF1010_COEFF10_SHIFT, 
                                                                // CSL_H3A_AFCOEF1010_COEFF10_MASK); 
                                                                // //
                                                                // AFCOEF1010 
                                                                // COEFF10

        /* update the threshold value */
        ISP_SET32(h3a_reg->HFV_THR, cfg->hfv_thres, CSL_H3A_HFV_THR_HTHR2_SHIFT, CSL_H3A_HFV_THR_HTHR2_MASK);   // HFV_THR 
                                                                                                                // HTHR2

    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:-This routine configures the dimensions and number of AF paxels 
 *  
 *
 *  @param   cfg h3a_paxel_cfg_t * cfg, pointer to the configuration structures.

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
ISP_RETURN h3a_config_af_paxels(h3a_paxel_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);
    ISP_SET32(h3a_reg->AFPAX1, (cfg->h_size / 2) - 1, CSL_H3A_AFPAX1_PAXW_SHIFT, CSL_H3A_AFPAX1_PAXW_MASK); // AFPAX1 
                                                                                                            // PAXW
    ISP_SET32(h3a_reg->AFPAX1, (cfg->v_size / 2) - 1, CSL_H3A_AFPAX1_PAXH_SHIFT, CSL_H3A_AFPAX1_PAXH_MASK); // AFPAX1 
                                                                                                            // PAXH
    ISP_SET32(h3a_reg->AFPAX2, (cfg->h_incr / 2) - 1, CSL_H3A_AFPAX2_AFINCH_SHIFT, CSL_H3A_AFPAX2_AFINCH_MASK); // AFPAX2 
                                                                                                                // AFINCH
    ISP_SET32(h3a_reg->AFPAX2, (cfg->v_incr / 2) - 1, CSL_H3A_AFPAX2_AFINCV_SHIFT, CSL_H3A_AFPAX2_AFINCV_MASK); // AFPAX2 
                                                                                                                // AFINCV
    ISP_SET32(h3a_reg->AFPAX2, cfg->v_count - 1, CSL_H3A_AFPAX2_PAXVC_SHIFT, CSL_H3A_AFPAX2_PAXVC_MASK);    // AFPAX2 
                                                                                                            // PAXVC
    ISP_SET32(h3a_reg->AFPAX2, cfg->h_count - 1, CSL_H3A_AFPAX2_PAXHC_SHIFT, CSL_H3A_AFPAX2_PAXHC_MASK);    // AFPAX2 
                                                                                                            // PAXHC
    ISP_SET32(h3a_reg->AFPAXSTART, cfg->h_pos, CSL_H3A_AFPAXSTART_PAXSH_SHIFT, CSL_H3A_AFPAXSTART_PAXSH_MASK);  // AFPAXSTART 
                                                                                                                // PAXSH
    ISP_SET32(h3a_reg->AFPAXSTART, cfg->v_pos, CSL_H3A_AFPAXSTART_PAXSV_SHIFT, CSL_H3A_AFPAXSTART_PAXSV_MASK);  // AFPAXSTART 
                                                                                                                // PAXSV

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:-This routine configures the AEWB paxel dimensions
 *  
 *
 *  @param   cfg h3a_paxel_cfg_t * cfg :- pointer to the configuration structures.

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
ISP_RETURN h3a_config_aewb_paxels(h3a_paxel_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);
    ISP_SET32(h3a_reg->AEWWIN1, (cfg->v_size / 2) - 1, CSL_H3A_AEWWIN1_WINH_SHIFT, CSL_H3A_AEWWIN1_WINH_MASK);  // AEWWIN1 
                                                                                                                // WINH
    ISP_SET32(h3a_reg->AEWWIN1, (cfg->h_size / 2) - 1, CSL_H3A_AEWWIN1_WINW_SHIFT, CSL_H3A_AEWWIN1_WINW_MASK);  // AEWWIN1 
                                                                                                                // WINW
    ISP_SET32(h3a_reg->AEWWIN1, cfg->v_count - 1, CSL_H3A_AEWWIN1_WINVC_SHIFT, CSL_H3A_AEWWIN1_WINVC_MASK); // AEWWIN1 
                                                                                                            // WINVC
    ISP_SET32(h3a_reg->AEWWIN1, cfg->h_count - 1, CSL_H3A_AEWWIN1_WINHC_SHIFT, CSL_H3A_AEWWIN1_WINHC_MASK); // AEWWIN1 
                                                                                                            // WINHC
    ISP_SET32(h3a_reg->AEWINSTART, cfg->v_pos, CSL_H3A_AEWINSTART_WINSV_SHIFT, CSL_H3A_AEWINSTART_WINSV_MASK);  // AEWINSTART 
                                                                                                                // WINSV
    ISP_SET32(h3a_reg->AEWINSTART, cfg->h_pos, CSL_H3A_AEWINSTART_WINSH_SHIFT, CSL_H3A_AEWINSTART_WINSH_MASK);  // AEWINSTART 
                                                                                                                // WINSH
    ISP_SET32(h3a_reg->AEWSUBWIN, (cfg->v_incr / 2) - 1, CSL_H3A_AEWSUBWIN_AEWINCV_SHIFT, CSL_H3A_AEWSUBWIN_AEWINCV_MASK);  // AEWSUBWIN 
                                                                                                                            // AEWINCV
    ISP_SET32(h3a_reg->AEWSUBWIN, (cfg->h_incr / 2) - 1, CSL_H3A_AEWSUBWIN_AEWINCH_SHIFT, CSL_H3A_AEWSUBWIN_AEWINCH_MASK);  // AEWSUBWIN 
                                                                                                                            // AEWINCH

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:-This routine configures the AF o.p address
 *  
 *
 *  @param   address  uint32 address, is the 64-bit aligned address

 *  @return  ISP_RETURN   
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
ISP_RETURN h3a_config_af_op_addr(uint32 address)
{
    ISP_WRITE32(h3a_reg->AFBUFST, address);                // CSL_H3A_AFBUFST_AFBUFST_SHIFT, 
                                                           // CSL_H3A_AFBUFST_AFBUFST_MASK); 
                                                           // // AFBUFST
                                                           // AFBUFST

    return ISP_SUCCESS;
}

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
ISP_RETURN h3a_config_aewb_op_addr(uint32 address)
{
    ISP_ASSERT(address, NULL);
    // if(address&0x000000)

    /* TODO :- CHECK the alignment 32 or 64? */

    ISP_WRITE32(h3a_reg->AEWBUFST, address);               // CSL_H3A_AEWBUFST_AEWBUFST_SHIFT, 
                                                           // CSL_H3A_AEWBUFST_AEWBUFST_MASK); 
                                                           // // AEWBUFST
                                                           // AEWBUFST

    return ISP_SUCCESS;
}
