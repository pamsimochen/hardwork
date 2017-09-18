/** ==================================================================
 *  @file   grpx.h                                                  
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
    \defgroup VPSEXAMPLE_GRPX_LINK_API GRPX Link API

    This will start the GRPX plane displays and keeps displaying the
    TI logo on top of the video planes.

    @{
*/

/**
    \file grpxLink.h
    \brief GRPX Link API
*/

#ifndef _GRPX_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GRPX_H

/**
    \brief Graphics link register and init

    For each display instance (HDMI, HDDAC, SDTV)
    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     Grpx_init                                               
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
Int32 Grpx_init(void);

/**
    \brief Graphics link de-register and de-init

    For each display instance (HDMI, HDDAC, SDTV)
    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     Grpx_deInit                                               
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
Int32 Grpx_deInit(void);

/**
    \brief Graphics link start
    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     Grpx_start                                               
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
Int32 Grpx_start(UInt32 linkId);

/**
    \brief Graphics link stop
    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     Grpx_stop                                               
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
Int32 Grpx_stop(UInt32 linkId);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _GRPX_LINK_H */

/* @} */
