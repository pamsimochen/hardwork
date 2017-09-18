/** ==================================================================
 *  @file   alg_ti_flicker_detect.h                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/alg/aewb/ti2a/fd/inc/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef __FLICKER_DETECTION_TI_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __FLICKER_DETECTION_TI_H__

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned short ushort;
typedef unsigned long uint32;
typedef unsigned long long uint64;
//typedef unsigned int Bool;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
typedef long long int64;

typedef enum
{
    FLICKER_STATE_STAT = 0,
    FLICKER_STATE_CALC
} flicker_state_st;

typedef enum
{
    NO_DETECT = 0,
    NO_INFO,
    DETECT_50Hz,
    DETECT_60Hz
} flicker_ret_st;

//flicker_block_w: # of H windows for H3A (has to be a multiple of 3)
//flicker_block_h: # of V windows for H3A (has to be 32)
//return value: memory size needed
/* ===================================================================
 *  @func     flicker_alloc                                               
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
int32 flicker_alloc(int32 flicker_block_w, int32 flicker_block_h);

//algo: pointer to persistent algorithm memory
//flicker_block_w: # of H windows for H3A (has to be a multiple of 3)
//flicker_block_h: # of V windows for H3A (has to be 32)
//sample_period: sensor row time in us (rolling shutter period)
//return value: 0 (success), 1 (failure)
//requirement: sample_period*row_in_pax <= 4166
/* ===================================================================
 *  @func     flicker_init                                               
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
int32 flicker_init(int32 *algo,
                   int32 flicker_block_w,
                   int32 flicker_block_h,
                   int32 sample_period,
                   int32 row_in_pax,
                   int32 thr);

//algo: pointer to persistent algorithm memory
//h3a: the brightness image from H3A
//p_in_p: # of pixels accumulated in each H3A paxel
//state: algorithm call status
//return value: as described by the definition of flicker_ret_st
/* ===================================================================
 *  @func     flicker_detect                                               
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
flicker_ret_st flicker_detect(int32 *algo,
                              int16 *h3a,
                              int32 p_in_p,
                              flicker_state_st state);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

