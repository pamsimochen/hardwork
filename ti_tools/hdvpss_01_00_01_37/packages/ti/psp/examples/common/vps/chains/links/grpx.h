/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

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
#define _GRPX_H

/**
    \brief Graphics link register and init

    For each display instance (HDMI, HDDAC, SDTV)
    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 Grpx_init(void);

/**
    \brief Graphics link de-register and de-init

    For each display instance (HDMI, HDDAC, SDTV)
    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 Grpx_deInit(void);

/**
    \brief Graphics link start
    \return FVID2_SOK on success
*/
Int32 Grpx_start(UInt32 linkId);

/**
    \brief Graphics link stop
    \return FVID2_SOK on success
*/
Int32 Grpx_stop(UInt32 linkId);

#endif  /* _GRPX_LINK_H */

/*@}*/
