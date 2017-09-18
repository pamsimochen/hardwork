/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_displayCfg.h
 *
 *  \brief This file contains the different configurations for the display
 *  driver.
 *
 */

#ifndef _VPSDRV_DISPLAYCFG_H
#define _VPSDRV_DISPLAYCFG_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/vps_config.h>


#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of frames that can be queued or dequeued.
 *  This is used to create the frame pointer array used in frame list
 *  statically.
 */
#define VPSDDRV_MAX_FRAMES              (VPS_CFG_MAX_MULTI_WIN)

/** \brief Maximum number of pending request per driver instance. */
#define VPSDDRV_MAX_QOBJS_PER_INST      (10u)

/**
 *  \brief Maximum number of multiwindow settings that can be set for an
 *  instance. This is used in dynamic mosaic layout change.
 */
#ifdef VPS_CFG_DESC_IN_OCMC
#define VPSDDRV_MAX_MULTIWIN_SETTINGS   (2u)
#else
#define VPSDDRV_MAX_MULTIWIN_SETTINGS   (16u)
#endif


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSDRV_DISPLAYCFG_H */
