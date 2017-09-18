/** ==================================================================
 *  @file   grpxLink.h                                                  
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
    \defgroup VPSEXAMPLE_GRAPHICS_LINK_API Graphics Link API

    Graphics Link can be used to instantiate single-region graphics
    over HDMI, HDDAC or SDTV.

    For each of the graphics a different graphics link instance needs to be
    created using the system API.

    The graphcs link can take input for a single input queue.
    The single input can contain multiple channels but since this is
    a single-region display only one of the channel can be shown at a time.

    By default CH0 is shown on the display.

    User can use the command GRPX_LINK_CMD_SWITCH_CH to switch the
    channel that is displayed on the diplay

    @{
*/

/**
    \file grpxLink.h
    \brief Grpahics link API

*/

#ifndef _GRPX_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GRPX_LINK_H_

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/vps_displayCtrl.h>

/**
    \brief Link CMD: Switch channel that is being displayed

    \param UInt32 chId  [IN] channel ID to display
*/
#define GRPX_LINK_CMD_SWITCH_CH       (0x5000)

/**
    \brief Graphics link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Graphics link input information */

    UInt32 displayRes;
    /**< Graphics resolution ID, SYSTEM_RES_xxx */

} GrpxLink_CreateParams;

/**
    \brief Graphics link register and init

    For each display instance (HDMI, HDDAC, SDTV)
    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     GrpxLink_init                                               
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
Int32 GrpxLink_init();

/**
    \brief Graphics link de-register and de-init

    For each display instance (HDMI, HDDAC, SDTV)
    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     GrpxLink_deInit                                               
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
Int32 GrpxLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
