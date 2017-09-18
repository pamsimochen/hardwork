/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshalDeiDefaults.h
 *
 *  \brief VPS DEI HAL default configuration file
 *  This file contains default configuration i.e. expert values for
 *  Deinterlacer.
 *
 */

#ifndef _VPSHALDEIDEFAULTS_H
#define _VPSHALDEIDEFAULTS_H

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
 *  Values to be programmed for expert registers. These values needs to
 *  be programmed according to the recommended configuration.
 *  Note: These macros needs to be changed as when the recommended value
 *  changes in specification.
 */
/* Todo: Update with recommended values from DEI IP team */
#define VPSHAL_DEI_DEFAULT_EXPERT_VAL                                          \
{                                                                              \
    0x020C0804u,                /* Expert value for DEI REG2 register */       \
    0x0118100Fu,                /* Expert value for DEI REG3 register */       \
    0x08040200u,                /* Expert value for DEI REG4 register */       \
    0x1010100Cu,                /* Expert value for DEI REG5 register */       \
    0x10101010u,                /* Expert value for DEI REG6 register */       \
    0x10101010u,                /* Expert value for DEI REG7 register */       \
    0x080A0000u,                /* Expert value for DEI REG10 register */      \
    0x00002004u                 /* Expert value for DEI REG11 register */      \
}


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_DeiExpertConfig
 *  \brief Structure containing the default values for the expert registers.
 */
typedef struct
{
    UInt32      deiReg2;
    /**< DEI REG2 register */
    UInt32      deiReg3;
    /**< DEI REG3 register */
    UInt32      deiReg4;
    /**< DEI REG4 register */
    UInt32      deiReg5;
    /**< DEI REG5 register */
    UInt32      deiReg6;
    /**< DEI REG6 register */
    UInt32      deiReg7;
    /**< DEI REG7 register */
    UInt32      deiReg10;
    /**< DEI REG10 register */
    UInt32      deiReg11;
    /**< DEI REG11 register */
} VpsHal_DeiExpertConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHALDEIDEFAULTS_H */
