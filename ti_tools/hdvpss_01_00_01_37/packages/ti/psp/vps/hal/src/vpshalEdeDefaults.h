/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshalEdeDefaults.h
 *
 * \brief VPS EDE HAL default configuration file
 * This file contains default configuration i.e. expert values for Edge
 * Enhancer.
 *
 */

#ifndef _VPS_EDEDEFAULTS_H
#define _VPS_EDEDEFAULTS_H

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
 * Values to be programmed for expert registers. These values needs to be
 * programmed according to the recommended configuration.
 * Note: These macros needs to be changed as when the recommended value changes
 * in specification.
 */
/* Todo: Update with recommended values from EDE IP team */
#define EDE_DEFAULT_EXPERT_VAL                                                 \
{                                                                              \
    0x08120010u,                                                               \
    /**< Expert value for CFG_EDE0 */                                          \
    0x0101000Au,                                                               \
    /**< Expert value for CFG_EDE1 */                                          \
    0x082801D8u,                                                               \
    /**< Expert value for CFG_EDE2 */                                          \
    0x28120004u,                                                               \
    /*<* Expert value for CFG_EDE3 */                                          \
    0x100A0004u,                                                               \
    /**< Expert value for CFG_EDE4 */                                          \
    0x00070024u,                                                               \
    /**< Expert value for CFG_EDE5 */                                          \
    0xFAF6FAFEu,                                                               \
    /**< Expert value for CFG_EDE6 */                                          \
    0x0CF60C00u,                                                               \
    /**< Expert value for CFG_EDE7 */                                          \
    0x04300011u,                                                               \
    /**< Expert value for CFG_EDE8 */                                          \
    0x019003E4u,                                                               \
    /**< Expert value for CFG_EDE9 */                                          \
    0x070B0270u,                                                               \
     /*<* Expert value for CFG_EDE10 */                                        \
    0x04040064u,                                                               \
     /*<* Expert value for CFG_EDE11 */                                        \
    0x0B00048Cu,                                                               \
    /**< Expert value for CFG_EDE12 */                                         \
    0x03D00060u,                                                               \
    /**< Expert value for CFG_EDE13 */                                         \
    0x0C100410u,                                                               \
    /**< Expert value for CFG_EDE14 */                                         \
    0x02000264u,                                                               \
    /**< Expert value for CFG_EDE15 */                                         \
    0x000003FFu,                                                               \
    /**< Expert value for CFG_EDE16 */                                         \
    0x000003FFu,                                                               \
    /**< Expert value for CFG_EDE17 */                                         \
    0x00000000u,                                                               \
    /**< Expert value for CFG_EDE18 */                                         \
    0x00640014u,                                                               \
    /**< Expert value for CFG_EDE19 */                                         \
    0x0000003cu,                                                               \
    /**< Expert value for CFG_EDE20 */                                         \
    0x00100106u                                                                \
    /**< Expert value for CFG_EDE21 */                                         \
}


#define EDE_PEAKING_EXPERT_VAL                                                 \
{                                                                              \
    {                                                                          \
        {0xFF, 0xFC, 0xFA, 0xFC, 0x00, 0x07, 0xFA, 0x07},                      \
        29,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        4,                                                                     \
        80,                                                                    \
        0x3D8,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xFB, 0xF8, 0xFB, 0x00, 0x0A, 0xF8, 0x0A},                      \
        38,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        4,                                                                     \
        100,                                                                   \
        0x3D0,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xFA, 0xF6, 0xFA, 0x00, 0x0C, 0xF6, 0x0C},                      \
        48,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        4,                                                                     \
        100,                                                                   \
        0x3D0,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xF9, 0xF4, 0xF9, 0x00, 0x0E, 0xF4, 0x0E},                      \
        58,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        4,                                                                     \
        100,                                                                   \
        0x3D0,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFD, 0xF7, 0xF1, 0xF7, 0x00, 0x12, 0xF1, 0x12},                      \
        72,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        4,                                                                     \
        100,                                                                   \
        0x3D0,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFC, 0xF5, 0xEE, 0xF5, 0x00, 0x16, 0xEE, 0x16},                      \
        86,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        4,                                                                     \
        100,                                                                   \
        0x3D0,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xFA, 0xF6, 0xFA, 0x00, 0x0C, 0xF6, 0x0C},                      \
        48,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        3,                                                                     \
        120,                                                                   \
        0x3CA,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xF9, 0xF4, 0xF9, 0x00, 0x0E, 0xF4, 0x0E},                      \
        58,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        3,                                                                     \
        130,                                                                   \
        0x3C0,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFD, 0xF7, 0xF1, 0xF7, 0x00, 0x12, 0xF1, 0x12},                      \
        72,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        16,                                                                    \
        12,                                                                    \
        3,                                                                     \
        150,                                                                   \
        0x3B5,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xFA, 0xF6, 0xFA, 0x00, 0x0C, 0xF6, 0x0C},                      \
        48,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        0,                                                                     \
        0,                                                                     \
        2,                                                                     \
        200,                                                                   \
        0x338,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xFA, 0xF6, 0xFA, 0x00, 0x0C, 0xF6, 0x0C},                      \
        48,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        0,                                                                     \
        0,                                                                     \
        2,                                                                     \
        300,                                                                   \
        0x2D4,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFE, 0xF9, 0xF4, 0xF9, 0x00, 0x0e, 0xF4, 0x0e},                      \
        58,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        0,                                                                     \
        0,                                                                     \
        2,                                                                     \
        360,                                                                   \
        0x298,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
    {                                                                          \
        {0xFD, 0xF7, 0xF1, 0xF7, 0x00, 0x12, 0xF1, 0x12},                      \
        72,                                                                    \
        17,                                                                    \
        0x3E4,                                                                 \
        400,                                                                   \
        0x270,                                                                 \
        11,                                                                    \
        7,                                                                     \
        4,                                                                     \
        4,                                                                     \
        0,                                                                     \
        0,                                                                     \
        2,                                                                     \
        400,                                                                   \
        0x270,                                                                 \
        0xC,                                                                   \
        0x8,                                                                   \
        4,                                                                     \
        0,                                                                     \
        0xB,                                                                   \
        16,                                                                    \
        4,                                                                     \
        0x264,                                                                 \
        0x200,                                                                 \
        100,                                                                   \
    },                                                                         \
}

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Ede_ExpertConfig
 *  \brief Structure containing the default values for the expert registers.
 *
 */
typedef struct
{
    UInt32      cfgEde0;
    /**< Expert value for CFG_EDE0  register */
    UInt32      cfgEde1;
    /**< Expert value for CFG_EDE1  register */
    UInt32      cfgEde2;
    /**< Expert value for CFG_EDE2  register */
    UInt32      cfgEde3;
    /**< Expert value for CFG_EDE3  register */
    UInt32      cfgEde4;
    /**< Expert value for CFG_EDE4  register */
    UInt32      cfgEde5;
    /**< Expert value for CFG_EDE5  register */
    UInt32      cfgEde6;
    /**< Expert value for CFG_EDE6  register */
    UInt32      cfgEde7;
    /**< Expert value for CFG_EDE7  register */
    UInt32      cfgEde8;
    /**< Expert value for CFG_EDE8  register */
    UInt32      cfgEde9;
    /**< Expert value for CFG_EDE9  register */
    UInt32      cfgEde10;
    /**< Expert value for CFG_EDE10 register */
    UInt32      cfgEde11;
    /**< Expert value for CFG_EDE11 register */
    UInt32      cfgEde12;
    /**< Expert value for CFG_EDE12 register */
    UInt32      cfgEde13;
    /**< Expert value for CFG_EDE13 register */
    UInt32      cfgEde14;
    /**< Expert value for CFG_EDE14 register */
    UInt32      cfgEde15;
    /**< Expert value for CFG_EDE15 register */
    UInt32      cfgEde16;
    /**< Expert value for CFG_EDE16 register */
    UInt32      cfgEde17;
    /**< Expert value for CFG_EDE17 register */
    UInt32      cfgEde18;
    /**< Expert value for CFG_EDE18 register */
    UInt32      cfgEde19;
    /**< Expert value for CFG_EDE19 register */
    UInt32      cfgEde20;
    /**< Expert value for CFG_EDE20 register */
    UInt32      cfgEde21;
    /**< Expert value for CFG_EDE21 register */
} Ede_ExpertConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_EDEDEFAULTS_H */
