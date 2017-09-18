/** ==================================================================
 *  @file   deiLink.h                                                  
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
    \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
    \defgroup VPSEXAMPLE_DEI_LINK_API Dei Link API

    Dei Link can be used to take input from a link and after doing DEI output
    the frames to output queue.

    DEI can have upto two outputs
    - Output from DEI scaler
    - Output from VIP scaler

    Each can be individually enabled/disabled during link create.

    Each frame output is put in its corresponding output queue.

    @{
*/

/**
    \file deiLink.h
    \brief Dei Link API
*/

#ifndef _DEI_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DEI_LINK_H_

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>

/** \brief DEI Scaler output ID */
#define DEI_LINK_OUT_QUE_DEI_SC  (0)

/** \brief VIP Scaler output ID */
#define DEI_LINK_OUT_QUE_VIP_SC  (1)

/** \brief Max DEI outputs/output queues */
#define DEI_LINK_MAX_OUT_QUE     (2)

/**
    \brief Dei link create parameters
*/
typedef struct {
    Sys_DeiCfgId deiCfgId;
    /**< System configuration ID. */
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams outQueParams[DEI_LINK_MAX_OUT_QUE];
    /**< Output queue information. */
    Bool enableOut[DEI_LINK_MAX_OUT_QUE];
    /**< enableOut[x] = TRUE, enable the corresponding output
         enableOut[x] = FALSE, disable the corresponding output. */

    Bool tilerEnable;
    /**< Indicates whether tiler is enabled for this link. */
    Bool skipAlternateFrames;
    /**< Indicates whether alternate frames should be skipped. */
    Bool comprEnable;
    /**< Indicates whether compression is enabled. */
    Bool setVipScYuv422Format;
    /**< Indicates whether the VIP scalar YUV422 format is set. */
} DeiLink_CreateParams;

/**
    \brief Dei link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     DeiLink_init                                               
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
Int32 DeiLink_init();

/**
    \brief Dei link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     DeiLink_deInit                                               
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
Int32 DeiLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
