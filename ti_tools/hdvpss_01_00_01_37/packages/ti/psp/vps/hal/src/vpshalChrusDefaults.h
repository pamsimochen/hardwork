/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshalChrusDefaults.h
 *
 * \brief VPS CHR_US HAL default configuration file
 * This file contains default configuration i.e. expert values for
 * Chroma UpSampler.
 *
 */

#ifndef _VPSHALCHRUS_H
#define _VPSHALCHRUS_H

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
 *  \brief Expert value for the anchor and interpolated pixel
 */
#define VPSHAL_CHRUS_DEFAULT_EXPERT_VAL                                        \
{                                                                              \
    /* Coefficients for progressive input */                                   \
    {{{0x0C8, 0x348, 0x18, 0x3FD8}, {0x3FB8, 0x378, 0xE8, 0x3FE8},             \
     {0x0C8, 0x348, 0x18, 0x3FD8}, {0x3FB8, 0x378, 0xE8, 0x3FE8}}},            \
    /* Coefficients for Top Field Interlaced input */                          \
    {{{0x51, 0x3D5, 0x3FE3, 0x3FF7}, {0x3FB5, 0x2E9, 0x18F, 0x3FD3},           \
    /* Coefficients for Bottom Field Interlaced input */                       \
     {0x16B, 0x247, 0xB1, 0x3F9D}, {0x3FCF, 0x3DB, 0x5D, 0x3FF9}}}             \
}


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None*/

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHALCHRUS_H */
