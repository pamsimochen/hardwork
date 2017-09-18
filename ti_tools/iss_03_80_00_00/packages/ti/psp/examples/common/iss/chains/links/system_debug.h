/** ==================================================================
 *  @file   system_debug.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    \file system_debug.h
    \brief Links and chain - system debug API

    Modify this file to enable/disable prints from different links.

    When SYSTEM_DEBUG_xxx_RT  is enabled a print is done for every frame.
    In real-time system such prints may slow down the system and hence
    are intended to used only for debug purposes.
*/

#ifndef _SYSTEM_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_DEBUG_H_

// #define SYSTEM_DEBUG_VIP_RES_ALLOC
// #define SYSTEM_DEBUG_TILER_ALLOC
// #define VPS_DEBUG_CHROMA_TEAR

/* ===================================================================
 *  @func     defined                                               
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
#if defined( PLATFORM_SIM) || defined(PLATFORM_EVM_SI)

#ifndef PLATFORM_ZEBU
#define SYSTEM_DEBUG
#define SYSTEM_DEBUG_CAPTURE
#define SYSTEM_DEBUG_NSF
#define SYSTEM_DEBUG_DEI
#define SYSTEM_DEBUG_DISPLAY
#define SYSTEM_DEBUG_DISPLAY_HWMS
#define SYSTEM_DEBUG_SCALAR
#define SYSTEM_DEBUG_NULL
#define SYSTEM_DEBUG_DUP
#define SYSTEM_DEBUG_SWMS
#endif

#endif

// #define SYSTEM_VERBOSE_PRINTS

#ifdef PLATFORM_SIM
#define SYSTEM_DEBUG_RT
#endif

#ifdef SYSTEM_DEBUG_RT
#define SYSTEM_DEBUG_CAPTURE_RT
#define SYSTEM_DEBUG_NSF_RT
#define SYSTEM_DEBUG_DEI_RT
#define SYSTEM_DEBUG_DISPLAY_RT
#define SYSTEM_DEBUG_SCALAR_RT
#define SYSTEM_DEBUG_NULL_RT
#define SYSTEM_DEBUG_SWMS_RT
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
