/** ==================================================================
 *  @file   ccp2.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/ccp2/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*****************************************************************************/
/* File Name :Ccp2.c */
/* */
/* Description :Program file containing all the functions for configuration,
 * enable and disable functionality for CCCP2 module */
/* */
/* Created : Ducati Team (OMAP4) */
/* */
/*****************************************************************************/
/* */
/**
 * copyright 2008-09 by Texas Instruments Incorporated. All rights           */
/* reserved. Property of Texas Instruments Incorporated. Restricted */
/* rights to use, duplicate or disclose this code are granted */
/* through contract.  */
/* "Texas Instruments OMAP(tm) Platform Software" */
/* */
/*****************************************************************************/

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include <ti/psp/iss/hal/iss/ccp2/ccp2.h>
#include "../inc/ccp2_utils.h"
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>
#if 1

CSL_Ccp2Regs ccp2_regs_samp;

ccp2_regs_ovly ccp2_reg;
#else
ipipe_regs_ovly ipipe_reg = IPIPE_REG_BASE_ADDRESS;
#endif

ccp2_dev_data_t *ccp2_devp;

ccp2_dev_data_t ccp2_dev;

/* ================================================================ */
/* 
 *  Description:- Init should be called only once before calling any other function
 *  
 *
 *   @param   none
 
 *   @return    CCP2_RETURN     
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ccp2_init                                               
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
CCP2_RETURN ccp2_init()
{
    /* This memory can be malloc'ed */
    ccp2_devp = &ccp2_dev;

    ccp2_devp->opened = DISABLE;
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/**
 * Reset_CCP2() resets CCP2
 * @param reset_state instance of CCP2_SYSCONFIG
 * @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

CCP2_RETURN Reset_CCP2(CCP2_SYSCONFIG reset_state)
{
    int status = CCP2_SUCCESS;

    uint16 retrycnt = 0;

    uint8 reset_done = 0;

    switch (reset_state)
    {
        case SOFT_RESET:
            // Reset the CCP2_SYSCONFIG register. 
            CCP2_SET32(ccp2_reg->SYSCONFIG, SOFT_RESET,
                       CSL_CCP2_SYSCONFIG_SOFT_RESET_SHIFT,
                       CSL_CCP2_SYSCONFIG_SOFT_RESET_MASK);

            while ((retrycnt++ < CCP2_RETRY_CNT) && (reset_done != 1))

            {
                reset_done =
                    CCP2_FEXT(ccp2_reg->SYSCONFIG,
                              CSL_CCP2_SYSSTATUS_RESET_DONE_SHIFT,
                              CSL_CCP2_SYSSTATUS_RESET_DONE_MASK);
                // no sleep introduced here as register read will itself take 
                // more than one cycle
                retrycnt++;
            }

            if (retrycnt == CCP2_RETRY_CNT)
            {
                return CCP2_FAILURE;
            }
            break;
        default:
            status = CCP2_INVALID_INPUT;                   // incorrect input 
                                                           // parameter
    }
    return status;
}

/* ======================================================================= */
/**
 * Config_CCP2_Power configues the power state of CCP2 
 * @param power_state instance of CCP2_SYSCONFIG
 * @return         status      CCP2_SUCCESS Succesfull open \n
 *                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     Config_CCP2_Power                                               
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
CCP2_RETURN Config_CCP2_Power(CCP2_SYSCONFIG power_state)
{
    int status = CCP2_SUCCESS;

    switch (power_state)
    {
        case MSTANDBY_MODE:

            CCP2_SET32(ccp2_reg->SYSCONFIG, MSTANDBY_MODE,
                       CSL_CCP2_SYSCONFIG_MSTANDBY_MODE_SHIFT,
                       CSL_CCP2_SYSCONFIG_MSTANDBY_MODE_MASK);

            break;
        case AUTO_IDLE:
            CCP2_SET32(ccp2_reg->SYSCONFIG, AUTO_IDLE,
                       CSL_CCP2_SYSCONFIG_AUTO_IDLE_SHIFT,
                       CSL_CCP2_SYSCONFIG_AUTO_IDLE_MASK);
            break;
        default:
            status = CCP2_INVALID_INPUT;                   // incorrect input 
                                                           // parameter
    }
    return status;
}

/* ======================================================================= */
/**
 * ccp2_config()  configures the ccp2 global control 
 *  @  param ccp2_cfg pointer to instance of object ccp2_path_enable_t
 *   @return         status      CCP2_SUCCESS Succesfull open \n
 *                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     ccp2_config                                               
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
CCP2_RETURN ccp2_config(ccp2_path_enable_t * ccp2_cfg)
{
    volatile uint32 ptr = 0;

    volatile uint32 ptr1 = 0;

/*** cONFIGURING THE GLOBAL CTRL REGISTER */

    CCP2_MODE mode_select = ccp2_cfg->ctrl_global.MODE;

/*** CONFIGURATION FOR THE CCP2 MODE */
    CCP2_POSTED posted = ccp2_cfg->ctrl_global.POSTED;

    CCP2_CTRL_BURST burst_ctrl = ccp2_cfg->ctrl_global.BURST;

    CCP2_CTRL_IO_OUT_SEL io_sel_ctrl = ccp2_cfg->ctrl_global.IO_OUT_SEL;

    CCP2_CTRL_PHY_SEL phy_sel_ctrl = ccp2_cfg->ctrl_global.PHY_SEL;

    CCP2_CTRL_VP_CLK_POL vp_clk_pol_ctrl = ccp2_cfg->ctrl_global.VP_CLK_POL;

    uint32 frac_div = ccp2_cfg->ctrl_global.FRACDIV;

    CCP2_CTRL_IF_EN if_en = ccp2_cfg->ctrl_global.IF_EN;

    CCP2_CTRL_FRAME frame_ctrl = ccp2_cfg->ctrl_global.FRAME;

    CCP2_CTRL_INV inv_ctrl = ccp2_cfg->ctrl_global.INV;

    CCP2_CTRL_VP_CLK_FORCE vp_clk_force = ccp2_cfg->ctrl_global.VP_CLK_FORCE_ON;

    uint8 debug_ctrl = ccp2_cfg->ctrl_global.CCP2_CTRL_DEBUG_EN;

    uint8 vp_only_ctrl = ccp2_cfg->ctrl_global.CCP2_CTRL_VP_ONLY_EN;

    /*** cONFIGURING THE GLOBAL CTRL REGISTER 1*/
    CCP2_CTRL1_LEVH levh = ccp2_cfg->ctrl_global.LEVH;

    CCP2_CTRL1_LEVL levl = ccp2_cfg->ctrl_global.LEVL;

    CCP2_CTRL1_BLANKING blanking = ccp2_cfg->ctrl_global.BLANKING;

    ptr = ccp2_reg->CTRL;
    ptr1 = ccp2_reg->CTRL1;

    if (mode_select == MIPI_MODE)
    {
        CCP2_SET32(ptr, mode_select, CSL_CCP2_CTRL_MODE_SHIFT,
                   CSL_CCP2_CTRL_MODE_MASK);
        /* In MIPI mode disable CCP2 SPECIFIC FEATURES like data/strobe, CRC, 
         * LOGICAL CHANNELS , RAW 6,7 FORMATS DPCM CONFIGURATION */
        CCP2_SET32(ptr, DISABLE, CSL_CCP2_CTRL_PHY_SEL_SHIFT,
                   CSL_CCP2_CTRL_PHY_SEL_MASK);
        /* Disabling CRC for channels */
        CCP2_SET32(ccp2_reg->LC0_CTRL, DISABLE, CSL_CCP2_LC0_CTRL_CRC_EN_SHIFT,
                   CSL_CCP2_LC0_CTRL_CRC_EN_MASK);
        CCP2_SET32(ccp2_reg->LC1_CTRL, DISABLE, CSL_CCP2_LC1_CTRL_CRC_EN_SHIFT,
                   CSL_CCP2_LC1_CTRL_CRC_EN_MASK);
        CCP2_SET32(ccp2_reg->LC2_CTRL, DISABLE, CSL_CCP2_LC2_CTRL_CRC_EN_SHIFT,
                   CSL_CCP2_LC2_CTRL_CRC_EN_MASK);
        CCP2_SET32(ccp2_reg->LC3_CTRL, DISABLE, CSL_CCP2_LC3_CTRL_CRC_EN_SHIFT,
                   CSL_CCP2_LC3_CTRL_CRC_EN_MASK);
        /* Disabling logical channels */
        CCP2_SET32(ccp2_reg->LC0_CTRL, DISABLE, CSL_CCP2_LC0_CTRL_CHAN_EN_SHIFT,
                   CSL_CCP2_LC0_CTRL_CHAN_EN_MASK);
        CCP2_SET32(ccp2_reg->LC1_CTRL, DISABLE, CSL_CCP2_LC1_CTRL_CHAN_EN_SHIFT,
                   CSL_CCP2_LC1_CTRL_CHAN_EN_MASK);
        CCP2_SET32(ccp2_reg->LC2_CTRL, DISABLE, CSL_CCP2_LC2_CTRL_CHAN_EN_SHIFT,
                   CSL_CCP2_LC2_CTRL_CHAN_EN_MASK);
        CCP2_SET32(ccp2_reg->LC3_CTRL, DISABLE, CSL_CCP2_LC3_CTRL_CHAN_EN_SHIFT,
                   CSL_CCP2_LC3_CTRL_CHAN_EN_MASK);
    }

    else if (mode_select == NONMIPI_CCP2_MODE)
    {
/**
 * cCP2 mode is selected*/
        CCP2_SET32(ptr, mode_select, CSL_CCP2_CTRL_MODE_SHIFT,
                   CSL_CCP2_CTRL_MODE_MASK);
        CCP2_SET32(ptr, phy_sel_ctrl, CSL_CCP2_CTRL_PHY_SEL_SHIFT,
                   CSL_CCP2_CTRL_PHY_SEL_MASK);
        CCP2_SET32(ptr, io_sel_ctrl, CSL_CCP2_CTRL_IO_OUT_SEL_SHIFT,
                   CSL_CCP2_CTRL_IO_OUT_SEL_MASK);
        CCP2_SET32(ptr, burst_ctrl, CSL_CCP2_CTRL_BURST_SHIFT,
                   CSL_CCP2_CTRL_BURST_MASK);
        CCP2_SET32(ptr, posted, CSL_CCP2_CTRL_POSTED_SHIFT,
                   CSL_CCP2_CTRL_POSTED_MASK);
        CCP2_SET32(ptr, vp_clk_pol_ctrl, CSL_CCP2_CTRL_VP_CLK_POL_SHIFT,
                   CSL_CCP2_CTRL_VP_CLK_POL_MASK);
        CCP2_SET32(ptr, frac_div, CSL_CCP2_CTRL_FRACDIV_SHIFT,
                   CSL_CCP2_CTRL_FRACDIV_MASK);
        CCP2_SET32(ptr, if_en, CSL_CCP2_CTRL_IF_EN_SHIFT,
                   CSL_CCP2_CTRL_IF_EN_MASK);
        CCP2_SET32(ptr, frame_ctrl, CSL_CCP2_CTRL_FRAME_SHIFT,
                   CSL_CCP2_CTRL_FRAME_MASK);
        CCP2_SET32(ptr, inv_ctrl, CSL_CCP2_CTRL_INV_SHIFT,
                   CSL_CCP2_CTRL_INV_MASK);
        CCP2_SET32(ptr, debug_ctrl, CSL_CCP2_CTRL_DBG_EN_SHIFT,
                   CSL_CCP2_CTRL_DBG_EN_MASK);
        CCP2_SET32(ptr, vp_only_ctrl, CSL_CCP2_CTRL_VP_ONLY_EN_SHIFT,
                   CSL_CCP2_CTRL_VP_ONLY_EN_MASK);
        CCP2_SET32(ptr, vp_clk_force, CSL_CCP2_CTRL_VP_CLK_FORCE_SHIFT,
                   CSL_CCP2_CTRL_VP_CLK_FORCE_MASK);

        CCP2_SET32(ptr1, levh, CSL_CCP2_CTRL1_LEVH_SHIFT,
                   CSL_CCP2_CTRL1_LEVH_MASK);
        CCP2_SET32(ptr1, levl, CSL_CCP2_CTRL1_LEVL_SHIFT,
                   CSL_CCP2_CTRL1_LEVL_MASK);
        CCP2_SET32(ptr1, blanking, CSL_CCP2_CTRL1_BLANKING_SHIFT,
                   CSL_CCP2_CTRL1_BLANKING_MASK);
    }
    ccp2_reg->CTRL = ptr;
    ccp2_reg->CTRL1 = ptr1;
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/**
 * ccp2_start()  enables CCp2
 *  
 *   @return         status      CCP2_SUCCESS Succesfull open \n
 *                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/
/* ===================================================================
 *  @func     ccp2_start                                               
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
CCP2_RETURN ccp2_start(ccp2_global_ctrl_flag flag)
{
    /* ENABLING THE CCP2 INTERFACE */
    // CCP2_SET32(ccp2_reg->CTRL,ENABLE,CSL_CCP2_CTRL_IF_EN_SHIFT
    // ,CSL_CCP2_CTRL_IF_EN_MASK);

    if (flag == MEMORY_READ)
    {
        /* DISABLING ALL LOGICAL CHANNELS FIRST */
        CCP2_SET32(ccp2_reg->CTRL, DISABLE, CSL_CCP2_CTRL_IF_EN_SHIFT,
                   CSL_CCP2_CTRL_IF_EN_MASK);
        /* ENABLE MEMORY READ CHANNEL */
        CCP2_SET32(ccp2_reg->LCM_CTRL, ENABLE, CSL_CCP2_LCM_CTRL_CHAN_EN_SHIFT,
                   CSL_CCP2_LCM_CTRL_CHAN_EN_MASK);

    }

    else if (flag == CHANNEL_WRITE)
    {
        return CCP2_INVALID_INPUT;
    }

    return CCP2_SUCCESS;
}

/* ======================================================================= */
/** 
*   ccp2_disable()  disables CCp2
*   @param dis_status instance of enum CCP2_DISABLE
*   @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     ccp2_disable                                               
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
CCP2_RETURN ccp2_disable(CCP2_DISABLE dis_status)
{

    CCP2_RETURN retval = CCP2_SUCCESS;

    if (dis_status == CCP2_DISABLE_AFTER_FEC)
        /* DISABLING CCP2 INTERFACE */
        CCP2_SET32(ccp2_reg->CTRL, DISABLE, CSL_CCP2_CTRL_IF_EN_SHIFT,
                   CSL_CCP2_CTRL_IF_EN_MASK);
    else
        /* DISABLING IMMEDIATELY */
        CCP2_SET32(ccp2_reg->CTRL, DISABLE, CSL_CCP2_CTRL_FRAME_SHIFT,
                   CSL_CCP2_CTRL_FRAME_MASK);
    return retval;
}

/* ======================================================================= */
/** 
*   ccp2_disableLCM()  disables CCp2
*   @param None
*   @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     ccp2_disableLCM                                               
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
CCP2_RETURN ccp2_disableLCM(void)
{

    CCP2_RETURN retval = CCP2_SUCCESS;

    /* DISABLE MEMORY READ CHANNEL */
    CCP2_SET32(ccp2_reg->LCM_CTRL, DISABLE, CSL_CCP2_LCM_CTRL_CHAN_EN_SHIFT,
               CSL_CCP2_LCM_CTRL_CHAN_EN_MASK);

    return retval;
}

/* ================================================================ */
/**
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable ipipe clk
 *  
 *
*   @param   :- none

*   @return  :- CCP2_RETURN       
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ccp2_open                                               
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
CCP2_RETURN ccp2_open()
{

    CCP2_RETURN retval = CCP2_SUCCESS;

    if (ccp2_devp->opened == ENABLE)
    {

        return CCP2_FAILURE;

    }
    ccp2_devp->opened = ENABLE;

    ccp2_reg = (ccp2_regs_ovly) CCP2_BASE_ADDRESS;

    // To DO check if ccp2 requires any clock enable & do it here 

    ccp2_reg->SYSCONFIG = 0x00001000;
    // TODO: Instead of the above hard coding the SYSCONFIG to No standby
    // mode; one could use the below statement
    // Config_CCP2_Power(MSTANDBY_MODE);

    iss_module_clk_ctrl(ISS_CCP2_CLK, ISS_CLK_ENABLE);

    return retval;

}

/* ================================================================ */
/**
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler 
 *  
 *
*   @param   none

*   @return   CCP2_RETURN      
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ccp2_close                                               
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
CCP2_RETURN ccp2_close()
{
    CCP2_RETURN retval = CCP2_SUCCESS;

    if (ccp2_devp->opened != ENABLE)
    {

        return CCP2_FAILURE;

    }
    // Added to enable power standby mode for CCP2
    Config_CCP2_Power(MSTANDBY_MODE);
    ccp2_reg = NULL;

    // To DO check if ccp2 requires any clock disabling & do it here 

    iss_module_clk_ctrl(ISS_CCP2_CLK, ISS_CLK_DISABLE);
    ccp2_devp->opened = DISABLE;
    return retval;

}

/* ======================================================================= */
/**
*  ccp2_channel_config  configues ccp2 channel
*   @param ccp2_chan_cfg pointer to instance of object ccp2_chan_enable_t
*   @param lcx_flag instance of ccp2_global_ctrl_flag
*   @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     ccp2_channel_config                                               
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
CCP2_RETURN ccp2_channel_config(ccp2_chan_enable_t * ccp2_chan_cfg,
                                ccp2_global_ctrl_flag lcx_flag)
{
/**
 * Configuring the Channel id */
    uint16 lec_code;

    uint16 lsc_code;

    uint16 fec_code;

    uint16 fsc_code;

    uint32 lcx_start_sof;

    uint32 lcx_start_eof;

    uint32 lcx_stat_eof_size;

    uint32 lcx_stat_sof_size;

    uint32 lcx_sof_addr;

    uint32 lcx_eof_addr;

    uint32 lcx_dat_start;

    uint32 lcx_dat_size;

    uint32 lcx_dat_ping_addr;

    uint32 lcx_dat_pong_addr;

    uint32 lcx_dat_ofst;

    volatile uint32 ptr_ctrl;

    volatile uint32 ptr1_ctrl;

    CCP2_CONTROL_BIT reg_status;

    CCP2_CONTROL_BIT crc_status;

    CCP2_CONTROL_BIT chan_status;

    uint32 chan_count;

    CCP2_LCX_DPCM_PRED pred_bit;

    uint32 alpha_val;

    CCP2_LCX_COUNT_UNLOCK lock_unlock;

    CCP2_LCX_CTRL_FORMAT lcx_format;

    CCP2_LCX_PING_PONG pingpong_sel;

    Channel_Number chan_id = NULL;

    if (ccp2_chan_cfg == NULL)
        return CCP2_FAILURE;

    chan_id = ccp2_chan_cfg->chan_num;

/**
 * configuring the LCX_CODE register*/
    lec_code = ccp2_chan_cfg->chan_lcx_code.LCX_CODE_LEC;
    lsc_code = ccp2_chan_cfg->chan_lcx_code.LCX_CODE_LSC;
    fec_code = ccp2_chan_cfg->chan_lcx_code.LCX_CODE_FEC;
    fsc_code = ccp2_chan_cfg->chan_lcx_code.LCX_CODE_FSC;

/**
 * configuring the LCX_STAT_START register */
    lcx_start_sof = ccp2_chan_cfg->chan_lcx_stat_start.LCX_STAT_START_SOF;
    lcx_start_eof = ccp2_chan_cfg->chan_lcx_stat_start.LCX_STAT_START_EOF;

/**
 * configuring the LCX_STAT_SIZE register */
    lcx_stat_eof_size = ccp2_chan_cfg->chan_lcx_stat_size.LCX_STAT_SIZE_SOF;
    lcx_stat_sof_size = ccp2_chan_cfg->chan_lcx_stat_size.LCX_STAT_SIZE_EOF;

/**
 * configuring the LCX_DAT_ADDR  register */
    lcx_sof_addr = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_SOF_ADDR;
    lcx_eof_addr = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_EOF_ADDR;
    lcx_dat_start = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_DAT_START;
    lcx_dat_size = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_DAT_SIZE;
    lcx_dat_ping_addr = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_DAT_PING_ADDR;
    lcx_dat_pong_addr = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_DAT_PONG_ADDR;
    lcx_dat_ofst = ccp2_chan_cfg->chan_lcx_dat_addr.LCX_DAT_OFST;

/**
 * configuring the LCX_CTRL register */

    reg_status = ccp2_chan_cfg->chan_lcx_ctrl.CCP2_LCX_REGION_EN_CTRL;
    crc_status = ccp2_chan_cfg->chan_lcx_ctrl.CCP2_LCX_CRC_EN_CTRL;
    chan_status = ccp2_chan_cfg->chan_lcx_ctrl.CCP2_LCX_ENABLE_CTRL;
    chan_count = ccp2_chan_cfg->chan_lcx_ctrl.COUNT;
    pred_bit = ccp2_chan_cfg->chan_lcx_ctrl.DPCM_PRED;
    alpha_val = ccp2_chan_cfg->chan_lcx_ctrl.LCX_ALPHA;
    lock_unlock = ccp2_chan_cfg->chan_lcx_ctrl.COUNT_UNLOCK;
    lcx_format = ccp2_chan_cfg->chan_lcx_ctrl.LCX_FORMAT;
    pingpong_sel = ccp2_chan_cfg->chan_lcx_ctrl.PING_PONG;
    /* since lcs is being enabled memory channel is being disabled */

    if (lcx_flag == CHANNEL_WRITE)
    {
        /* DISABLE MEMORY READ CHANNEL */
        CCP2_SET32(ccp2_reg->LCM_CTRL, DISABLE, CSL_CCP2_LCM_CTRL_CHAN_EN_SHIFT,
                   CSL_CCP2_LCM_CTRL_CHAN_EN_MASK);
        /* ENABLE LCX CHANNEL */
        CCP2_SET32(ccp2_reg->CTRL, ENABLE, CSL_CCP2_CTRL_IF_EN_SHIFT,
                   CSL_CCP2_CTRL_IF_EN_MASK);
    }
    else
        return CCP2_INVALID_INPUT;

    /* Geting the Generic params */

    ccp2_chan_cfg->gen_ric_params.ocp =
        CCP2_FEXT(ccp2_reg->GNQ, CSL_CCP2_GNQ_OCPREADPORT_SHIFT,
                  CSL_CCP2_GNQ_OCPREADPORT_MASK);
    ccp2_chan_cfg->gen_ric_params.no_of_channels =
        CCP2_FEXT(ccp2_reg->GNQ, CSL_CCP2_GNQ_NBCHANNELS_SHIFT,
                  CSL_CCP2_GNQ_NBCHANNELS_MASK);
    ccp2_chan_cfg->gen_ric_params.fifo_depth =
        CCP2_FEXT(ccp2_reg->GNQ, CSL_CCP2_GNQ_FIFODEPTH_SHIFT,
                  CSL_CCP2_GNQ_FIFODEPTH_MASK);

    switch (chan_id)
    {
        case 0:
            /* Configuring the LC0_CTRL register */
            ptr_ctrl = ccp2_reg->LC0_CTRL;

            CCP2_SET32(ptr_ctrl, chan_status, CSL_CCP2_LC0_CTRL_CHAN_EN_SHIFT,
                       CSL_CCP2_LC0_CTRL_CHAN_EN_MASK);
            CCP2_SET32(ptr_ctrl, reg_status, CSL_CCP2_LC0_CTRL_REGION_EN_SHIFT,
                       CSL_CCP2_LC0_CTRL_REGION_EN_MASK);
            CCP2_SET32(ptr_ctrl, crc_status, CSL_CCP2_LC0_CTRL_CRC_EN_SHIFT,
                       CSL_CCP2_LC0_CTRL_CRC_EN_MASK);
            CCP2_SET32(ptr_ctrl, chan_count, CSL_CCP2_LC0_CTRL_COUNT_SHIFT,
                       CSL_CCP2_LC0_CTRL_COUNT_MASK);

            /* ONLY IF OCP PORT IS ENABLED DPCM PRED IS THERE FOR LCX CHANNEL 
             */

            if (ccp2_chan_cfg->gen_ric_params.ocp == 1)
            {
                CCP2_SET32(ptr_ctrl, pred_bit,
                           CSL_CCP2_LC0_CTRL_DPCM_PRED_SHIFT,
                           CSL_CCP2_LC0_CTRL_DPCM_PRED_MASK);
            }
            CCP2_SET32(ptr_ctrl, alpha_val, CSL_CCP2_LC0_CTRL_ALPHA_SHIFT,
                       CSL_CCP2_LC0_CTRL_ALPHA_MASK);
            CCP2_SET32(ptr_ctrl, lock_unlock,
                       CSL_CCP2_LC0_CTRL_COUNT_UNLOCK_SHIFT,
                       CSL_CCP2_LC0_CTRL_COUNT_UNLOCK_MASK);
            CCP2_SET32(ptr_ctrl, lcx_format, CSL_CCP2_LC0_CTRL_FORMAT_SHIFT,
                       CSL_CCP2_LC0_CTRL_FORMAT_MASK);
            CCP2_SET32(ptr_ctrl, pingpong_sel,
                       CSL_CCP2_LC0_CTRL_PING_PONG_SHIFT,
                       CSL_CCP2_LC0_CTRL_PING_PONG_MASK);

            ccp2_reg->LC0_CTRL = ptr_ctrl;

            /* Configuring the LC0_CODE register */
            ptr1_ctrl = ccp2_reg->LC0_CODE;
            CCP2_SET32(ptr_ctrl, chan_id, CSL_CCP2_LC0_CODE_CHAN_ID_SHIFT,
                       CSL_CCP2_LC0_CODE_CHAN_ID_MASK);
            CCP2_SET32(ptr1_ctrl, lec_code, CSL_CCP2_LC0_CODE_LEC_SHIFT,
                       CSL_CCP2_LC0_CODE_LEC_MASK);
            CCP2_SET32(ptr1_ctrl, lsc_code, CSL_CCP2_LC0_CODE_LSC_SHIFT,
                       CSL_CCP2_LC0_CODE_LSC_MASK);
            CCP2_SET32(ptr1_ctrl, fec_code, CSL_CCP2_LC0_CODE_FEC_SHIFT,
                       CSL_CCP2_LC0_CODE_FEC_MASK);
            CCP2_SET32(ptr1_ctrl, fsc_code, CSL_CCP2_LC0_CODE_FSC_SHIFT,
                       CSL_CCP2_LC0_CODE_FSC_MASK);

            ccp2_reg->LC0_CODE = ptr1_ctrl;
            /* Configuring the LC0_STAT_START register */
            ptr_ctrl = ccp2_reg->LC0_STAT_START;
            CCP2_SET32(ptr_ctrl, lcx_start_sof,
                       CSL_CCP2_LC0_STAT_START_SOF_SHIFT,
                       CSL_CCP2_LC0_STAT_START_SOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_start_eof,
                       CSL_CCP2_LC0_STAT_START_EOF_SHIFT,
                       CSL_CCP2_LC0_STAT_START_EOF_MASK);

            ccp2_reg->LC0_STAT_START = ptr_ctrl;

            /* Configuring the LC0_STAT_SIZE register */
            ptr_ctrl = ccp2_reg->LC0_STAT_SIZE;

            CCP2_SET32(ptr_ctrl, lcx_stat_eof_size,
                       CSL_CCP2_LC0_STAT_SIZE_EOF_SHIFT,
                       CSL_CCP2_LC0_STAT_SIZE_EOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_stat_sof_size,
                       CSL_CCP2_LC0_STAT_SIZE_SOF_SHIFT,
                       CSL_CCP2_LC0_STAT_SIZE_SOF_MASK);

            ccp2_reg->LC0_STAT_SIZE = ptr_ctrl;

/**
 * configuring the LC0_DAT_ADDR  register */
            CCP2_SET32(ccp2_reg->LC0_SOF_ADDR, lcx_sof_addr,
                       CSL_CCP2_LC0_SOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC0_SOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC0_EOF_ADDR, lcx_eof_addr,
                       CSL_CCP2_LC0_EOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC0_EOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_START, lcx_dat_start,
                       CSL_CCP2_LC0_DAT_START_VERT_SHIFT,
                       CSL_CCP2_LC0_DAT_START_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_SIZE, lcx_dat_size,
                       CSL_CCP2_LC0_DAT_SIZE_VERT_SHIFT,
                       CSL_CCP2_LC0_DAT_SIZE_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_PING_ADDR, lcx_dat_ping_addr,
                       CSL_CCP2_LC0_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC0_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_PONG_ADDR, lcx_dat_pong_addr,
                       CSL_CCP2_LC0_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC0_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_OFST, lcx_dat_ofst,
                       CSL_CCP2_LC0_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC0_DAT_OFST_OFST_MASK);

            break;
        case 1:
            /* Configuring the LC1_CTRL register */
            ptr_ctrl = ccp2_reg->LC1_CTRL;
            CCP2_SET32(ptr_ctrl, chan_status, CSL_CCP2_LC1_CTRL_CHAN_EN_SHIFT,
                       CSL_CCP2_LC1_CTRL_CHAN_EN_MASK);
            CCP2_SET32(ptr_ctrl, reg_status, CSL_CCP2_LC1_CTRL_REGION_EN_SHIFT,
                       CSL_CCP2_LC1_CTRL_REGION_EN_MASK);
            CCP2_SET32(ptr_ctrl, crc_status, CSL_CCP2_LC1_CTRL_CRC_EN_SHIFT,
                       CSL_CCP2_LC1_CTRL_CRC_EN_MASK);
            CCP2_SET32(ptr_ctrl, chan_count, CSL_CCP2_LC1_CTRL_COUNT_SHIFT,
                       CSL_CCP2_LC1_CTRL_COUNT_MASK);
            /* ONLY IF OCP PORT IS ENABLED DPCM PRED IS THERE FOR LCX CHANNEL 
             */

            if (ccp2_chan_cfg->gen_ric_params.ocp == CONTROL_ENABLE)
            {
                CCP2_SET32(ptr_ctrl, pred_bit,
                           CSL_CCP2_LC1_CTRL_DPCM_PRED_SHIFT,
                           CSL_CCP2_LC1_CTRL_DPCM_PRED_MASK);
            }
            CCP2_SET32(ptr_ctrl, alpha_val, CSL_CCP2_LC1_CTRL_ALPHA_SHIFT,
                       CSL_CCP2_LC1_CTRL_ALPHA_MASK);
            CCP2_SET32(ptr_ctrl, lock_unlock,
                       CSL_CCP2_LC1_CTRL_COUNT_UNLOCK_SHIFT,
                       CSL_CCP2_LC1_CTRL_COUNT_UNLOCK_MASK);
            CCP2_SET32(ptr_ctrl, lcx_format, CSL_CCP2_LC1_CTRL_FORMAT_SHIFT,
                       CSL_CCP2_LC1_CTRL_FORMAT_MASK);
            CCP2_SET32(ptr_ctrl, pingpong_sel,
                       CSL_CCP2_LC1_CTRL_PING_PONG_SHIFT,
                       CSL_CCP2_LC1_CTRL_PING_PONG_MASK);
            ccp2_reg->LC1_CTRL = ptr_ctrl;

/**
 * configuring the LC1_CODE register*/
            ptr1_ctrl = ccp2_reg->LC1_CODE;
            CCP2_SET32(ptr_ctrl, chan_id, CSL_CCP2_LC1_CODE_CHAN_ID_SHIFT,
                       CSL_CCP2_LC1_CODE_CHAN_ID_MASK);
            CCP2_SET32(ptr1_ctrl, lec_code, CSL_CCP2_LC1_CODE_LEC_SHIFT,
                       CSL_CCP2_LC1_CODE_LEC_MASK);
            CCP2_SET32(ptr1_ctrl, lsc_code, CSL_CCP2_LC1_CODE_LSC_SHIFT,
                       CSL_CCP2_LC1_CODE_LSC_MASK);
            CCP2_SET32(ptr1_ctrl, fec_code, CSL_CCP2_LC1_CODE_FEC_SHIFT,
                       CSL_CCP2_LC1_CODE_FEC_MASK);
            CCP2_SET32(ptr1_ctrl, fsc_code, CSL_CCP2_LC1_CODE_FSC_SHIFT,
                       CSL_CCP2_LC1_CODE_FSC_MASK);
            ccp2_reg->LC1_CODE = ptr1_ctrl;

/**
 * configuring the LC1_STAT_START register */
            ptr_ctrl = ccp2_reg->LC1_STAT_START;
            CCP2_SET32(ptr_ctrl, lcx_start_sof,
                       CSL_CCP2_LC1_STAT_START_SOF_SHIFT,
                       CSL_CCP2_LC1_STAT_START_SOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_start_eof,
                       CSL_CCP2_LC1_STAT_START_EOF_SHIFT,
                       CSL_CCP2_LC1_STAT_START_EOF_MASK);
            ccp2_reg->LC1_STAT_START = ptr_ctrl;

/**
 * configuring the LC1_STAT_SIZE register */
            ptr_ctrl = ccp2_reg->LC1_STAT_SIZE;

            CCP2_SET32(ptr_ctrl, lcx_stat_eof_size,
                       CSL_CCP2_LC1_STAT_SIZE_EOF_SHIFT,
                       CSL_CCP2_LC1_STAT_SIZE_EOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_stat_sof_size,
                       CSL_CCP2_LC1_STAT_SIZE_SOF_SHIFT,
                       CSL_CCP2_LC1_STAT_SIZE_SOF_MASK);

            ccp2_reg->LC1_STAT_SIZE = ptr_ctrl;

/**
 * configuring the LC1_DAT_ADDR  register */
            CCP2_SET32(ccp2_reg->LC1_SOF_ADDR, lcx_sof_addr,
                       CSL_CCP2_LC1_SOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC1_SOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC1_EOF_ADDR, lcx_eof_addr,
                       CSL_CCP2_LC1_EOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC1_EOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_START, lcx_dat_start,
                       CSL_CCP2_LC1_DAT_START_VERT_SHIFT,
                       CSL_CCP2_LC1_DAT_START_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_SIZE, lcx_dat_size,
                       CSL_CCP2_LC1_DAT_SIZE_VERT_SHIFT,
                       CSL_CCP2_LC1_DAT_SIZE_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_PING_ADDR, lcx_dat_ping_addr,
                       CSL_CCP2_LC1_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC1_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_PONG_ADDR, lcx_dat_pong_addr,
                       CSL_CCP2_LC1_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC1_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_OFST, lcx_dat_ofst,
                       CSL_CCP2_LC1_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC1_DAT_OFST_OFST_MASK);

            break;

        case 2:
            /* Configuring the LC2_CTRL register */
            ptr_ctrl = ccp2_reg->LC2_CTRL;
            CCP2_SET32(ptr_ctrl, chan_status, CSL_CCP2_LC2_CTRL_CHAN_EN_SHIFT,
                       CSL_CCP2_LC2_CTRL_CHAN_EN_MASK);
            CCP2_SET32(ptr_ctrl, reg_status, CSL_CCP2_LC2_CTRL_REGION_EN_SHIFT,
                       CSL_CCP2_LC2_CTRL_REGION_EN_MASK);
            CCP2_SET32(ptr_ctrl, crc_status, CSL_CCP2_LC2_CTRL_CRC_EN_SHIFT,
                       CSL_CCP2_LC2_CTRL_CRC_EN_MASK);
            CCP2_SET32(ptr_ctrl, chan_count, CSL_CCP2_LC2_CTRL_COUNT_SHIFT,
                       CSL_CCP2_LC2_CTRL_COUNT_MASK);

            /* ONLY IF OCP PORT IS ENABLED DPCM PRED IS THERE FOR LCX CHANNEL 
             */

            if (ccp2_chan_cfg->gen_ric_params.ocp == 1)
            {
                CCP2_SET32(ptr_ctrl, pred_bit,
                           CSL_CCP2_LC2_CTRL_DPCM_PRED_SHIFT,
                           CSL_CCP2_LC2_CTRL_DPCM_PRED_MASK);
            }
            CCP2_SET32(ptr_ctrl, alpha_val, CSL_CCP2_LC2_CTRL_ALPHA_SHIFT,
                       CSL_CCP2_LC2_CTRL_ALPHA_MASK);
            CCP2_SET32(ptr_ctrl, lock_unlock,
                       CSL_CCP2_LC2_CTRL_COUNT_UNLOCK_SHIFT,
                       CSL_CCP2_LC2_CTRL_COUNT_UNLOCK_MASK);
            CCP2_SET32(ptr_ctrl, lcx_format, CSL_CCP2_LC2_CTRL_FORMAT_SHIFT,
                       CSL_CCP2_LC2_CTRL_FORMAT_MASK);
            CCP2_SET32(ptr_ctrl, pingpong_sel,
                       CSL_CCP2_LC2_CTRL_PING_PONG_SHIFT,
                       CSL_CCP2_LC2_CTRL_PING_PONG_MASK);

            ccp2_reg->LC2_CTRL = ptr_ctrl;
            /* Configuring the LC2_CODE register */
            ptr1_ctrl = ccp2_reg->LC2_CODE;
            CCP2_SET32(ptr_ctrl, chan_id, CSL_CCP2_LC2_CODE_CHAN_ID_SHIFT,
                       CSL_CCP2_LC2_CODE_CHAN_ID_MASK);
            CCP2_SET32(ptr1_ctrl, lec_code, CSL_CCP2_LC2_CODE_LEC_SHIFT,
                       CSL_CCP2_LC2_CODE_LEC_MASK);
            CCP2_SET32(ptr1_ctrl, lsc_code, CSL_CCP2_LC2_CODE_LSC_SHIFT,
                       CSL_CCP2_LC2_CODE_LSC_MASK);
            CCP2_SET32(ptr1_ctrl, fec_code, CSL_CCP2_LC2_CODE_FEC_SHIFT,
                       CSL_CCP2_LC2_CODE_FEC_MASK);
            CCP2_SET32(ptr1_ctrl, fsc_code, CSL_CCP2_LC2_CODE_FSC_SHIFT,
                       CSL_CCP2_LC2_CODE_FSC_MASK);
            ccp2_reg->LC2_CODE = ptr1_ctrl;

/**
 * configuring the LC2_STAT_START register */
            ptr_ctrl = ccp2_reg->LC2_STAT_START;
            CCP2_SET32(ptr_ctrl, lcx_start_sof,
                       CSL_CCP2_LC2_STAT_START_SOF_SHIFT,
                       CSL_CCP2_LC2_STAT_START_SOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_start_eof,
                       CSL_CCP2_LC2_STAT_START_EOF_SHIFT,
                       CSL_CCP2_LC2_STAT_START_EOF_MASK);

            ccp2_reg->LC2_STAT_START = ptr_ctrl;

/**
 * configuring the LC2_STAT_SIZE register */
            ptr_ctrl = ccp2_reg->LC2_STAT_SIZE;

            CCP2_SET32(ptr_ctrl, lcx_stat_eof_size,
                       CSL_CCP2_LC2_STAT_SIZE_EOF_SHIFT,
                       CSL_CCP2_LC2_STAT_SIZE_EOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_stat_sof_size,
                       CSL_CCP2_LC2_STAT_SIZE_SOF_SHIFT,
                       CSL_CCP2_LC2_STAT_SIZE_SOF_MASK);

            ccp2_reg->LC2_STAT_SIZE = ptr_ctrl;

/**
 * configuring the LC2_DAT_ADDR  register */
            CCP2_SET32(ccp2_reg->LC2_SOF_ADDR, lcx_sof_addr,
                       CSL_CCP2_LC2_SOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC2_SOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC2_EOF_ADDR, lcx_eof_addr,
                       CSL_CCP2_LC2_EOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC2_EOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_START, lcx_dat_start,
                       CSL_CCP2_LC2_DAT_START_VERT_SHIFT,
                       CSL_CCP2_LC2_DAT_START_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_SIZE, lcx_dat_size,
                       CSL_CCP2_LC2_DAT_SIZE_VERT_SHIFT,
                       CSL_CCP2_LC2_DAT_SIZE_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_PING_ADDR, lcx_dat_ping_addr,
                       CSL_CCP2_LC2_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC2_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_PONG_ADDR, lcx_dat_pong_addr,
                       CSL_CCP2_LC2_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC2_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_OFST, lcx_dat_ofst,
                       CSL_CCP2_LC2_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC2_DAT_OFST_OFST_MASK);

            break;

        case 3:
            /* Configuring the LC3_CTRL register */
            ptr_ctrl = ccp2_reg->LC3_CTRL;
            CCP2_SET32(ptr_ctrl, chan_status, CSL_CCP2_LC3_CTRL_CHAN_EN_SHIFT,
                       CSL_CCP2_LC3_CTRL_CHAN_EN_MASK);
            CCP2_SET32(ptr_ctrl, reg_status, CSL_CCP2_LC3_CTRL_REGION_EN_SHIFT,
                       CSL_CCP2_LC3_CTRL_REGION_EN_MASK);
            CCP2_SET32(ptr_ctrl, crc_status, CSL_CCP2_LC3_CTRL_CRC_EN_SHIFT,
                       CSL_CCP2_LC3_CTRL_CRC_EN_MASK);
            CCP2_SET32(ptr_ctrl, chan_count, CSL_CCP2_LC3_CTRL_COUNT_SHIFT,
                       CSL_CCP2_LC3_CTRL_COUNT_MASK);
            /* ONLY IF OCP PORT IS ENABLED DPCM PRED IS THERE FOR LCX CHANNEL 
             */

            if (ccp2_chan_cfg->gen_ric_params.ocp == 1)
            {
                CCP2_SET32(ptr_ctrl, pred_bit,
                           CSL_CCP2_LC3_CTRL_DPCM_PRED_SHIFT,
                           CSL_CCP2_LC3_CTRL_DPCM_PRED_MASK);
            }
            CCP2_SET32(ptr_ctrl, alpha_val, CSL_CCP2_LC3_CTRL_ALPHA_SHIFT,
                       CSL_CCP2_LC3_CTRL_ALPHA_MASK);
            CCP2_SET32(ptr_ctrl, lock_unlock,
                       CSL_CCP2_LC3_CTRL_COUNT_UNLOCK_SHIFT,
                       CSL_CCP2_LC3_CTRL_COUNT_UNLOCK_MASK);
            CCP2_SET32(ptr_ctrl, lcx_format, CSL_CCP2_LC3_CTRL_FORMAT_SHIFT,
                       CSL_CCP2_LC3_CTRL_FORMAT_MASK);
            CCP2_SET32(ptr_ctrl, pingpong_sel,
                       CSL_CCP2_LC3_CTRL_PING_PONG_SHIFT,
                       CSL_CCP2_LC3_CTRL_PING_PONG_MASK);

            ccp2_reg->LC3_CTRL = ptr_ctrl;

/**
 * configuring the LC3_CODE register*/
            ptr1_ctrl = ccp2_reg->LC3_CODE;
            CCP2_SET32(ptr_ctrl, chan_id, CSL_CCP2_LC3_CODE_CHAN_ID_SHIFT,
                       CSL_CCP2_LC3_CODE_CHAN_ID_MASK);
            CCP2_SET32(ptr1_ctrl, lec_code, CSL_CCP2_LC3_CODE_LEC_SHIFT,
                       CSL_CCP2_LC3_CODE_LEC_MASK);
            CCP2_SET32(ptr1_ctrl, lsc_code, CSL_CCP2_LC3_CODE_LSC_SHIFT,
                       CSL_CCP2_LC3_CODE_LSC_MASK);
            CCP2_SET32(ptr1_ctrl, fec_code, CSL_CCP2_LC3_CODE_FEC_SHIFT,
                       CSL_CCP2_LC3_CODE_FEC_MASK);
            CCP2_SET32(ptr1_ctrl, fsc_code, CSL_CCP2_LC3_CODE_FSC_SHIFT,
                       CSL_CCP2_LC3_CODE_FSC_MASK);

            ccp2_reg->LC3_CODE = ptr1_ctrl;

/**
 * configuring the LC3_STAT_START register */
            ptr_ctrl = ccp2_reg->LC3_STAT_START;
            CCP2_SET32(ptr_ctrl, lcx_start_sof,
                       CSL_CCP2_LC3_STAT_START_SOF_SHIFT,
                       CSL_CCP2_LC3_STAT_START_SOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_start_eof,
                       CSL_CCP2_LC3_STAT_START_EOF_SHIFT,
                       CSL_CCP2_LC3_STAT_START_EOF_MASK);

            ccp2_reg->LC3_STAT_START = ptr_ctrl;

/**
 * configuring the LC3_STAT_SIZE register */
            ptr_ctrl = ccp2_reg->LC3_STAT_SIZE;

            CCP2_SET32(ptr_ctrl, lcx_stat_eof_size,
                       CSL_CCP2_LC3_STAT_SIZE_EOF_SHIFT,
                       CSL_CCP2_LC3_STAT_SIZE_EOF_MASK);
            CCP2_SET32(ptr_ctrl, lcx_stat_sof_size,
                       CSL_CCP2_LC3_STAT_SIZE_SOF_SHIFT,
                       CSL_CCP2_LC3_STAT_SIZE_SOF_MASK);

            ccp2_reg->LC3_STAT_SIZE;

/**
 * configuring the LC3_DAT_ADDR  register */
            CCP2_SET32(ccp2_reg->LC3_SOF_ADDR, lcx_sof_addr,
                       CSL_CCP2_LC3_SOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC3_SOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC3_EOF_ADDR, lcx_eof_addr,
                       CSL_CCP2_LC3_EOF_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC3_EOF_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_START, lcx_dat_start,
                       CSL_CCP2_LC3_DAT_START_VERT_SHIFT,
                       CSL_CCP2_LC3_DAT_START_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_SIZE, lcx_dat_size,
                       CSL_CCP2_LC3_DAT_SIZE_VERT_SHIFT,
                       CSL_CCP2_LC3_DAT_SIZE_VERT_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_PING_ADDR, lcx_dat_ping_addr,
                       CSL_CCP2_LC3_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC3_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_PONG_ADDR, lcx_dat_pong_addr,
                       CSL_CCP2_LC3_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC3_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_OFST, lcx_dat_ofst,
                       CSL_CCP2_LC3_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC3_DAT_OFST_OFST_MASK);
            break;
        default:
            return CCP2_FAILURE;
    }
    return CCP2_SUCCESS;
}

/* Enables or Disables all fields of Interrupt register of a Channel
 * depending upon status */

/* ======================================================================= */
     /* Configure_CCP2_Lcx_Interrupt() configures the logical channel
      * interrupt * @param status handle to the enum CCP2_CONTROL_BIT *
      * @param irq_num handle to the enum CCP2_LCX_IRQ_NUM * @return status
      * CCP2_SUCCESS Succesfull open \n * Other value = Open failed and the
      * error code is returned. */
 /*=======================================================================*/

/* ===================================================================
 *  @func     Configure_CCP2_Lcx_Interrupt                                               
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
CCP2_RETURN Configure_CCP2_Lcx_Interrupt(Channel_Number number,
                                         CCP2_CONTROL_BIT status,
                                         CCP2_LCX_IRQ_NUM irq_num)
{
    volatile uint32 ptr_ctrl = 0;

    switch (number)
    {
        case 0:
            ptr_ctrl = ccp2_reg->LC01_IRQENABLE;
            switch (irq_num)
            {
                case SSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_SSC_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_SSC_IRQ_MASK);
                    break;
                case FSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FSC_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FSC_IRQ_MASK);
                    break;
                case FW_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FW_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FW_IRQ_MASK);
                    break;
                case FSP_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FSP_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FSP_IRQ_MASK);
                    break;
                case CRC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_CRC_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_CRC_IRQ_MASK);
                    break;
                case FIFO_OVF_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FIFO_OVF_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FIFO_OVF_IRQ_MASK);
                    break;
                case COUNT_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_COUNT_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_COUNT_IRQ_MASK);
                    break;
                case FE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FE_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FE_IRQ_MASK);
                    break;
                case LS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_LS_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_LS_IRQ_MASK);
                    break;
                case LE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_LE_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_LE_IRQ_MASK);
                    break;
                case FS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FS_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_FS_IRQ_MASK);
                    break;
                case OCP_ERR_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC0_OCPERROR_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC0_OCPERROR_IRQ_MASK);
                    break;
                default:
                    return CCP2_FAILURE;

            }

            ccp2_reg->LC01_IRQENABLE = ptr_ctrl;

        case 1:
            ptr_ctrl = ccp2_reg->LC01_IRQENABLE;
            switch (irq_num)
            {
                case SSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_SSC_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_SSC_IRQ_MASK);
                    break;
                case FSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FSC_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FSC_IRQ_MASK);
                    break;
                case FW_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FW_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FW_IRQ_MASK);
                    break;
                case FSP_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FSP_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FSP_IRQ_MASK);
                    break;
                case CRC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_CRC_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_CRC_IRQ_MASK);
                    break;
                case FIFO_OVF_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FIFO_OVF_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FIFO_OVF_IRQ_MASK);
                    break;
                case COUNT_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_COUNT_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_COUNT_IRQ_MASK);
                    break;
                case FE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FE_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FE_IRQ_MASK);
                    break;
                case LS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_LS_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_LS_IRQ_MASK);
                    break;
                case LE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_LE_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_LE_IRQ_MASK);
                    break;
                case FS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FS_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_FS_IRQ_MASK);
                    break;
                case OCP_ERR_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC01_IRQENABLE_LC1_OCPERROR_IRQ_SHIFT,
                               CSL_CCP2_LC01_IRQENABLE_LC1_OCPERROR_IRQ_MASK);
                    break;
                default:
                    return CCP2_FAILURE;

            }
            ccp2_reg->LC01_IRQENABLE = ptr_ctrl;

        case 2:
            ptr_ctrl = ccp2_reg->LC23_IRQENABLE;
            switch (irq_num)
            {
                case SSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_SSC_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_SSC_IRQ_MASK);
                    break;
                case FSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FSC_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FSC_IRQ_MASK);
                    break;
                case FW_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FW_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FW_IRQ_MASK);
                    break;
                case FSP_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FSP_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FSP_IRQ_MASK);
                    break;
                case CRC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_CRC_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_CRC_IRQ_MASK);
                    break;
                case FIFO_OVF_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FIFO_OVF_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FIFO_OVF_IRQ_MASK);
                    break;
                case COUNT_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_COUNT_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_COUNT_IRQ_MASK);
                    break;
                case FE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FE_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FE_IRQ_MASK);
                    break;
                case LS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_LS_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_LS_IRQ_MASK);
                    break;
                case LE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_LE_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_LE_IRQ_MASK);
                    break;
                case FS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FS_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_FS_IRQ_MASK);
                    break;
                case OCP_ERR_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC2_OCPERROR_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC2_OCPERROR_IRQ_MASK);
                    break;
                default:
                    return CCP2_FAILURE;

            }
            ccp2_reg->LC23_IRQENABLE = ptr_ctrl;
        case 3:
            ptr_ctrl = ccp2_reg->LC23_IRQENABLE;
            switch (irq_num)
            {
                case SSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_SSC_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_SSC_IRQ_MASK);
                    break;
                case FSC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FSC_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FSC_IRQ_MASK);
                    break;
                case FW_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FW_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FW_IRQ_MASK);
                    break;
                case FSP_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FSP_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FSP_IRQ_MASK);
                    break;
                case CRC_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_CRC_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_CRC_IRQ_MASK);
                    break;
                case FIFO_OVF_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FIFO_OVF_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FIFO_OVF_IRQ_MASK);
                    break;
                case COUNT_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_COUNT_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_COUNT_IRQ_MASK);
                    break;
                case FE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FE_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FE_IRQ_MASK);
                    break;
                case LS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_LS_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_LS_IRQ_MASK);
                    break;
                case LE_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_LE_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_LE_IRQ_MASK);
                    break;
                case FS_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FS_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_FS_IRQ_MASK);
                    break;
                case OCP_ERR_IRQ:
                    CCP2_SET32(ptr_ctrl, status,
                               CSL_CCP2_LC23_IRQENABLE_LC3_OCPERROR_IRQ_SHIFT,
                               CSL_CCP2_LC23_IRQENABLE_LC3_OCPERROR_IRQ_MASK);
                    break;
                default:
                    return CCP2_FAILURE;

            }
            ccp2_reg->LC23_IRQENABLE = ptr_ctrl;

    }
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/**
 * Configure_CCP2_Lcm_Interrupt() configures the memory channel interrupt for CCP2 
*   @param lcm_status handle to the enum CCP2_CONTROL_BIT
*   @param lcm_num handle to the enum CCP2_LCM_IRQ_NUM
*   @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     Configure_CCP2_Lcm_Interrupt                                               
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
CCP2_RETURN Configure_CCP2_Lcm_Interrupt(CCP2_CONTROL_BIT lcm_status,
                                         CCP2_LCM_IRQ_NUM lcm_num)
{
    switch (lcm_num)
    {
        case OCPERROR_IRQ:
            CCP2_SET32(ccp2_reg->LCM_IRQENABLE, lcm_status,
                       CSL_CCP2_LCM_IRQENABLE_LCM_OCPERROR_SHIFT,
                       CSL_CCP2_LCM_IRQENABLE_LCM_OCPERROR_MASK);
            break;
        case EOF_IRQ:
            CCP2_SET32(ccp2_reg->LCM_IRQENABLE, lcm_status,
                       CSL_CCP2_LCM_IRQENABLE_LCM_EOF_SHIFT,
                       CSL_CCP2_LCM_IRQENABLE_LCM_EOF_MASK);
            break;
        default:
            return CCP2_FAILURE;
    }
    return CCP2_SUCCESS;
}

/* ===================================================================
 *  @func     CCP2_get_irq_status                                               
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
uint8 CCP2_get_irq_status(CCP2_LCM_IRQ_NUM lcm_num)
{
    switch (lcm_num)
    {
        case OCPERROR_IRQ:
            return CCP2_FEXT(ccp2_reg->LCM_IRQSTATUS,
                             CSL_CCP2_LCM_IRQENABLE_LCM_OCPERROR_SHIFT,
                             CSL_CCP2_LCM_IRQENABLE_LCM_OCPERROR_MASK);
        case EOF_IRQ:
            return CCP2_FEXT(ccp2_reg->LCM_IRQSTATUS,
                             CSL_CCP2_LCM_IRQENABLE_LCM_EOF_SHIFT,
                             CSL_CCP2_LCM_IRQENABLE_LCM_EOF_MASK);
        default:
            return 0;
    }

}

/* ===================================================================
 *  @func     CCP2_clear_irq_status                                               
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
CCP2_RETURN CCP2_clear_irq_status(CCP2_LCM_IRQ_NUM lcm_num)
{
    switch (lcm_num)
    {
        case OCPERROR_IRQ:
            CCP2_SET32(ccp2_reg->LCM_IRQSTATUS, 1,
                       CSL_CCP2_LCM_IRQENABLE_LCM_OCPERROR_SHIFT,
                       CSL_CCP2_LCM_IRQENABLE_LCM_OCPERROR_MASK);
            break;
        case EOF_IRQ:
            CCP2_SET32(ccp2_reg->LCM_IRQSTATUS, 1,
                       CSL_CCP2_LCM_IRQENABLE_LCM_EOF_SHIFT,
                       CSL_CCP2_LCM_IRQENABLE_LCM_EOF_MASK);
            break;
        default:
            return CCP2_FAILURE;
    }
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/**
 * cCP2_Read_from_Memory() configures the read from memory channel
*   @param lcm_enable instance of object ccp2_lcm_enable_t
*   @param flag handle to the ccp2_global_ctrl_flag
*   @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     CCP2_Read_from_Memory_Config                                               
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
CCP2_RETURN CCP2_Read_from_Memory_Config(ccp2_lcm_enable_t * lcm_enable,
                                         ccp2_global_ctrl_flag flag)
{

/**
 * cONFIGURING THE LCM CHANNEL*/

    uint32 lcm_dst_addr = lcm_enable->mem_chan_config.LCM_DST_ADDR;

    uint32 lcm_dst_ofst = lcm_enable->mem_chan_config.LCM_DST_OFST;

    uint32 lcm_hsize_count = lcm_enable->mem_chan_config.LCM_HSIZE_COUNT;

    uint32 lcm_hsize_skip = lcm_enable->mem_chan_config.LCM_HSIZE_SKIP;

    uint32 lcm_prefetch = lcm_enable->mem_chan_config.LCM_PREFETCH_SWORDS;

    uint32 lcm_src_addr = lcm_enable->mem_chan_config.LCM_SRC_ADDR;

    uint32 lcm_src_ofst = lcm_enable->mem_chan_config.LCM_SRC_OFST;

    uint32 lcm_vsize = lcm_enable->mem_chan_config.LCM_VSIZE_COUNT;

    volatile uint32 ptr_ctrl = 0;

    /**
     * configuring LCM Ctrl register */
    CCP2_LCM_DST_PACK dst_pack = lcm_enable->mem_chan_ctrl.LCM_DST_PACK;

    CCP2_LCM_DPCM_PRED dst_dpcm_pred =
        lcm_enable->mem_chan_ctrl.LCM_DST_DPCM_PRED;
    CCP2_LCM_DST_COMPR dst_dpcm_en =
        lcm_enable->mem_chan_ctrl.LCM_DST_COMPRESSION;
    CCP2_LCM_SRC_PACK src_pack = lcm_enable->mem_chan_ctrl.LCM_SRC_PACK;

    CCP2_LCM_DPCM_PRED src_dpcm_pred =
        lcm_enable->mem_chan_ctrl.LCM_SRC_DPCM_PRED;
    CCP2_LCM_SRC_DECOMPR src_dpcm_en =
        lcm_enable->mem_chan_ctrl.LCM_SRC_DECOPRESS;
    CCP2_LCM_FORMAT src_format = lcm_enable->mem_chan_ctrl.LCM_SRC_FORMAT;

    CCP2_CTRL_BURST lcm_burstsize = lcm_enable->mem_chan_ctrl.LCM_BURST_SIZE;

    CCP2_LCM_READ_THROTTLE read_throttle =
        lcm_enable->mem_chan_ctrl.LCM_READ_THROTTLE;
    CCP2_DST_PORT dst_port = lcm_enable->mem_chan_ctrl.DST_PORT;

    CCP2_LCM_FORMAT dst_format = lcm_enable->mem_chan_ctrl.LCM_DST_FORMAT;

    // uint32 mem_debug = lcm_enable->mem_chan_ctrl.DEBUG;

    if (flag == MEMORY_READ)
    {

/**
 * Check address 32- bit alignment 
*/

        if ((lcm_src_addr & 0x0000001F) != 0)
            return CCP2_INVALID_INPUT;

        CCP2_SET32(ccp2_reg->LCM_DST_ADDR, lcm_dst_addr,
                   CSL_CCP2_LCM_DST_ADDR_ADDR_SHIFT,
                   CSL_CCP2_LCM_DST_ADDR_ADDR_MASK);
        CCP2_SET32(ccp2_reg->LCM_DST_OFST, lcm_dst_ofst,
                   CSL_CCP2_LCM_DST_OFST_OFST_SHIFT,
                   CSL_CCP2_LCM_DST_OFST_OFST_MASK);
        CCP2_SET32(ccp2_reg->LCM_HSIZE, lcm_hsize_count,
                   CSL_CCP2_LCM_HSIZE_COUNT_SHIFT,
                   CSL_CCP2_LCM_HSIZE_COUNT_MASK);
        CCP2_SET32(ccp2_reg->LCM_HSIZE, lcm_hsize_skip,
                   CSL_CCP2_LCM_HSIZE_SKIP_SHIFT, CSL_CCP2_LCM_HSIZE_SKIP_MASK);
        CCP2_SET32(ccp2_reg->LCM_PREFETCH, lcm_prefetch,
                   CSL_CCP2_LCM_PREFETCH_HWORDS_SHIFT,
                   CSL_CCP2_LCM_PREFETCH_HWORDS_MASK);

        CCP2_WRITE32(ccp2_reg->LCM_SRC_ADDR, lcm_src_addr);

        // CCP2_SET32(ccp2_reg->LCM_SRC_ADDR,lcm_src_addr,CSL_CCP2_LCM_SRC_ADDR_ADDR_SHIFT,CSL_CCP2_LCM_SRC_ADDR_ADDR_MASK);
        CCP2_SET32(ccp2_reg->LCM_SRC_OFST, lcm_src_ofst,
                   CSL_CCP2_LCM_SRC_OFST_OFST_SHIFT,
                   CSL_CCP2_LCM_SRC_OFST_OFST_MASK);
        CCP2_SET32(ccp2_reg->LCM_VSIZE, lcm_vsize,
                   CSL_CCP2_LCM_VSIZE_COUNT_SHIFT,
                   CSL_CCP2_LCM_VSIZE_COUNT_MASK);
        ptr_ctrl = ccp2_reg->LCM_CTRL;
        CCP2_SET32(ptr_ctrl, dst_pack, CSL_CCP2_LCM_CTRL_DST_PACK_SHIFT,
                   CSL_CCP2_LCM_CTRL_DST_PACK_MASK);
        CCP2_SET32(ptr_ctrl, dst_dpcm_en, CSL_CCP2_LCM_CTRL_DST_COMPR_SHIFT,
                   CSL_CCP2_LCM_CTRL_DST_COMPR_MASK);
        if (dst_dpcm_en == CONTROL_ENABLE)
        {
            CCP2_SET32(ptr_ctrl, dst_dpcm_pred,
                       CSL_CCP2_LCM_CTRL_DST_DPCM_PRED_SHIFT,
                       CSL_CCP2_LCM_CTRL_DST_DPCM_PRED_MASK);
        }
        CCP2_SET32(ptr_ctrl, src_pack, CSL_CCP2_LCM_CTRL_SRC_PACK_SHIFT,
                   CSL_CCP2_LCM_CTRL_SRC_PACK_MASK);
        CCP2_SET32(ptr_ctrl, src_dpcm_en, CSL_CCP2_LCM_CTRL_SRC_DECOMPR_SHIFT,
                   CSL_CCP2_LCM_CTRL_SRC_DECOMPR_MASK);
        if (src_dpcm_en == CONTROL_ENABLE)
        {
            CCP2_SET32(ptr_ctrl, src_dpcm_pred,
                       CSL_CCP2_LCM_CTRL_SRC_DPCM_PRED_SHIFT,
                       CSL_CCP2_LCM_CTRL_SRC_DPCM_PRED_MASK);
        }
        CCP2_SET32(ptr_ctrl, src_format, CSL_CCP2_LCM_CTRL_SRC_FORMAT_SHIFT,
                   CSL_CCP2_LCM_CTRL_SRC_FORMAT_MASK);
        CCP2_SET32(ptr_ctrl, lcm_burstsize, CSL_CCP2_LCM_CTRL_BURST_SIZE_SHIFT,
                   CSL_CCP2_LCM_CTRL_BURST_SIZE_MASK);
        CCP2_SET32(ptr_ctrl, read_throttle,
                   CSL_CCP2_LCM_CTRL_READ_THROTTLE_SHIFT,
                   CSL_CCP2_LCM_CTRL_READ_THROTTLE_MASK);
        CCP2_SET32(ptr_ctrl, dst_port, CSL_CCP2_LCM_CTRL_DST_PORT_SHIFT,
                   CSL_CCP2_LCM_CTRL_DST_PORT_MASK);
        CCP2_SET32(ptr_ctrl, dst_format, CSL_CCP2_LCM_CTRL_DST_FORMAT_SHIFT,
                   CSL_CCP2_LCM_CTRL_DST_FORMAT_MASK);

        ccp2_reg->LCM_CTRL = ptr_ctrl;

    }

    else if (flag == CHANNEL_WRITE)
    {
        return CCP2_INVALID_INPUT;

    }
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/* CCP2_Lcm_Set_Read_Address() configures read address for the memory channel
 * * @param lcm_src_addr source address where CCP to read from * @return
 * status CCP2_SUCCESS Succesfull open \n Other value = Open failed and the
 * error code is returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     CCP2_Lcm_Set_Read_Address                                               
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
CCP2_RETURN CCP2_Lcm_Set_Read_Address(void *lcm_src_addr)
{
    if (((uint32) lcm_src_addr & 0x0000001F) != 0)
        return CCP2_INVALID_INPUT;

    CCP2_WRITE32(ccp2_reg->LCM_SRC_ADDR, lcm_src_addr);
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/* Set_CCP2_Lcx_Ping_Buffer() writes to the ping buffer of a channel * @param 
 * number instance of the Channel_Number * @param address uint32 physical
 * address for ping buffer of channel * @param offset uint32 offset for
 * physical address for ping buffer of channel * @return status CCP2_SUCCESS
 * Succesfull open \n * Other value = Open failed and the error code is
 * returned. */
 /*=======================================================================*/

/* ===================================================================
 *  @func     Set_CCP2_Lcx_Ping_Buffer                                               
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
CCP2_RETURN Set_CCP2_Lcx_Ping_Buffer(Channel_Number number,
                                     PhyAddress_t address, PhyAddress_t offset)
{

    if (number > 3)
        return CCP2_INVALID_INPUT;
    switch (number)
    {
        case 0:
            CCP2_SET32(ccp2_reg->LC0_DAT_PING_ADDR, address,
                       CSL_CCP2_LC0_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC0_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_OFST, offset,
                       CSL_CCP2_LC0_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC0_DAT_OFST_OFST_MASK);
            break;
        case 1:
            CCP2_SET32(ccp2_reg->LC1_DAT_PING_ADDR, address,
                       CSL_CCP2_LC1_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC1_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_OFST, offset,
                       CSL_CCP2_LC1_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC1_DAT_OFST_OFST_MASK);
            break;
        case 2:
            CCP2_SET32(ccp2_reg->LC2_DAT_PING_ADDR, address,
                       CSL_CCP2_LC2_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC2_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_OFST, offset,
                       CSL_CCP2_LC2_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC2_DAT_OFST_OFST_MASK);
            break;
        case 3:
            CCP2_SET32(ccp2_reg->LC3_DAT_PING_ADDR, address,
                       CSL_CCP2_LC3_DAT_PING_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC3_DAT_PING_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_OFST, offset,
                       CSL_CCP2_LC3_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC3_DAT_OFST_OFST_MASK);
            break;

        default:
            return CCP2_FAILURE;
    }
    return CCP2_SUCCESS;

}

/* ======================================================================= */
/* Set_CCP2_Lcx_Pong_Buffer() writes to the pong buffer of a channel * @param 
 * number instance of the Channel_Number * @param address uint32 physical
 * address for pong buffer of channel * @param offset uint32 offset for
 * physical address for pong buffer of channel * @return status CCP2_SUCCESS
 * Succesfull open \n * Other value = Open failed and the error code is
 * returned. */
 /*=======================================================================*/

/* ===================================================================
 *  @func     Set_CCP2_Lcx_Pong_Buffer                                               
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
CCP2_RETURN Set_CCP2_Lcx_Pong_Buffer(Channel_Number number,
                                     PhyAddress_t address, PhyAddress_t offset)
{

    switch (number)
    {
        case 0:
            CCP2_SET32(ccp2_reg->LC0_DAT_PONG_ADDR, address,
                       CSL_CCP2_LC0_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC0_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC0_DAT_PONG_ADDR, offset,
                       CSL_CCP2_LC0_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC0_DAT_OFST_OFST_MASK);
            break;
        case 1:
            CCP2_SET32(ccp2_reg->LC1_DAT_PONG_ADDR, address,
                       CSL_CCP2_LC1_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC1_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC1_DAT_OFST, offset,
                       CSL_CCP2_LC1_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC1_DAT_OFST_OFST_MASK);
            break;
        case 2:
            CCP2_SET32(ccp2_reg->LC2_DAT_PONG_ADDR, address,
                       CSL_CCP2_LC2_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC2_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC2_DAT_OFST, offset,
                       CSL_CCP2_LC2_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC2_DAT_OFST_OFST_MASK);
            break;
        case 3:
            CCP2_SET32(ccp2_reg->LC3_DAT_PONG_ADDR, address,
                       CSL_CCP2_LC3_DAT_PONG_ADDR_ADDR_SHIFT,
                       CSL_CCP2_LC3_DAT_PONG_ADDR_ADDR_MASK);
            CCP2_SET32(ccp2_reg->LC3_DAT_OFST, offset,
                       CSL_CCP2_LC3_DAT_OFST_OFST_SHIFT,
                       CSL_CCP2_LC3_DAT_OFST_OFST_MASK);
            break;

        default:
            return CCP2_FAILURE;
    }
    return CCP2_SUCCESS;
}

/* ======================================================================= */
/* CCP2_Write_Debug() writes to the debug register of CCP2 for debugging *
 * @param value the data to be written to debug register * @return status
 * CCP2_SUCCESS Succesfull open \n * Other value = Open failed and the error
 * code is returned. */
 /*=======================================================================*/

/* ===================================================================
 *  @func     CCP2_Write_Debug                                               
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
CCP2_RETURN CCP2_Write_Debug(uint32 value)
{
    CCP2_SET32(ccp2_reg->DBG, value, CSL_CCP2_DBG_DBG_SHIFT,
               CSL_CCP2_DBG_DBG_MASK);
    return CCP2_SUCCESS;
}

/* 
 * 
 * CCP2_RETURN ccp2_hook_int_handler(ccp2_interrupt_id_t
 * int_id,Interrupt_Handle_T * handle) {
 * 
 * 
 * 
 * 
 * 
 * }
 * 
 * 
 * CCP2_RETURN ccp2_hook_int_handler(ccp2_interrupt_id_t
 * int_id,Interrupt_Handle_T * handle,CCP2_CONTEXT_T context_num) {
 * 
 * 
 * 
 * }
 * 
 * CCP2_RETURN ccp2_enable_interrupt(ccp2_interrupt_id_t int_id)
 * 
 * {
 * 
 * 
 * }
 * 
 * 
 * 
 * CCP2_RETURN ccp2_enable_interrupt(ccp2_interrupt_id_t int_id)
 * 
 * {
 * 
 * 
 * } */

#if 0
Interrupt_Handle_T *cpp2_hwi_cb_list_head[CCP2_CONTEXT_MAX][CCP2_LC0_MAX_IRQ];

Interrupt_Handle_T *cpp2_swi_cb_list_head[CCP2_CONTEXT_MAX][CCP2_LC0_MAX_IRQ];

extern uint16 ccp2_get_int_sts(CCP2_CONTEXT_T context_num);

/* ===================================================================
 *  @func     ccp2_get_int_sts                                               
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
uint16 ccp2_get_int_sts(CCP2_CONTEXT_T context_num)
{

    uint16 sts = 0;

    if (context_num == CCP2_CONTEXT_0)
        sts = (ccp2_reg->LC01_IRQSTATUS) & 0xFFFF;

    if (context_num == CCP2_CONTEXT_1)
        sts = ((ccp2_reg->LC01_IRQSTATUS) >> 16) & 0xFFFF;

    if (context_num == CCP2_CONTEXT_2)
        sts = (ccp2_reg->LC23_IRQSTATUS) & 0xFFFF;

    if (context_num == CCP2_CONTEXT_3)
        sts = ((ccp2_reg->LC23_IRQSTATUS) >> 16) & 0xFFFF;

    if (context_num == CCP2_CONTEXT_M)
        sts = (ccp2_reg->LCM_IRQSTATUS) & 0xFFFF;

    return sts;

}

/* ===================================================================
 *  @func     ccp2_clear_int_sts                                               
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
uint16 ccp2_clear_int_sts(CCP2_CONTEXT_T context_num,
                          ccp2_interrupt_id_t int_id)
{

    uint32 sts = 0;

    if (context_num == CCP2_CONTEXT_0)
        ccp2_reg->LC01_IRQSTATUS = 0x00000001 << int_id;

    if (context_num == CCP2_CONTEXT_1)
        ccp2_reg->LC01_IRQSTATUS = 0x00000001 << (int_id + CCP2_LC0_MAX_IRQ);

    if (context_num == CCP2_CONTEXT_2)
        ccp2_reg->LC23_IRQSTATUS = 0x00000001 << int_id;

    if (context_num == CCP2_CONTEXT_3)
        ccp2_reg->LC23_IRQSTATUS = 0x00000001 << (int_id + CCP2_LC0_MAX_IRQ);

    if (context_num == CCP2_CONTEXT_M)
        ccp2_reg->LCM_IRQSTATUS = int_id;

    return sts;

}

/* ===================================================================
 *  @func     ccp2_hwi_interrupt_handler                                               
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
Void ccp2_hwi_interrupt_handler(ISS_RETURN status, uint32 arg1, void *arg2)
{

    // CCP2_regs_ovly CCP2_regs=NULL;

    uint8 i, j = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    // isp_interrupt_id_t id;
    uint16 sts_reg = 0;

    uint32 base_add = 0;

    CCP2_CONTEXT_T context_num = CCP2_CONTEXT_MAX;

    ccp2_interrupt_id_t int_id;

    uint32 temp_reg;

    /* cheack all the callbacks that need to be called from HWI context */

     /**/ for (j = 0; j < CCP2_CONTEXT_MAX; j++)
    {

        context_num = j;

        sts_reg = ccp2_get_int_sts(context_num);

        for (i = 0; i < CCP2_LC0_MAX_IRQ; i++)
        {
            tmp = NULL;

            int_id = i;
            // id=isp_callback_priority[i];

            if (((sts_reg) & (0x0001 << i)) != 0)
            {

                tmp = cpp2_hwi_cb_list_head[context_num][int_id];
                if (tmp != NULL)
                {

                    while (tmp != NULL)
                    {
                        if (tmp->callback != NULL)
                        {
                            tmp->callback(CCP2_SUCCESS, tmp->arg1, tmp->arg2);

                        }
                        else
                        {
                            // isp_error_count++;
                            /* ERROR , because interrupt ocurred and no
                             * client has registered, but list head is still 
                             * valid */
                        }
                        tmp = tmp->list_next;
                    }

                }
                else
                {
                    // isp_error_count++;
                    /* ERROR , because interrupt ocurred and no client has
                     * registered */
                }

                if (cpp2_swi_cb_list_head[context_num][int_id] == NULL)
                {
                    /* clear the status bit only if there is no corresponding 
                     * HWI handler. */
                    /* the register has one bit for status of HS_VS_IRQ which 
                     * is WR and 15 other status bits are RO , instead of
                     * handleing the special case with a "if" statement we
                     * just write to the read only bits */
                    // isp_regs->ISP5_IRQSTATUS_0=0x00000001<<id;
                    ccp2_clear_int_sts(context_num, int_id);
                }
                else
                {
                    /* a particular interrupt occured for which there is a
                     * valid swi callback */
                    flag = 1;
                }

            }

        }

    }

    if (flag == 1)
    {
        /* Need to activate SWI, so diasable the interrupt at ISS level to
         * prevent preemption of SWI while calling the callbacks */

        // CCP2_disable_interrupt(device_num,CCP2_int_id);
        // Swi_post(CCP2_context_irq_swi_handle);

    }

    return;
}

/* ===================================================================
 *  @func     ccp2_swi_interrupt_handler                                               
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
Void ccp2_swi_interrupt_handler(ISS_RETURN status, uint32 arg1, void *arg2)
{

    // CCP2_regs_ovly CCP2_regs=NULL;

    uint8 i, j = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    // isp_interrupt_id_t id;
    uint16 sts_reg = 0;

    uint32 base_add = 0;

    CCP2_CONTEXT_T context_num = CCP2_CONTEXT_MAX;

    ccp2_interrupt_id_t int_id;

    uint32 temp_reg;

    for (j = 0; j < CCP2_CONTEXT_MAX; j++)
    {

        context_num = j;

        sts_reg = ccp2_get_int_sts(context_num);

        for (i = 0; i < CCP2_LC0_MAX_IRQ; i++)
        {
            tmp = NULL;

            int_id = i;

            // id=isp_callback_priority[i];

            if (((sts_reg) & (0x0001 << i)) != 0)
            {

                tmp = cpp2_swi_cb_list_head[context_num][int_id];
                if (tmp != NULL)
                {

                    while (tmp != NULL)
                    {
                        if (tmp->callback != NULL)
                        {
                            tmp->callback(CCP2_SUCCESS, tmp->arg1, tmp->arg2);

                        }
                        else
                        {
                            // isp_error_count++;
                            /* ERROR , because interrupt ocurred and no
                             * client has registered, but list head is still 
                             * valid */
                        }
                        tmp = tmp->list_next;
                    }

                }
                else
                {
                    // isp_error_count++;
                    /* ERROR , because interrupt ocurred and no client has
                     * registered */
                }

                ccp2_clear_int_sts(context_num, int_id);

            }

        }

    }

    /* Enable ISS level interrupt here */
    iss_enable_interrupt(ISS_INTERRUPT_ISP_IRQ0);

    return;

}

CCP2_complex_io_hook_int_handler(CCP2_CONTEXT_T context,
                                 ccp2_interrupt_id_t int_id,
                                 Interrupt_Handle_T * handle)
{
    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL, *head_ptr =
        (Interrupt_Handle_T *) NULL;
    uint8 flag = 0;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CCP2_FAILURE;
    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        head_ptr = cpp2_hwi_cb_list_head[context][int_id];
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        head_ptr = cpp2_swi_cb_list_head[context][int_id];
    else
        return CCP2_FAILURE;

    if (head_ptr == NULL)
        return CCP2_FAILURE;

    /* 
     * If the handle is the first element update the head ptr */

    if (handle == head_ptr)
    {

        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            cpp2_hwi_cb_list_head[context][int_id] = head_ptr->list_next;

        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            cpp2_swi_cb_list_head[context][int_id] = head_ptr->list_next;

        }

    }
    else
    {
        ptr_curr = head_ptr;
        while (ptr_curr->list_next != NULL)
        {
            if (ptr_curr->list_next == handle)
            {                                              /* found the
                                                            * element to be
                                                            * removed and
                                                            * de-linked from
                                                            * the list */
                flag = 1;
                ptr_curr->list_next = handle->list_next;
            }
        }
        if (flag == 0)
            return CCP2_FAILURE;

    }

    return CCP2_SUCCESS;

}

#endif
