/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshalNfDefaults.h
 *
 * \brief VPS NF HAL default configuration file
 * This file contains default configuration i.e. expert values for Noise Filter.
 *
 */

#ifndef _VPS_NFDEFAULTS_H
#define _VPS_NFDEFAULTS_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typnffs                                */
/* ========================================================================== */

/**
 *  \brief Values to be programmed for expert registers. These values needs to
 *  be programmed according to the recommended configuration.
 *  Note: These macros needs to be changed as when the recommended value changes
 *  in specification.
 */
/* Todo: Update with recommended values from NF IP team */
#define NF_EXPERT_VALUES    {0x8, 0x8, 0x10, 0x10, 0x10, 0x10, 0xD, 0x6, 0xD,  \
                                0xA, 0xF, 0xF}

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Nf_ExpertConfig
 *  \brief Structure containing the default values for the expert registers.
 *
 */
typedef struct
{
    UInt8 spatialStrengthYLow;
    UInt8 spatialStrengthYHigh;
    UInt8 spatialStrengthCbLow;
    UInt8 spatialStrengthCbHigh;
    UInt8 spatialStrengthCrLow;
    UInt8 spatialStrengthCrHigh;
    UInt8 temporalStrength;
    UInt8 temporalFilterNoise;
    UInt8 noiseIirCoeff;
    UInt8 maxNoise;
    UInt8 pureBlackThreshold;
    UInt8 pureWhiteThreshold;
} Nf_ExpertParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_NFDEFAULTS_H */
