/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

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
#define _DISPLAY_LINK_H_

#include <ti/psp/examples/common/vps/chains/links/system.h>
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
typedef struct
{
    System_LinkInQueParams   inQueParams;
    /**< Display link input information */

    UInt32                   displayRes;
    /**< Display resolution ID, SYSTEM_RES_xxx */
    UInt32                   fieldsMerged;
    /**< In case of interlaced display, application needs to submit both the 
     *   fields for display. Now this fields can be merged or can be 
     *   separate
     */
} DisplayLink_CreateParams;

/**
    \brief Display link register and init

    For each display instance (HDMI, HDDAC, SDTV)
    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 DisplayLink_init();

/**
    \brief Display link de-register and de-init

    For each display instance (HDMI, HDDAC, SDTV)
    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 DisplayLink_deInit();

#endif

/*@}*/
