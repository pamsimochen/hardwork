/** ==================================================================
 *  @file   chains_swMs.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _CHAINS_SW_MS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CHAINS_SW_MS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/psp/examples/common/iss/chains/links/swMsLink.h>

#define CHAINS_SW_MS_MAX_DISPLAYS    (3)

/* ===================================================================
 *  @func     Chains_swMsInit                                               
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
Int32 Chains_swMsInit();

/* ===================================================================
 *  @func     Chains_swMsExit                                               
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
Int32 Chains_swMsExit();

/* ===================================================================
 *  @func     Chains_swMsSwitchLayout                                               
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
Int32 Chains_swMsSwitchLayout(UInt32 swMsLinkId[CHAINS_SW_MS_MAX_DISPLAYS],
                              SwMsLink_CreateParams
                              swMsPrm[CHAINS_SW_MS_MAX_DISPLAYS],
                              Bool switchLayout, Bool switchCh,
                              UInt32 numDisplay);

/* ===================================================================
 *  @func     Chains_isSwMsAutoSwitchLayout                                               
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
Bool Chains_isSwMsAutoSwitchLayout(Bool autoSwitchEnable);

/* ===================================================================
 *  @func     Chains_isSwMsAutoSwitchCh                                               
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
Bool Chains_isSwMsAutoSwitchCh(Bool autoSwitchEnable);

/* ===================================================================
 *  @func     Chains_swMsGetLayoutPrm                                               
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
Void Chains_swMsGetLayoutPrm(UInt32 displayNum,
                             Bool switchLayout,
                             Bool switchCh, SwMsLink_LayoutParams * pLayoutPrm);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
