/** ==================================================================
 *  @file   iss_drv.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/core/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*******************************************************************
 *
 *    DESCRIPTION:
 *
 *    AUTHOR:
 *
 *    HISTORY:
 *
 *******************************************************************/
#ifndef _ISS_DRV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_DRV_H_
/** include files **/
#include "iss_drv_common.h"

/** public data **/
#define CCP2_FRACK_CLOCK_MAX          (131071)             // (65535)//131071 
                                                           // //65536 //60620 
                                                           // //65536

#define RSZ_UPSCALE_MAX               20
#define RSZ_DOWNSCALE_MAX             4096

#define RSZ_MAX_LINE_SIZE             5376

#define RSZB_MAX_LINE_SIZE            2336

#define H3A_MAX_LINE_SIZE             3008

#define IPIPEIF_DOWNSCALE_NOM         16

/** public functions **/

/* ===================================================================
 *  @func     issSetRszOutAddress                                               
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
ISP_RETURN issSetRszOutAddress(RESIZER_T resizer, RSZ_OP_ADDRESS_T addr_mode,
                               void *address, uint32 ppln);

/* ===================================================================
 *  @func     issRszPreviewParamsInit                                               
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
ISP_RETURN issRszPreviewParamsInit(rsz_config_t * rsz_cfg,
                                   iss_config_params_t * iss_config_params,
                                   ipipe_size_cfg_t * ipipe_size_cfg);

/* ===================================================================
 *  @func     issIpipeifPreviewParamsInit                                               
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
ISP_RETURN issIpipeifPreviewParamsInit(ipipeif_path_cfg_t * cfg,
                                       iss_config_processing_t * iss_config);

/* ===================================================================
 *  @func     issIpipeifYuvInputParamsInit                                               
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
ISP_RETURN issIpipeifYuvInputParamsInit(ipipeif_path_cfg_t * cfg,
                                        ipipe_size_cfg_t * ipipe_size_cfg);

/* ===================================================================
 *  @func     issIpipeifYuv420InputParamsInit                                               
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
ISP_RETURN issIpipeifYuv420InputParamsInit(ipipeif_path_cfg_t * cfg,
                                           ipipe_size_cfg_t * ipipe_size_cfg,
                                           uint8 YC_sel);

/* ===================================================================
 *  @func     issIpipeifH3aInputParamsInit                                               
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
ISP_RETURN issIpipeifH3aInputParamsInit(ipipeif_h3a_input_cfg_t * h3a_cfg,
                                        uint32 start, uint32 decimation_enable);

/* ===================================================================
 *  @func     issIpipeifDarkFrameParamsInit                                               
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
ISP_RETURN issIpipeifDarkFrameParamsInit(ipipeif_path_cfg_t * cfg,
                                         iss_config_processing_t * iss_config,
                                         in_frame_sizes_t * sizes,
                                         IPIPE_PROC_COMPRESSION f_compres_in);

/* ===================================================================
 *  @func     issIpipePreviewParamsInit                                               
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
ISP_RETURN issIpipePreviewParamsInit(ipipe_cfg_t * cfg,
                                     iss_config_params_t * iss_cfg_params,
                                     in_frame_sizes_t * sizes);

/* ===================================================================
 *  @func     issIpipeCaptureParamsInitNoWrite                                               
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
ISP_RETURN issIpipeCaptureParamsInitNoWrite(ipipe_cfg_t * cfg,
                                            iss_config_processing_t *
                                            iss_config,
                                            in_frame_sizes_t * sizes);

/* ===================================================================
 *  @func     issIpipeConfigProcessing                                               
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
ISP_RETURN issIpipeConfigProcessing(ipipe_cfg_t * cfg,
                                    iss_config_processing_t * iss_config);

/* ===================================================================
 *  @func     issCcp2PreviewParamsInit                                               
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
ISP_RETURN issCcp2PreviewParamsInit(ccp2_path_enable_t * cfg, uint32 clock_div,
                                    CCP2_OUTPUT_SELECT out_sel);

/* ===================================================================
 *  @func     issCcp2PreviewLcmInit                                               
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
ISP_RETURN issCcp2PreviewLcmInit(ccp2_lcm_enable_t * lcm_cfg,
                                 in_frame_sizes_t * sizes, uint32 dst_bpln,
                                 IPIPE_PROC_COMPRESSION f_compres_in,
                                 CCP2_OUTPUT_SELECT out_sel);

/* ===================================================================
 *  @func     IssIsifConfigFilters                                               
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
void IssIsifConfigFilters(isif_cfg_t * cfg,
                          iss_config_processing_t * iss_config);

/* ===================================================================
 *  @func     IssIsifParamsInitPreview                                               
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
ISP_RETURN IssIsifParamsInitPreview(isif_cfg_t * cfg, in_frame_sizes_t * sizes,
                                    iss_config_params_t * iss_params);

/* ===================================================================
 *  @func     IssIsifConfig2dLsc                                               
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
void IssIsifConfig2dLsc(isif_2dlsc_cfg_t ** lsc_cfg,
                        iss_config_processing_t * iss_config);

/* ===================================================================
 *  @func     IssIsifParamsInitCaptureInMemory                                               
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
ISP_RETURN IssIsifParamsInitCaptureInMemory(isif_cfg_t * cfg,
                                            iss_config_processing_t *
                                            iss_config,
                                            in_frame_sizes_t * sizes,
                                            IPIPE_PROC_COMPRESSION
                                            f_compres_out);

/* ===================================================================
 *  @func     h3aParamsInitPreview                                               
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
ISP_RETURN h3aParamsInitPreview(h3a_cfg_t * cfg,
                                iss_config_processing_t * iss_config,
                                void *op_h3a_af_buff, void *op_h3a_aewb_buff);

/* ===================================================================
 *  @func     issSetRszInputPort                                               
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
ISP_RETURN issSetRszInputPort(RSZ_IP_PORT_SEL_T ip_port);

/* ===================================================================
 *  @func     h3aGetState                                               
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
ISP_H3A_STATES_T h3aGetState(void);

/* ===================================================================
 *  @func     updateAewbOpAddr                                               
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
ISP_RETURN updateAewbOpAddr(iss_config_processing_t * iss_config,
                            void *op_h3a_aewb_buff);

/* ===================================================================
 *  @func     IssIsifCopyAndSetLscTable                                               
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
void IssIsifCopyAndSetLscTable(uint32 new_table_ptr, isif_2dlsc_cfg_t * lsc_cfg,
                               iss_config_processing_t * iss_config,
                               ipipe_size_cfg_t * ipipe_size_cfg);

/* ===================================================================
 *  @func     rszASetOutConfig                                               
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
ISP_RETURN rszASetOutConfig(int inWidth, int inHeight, int outWidth,
                            int outHeight);

/* ===================================================================
 *  @func     rszBSetOutConfig                                               
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
ISP_RETURN rszBSetOutConfig(int inWidth, int inHeight, int outWidth,
                            int outHeight);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _ISS_DRV_H_
