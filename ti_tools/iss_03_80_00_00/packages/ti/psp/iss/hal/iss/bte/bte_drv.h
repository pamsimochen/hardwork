/** ==================================================================
 *  @file   bte_drv.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/bte/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*****************************************************************************/
/* */
/* Copyright 2008-09 by Texas Instruments Incorporated. All rights */
/* reserved. Property of Texas Instruments Incorporated. Restricted */
/* rights to use, duplicate or disclose this code are granted */
/* through contract.  */
/* "Texas Instruments OMAP(tm) Platform Software" */
/* */
/*****************************************************************************/

/*****************************************************************************/
/* File Name :bte_drv.h */
/* */
/* Description :Program file containing the BTE driver API procedures */
/* */
/* Created : MMS Team (Monica) */
/* */
/* @rev 0.1 */
/*========================================================================
*!
*! Revision History
*! ===================================*/
#ifndef _CSLR_BTE_DRV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_BTE_DRV_H_

#include "inc/bte_utils.h"

/* ================================================================ */
/**
* Description:- struct to hold device specific data
*/
/*================================================================== */
typedef struct {
    uint8 opened;
} bte_dev_data_t;

/* ================================================================ */
/**
 *Description:- enum for selecting the context
 */
 /*================================================================== */
typedef enum {
    BTE_CONTEXT_0,
    BTE_CONTEXT_1,
    BTE_CONTEXT_2,
    BTE_CONTEXT_3,
    BTE_CONTEXT_4,
    BTE_CONTEXT_5,
    BTE_CONTEXT_6,
    BTE_CONTEXT_7,
    BTE_CONTEXT_MAX = 8
} BTE_CONTEXT;

/* ================================================================ */
/**
 *Description:- stucture which specifies the context control configuration
 */
 /*================================================================== */
typedef struct {
    uint16 trigger_threshold;
    uint8 init_sx;
    uint8 init_sy;
    uint8 addr32;
    uint8 autoflush;
    uint8 one_shot;
    uint8 grid;
    uint8 mode;
    uint8 flush;
    uint8 stop;
    uint8 start;
} bte_context_ctrl_t;

/* ================================================================ */
/**
 *Description:- structure to config the context
 */
 /*================================================================== */
typedef struct {
    BTE_CONTEXT context_num;
    bte_context_ctrl_t context_ctrl;
    uint32 frame_buffer_addr;                              // 31:5 of the
                                                           // tiler addresss
    uint16 context_start_addr;
    uint32 context_end_addr;
} bte_config_t;

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
BTE_RETURN bte_init();

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
BTE_RETURN bte_open(uint32 start_address);

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
BTE_RETURN bte_close();

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
BTE_RETURN bte_config_base_address(uint32 addr, BTE_CONTEXT context);

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
BTE_RETURN bte_config_start_address(uint32 addr, BTE_CONTEXT context);

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
BTE_RETURN bte_config_end_address(uint32 addr, BTE_CONTEXT context);

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
                           BTE_CONTEXT context);

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
BTE_RETURN bte_config_context_ctrl(bte_config_t * cfg);

/* ===================================================================
 *  @func     bteStartContext                                               
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
void *bteStartContext(BTE_CONTEXT context, uint32 tilerAddress, uint32 width,
                      uint32 height, uint32 bpp);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
