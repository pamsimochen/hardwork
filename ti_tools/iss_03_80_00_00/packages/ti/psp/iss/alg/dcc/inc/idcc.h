/*
********************************************************************************
 * DCC API
 * 
 * "DCC API" is software module developed for TI's ISS based SOCs. 
 * This module provides APIs for programming of ISS hardware accelerators 
 * which can be used for Imaging and video applications
 * 
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
 * ALL RIGHTS RESERVED 
********************************************************************************
*/
/**  
********************************************************************************
 * @file  hdvicp20_ti_common.c                          
 *                                       
 * @brief This file contains the implementation of the common functions used in
 *        the IVA-HD/HDVICP2.0 APIs. 
 *
********************************************************************************
*/
#ifndef _I_DCC_
#define _I_DCC_
/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
#include <ti/psp/iss/core/inc/iss_comp.h>
#include <ti/psp/iss/alg/vnf/msp_vnf.h>
#include <imctnf.h>
/*---------------------- data declarations -----------------------------------*/

typedef struct VnfRunTimeParams {
MSP_VNF_CREATE_PARAMS *pVnfQueryParam;
MSP_S16 * shift_factor_for_3dnf_diff ; 
MSP_S16 * scale_factor_for_3dnf_diff ; 
MSP_S16 * vnf_user_alpha_lookup;
MSP_S16 vnf_dcc_params_exist;
}VnfRunTimeParams;

typedef struct MctnfRunTimeParams {
IMCTNF_DynamicParams *pMctnfQueryParam;
uint32 mctnf_dcc_params_exist;
}MctnfRunTimeParams;


typedef struct GlbceRunTimeParams {
uint32 num_gain_limit_points;
MSP_S16 gain_limit_x_points[16];
MSP_S16 gain_limit_y_points[16];
MSP_S16 gce_strength;
MSP_S16 lce_strength;
MSP_S16 gbe_strength;
MSP_S16 lbe_strength;
uint32 glbce_dcc_params_exist;

}GlbceRunTimeParams;

/**
 *******************************************************************************
 *  @struct dcc_parser_input_params_t
 *  @brief  This structure contains input parameters
 *
 *  @param  dcc_buf           : pointer to the buffer where dcc profile 
                                are stored 
 *  @param  dcc_buf_size      : Size of the dcc profile buffer
 *  @param  color_temparature : Color temperature of the scene
 *  @param  exposure_time     : exposure time use gad for the current scene
 *  @param  analog_gain       : analog gain used used in the current scene
 *
 *******************************************************************************
*/
typedef struct {
  int8* dcc_buf;
  uint32 dcc_buf_size;
  uint32 color_temparature;
  uint32 exposure_time;
  uint32 analog_gain;

} dcc_parser_input_params_t;

/**
 *******************************************************************************
 *  @struct dcc_parser_input_params_t
 *  @brief  This structure contains output parameters
 *
 *  @param  iss_drv_config           : Pointer to iss drivers config 
 *  @param  dcc_buf_size      : Size of the dcc profile buffer
 *  @param  color_temparature : Color temperature of the scene
 *  @param  exposure_time     : exposure time use gad for the current scene
 *  @param  analog_gain       : analog gain used used in the current scene
 *
 *******************************************************************************
*/
typedef struct {
  iss_drv_config_t *iss_drv_config;
  VnfRunTimeParams *pVnfParam;
  MctnfRunTimeParams *pMctnfParam;
  GlbceRunTimeParams * pGlbceParams;

} dcc_parser_output_params_t;

#ifdef __cplusplus
extern "C"
{
#endif
/*******************************************************************************
*                         FUNCTION DEFINITIONS
*******************************************************************************/

/** 
********************************************************************************
 * @fn      dcc_update(dcc_parser_input_params_t * input_params,
 *                     iss_drv_config_t *iss_drv_config
 *                    )
 *
 * @brief   This function identfies the dcc profile from input params structure 
 *          and updates the iss driver configuration 
 *
 * @param   input_params
 *          input parameters for the dcc parser 
 *
 * @param   iss_drv_config
 *          pointer to the driver configuration
 *
 *
 * @return  int
 *          sucess/failure
********************************************************************************
*/

/* ===================================================================
 *  @func     dcc_update                                               
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
int dcc_update(dcc_parser_input_params_t * input_params,
               dcc_parser_output_params_t *output_params
               );
#ifdef __cplusplus
}
#endif

#endif 
