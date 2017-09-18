/** ==================================================================
 *  @file   nsfLink.h                                                  
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

#ifndef _NSF_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _NSF_LINK_H_

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/vps/vps_m2mNsf.h>

#define NSF_LINK_MAX_OUT_QUE (2)

typedef struct {
    System_LinkInQueParams inQueParams;

    /* Since NSF link can have 2 output queues, incoming channels will be
     * splitted in half automatically if user enables both the queues.
     * Channels {0 to (incomingChannels/2 - 1)} will goto output queue 0 and
     * channels {incomingChannels/2 to incomingChannels} will goto output
     * queue 1. If only 1 output queue is enabled, incoming channels will not 
     * be splitted and sent only to output queue 0. For e.g.: Incoming
     * channels = 16, numOutQue = 1 -> outQueue0 = 16, outQueue1 = 0 Incoming 
     * channels = 16, numOutQue = 2 -> outQueue0 = 8, outQueue1 = 8 Incoming
     * channels = 8, numOutQue = 1 -> outQueue0 = 8, outQueue1 = 0 Incoming
     * channels = 8, numOutQue = 2 -> outQueue0 = 4, outQueue1 = 4 */
    UInt32 numOutQue;
    System_LinkOutQueParams outQueParams[NSF_LINK_MAX_OUT_QUE];

    Bool bypassNsf;                                        /* applied for all 
                                                            * Chs, do only
                                                            * Chroma DS */
    Bool tilerEnable;

    /* send even fields only to another link on another queue */
    Bool enableEvenFieldOutput;
    System_LinkOutQueParams enableEvenFieldOutputQueParams;

} NsfLink_CreateParams;

/* ===================================================================
 *  @func     NsfLink_init                                               
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
Int32 NsfLink_init();

/* ===================================================================
 *  @func     NsfLink_deInit                                               
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
Int32 NsfLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
