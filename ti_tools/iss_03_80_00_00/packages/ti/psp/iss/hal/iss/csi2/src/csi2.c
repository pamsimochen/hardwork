/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file csi2.c
 *
 * This file contains structures and functions for CSI2 module of Ducati subsystem in OMAP4/Monica
 *
 * @path drv_csi2/src
 *
 * @rev 1.0
 *
 * @developer: 
 */
/*========================================================================*/

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/
#include <ti/psp/iss/hal/iss/csi2/csi2.h>
#include <xdc/std.h>

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

csi2_regs_ovly csi2A_regs;

csi2_dev_data_t *csi2_devp;

csi2_dev_data_t csi2_dev;

/* ================================================================ */
/**
 *  csi2_reset() resets the CSI2 module. This is a software reset, implemented through SYSCONFIG register
 *  bit. Complex-IO interfaces have been reset separately here. It will return whether RESET was successful or 
 *  a failure.
 *
 *  @param  
 *  @return    status                      Success/failure of the Reset
 *================================================================== */
/* ===================================================================
 *  @func     csi2_reset                                               
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
CSI2_RETURN csi2_reset(CSI2_DEVICE_T device_num, uint32 reset_complexio)
{

    uint16 retrycnt = 0;

    uint8 reset_done = 0;

    uint8 reset_done_io1 = 0, reset_done_io2 = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    /* Reset through CSI2_SYSCONFIG register. */
    CSI2_SET32(csi2_regs->CSI2_SYSCONFIG, CSI2_ENABLED_BIT,
               CSI2_SYSCONFIG_SOFT_RESET_SHIFT, CSI2_SYSCONFIG_SOFT_RESET_MASK);

    /* Wait for RESET_DONE */
    while ((retrycnt++ < CSI2_RETRY_CNT) && (reset_done != 1))
    {
        reset_done =
            CSI2_FEXT(csi2_regs->CSI2_SYSSTATUS,
                      CSI2_SYSSTATUS_RESET_DONE_SHIFT,
                      CSI2_SYSSTATUS_RESET_DONE_MASK);
    }

    /* RESET the COMPLEX_IO separately */
    CSI2_SET32(csi2_regs->CSI2_COMPLEXIO_CFG1, 0,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_SHIFT,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_MASK);
    CSI2_SET32(csi2_regs->CSI2_COMPLEXIO_CFG2, 0,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_SHIFT,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_MASK);

    if (retrycnt == CSI2_RETRY_CNT)
    {
        //Vps_rprintf("\nCSI2_RESET failed!\n!");
        return CSI2_FAILURE;
    }

    if (reset_complexio)
    {

        /* reset the counter to 0 for complex-io reset_done status */
        retrycnt = 0;

        /* Wait for RESET_DONE of Complex-IO */
        while ((retrycnt++ < CSI2_RETRY_CNT) &&
               ((reset_done_io1 != 1) || (reset_done_io2 != 1)))
        {

            if (reset_done_io1 != 1)
                reset_done_io1 =
                    CSI2_FEXT(csi2_regs->CSI2_COMPLEXIO_CFG1,
                              CSI2_COMPLEXIO_CFG_RESET_DONE_SHIFT,
                              CSI2_COMPLEXIO_CFG_RESET_DONE_MASK);

            if (reset_done_io2 != 1)
                reset_done_io2 =
                    CSI2_FEXT(csi2_regs->CSI2_COMPLEXIO_CFG2,
                              CSI2_COMPLEXIO_CFG_RESET_DONE_SHIFT,
                              CSI2_COMPLEXIO_CFG_RESET_DONE_MASK);

        }

        if (retrycnt >= CSI2_RETRY_CNT)
        {
            return CSI2_FAILURE;
        }

    }

    return CSI2_SUCCESS;

}

extern CSI2_RETURN csi2_irq_init();

/* ================================================================ */
/* 
 *  Description:- Init should be called before calling any other function
 *  
 *
 *  @param   none
 
 *  @return    CSI2_RETURN     
 *================================================================== */
/* ===================================================================
 *  @func     csi2_init                                               
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
CSI2_RETURN csi2_init()
{

    CSI2_RETURN retval = CSI2_SUCCESS;

    csi2_devp = &csi2_dev;

    csi2_devp->opened = FALSE;

    /* Reset the CSI2 receiver module */
    // retval = csi2_reset();
    csi2_irq_init();
    return retval;
}

/* ================================================================ */
/* 
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable  clk
 *  
 *
 *  @param   :- none
 
 *  @return  :- CSI2_RETURN       
 *================================================================== */
/* ===================================================================
 *  @func     csi2_open                                               
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
CSI2_RETURN csi2_open()
{

    CSI2_RETURN retval = CSI2_SUCCESS;

    if (csi2_devp->opened == TRUE)
    {
        return CSI2_FAILURE;
    }

    csi2_devp->opened = TRUE;

    csi2A_regs = (csi2_regs_ovly) CSI2_BASE_ADDRESS;
    
	return retval;
}

/* ================================================================ */
/* 
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler 
 *  
 *
 *  @param   none
 
 *  @return   CSI2_RETURN      
 *================================================================== */
/* ===================================================================
 *  @func     csi2_close                                               
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
CSI2_RETURN csi2_close()
{

    CSI2_RETURN retval = CSI2_SUCCESS;

    if (csi2_devp->opened != TRUE)
    {
        return CSI2_FAILURE;
    }

    csi2_devp->opened = FALSE;
    csi2A_regs = NULL;
    
    /* Release the malloced memory csi2_dev */
    return retval;

}

/* ================================================================ */
/* 
 *  Description:- This function validates the configuration structure for errors before actually configuring CSI2. 
 *
 *  @param   csi2_cfg_t *
 
 *  @return    CSI2_RETURN     
 *================================================================== */
/* ===================================================================
 *  @func     csi2_validate_params                                               
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
CSI2_RETURN csi2_validate_params(CSI2_DEVICE_T device_num,
                                 csi2_cfg_t * csi2_config)
{

    uint32 regval = 0, ctr_context = 0, number_of_contexts = 0, on_off =
        0, retval = CSI2_SUCCESS;
    /* 
     * 1)check enabled context has the valid pointer locations for
     * configurations */

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    number_of_contexts =
        CSI2_FEXT(csi2_regs->CSI2_GNQ, CSI2_GNQ_NBCONTEXTS_SHIFT,
                  CSI2_GNQ_NBCONTEXTS_MASK);
    regval = (uint32) csi2_config->context_set_flag;

    for (ctr_context = 0; ctr_context < number_of_contexts; ctr_context++)

    {
        /* Find out whether the context is to be configured */
        on_off = regval & (0x00000001);
        regval = regval >> 1;

        if (on_off == CSI2_ENABLED_BIT && retval != CSI2_FAILURE &&
            csi2_config->ctx_cfg_ptr_array[ctr_context] != NULL)
            retval = CSI2_SUCCESS;
        else
            retval = CSI2_FAILURE;
    }

    /* 
     * 3)vp clock must be enabled if (only VP i/f is enabled and OCP i/f is
     * disabled) */

    if (retval != CSI2_FAILURE)
    {
        if ((csi2_config->control->vp_only_enable == CSI2_ENABLED_BIT) &&
            (csi2_config->control->video_clock_mode) == CSI2_DISABLE_VP_CLK)

            retval = CSI2_FAILURE;

    }

    /* 
     * 4)data lane positions 0-5 only. 6,7 are reserved */
    if (retval != CSI2_FAILURE)
    {
        if (csi2_config->complexio_cfg_1->clock_position > 5 ||
            csi2_config->complexio_cfg_1->data1_position > 5 ||
            csi2_config->complexio_cfg_1->data2_position > 5 ||
            csi2_config->complexio_cfg_1->data3_position > 5 ||
            csi2_config->complexio_cfg_1->data4_position > 5)
        {
            retval = CSI2_FAILURE;
        }

    }

    if (retval != CSI2_FAILURE)
    {

        if (csi2_config->complexio_cfg_2->clock_position > 5 ||
            csi2_config->complexio_cfg_2->data1_position > 5 ||
            csi2_config->complexio_cfg_2->data2_position > 5 ||
            csi2_config->complexio_cfg_2->data3_position > 5 ||
            csi2_config->complexio_cfg_2->data4_position > 5)
        {
            retval = CSI2_FAILURE;
        }
    }

    return retval;

}

/* ================================================================ */
/**
 *  csi2_config_power() configures the power settings of the CSI2 module. 
 *
 *  @param  master_standby_mode Standby-mode of the CSI2 module
 *  @param  auto_idle_ocp_clock OCP clock..either free running/gated
 *  @return    status                      Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_power                                               
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
CSI2_RETURN csi2_config_power(CSI2_DEVICE_T device_num,
                              CSI2_MASTER_STANDBY_MODE_T master_standby_mode,
                              CSI2_OCP_CLOCK_MODE_T auto_idle_ocp_clock)
{

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);
    CSI2_SET32(csi2_regs->CSI2_SYSCONFIG, master_standby_mode,
               CSI2_SYSCONFIG_MSTANDBY_MODE_SHIFT,
               CSI2_SYSCONFIG_MSTANDBY_MODE_MASK);
    CSI2_SET32(csi2_regs->CSI2_SYSCONFIG, auto_idle_ocp_clock,
               CSI2_SYSCONFIG_AUTO_IDLE_SHIFT, CSI2_SYSCONFIG_AUTO_IDLE_MASK);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
 *  csi2_config_control() configures the control register of CSI2. This function should be called only when CSI2 
 *  Interface has been DISABLED!!!Interface is not enabled in this function, CSI2 Control register is only configured.
 *
 *  @param  cfg  a pointer to an instance of csi2_ctrl_cfg_t structure which stores the configuration settings
 *  @return    status                      Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_control                                               
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
CSI2_RETURN csi2_config_control(CSI2_DEVICE_T device_num, csi2_ctrl_cfg_t * cfg)
{

    volatile uint32 *ptr = NULL;

    uint32 regval = 0;

    uint32 fifodepth = 0;

    uint32 iEnabled = 0;

    uint32 expanded_burst_size = 0, dma_burst_size = 0;

    uint32 streaming_32_bit = 0, streaming_mode = 0;

    uint32 video_clock_enable = 0, video_clock_ctrl = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    if (cfg == NULL)
        return CSI2_FAILURE;

    switch (cfg->streaming)
    {
        case CSI2_DISABLE_STREAMING:
            streaming_mode = CSI2_DISABLED_BIT;
            streaming_32_bit = 0;
            break;

        case CSI2_STREAMING_32BIT:
            streaming_mode = CSI2_ENABLED_BIT;
            streaming_32_bit = 1;
            break;

        case CSI2_STREAMING_64BIT:
            streaming_mode = CSI2_ENABLED_BIT;
            streaming_32_bit = 0;                          /* 64-bit
                                                            * streaming */
            break;

        default:
            return CSI2_FAILURE;
    }

    switch (cfg->dma_burst_size)
    {
        case CSI2_SIZE_1x64:
            expanded_burst_size = 0;
            dma_burst_size = CSI2_SIZE_1x64;
            break;

        case CSI2_SIZE_2x64:
            expanded_burst_size = 0;
            dma_burst_size = CSI2_SIZE_2x64;
            break;

        case CSI2_SIZE_4x64:
            expanded_burst_size = 0;
            dma_burst_size = CSI2_SIZE_4x64;
            break;

        case CSI2_SIZE_8x64:
            expanded_burst_size = 0;
            dma_burst_size = CSI2_SIZE_8x64;
            break;

        case CSI2_SIZE_16x64:
            expanded_burst_size = 1;
            dma_burst_size = CSI2_SIZE_8x64;
            /* The value written in DMA_BURST_SIZE field does not matter in
             * this case */
            break;

        default:
            return CSI2_FAILURE;
    }

    switch (cfg->video_clock_mode)
    {
        case CSI2_DISABLE_VP_CLK:
            video_clock_enable = CSI2_DISABLED_BIT;
            video_clock_ctrl = 0;
            break;

        case CSI2_OCPCLK:
            video_clock_enable = CSI2_ENABLED_BIT;
            video_clock_ctrl = 0;
            break;

        case CSI2_OCPCLK_BY_2:
            video_clock_enable = CSI2_DISABLED_BIT;
            video_clock_ctrl = 1;
            break;

        case CSI2_OCPCLK_BY_3:
            video_clock_enable = CSI2_DISABLED_BIT;
            video_clock_ctrl = 2;
            break;

        case CSI2_OCPCLK_BY_4:
            video_clock_enable = CSI2_DISABLED_BIT;
            video_clock_ctrl = 3;
            break;

        default:
            return CSI2_FAILURE;
    }

    /* Check that interface has been disabled before making any changes to
     * the CSI2_CTRL register */
    iEnabled =
        CSI2_FEXT(csi2_regs->CSI2_CTRL, CSI2_CTRL_IF_EN_SHIFT,
                  CSI2_CTRL_IF_EN_MASK);

    /* Check sufficient FIFODEPTH to allow given burst size */
    fifodepth =
        CSI2_FEXT(csi2_regs->CSI2_GNQ, CSI2_GNQ_FIFODEPTH_SHIFT,
                  CSI2_GNQ_FIFODEPTH_MASK);

    /* Check for validity of configuration structure */
    if ((cfg == NULL) ||
        (fifodepth == 2 && cfg->dma_burst_size == CSI2_SIZE_16x64) ||
        (iEnabled == CSI2_ENABLED_BIT))
        return CSI2_FAILURE;

    ptr = &(csi2_regs->CSI2_CTRL);

    /* Make a local copy of CSI2 register to avoid high-latency peripheral
     * register accesses */
    regval = *ptr;

    CSI2_SET32(regval, cfg->level_h, CSI2_CTRL_MFLAG_LEVH_SHIFT,
               CSI2_CTRL_MFLAG_LEVH_MASK);
    CSI2_SET32(regval, cfg->level_l, CSI2_CTRL_MFLAG_LEVL_SHIFT,
               CSI2_CTRL_MFLAG_LEVL_MASK);
    CSI2_SET32(regval, expanded_burst_size, CSI2_CTRL_BURST_SIZE_EXPAND_SHIFT,
               CSI2_CTRL_BURST_SIZE_EXPAND_MASK);
    CSI2_SET32(regval, video_clock_enable, CSI2_CTRL_VP_CLK_EN_SHIFT,
               CSI2_CTRL_VP_CLK_EN_MASK);
    CSI2_SET32(regval, streaming_mode, CSI2_CTRL_STREAMING_SHIFT,
               CSI2_CTRL_STREAMING_MASK);
    CSI2_SET32(regval, cfg->non_posted_write, CSI2_CTRL_NON_POSTED_WRITE_SHIFT,
               CSI2_CTRL_NON_POSTED_WRITE_MASK);
    CSI2_SET32(regval, cfg->vp_only_enable, CSI2_CTRL_VP_ONLY_EN_SHIFT,
               CSI2_CTRL_VP_ONLY_EN_MASK);
    CSI2_SET32(regval, streaming_32_bit, CSI2_CTRL_STREAMING_32_BIT_SHIFT,
               CSI2_CTRL_STREAMING_32_BIT_MASK);
    CSI2_SET32(regval, video_clock_ctrl, CSI2_CTRL_VP_OUT_CTRL_SHIFT,
               CSI2_CTRL_VP_OUT_CTRL_MASK);
    CSI2_SET32(regval, cfg->dbg_enable, CSI2_CTRL_DBG_EN_SHIFT,
               CSI2_CTRL_DBG_EN_MASK);
    CSI2_SET32(regval, dma_burst_size, CSI2_CTRL_BURST_SIZE_SHIFT,
               CSI2_CTRL_BURST_SIZE_MASK);
    CSI2_SET32(regval, cfg->little_big_endian, CSI2_CTRL_ENDIANNESS_SHIFT,
               CSI2_CTRL_ENDIANNESS_MASK);
    CSI2_SET32(regval, cfg->ecc_enable, CSI2_CTRL_ECC_EN_SHIFT,
               CSI2_CTRL_ECC_EN_MASK);
    CSI2_SET32(regval, cfg->secure_mode, CSI2_CTRL_SECURE_SHIFT,
               CSI2_CTRL_SECURE_MASK);

    /* Write back to the peripheral register */
    *ptr = regval;

    return CSI2_SUCCESS;
}

/* ===================================================================
 *  @func     csi2_set_video_port                                               
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
void csi2_set_video_port(CSI2_DEVICE_T device_num, uint32 vp_only_enable,
                         uint32 video_clock_enable)
{
    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    CSI2_SET32(csi2_regs->CSI2_CTRL, video_clock_enable,
               CSI2_CTRL_VP_CLK_EN_SHIFT, CSI2_CTRL_VP_CLK_EN_MASK);
    CSI2_SET32(csi2_regs->CSI2_CTRL, vp_only_enable, CSI2_CTRL_VP_ONLY_EN_SHIFT,
               CSI2_CTRL_VP_ONLY_EN_MASK);
}

/* ================================================================ */
/**
 *  csi2_config_complexio() configures the control register of CSI2 Complex-IO. 
 *  
 *  @param  number indicates which Complex-IO is to be configured (1 or 2)
 *  @param  cfg  a pointer to an instance of csi2_complexio_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_complexio                                               
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
CSI2_RETURN csi2_config_complexio(CSI2_DEVICE_T device_num,
                                  CSI2_COMPLEXIO_NUMBER_T number,
                                  csi2_complexio_cfg_t * cfg)
{
    volatile uint32 *ptr = NULL;

    uint32 regval = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    if (csi2A_regs == NULL)
    {
        return CSI2_FAILURE;
    }

    /* Check for validity of configuration structure */
    if ((cfg == NULL) || ((number != CSI2_IO_1) && (number != CSI2_IO_2)))
    {
        return CSI2_FAILURE;
    }

    /* point to the address of register CSI2_COMPLEXIO_2 */
    if (number == CSI2_IO_2)
        ptr = &(csi2_regs->CSI2_COMPLEXIO_CFG2);

    /* point to the address of register CSI2_COMPLEXIO_1 */
    if (number == CSI2_IO_1)
        ptr = &(csi2_regs->CSI2_COMPLEXIO_CFG1);

    if (ptr == NULL)
        return CSI2_FAILURE;

    /* Make a local copy of CSI2 register to avoid high-latency peripheral
     * register accesses */
    regval = *ptr;

    /* 0:CSI2_OFF, 1:CSI2_ON, 2:CSI2_ULP */
    CSI2_SET32(regval, cfg->power_state, CSI2_COMPLEXIO_CFG_PWR_CMD_SHIFT,
               CSI2_COMPLEXIO_CFG_PWR_CMD_MASK);

    /* Automatic switch between CSI2_ULP and CSI2_ON states--0:disable;
     * 1:enable */
    CSI2_SET32(regval, cfg->power_mode, CSI2_COMPLEXIO_CFG_PWR_AUTO_SHIFT,
               CSI2_COMPLEXIO_CFG_PWR_AUTO_MASK);

    /* Set the lane polarities */
    CSI2_SET32(regval, cfg->data4_polarity, CSI2_COMPLEXIO_CFG_DATA4_POL_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA4_POL_MASK);
    CSI2_SET32(regval, cfg->data3_polarity, CSI2_COMPLEXIO_CFG_DATA3_POL_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA3_POL_MASK);
    CSI2_SET32(regval, cfg->data2_polarity, CSI2_COMPLEXIO_CFG_DATA2_POL_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA2_POL_MASK);
    CSI2_SET32(regval, cfg->data1_polarity, CSI2_COMPLEXIO_CFG_DATA1_POL_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA1_POL_MASK);
    CSI2_SET32(regval, cfg->clock_polarity, CSI2_COMPLEXIO_CFG_CLOCK_POL_SHIFT,
               CSI2_COMPLEXIO_CFG_CLOCK_POL_MASK);

    /* Set the lane positions */
    CSI2_SET32(regval, cfg->data4_position,
               CSI2_COMPLEXIO_CFG_DATA4_POSITION_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA4_POSITION_MASK);
    CSI2_SET32(regval, cfg->data3_position,
               CSI2_COMPLEXIO_CFG_DATA3_POSITION_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA3_POSITION_MASK);
    CSI2_SET32(regval, cfg->data2_position,
               CSI2_COMPLEXIO_CFG_DATA2_POSITION_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA2_POSITION_MASK);
    CSI2_SET32(regval, cfg->data1_position,
               CSI2_COMPLEXIO_CFG_DATA1_POSITION_SHIFT,
               CSI2_COMPLEXIO_CFG_DATA1_POSITION_MASK);
    CSI2_SET32(regval, cfg->clock_position,
               CSI2_COMPLEXIO_CFG_CLOCK_POSITION_SHIFT,
               CSI2_COMPLEXIO_CFG_CLOCK_POSITION_MASK);

    /* Write back to the peripheral register */
    *ptr = regval;

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
 *  csi2_config_complexio_irq() configures the interrupt settings of the CSI2 Complex-IO. This function is made
 *  obsolete by the functions in csi2_irq.c which enable/disable a particular interrupt within complex-io. This function can
 *  be used to configure the interrupts in one-go after filling the data-structure with appropriate values.
 *  
 *  @param  number indicates which Complex-IO is to be configured (1 or 2)
 *  @param  irq_num  is a enumearated number of complexio- interrupt
 *  @param   on_off  indicates whether to enable/disable the interrupt
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_complexio_irq                                               
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
CSI2_RETURN csi2_config_complexio_irq(CSI2_DEVICE_T device_num,
                                      CSI2_COMPLEXIO_NUMBER_T number,
                                      CSI2_COMPLEXIO_INTERRUPT_ID_T irq_num,
                                      CSI2_CONTROL_FEATURE_BIT_T on_off)
{
    uint32 regval = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    if ((number != CSI2_IO_1) && (number != CSI2_IO_2) ||
        (irq_num >= MAX_CSI2_COMPLEXIO_INTERRUPTS) ||
        ((on_off != CSI2_ENABLED_BIT) && (on_off != CSI2_DISABLED_BIT)))
    {
        return CSI2_FAILURE;
    }

    if (on_off == CSI2_ENABLED_BIT)
    {
        regval = (0x00000001 << irq_num);
        if (number == CSI2_IO_2)
            csi2_regs->CSI2_COMPLEXIO2_IRQENABLE =
                (regval | csi2_regs->CSI2_COMPLEXIO2_IRQENABLE);

        /* point to the address of register CSI2_COMPLEXIO_1 */
        if (number == CSI2_IO_1)
            csi2_regs->CSI2_COMPLEXIO1_IRQENABLE =
                (regval | csi2_regs->CSI2_COMPLEXIO1_IRQENABLE);

    }
    else
    {
        regval = (~(0x00000001 << irq_num));

        if (number == CSI2_IO_2)
            csi2_regs->CSI2_COMPLEXIO2_IRQENABLE =
                (regval & csi2_regs->CSI2_COMPLEXIO2_IRQENABLE);

        /* point to the address of register CSI2_COMPLEXIO_1 */
        if (number == CSI2_IO_1)
            csi2_regs->CSI2_COMPLEXIO1_IRQENABLE =
                (regval & csi2_regs->CSI2_COMPLEXIO1_IRQENABLE);
        /* point to the address of register CSI2_COMPLEXIO_2 */

    }
    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
 *  csi2_config_phy() configures the PHY settings of the CSI2. 
 *  
 *  @param  cfg  a pointer to an instance of csi2_phy_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_phy                                               
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
CSI2_RETURN csi2_config_phy(CSI2_DEVICE_T device_num, csi2_phy_cfg_t * cfg)
{

    // // uint32* ptr=NULL;
    // // uint32 base_add=0; 

    csi2_regs_ovly csi2_regs = NULL;                       // // 

    csi2_regs = (csi2A_regs);    // // 


    if (cfg == NULL)
    {
        return CSI2_FAILURE;
    }

    /* Type Cast the Physical Base address as a pointer */
    // // ptr = (uint32*)(CSI2_PHY_BASE(base_add)); 

    /* We use memcpy() here as direct 32 bit data is to be written */
    // // memcpy((void*)(ptr),(const void*) (cfg),sizeof(csi2_phy_cfg_t)); 

    // CSI2_SET32 (csi2_regs->CSI2_PHY_CFG_00, (cfg->phy_00 & 0xFF00),
    // CSI2_PHY_REGISTER0_THS_TERM_SHIFT, CSI2_PHY_REGISTER0_THS_TERM_MASK);
    // ////

    // CSI2_SET32 (csi2_regs->CSI2_PHY_CFG_00, (cfg->phy_00 & 0xFF),
    // CSI2_PHY_REGISTER0_THS_SETTLE_SHIFT,
    // CSI2_PHY_REGISTER0_THS_SETTLE_MASK); ////
    csi2_regs->CSI2_PHY_CFG_00 = cfg->phy_00;

    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
 *  csi2_config_timing() configures the timing registers of the complex-io interface of  the CSI2 module. 
 *  It primarily sets the Stop_State_Counter in terms of number of OCP clock cycles. 
 *
 *  @param  number indicates which Complex-IO is to be configured (1 or 2)
 *  @param  cfg  a pointer to an instance of csi2_timing_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_timing                                               
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
CSI2_RETURN csi2_config_timing(CSI2_DEVICE_T device_num,
                               csi2_timing_cfg_t * cfg)
{

    uint32 timing_register = 0;

    uint16 multiplier_16_io1 = 0, multiplier_16_io2 = 0;

    uint16 multiplier_04_io1 = 0, multiplier_04_io2 = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    if ((cfg == NULL))
    {
        return CSI2_FAILURE;
    }

    switch (cfg->multiplier_io1)
    {
        case CSI2_COUNTER_x1:
            multiplier_04_io1 = 0;
            multiplier_16_io1 = 0;
            break;

        case CSI2_COUNTER_x04:
            multiplier_04_io1 = 1;
            multiplier_16_io1 = 0;
            break;

        case CSI2_COUNTER_x16:
            multiplier_04_io1 = 0;
            multiplier_16_io1 = 1;
            break;

        default:
            return CSI2_FAILURE;

    }

    switch (cfg->multiplier_io2)
    {
        case CSI2_COUNTER_x1:
            multiplier_04_io2 = 0;
            multiplier_16_io2 = 0;
            break;

        case CSI2_COUNTER_x04:
            multiplier_04_io2 = 1;
            multiplier_16_io2 = 0;
            break;

        case CSI2_COUNTER_x16:
            multiplier_04_io2 = 0;
            multiplier_16_io2 = 1;
            break;

        default:
            return CSI2_FAILURE;

    }

    /* Make a local copy of CSI2 register to avoid high-latency peripheral
     * register accesses */
    timing_register = csi2_regs->CSI2_TIMING;

    // // /* 
    CSI2_SET32(timing_register, cfg->force_rx_mode_io1,
               CSI2_TIMING_FORCE_RX_MODE_IO1_SHIFT,
               CSI2_TIMING_FORCE_RX_MODE_IO1_MASK);
    CSI2_SET32(timing_register, multiplier_16_io1,
               CSI2_TIMING_STOP_STATE_X16_IO1_SHIFT,
               CSI2_TIMING_STOP_STATE_X16_IO1_MASK);
    CSI2_SET32(timing_register, multiplier_04_io1,
               CSI2_TIMING_STOP_STATE_X4_IO1_SHIFT,
               CSI2_TIMING_STOP_STATE_X4_IO1_MASK);
    CSI2_SET32(timing_register, cfg->stop_state_counter_io1,
               CSI2_TIMING_STOP_STATE_COUNTER_IO1_SHIFT,
               CSI2_TIMING_STOP_STATE_COUNTER_IO1_MASK);

    CSI2_SET32(timing_register, cfg->force_rx_mode_io2,
               CSI2_TIMING_FORCE_RX_MODE_IO2_SHIFT,
               CSI2_TIMING_FORCE_RX_MODE_IO2_MASK);
    CSI2_SET32(timing_register, multiplier_16_io2,
               CSI2_TIMING_STOP_STATE_X16_IO2_SHIFT,
               CSI2_TIMING_STOP_STATE_X16_IO2_MASK);
    CSI2_SET32(timing_register, multiplier_04_io2,
               CSI2_TIMING_STOP_STATE_X4_IO2_SHIFT,
               CSI2_TIMING_STOP_STATE_X4_IO2_MASK);
    CSI2_SET32(timing_register, cfg->stop_state_counter_io2,
               CSI2_TIMING_STOP_STATE_COUNTER_IO2_SHIFT,
               CSI2_TIMING_STOP_STATE_COUNTER_IO2_MASK);
    // // */ 
    // *((uint32*)0x5000106C) = 0x7fffc1d6;
    /* Write back to the peripheral register */
    csi2_regs->CSI2_TIMING = 0x7fffc1d6;                   // timing_register; 
                                                           // 

    return CSI2_SUCCESS;

}

/* ================================================================ */
/* 
 *  csi2_config_context_irq() configures the interrupt settings of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  irq_id  is the enumerated interrupt id 
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_context_irq                                               
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
CSI2_RETURN csi2_config_context_irq(CSI2_DEVICE_T device_num,
                                    csi2_context_number number,
                                    CSI2_CTX_INTERRUPT_ID_T irq_id,
                                    CSI2_CONTROL_FEATURE_BIT_T on_off)
{

    uint32 *ptr_irq_enable = NULL;

    uint32 regval = 0;

    uint32 base_add;

    base_add = (uint32) csi2A_regs;

    /* pointer to the context-specific IRQ register */
    ptr_irq_enable = (uint32 *) (CSI2_CTX_IRQENABLE(base_add, number));

    regval = *ptr_irq_enable;

    if (on_off == CSI2_ENABLED_BIT)
        regval = regval | (0x00000001 << irq_id);
    else
        regval = regval & (~(0x00000001 << irq_id));

    *ptr_irq_enable = regval;

    return CSI2_SUCCESS;
}

/* ================================================================ */
/* 
 *  csi2_config_context_ping_buffer() configures the ping-address of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  address is a csi2_phyaddress_t structure which stores the ping-address
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_context_buffer                                               
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
CSI2_RETURN csi2_config_context_buffer(CSI2_DEVICE_T device_num,
                                       csi2_context_number number,
                                       CSI2_BUFFER_SELECTION_T buffer,
                                       csi2_phyaddress_t address,
                                       csi2_phyaddress_t offset)
{
    uint32 *ptr_offset = NULL;

    uint32 *ptr_buffer = NULL;

    uint32 base_add = 0;

    base_add = (uint32) csi2A_regs;

    if (buffer == CSI2_PING)
        ptr_buffer = (uint32 *) (CSI2_CTX_DAT_PING_ADDR(base_add, number));
    else if (buffer == CSI2_PONG)
        ptr_buffer = (uint32 *) (CSI2_CTX_DAT_PONG_ADDR(base_add, number));
    else
        return CSI2_FAILURE;

    ptr_offset = (uint32 *) (CSI2_CTX_DAT_OFST(base_add, number));

    /* No need to check for the frame completion. As BUFFER-address register
     * is shadowed, modifications take place after the FEC synch code is
     * received */

    *ptr_buffer = address;
    *ptr_offset = offset;

    return CSI2_SUCCESS;

}

/* ================================================================ */
/* 
 *  csi2_config_context_ping_pong_buffer() configures the ping or pong address of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  address is a csi2_phyaddress_t structure which stores the ping-address
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_context_ping_pong_buffer                                               
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
CSI2_RETURN csi2_config_context_ping_pong_buffer(CSI2_DEVICE_T device_num,
                                                 csi2_context_number number,
                                                 CSI2_BUFFER_SELECTION_T buffer,
                                                 csi2_phyaddress_t address)
{
    uint32 *ptr_buffer = NULL;

    uint32 base_add = 0;

    base_add =(uint32) csi2A_regs;

    if (buffer == CSI2_PING)
        ptr_buffer = (uint32 *) (CSI2_CTX_DAT_PING_ADDR(base_add, number));
    else if (buffer == CSI2_PONG)
        ptr_buffer = (uint32 *) (CSI2_CTX_DAT_PONG_ADDR(base_add, number));
    else
        return CSI2_FAILURE;

    *ptr_buffer = address;

    return CSI2_SUCCESS;
}

/* ================================================================ */
/* 
 *  csi2_config_context_transcode() configures the transcode settings of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  cfg is a csi2_ctx_transcode_cfg_t structure which stores the pong-address
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_context_transcode                                               
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
CSI2_RETURN csi2_config_context_transcode(CSI2_DEVICE_T device_num,
                                          csi2_context_number number,
                                          csi2_ctx_transcode_cfg_t * cfg)
{
    uint32 *ptr_transcodeh = NULL;

    uint32 *ptr_transcodev = NULL;

    uint32 *ptr_ctrl1 = NULL;

    uint32 base_add = 0;

    base_add = (uint32)(csi2A_regs);

    ptr_ctrl1 = (uint32 *) (CSI2_CTX_CTRL1(base_add, number));
    ptr_transcodeh = (uint32 *) (CSI2_CTX_TRANSCODEH(base_add, number));
    ptr_transcodev = (uint32 *) (CSI2_CTX_TRANSCODEV(base_add, number));

    CSI2_SET32(*ptr_ctrl1, cfg->transcode_mode, CSI2_CTX_CTRL1_TRANSCODE_SHIFT,
               CSI2_CTX_CTRL1_TRANSCODE_MASK);
    CSI2_SET32(*ptr_transcodeh, cfg->hcount, CSI2_CTX_TRANSCODEH_HCOUNT_SHIFT,
               CSI2_CTX_TRANSCODEH_HCOUNT_MASK);
    CSI2_SET32(*ptr_transcodeh, cfg->hskip, CSI2_CTX_TRANSCODEH_HSKIP_SHIFT,
               CSI2_CTX_TRANSCODEH_HSKIP_MASK);
    CSI2_SET32(*ptr_transcodev, cfg->vcount, CSI2_CTX_TRANSCODEV_VCOUNT_SHIFT,
               CSI2_CTX_TRANSCODEV_VCOUNT_MASK);
    CSI2_SET32(*ptr_transcodev, cfg->vskip, CSI2_CTX_TRANSCODEV_VSKIP_SHIFT,
               CSI2_CTX_TRANSCODEV_VSKIP_MASK);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
 *  csi2_config_context() configures the context of the CSI2. Context is not enabled in this function. It is
 *  only configured.
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  cfg  a pointer to an instance of csi2_ctx_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_context                                               
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
CSI2_RETURN csi2_config_context(CSI2_DEVICE_T device_num,
                                csi2_context_number number,
                                csi2_ctx_cfg_t * cfg)
{

    uint32 *ptr_ctrl1 = NULL;

    uint32 *ptr_ctrl2 = NULL;

    uint32 *ptr_ctrl3 = NULL;

    uint32 *ptr_irq = NULL;

    uint32 regval = 0;

    uint32 base_add = 0;

    base_add =
        (uint32)(csi2A_regs);

    if ((number > 7) || (cfg == NULL))
    {
        return CSI2_FAILURE;
    }

    /* pointer to the given context's register */
    ptr_ctrl1 = (uint32 *) (CSI2_CTX_CTRL1(base_add, number));
    ptr_ctrl2 = (uint32 *) (CSI2_CTX_CTRL2(base_add, number));
    ptr_ctrl3 = (uint32 *) (CSI2_CTX_CTRL3(base_add, number));
    ptr_irq = (uint32 *) (CSI2_CTX_IRQENABLE(base_add, number));

    /* CTRL1 update */
    /* Make a local copy of CSI2 register to avoid high-latency peripheral
     * register accesses */
    regval = *ptr_ctrl1;

    CSI2_SET32(regval, cfg->byteswap, CSI2_CTX_CTRL1_BYTESWAP_SHIFT,
               CSI2_CTX_CTRL1_BYTESWAP_MASK);
    CSI2_SET32(regval, cfg->generic, CSI2_CTX_CTRL1_GENERIC_SHIFT,
               CSI2_CTX_CTRL1_GENERIC_MASK);
    CSI2_SET32(regval, cfg->fec_number, CSI2_CTX_CTRL1_FEC_NUMBER_SHIFT,
               CSI2_CTX_CTRL1_FEC_NUMBER_MASK);
    CSI2_SET32(regval, cfg->count, CSI2_CTX_CTRL1_COUNT_SHIFT,
               CSI2_CTX_CTRL1_COUNT_MASK);
    CSI2_SET32(regval, cfg->eof_en, CSI2_CTX_CTRL1_EOF_EN_SHIFT,
               CSI2_CTX_CTRL1_EOF_EN_MASK);
    CSI2_SET32(regval, cfg->eol_en, CSI2_CTX_CTRL1_EOL_EN_SHIFT,
               CSI2_CTX_CTRL1_EOL_EN_MASK);
    CSI2_SET32(regval, cfg->cs_en, CSI2_CTX_CTRL1_CS_EN_SHIFT,
               CSI2_CTX_CTRL1_CS_EN_MASK);
    CSI2_SET32(regval, cfg->count_unlock, CSI2_CTX_CTRL1_COUNT_UNLOCK_SHIFT,
               CSI2_CTX_CTRL1_COUNT_UNLOCK_MASK);
    CSI2_SET32(regval, cfg->vp_force, CSI2_CTX_CTRL1_VP_FORCE_SHIFT,
               CSI2_CTX_CTRL1_VP_FORCE_MASK);
    CSI2_SET32(regval, cfg->line_modulo, CSI2_CTX_CTRL1_LINE_MODULO_SHIFT,
               CSI2_CTX_CTRL1_LINE_MODULO_MASK);

    /* Write back to the peripheral register */
    *ptr_ctrl1 = regval;

    /* CTRL2 update */
    /* Make a local copy of CSI2 register to avoid high-latency peripheral
     * register accesses */
    regval = *ptr_ctrl2;

    CSI2_SET32(regval, cfg->pixel_format, CSI2_CTX_CTRL2_USER_DEF_MAPPING_SHIFT,
               CSI2_CTX_CTRL2_USER_DEF_MAPPING_MASK);
    CSI2_SET32(regval, cfg->channel_number, CSI2_CTX_CTRL2_VIRTUAL_ID_SHIFT,
               CSI2_CTX_CTRL2_VIRTUAL_ID_MASK);
    CSI2_SET32(regval, cfg->predictor, CSI2_CTX_CTRL2_DPCM_PRED_SHIFT,
               CSI2_CTX_CTRL2_DPCM_PRED_MASK);
    CSI2_SET32(regval, cfg->data_format, CSI2_CTX_CTRL2_FORMAT_SHIFT,
               CSI2_CTX_CTRL2_FORMAT_MASK);

    /* Write back to the peripheral register */
    *ptr_ctrl2 = regval;

    /* CTRL3 update */
    CSI2_SET32(*ptr_ctrl3, cfg->alpha, CSI2_CTX_CTRL3_ALPHA_SHIFT,
               CSI2_CTX_CTRL3_ALPHA_MASK);
    CSI2_SET32(*ptr_ctrl3, cfg->line_number, CSI2_CTX_CTRL3_LINE_NUMBER_SHIFT,
               CSI2_CTX_CTRL3_LINE_NUMBER_MASK);

    /* Configure the interrupt register of the context at one go */
    /* Re-arrange the 8-bit mask for CTX_IRQ according to the
     * ctx_irq_register field */

    regval =
        ((uint32) (cfg->irq_enable) & 0x0000000F) |
        ((uint32) ((cfg->irq_enable) >> 4) << 5);
    CSI2_WRITE32(*ptr_irq, regval);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
 *  csi2_setIntr_lineNum() sets interrupt line number
 *  only configured.
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_setIntr_lineNum                                               
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
CSI2_RETURN csi2_setIntr_lineNum(CSI2_DEVICE_T device_num,
                                 csi2_context_number number, uint32 lineNum)
{

    uint32 *ptr_ctrl3 = NULL;

    uint32 base_add = 0;

    base_add =
        (uint32)(csi2A_regs);

    if (number > 7)
    {
        return CSI2_FAILURE;
    }

    /* pointer to the given context's register */
    ptr_ctrl3 = (uint32 *) (CSI2_CTX_CTRL3(base_add, number));

    CSI2_SET32(*ptr_ctrl3, lineNum, CSI2_CTX_CTRL3_LINE_NUMBER_SHIFT,
               CSI2_CTX_CTRL3_LINE_NUMBER_MASK);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
 *  csi2_config_global_interrupt() configures the global interrupt settings of the CSI2 receiver.  
 *  
 *  @param  irq_id is the enumerated interrupt id
 *  @param   on_off  indicates whether to enable/disable the interrupt
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_global_interrupt                                               
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
CSI2_RETURN csi2_config_global_interrupt(CSI2_DEVICE_T device_num,
                                         CSI2_GLOBAL_INTERRUPT_ID_T irq_id,
                                         CSI2_CONTROL_FEATURE_BIT_T on_off)
{

    uint32 regval = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    if ((irq_id >= MAX_CSI2_INTERRUPTS) ||
        ((on_off != CSI2_ENABLED_BIT) && (on_off != CSI2_DISABLED_BIT)))
    {
        return CSI2_FAILURE;
    }

    if (on_off == CSI2_ENABLED_BIT)
    {
        regval = (0x00000001 << irq_id);
        csi2_regs->CSI2_IRQENABLE = (csi2_regs->CSI2_IRQENABLE) | (regval);
    }
    else
    {
        regval = ~(0x00000001 << irq_id);
        csi2_regs->CSI2_IRQENABLE = (csi2_regs->CSI2_IRQENABLE) & (regval);
    }

    /* Set/Reset the specific interrupt */
    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
 *  csi2_init() configures the CSI2 receiver. It configures CSI2 receiver, complex-ios and PHY interface w/o
 *  enabling it. 
 *  
 *  @param  aConfig  a pointer to an instance of csi2_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config                                               
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
CSI2_RETURN csi2_config(CSI2_DEVICE_T device_num, csi2_cfg_t * aConfig)
{

    uint32 iEnabled = 0;

    CSI2_RETURN retval = CSI2_SUCCESS;

    uint8 number_of_contexts = 0;

    uint8 ctr_context = 0;

    uint16 ctr_global_irq = 0;

    uint32 regval = 0;

    CSI2_CONTROL_FEATURE_BIT_T on_off = CSI2_DISABLED_BIT;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    iEnabled =
        CSI2_FEXT(csi2_regs->CSI2_CTRL, CSI2_CTRL_IF_EN_SHIFT,
                  CSI2_CTRL_IF_EN_MASK);

    if (iEnabled)
    {
        /* CSI2 already in Use */
        return CSI2_BUSY;
    }

    /* Cofiguraion of the power mode of CSI2 */
    if (retval != CSI2_FAILURE)
        retval =
            csi2_config_power(device_num, aConfig->master_standby_mode,
                              aConfig->auto_idle_ocp_clock);

    /* Configuration of CSI2 GLOBAL CONTROL register */
    if (retval != CSI2_FAILURE)
        retval = csi2_config_control(device_num, aConfig->control);

    /* Set the PHY configurations */
    // // if(retval != CSI2_FAILURE) 
    // // retval =csi2_config_phy(device_num,aConfig->physical);

    /* Configuration complex i/o settings of the CSI2 receiver.  */

    if (retval != CSI2_FAILURE)
        retval =
            csi2_config_complexio(device_num, CSI2_IO_1,
                                  (aConfig->complexio_cfg_1));

    if (retval != CSI2_FAILURE)
        retval =
            csi2_config_complexio(device_num, CSI2_IO_2,
                                  (aConfig->complexio_cfg_2));

    /* Configuration of interrupts of complex i/o of the CSI2 receiver */

    /* IO-1 */
    if (retval != CSI2_FAILURE)
    {
        CSI2_WRITE32(csi2_regs->CSI2_COMPLEXIO1_IRQENABLE,
                     aConfig->complexio_irq_enable_1);
        /* IO-2 */
        CSI2_WRITE32(csi2_regs->CSI2_COMPLEXIO2_IRQENABLE,
                     aConfig->complexio_irq_enable_2);
    }

    /* Configuration of CSI2 timing register */

    if (retval != CSI2_FAILURE)
        retval = csi2_config_timing(device_num, (aConfig->timing_io));

    /* Find out the number of contexts supported by the module in H/W and
     * ensure oen does not exceed this limit */
    number_of_contexts =
        CSI2_FEXT(csi2_regs->CSI2_GNQ, CSI2_GNQ_NBCONTEXTS_SHIFT,
                  CSI2_GNQ_NBCONTEXTS_MASK);

    /* Setting the context info */

    regval = (uint32) aConfig->context_set_flag;

    for (ctr_context = 0; ctr_context < number_of_contexts; ctr_context++)
    {
        /* Find out whether the context is to be configured */

        on_off = (CSI2_CONTROL_FEATURE_BIT_T) (regval & (0x00000001));
        regval = regval >> 1;

        if (on_off == CSI2_ENABLED_BIT && retval != CSI2_FAILURE)
            retval =
                csi2_config_context(device_num, ctr_context,
                                    aConfig->ctx_cfg_ptr_array[ctr_context]);
    }

    /* configuring the global interrupts for CSI2, Complex i/o and Context
     * interrupts */
    regval = (uint32) aConfig->csi2_irq_enable;

    for (ctr_global_irq = 0; ctr_global_irq < MAX_CSI2_GLOBAL_INTERRUPTS;
         ctr_global_irq++)
    {
        /* Find out whether the context is to be configured */
        on_off = (CSI2_CONTROL_FEATURE_BIT_T) (regval & (0x00000001));
        regval = regval >> 1;

        if ((on_off == CSI2_ENABLED_BIT) && (retval != CSI2_FAILURE))
            retval =
                csi2_config_global_interrupt(device_num,
                                             (CSI2_GLOBAL_INTERRUPT_ID_T)
                                             ctr_global_irq, on_off);

    }

    CSI2_SET32(csi2_regs->CSI2_CTRL, CSI2_IRQ_ENABLE, CSI2_CTRL_FRAME_SHIFT,
               CSI2_CTRL_FRAME_MASK);
    // ///* 
    // release reset of COMPLEXIO in order to release reset on the PHY so
    // csi2_config_phy () will have meaning 
    CSI2_SET32(csi2_regs->CSI2_COMPLEXIO_CFG1, CSI2_IRQ_ENABLE,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_SHIFT,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_MASK);
    CSI2_SET32(csi2_regs->CSI2_COMPLEXIO_CFG2, CSI2_IRQ_ENABLE,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_SHIFT,
               CSI2_COMPLEXIO_CFG_RESET_CTRL_MASK);

	
    // Set the PHY configurations 
    if (retval != CSI2_FAILURE)
        retval = csi2_config_phy(device_num, aConfig->physical);

    return retval;
}

void csi2_wait_phy_reset()
{
	    while ((*(unsigned int *)0x55041050 & 0x20000000 )== 0);
}
/* ================================================================ */
/* 
 *  csi2_enable_context() enables the specific context of the CSI2. 
 *
 *  @param  number indicates which context  is to be enabled (0-7)
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_enable_context                                               
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
CSI2_RETURN csi2_enable_context(CSI2_DEVICE_T device_num,
                                csi2_context_number number)
{
    uint32 *ptr = NULL;

    uint32 base_add = 0;

    base_add =
        (uint32)(csi2A_regs);

    if (number > 7)
    {
        return CSI2_FAILURE;
    }

    ptr = (uint32 *) (CSI2_CTX_CTRL1(base_add, number));

    /* set the enable bit in the CSI2_CTX_CTRL1 register */
    CSI2_SET32(*ptr, CSI2_ENABLED_BIT, CSI2_CTX_CTRL1_CTX_EN_SHIFT,
               CSI2_CTX_CTRL1_CTX_EN_MASK);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/* 
 *  csi2_disable_context() disables the specific context of the CSI2. 
 *
 *  @param  number indicates which context  is to be disabled (0-7)
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_disable_context                                               
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
CSI2_RETURN csi2_disable_context(CSI2_DEVICE_T device_num,
                                 csi2_context_number number)
{
    uint32 *ptr = NULL;

    uint32 base_add = 0;

    base_add =
        (uint32)(csi2A_regs);

    if ((number > 7))
    {
        return CSI2_FAILURE;
    }

    ptr = (uint32 *) (CSI2_CTX_CTRL1(base_add, number));

    /* reset the enable bit in the CSI2_CTX_CTRL1 register */
    CSI2_SET32(*ptr, CSI2_IRQ_DISABLE, CSI2_CTX_CTRL1_CTX_EN_SHIFT,
               CSI2_CTX_CTRL1_CTX_EN_MASK);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/* 
 *  csi2_start() enables the CSI2 module, by enabling the complex-io interfaces and CSI2 receiver. User 
 *  as to separately enable the contexts (csi2_enable_context() function)which he wants to enable. 
 *  This function does not enables the contexts. For proper functioning, first enable the required context and then
 *  enable csi2 module.
 *
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_start                                               
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
CSI2_RETURN csi2_start(CSI2_DEVICE_T device_num)
{
    uint32 iEnabled = 0;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    /* Check whether CSI2 is already in use */
    iEnabled =
        CSI2_FEXT(csi2_regs->CSI2_CTRL, CSI2_CTRL_IF_EN_SHIFT,
                  CSI2_CTRL_IF_EN_MASK);

    if (iEnabled)
    {
        /* CSI2 Already in Use */
        return CSI2_BUSY;
    }

    /* De-assert the Complex-IO RESET in S/W as this bit is NOT RESET in H/W
     * automatically */
    /* IMPORTANT TO DO THIS OTHERWISE COMPLEXIO WILL NOT FUNCTION AT ALL */

    CSI2_SET32(csi2_regs->CSI2_CTRL, CSI2_IRQ_ENABLE, CSI2_CTRL_FRAME_SHIFT, CSI2_CTRL_FRAME_MASK); // //
    // // CSI2_SET32(csi2_regs->CSI2_COMPLEXIO_CFG1, CSI2_IRQ_ENABLE,
    // CSI2_COMPLEXIO_CFG_RESET_CTRL_SHIFT,
    // CSI2_COMPLEXIO_CFG_RESET_CTRL_MASK); 
    // // CSI2_SET32(csi2_regs->CSI2_COMPLEXIO_CFG2, CSI2_IRQ_ENABLE,
    // CSI2_COMPLEXIO_CFG_RESET_CTRL_SHIFT,
    // CSI2_COMPLEXIO_CFG_RESET_CTRL_MASK); 

    // Enable the Physical Interface?? How to do this??

    /* Start the CSI2 receiver */

    CSI2_SET32(csi2_regs->CSI2_CTRL, CSI2_IRQ_ENABLE, CSI2_CTRL_IF_EN_SHIFT,
               CSI2_CTRL_IF_EN_MASK);
    // csi2_regs->CSI2_CTRL = 0x00000001;

    return CSI2_SUCCESS;

}

/* ================================================================ */
/* 
 *  csi2_disable() disables the CSI2 module, by disabling the physical interface and receiver.
 *
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_disable                                               
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
CSI2_RETURN csi2_disable(CSI2_DEVICE_T device_num)
{

    uint32 iEnabled = 0;                                   // retval=CSI2_SUCCESS;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    iEnabled =
        CSI2_FEXT(csi2_regs->CSI2_CTRL, CSI2_CTRL_IF_EN_SHIFT,
                  CSI2_CTRL_IF_EN_MASK);

    if (iEnabled == 0)
    {
        return CSI2_SUCCESS;
    }

    /* Disable the Physical Interface! How to do this?? */

    /* Stop the CSI2 receiver */
    CSI2_SET32(csi2_regs->CSI2_CTRL, CSI2_IRQ_DISABLE, CSI2_CTRL_IF_EN_SHIFT,
               CSI2_CTRL_IF_EN_MASK);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/* 
 *  csi2_disable_immediate() disables the CSI2 module immediately, without waiting for the FEC 
 *  synch codes to be received.
 *
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_disable_immediate                                               
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
CSI2_RETURN csi2_disable_immediate(CSI2_DEVICE_T device_num)
{
    uint32 retval = CSI2_FAILURE;

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    /* Reset the FRAME bit in the CSI2_CTRL register */
    CSI2_SET32(csi2_regs->CSI2_CTRL, CSI2_IRQ_DISABLE, CSI2_CTRL_FRAME_SHIFT,
               CSI2_CTRL_FRAME_MASK);
    retval = csi2_disable(device_num);

    return retval;
}

/* ================================================================ */
/**
 *  csi2_get_latest_buffer() returns the last buffer which was used to store a frame. 
 *  
 *  @param  number  indicates context number (0-7)
 *  @return   buffer indicates the buffer used (ping/pong)
 *================================================================== */

/* ===================================================================
 *  @func     csi2_get_latest_buffer                                               
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
CSI2_BUFFER_SELECTION_T csi2_get_latest_buffer(CSI2_DEVICE_T device_num,
                                               csi2_context_number number)
{

    CSI2_BUFFER_SELECTION_T buffer;

    uint32 *ptr_ctrl1 = NULL;

    uint32 base_add = 0;

    base_add =
        (uint32)(csi2A_regs);

    ptr_ctrl1 = (uint32 *) (CSI2_CTX_CTRL1(base_add, number));

    buffer =
        (CSI2_BUFFER_SELECTION_T) CSI2_FEXT(*ptr_ctrl1,
                                            CSI2_CTX_CTRL1_PING_PONG_SHIFT,
                                            CSI2_CTX_CTRL1_PING_PONG_MASK);

    return buffer;
}
