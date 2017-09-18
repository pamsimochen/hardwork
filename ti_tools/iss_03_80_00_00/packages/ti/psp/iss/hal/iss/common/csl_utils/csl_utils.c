/** ==================================================================
 *  @file   csl_utils.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/common/csl_utils/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include "csl_types.h"

/* ===================================================================
 *  @func     REG_SET32_debug                                               
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
void REG_SET32_debug(uint32 * reg_add, uint32 val, uint32 shift, uint32 mask)
{

    if (reg_add == NULL)
        while (1) ;

    ((*reg_add) = (((*reg_add) & (~mask)) | (val << shift)));

}

/* ===================================================================
 *  @func     REG_WRITE32_debug                                               
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
void REG_WRITE32_debug(uint32 * reg_add, uint32 val)
{

    if (reg_add == NULL)
        while (1) ;

    ((*reg_add) = (uint32) (val));

}
