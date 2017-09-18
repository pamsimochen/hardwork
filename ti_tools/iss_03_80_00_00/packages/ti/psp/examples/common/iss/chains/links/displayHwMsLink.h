/** ==================================================================
 *  @file   displayHwMsLink.h                                                  
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
 *  \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
 *  \defgroup VPSEXAMPLE_DISPLAY_HWMS_LINK_API Display Mosaic Link API
 *
 *  Display Mosaic Link can be used to instantiate - mosiac display
 *  over HDMI or HDDAC.
 *
 *  For each of the display a different display link instance needs to be
 *  created using the system API.
 *
 *  The system config ID will determine the position/size of each channel and
 *  the chain application should judicially select the config ID depending
 *  on the number of channels and number of input queues.
 *
 *  The display link can take input for multiple input queues. Each input
 *  queue could give n number of channels needed for the mosaic display.
 *  This is used in case channels from two different links needs to be
 *  displayed on the same output.
 *
 *  When a channel is not available at a VSYNC interval, the link will
 *  repeat the previous frame up to some time and after that displays the
 *  blank frame for that channel.
 *
 *  @{
 */

/**
 *  \file displayHwMsLink.h
 *
 *  \brief Display Mosaic Link API
 */

#ifndef _DISPLAY_HWMS_LINK_H_
#define _DISPLAY_HWMS_LINK_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/vps/vps_displayCtrl.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

/** \brief Maximum number of input queue supported per link instance. */
#define DISP_HWMS_LINK_MAX_IN_QUE       (2u)

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

/**
 *  \brief Display Mosaic link create parameters
 */
    typedef struct {
        Sys_DispCfgId dispCfgId;
    /**< System configuration ID. */
        UInt32 numInQue;
    /**< Number of input queues. */
        System_LinkInQueParams inQueParams[DISP_HWMS_LINK_MAX_IN_QUE];
    /**< Display link input information for each of the queues. */
        UInt32 displayRes;
    /**< Display resolution ID, SYSTEM_RES_xxx */
    } DispHwMsLink_CreateParams;

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

/**
 *  \brief Display Mosaic link register and init
 *
 *  For each display instance (HDMI, HDDAC, SDTV)
 *  - Creates link task
 *  - Registers as a link with the system API
 *
 *  \return FVID2_SOK on success
 */
    Int32 DispHwMsLink_init(void);

/**
 *  \brief Display Mosaic link de-register and de-init
 *
 *  For each display instance (HDMI, HDDAC, SDTV)
 *  - Deletes link task
 *  - De-registers the link with the system API
 *
 *  \return FVID2_SOK on success
 */
    Int32 DispHwMsLink_deInit(void);

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _DISPLAY_HWMS_LINK_H_ 
   *//* @} */
