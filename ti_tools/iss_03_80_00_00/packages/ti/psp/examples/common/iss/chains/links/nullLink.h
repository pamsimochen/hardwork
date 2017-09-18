/** ==================================================================
 *  @file   nullLink.h                                                  
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
    \defgroup VPSEXAMPLE_NULL_LINK_API Null Link API

    Null Link can be used to take input from a link and then without doing
    anything return it back to the same link.

    This useful when a link output cannot be given to any other link for testing
    purpose we just want to run a given link but not really use the output.

    In such cases the output queue of link can be connected to a Null link.

    The null link will operate like any other link from interface point of view.
    But it wont do anything with the frames it gets.
    It will simply return it back to the sending link.

    @{
*/

/**
    \file nullLink.h
    \brief Null Link API
*/

#ifndef _NULL_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _NULL_LINK_H_

#include <ti/psp/examples/common/iss/chains/links/system.h>

/** \brief Max input queues to which a given null link can connect to */
#define NULL_LINK_MAX_IN_QUE        (4)

/**
    \brief Null link create parameters
*/
typedef struct {
    UInt32 numInQue;
    /**< Number of input queues */

    System_LinkInQueParams inQueParams[NULL_LINK_MAX_IN_QUE];
    /**< Input queue information */

} NullLink_CreateParams;

/**
    \brief Null link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     NullLink_init                                               
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
Int32 NullLink_init();

/**
    \brief Null link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     NullLink_deInit                                               
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
Int32 NullLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
