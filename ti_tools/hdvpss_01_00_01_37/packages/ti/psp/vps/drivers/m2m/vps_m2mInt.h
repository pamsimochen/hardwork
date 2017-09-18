 /*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_m2mInt.h
 *
 *  \brief VPS Mem-Mem driver common header file.
 *
 */

#ifndef _VPS_M2MINT_H_
#define _VPS_M2MINT_H_

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
 *  struct VpsMdrv_SyncMode
 *  \brief Enum to indicate whether Sync on Channel (SOCH) or
 *  Sync on Client (SOC) to be used between each channel descriptors while
 *  chaining.
 */
typedef enum
{
    VPSMDRV_SYNC_MODE_SOCH = 0u,
    /**< Use Sync on Channel. */
    VPSMDRV_SYNC_MODE_SOC,
    /**< Use Sync on Client. */
    VPSMDRV_SYNC_MODE_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} VpsMdrv_SyncMode;

/**
 *  struct VpsMdrv_QueObjType
 *  \brief To know whether the queue objects contain the frames or
 *  coefficient descriptors. So that it can be returned to appropriate
 *  queue after callback.
 */
typedef enum
{
    VPSMDRV_QOBJ_TYPE_FRAMES = 0u,
    /**< Container contains process list to be processed. */
    VPSMDRV_QOBJ_TYPE_COEFF = 1u,
    /**< Container contains the coefficients. */
    VPSMDRV_QOBJ_TYPE_MAX
    /**< Should be the last value of this enumeration. */
} VpsMdrv_QueObjType;


#ifdef __cplusplus
}
#endif

#endif /* _VPS_M2MINT_H_ */
