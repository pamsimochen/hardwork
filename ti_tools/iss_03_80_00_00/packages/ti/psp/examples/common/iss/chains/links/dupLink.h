/** ==================================================================
 *  @file   dupLink.h                                                  
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
    \defgroup VPSEXAMPLE_DUP_LINK_API Frame Duplicator (DUP) Link API

    @{
*/

/**
    \file dupLink.h
    \brief Frame Duplicator (DUP) Link API
*/

#ifndef _DUP_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DUP_LINK_H_

#include <ti/psp/examples/common/iss/chains/links/system.h>

/** \brief Max output queues to which a given DUP link can connect to */
#define DUP_LINK_MAX_OUT_QUE	(4)

/**
    \brief DUP link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */

    UInt32 numOutQue;
    /**< Number of output queues */

    System_LinkOutQueParams outQueParams[DUP_LINK_MAX_OUT_QUE];
    /**< Input queue information */

    Bool notifyNextLink;
    /**< TRUE: send command to next link notifying that new data is ready in que */

} DupLink_CreateParams;

/**
    \brief DUP link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     DupLink_init                                               
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
Int32 DupLink_init();

/**
    \brief DUP link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     DupLink_deInit                                               
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
Int32 DupLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
