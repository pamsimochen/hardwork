/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file ipipe.c
 *     this file implements the routines required to configure ipipe.
 *
 *
 * @path Centaurus\drivers\drv_isp\src\csl
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

// #include "../inc/ipipe.h"
#include "../../isp5_utils/isp5_sys_types.h"

#include "../../isp5_utils/isp5_csl_utils.h"
// #include "../../common/inc/isp_common.h"

#include "../../isp.h"

ipipe_regs_ovly ipipe_reg;

ipipe_dev_data_t *ipipe_devp;

ipipe_dev_data_t ipipe_dev;

/*******************************************************
* STATIC FUNCTIONS
********************************************************/

/* ===================================================================
 *  @func     ipipe_sub_module_clk_enable                                               
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
static ISP_RETURN ipipe_sub_module_clk_enable(ipipe_module_clk clk_module,
                                              IPIPE_MODULE_CLK_CTRL_T enable);

/* ===================================================================
 *  @func     ipipe_clk_reset                                               
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
static ISP_RETURN ipipe_clk_reset();

/* ================================================================ */
/**
 *  ipipe_init()
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
ISP_RETURN ipipe_init()
{

    /* This memory ipipe_dev can be malloc'ed */
    ipipe_devp = &ipipe_dev;

    ipipe_devp->opened = FALSE;

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 * ipipe_open()
 * Description :- open call will set up the CSL register pointers to
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
ISP_RETURN ipipe_open()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (ipipe_devp->opened == TRUE)
    {

        return ISP_FAILURE;

    }
    ipipe_devp->opened = TRUE;

    ipipe_reg = (ipipe_regs_ovly) IPIPE_BASE_ADDRESS;

    retval = isp_common_enable_clk(ISP_IPIPE_CLK);

    if (retval == ISP_SUCCESS)
    {
        // retval=isp_common_set_interrupt_handler(ISP_IPIPE_INT_REG ,
        // ipipe_reg_update_callback,
        // 0,0 );
    }

    /* enable register access clk's */
    ISP_SET32(ipipe_reg->GCK_MMR, CSL_IPIPE_GCK_MMR_REG_ON,
              CSL_IPIPE_GCK_MMR_REG_SHIFT, CSL_IPIPE_GCK_MMR_REG_MASK);

    if (retval == ISP_SUCCESS)
    {
        retval = ipipe_clk_reset();

    }
    return retval;

}

/* ================================================================ */
/**
 * ipipe_close()
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
ISP_RETURN ipipe_close()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (ipipe_devp->opened != TRUE)
    {

        return ISP_FAILURE;

    }

#if 0
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_unset_interrupt_handler(ISP_IPIPE_INT_REG);
    }
#endif

    ipipe_clk_reset();

    retval = isp_common_disable_clk(ISP_IPIPE_CLK);

    /* Disable register access clks */
    // ISP_SET32(ipipe_reg->GCK_MMR,CSL_IPIPE_GCK_MMR_REG_OFF,CSL_IPIPE_GCK_MMR_REG_SHIFT,CSL_IPIPE_GCK_MMR_REG_MASK);

    ipipe_reg = NULL;
    ipipe_devp->opened = FALSE;

    /* Release the malloced memory ipipe_dev */
    return retval;

}

/* ================================================================ */
/**
 *  ipipe_validate_params()
 *  Description:- all error checking can be done in this function during
 *                inital configuration, this is a place holder currently and needs to completed appropriately
 *   NOTE!!!this is a place holder to to param check if needed
 *  @param   ipipe_config 	ipipe_cfg_t * ipipe_config

 *  @return    ISP_RETURN
 */
/*================================================================== */
/* ===================================================================
 *  @func     ipipe_validate_params                                               
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
ISP_RETURN ipipe_validate_params(ipipe_cfg_t * ipipe_config)
{

    return ISP_SUCCESS;

}

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
ISP_RETURN ipipe_config_filters(ipipe_cfg_t * ipipe_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    if (((ipipe_config->filter_flag) & IPIPE_LUT_DPC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_DPC_lut(ipipe_config->dpc_lut_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_OTF_DPC_FLAG) &&
        (retval != ISP_FAILURE))
    {

        retval = ipipe_config_DPC_otf(ipipe_config->dpc_otf_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_NOISE_FILTER_1_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            ipipe_config_noise_filter(NOISE_FILTER_1, ipipe_config->nf1_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_NOISE_FILTER_2_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            ipipe_config_noise_filter(NOISE_FILTER_2, ipipe_config->nf2_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_GIC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_gic_filter(ipipe_config->gic_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_WB_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_wbal(ipipe_config->wb_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_CFA_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_cfa(ipipe_config->cfa_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_RGB_RGB_1_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            ipipe_config_rgb_to_rgb(IPIPE_RGB_RGB_FILTER_1,
                                    ipipe_config->rgb_rgb1_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_GAMMA_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_gamma(ipipe_config->gamma_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_RGB_RGB_2_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            ipipe_config_rgb_to_rgb(IPIPE_RGB_RGB_FILTER_2,
                                    ipipe_config->rgb_rgb2_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_3D_LUT_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_3d_lut(ipipe_config->ipipe_3d_lut_params);
    }

    if (((ipipe_config->filter_flag) & IPIPE_RGB_TO_YUV_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_rgb2ycbcr(ipipe_config->rgb_yuv_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_YUV444_YUV422_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            ipipe_config_yuv444_to_yuv422(ipipe_config->yuv444_yuv422_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_GBCE_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_GBCE(ipipe_config->gbce_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_EE_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_edge_enhancement(ipipe_config->ee_param);
    }

    if (((ipipe_config->filter_flag) & IPIPE_CAR_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_car(ipipe_config->car_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_CGS_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_chroma_supression(ipipe_config->cgs_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_LSC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_lsc(ipipe_config->lsc_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_BOXCAR_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_boxcar(ipipe_config->boxcar_params);

    }

    if (((ipipe_config->filter_flag) & IPIPE_HST_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_histogram(ipipe_config->histogram_params);
    }

    if (((ipipe_config->filter_flag) & IPIPE_BSC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = ipipe_config_bsc(ipipe_config->bsc_params);
    }

    return retval;
}

/* ================================================================ */
/**
 * ipipe_config()
 *  Description:- this is the main cfg call, it sets up all the required filters,
 *                ip/op pix types.
 *
 *
 *  @param ipipe_config 	 ipipe_cfg_t * ipipe_config

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
ISP_RETURN ipipe_config(ipipe_cfg_t * ipipe_config)
{

    ISP_RETURN retval = ISP_SUCCESS;

    retval = ipipe_validate_params(ipipe_config);

    if ((retval != ISP_FAILURE))
    {
        retval = ipipe_config_input_src(ipipe_config->src_cfg);

    }

    if ((retval != ISP_FAILURE))
    {
        retval = ipipe_config_filters(ipipe_config);
    }
    return retval;
}

/* ================================================================ */
/**
 *  ipipe_start()
 *  Description:- This routine will start /stop the ipipe module,
 *                 start is of one shot type or continuous typt
 *
 *
 *  @param enable	IPIPE_START_T enable - enumeration of the start / stop commands

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
ISP_RETURN ipipe_start(IPIPE_START_T enable)
{

    int status = ISP_SUCCESS;

    switch (enable)
    {
        case IPIPE_STOP:
            ISP_SET32(ipipe_reg->SRC_MODE, CSL_IPIPE_SRC_MODE_OST_DISABLE,
                      CSL_IPIPE_SRC_MODE_OST_SHIFT,
                      CSL_IPIPE_SRC_MODE_OST_MASK);
            ISP_SET32(ipipe_reg->SRC_EN, CSL_IPIPE_SRC_EN_EN_DISABLE,
                      CSL_IPIPE_SRC_EN_EN_SHIFT, CSL_IPIPE_SRC_EN_EN_MASK);
            break;
        case IPIPE_RUN:
            ISP_SET32(ipipe_reg->SRC_MODE, CSL_IPIPE_SRC_MODE_OST_DISABLE,
                      CSL_IPIPE_SRC_MODE_OST_SHIFT,
                      CSL_IPIPE_SRC_MODE_OST_MASK);
            ISP_SET32(ipipe_reg->SRC_EN, CSL_IPIPE_SRC_EN_EN_ENABLE,
                      CSL_IPIPE_SRC_EN_EN_SHIFT, CSL_IPIPE_SRC_EN_EN_MASK);
            break;
        case IPIPE_SINGLE:
            ISP_SET32(ipipe_reg->SRC_MODE, CSL_IPIPE_SRC_MODE_OST_ENABLE,
                      CSL_IPIPE_SRC_MODE_OST_SHIFT,
                      CSL_IPIPE_SRC_MODE_OST_MASK);
            ISP_SET32(ipipe_reg->SRC_EN, CSL_IPIPE_SRC_EN_EN_ENABLE,
                      CSL_IPIPE_SRC_EN_EN_SHIFT, CSL_IPIPE_SRC_EN_EN_MASK);
            break;
        default:
            status = ISP_INVALID_INPUT;                    // incorrect input 
                                                           // parameter
    }

    return (status);

}

/* ================================================================ */
/**
 *  ipipe_config_DPC_lut()
 *  Description:-This routine configures the LUT Defect Pixel correction
 *                 block .
 *
 *  @param   cfg	ipipe_dpc_lut_cfg_t * cfg -pointer to cfg params

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
ISP_RETURN ipipe_config_DPC_lut(ipipe_dpc_lut_cfg_t * cfg)
{
    if (cfg == NULL)
    {
        return ISP_FAILURE;
    }
    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_DPC_LUT_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_WRITE32(ipipe_reg->DPC_LUT_EN, cfg->enable);
    ISP_SET32(ipipe_reg->DPC_LUT_SEL, cfg->table_type,
              CSL_IPIPE_DPC_LUT_SEL_TBL_SHIFT, CSL_IPIPE_DPC_LUT_SEL_TBL_MASK);
    ISP_SET32(ipipe_reg->DPC_LUT_SEL, cfg->replace_type,
              CSL_IPIPE_DPC_LUT_SEL_DOT_SHIFT, CSL_IPIPE_DPC_LUT_SEL_DOT_MASK);
    ISP_WRITE32(ipipe_reg->DPC_LUT_ADR, cfg->lut_valid_addr);
    ISP_WRITE32(ipipe_reg->DPC_LUT_SIZ, cfg->lut_size - 1);

    /* Update the table also */
    /* TODO********************* */

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 * ipipe_config_DPC_lut()
 * Description:-This routine configures the ipipe On-the fly Defect pixel correction module
 *
 *
 *  @param   cfg ipipe_dpc_otf_cfg_t *  cfg -pointer to cfg params

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
ISP_RETURN ipipe_config_DPC_otf(ipipe_dpc_otf_cfg_t * cfg)
{

    int i = 0;

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_DPC_OTF_CLK, IPIPE_MODULE_CLK_ENABLE);

    /* set the interrupt handler for updating the lookup table
     * "isp_common_set_interrupt_handler" */
    ISP_WRITE32(ipipe_reg->DPC_OTF_EN, cfg->enable);
    ISP_SET32(ipipe_reg->DPC_OTF_TYP, cfg->type,
              CSL_IPIPE_DPC_OTF_TYP_TYP_SHIFT, CSL_IPIPE_DPC_OTF_TYP_TYP_MASK);
    ISP_SET32(ipipe_reg->DPC_OTF_TYP, cfg->algo,
              CSL_IPIPE_DPC_OTF_TYP_ALG_SHIFT, CSL_IPIPE_DPC_OTF_TYP_ALG_MASK);

    if (cfg->algo == IPIPE_DPC_OTF_ALG_MINMAX2)
    {
        uint32 *reg = (uint32 *) & ipipe_reg->DPC_OTF_2_D_THR_R;

        uint16 *dat_ptr = cfg->dpc_data.dpc2_params.thr_d;

        ISP_ASSERT(dat_ptr, NULL);
        for (i = 0; i < 4; i++)                            /* update D-table */
        {
            *reg = (uint32) * dat_ptr;
            reg++;
            dat_ptr++;
        }

        reg = (uint32 *) & ipipe_reg->DPC_OTF_2_C_THR_R;
        dat_ptr = cfg->dpc_data.dpc2_params.thr_c;

        ISP_ASSERT(dat_ptr, NULL);
        for (i = 0; i < 4; i++)                            /* update c-table */
        {
            *reg = (uint32) * dat_ptr;
            reg++;
            dat_ptr++;
        }

    }

    if (cfg->algo == IPIPE_DPC_OTF_ALG_MINMAX3)
    {

        ISP_WRITE32(ipipe_reg->DPC_OTF_3_SHF, cfg->dpc_data.dpc3_params.shift);

        ISP_WRITE32(ipipe_reg->DPC_OTF_3_D_THR, cfg->dpc_data.dpc3_params.dett.d_thr);
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_D_SLP, cfg->dpc_data.dpc3_params.dett.d_slp);
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_D_MIN, cfg->dpc_data.dpc3_params.dett.d_min);
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_D_MAX, cfg->dpc_data.dpc3_params.dett.d_max);
		
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_C_THR, cfg->dpc_data.dpc3_params.corr.d_thr);
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_C_SLP, cfg->dpc_data.dpc3_params.corr.d_slp);
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_C_MIN, cfg->dpc_data.dpc3_params.corr.d_min);
		ISP_WRITE32(ipipe_reg->DPC_OTF_3_C_MAX, cfg->dpc_data.dpc3_params.corr.d_max);

        /* 
         * ISP_SET32(IPIPE_DPC_OTF_3_D_THR,cfg->dpc_data.dpc3_params.otf3d.thr,CSL_IPIPE_DPC_OTF_3_D_THR_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_THR_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_D_SPL,cfg->dpc_data.dpc3_params.otf3d.d_slp,CSL_IPIPE_DPC_OTF_3_D_SPL_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_SPL_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_D_MAX,cfg->dpc_data.dpc3_params.otf3d.d_max,CSL_IPIPE_DPC_OTF_3_D_MAX_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_MAX_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_D_MIN,cfg->dpc_data.dpc3_params.otf3d.d_min,CSL_IPIPE_DPC_OTF_3_D_MIN_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_MAX_VAL_MASK);
         * 
         * ISP_SET32(IPIPE_DPC_OTF_3_C_THR,cfg->dpc_data.dpc3_params.otf3d.thr,CSL_IPIPE_DPC_OTF_3_C_THR_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_THR_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_C_SLP,cfg->dpc_data.dpc3_params.otf3d.d_slp,CSL_IPIPE_DPC_OTF_3_C_SPL_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_SPL_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_C_MAX,cfg->dpc_data.dpc3_params.otf3d.d_max,CSL_IPIPE_DPC_OTF_3_C_MAX_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_MAX_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_C_MIN,cfg->dpc_data.dpc3_params.otf3d.d_min,CSL_IPIPE_DPC_OTF_3_C_MIN_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_MAX_VAL_MASK);
         * 
         */
    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_noise_filter()
 *  Description:-This routine configures one of the 2 the noise filters specified by "noise filter",
 *			   with the params specified in "cfg"
 *
 *
 *  @param noise_filter   IPIPE_NOISE_FILTER_T  noise_filter :- specifies the noise filter
 *  @param cfg	 ipipe_noise_filter_cfg_t * cfg :-pointer to  the cfg of the noise filter
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
                                     ipipe_noise_filter_cfg_t * cfg)
{

    volatile uint32 *reg = 0;

    uint8 i = 0;

    uint32 typ_reg_val = 0;

    // ipipe_module_clk module_clk=0; /*########## Declared but never used
    // ###################*/

    uint16 *thr = 0;

    uint8 *str = 0;

    uint8 *spr = 0;

    ISP_ASSERT(cfg, NULL);

    /* Check this */
    if (noise_filter == NOISE_FILTER_1)
        reg = &(ipipe_reg->D2F_1ST_EN);
    else if (noise_filter == NOISE_FILTER_2)
        reg = &(ipipe_reg->D2F_2ND_EN);
    else
        return ISP_FAILURE;

    // module_clk=(noise_filter==NOISE_FILTER_1)?(IPIPE_NOISE_FILTER_1_CLK):(IPIPE_NOISE_FILTER_2_CLK);
    // pipe_sub_module_clk_enable(module_clk,IPIPE_MODULE_CLK_ENABLE );
    /* Clk for both NF1 and NF2 is same */
    ipipe_sub_module_clk_enable(IPIPE_NOISE_FILTER_1_CLK,
                                IPIPE_MODULE_CLK_ENABLE);

    ISP_WRITE32(*reg, cfg->enable);

    /* Use local variable to make it fast. */

    typ_reg_val = ISP_READ32(reg);

    ISP_SET32(typ_reg_val, cfg->sel, CSL_IPIPE_D2F_1ST_TYP_SEL_SHIFT,
              CSL_IPIPE_D2F_1ST_TYP_SEL_MASK);
    ISP_SET32(typ_reg_val, cfg->lsc_gain, CSL_IPIPE_D2F_1ST_TYP_LSC_SHIFT,
              CSL_IPIPE_D2F_1ST_TYP_LSC_MASK);
    ISP_SET32(typ_reg_val, cfg->typ, CSL_IPIPE_D2F_1ST_TYP_TYP_SHIFT,
              CSL_IPIPE_D2F_1ST_TYP_TYP_MASK);
    ISP_SET32(typ_reg_val, cfg->down_shift_val, CSL_IPIPE_D2F_1ST_TYP_SHF_SHIFT,
              CSL_IPIPE_D2F_1ST_TYP_SHF_MASK);
    ISP_SET32(typ_reg_val, cfg->spread, CSL_IPIPE_D2F_1ST_TYP_SPR_SHIFT,
              CSL_IPIPE_D2F_1ST_TYP_SPR_MASK);

    /* Now commit to IPIPE, regaddr is pointing to EN, TYP is next register
     * so add4 */

    ISP_WRITE32(*(reg + 1), typ_reg_val);

    /* Set filter thresholds */

    reg =
        (volatile uint32 *) (noise_filter ==
                             NOISE_FILTER_2) ? ((ipipe_reg->
                                                 D2F_2ND_THR)) : ((ipipe_reg->
                                                                   D2F_1ST_THR));
    thr = cfg->thr;
    ISP_ASSERT(thr, NULL);
    for (i = 0; i < 8; i++)
    {
        ISP_WRITE32(*reg, *thr);
        reg++;                                             /* increases by 32 
                                                            * bits */
        thr++;                                             /* increases by 16 
                                                            * bit */

    }

    /* Set filter str */
    reg =
        (volatile uint32 *) (noise_filter ==
                             NOISE_FILTER_2) ? (ipipe_reg->
                                                D2F_2ND_STR) : (ipipe_reg->
                                                                D2F_1ST_STR);
    str = cfg->str;
    ISP_ASSERT(str, NULL);

    for (i = 0; i < 8; i++)
    {
        ISP_WRITE32(*reg, *str);
        reg++;                                             /* increases by 32 
                                                            * bits */
        str++;                                             /* increases by 8
                                                            * bit */

    }

    /* Set filter spread */
    reg =
        (volatile uint32 *) (noise_filter ==
                             NOISE_FILTER_2) ? (ipipe_reg->
                                                D2F_2ND_SPR) : (ipipe_reg->
                                                                D2F_1ST_SPR);
    spr = cfg->spr;
    ISP_ASSERT(spr, NULL);
    for (i = 0; i < 8; i++)
    {
        ISP_WRITE32(*reg, *spr);
        reg++;                                             /* increases by 32 
                                                            * bits */
        spr++;                                             /* increases by 8
                                                            * bit */

    }

    /**SET edge max and min*/

    reg =
        (volatile uint32 *) ((noise_filter == NOISE_FILTER_2)
                             ? (&ipipe_reg->D2F_2ND_EDG_MAX) : (&ipipe_reg->
                                                                D2F_1ST_EDG_MAX));

    ISP_WRITE32(*reg, cfg->edge_max);

    reg =
        (volatile uint32 *) ((noise_filter == NOISE_FILTER_2)
                             ? (&ipipe_reg->D2F_2ND_EDG_MIN) : (&ipipe_reg->
                                                                D2F_1ST_EDG_MIN));

    ISP_WRITE32(*reg, cfg->edge_min);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_config_gic_filter()
 *  Description:-This routine configures the Green Imbalance Correction module ipipe
 *
 *
 *  @param   cfg  ipipe_gic_cfg_t * cfg-pointer to cfg params
 *
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
ISP_RETURN ipipe_config_gic_filter(ipipe_gic_cfg_t * cfg)
{

    uint32 val = 0;

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_GIC_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_WRITE32(ipipe_reg->GIC_EN, cfg->enable);

    val = ipipe_reg->GIC_TYP;

    ISP_SET32(val, cfg->typ, CSL_IPIPE_GIC_TYP_TYP_SHIFT,
              CSL_IPIPE_GIC_TYP_TYP_MASK);
    ISP_SET32(val, cfg->lsc_gain, CSL_IPIPE_GIC_TYP_LSC_SHIFT,
              CSL_IPIPE_GIC_TYP_LSC_MASK);
    ISP_SET32(val, cfg->sel, CSL_IPIPE_GIC_TYP_SEL_SHIFT,
              CSL_IPIPE_GIC_TYP_SEL_MASK);

    ISP_WRITE32(ipipe_reg->GIC_TYP, val);

    ISP_WRITE32(ipipe_reg->GIC_GAN, cfg->gic_gain);
    ISP_WRITE32(ipipe_reg->GIC_NFGAN, cfg->gic_nfgain);

    ISP_WRITE32(ipipe_reg->GIC_THR, cfg->gic_thr);

    ISP_WRITE32(ipipe_reg->GIC_SLP, cfg->gic_slope);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_wbal()
 *  Description:-This routine configures the White Balance module in ipipe
 *
 *
 *  @param   cfg   ipipe_wb_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_wbal(ipipe_wb_cfg_t * cfg)
{

    uint16 *val_ptr = NULL;

    volatile uint32 *reg_ptr = (volatile uint32 *) &(ipipe_reg->WB2_OFT_R);

    ISP_ASSERT(cfg, NULL);
    val_ptr = cfg->offset;

    ISP_ASSERT(val_ptr, NULL);

    ipipe_sub_module_clk_enable(IPIPE_GIC_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_SET32(*reg_ptr, *(val_ptr), CSL_IPIPE_WB2_OFT_R_VAL_SHIFT,
              CSL_IPIPE_WB2_OFT_R_VAL_MASK);
    ISP_SET32(*(reg_ptr+1), *(val_ptr + 1), CSL_IPIPE_WB2_OFT_GR_VAL_SHIFT,
              CSL_IPIPE_WB2_OFT_GR_VAL_MASK);
    ISP_SET32(*(reg_ptr+2), *(val_ptr + 2), CSL_IPIPE_WB2_OFT_GB_VAL_SHIFT,
              CSL_IPIPE_WB2_OFT_GB_VAL_MASK);
    ISP_SET32(*(reg_ptr+3), *(val_ptr + 3), CSL_IPIPE_WB2_OFT_B_VAL_SHIFT,
              CSL_IPIPE_WB2_OFT_B_VAL_MASK);

    val_ptr = cfg->gain;
    ISP_ASSERT(val_ptr, NULL);
    reg_ptr = (volatile uint32 *) &(ipipe_reg->WB2_WGN_R);

    ISP_SET32(*reg_ptr, *(val_ptr), CSL_IPIPE_WB2_WGN_R_VAL_SHIFT,
              CSL_IPIPE_WB2_WGN_R_VAL_MASK);
    ISP_SET32(*(reg_ptr+1), *(val_ptr + 1), CSL_IPIPE_WB2_WGN_GR_VAL_SHIFT,
              CSL_IPIPE_WB2_WGN_GR_VAL_MASK);
    ISP_SET32(*(reg_ptr+2), *(val_ptr + 2), CSL_IPIPE_WB2_WGN_GB_VAL_SHIFT,
              CSL_IPIPE_WB2_WGN_GB_VAL_MASK);
    ISP_SET32(*(reg_ptr+3), *(val_ptr + 3), CSL_IPIPE_WB2_WGN_B_VAL_SHIFT,
              CSL_IPIPE_WB2_WGN_B_VAL_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_cfa()
 *  Description:-This routine configures the CFA module in ipipe
 *
 *
 *  @param    cfg  ipipe_cfa_cfg_t * cfg -pointer to cfg params
 *
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
ISP_RETURN ipipe_config_cfa(ipipe_cfa_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_CFA_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_SET32(ipipe_reg->CFA_MODE, cfg->mode, CSL_IPIPE_CFA_MODE_MODE_SHIFT,
              CSL_IPIPE_CFA_MODE_MODE_MASK);

    ISP_WRITE32(ipipe_reg->CFA_2DIR_HPF_THR, cfg->dir.hpf_thr);
    ISP_WRITE32(ipipe_reg->CFA_2DIR_HPF_SLP, cfg->dir.hpf_slope);
    ISP_WRITE32(ipipe_reg->CFA_2DIR_MIX_THR, cfg->dir.mix_thr);
    ISP_WRITE32(ipipe_reg->CFA_2DIR_MIX_SLP, cfg->dir.mix_slope);
    ISP_WRITE32(ipipe_reg->CFA_2DIR_DIR_THR, cfg->dir.dir_thr);
    ISP_WRITE32(ipipe_reg->CFA_2DIR_DIR_SLP, cfg->dir.dir_slope);
    ISP_WRITE32(ipipe_reg->CFA_2DIR_NDWT, cfg->dir.dir_ndwt);
    ISP_WRITE32(ipipe_reg->CFA_MONO_HUE_FRA, cfg->daa.mono_hue_fra);
    ISP_WRITE32(ipipe_reg->CFA_MONO_EDG_THR, cfg->daa.mono_edg_thr);
    ISP_WRITE32(ipipe_reg->CFA_MONO_THR_MIN, cfg->daa.mono_thr_min);
    ISP_WRITE32(ipipe_reg->CFA_MONO_THR_SLP, cfg->daa.mono_thr_slope);
    ISP_WRITE32(ipipe_reg->CFA_MONO_SLP_MIN, cfg->daa.mono_slp_min);
    ISP_WRITE32(ipipe_reg->CFA_MONO_SLP_SLP, cfg->daa.mono_slp_slp);
    ISP_WRITE32(ipipe_reg->CFA_MONO_LPWT, cfg->daa.mono_lpwt);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_rgb_to_rgb()
 *  Description:-This routine configures one of the two  RGB to RGB blending filters in ipipe,
 *
 *
 *  @param    filter_num   IPIPE_RGB_RGB_FILTER_NUM_T  filter_num - pointer to cfg params
 *  @param    cfg   ipipe_rgb_rgb_cfg_t * cfg - specifies the RGB to RGB blending filters number
 *
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
                                   ipipe_rgb_rgb_cfg_t * cfg)
{

    int16 *val_adr = NULL;

    int i = 0;

    volatile uint32 *reg_adr = 0;

    // ipipe_module_clk module_clk=0; /*########## Declared but never used
    // ###################*/

    ISP_ASSERT(cfg, NULL);

    val_adr = cfg->mul_off;

    ISP_ASSERT(val_adr, NULL);

    ipipe_sub_module_clk_enable(IPIPE_RGB_RGB_CLK, IPIPE_MODULE_CLK_ENABLE);

    reg_adr =
        (volatile uint32 *) ((filter_num == IPIPE_RGB_RGB_FILTER_1)
                             ? (&ipipe_reg->RGB1_MUL_RR) : (&ipipe_reg->
                                                            RGB2_MUL_RR));

    /* Set the matrix co- efficients */
    for (i = 0; i < 9; i++)
    {
        // ISP_WRITE32(*(reg_adr+i),*(val_adr+i));
        *(reg_adr + i) = *(val_adr + i);

    }

    /* Set the offsets */
    val_adr = cfg->oft;
    ISP_ASSERT(val_adr, NULL);
    reg_adr =
        (volatile uint32 *) ((filter_num == IPIPE_RGB_RGB_FILTER_1)
                             ? (&ipipe_reg->RGB1_OFT_OR) : (&ipipe_reg->
                                                            RGB2_OFT_OR));

    for (i = 0; i < 3; i++)
    {
        // ISP_WRITE32(*(reg_adr+i),*(val_adr+i));
        *(reg_adr + i) = *(val_adr + i);
    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_gamma()
 *  Description:-This routine configures the Gamma correction filter in ipipe
 *
 *
 *  @param   cfg  ipipe_gamma_cfg_t * cfg :-pointer to cfg params
 *
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
ISP_RETURN ipipe_config_gamma(ipipe_gamma_cfg_t * cfg)
{

    uint32 reg_val = ipipe_reg->GMM_CFG;

    uint32 table_size = 0;

    uint32 *table_red_mem, *table_green_mem, *table_blue_mem;

    int16 *red_table, *green_table, *blue_table;

    uint32 dummy = 0, i = 0;

    ISP_ASSERT(cfg, NULL);

    red_table = cfg->red_table;

    green_table = cfg->green_table;

    blue_table = cfg->blue_table;

    ISP_ASSERT(red_table, NULL);
    ISP_ASSERT(green_table, NULL);
    ISP_ASSERT(blue_table, NULL);

    ipipe_sub_module_clk_enable(IPIPE_GAMMA_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_SET32(reg_val, cfg->tbl, CSL_IPIPE_GMM_CFG_TBL_SHIFT,
              CSL_IPIPE_GMM_CFG_TBL_MASK);

    ISP_SET32(reg_val, cfg->gamma_tbl_size, CSL_IPIPE_GMM_CFG_SIZ_SHIFT,
              CSL_IPIPE_GMM_CFG_SIZ_MASK);

    ISP_SET32(reg_val, cfg->bypass_b, CSL_IPIPE_GMM_CFG_BYPB_SHIFT,
              CSL_IPIPE_GMM_CFG_BYPB_MASK);

    ISP_SET32(reg_val, cfg->bypass_r, CSL_IPIPE_GMM_CFG_BYPR_SHIFT,
              CSL_IPIPE_GMM_CFG_BYPR_MASK);

    ISP_SET32(reg_val, cfg->bypass_g, CSL_IPIPE_GMM_CFG_BYPG_SHIFT,
              CSL_IPIPE_GMM_CFG_BYPG_MASK);

    ISP_WRITE32(ipipe_reg->GMM_CFG, reg_val);

    /* Point to the respective gamma table locations */

    table_red_mem = (uint32 *) IPIPE_GAMRtable;

    table_green_mem = (uint32 *) IPIPE_GAMGtable;

    table_blue_mem = (uint32 *) IPIPE_GAMBtable;

    if (cfg->gamma_tbl_size == IPIPE_GAMMA_TBL_512)
        table_size = 512;
    if (cfg->gamma_tbl_size == IPIPE_GAMMA_TBL_128)
        table_size = 128;
    if (cfg->gamma_tbl_size == IPIPE_GAMMA_TBL_256)
        table_size = 256;
    if (cfg->gamma_tbl_size == IPIPE_GAMMA_TBL_64)
        table_size = 64;

    for (i = 0; i < table_size; i++)

    {

        dummy = (((*red_table) & 0x03FF) << 10) | ((*(red_table + 1)) & 0x03FF);

        ISP_WRITE32(*table_red_mem, dummy);

        dummy =
            (((*blue_table) & 0x03FF) << 10) | ((*(blue_table + 1)) & 0x03FF);

        ISP_WRITE32(*table_blue_mem, dummy);

        dummy =
            (((*green_table) & 0x03FF) << 10) | ((*(green_table + 1)) & 0x03FF);

        ISP_WRITE32(*table_green_mem, dummy);

        table_red_mem++;
        red_table += 2;

        table_green_mem++;
        green_table += 2;

        table_blue_mem++;
        blue_table += 2;

    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_config_3d_lut()
 *  Description:-This routine configures the 3D Look up Table based correction module in ipipe
 *
 *
 *  @param  cfg  ipipe_3d_lut_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_3d_lut(ipipe_3d_lut_cfg_t * cfg)
{
    uint32 i;

    ISP_RETURN retVal = ISP_SUCCESS;

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_3D_LUT_CLK, IPIPE_MODULE_CLK_ENABLE);

#ifndef LUTTBL16BIT
    {
        uint32 *table_adr = (uint32 *) cfg->table_adr;

        uint32 *plut_addr;

        uint32 lut_addr;

        retVal = ipipe_get_3dlut(&lut_addr, 0 /* T0 */ );
        ISP_ASSERT(lut_addr, NULL);
        plut_addr = (uint32 *) lut_addr;
        for (i = 0; i < (192 * 4); i++)
        {
            *(plut_addr + ((i % 4) * (1024 / 4)) + (i / 4)) = (uint32) (*(table_adr++));    // banks 
                                                                                            // 0,1,2,3
        }
    }
#else
    {
        uint16 *table_adr = (uint16 *) cfg->table_adr;

        uint32 lut_addr;

        uint32 *plut_addr;

        uint32 tmpData;

        retVal = ipipe_get_3dlut(&lut_addr, 0 /* T0 */ );
        ISP_ASSERT(lut_addr, NULL);
        plut_addr = (uint32 *) lut_addr;
        for (i = 0; i < 192 * 4; i++)
        {
            tmpData = 0;
            tmpData = (((uint32) (*(table_adr++)) & 0x000003FF) << 20);
            tmpData |= (((uint32) (*(table_adr++)) & 0x000003FF) << 10);
            tmpData |= (((uint32) (*(table_adr++)) & 0x000003FF));
            *(plut_addr + ((i % 4) * (1024 / 4)) + (i / 4)) = tmpData;  // banks 
                                                                        // 0,1,2,3
        }
    }
#endif
    ISP_WRITE32(ipipe_reg->D3LUT_EN, cfg->enable);
    return retVal;

}

/* ================================================================ */
/**
 *  ipipe_config_3d_lut()
 *  Description:-This routine configures the 3D Look up Table based correction module in ipipe
 *
 *
 *  @param  cfg  ipipe_3d_lut_cfg_t * cfg:- pointer to cfg params
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
static ISP_RETURN ipipe_get_3dlut(uint32 * tbl_addr, uint8 table)
{
    ISP_RETURN retVal = ISP_SUCCESS;

    ISP_ASSERT(tbl_addr, NULL);

    switch (table)
    {
        case 0:
            *tbl_addr = IPIPE_3DLUTtable0;
            break;
        case 1:
            *tbl_addr = IPIPE_3DLUTtable1;
            break;
        case 2:
            *tbl_addr = IPIPE_3DLUTtable2;
            break;
        case 3:
            *tbl_addr = IPIPE_3DLUTtable3;
            break;
        default:
            retVal = ISP_FAILURE;
            break;
    }
    return retVal;

}

/* ================================================================ */
/**
 *  ipipe_config_rgb2ycbcr()
 *  Description:-This routine will configure the RGB yo YUV 444 conversion filter in ipipe
 *
 *
 *  @param   cfg  ipipe_3d_lut_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_rgb2ycbcr(ipipe_rgb_yuv_cfg_t * cfg)
{

    int i = 0;

    volatile uint32 *reg_adr = 0;

    volatile int16 *ptr = 0;

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_RGB_TO_YUV_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_SET32(ipipe_reg->YUV_ADJ, cfg->brightness, CSL_IPIPE_YUV_ADJ_BRT_SHIFT,
              CSL_IPIPE_YUV_ADJ_BRT_MASK);
    ISP_SET32(ipipe_reg->YUV_ADJ, cfg->contrast, CSL_IPIPE_YUV_ADJ_CTR_SHIFT,
              CSL_IPIPE_YUV_ADJ_CTR_MASK);

    /* Update matrix */
    reg_adr = (volatile uint32 *) &(ipipe_reg->YUV_MUL_RY);
    ptr = cfg->mul_val;

    for (i = 0; i < 9; i++)
    {

        ISP_WRITE32(*(reg_adr+i),*(ptr+i));

    }

    /* update OFFSETS */
    reg_adr = (volatile uint32 *) &(ipipe_reg->YUV_OFT_Y);
    ptr = cfg->offset;
    for (i = 0; i < 3; i++)
    {

        ISP_WRITE32(*(reg_adr+i),*(ptr+i));

    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_GBCE()
 *  Description:-This routine will configure the Global Brightness and contrast enhancement module in ipipe
 *
 *
 *  @param   cfg  ipipe_gbce_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_GBCE(ipipe_gbce_cfg_t * cfg)
{

    int i = 0;

    uint16 *tblptr = NULL;

    uint32 *memptr = NULL;

    ISP_ASSERT(cfg, NULL);
    ipipe_sub_module_clk_enable(IPIPE_GBCE_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_SET32(ipipe_reg->GBCE_TYP, cfg->typ, CSL_IPIPE_GBCE_TYP_TYP_SHIFT,
              CSL_IPIPE_GBCE_TYP_TYP_MASK);

    if (cfg->enable == 1)
    {
        tblptr = cfg->lookup_table;
        ISP_ASSERT(tblptr, NULL);
        memptr = (uint32 *) IPIPE_GBCtable;

        if (cfg->typ == IPIPE_GBCE_METHOD_Y_VALUE)
        {
            for (i = 0; i < 512; i++)
            {
                *memptr =
                    (uint32) (((*(tblptr + 1) << 10) & 0x0003FC00) |
                              ((*tblptr) & 0x000000FF));

                tblptr += 2;
                memptr++;
            }
        }
        else
        {
            for (i = 0; i < 512; i++)
            {
                *memptr =
                    (uint32) (((*(tblptr + 1) << 10) & 0x000FFC00) |
                              ((*tblptr) & 0x000003FF));

                tblptr += 2;
                memptr++;
            }
        }
    }
    /* Enable GBCE */
    ISP_WRITE32(ipipe_reg->GBCE_EN, cfg->enable);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_config_edge_enhancement()
 *  Description:-This routine will configure the Edge Enhancement module in ipipe
 *
 *
 *  @param   cfg  ipipe_ee_cfg_t * cfg :- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_edge_enhancement(ipipe_ee_cfg_t * cfg)
{

    volatile uint32 *reg_adr = 0;

    volatile int16 *ptr = 0;

    int i = 0;

    int16 *tblptr = NULL;

    uint32 *memptr = NULL;

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_EE_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_WRITE32(ipipe_reg->YEE_EN, cfg->enable);

    ISP_SET32(ipipe_reg->YEE_TYP, cfg->halo_reduction,
              CSL_IPIPE_YEE_TYP_HAL_SHIFT, CSL_IPIPE_YEE_TYP_HAL_MASK);
    ISP_SET32(ipipe_reg->YEE_TYP, cfg->sel, CSL_IPIPE_YEE_TYP_SEL_SHIFT,
              CSL_IPIPE_YEE_TYP_SEL_MASK);

    ISP_SET32(ipipe_reg->YEE_SHF, cfg->shift_hp, CSL_IPIPE_YEE_SHF_SHF_SHIFT,
              CSL_IPIPE_YEE_SHF_SHF_MASK);

    reg_adr = (volatile uint32 *) &(ipipe_reg->YEE_MUL_00);
    ptr = cfg->mul_val;

    for (i = 0; i < 9; i++)
    {

        ISP_WRITE32(*(reg_adr+i),*(ptr+i));

    }

    ISP_SET32(ipipe_reg->YEE_THR, cfg->threshold, CSL_IPIPE_YEE_THR_VAL_SHIFT,
              CSL_IPIPE_YEE_THR_VAL_MASK);

    ISP_SET32(ipipe_reg->YEE_E_GAN, cfg->gain, CSL_IPIPE_YEE_E_GAN_VAL_SHIFT,
              CSL_IPIPE_YEE_E_GAN_VAL_MASK);
    ISP_SET32(ipipe_reg->YEE_E_GAN, cfg->gain, CSL_IPIPE_YEE_E_GAN_VAL_SHIFT,
              CSL_IPIPE_YEE_E_GAN_VAL_MASK);

    ISP_SET32(ipipe_reg->YEE_E_THR_1, cfg->hpf_low_thr,
              CSL_IPIPE_YEE_E_THR_1_VAL_SHIFT, CSL_IPIPE_YEE_E_THR_1_VAL_MASK);
    ISP_SET32(ipipe_reg->YEE_E_THR_2, cfg->hpf_high_thr,
              CSL_IPIPE_YEE_E_THR_2_VAL_SHIFT, CSL_IPIPE_YEE_E_THR_2_VAL_MASK);

    ISP_SET32(ipipe_reg->YEE_G_GAN, cfg->hpf_gradient_gain,
              CSL_IPIPE_YEE_G_GAN_VAL_SHIFT, CSL_IPIPE_YEE_G_GAN_VAL_MASK);
    ISP_SET32(ipipe_reg->YEE_G_OFT, cfg->hpf_gradient_offset,
              CSL_IPIPE_YEE_G_OFT_VAL_SHIFT, CSL_IPIPE_YEE_G_OFT_VAL_MASK);

    tblptr = cfg->ee_table;
    memptr = (uint32 *) IPIPE_YEEtable;

    for (i = 0; i < 512; i++)
    {
        *memptr =
            (uint32) (((*(tblptr + 1) << 9) & 0x0003FE00) |
                      ((*tblptr) & 0x000001FF));

        tblptr += 2;
        memptr++;
    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_car()
 *  Description:-This routinewill configure the Chroma Artiffact redustion filter in ipipe
 *
 *
 *  @param   cfg  ipipe_car_cfg_t  * cfg  - pointer to cfg params
 *
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
ISP_RETURN ipipe_config_car(ipipe_car_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_CAR_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_WRITE32(ipipe_reg->CAR_EN, cfg->enable);

    ISP_WRITE32(ipipe_reg->CAR_TYP, cfg->typ);

    ISP_SET32(ipipe_reg->CAR_SW, cfg->sw0_thr, CSL_IPIPE_CAR_SW_SW0_SHIFT,
              CSL_IPIPE_CAR_SW_SW0_MASK);
    ISP_SET32(ipipe_reg->CAR_SW, cfg->sw1_thr, CSL_IPIPE_CAR_SW_SW1_SHIFT,
              CSL_IPIPE_CAR_SW_SW1_MASK);

    ISP_WRITE32(ipipe_reg->CAR_TYP, cfg->typ);

    ISP_WRITE32(ipipe_reg->CAR_HPF_SHF, cfg->hpf_shift);
    ISP_WRITE32(ipipe_reg->CAR_HPF_THR, cfg->hpf_thr);
    ISP_WRITE32(ipipe_reg->CAR_HPF_TYP, cfg->hpf_type);

    ISP_WRITE32(ipipe_reg->CAR_GN1_MIN, cfg->gn1_min);
    ISP_WRITE32(ipipe_reg->CAR_GN1_GAN, cfg->gn1_gain);
    ISP_WRITE32(ipipe_reg->CAR_GN1_SHF, cfg->gn1_shift);

    ISP_WRITE32(ipipe_reg->CAR_GN2_MIN, cfg->gn2_min);
    ISP_WRITE32(ipipe_reg->CAR_GN2_GAN, cfg->gn2_gain);
    ISP_WRITE32(ipipe_reg->CAR_GN2_SHF, cfg->gn2_shift);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_config_chroma_supression()
 *  Description:-This routine will configure the Chroma Supression unit in ipipe
 *
 *
 *  @param   cfg  ipipe_cgs_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_chroma_supression(ipipe_cgs_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_CGS_CLK, IPIPE_MODULE_CLK_ENABLE);

    ISP_WRITE32(ipipe_reg->CGS_EN, cfg->enable);

    /* confirm gain1 gain2#####TODO */
    ISP_WRITE32(ipipe_reg->CGS_GN1_H_GAN, cfg->y_chroma_high.gain);
    ISP_WRITE32(ipipe_reg->CGS_GN1_H_MIN, cfg->y_chroma_high.min);
    ISP_WRITE32(ipipe_reg->CGS_GN1_H_THR, cfg->y_chroma_high.thr);
    ISP_WRITE32(ipipe_reg->CGS_GN1_H_SHF, cfg->y_chroma_high.shift);

    ISP_WRITE32(ipipe_reg->CGS_GN1_L_GAN, cfg->y_chroma_low.gain);
    ISP_WRITE32(ipipe_reg->CGS_GN1_L_MIN, cfg->y_chroma_low.min);
    ISP_WRITE32(ipipe_reg->CGS_GN1_L_THR, cfg->y_chroma_low.thr);
    ISP_WRITE32(ipipe_reg->CGS_GN1_L_SHF, cfg->y_chroma_low.shift);

    ISP_WRITE32(ipipe_reg->CGS_GN2_L_GAN, cfg->c_chroma.gain);
    ISP_WRITE32(ipipe_reg->CGS_GN2_L_MIN, cfg->c_chroma.min);
    ISP_WRITE32(ipipe_reg->CGS_GN2_L_THR, cfg->c_chroma.thr);
    ISP_WRITE32(ipipe_reg->CGS_GN2_L_SHF, cfg->c_chroma.shift);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_config_lsc()
 *  Description:-This routine will configure the Lens Shading Component in ipipe
 *
 *
 *  @param  cfg  ipipe_lsc_cfg_t * cfg:-  pointer to cfg params
 *
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
ISP_RETURN ipipe_config_lsc(ipipe_lsc_cfg_t * cfg)
{

    ISP_WRITE32(ipipe_reg->LSC_VOFT, cfg->v_offset);
    ISP_WRITE32(ipipe_reg->LSC_VA1, cfg->v_linear_coeff);
    ISP_WRITE32(ipipe_reg->LSC_VA2, cfg->v_quadratic_coeff);
    ISP_SET32(ipipe_reg->LSC_VS, cfg->v_linear_shift,
              CSL_IPIPE_LSC_VS_VS1_SHIFT, CSL_IPIPE_LSC_VS_VS1_MASK);
    ISP_SET32(ipipe_reg->LSC_VS, cfg->v_quadratic_shift,
              CSL_IPIPE_LSC_VS_VS2_SHIFT, CSL_IPIPE_LSC_VS_VS2_MASK);

    ISP_WRITE32(ipipe_reg->LSC_HOFT, cfg->h_offset);
    ISP_WRITE32(ipipe_reg->LSC_HA1, cfg->h_linear_coeff);
    ISP_WRITE32(ipipe_reg->LSC_HA2, cfg->h_quadratic_coeff);
    ISP_SET32(ipipe_reg->LSC_HS, cfg->h_linear_shift,
              CSL_IPIPE_LSC_HS_HS1_SHIFT, CSL_IPIPE_LSC_HS_HS1_MASK);
    ISP_SET32(ipipe_reg->LSC_HS, cfg->h_quadratic_shift,
              CSL_IPIPE_LSC_HS_HS2_SHIFT, CSL_IPIPE_LSC_HS_HS2_MASK);

    ISP_WRITE32(ipipe_reg->LSC_GAN_R, cfg->gain_r);
    ISP_WRITE32(ipipe_reg->LSC_GAN_GR, cfg->gain_gr);
    ISP_WRITE32(ipipe_reg->LSC_GAN_GB, cfg->gain_gb);
    ISP_WRITE32(ipipe_reg->LSC_GAN_B, cfg->gain_b);

    ISP_WRITE32(ipipe_reg->LSC_OFT_R, cfg->off_r);
    ISP_WRITE32(ipipe_reg->LSC_OFT_GR, cfg->off_gr);
    ISP_WRITE32(ipipe_reg->LSC_OFT_GB, cfg->off_gb);
    ISP_WRITE32(ipipe_reg->LSC_OFT_B, cfg->off_b);

    ISP_WRITE32(ipipe_reg->LSC_SHF, cfg->shift);
    ISP_WRITE32(ipipe_reg->LSC_MAX, cfg->max);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_input_src()
 *  Description:-This routine configures the input pixel format , and the position of color components in the RAw pixel data,
 *			    and the dimensions of the input data
 *
 *
 *  @param   cfg  ipipe_src_cfg_t * cfg:-pointer to cfg params
 *
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
ISP_RETURN ipipe_config_input_src(ipipe_src_cfg_t * cfg)
{

    uint32 val = 0;

    ISP_ASSERT(cfg, NULL);

    ISP_SET32(ipipe_reg->SRC_MODE, cfg->wrt, CSL_IPIPE_SRC_MODE_WRT_SHIFT,
              CSL_IPIPE_SRC_MODE_WRT_MASK);

    ISP_SET32(ipipe_reg->SRC_FMT, cfg->io_pixel_fmt,
              CSL_IPIPE_SRC_FMT_FMT_SHIFT, CSL_IPIPE_SRC_FMT_FMT_MASK);

    val = ipipe_reg->SRC_COL;

    ISP_SET32(val, cfg->Odd_line_Odd_pixel, CSL_IPIPE_SRC_COL_OO_SHIFT,
              CSL_IPIPE_SRC_COL_OO_MASK);
    ISP_SET32(val, cfg->Odd_line_Even_pixel, CSL_IPIPE_SRC_COL_OE_SHIFT,
              CSL_IPIPE_SRC_COL_OE_MASK);
    ISP_SET32(val, cfg->Even_line_Odd_pixel, CSL_IPIPE_SRC_COL_EO_SHIFT,
              CSL_IPIPE_SRC_COL_EO_MASK);
    ISP_SET32(val, cfg->Even_line_Even_pixel, CSL_IPIPE_SRC_COL_EE_SHIFT,
              CSL_IPIPE_SRC_COL_EE_MASK);

    ISP_WRITE32(ipipe_reg->SRC_COL, val);

    ISP_SET32(ipipe_reg->SRC_VPS, cfg->dims.vps, CSL_IPIPE_SRC_VPS_VAL_SHIFT,
              CSL_IPIPE_SRC_VPS_VAL_MASK);
    ISP_SET32(ipipe_reg->SRC_VSZ, cfg->dims.v_size - 1,
              CSL_IPIPE_SRC_VSZ_VAL_SHIFT, CSL_IPIPE_SRC_VSZ_VAL_MASK);

    ISP_SET32(ipipe_reg->SRC_HPS, cfg->dims.hps, CSL_IPIPE_SRC_HPS_VAL_SHIFT,
              CSL_IPIPE_SRC_HPS_VAL_MASK);
    ISP_SET32(ipipe_reg->SRC_HSZ, cfg->dims.hsz - 1,
              CSL_IPIPE_SRC_HSZ_VAL_SHIFT, CSL_IPIPE_SRC_HSZ_VAL_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_config_dimensions()
 *  Description:-This routine configures the input dimensions of the data  to ipipe module ,
 *				this can be used at run time during V-blank to change the dimensions , for eg to crop the input data
 *
 *
 *  @param   cfg  ipipe_dims_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_dimensions(ipipe_dims_t * cfg)
{

    ISP_SET32(ipipe_reg->SRC_VPS, cfg->vps, CSL_IPIPE_SRC_VPS_VAL_SHIFT,
              CSL_IPIPE_SRC_VPS_VAL_MASK);
    ISP_SET32(ipipe_reg->SRC_VSZ, cfg->v_size - 1, CSL_IPIPE_SRC_VSZ_VAL_SHIFT,
              CSL_IPIPE_SRC_VSZ_VAL_MASK);

    ISP_SET32(ipipe_reg->SRC_HPS, cfg->hps, CSL_IPIPE_SRC_HPS_VAL_SHIFT,
              CSL_IPIPE_SRC_HPS_VAL_MASK);
    ISP_SET32(ipipe_reg->SRC_HSZ, cfg->hsz - 1, CSL_IPIPE_SRC_HSZ_VAL_SHIFT,
              CSL_IPIPE_SRC_HSZ_VAL_MASK);

    return ISP_FAILURE;
}

/* ================================================================ */
/**
 *  ipipe_config_yuv444_to_yuv422()
 *  Description:-This routine controls the change of the YUV 444 to YUV 422 conversions it select the co-sited or
 *			centerd phase for the y samples
 *
 *
 *  @param   cfg  ipipe_yuv444_yuv422_cfg_t * cfg :-pointer to cfg params
 *
 *  @return    ISP_RETURN
 */
/*================================================================== */
ISP_RETURN ipipe_config_yuv444_to_yuv422(ipipe_yuv444_yuv422_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ISP_SET32(ipipe_reg->YUV_PHS, cfg->pos, CSL_IPIPE_YUV_PHS_POS_SHIFT,
              CSL_IPIPE_YUV_PHS_POS_MASK);
    ISP_SET32(ipipe_reg->YUV_PHS, cfg->lpf_en, CSL_IPIPE_YUV_PHS_LPF_SHIFT,
              CSL_IPIPE_YUV_PHS_LPF_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_boxcar()
 *  Description:-This routineconfigure the BOXCAR filter in ipipe.
 *
 *
 *  @param   cfg  ipipe_boxcar_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_boxcar(ipipe_boxcar_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    if (((cfg->addr) & (0x20 - 1)) != 0)
        return ISP_FAILURE;
    /* set the interrupt handler for updating the lookup table
     * "isp_common_set_interrupt_handler" */
    ipipe_sub_module_clk_enable(IPIPE_BOXCAR_CLK, IPIPE_MODULE_CLK_ENABLE);
    ISP_SET32(ipipe_reg->BOX_EN, cfg->enable, CSL_IPIPE_BOX_EN_EN_SHIFT,
              CSL_IPIPE_BOX_EN_EN_MASK);
    ISP_SET32(ipipe_reg->BOX_MODE, 0 /* cfg->ost */ ,
              CSL_IPIPE_BOX_MODE_OST_SHIFT, CSL_IPIPE_BOX_MODE_OST_MASK);
    ISP_SET32(ipipe_reg->BOX_TYP, cfg->box_size, CSL_IPIPE_BOX_TYP_SEL_SHIFT,
              CSL_IPIPE_BOX_TYP_SEL_MASK);
    ISP_SET32(ipipe_reg->BOX_SHF, cfg->shift, CSL_IPIPE_BOX_SHF_VAL_SHIFT,
              CSL_IPIPE_BOX_SHF_VAL_MASK);

    ISP_SET32(ipipe_reg->BOX_SDR_SAD_H, (cfg->addr >> 16),
              CSL_IPIPE_BOX_SDR_SAD_H_VAL_SHIFT,
              CSL_IPIPE_BOX_SDR_SAD_H_VAL_MASK);
    ISP_SET32(ipipe_reg->BOX_SDR_SAD_L, (cfg->addr & 0x0000FFFF),
              CSL_IPIPE_BOX_SDR_SAD_L_VAL_SHIFT,
              CSL_IPIPE_BOX_SDR_SAD_L_VAL_MASK);
    return ISP_SUCCESS;

}
/* ================================================================ */
/**
 *  ipipe_config_boxcar_addr()
 *  Description:-This routineconfigure the BOXCAR filter in ipipe.
 *
 *
 *  @param   cfg  ipipe_boxcar_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_boxcar_addr(uint32 addr)
{

    ISP_ASSERT(addr, NULL);

    if (((addr) & (0x20 - 1)) != 0)
        return ISP_FAILURE;
    ISP_SET32(ipipe_reg->BOX_SDR_SAD_H, (addr >> 16),
              CSL_IPIPE_BOX_SDR_SAD_H_VAL_SHIFT,
              CSL_IPIPE_BOX_SDR_SAD_H_VAL_MASK);
    ISP_SET32(ipipe_reg->BOX_SDR_SAD_L, (addr & 0x0000FFFF),
              CSL_IPIPE_BOX_SDR_SAD_L_VAL_SHIFT,
              CSL_IPIPE_BOX_SDR_SAD_L_VAL_MASK);
    return ISP_SUCCESS;

}
/* ================================================================ */
/**
 *  ipipe_config_histogram()
 *  Description:-This routine configures the Histogram unit in ipipe
 *
 *
 *  @param  cfg  ipipe_hist_cfg_t * cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_histogram(ipipe_hist_cfg_t * cfg)
{

    uint32 regval = 0;

    uint8 i;

    volatile uint32 *regptr = &ipipe_reg->HST_0_VPS;

    ISP_ASSERT(cfg, NULL);

    ipipe_sub_module_clk_enable(IPIPE_HST_CLK, IPIPE_MODULE_CLK_ENABLE);
    /* set the interrupt handler for updating the lookup table
     * "isp_common_set_interrupt_handler" */
    ISP_SET32(ipipe_reg->HST_EN, cfg->enable, CSL_IPIPE_HST_EN_EN_SHIFT,
              CSL_IPIPE_HST_EN_EN_MASK);
    ISP_SET32(ipipe_reg->HST_MODE, cfg->ost, CSL_IPIPE_HST_MODE_OST_SHIFT,
              CSL_IPIPE_HST_MODE_OST_MASK);
    ISP_SET32(ipipe_reg->HST_SEL, cfg->sel, CSL_IPIPE_HST_SEL_SEL_SHIFT,
              CSL_IPIPE_HST_SEL_SEL_MASK);
    ISP_SET32(ipipe_reg->HST_SEL, cfg->type, CSL_IPIPE_HST_SEL_TYP_SHIFT,
              CSL_IPIPE_HST_SEL_TYP_MASK);

    regval = ipipe_reg->HST_PARA;                          /* Local copy to
                                                            * increase speed */

    ISP_SET32(regval, cfg->bins, CSL_IPIPE_HST_PARA_BIN_SHIFT,
              CSL_IPIPE_HST_PARA_BIN_MASK);
    ISP_SET32(regval, cfg->shift, CSL_IPIPE_HST_PARA_SHF_SHIFT,
              CSL_IPIPE_HST_PARA_SHF_MASK);
    ISP_SET32(regval, cfg->col, CSL_IPIPE_HST_PARA_COL0_SHIFT,
              (CSL_IPIPE_HST_PARA_COL0_MASK | CSL_IPIPE_HST_PARA_COL1_SHIFT |
               CSL_IPIPE_HST_PARA_COL2_SHIFT | CSL_IPIPE_HST_PARA_COL3_SHIFT));
    ISP_SET32(regval, cfg->regions, CSL_IPIPE_HST_PARA_RGN0_SHIFT,
              (CSL_IPIPE_HST_PARA_RGN0_MASK | CSL_IPIPE_HST_PARA_RGN1_MASK |
               CSL_IPIPE_HST_PARA_RGN2_MASK | CSL_IPIPE_HST_PARA_RGN3_MASK));

    ipipe_reg->HST_PARA = regval;                          /* Write-back in
                                                            * the register */

    /* update the clear and table bits */
    ISP_SET32(ipipe_reg->HST_TBL, cfg->clear_table, CSL_IPIPE_HST_TBL_CLR_SHIFT,
              CSL_IPIPE_HST_TBL_CLR_MASK);
    ISP_SET32(ipipe_reg->HST_TBL, cfg->table_sel, CSL_IPIPE_HST_TBL_SEL_SHIFT,
              CSL_IPIPE_HST_TBL_SEL_MASK);

    /* update the dimensions */

    for (i = 0; i < 4; i++)
    {
        ISP_SET32(*regptr, cfg->hist_dim[i].v_pos,
                  CSL_IPIPE_HST_0_VPS_VAL_SHIFT, CSL_IPIPE_HST_0_VPS_VAL_MASK);
        regptr++;
        ISP_SET32(*regptr, (cfg->hist_dim[i].v_size),
                  CSL_IPIPE_HST_0_VSZ_VAL_SHIFT, CSL_IPIPE_HST_0_VSZ_VAL_MASK);
        regptr++;
        ISP_SET32(*regptr, cfg->hist_dim[i].h_pos,
                  CSL_IPIPE_HST_0_HPS_VAL_SHIFT, CSL_IPIPE_HST_0_HPS_VAL_MASK);
        regptr++;
        ISP_SET32(*regptr, (cfg->hist_dim[i].h_size),
                  CSL_IPIPE_HST_0_HSZ_VAL_SHIFT, CSL_IPIPE_HST_0_HSZ_VAL_MASK);
        regptr++;
    }

    /* update the multiplier gains */

    regptr = &ipipe_reg->HST_MUL_R;

    for (i = 0; i < 4; i++)
    {
        ISP_SET32(*regptr, cfg->gain_tbl[i], CSL_IPIPE_HST_MUL_R_VAL_SHIFT,
                  CSL_IPIPE_HST_MUL_R_VAL_MASK);
        regptr++;

    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_config_bsc()
 *  Description:-This routine configurs the Bondary signal calculator module in ipipe
 *
 *
 *  @param   cfg  ipipe_bsc_cfg_t *  cfg:- pointer to cfg params
 *
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
ISP_RETURN ipipe_config_bsc(ipipe_bsc_cfg_t * cfg)
{
    uint32 regval = 0;

    ISP_ASSERT(cfg, NULL);
    ipipe_sub_module_clk_enable(IPIPE_BSC_CLK, IPIPE_MODULE_CLK_ENABLE);
    /* set the interrupt handler for updating the lookup table
     * "isp_common_set_interrupt_handler" */

    ISP_SET32(ipipe_reg->BSC_EN, cfg->enable, CSL_IPIPE_BSC_EN_EN_SHIFT,
              CSL_IPIPE_BSC_EN_EN_MASK);
    ISP_SET32(ipipe_reg->BSC_MODE, cfg->mode, CSL_IPIPE_BSC_MODE_OST_SHIFT,
              CSL_IPIPE_BSC_MODE_OST_MASK);

    regval = ipipe_reg->BSC_TYP;                           /* Local Copy for
                                                            * modifications */

    ISP_SET32(regval, cfg->col_sample, CSL_IPIPE_BSC_TYP_CEN_SHIFT,
              CSL_IPIPE_BSC_TYP_CEN_MASK);
    ISP_SET32(regval, cfg->row_sample, CSL_IPIPE_BSC_TYP_REN_SHIFT,
              CSL_IPIPE_BSC_TYP_REN_MASK);
    ISP_SET32(regval, cfg->element, CSL_IPIPE_BSC_TYP_COL_SHIFT,
              CSL_IPIPE_BSC_TYP_COL_MASK);
    ipipe_reg->BSC_TYP = regval;                           /* Write-back */

    /* Set the Row Position */
    ISP_SET32(ipipe_reg->BSC_ROW_VCT, cfg->row_pos.vectors - 1,
              CSL_IPIPE_BSC_ROW_VCT_ROWNUM_SHIFT,
              CSL_IPIPE_BSC_ROW_VCT_ROWNUM_MASK);
    ISP_SET32(ipipe_reg->BSC_ROW_SHF, cfg->row_pos.shift,
              CSL_IPIPE_BSC_ROW_SHF_VAL_SHIFT, CSL_IPIPE_BSC_ROW_SHF_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_ROW_VPOS, cfg->row_pos.v_pos,
              CSL_IPIPE_BSC_ROW_VPOS_VAL_SHIFT,
              CSL_IPIPE_BSC_ROW_VPOS_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_ROW_HPOS, cfg->row_pos.h_pos,
              CSL_IPIPE_BSC_ROW_HPOS_VAL_SHIFT,
              CSL_IPIPE_BSC_ROW_HPOS_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_ROW_VNUM, cfg->row_pos.v_num - 1,
              CSL_IPIPE_BSC_ROW_VNUM_VAL_SHIFT,
              CSL_IPIPE_BSC_ROW_VNUM_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_ROW_VSKIP, cfg->row_pos.v_skip - 1,
              CSL_IPIPE_BSC_ROW_VSKIP_ROWSKIPV_SHIFT,
              CSL_IPIPE_BSC_ROW_VSKIP_ROWSKIPV_MASK);

    ISP_SET32(ipipe_reg->BSC_ROW_HNUM, cfg->row_pos.h_num - 1,
              CSL_IPIPE_BSC_ROW_HNUM_VAL_SHIFT,
              CSL_IPIPE_BSC_ROW_HNUM_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_ROW_HSKIP, cfg->row_pos.h_skip - 1,
              CSL_IPIPE_BSC_ROW_HSKIP_ROWSKIPH_SHIFT,
              CSL_IPIPE_BSC_ROW_HSKIP_ROWSKIPH_MASK);

    /* Set the Column Position */
    ISP_SET32(ipipe_reg->BSC_COL_VCT, cfg->col_pos.vectors - 1,
              CSL_IPIPE_BSC_COL_VCT_COLNUM_SHIFT,
              CSL_IPIPE_BSC_COL_VCT_COLNUM_MASK);
    ISP_SET32(ipipe_reg->BSC_COL_SHF, cfg->col_pos.shift,
              CSL_IPIPE_BSC_COL_SHF_VAL_SHIFT, CSL_IPIPE_BSC_COL_SHF_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_COL_VPOS, cfg->col_pos.v_pos,
              CSL_IPIPE_BSC_COL_VPOS_VAL_SHIFT,
              CSL_IPIPE_BSC_COL_VPOS_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_COL_HPOS, cfg->col_pos.h_pos,
              CSL_IPIPE_BSC_COL_HPOS_VAL_SHIFT,
              CSL_IPIPE_BSC_COL_HPOS_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_COL_VNUM, cfg->col_pos.v_num - 1,
              CSL_IPIPE_BSC_COL_VNUM_VAL_SHIFT,
              CSL_IPIPE_BSC_COL_VNUM_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_COL_VSKIP, cfg->col_pos.v_skip - 1,
              CSL_IPIPE_BSC_COL_VSKIP_COLSKIPV_SHIFT,
              CSL_IPIPE_BSC_COL_VSKIP_COLSKIPV_MASK);

    ISP_SET32(ipipe_reg->BSC_COL_HNUM, cfg->col_pos.h_num - 1,
              CSL_IPIPE_BSC_COL_HNUM_VAL_SHIFT,
              CSL_IPIPE_BSC_COL_HNUM_VAL_MASK);
    ISP_SET32(ipipe_reg->BSC_COL_HSKIP, cfg->col_pos.h_skip - 1,
              CSL_IPIPE_BSC_COL_HSKIP_COLSKIPH_SHIFT,
              CSL_IPIPE_BSC_COL_HSKIP_COLSKIPH_MASK);
    return ISP_SUCCESS;
}

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
ISP_RETURN ipipe_get_bsc_address(uint32 * ptr, IPIPE_BSC_MEMORY_T table_select)
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (table_select == IPIPE_BSC_MEMORY_0)
    {
        *ptr = IPIPE_BSC_MEM0_ADDRESS;
    }
    else if (table_select == IPIPE_BSC_MEMORY_1)
    {
        *ptr = IPIPE_BSC_MEM1_ADDRESS;
    }
    else
    {
        retval = ISP_FAILURE;
    }

    return retval;

}

/* ================================================================ */
/**
 * ipipe_sub_module_clk_enable()
 *  Description:-This routine enables the clock of sub modules within ipipe, this is to be called only by the ipipe cfg functions and
 *				not directly from exernal components, the global variables are used for reference counting of the clock
 *                        requests since the clocks are shared between the sub modules within ipipe
 *
 *
 *  @param  clk_module  ipipe_module_clk clk_module:- specifies the module whose clk need a change
 *  @param  enable IPIPE_MODULE_CLK_CTRL_T enable:-  enable/disable/param
 *  @return    ISP_RETURN
 */
/*================================================================== */

static uint8 g0_clk_ref_cnt = 0;

static uint8 g1_clk_ref_cnt = 0;

static uint8 g2_clk_ref_cnt = 0;

static uint8 g3_clk_ref_cnt = 0;

/* ===================================================================
 *  @func     ipipe_sub_module_clk_enable                                               
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
static ISP_RETURN ipipe_sub_module_clk_enable(ipipe_module_clk clk_module,
                                              IPIPE_MODULE_CLK_CTRL_T enable)
{
    // ISP_RETURN retval=ISP_FAILURE; /*########## Declared but never used
    // ###################*/

    switch (clk_module)

    {

        case G0:
        {
            if (enable == IPIPE_MODULE_CLK_ENABLE)
            {
                g0_clk_ref_cnt++;
                ISP_SET32(ipipe_reg->GCK_PIX, enable,
                          CSL_IPIPE_GCK_PIX_G0_SHIFT,
                          CSL_IPIPE_GCK_PIX_G0_MASK);

            }
            else
            {
                if (g0_clk_ref_cnt == 0)
                    return ISP_FAILURE;
                g0_clk_ref_cnt--;
                if (g0_clk_ref_cnt == 0)
                    ISP_SET32(ipipe_reg->GCK_PIX, enable,
                              CSL_IPIPE_GCK_PIX_G0_SHIFT,
                              CSL_IPIPE_GCK_PIX_G0_MASK);
            }
            break;
        }

        case G1:
        {
            if (enable == IPIPE_MODULE_CLK_ENABLE)
            {
                g1_clk_ref_cnt++;
                ISP_SET32(ipipe_reg->GCK_PIX, enable,
                          CSL_IPIPE_GCK_PIX_G1_SHIFT,
                          CSL_IPIPE_GCK_PIX_G1_MASK);

            }
            else
            {
                if (g1_clk_ref_cnt == 0)
                    return ISP_FAILURE;
                g1_clk_ref_cnt--;
                if (g1_clk_ref_cnt == 0)
                    ISP_SET32(ipipe_reg->GCK_PIX, enable,
                              CSL_IPIPE_GCK_PIX_G1_SHIFT,
                              CSL_IPIPE_GCK_PIX_G1_MASK);
            }
            break;
        }
        case G2:
        {
            if (enable == IPIPE_MODULE_CLK_ENABLE)
            {
                g2_clk_ref_cnt++;
                ISP_SET32(ipipe_reg->GCK_PIX, enable,
                          CSL_IPIPE_GCK_PIX_G2_SHIFT,
                          CSL_IPIPE_GCK_PIX_G2_MASK);

            }
            else
            {
                if (g2_clk_ref_cnt == 0)
                    return ISP_FAILURE;
                g2_clk_ref_cnt--;
                if (g2_clk_ref_cnt == 0)
                    ISP_SET32(ipipe_reg->GCK_PIX, enable,
                              CSL_IPIPE_GCK_PIX_G2_SHIFT,
                              CSL_IPIPE_GCK_PIX_G2_MASK);
            }
            break;
        }

        case G3:
        {
            if (enable == IPIPE_MODULE_CLK_ENABLE)
            {
                g3_clk_ref_cnt++;
                ISP_SET32(ipipe_reg->GCK_PIX, enable,
                          CSL_IPIPE_GCK_PIX_G3_SHIFT,
                          CSL_IPIPE_GCK_PIX_G3_MASK);

            }
            else
            {
                if (g3_clk_ref_cnt == 0)
                    return ISP_FAILURE;
                g3_clk_ref_cnt--;
                if (g3_clk_ref_cnt == 0)
                    ISP_SET32(ipipe_reg->GCK_PIX, enable,
                              CSL_IPIPE_GCK_PIX_G3_SHIFT,
                              CSL_IPIPE_GCK_PIX_G3_MASK);
            }
            break;
        }

        default:
            return ISP_FAILURE;
    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_clk_reset()
 *  Description:-This routine resets the variables used for reference counting , it is not to be used from external components
 *
 *
 *  @param   :- none
 *
 *  @return    ISP_RETURN
 */
/*================================================================== */
/* ===================================================================
 *  @func     ipipe_clk_reset                                               
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
static ISP_RETURN ipipe_clk_reset()
{

    if (ipipe_reg->SRC_EN == 0)
    {
        g0_clk_ref_cnt = 0;
        g1_clk_ref_cnt = 0;
        g2_clk_ref_cnt = 0;
        g3_clk_ref_cnt = 0;
        ISP_SET32(ipipe_reg->GCK_PIX, 0, CSL_IPIPE_GCK_PIX_G0_SHIFT,
                  CSL_IPIPE_GCK_PIX_G0_MASK);
        ISP_SET32(ipipe_reg->GCK_PIX, 0, CSL_IPIPE_GCK_PIX_G1_SHIFT,
                  CSL_IPIPE_GCK_PIX_G1_MASK);
        ISP_SET32(ipipe_reg->GCK_PIX, 0, CSL_IPIPE_GCK_PIX_G2_SHIFT,
                  CSL_IPIPE_GCK_PIX_G2_MASK);
        ISP_SET32(ipipe_reg->GCK_PIX, 0, CSL_IPIPE_GCK_PIX_G3_SHIFT,
                  CSL_IPIPE_GCK_PIX_G3_MASK);

    }

    else
        return ISP_FAILURE;

    return ISP_SUCCESS;
}

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
uint32 ipipe_get_histogram(uint32 * hist_buf, uint32 region_num,
                           IPIPE_HIST_COLOR_T color)
{
    uint32 histSize;

    uint32 histMemAddr;

    uint32 histMemOfst;

    if (region_num > 3)
    {
        return 0;
    }

    histSize = ISP_FEXT(ipipe_reg->HST_PARA, CSL_IPIPE_HST_PARA_BIN_SHIFT,
                        CSL_IPIPE_HST_PARA_BIN_MASK);

    if (ISP_FEXT(ipipe_reg->HST_TBL, CSL_IPIPE_HST_TBL_SEL_SHIFT,
                 CSL_IPIPE_HST_TBL_SEL_MASK))
    {
        if (color >> 1)
        {
            histMemAddr = IPIPE_HIST_MEM3_ADDRESS;
        }
        else
        {
            histMemAddr = IPIPE_HIST_MEM2_ADDRESS;
        }
    }
    else
    {
        if (color >> 1)
        {
            histMemAddr = IPIPE_HIST_MEM1_ADDRESS;
        }
        else
        {
            histMemAddr = IPIPE_HIST_MEM0_ADDRESS;
        }
    }

    histMemOfst = (region_num << 6) | ((color & 1) << 5);
    histMemOfst = (histMemOfst << histSize) & 0x1ff;

    histMemAddr += (histMemOfst << 2);                     /* histMemOfst is
                                                            * in 32 bit words 
                                                            * - multiplying
                                                            * it by 4 to get
                                                            * the address
                                                            * offset in bytes 
                                                            */
    histSize = 32 << histSize;

    memcpy(hist_buf, (uint32 *) histMemAddr, histSize << 2);

    return histSize;
}

/* ================================================================ */
/**
 *  ipipe_get_config_DPC_lut()
 *  Description:-This routine gets the cfg of DPC lut module
 *
 *
 *  @param   cfg  ipipe_dpc_lut_cfg_t *  cfg:- pointer to the cfg struct
 *
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
ISP_RETURN ipipe_get_config_DPC_lut(ipipe_dpc_lut_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(cfg->enable, ipipe_reg->DPC_LUT_EN);
    cfg->table_type =
        (IPIPE_DPC_LUT_TBL_SIZE_T) (IPIPE_DPC_LUT_TBL_SIZE_T) (ISP_FEXT
                                                               (ipipe_reg->
                                                                DPC_LUT_SEL,
                                                                CSL_IPIPE_DPC_LUT_SEL_TBL_SHIFT,
                                                                CSL_IPIPE_DPC_LUT_SEL_TBL_MASK));
    cfg->replace_type =
        (IPIPE_DPC_LUT_REPLACEMENT_T) ISP_FEXT(ipipe_reg->DPC_LUT_SEL,
                                               CSL_IPIPE_DPC_LUT_SEL_DOT_SHIFT,
                                               CSL_IPIPE_DPC_LUT_SEL_DOT_MASK);

    ISP_WRITE32(cfg->lut_valid_addr, ipipe_reg->DPC_LUT_ADR);
    ISP_WRITE32(cfg->lut_size, ipipe_reg->DPC_LUT_SIZ);

    /* Update the table also */

    /* TODO********************* */

    return ISP_FAILURE;
}

/* ================================================================ */
/**
 *  ipipe_get_config_DPC_otf()
 *  Description:-This routine gets the cfg of DPC OTF module
 *
 *
 *
 *  @param   cfg	ipipe_dpc_otf_cfg_t * cfg:- pointer to the cfg struct
 *
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
ISP_RETURN ipipe_get_config_DPC_otf(ipipe_dpc_otf_cfg_t * cfg)
{

    int i = 0;

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(cfg->enable, ipipe_reg->DPC_OTF_EN);
    cfg->type =
        (IPIPE_DPC_OTF_TYPE_T) ISP_FEXT(ipipe_reg->DPC_OTF_TYP,
                                        CSL_IPIPE_DPC_OTF_TYP_TYP_SHIFT,
                                        CSL_IPIPE_DPC_OTF_TYP_TYP_MASK);
    cfg->algo =
        (IPIPE_DPC_ALGO_T) ISP_FEXT(ipipe_reg->DPC_OTF_TYP,
                                    CSL_IPIPE_DPC_OTF_TYP_ALG_SHIFT,
                                    CSL_IPIPE_DPC_OTF_TYP_ALG_MASK);

    if (cfg->algo == IPIPE_DPC_OTF_ALG_MINMAX2)
    {
        uint32 *reg = (uint32 *) ipipe_reg->DPC_OTF_2_D_THR_R;

        uint16 *dat_ptr = (cfg->dpc_data.dpc2_params.thr_c);

        for (i = 0; i < 4; i++)                            /* get values of
                                                            * D-table */
        {
            *dat_ptr = (uint16) * reg;
            reg++;
            dat_ptr++;
        }

        reg = (uint32 *) ipipe_reg->DPC_OTF_2_C_THR_R;
        dat_ptr = cfg->dpc_data.dpc2_params.thr_d;

        for (i = 0; i < 4; i++)                            /* get values of
                                                            * C-table */
        {
            *dat_ptr = (uint16) * reg;
            reg++;
            dat_ptr++;
        }

    }

    if (cfg->algo == IPIPE_DPC_OTF_ALG_MINMAX3)
    {

        ISP_WRITE32(cfg->dpc_data.dpc3_params.shift, ipipe_reg->DPC_OTF_3_SHF);

        /* ISP_SET32(IPIPE_DPC_OTF_3_D_THR,cfg->dpc_data.dpc3_params.otf3d.thr,CSL_IPIPE_DPC_OTF_3_D_THR_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_THR_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_D_SPL,cfg->dpc_data.dpc3_params.otf3d.d_slp,CSL_IPIPE_DPC_OTF_3_D_SPL_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_SPL_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_D_MAX,cfg->dpc_data.dpc3_params.otf3d.d_max,CSL_IPIPE_DPC_OTF_3_D_MAX_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_MAX_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_D_MIN,cfg->dpc_data.dpc3_params.otf3d.d_min,CSL_IPIPE_DPC_OTF_3_D_MIN_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_D_MAX_VAL_MASK);
         * 
         * ISP_SET32(IPIPE_DPC_OTF_3_C_THR,cfg->dpc_data.dpc3_params.otf3d.thr,CSL_IPIPE_DPC_OTF_3_C_THR_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_THR_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_C_SLP,cfg->dpc_data.dpc3_params.otf3d.d_slp,CSL_IPIPE_DPC_OTF_3_C_SPL_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_SPL_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_C_MAX,cfg->dpc_data.dpc3_params.otf3d.d_max,CSL_IPIPE_DPC_OTF_3_C_MAX_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_MAX_VAL_MASK);
         * ISP_SET32(IPIPE_DPC_OTF_3_C_MIN,cfg->dpc_data.dpc3_params.otf3d.d_min,CSL_IPIPE_DPC_OTF_3_C_MIN_VAL_SHIFT,CSL_IPIPE_DPC_OTF_3_C_MAX_VAL_MASK);
         * 
         */
    }

    return ISP_FAILURE;
}

/* ================================================================ */
/**
 *  ipipe_get_config_noise_filter()
 *  Description:-This routine gets the cfg of  one of the 2 noise filters specified
 *
 *  @param  noise_filter  IPIPE_NOISE_FILTER_T noise_filter :- specifies the noise filter
 *  @param  cfg   ipipe_noise_filter_cfg_t  * cfg :- pointer to the cfg struct
 *
 *
 *
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
                                         ipipe_noise_filter_cfg_t * cfg)
{

    volatile uint32 *reg = 0;

    uint8 i = 0;

    uint32 typ_reg_val = 0;

    // ipipe_module_clk module_clk=0; /*#################### Delcared but
    // never used ########################*/

    uint16 *thr = 0;

    uint8 *str = 0;

    uint8 *spr = 0;

    ISP_ASSERT(cfg, NULL);

    /* Check this */
    if (noise_filter == NOISE_FILTER_1)
        reg = &(ipipe_reg->D2F_1ST_EN);
    else if (noise_filter == NOISE_FILTER_2)
        reg = &(ipipe_reg->D2F_2ND_EN);
    else
        return ISP_FAILURE;

    // module_clk=(noise_filter==NOISE_FILTER_1)?(IPIPE_NOISE_FILTER_1_CLK):(IPIPE_NOISE_FILTER_2_CLK);
    // pipe_sub_module_clk_enable(module_clk,IPIPE_MODULE_CLK_ENABLE );
    /* Clk for both NF1 and NF2 is same */

    /* ipipe_sub_module_clk_enable(IPIPE_NOISE_FILTER_1_CLK,IPIPE_MODULE_CLK_ENABLE 
     * ); */

    ISP_WRITE32(cfg->enable, *reg);

    /* Use local variable to make it fast. */

    typ_reg_val = ISP_READ32(reg);

    cfg->sel =
        (IPIPE_NF_SEL_T) ISP_FEXT(typ_reg_val, CSL_IPIPE_D2F_1ST_TYP_SEL_SHIFT,
                                  CSL_IPIPE_D2F_1ST_TYP_SEL_MASK);
    cfg->lsc_gain =
        (IPIPE_NF_LSC_GAIN_T) ISP_FEXT(typ_reg_val,
                                       CSL_IPIPE_D2F_1ST_TYP_LSC_SHIFT,
                                       CSL_IPIPE_D2F_1ST_TYP_LSC_MASK);
    cfg->typ =
        (IPIPE_NF_SAMPLE_METHOD_T) ISP_FEXT(typ_reg_val,
                                            CSL_IPIPE_D2F_1ST_TYP_TYP_SHIFT,
                                            CSL_IPIPE_D2F_1ST_TYP_TYP_MASK);
    cfg->down_shift_val =
        ISP_FEXT(typ_reg_val, CSL_IPIPE_D2F_1ST_TYP_SHF_SHIFT,
                 CSL_IPIPE_D2F_1ST_TYP_SHF_MASK);
    cfg->spread =
        ISP_FEXT(typ_reg_val, CSL_IPIPE_D2F_1ST_TYP_SPR_SHIFT,
                 CSL_IPIPE_D2F_1ST_TYP_SPR_MASK);

    /* Now commit to IPIPE, regaddr is pointing to EN, TYP is next register
     * so add4 */

    /* ################################################################ Is
     * this required in the get_config also??
     * ---------------------------------
     * 
     * ISP_WRITE32(*(reg+1),typ_reg_val);
     * 
     * 
     * ################################################################### */

    /* Set filter thresholds */

    reg =
        (volatile uint32 *) (noise_filter ==
                             NOISE_FILTER_2) ? ((ipipe_reg->
                                                 D2F_2ND_THR)) : ((ipipe_reg->
                                                                   D2F_1ST_THR));
    thr = cfg->thr;
    for (i = 0; i < 8; i++)
    {
        ISP_WRITE16(*thr, *reg);
        reg++;                                             /* increases by 4
                                                            * bytes */
        thr++;                                             /* increases by 2
                                                            * bytes */

    }

    /* Set filter str */
    reg =
        (volatile uint32 *) (noise_filter ==
                             NOISE_FILTER_2) ? (ipipe_reg->
                                                D2F_2ND_STR) : (ipipe_reg->
                                                                D2F_1ST_STR);
    str = cfg->str;
    for (i = 0; i < 8; i++)
    {
        ISP_WRITE8(*str, *reg);
        reg++;                                             /* increases by 32 
                                                            * bits */
        str++;                                             /* increases by 8
                                                            * bit */

    }

    /* Set filter spread */
    reg =
        (volatile uint32 *) (noise_filter ==
                             NOISE_FILTER_2) ? (ipipe_reg->
                                                D2F_2ND_SPR) : (ipipe_reg->
                                                                D2F_1ST_SPR);
    spr = cfg->spr;
    for (i = 0; i < 8; i++)
    {
        ISP_WRITE8(*spr, *reg);
        reg++;                                             /* increases by 32 
                                                            * bits */
        spr++;                                             /* increases by 8
                                                            * bit */

    }

    /**SET edge max and min*/

    reg =
        (volatile uint32 *) ((noise_filter == NOISE_FILTER_2)
                             ? (&ipipe_reg->D2F_2ND_EDG_MAX) : (&ipipe_reg->
                                                                D2F_1ST_EDG_MAX));

    ISP_WRITE16(cfg->edge_max, *reg);

    reg =
        (volatile uint32 *) ((noise_filter == NOISE_FILTER_2)
                             ? (&ipipe_reg->D2F_2ND_EDG_MIN) : (&ipipe_reg->
                                                                D2F_1ST_EDG_MIN));

    ISP_WRITE16(cfg->edge_min, *reg);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_get_config_gic_filter()
 *  Description:-This routine gets the cfg of  Green Imbalance correction
 *
 *
 *  @param   cfg  ipipe_gic_cfg_t * cfg:- pointer to the cfg struct
 *
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
ISP_RETURN ipipe_get_config_gic_filter(ipipe_gic_cfg_t * cfg)
{

    uint32 val = 0;

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(cfg->enable, ipipe_reg->GIC_EN);

    val = ipipe_reg->GIC_TYP;

    cfg->typ =
        (IPIPE_GIC_INDEX_T) ISP_FEXT(val, CSL_IPIPE_GIC_TYP_TYP_SHIFT,
                                     CSL_IPIPE_GIC_TYP_TYP_MASK);
    cfg->lsc_gain =
        (IPIPE_GIC_LSC_GAIN_T) ISP_FEXT(val, CSL_IPIPE_GIC_TYP_LSC_SHIFT,
                                        CSL_IPIPE_GIC_TYP_LSC_MASK);
    cfg->sel = (IPIPE_GIC_SEL_T) ISP_FEXT(val, CSL_IPIPE_GIC_TYP_SEL_SHIFT, CSL_IPIPE_GIC_TYP_SEL_MASK);    /* Here, 
                                                                                                             * code 
                                                                                                             * had 
                                                                                                             * 2 
                                                                                                             * shift 
                                                                                                             * inputs, 
                                                                                                             * so 
                                                                                                             * changed 
                                                                                                             * to 
                                                                                                             * shift+mask */

    ISP_WRITE32(cfg->gic_gain, ipipe_reg->GIC_GAN);
    ISP_WRITE32(cfg->gic_nfgain, ipipe_reg->GIC_NFGAN);
    ISP_WRITE32(cfg->gic_thr, ipipe_reg->GIC_THR);
    ISP_WRITE32(cfg->gic_slope, ipipe_reg->GIC_SLP);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_get_config_wbal()
 *  Description:-This routine gets the cfg of    White Balance filter
 *
 *
 *  @param   cfg  ipipe_wb_cfg_t * cfg:- pointer to the cfg struct
 *
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
ISP_RETURN ipipe_get_config_wbal(ipipe_wb_cfg_t * cfg)
{

    uint16 *val_ptr = NULL;

    volatile uint32 *reg_ptr = (volatile uint32 *) &(ipipe_reg->WB2_OFT_R);

    ISP_ASSERT(cfg, NULL);

    val_ptr = cfg->offset;
    ISP_ASSERT(val_ptr, NULL);

    *(val_ptr) =
        (uint16) ISP_FEXT(*reg_ptr, CSL_IPIPE_WB2_OFT_R_VAL_SHIFT,
                          CSL_IPIPE_WB2_OFT_R_VAL_MASK);
    *(val_ptr + 1) =
        (uint16) ISP_FEXT(*(reg_ptr+1), CSL_IPIPE_WB2_OFT_GR_VAL_SHIFT,
                          CSL_IPIPE_WB2_OFT_GR_VAL_MASK);
    *(val_ptr + 2) =
        (uint16) ISP_FEXT(*(reg_ptr+2), CSL_IPIPE_WB2_OFT_GB_VAL_SHIFT,
                          CSL_IPIPE_WB2_OFT_GB_VAL_MASK);
    *(val_ptr + 3) =
        (uint16) ISP_FEXT(*(reg_ptr+3), CSL_IPIPE_WB2_OFT_B_VAL_SHIFT,
                          CSL_IPIPE_WB2_OFT_B_VAL_MASK);

    val_ptr = cfg->gain;
    reg_ptr = (volatile uint32 *) &ipipe_reg->WB2_WGN_R;

    *(val_ptr) =
        (uint16) ISP_FEXT(*reg_ptr, CSL_IPIPE_WB2_WGN_R_VAL_SHIFT,
                          CSL_IPIPE_WB2_WGN_R_VAL_MASK);
    *(val_ptr + 1) =
        (uint16) ISP_FEXT(*(reg_ptr+1), CSL_IPIPE_WB2_WGN_GR_VAL_SHIFT,
                          CSL_IPIPE_WB2_WGN_GR_VAL_MASK);
    *(val_ptr + 2) =
        (uint16) ISP_FEXT(*(reg_ptr+2), CSL_IPIPE_WB2_WGN_GB_VAL_SHIFT,
                          CSL_IPIPE_WB2_WGN_GB_VAL_MASK);
    *(val_ptr + 3) =
        (uint16) ISP_FEXT(*(reg_ptr+3), CSL_IPIPE_WB2_WGN_B_VAL_SHIFT,
                          CSL_IPIPE_WB2_WGN_B_VAL_MASK);

    return ISP_FAILURE;
}

/* ================================================================ */
/**
 *  ipipe_get_config_cfa()
 *  Description:-This routine gets the cfg of     CFA filter
 *
 *
 *  @param   cfg  ipipe_cfa_cfg_t * cfg:- pointer to the cfg struct
 *
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
ISP_RETURN ipipe_get_config_cfa(ipipe_cfa_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    cfg->mode =
        (IPIPE_CFA_MODE_T) ISP_FEXT(ipipe_reg->CFA_MODE,
                                    CSL_IPIPE_CFA_MODE_MODE_SHIFT,
                                    CSL_IPIPE_CFA_MODE_MODE_MASK);

    ISP_WRITE32(cfg->dir.hpf_thr, ipipe_reg->CFA_2DIR_HPF_THR);
    ISP_WRITE32(cfg->dir.hpf_slope, ipipe_reg->CFA_2DIR_HPF_SLP);
    ISP_WRITE32(cfg->dir.mix_thr, ipipe_reg->CFA_2DIR_MIX_THR);
    ISP_WRITE32(cfg->dir.mix_slope, ipipe_reg->CFA_2DIR_MIX_SLP);
    ISP_WRITE32(cfg->dir.dir_thr, ipipe_reg->CFA_2DIR_DIR_THR);
    ISP_WRITE32(cfg->dir.dir_slope, ipipe_reg->CFA_2DIR_DIR_SLP);
    ISP_WRITE32(cfg->dir.dir_ndwt, ipipe_reg->CFA_2DIR_NDWT);
    ISP_WRITE32(cfg->daa.mono_hue_fra, ipipe_reg->CFA_MONO_HUE_FRA);
    ISP_WRITE32(cfg->daa.mono_edg_thr, ipipe_reg->CFA_MONO_EDG_THR);
    ISP_WRITE32(cfg->daa.mono_thr_min, ipipe_reg->CFA_MONO_THR_MIN);
    ISP_WRITE32(cfg->daa.mono_thr_slope, ipipe_reg->CFA_MONO_THR_SLP);
    ISP_WRITE32(cfg->daa.mono_slp_min, ipipe_reg->CFA_MONO_SLP_MIN);
    ISP_WRITE32(cfg->daa.mono_slp_slp, ipipe_reg->CFA_MONO_SLP_SLP);
    ISP_WRITE32(cfg->daa.mono_lpwt, ipipe_reg->CFA_MONO_LPWT);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_get_config_rgb_to_rgb()
 *  Description:-This routine gets the cfg of  one of the 2 RGB to RGB blending modules
 *
 *
 *  @param  filter_num  IPIPE_RGB_RGB_FILTER_NUM_T  filter_num :- specifies one of the two filters
 *  @param cfg  ipipe_rgb_rgb_cfg_t  * cfg :- pointer to the cfg struct
 *
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
                                       ipipe_rgb_rgb_cfg_t * cfg)
{

    int16 *val_adr = NULL;

    int i = 0;

    volatile uint32 *reg_adr = 0;

    ISP_ASSERT(cfg, NULL);
    val_adr = cfg->mul_off;

    reg_adr =
        (volatile uint32 *) ((filter_num == IPIPE_RGB_RGB_FILTER_1)
                             ? (&ipipe_reg->RGB1_MUL_RR) : (&ipipe_reg->
                                                            RGB2_MUL_RR));

    /* Get the matrix co- efficients */
    for (i = 0; i < 9; i++)
    {
        ISP_WRITE16(*(val_adr+i),*(reg_adr+i));
    }

    /* Get the offsets */
    val_adr = cfg->oft;
    reg_adr =
        (uint32 *) ((filter_num == IPIPE_RGB_RGB_FILTER_1)
                    ? (ipipe_reg->RGB1_OFT_OR) : (ipipe_reg->RGB2_OFT_OR));

    for (i = 0; i < 3; i++)
    {
        ISP_WRITE16(*(val_adr+i),*(reg_adr+i));

    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_get_config_gamma()
 *  Description:-This routine gets the cfg of  Gamma correction filters
 *
 *
 *  @param   cfg  ipipe_gamma_cfg_t  * :- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_gamma(ipipe_gamma_cfg_t * cfg)
{

    uint32 reg_val = ipipe_reg->GMM_CFG;

    if (cfg == NULL)
    {
        return ISP_FAILURE;
    }

    cfg->tbl =
        (uint8) ISP_FEXT(reg_val, CSL_IPIPE_GMM_CFG_TBL_SHIFT,
                         CSL_IPIPE_GMM_CFG_TBL_MASK);
    cfg->gamma_tbl_size =
        (IPIPE_GAMMA_TABLE_SIZE_T) ISP_FEXT(reg_val,
                                            CSL_IPIPE_GMM_CFG_SIZ_SHIFT,
                                            CSL_IPIPE_GMM_CFG_SIZ_MASK);
    cfg->bypass_b =
        (IPIPE_GAMMA_BYPASS_T) ISP_FEXT(reg_val, CSL_IPIPE_GMM_CFG_BYPB_SHIFT,
                                        CSL_IPIPE_GMM_CFG_BYPB_MASK);
    cfg->bypass_r =
        (IPIPE_GAMMA_BYPASS_T) ISP_FEXT(reg_val, CSL_IPIPE_GMM_CFG_BYPR_SHIFT,
                                        CSL_IPIPE_GMM_CFG_BYPR_MASK);
    cfg->bypass_g =
        (IPIPE_GAMMA_BYPASS_T) ISP_FEXT(reg_val, CSL_IPIPE_GMM_CFG_BYPG_SHIFT,
                                        CSL_IPIPE_GMM_CFG_BYPG_MASK);

    /* TODO########update gamma table in memory */

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_get_config_3d_lut()
 *  Description:-This routine gets the cfg of  3D LUT module in ipippe
 *
 *
 *  @param   cfg  ipipe_3d_lut_cfg_t * cfg:- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_3d_lut(ipipe_3d_lut_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(cfg->enable, (uint8) ipipe_reg->D3LUT_EN);
    /* TO##########update tables */

    return ISP_FAILURE;

}

/* ================================================================ */
/**
 *  ipipe_get_config_rgb2ycbcr()
 *  Description:-This routine gets the cfg of   RGB to YUV444 conversion module
 *
 *
 *  @param   cfg ipipe_rgb_yuv_cfg_t * cfg:- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_rgb2ycbcr(ipipe_rgb_yuv_cfg_t * cfg)
{

    int i = 0;

    volatile uint32 *reg_adr = 0;

    volatile int16 *ptr = 0;

    ISP_ASSERT(cfg, NULL);

    cfg->brightness =
        ISP_FEXT(ipipe_reg->YUV_ADJ, CSL_IPIPE_YUV_ADJ_BRT_SHIFT,
                 CSL_IPIPE_YUV_ADJ_BRT_MASK);
    cfg->contrast =
        ISP_FEXT(ipipe_reg->YUV_ADJ, CSL_IPIPE_YUV_ADJ_CTR_SHIFT,
                 CSL_IPIPE_YUV_ADJ_CTR_MASK);

    /* Get the matrix values */
    reg_adr = (volatile uint32 *) &(ipipe_reg->YUV_MUL_RY);
    ptr = cfg->mul_val;

    for (i = 0; i < 9; i++)
    {
        ISP_WRITE16(*(ptr+i),*(reg_adr+i));
    }

    /* Get the OFFSETS */
    reg_adr = (volatile uint32 *) &(ipipe_reg->YUV_OFT_Y);
    ptr = cfg->offset;
    for (i = 0; i < 3; i++)
    {
        ISP_WRITE16(*(ptr+i),*(reg_adr+i));
    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipe_get_config_GBCE()
 *  Description:-This routine gets the cfg of  Global brightness and Contrast enhancement module
 *
 *
 *  @param   cfg ipipe_gbce_cfg_t * cfg:- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_GBCE(ipipe_gbce_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    /* ipipe_sub_module_clk_enable(IPIPE_GBCE_CLK,IPIPE_MODULE_CLK_ENABLE ); */

    ISP_WRITE32(cfg->enable, ipipe_reg->GBCE_EN);
    cfg->typ =
        (IPIPE_GBCE_METHOD_T) ISP_FEXT(ipipe_reg->GBCE_TYP,
                                       CSL_IPIPE_GBCE_TYP_TYP_SHIFT,
                                       CSL_IPIPE_GBCE_TYP_TYP_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_get_config_edge_enhancement()
 *  Description:-This routine gets the cfg of     Edge Enhancement
 *
 *
 *  @param   cfg ipipe_ee_cfg_t * cfg:- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_edge_enhancement(ipipe_ee_cfg_t * cfg)
{

    volatile uint32 *reg_adr = 0;

    volatile int16 *ptr = 0;

    int i = 0;

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE16(cfg->enable, ipipe_reg->YEE_EN);

    cfg->halo_reduction =
        (IPIPE_EE_HALO_CTRL_T) ISP_FEXT(ipipe_reg->YEE_TYP,
                                        CSL_IPIPE_YEE_TYP_HAL_SHIFT,
                                        CSL_IPIPE_YEE_TYP_HAL_MASK);
    cfg->sel =
        ISP_FEXT(ipipe_reg->YEE_TYP, CSL_IPIPE_YEE_TYP_SEL_SHIFT,
                 CSL_IPIPE_YEE_TYP_SEL_MASK);
    cfg->shift_hp =
        ISP_FEXT(ipipe_reg->YEE_SHF, CSL_IPIPE_YEE_SHF_SHF_SHIFT,
                 CSL_IPIPE_YEE_SHF_SHF_MASK);

    reg_adr = (volatile uint32 *) &(ipipe_reg->YEE_MUL_00);
    ptr = cfg->mul_val;

    for (i = 0; i < 9; i++)
    {
        ISP_WRITE16(*(ptr+i),*(reg_adr+i));
    }

    cfg->threshold =
        ISP_FEXT(ipipe_reg->YEE_THR, CSL_IPIPE_YEE_THR_VAL_SHIFT,
                 CSL_IPIPE_YEE_THR_VAL_MASK);
    cfg->gain =
        ISP_FEXT(ipipe_reg->YEE_E_GAN, CSL_IPIPE_YEE_E_GAN_VAL_SHIFT,
                 CSL_IPIPE_YEE_E_GAN_VAL_MASK);

    cfg->hpf_low_thr =
        ISP_FEXT(ipipe_reg->YEE_E_THR_1, CSL_IPIPE_YEE_E_THR_1_VAL_SHIFT,
                 CSL_IPIPE_YEE_E_THR_1_VAL_MASK);
    cfg->hpf_high_thr =
        ISP_FEXT(ipipe_reg->YEE_E_THR_2, CSL_IPIPE_YEE_E_THR_2_VAL_SHIFT,
                 CSL_IPIPE_YEE_E_THR_2_VAL_MASK);

    cfg->hpf_gradient_gain =
        ISP_FEXT(ipipe_reg->YEE_G_GAN, CSL_IPIPE_YEE_G_GAN_VAL_SHIFT,
                 CSL_IPIPE_YEE_G_GAN_VAL_MASK);
    cfg->hpf_gradient_offset =
        ISP_FEXT(ipipe_reg->YEE_G_OFT, CSL_IPIPE_YEE_G_OFT_VAL_SHIFT,
                 CSL_IPIPE_YEE_G_OFT_VAL_MASK);

    return ISP_FAILURE;
}

/* ================================================================ */
/**
 *  ipipe_get_config_car()
 *  Description:-This routine gets the cfg of    CAR module in ipipe
 *
 *
 *  @param   cfg  ipipe_car_cfg_t  * cfg :- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_car(ipipe_car_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(cfg->enable, ipipe_reg->CAR_EN);
    ISP_WRITE32(cfg->typ, ipipe_reg->CAR_TYP);

    cfg->sw0_thr =
        ISP_FEXT(ipipe_reg->CAR_SW, CSL_IPIPE_CAR_SW_SW0_SHIFT,
                 CSL_IPIPE_CAR_SW_SW0_MASK);
    cfg->sw1_thr =
        ISP_FEXT(ipipe_reg->CAR_SW, CSL_IPIPE_CAR_SW_SW1_SHIFT,
                 CSL_IPIPE_CAR_SW_SW1_MASK);

    ISP_WRITE32(cfg->typ, ipipe_reg->CAR_TYP);
    ISP_WRITE32(cfg->hpf_shift, ipipe_reg->CAR_HPF_SHF);
    ISP_WRITE32(cfg->hpf_thr, ipipe_reg->CAR_HPF_THR);
    ISP_WRITE32(cfg->hpf_type, ipipe_reg->CAR_HPF_TYP);

    ISP_WRITE32(cfg->gn1_min, ipipe_reg->CAR_GN1_MIN);
    ISP_WRITE32(cfg->gn1_gain, ipipe_reg->CAR_GN1_GAN);
    ISP_WRITE32(cfg->gn1_shift, ipipe_reg->CAR_GN1_SHF);

    ISP_WRITE32(cfg->gn2_min, ipipe_reg->CAR_GN2_MIN);
    ISP_WRITE32(cfg->gn2_gain, ipipe_reg->CAR_GN2_GAN);
    ISP_WRITE32(cfg->gn2_shift, ipipe_reg->CAR_GN2_SHF);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_get_config_chroma_supression()
 *  Description:-This routine gets the cfg of     CAS module
 *
 *
 *  @param  cfg  ipipe_cgs_cfg_t *  cfg:- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_chroma_supression(ipipe_cgs_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(cfg->enable, ipipe_reg->CGS_EN);

    /* confirm gain1 gain2#####TODO */
    ISP_WRITE32(cfg->y_chroma_high.gain, ipipe_reg->CGS_GN1_H_GAN);
    ISP_WRITE32(cfg->y_chroma_high.min, ipipe_reg->CGS_GN1_H_MIN);
    ISP_WRITE32(cfg->y_chroma_high.thr, ipipe_reg->CGS_GN1_H_THR);
    ISP_WRITE32(cfg->y_chroma_high.shift, ipipe_reg->CGS_GN1_H_SHF);

    ISP_WRITE32(cfg->y_chroma_low.gain, ipipe_reg->CGS_GN1_L_GAN);
    ISP_WRITE32(cfg->y_chroma_low.min, ipipe_reg->CGS_GN1_L_MIN);
    ISP_WRITE32(cfg->y_chroma_low.thr, ipipe_reg->CGS_GN1_L_THR);
    ISP_WRITE32(cfg->y_chroma_low.shift, ipipe_reg->CGS_GN1_L_SHF);

    ISP_WRITE32(cfg->c_chroma.gain, ipipe_reg->CGS_GN2_L_GAN);
    ISP_WRITE32(cfg->c_chroma.min, ipipe_reg->CGS_GN2_L_MIN);
    ISP_WRITE32(cfg->c_chroma.thr, ipipe_reg->CGS_GN2_L_THR);
    ISP_WRITE32(cfg->c_chroma.shift, ipipe_reg->CGS_GN2_L_SHF);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipe_get_config_input_src()
 *  Description:-This routine gets the cfg of  i/p pixel format and the pixel pos of color components
 *
 *
 *  @param   cfg ipipe_src_cfg_t * cfg:- pointer to the cfg struct

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
ISP_RETURN ipipe_get_config_input_src(ipipe_src_cfg_t * cfg)
{
    // uint32 val=0;

    ISP_ASSERT(cfg, NULL);

    cfg->wrt =
        ISP_FEXT(ipipe_reg->SRC_MODE, CSL_IPIPE_SRC_MODE_WRT_SHIFT,
                 CSL_IPIPE_SRC_MODE_WRT_MASK);
    cfg->io_pixel_fmt =
        (IPIPE_IO_PIX_FMT_T) ISP_FEXT(ipipe_reg->SRC_FMT,
                                      CSL_IPIPE_SRC_FMT_FMT_SHIFT,
                                      CSL_IPIPE_SRC_FMT_FMT_MASK);

    // val=ipipe_reg->SRC_FMT;

    cfg->Odd_line_Odd_pixel =
        (IPIPE_RAW_PIXEL_COLOR_FMT_T) ISP_FEXT(ipipe_reg->SRC_COL,
                                               CSL_IPIPE_SRC_COL_OO_SHIFT,
                                               CSL_IPIPE_SRC_COL_OO_MASK);
    cfg->Odd_line_Even_pixel =
        (IPIPE_RAW_PIXEL_COLOR_FMT_T) ISP_FEXT(ipipe_reg->SRC_COL,
                                               CSL_IPIPE_SRC_COL_OE_SHIFT,
                                               CSL_IPIPE_SRC_COL_OE_MASK);
    cfg->Even_line_Odd_pixel =
        (IPIPE_RAW_PIXEL_COLOR_FMT_T) ISP_FEXT(ipipe_reg->SRC_COL,
                                               CSL_IPIPE_SRC_COL_EO_SHIFT,
                                               CSL_IPIPE_SRC_COL_EO_MASK);
    cfg->Even_line_Even_pixel =
        (IPIPE_RAW_PIXEL_COLOR_FMT_T) ISP_FEXT(ipipe_reg->SRC_COL,
                                               CSL_IPIPE_SRC_COL_EE_SHIFT,
                                               CSL_IPIPE_SRC_COL_EE_MASK);

    cfg->dims.vps =
        ISP_FEXT(ipipe_reg->SRC_VPS, CSL_IPIPE_SRC_VPS_VAL_SHIFT,
                 CSL_IPIPE_SRC_VPS_VAL_MASK);
    cfg->dims.v_size =
        ISP_FEXT(ipipe_reg->SRC_VSZ, CSL_IPIPE_SRC_VSZ_VAL_SHIFT,
                 CSL_IPIPE_SRC_VSZ_VAL_MASK) + 1;

    cfg->dims.hps =
        ISP_FEXT(ipipe_reg->SRC_HPS, CSL_IPIPE_SRC_HPS_VAL_SHIFT,
                 CSL_IPIPE_SRC_HPS_VAL_MASK);
    cfg->dims.hsz =
        ISP_FEXT(ipipe_reg->SRC_HSZ, CSL_IPIPE_SRC_HSZ_VAL_SHIFT,
                 CSL_IPIPE_SRC_HSZ_VAL_MASK) + 1;

    return ISP_FAILURE;

}

/* ================================================================ */
/**
 *  ipipe_get_config_yuv444_to_yuv422()
 *  Description:-This routine gets the cfg of  YUV444 to YUV422
 *
 *
 *  @param   cfg ipipe_yuv444_yuv422_cfg_t * cfg:- pointer to the cfg struct

 *  @return    ISP_RETURN
 */
/*================================================================== */

ISP_RETURN ipipe_get_config_yuv444_to_yuv422(ipipe_yuv444_yuv422_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    cfg->pos =
        (IPIPE_YUV_PHASE_POS_T) ISP_FEXT(ipipe_reg->YUV_PHS,
                                         CSL_IPIPE_YUV_PHS_POS_SHIFT,
                                         CSL_IPIPE_YUV_PHS_POS_MASK);
    cfg->lpf_en =
        ISP_FEXT(ipipe_reg->YUV_PHS, CSL_IPIPE_YUV_PHS_LPF_SHIFT,
                 CSL_IPIPE_YUV_PHS_LPF_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This is a dummy function a place holder to update the registers during V blank...
 *
 *
 *  @param

 *  @return
 */
/*================================================================== */
/* ===================================================================
 *  @func     ipipe_reg_update_callback                                               
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
void ipipe_reg_update_callback(ISP_RETURN status, uint32 arg1, void *arg2)
{

    return;
}

/* 
 * 
 * CSL_Status CSL_ipipeSetCntBrtConfig(CSL_IpipeHandle hndl,
 * CSL_IpipeCntBrtConfig * data) { if (hndl == NULL || data == NULL) return
 * CSL_EFAIL;
 * 
 * CSL_FINS(hndl->regs->YUV_ADJ, IPIPE_YUV_ADJ_BRT, data->brightness);
 * CSL_FINS(hndl->regs->YUV_ADJ, IPIPE_YUV_ADJ_CTR, data->contrast);
 * 
 * return CSL_SOK; } */
