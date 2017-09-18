/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshalDrnDefaults.h
 *
 * \brief VPS De-Ringing HAL header file
 * This file exposes the HAL APIs of the VPS De-Ringing.
 *
 */

#ifndef _VPSHALDRNDEFAULTS_H
#define _VPSHALDRNDEFAULTS_H

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

#define DRN_EXPERT_VAL_DEFAULT_CONFIG   {0x3u, 0x3u, 0x14u, 0x100u, 0x9Cu,\
                                         0x10u, 0x64u, 0x28u, 0x10u, 0x02u, \
                                         0x0u, 0x20u, 0x10u, 0x06u, 0x0A}

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    UInt8 thrNumEdge;
    UInt8 thrNumEdgeStrong;
    UInt8 thrNumFlatpixBlck;
    UInt16 thrStrongEdge;
    UInt16 thrWeakEdge;
    UInt16 thrVeryFlatEdge;
    UInt16 thrFlatEdge;
    UInt8 maxBldFactor;
    UInt8 maxSigmaThr1;
    UInt8 bldCoeffQp;
    UInt8 mpegQ;
    UInt8 bldCoeffBlockEdge;
    UInt8 bldCoeffLocalEdge;
    UInt8 bldScaleWeakEdge;
    UInt8 bldScaleStrongEdge;
} Drn_DefaultConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHALDRNDEFAULTS_H */
