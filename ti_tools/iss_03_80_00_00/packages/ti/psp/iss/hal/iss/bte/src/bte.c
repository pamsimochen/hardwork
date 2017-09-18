/** ==================================================================
 *  @file   bte.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/bte/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*****************************************************************************/
/* File Name :bte.c */
/* */
/* Description :Program file containing all the functions for configuration,
 * enable and disable * functionality for CBTE module */
/* */
/* Created : Ducati Team (OMAP4) */
/* */
/*****************************************************************************/
/* */
/* Copyright 2008-09 by Texas Instruments Incorporated. All rights */
/* reserved. Property of Texas Instruments Incorporated. Restricted */
/* rights to use, duplicate or disclose this code are granted */
/* through contract.  */
/* "Texas Instruments OMAP(tm) Platform Software" */
/* */
/*****************************************************************************/

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include <ti/psp/iss/hal/iss/bte/bte.h>

bte_regs_ovly bte_reg;

bte_dev_data_t *bte_devp;

bte_dev_data_t bte_dev;

#define BTE_CONTEXT_OFFSET(reg, context)        \
    ((uint32*)((uint32)(uint32*)&bte_reg->reg + (uint32)((((uint32)(&bte_reg->BTE_CONTEXT_BASE_1)) - ((uint32)&(bte_reg->BTE_CONTEXT_BASE_0)))*context)))

/*================================================================ */
/* 
 *bte_init()  
 *Description:- Init should be called only once before calling any other function
 *  
 *
 *  @param   none
 
 *  @return    BTE_RETURN     
 *================================================================== */

/* ===================================================================
 *  @func     bte_init                                               
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
BTE_RETURN bte_init()
{
    /* This memory can be malloc'ed */
    bte_devp = &bte_dev;

    bte_devp->opened = FALSE;

    return BTE_SUCCESS;
}

/* ================================================================ */
/**
*  bte_open()
*  Description:- This routine will assign the base address of BTE to CSL registers.
*  
*
*  @param   none

*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_open                                               
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
BTE_RETURN bte_open(uint32 start_address)
{
    BTE_RETURN retval = BTE_SUCCESS;

    volatile int i;

    if (bte_devp->opened != FALSE)
    {
        return BTE_FAILURE;
    }

    bte_reg = (bte_regs_ovly) BTE_BASE_ADDRESS;
    // To DO check if bte requires any clock disabling & do it here 
    /* ~ BTE_SET32(bte_reg->BTE_HL_SYSCONFIG, 1,
     * CSL_BTE_BTE_HL_SYSCONFIG_SOFTRESET_SHIFT,
     * CSL_BTE_BTE_HL_SYSCONFIG_SOFTRESET_MASK);
     * 
     * for ( i= 0; i < 32; i++) ; */
    // Set virtual BTE virtual container memory base pointer to 0x6000 0000
    BTE_SET32(bte_reg->BTE_CTRL, start_address, CSL_BTE_BTE_CTRL_BASE_SHIFT,
              CSL_BTE_BTE_CTRL_BASE_MASK);

    // To DO check if bte requires any clock disabling & do it here 

    bte_devp->opened = TRUE;

    return retval;
}

/* ================================================================ */
/**
*  bte_close()
*  Description:- this routine de -initialises the CSL regsiters.
*  
*
*  @param   none

*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_close                                               
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
BTE_RETURN bte_close()
{
    BTE_RETURN retval = BTE_SUCCESS;

    if (bte_devp->opened != TRUE)
    {
        return BTE_FAILURE;
    }

    bte_reg = NULL;
    bte_devp->opened = FALSE;

    /* Release the malloced memory ipipe_dev */
    return retval;
}

/* ================================================================ */
/**
*  bte_config_base_address()
*  Description:- this routine configures the base address of the specified context
*  
*
*  @param   addr 		uint32 addr, is the base  address
*  @param   context 	BTE_CONTEXT context, specifies the context
*
*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_config_base_address                                               
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
BTE_RETURN bte_config_base_address(uint32 addr, BTE_CONTEXT context)
{
    uint32 *base_addr = BTE_CONTEXT_OFFSET(BTE_CONTEXT_BASE_0, context);

    BTE_SET32(*base_addr, (addr >> CSL_BTE_BTE_CONTEXT_BASE_0_ADDR_SHIFT),
              CSL_BTE_BTE_CONTEXT_BASE_0_ADDR_SHIFT,
              CSL_BTE_BTE_CONTEXT_BASE_0_ADDR_MASK);

    return BTE_SUCCESS;
}

/* ================================================================ */
/**
*  bte_config_start_address()
*  Description:- this routine configures the start address of the specified context
*  
*  @param   addr 		uint32 addr, is the start  address
*  @param   context 	BTE_CONTEXT context, specifies the context
*
*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_config_start_address                                               
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
BTE_RETURN bte_config_start_address(uint32 addr, BTE_CONTEXT context)
{
    uint32 *start_addr = BTE_CONTEXT_OFFSET(BTE_CONTEXT_START_0, context);

    BTE_SET32(*start_addr, (addr >> CSL_BTE_BTE_CONTEXT_START_0_X_SHIFT),
              CSL_BTE_BTE_CONTEXT_START_0_X_SHIFT,
              CSL_BTE_BTE_CONTEXT_START_0_X_MASK);

    return BTE_SUCCESS;
}

/* ================================================================ */
/**
*  bte_config_end_address()
*  Description:- this routine configures the end address of the specified context
*
*  @param   addr 		uint32 addr, is end  address
*  @param   context 	BTE_CONTEXT context, specifies the context
*
*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_config_end_address                                               
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
BTE_RETURN bte_config_end_address(uint32 addr, BTE_CONTEXT context)
{
    uint32 *end_addr = BTE_CONTEXT_OFFSET(BTE_CONTEXT_END_0, context);

    BTE_SET32(*end_addr, (addr >> CSL_BTE_BTE_CONTEXT_END_0_X_SHIFT),
              CSL_BTE_BTE_CONTEXT_END_0_X_SHIFT,
              CSL_BTE_BTE_CONTEXT_END_0_X_MASK |
              CSL_BTE_BTE_CONTEXT_END_0_Y_MASK);

    return BTE_SUCCESS;
}

/* ================================================================ */
/**
*  bte_config_ctrl()
*  Description:-this routine configures the specified context.
*  
* @param context_ctrlp		bte_context_ctrl_t * context_ctrlp, is the pointer to the cfg struct
*  @param context 			BTE_CONTEXT context, specifies the context

*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_config_ctrl                                               
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
BTE_RETURN bte_config_ctrl(bte_context_ctrl_t * context_ctrlp,
                           BTE_CONTEXT context)
{
    uint32 ctrl = 0;

    uint32 *ctrl_addr = BTE_CONTEXT_OFFSET(BTE_CONTEXT_CTRL_0, context);

    if (context_ctrlp == NULL)
    {
        return BTE_FAILURE;
    }

    /* Fill the values in a temp register and then transfer at one shot */
    ctrl = *ctrl_addr;

    BTE_SET32(ctrl, context_ctrlp->addr32,
              CSL_BTE_BTE_CONTEXT_CTRL_0_ADDR32_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_ADDR32_MASK);
    BTE_SET32(ctrl, context_ctrlp->autoflush,
              CSL_BTE_BTE_CONTEXT_CTRL_0_AUTOFLUSH_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_AUTOFLUSH_MASK);
    BTE_SET32(ctrl, context_ctrlp->flush,
              CSL_BTE_BTE_CONTEXT_CTRL_0_FLUSH_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_FLUSH_MASK);
    BTE_SET32(ctrl, context_ctrlp->grid, CSL_BTE_BTE_CONTEXT_CTRL_0_GRID_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_GRID_MASK);
    BTE_SET32(ctrl, context_ctrlp->init_sx,
              CSL_BTE_BTE_CONTEXT_CTRL_0_INITSX_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_INITSY_MASK);
    BTE_SET32(ctrl, context_ctrlp->init_sy,
              CSL_BTE_BTE_CONTEXT_CTRL_0_INITSY_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_INITSY_MASK);
    BTE_SET32(ctrl, context_ctrlp->mode, CSL_BTE_BTE_CONTEXT_CTRL_0_MODE_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_MODE_MASK);
    BTE_SET32(ctrl, context_ctrlp->one_shot,
              CSL_BTE_BTE_CONTEXT_CTRL_0_ONESHOT_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_ONESHOT_MASK);

    BTE_SET32(ctrl, context_ctrlp->start,
              CSL_BTE_BTE_CONTEXT_CTRL_0_START_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_START_MASK);

    BTE_SET32(ctrl, context_ctrlp->stop, CSL_BTE_BTE_CONTEXT_CTRL_0_STOP_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_STOP_MASK);
    BTE_SET32(ctrl, context_ctrlp->trigger_threshold,
              CSL_BTE_BTE_CONTEXT_CTRL_0_TRIGGER_SHIFT,
              CSL_BTE_BTE_CONTEXT_CTRL_0_TRIGGER_MASK);

    BTE_WRITE32(*ctrl_addr, ctrl);

    return BTE_SUCCESS;
}

/* ================================================================ */
/**
*  bte_config_context_ctrl()
*  Description:- this is the top level routine that will configure a entire context
*  
*
*  @param   cfg 		bte_config_t* cfg, is the pointer to the config structure

*  @return    BTE_RETURN     
*/
/*================================================================== */
/* ===================================================================
 *  @func     bte_config_context_ctrl                                               
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
BTE_RETURN bte_config_context_ctrl(bte_config_t * cfg)
{
    BTE_CONTEXT context = cfg->context_num;

    BTE_RETURN retval = BTE_FAILURE;

    if (context >= BTE_CONTEXT_MAX)
    {
        return BTE_FAILURE;
    }

    retval = bte_config_ctrl(&cfg->context_ctrl, context);

    if (retval == BTE_SUCCESS)
        retval = bte_config_base_address(cfg->frame_buffer_addr, context);

    if (retval == BTE_SUCCESS)
        retval = bte_config_start_address(cfg->context_start_addr, context);

    if (retval == BTE_SUCCESS)
        retval = bte_config_end_address(cfg->context_end_addr, context);

    return retval;
}
