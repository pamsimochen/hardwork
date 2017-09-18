/** ==================================================================
 *  @file   scalarSwMsLink.h                                                  
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
    \defgroup VPSEXAMPLE_SCALAR_LINK_API Scalar Link API

    Scalar Link can be used to instantiate scalar driver which
    can take multi-ch input and output into a single buffer to make a
    mosiac output (software mosiacing)

    This output would be typically given to a display link.

    The scalar link can take input from a single input queue.
    The single input can contain multiple channels. Depending
    on the current layout only some channels will be scaled.

    Example, if there are 8 channels in the input queue and
    output layout mode is 4CH mode, then by default channel0-3
    will be scaled to make 4CH output buffer.

    User can switch the channels by using the
    SCALAR_SW_MS_LINK_CMD_SWITCH_LAYOUT API.

    As of now this command can be used to only switch channels.
    Layout switching is not supported.

    @{
*/

#ifndef _SCALAR_SW_MS_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SCALAR_SW_MS_LINK_H_

/**
    \file scalarSwMsLink.h
    \brief Scalar Link API
*/

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>

/**
    \brief Link CMD: Switch channel that is being scaled by scaler

    \param ScalarSwMsLink_LayoutParams * [IN] Command parameters
*/
#define SCALAR_SW_MS_LINK_CMD_SWITCH_LAYOUT        (0x4000)

/**
    \brief Command parameters for SCALAR_SW_MS_LINK_CMD_SWITCH_LAYOUT
*/
typedef struct {

    UInt32 outLayoutMode;
    /**< Output layout ID: SYSTEM_LAYOUT_MODE_xxx */

    UInt32 startWinChId;
    /**< Top left-window CH ID */

} ScalarSwMsLink_LayoutParams;

/**
    \brief Link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< input queue information */

    System_LinkOutQueParams outQueParams;
    /**< output queue information */

    UInt32 outRes;
    /**< output resolution SYSTEM_RES_xxx */

    UInt32 outLayoutMode;
    /**< output layout ID SYSTEM_LAYOUT_MODE_xxx */

    UInt32 timerPeriod;
    /**< Rate at which output frames should be generated,
         should typically be equal to the display rate

          Example, for 60fps display, timerPeriod should be 1000/60 = 16
    */

} ScalarSwMsLink_CreateParams;

/**
    \brief Scalar link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     ScalarSwMsLink_init                                               
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
Int32 ScalarSwMsLink_init();

/**
    \brief Scalar link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     ScalarSwMsLink_deInit                                               
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
Int32 ScalarSwMsLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
