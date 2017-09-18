/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpsDctrlDefaultsPriv.h
 *
 * \brief VPS Display Controller header file for default Values
 * This file exposes the HAL APIs of the VPS Display Controller to the other
 * drivers.
 *
 */

#ifndef _VPS_DCTRLDEFAULTSPRIV_H
#define _VPS_DCTRLDEFAULTSPRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef TI_814X_BUILD
#include <ti/psp/vps/drivers/display/src/vps_dctrlDefaultsTI814x.h>
#endif

#ifdef TI_816X_BUILD
#include <ti/psp/vps/drivers/display/src/vps_dctrlDefaultsTI816x.h>
#endif

#ifdef TI_8107_BUILD
#include <ti/psp/vps/drivers/display/src/vps_dctrlDefaultsTI8107.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif

