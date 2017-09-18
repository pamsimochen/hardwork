/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 * \file vps_init.h
 *
 * \brief VPS init  file
 * This file exposes init api's which initializes and de-initializes Hals,
 * Cores and other modules
 * drivers.
 *
 */

#ifndef _VPS_INIT_H
#define _VPS_INIT_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vps_init
 *  \brief Function to initialize all VPS modules.
 *
 *  \param arg        Currently not used. For the future.
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Vps_init(Ptr arg);

/**
 *  Vps_deInit
 *  \brief Function to de-initialize all VPS modules.
 *
 *  \param arg        Currently not used. For the future.
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Vps_deInit(Ptr arg);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPS_INIT_H */

