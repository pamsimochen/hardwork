/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file rsz.c
 * this file implements the routines required to configure resizer.
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
#include "../inc/rsz.h"
#include "../../isp5_utils/isp5_sys_types.h"
#include "../../isp5_utils/isp5_csl_utils.h"

#include "../../common/inc/isp_common.h"

 /**/ rsz_regs_ovly rsz_reg;
rsz_A_regs_ovly rszA_reg;

rsz_B_regs_ovly rszB_reg;

rsz_dev_data_t *rsz_devp;

rsz_dev_data_t rsz_dev;

uint8 rsz_a_clk = 0;

uint8 rsz_b_clk = 0;

/* ================================================================ */
/**
 *  Description:-This routine is the inittialisation routine,nit should be called before calling any other function
 *  
 *
 *  @param   none

 *  @return    ISP_RETURN     
 */
/*================================================================== */

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
ISP_RETURN rsz_init()
{
    /* This memory can be malloc'ed */
    rsz_devp = &rsz_dev;

    rsz_devp->opened = FALSE;
    rsz_a_clk = 0;
    rsz_b_clk = 0;
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:-open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable rsz clk
 *  
 *
 *  @param  none  

 *  @return    ISP_RETURN     
 */
/*================================================================== */
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
ISP_RETURN rsz_open()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (rsz_devp == NULL)
    {
        return ISP_FAILURE;

    }
    if (rsz_devp->opened == TRUE)
    {

        return ISP_FAILURE;

    }

    rsz_devp->opened = TRUE;

    rsz_reg = (rsz_regs_ovly) RESIZER_BASE_ADDRESS;
    rszA_reg = (rsz_A_regs_ovly) & (rsz_reg->RSZ_A_REG);
    rszB_reg = (rsz_B_regs_ovly) & (rsz_reg->RSZ_B_REG);

    retval = isp_common_enable_clk(ISP_RSZ_CLK);

    /* enable the register access clk's */
    ISP_WRITE32(rsz_reg->GCK_MMR, CSL_RSZ_GCK_MMR_REG_ON);

    if (retval == ISP_SUCCESS)
    {
        // retval=isp_common_set_interrupt_handler(ISP_RSZ_INT_REG ,
        // rsz_reg_update_callback,
        // 0,0 );
    }
    return retval;

}

/* ================================================================ */
/**
 *  Description:-close will de-init the CSL reg ptr, cut rsz clk,
 *                removes the int handler 
 *  
 *
 *  @param   none

 *  @return   ISP_RETURN      
 */
/*================================================================== */

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
ISP_RETURN rsz_close()
{
    ISP_RETURN retval = ISP_SUCCESS;

    if (rsz_devp->opened != TRUE)
    {

        return ISP_FAILURE;

    }

    /* Disable register access clk's */
    ISP_WRITE32(rsz_reg->GCK_MMR, CSL_RSZ_GCK_MMR_REG_OFF);

    retval = isp_common_disable_clk(ISP_RSZ_CLK);

#if 0
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_unset_interrupt_handler(ISP_RSZ_INT_REG);
    }
#endif
    rsz_reg = NULL;
    rszA_reg = NULL;
    rszB_reg = NULL;
    rsz_devp->opened = FALSE;
    return retval;

}

/* ================================================================ */
/**
 *  Description:-this routine will cfg the common aspects of resizer A and resizer B
 *  
 *
 *  @param  cfg rsz_common_cfg_t * cfg :- pointer to cfg struct

 *  @return       ISP_RETURN  
 */
/*================================================================== */
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
ISP_RETURN rsz_common_config(rsz_common_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);

    ISP_SET32(rsz_reg->SRC_FMT0, cfg->ip_port, CSL_RSZ_SRC_FMT0_SEL_SHIFT,
              CSL_RSZ_SRC_FMT0_SEL_MASK);
    ISP_SET32(rsz_reg->SRC_MODE, cfg->wrt_enable, CSL_RSZ_SRC_MODE_WRT_SHIFT,
              CSL_RSZ_SRC_MODE_WRT_MASK);
    ISP_WRITE32(rsz_reg->DMA_RZA, cfg->rszA_dma_size);
    ISP_WRITE32(rsz_reg->DMA_RZB, cfg->rszB_dma_size);
    ISP_WRITE32(rsz_reg->FRACDIV, cfg->fracdiv);

    ISP_SET32(rsz_reg->IN_FIFO_CTRL, cfg->fifo_thr_high,
              CSL_RSZ_IN_FIFO_CTRL_THRLD_HIGH_SHIFT,
              CSL_RSZ_IN_FIFO_CTRL_THRLD_HIGH_MASK);
    ISP_SET32(rsz_reg->IN_FIFO_CTRL, cfg->fifo_thr_low,
              CSL_RSZ_IN_FIFO_CTRL_THRLD_LOW_SHIFT,
              CSL_RSZ_IN_FIFO_CTRL_THRLD_LOW_MASK);

    ISP_WRITE32(rsz_reg->YUV_PHS, cfg->yuv_phase);

    ISP_WRITE32(rsz_reg->IRQ_RZA, cfg->rszA_int_lines);
    ISP_WRITE32(rsz_reg->IRQ_RZB, cfg->rszB_int_lines);

    return (rsz_cfg_io_pixel_format(cfg->rsz_pix_fmt_mode));

}

/* ================================================================ */
/**
 *  Description:-this routine cfg's the resizer module in bypass mode .
 *  
 *
 *  @param  cfg     rsz_bypass_mode_cfg_t * cfg :- ptr to bypass cfg struct 

 *  @return         ISP_RETURN
 */
/*================================================================== */

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
ISP_RETURN rsz_bypass_cfg(rsz_bypass_mode_cfg_t * cfg)
{
    ISP_RETURN retval = ISP_SUCCESS;

    uint8 pix_fmt = 0;

    ISP_ASSERT(cfg, NULL);
    ISP_SET32(rsz_reg->SRC_FMT0, CSL_RSZ_SRC_FMT0_BYPASS_OFF,
              CSL_RSZ_SRC_FMT0_BYPASS_SHIFT, CSL_RSZ_SRC_FMT0_BYPASS_MASK);

    ISP_SET32(rsz_reg->GCK_SDR, CSL_RSZ_GCK_SDR_CORE_OFF,
              CSL_RSZ_GCK_SDR_CORE_SHIFT, CSL_RSZ_GCK_SDR_CORE_MASK);
    /* Set the pixel; format , it matters when resizer is selected. */
    if (cfg->pix_fmt == RAW_IP_RAW_OP)
    {
        pix_fmt = CSL_RSZ_SRC_FMT1_RAW__RAW;
    }
    else if (cfg->pix_fmt == YUV422_IP_YUV422_OP)
    {

        pix_fmt = CSL_RSZ_SRC_FMT1_RAW__YCC;

    }

    ISP_SET32(rsz_reg->SRC_FMT1, pix_fmt, CSL_RSZ_SRC_FMT1_RAW_SHIFT,
              CSL_RSZ_SRC_FMT1_RAW_MASK);

    /* Set the global crop dimensions */

    retval = rsz_cfg_global_crop(cfg->crop_dims);

    if (retval == ISP_SUCCESS)
        retval = rsz_common_config(cfg->rsz_common_params);
    if (retval == ISP_SUCCESS)
        retval = rsz_cfg_io_pixel_format(cfg->pix_fmt);
    return retval;

}

/* ================================================================ */
/**
 *  Description:-this routine cfg's the resizer module in passthrough mode
 *  
 *
 *  @param   cfg    rsz_passthru_mode_cfg_t * cfg :- ptr to cfg struct

 *  @return         ISP_RETURN
 */
/*================================================================== */

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
ISP_RETURN rsz_passthru_cfg(rsz_passthru_mode_cfg_t * cfg)
{

    ISP_RETURN retval = ISP_SUCCESS;

    uint8 pix_fmt = 0;

    ISP_ASSERT(cfg, NULL);
    ISP_SET32(rsz_reg->SRC_FMT0, CSL_RSZ_SRC_FMT0_BYPASS_ON,
              CSL_RSZ_SRC_FMT0_BYPASS_SHIFT, CSL_RSZ_SRC_FMT0_BYPASS_MASK);

    ISP_SET32(rsz_reg->GCK_SDR, CSL_RSZ_GCK_SDR_CORE_OFF,
              CSL_RSZ_GCK_SDR_CORE_SHIFT, CSL_RSZ_GCK_SDR_CORE_MASK);

    /* Common Configuration goes here */
    /* Flipping shouyld preserve RAW format */
    ISP_SET32(rsz_reg->SRC_FMT1, pix_fmt, CSL_RSZ_SRC_FMT1_RAW_SHIFT,
              CSL_RSZ_SRC_FMT1_RAW_MASK);

    /* Set the global crop dimensions */

    retval = rsz_cfg_global_crop(cfg->crop_dims);

    if (retval == ISP_SUCCESS)
        retval = rsz_common_config(cfg->rsz_common_params);
    if (retval == ISP_SUCCESS)
        retval = rsz_cfg_io_pixel_format(cfg->pix_fmt);

    return retval;

}

/* ================================================================ */
/**
 *  Description:-this routine cfg's the resizer in "resize" mode wherein resizing is possible
 *  
 *
 *  @param     cfg  rsz_resize_mode_cfg_t* :- pointer to resizing cfg struct

 *  @return    ISP_RETURN     
 */
/*================================================================== */
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
ISP_RETURN rsz_resize_mode_cfg(rsz_resize_mode_cfg_t * cfg)
{
    ISP_RETURN retval = ISP_SUCCESS;

    rsz_A_regs_ovly rszreg = NULL;

    rsz_ip_op_cfg_t *rsz_cfg;

    RESIZER_T i = RESIZER_A;

    ISP_ASSERT(cfg, NULL);
    retval = rsz_common_config(cfg->rsz_common_params);

    if (retval == ISP_SUCCESS)
        retval = rsz_cfg_global_crop(cfg->crop_dims);

    ISP_SET32(rsz_reg->SRC_FMT0, CSL_RSZ_SRC_FMT0_BYPASS_OFF,
              CSL_RSZ_SRC_FMT0_BYPASS_SHIFT, CSL_RSZ_SRC_FMT0_BYPASS_MASK);

    ISP_SET32(rsz_reg->GCK_SDR, CSL_RSZ_GCK_SDR_CORE_ON,
              CSL_RSZ_GCK_SDR_CORE_SHIFT, CSL_RSZ_GCK_SDR_CORE_MASK);

    for (i = RESIZER_A; i < NUM_RESIZERS; i++)
    {
        RESIZER_T resizer = i;

        if (resizer == RESIZER_A)
        {
            rsz_cfg = cfg->rsz_A_cfg;
        }
        else
        {
            rsz_cfg = cfg->rsz_B_cfg;
        }

        rszreg =
            (resizer ==
             RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                          rszB_reg);

        if (rsz_cfg == NULL)
        {
            /* Disable the resizer and go to next resizer configuration */
            ISP_WRITE32(rszreg->RZA_EN, 0);
            continue;

        }
        if (retval == ISP_SUCCESS)
            retval = rsz_clock_enable(resizer, RSZ_CLK_ENABLE);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_yuv420_op(resizer, rsz_cfg->yuv420_yc_op);

        if ((retval == ISP_SUCCESS) && (NULL != rsz_cfg->h_lpf_p))
            retval = rsz_cfg_h_lpf(resizer, rsz_cfg->h_lpf_p);

        if ((retval == ISP_SUCCESS) && (rsz_cfg->v_lpf_p))
            retval = rsz_cfg_v_lpf(resizer, rsz_cfg->v_lpf_p);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_v_phs(resizer, rsz_cfg->v_phs_p);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_h_phs(resizer, rsz_cfg->h_phs_p);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_resize_h_method(resizer, rsz_cfg->h_resize_method);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_resize_v_method(resizer, rsz_cfg->v_resize_method);

        /* Down averager config */
        ISP_WRITE32(rszreg->RZA_DWN_EN, rsz_cfg->downscale_sel);
        if (rsz_cfg->downscale_sel == RSZ_DOWNSCALE_ENABLE)
        {
            if (retval == ISP_SUCCESS)
                retval = rsz_cfg_downscale_mode(resizer, rsz_cfg->downscale_p);
        }

        /* RGB o/p config */

        ISP_WRITE32(rszreg->RZA_RGB_EN, rsz_cfg->rgb_op_sel);

        if (rsz_cfg->rgb_op_sel == RSZ_RGB_OP_ENABLE)
        {
            if (retval == ISP_SUCCESS)
                retval = rsz_cfg_rgb_op(resizer, rsz_cfg->rgb_cfg_p);

        }
        /* Flip */
        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_flip(resizer, rsz_cfg->flip_param);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_resize_ratio(resizer, rsz_cfg->ratio_p);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_ip_dims(resizer, rsz_cfg->ip_dims_p);

        if (retval == ISP_SUCCESS)
            retval = rsz_cfg_op_dims(resizer, rsz_cfg->op_dims_p);

    }

    return retval;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg flip parameters in the specified resizer
 *  
 *
 *  @param   resizer  RESIZER_T  resizer :- enum for selecting the resizer
 *  @param   flip    RSZ_FLIP_CFG_T flip :- enum specifying the flip parameter
 *
 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_flip(RESIZER_T resizer, RSZ_FLIP_CFG_T flip)
{

    if (resizer == RESIZER_A)
        ISP_SET32(rsz_reg->SEQ, flip, CSL_RSZ_SEQ_HRVA_SHIFT,
                  (CSL_RSZ_SEQ_HRVA_MASK | CSL_RSZ_SEQ_VRVA_MASK));
    else if (resizer == RESIZER_B)
        ISP_SET32(rsz_reg->SEQ, flip, CSL_RSZ_SEQ_HRVB_SHIFT,
                  (CSL_RSZ_SEQ_HRVB_MASK | CSL_RSZ_SEQ_VRVB_MASK));

    else
        return ISP_FAILURE;

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This routine will cfg YUV output related, YUV422 or Y/C in case of YUV420 parameters in the specified resizer
 *  
 *
 *  @param  resizer     RESIZER_T resizer :- enum for selecting the resizer
 *	@param	yc_sel      RSZ_YUV420_YC_SEL_T yc_sel :- enum for selecting different o/p's
 *				
 *  @return      ISP_RETURN   
 */
 /*================================================================== */

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
ISP_RETURN rsz_cfg_yuv420_op(RESIZER_T resizer, RSZ_YUV420_YC_SEL_T yc_sel)
{
    rsz_A_regs_ovly rszreg = NULL;

    RSZ_IP_COL_T col;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_420, yc_sel);
    if (RSZ_YUV420_C_OP == yc_sel)
    {
        col = YUV420_IP_COL_C;
    }
    else
    {
        col = YUV420_IP_COL_Y;
    }
    rsz_cfg_io_pixel_col(col);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This routine will validate the parameters parameters in the  resizer module in ISP
 *  !!!!!NOTE this is a place holder for now 
 *
 *  @param   cfg  rsz_config_t * cfg :- pointer to resizer cfg struct

 *  @return      ISP_RETURN   
 */
/*================================================================== */
/* ===================================================================
 *  @func     rsz_validate_params                                               
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
ISP_RETURN rsz_validate_params(rsz_config_t * cfg)
{

    ISP_ASSERT(cfg, NULL);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This is the main routine which configures the resizer in either bypsass,passthru or resize mode
 *  
 *
 *  @param    cfg   rsz_config_t * cfg:- enum for selecting the resizer

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_config(rsz_config_t * cfg)
{

    ISP_RETURN retval = ISP_FAILURE;

    ISP_ASSERT(cfg, NULL);
    retval = rsz_validate_params(cfg);

    if ((cfg->mode == RESIZER_BYPASS) || (cfg->mode == RESIZER_PASSTHRU))
    {

        if (cfg->mode == RESIZER_BYPASS)
        {
            rsz_bypass_mode_cfg_t *rsz_cfg = (cfg->mode_cfg_p->bp_mode_cfgp);

            retval = rsz_bypass_cfg(rsz_cfg);

        }

        if (cfg->mode == RESIZER_PASSTHRU)
        {
            rsz_passthru_mode_cfg_t *rsz_cfg =
                (cfg->mode_cfg_p->passthru_mode_cfgp);
            retval = rsz_passthru_cfg(rsz_cfg);
        }

    }

    else
    {
        rsz_resize_mode_cfg_t *rsz_cfg = (cfg->mode_cfg_p->resize_mode_cfgp);

        retval = rsz_resize_mode_cfg(rsz_cfg);

    }
    return retval;

}

/* ================================================================ */
/**
 *  Description:- This routine will enable clocks of the specified resizer
 *  
 *
 *  @param   resizer  RESIZER_T resizer :- enum for selecting the resizer
 *  @param    enable RSZ_MODULE_CLK_CTRL_T enable :- enum to enable /disable clock
 *
 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_clock_enable(RESIZER_T resizer, RSZ_MODULE_CLK_CTRL_T enable)
{

    switch (resizer)
    {
        case RESIZER_A:
        {
            if (enable == RSZ_CLK_ENABLE)
            {
                rsz_a_clk++;
                ISP_SET32(rsz_reg->SYSCONFIG, enable,
                          CSL_RSZ_SYSCONFIG_RSZA_CLK_EN_SHIFT,
                          CSL_RSZ_SYSCONFIG_RSZA_CLK_EN_MASK);

            }
            else
            {
                if (rsz_a_clk == 0)
                    return ISP_FAILURE;
                rsz_a_clk--;
                if (rsz_a_clk == 0)
                    ISP_SET32(rsz_reg->SYSCONFIG, enable,
                              CSL_RSZ_SYSCONFIG_RSZA_CLK_EN_SHIFT,
                              CSL_RSZ_SYSCONFIG_RSZA_CLK_EN_MASK);

            }

            break;
        }
        case RESIZER_B:
        {
            if (enable == RSZ_CLK_ENABLE)
            {
                rsz_b_clk++;
                ISP_SET32(rsz_reg->SYSCONFIG, enable,
                          CSL_RSZ_SYSCONFIG_RSZB_CLK_EN_SHIFT,
                          CSL_RSZ_SYSCONFIG_RSZB_CLK_EN_MASK);

            }
            else
            {
                if (rsz_b_clk == 0)
                    return ISP_FAILURE;
                rsz_b_clk--;
                if (rsz_b_clk == 0)
                    ISP_SET32(rsz_reg->SYSCONFIG, enable,
                              CSL_RSZ_SYSCONFIG_RSZB_CLK_EN_SHIFT,
                              CSL_RSZ_SYSCONFIG_RSZB_CLK_EN_MASK);

            }

            break;
        }
        default:
        {

            return ISP_FAILURE;
        }
    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg global crop parameters in the resizer module
 *  
 *
 *  @param  cfg rsz_global_crop_dims_t * cfg :- pointer to cfg srct

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_global_crop(rsz_global_crop_dims_t * cfg)
{

    ISP_ASSERT(cfg, NULL);
    ISP_SET32(rsz_reg->SRC_HPS, cfg->h_pos, CSL_RSZ_SRC_HPS_VAL_SHIFT,
              CSL_RSZ_SRC_HPS_VAL_MASK);
    ISP_SET32(rsz_reg->SRC_HSZ, cfg->h_size - 1, CSL_RSZ_SRC_HSZ_VAL_SHIFT,
              CSL_RSZ_SRC_HSZ_VAL_MASK);
    ISP_SET32(rsz_reg->SRC_VPS, cfg->v_pos, CSL_RSZ_SRC_VPS_VAL_SHIFT,
              CSL_RSZ_SRC_VPS_VAL_MASK);
    ISP_SET32(rsz_reg->SRC_VSZ, cfg->v_size - 1, CSL_RSZ_SRC_VSZ_VAL_SHIFT,
              CSL_RSZ_SRC_VSZ_VAL_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This routine will cfg cropping at the i/p's of the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer :- enum for selecting the resizer
 *	@param   ip_dims rsz_input_crop_dims_t * ip_dims:-crop cfg struct
 *
 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_ip_dims(RESIZER_T resizer, rsz_input_crop_dims_t * ip_dims)
{
    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_I_VPS, ip_dims->vps);
    ISP_WRITE32(rszreg->RZA_I_HPS, ip_dims->hps);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg ouput dimensions of the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer:- enum for selecting the resizer
			 op_dims rsz_output_dims_t * op_dims:- pointer to cfg struct

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_op_dims(RESIZER_T resizer, rsz_output_dims_t * op_dims)
{
    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_O_VSZ, op_dims->v_size - 1);
    ISP_SET32(rszreg->RZA_O_HSZ, op_dims->h_size / 2 - 1,
              CSL_RSZ_RZA_O_HSZ_VAL_SHIFT, CSL_RSZ_RZA_O_HSZ_VAL_MASK);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg output address parameters of the specified resizer
 *  
 *
 *  @param  resizer   RESIZER_T resizer:- enum for selecting the resizer
 * @param    op_addr_type RSZ_OP_ADDRESS_T op_addr_type :- enum for selecting what is the op pix fmt, for yuv420 2 addresses are needed,one for Y component, one for C componenet
 *@param 	address		rsz_output_addr_t * address :- pointer to cfg struct

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
                           rsz_output_addr_t * address)
{

    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    if (op_addr_type > RSZ_YUV420_C_OP)
        return ISP_FAILURE;

    if ((op_addr_type == RSZ_YUV422_RAW_RGB_OP) ||
        (op_addr_type == RSZ_YUV420_Y_OP))
    {

        ISP_WRITE32(rszreg->RZA_SDR_Y_BAD_H,
                    ((address->base_addr) >> 16) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_Y_BAD_L, (address->base_addr) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_Y_SAD_H,
                    ((address->start_addr) >> 16) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_Y_SAD_L, (address->base_addr) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_Y_PTR_E, address->end_pointer);

        ISP_WRITE32(rszreg->RZA_SDR_Y_PTR_S, address->start_pointer);

        ISP_WRITE32(rszreg->RZA_SDR_Y_OFT, address->start_offset);

    }
    else if (op_addr_type == RSZ_YUV420_C_OP)
    {
        ISP_WRITE32(rszreg->RZA_SDR_C_BAD_H,
                    ((address->base_addr) >> 16) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_C_BAD_L, (address->base_addr) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_C_SAD_H,
                    ((address->start_addr) >> 16) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_C_SAD_L, (address->base_addr) & 0x0000FFFF);

        ISP_WRITE32(rszreg->RZA_SDR_C_PTR_E, address->end_pointer);

        ISP_WRITE32(rszreg->RZA_SDR_C_PTR_S, address->start_pointer);

        ISP_WRITE32(rszreg->RZA_SDR_C_OFT, address->start_offset);

    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:-This routine will cfg the Y and C min/max values
 *  
 *
 *  @param   cfg rsz_yc_min_max * cfg :- pointer to cfg struct

 *  @return    ISP_RETURN     
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_yc_max_min(rsz_yc_min_max * cfg)
{
    ISP_ASSERT(cfg, NULL);
    ISP_WRITE32(rsz_reg->YUV_Y_MIN, cfg->y_min);
    ISP_WRITE32(rsz_reg->YUV_C_MIN, cfg->c_min);

    ISP_WRITE32(rsz_reg->YUV_Y_MAX, cfg->y_max);
    ISP_WRITE32(rsz_reg->YUV_C_MAX, cfg->c_max);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This routine cfg's the i/p to rsz
 *  
 *
 *  @param  ip_port  RSZ_IP_PORT_SEL_T ip_port :- enum specifying the input to rsz, ipipe/ipipe-if

 *  @return     ISP_RETURN    
 */
/*================================================================== */

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
void rsz_cfg_ip_port(RSZ_IP_PORT_SEL_T ip_port)
{
    ISP_SET32(rsz_reg->SRC_FMT0, ip_port, CSL_RSZ_SRC_FMT0_SEL_SHIFT,
              CSL_RSZ_SRC_FMT0_SEL_MASK);

}

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
ISP_RETURN rsz_cfg_io_pixel_col(RSZ_IP_COL_T col)
{
    ISP_SET32(rsz_reg->SRC_FMT1, col, CSL_RSZ_SRC_FMT1_COL_SHIFT,
              CSL_RSZ_SRC_FMT1_COL_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:-This rutine cfg's the registers required for the different inpt oupt pixel format types
 *  
 *
 *  @param   io_pix_fmt  RSZ_PIXFMT_MODE_T  io_pix_fmt:- enumeration of possible i/p and o/p pixel formats

 *  @return    ISP_RETURN     
 */
/*================================================================== */
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
ISP_RETURN rsz_cfg_io_pixel_format(RSZ_PIXFMT_MODE_T io_pix_fmt)
{
    if (io_pix_fmt > RAW_IP_RAW_OP)
        return ISP_FAILURE;

    switch (io_pix_fmt)
    {

        case YUV420_IP_YUV420_OP:
        {
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_COL_Y,
                      CSL_RSZ_SRC_FMT1_COL_SHIFT, CSL_RSZ_SRC_FMT1_COL_MASK);
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_420__420,
                      CSL_RSZ_SRC_FMT1_420_SHIFT, CSL_RSZ_SRC_FMT1_420_MASK);
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_RAW__YCC,
                      CSL_RSZ_SRC_FMT1_RAW_SHIFT, CSL_RSZ_SRC_FMT1_RAW_MASK);
        }
            break;

        case YUV422_IP_YUV420_OP:
        {
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_420__422,
                      CSL_RSZ_SRC_FMT1_420_SHIFT, CSL_RSZ_SRC_FMT1_420_MASK);
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_RAW__YCC,
                      CSL_RSZ_SRC_FMT1_RAW_SHIFT, CSL_RSZ_SRC_FMT1_RAW_MASK);
        }
            break;

        case YUV422_IP_YUV422_OP:
        {
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_420__422,
                      CSL_RSZ_SRC_FMT1_420_SHIFT, CSL_RSZ_SRC_FMT1_420_MASK);
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_RAW__YCC,
                      CSL_RSZ_SRC_FMT1_RAW_SHIFT, CSL_RSZ_SRC_FMT1_RAW_MASK);
        }
            break;

        case YUV422_IP_RGB_OP:
        {
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_420__422,
                      CSL_RSZ_SRC_FMT1_420_SHIFT, CSL_RSZ_SRC_FMT1_420_MASK);
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_RAW__YCC,
                      CSL_RSZ_SRC_FMT1_RAW_SHIFT, CSL_RSZ_SRC_FMT1_RAW_MASK);
        }
            break;

        case RAW_IP_RAW_OP:
        {
            ISP_SET32(rsz_reg->SRC_FMT1, CSL_RSZ_SRC_FMT1_RAW__RAW,
                      CSL_RSZ_SRC_FMT1_RAW_SHIFT, CSL_RSZ_SRC_FMT1_RAW_MASK);
        }
            break;
    }
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This routine will cfg resize ratio in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer :- enum for selecting the resizer
 *	@param   rsz_ratio rsz_resize_ratio_t * rsz_ratio :- pointer to cfg struct of horizontal and vertical resize ratios
			
 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
                                rsz_resize_ratio_t * rsz_ratio)
{
    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_V_DIF, rsz_ratio->v_rsz_ratio);
    ISP_WRITE32(rszreg->RZA_H_DIF, rsz_ratio->h_rsz_ratio);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:- This routine will cfg rescaling mode in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer :- enum for selecting the resizer
 *  @param    rsz_operation_mode     RSZ_RESCALE_MODE_T rsz_operation_mode:- enum for specifying the resizing filter, downscale / normal
                   
 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
                                   RSZ_RESCALE_MODE_T rsz_operation_mode)
{
    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_DWN_EN, rsz_operation_mode);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg the horizontal,low pass filter parameters in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer:- enum for selecting the resizer
 *	@param		cfg  rsz_lpf_cfg_t * cfg :- pointer to lpf cfg
 *
 *  @return      ISP_RETURN   
 */
/*================================================================== */
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
ISP_RETURN rsz_cfg_h_lpf(RESIZER_T resizer, rsz_lpf_cfg_t * cfg)
{
    rsz_A_regs_ovly rszreg = NULL;

    ISP_ASSERT(cfg, NULL);
    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_SET32(rszreg->RZA_H_LPF, cfg->c_intensity, CSL_RSZ_RZA_H_LPF_C_SHIFT,
              CSL_RSZ_RZA_H_LPF_C_MASK);
    ISP_SET32(rszreg->RZA_H_LPF, cfg->y_intensity, CSL_RSZ_RZA_H_LPF_Y_SHIFT,
              CSL_RSZ_RZA_H_LPF_Y_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg vertical low pass filterparameters in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer :- enum for selecting the resizer
 *  @param	 cfg rsz_lpf_cfg_t * cfg :- pointer to lpf cfg

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_v_lpf(RESIZER_T resizer, rsz_lpf_cfg_t * cfg)
{
    rsz_A_regs_ovly rszreg = NULL;

    ISP_ASSERT(cfg, NULL);
    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_SET32(rszreg->RZA_V_LPF, cfg->c_intensity, CSL_RSZ_RZA_H_LPF_C_SHIFT,
              CSL_RSZ_RZA_H_LPF_C_MASK);
    ISP_SET32(rszreg->RZA_V_LPF, cfg->y_intensity, CSL_RSZ_RZA_H_LPF_Y_SHIFT,
              CSL_RSZ_RZA_H_LPF_Y_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg horizontal resizing filter parameters in the specified resizer
 *  
 *
 *  @param  resizer RESIZER_T resizer :- enum for selecting the resizer
    @param  method  RSZ_RESIZE_METHOD_T method :- enum specifying the algo cubic convolution/linear 

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
                                   RSZ_RESIZE_METHOD_T method)
{
    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_H_TYP, method);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg vertical resizing filter parameters in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer :- enum for selecting the resizer
 	@param   method RSZ_RESIZE_METHOD_T method :- enum specifying the algo cubic convolution/linear 

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
                                   RSZ_RESIZE_METHOD_T method)
{
    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_V_TYP, method);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg downscale mode parameters in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer :- enum for selecting the resizer
 *	@param		cfg rsz_downscale_cfg_t * cfg:-pointer to cfg struct for downsscale parameters
 *
 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_downscale_mode(RESIZER_T resizer, rsz_downscale_cfg_t * cfg)
{

    rsz_A_regs_ovly rszreg = NULL;

    ISP_ASSERT(cfg, NULL);
    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_SET32(rszreg->RZA_DWN_AV, cfg->v_avg_size, CSL_RSZ_RZA_DWN_AV_V_SHIFT,
              CSL_RSZ_RZA_DWN_AV_V_MASK);
    ISP_SET32(rszreg->RZA_DWN_AV, cfg->h_avg_size, CSL_RSZ_RZA_DWN_AV_H_SHIFT,
              CSL_RSZ_RZA_DWN_AV_H_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg Horizontal phase values  in the specified resizer
 *  
 *
 *  @param   resizer   RESIZER_T resizer:- enum for selecting the resizer
 	 @param  cfg       rsz_h_phs_cfg_t * cfg :- pointer to phase settings

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_h_phs(RESIZER_T resizer, rsz_h_phs_cfg_t * cfg)
{
    rsz_A_regs_ovly rszreg = NULL;

    ISP_ASSERT(cfg, NULL);
    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_H_PHS, cfg->h_phs);
    ISP_WRITE32(rszreg->RZA_H_PHS_ADJ, cfg->h_phs_adj);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg vertical phase values  in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer:- enum for selecting the resizer
 *  @param	 rsz_v_phs_cfg_t * cfg :- pointer to phase settings

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_v_phs(RESIZER_T resizer, rsz_v_phs_cfg_t * cfg)
{
    rsz_A_regs_ovly rszreg = NULL;

    ISP_ASSERT(cfg, NULL);
    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_WRITE32(rszreg->RZA_V_PHS_C, cfg->phs_c);
    ISP_WRITE32(rszreg->RZA_V_PHS_Y, cfg->phs_y);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- This routine will cfg YUV 444 to RGB conversion parameters in the specified resizer
 *  
 *
 *  @param   resizer RESIZER_T resizer:- enum for selecting the resizer
 *  @param 	 cfg  rsz_rgb_op_cfg_t * cfg:- pointer to RGB conversion parameters

 *  @return      ISP_RETURN   
 */
/*================================================================== */

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
ISP_RETURN rsz_cfg_rgb_op(RESIZER_T resizer, rsz_rgb_op_cfg_t * cfg)
{

    rsz_A_regs_ovly rszreg = NULL;

    ISP_ASSERT(cfg, NULL);
    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    ISP_SET32(rszreg->RZA_RGB_TYP, cfg->rgb_pix_size,
              CSL_RSZ_RZA_RGB_TYP_TYP_SHIFT, CSL_RSZ_RZA_RGB_TYP_TYP_MASK);
    ISP_SET32(rszreg->RZA_RGB_TYP, cfg->mask_ctrl,
              CSL_RSZ_RZA_RGB_TYP_MSK0_SHIFT,
              (CSL_RSZ_RZA_RGB_TYP_MSK0_MASK | CSL_RSZ_RZA_RGB_TYP_MSK1_MASK));

    ISP_SET32(rszreg->RZA_RGB_BLD, cfg->rgb_alpha_blend,
              CSL_RSZ_RZA_RGB_BLD_VAL_SHIFT, CSL_RSZ_RZA_RGB_BLD_VAL_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  Description:-Place holder for updating registers at frame boundaries
 *  NOTE !!!!!!!!!!!This is incomplete
 *
 *  @param   

 *  @return         
 */
/*================================================================== */
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
void rsz_reg_update_callback(ISP_RETURN status, uint32 arg1, void *arg2)
{

    return;
}

/* ================================================================ */
/**
 *  Description:- This routine will start /stop the rsz module,
 *                 start is of one shot type or continuous typt
 *  
 *  NOTE:- This routine enables the resizer at the top level , it does not enable individual resizers!!!!
 *  @param enable RSZ_START_T enable  - enumeration of the start / stop commands

 *  @return         ISP_RETURN
 */
/*================================================================== */
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
ISP_RETURN rsz_start(RSZ_START_T enable)
{

    if (enable > RSZ_SINGLE)
    {
        return ISP_FAILURE;
    }

    switch (enable)
    {
        case RSZ_STOP:
            ISP_SET32(rsz_reg->SRC_MODE, CSL_RSZ_SRC_MODE_OST_DISABLE,
                      CSL_RSZ_SRC_MODE_OST_SHIFT, CSL_RSZ_SRC_MODE_OST_MASK);
            ISP_SET32(rsz_reg->SRC_EN, CSL_RSZ_SRC_EN_EN_DISABLE,
                      CSL_RSZ_SRC_EN_EN_SHIFT, CSL_RSZ_SRC_EN_EN_MASK);
            break;
        case RSZ_RUN:
            ISP_SET32(rsz_reg->SRC_MODE, CSL_RSZ_SRC_MODE_OST_DISABLE,
                      CSL_RSZ_SRC_MODE_OST_SHIFT, CSL_RSZ_SRC_MODE_OST_MASK);
            ISP_SET32(rsz_reg->SRC_EN, CSL_RSZ_SRC_EN_EN_ENABLE,
                      CSL_RSZ_SRC_EN_EN_SHIFT, CSL_RSZ_SRC_EN_EN_MASK);
            break;
        case RSZ_SINGLE:
            ISP_SET32(rsz_reg->SRC_MODE, CSL_RSZ_SRC_MODE_OST_ENABLE,
                      CSL_RSZ_SRC_MODE_OST_SHIFT, CSL_RSZ_SRC_MODE_OST_MASK);
            ISP_SET32(rsz_reg->SRC_EN, CSL_RSZ_SRC_EN_EN_ENABLE,
                      CSL_RSZ_SRC_EN_EN_SHIFT, CSL_RSZ_SRC_EN_EN_MASK);
            break;

    }

    return (ISP_SUCCESS);
}

/* ================================================================ */
/**
 *  Description:- This routine will start /stop the rsz module,
 *                 start is of one shot type or continuous typt
 *  
 *  NOTE:- This routine does not enable the resizer at the top level , it enables only the individual resizers, this should be used along with rsz_start() routine. 
 *
 *  @param enable RSZ_START_T enable  - enumeration of the start / stop commands
 *  @params resizer RESIZER_T resizer -enumeration which selects the resizer

 *  @return         ISP_RETURN
 */
/*================================================================== */
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
ISP_RETURN rsz_submodule_start(RSZ_START_T enable, RESIZER_T resizer)
{

    rsz_A_regs_ovly rszreg = NULL;

    rszreg =
        (resizer ==
         RESIZER_A) ? ((rsz_A_regs_ovly) rszA_reg) : ((rsz_A_regs_ovly)
                                                      rszB_reg);

    if (enable > RSZ_SINGLE)
        return ISP_FAILURE;

    switch (enable)
    {
        case RSZ_STOP:
            ISP_SET32(rszreg->RZA_EN, CSL_RSZ_RZA_EN_EN_DISABLE,
                      CSL_RSZ_RZA_EN_EN_SHIFT, CSL_RSZ_RZA_EN_EN_MASK);
            ISP_SET32(rszreg->RZA_MODE, CSL_RSZ_SRC_MODE_OST_DISABLE,
                      CSL_RSZ_SRC_MODE_OST_SHIFT, CSL_RSZ_SRC_MODE_OST_MASK);
            break;
        case RSZ_RUN:
            ISP_SET32(rszreg->RZA_MODE, CSL_RSZ_SRC_MODE_OST_DISABLE,
                      CSL_RSZ_SRC_MODE_OST_SHIFT, CSL_RSZ_SRC_MODE_OST_MASK);
            ISP_SET32(rszreg->RZA_EN, CSL_RSZ_RZA_EN_EN_ENABLE,
                      CSL_RSZ_RZA_EN_EN_SHIFT, CSL_RSZ_RZA_EN_EN_MASK);

            break;
        case RSZ_SINGLE:
            ISP_SET32(rszreg->RZA_MODE, CSL_RSZ_SRC_MODE_OST_ENABLE,
                      CSL_RSZ_SRC_MODE_OST_SHIFT, CSL_RSZ_SRC_MODE_OST_MASK);
            ISP_SET32(rszreg->RZA_EN, CSL_RSZ_RZA_EN_EN_ENABLE,
                      CSL_RSZ_RZA_EN_EN_SHIFT, CSL_RSZ_RZA_EN_EN_MASK);

            break;

    }

    return (ISP_SUCCESS);

}
