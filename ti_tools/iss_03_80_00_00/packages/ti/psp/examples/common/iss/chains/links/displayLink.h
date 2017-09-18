/** ==================================================================
 *  @file   displayLink.h                                                  
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
    \defgroup VPSEXAMPLE_DISPLAY_LINK_API Display Link API

    Display Link can be used to instantiate non-mosiac display
    over HDMI, HDDAC or SDTV.

    For each of the display a different display link instance needs to be
    created using the system API.

    The display link can take input for a single input queue.
    The single input can contain multiple channels but since this is
    a non-mosiac display only one of the channel can be shown at a time.

    By default CH0 is shown on the display.

    User can use the command DISPLAY_LINK_CMD_SWITCH_CH to switch the
    channel that is displayed on the diplay

    @{
*/

/**
    \file displayLink.h
    \brief Display Link API

*/

#ifndef _DISPLAY_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DISPLAY_LINK_H_

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/vps/vps_displayCtrl.h>

/**
    \brief Link CMD: Switch channel that is being displayed

    \param UInt32 chId  [IN] channel ID to display
*/
#define DISPLAY_LINK_CMD_SWITCH_CH       (0x5000)

/**
    \brief Display link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Display link input information */

    UInt32 displayRes;
    /**< Display resolution ID, SYSTEM_RES_xxx */

} DisplayLink_CreateParams;

/**
    \brief Display link register and init

    For each display instance (HDMI, HDDAC, SDTV)
    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     DisplayLink_init                                               
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
Int32 DisplayLink_init();

/**
    \brief Display link de-register and de-init

    For each display instance (HDMI, HDDAC, SDTV)
    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     DisplayLink_deInit                                               
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
Int32 DisplayLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
