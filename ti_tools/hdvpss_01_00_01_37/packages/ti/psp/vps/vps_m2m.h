/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_M2M_COMMON M2M (Memory-to-Memory) API
 *
 * @{
 */

/**
 *  \file vps_m2m.h
 *
 *  \brief M2M (Memory-to-Memory) API
 */

#ifndef _VPS_M2M_H
#define _VPS_M2M_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum Vps_M2mMode
 *  \brief Mode of the memory-to-memory driver.  Mode depends on whether
 *  the configuration is required per handle or per channel of the handle.
 *  If the configuration is per handle same configuration will be applied
 *  to all the channels in that handle. If the configuration is per
 *  channel each channel in handle will have different configuration */
typedef enum
{
    VPS_M2M_CONFIG_PER_HANDLE = 0,
    /**< All the channels in the handle will have same configuration */
    VPS_M2M_CONFIG_PER_CHANNEL,
    /**< All the channels in handle will have different configuration */
    VPS_M2M_CONFIG_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Vps_M2mMode;


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

#endif /* #ifndef _VPS_M2M_H */

/*  @}  */
