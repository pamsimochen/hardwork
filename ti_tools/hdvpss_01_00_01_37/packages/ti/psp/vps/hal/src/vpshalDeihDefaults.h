/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshalDeihDefaults.h
 *
 *  \brief VPS DEIH HAL default configuration file
 *  This file contains default configuration i.e. expert values for high
 *  quality Deinterlacer.
 *
 */

#ifndef _VPSHALDEIHDEFAULTS_H
#define _VPSHALDEIHDEFAULTS_H

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
 *  \brief Values to be programmed for expert registers. These values needs to
 *  be programmed according to the recommended configuration.
 *  Note: These macros needs to be changed as when the recommended value
 *  changes in specification.
 */
/* Todo: Update with recommended values from DEIH IP team */
#define VPSHAL_DEIH_DEFAULT_EXPERT_VAL                                         \
{                                                                              \
    0x0C110142u,                /* Expert value for MDT CFG0 register */       \
    0x820C0804u,                /* Expert value for MDT CFG1 register */       \
    0x20302010u,                /* Expert value for MDT CFG2 register */       \
    0x140C0404u,                /* Expert value for MDT CFG3 register */       \
    0x50030804u,                /* Expert value for MDT CFG4 register */       \
    0x00001824u,                /* Expert value for MDT CFG5 register */       \
    0x20961808u,                /* Expert value for MDT CFG6 register */       \
    0x53100000u,                /* Expert value for MDT CFG7 register */       \
    0x00000020u,                /* Expert value for MDT CFG8 register */       \
    0x20961802u,                /* Expert value for MDT CFG9 register */       \
    0x401E1010u,                /* Expert value for EDI CFG1 register */       \
    0x01000032u,                /* Expert value for EDI CFG2 register */       \
    0x18201219u,                /* Expert value for EDI CFG3 register */       \
    0x08040200u,                /* Expert value for EDI CFG4 register */       \
    0x1010100Cu,                /* Expert value for EDI CFG5 register */       \
    0x10101010u,                /* Expert value for EDI CFG6 register */       \
    0x10101010u,                /* Expert value for EDI CFG7 register */       \
    0x06020602u,                /* Expert value for TNR CFG1 register */       \
    0x00000404u,                /* Expert value for TNR CFG2 register */       \
    0x10101010u,                /* Expert value for TNR CFG3 register */       \
    0x10101010u,                /* Expert value for TNR CFG4 register */       \
    0x10101010u,                /* Expert value for TNR CFG5 register */       \
    0x10101010u,                /* Expert value for TNR CFG6 register */       \
    0x2004080Au,                /* Expert value for FMD CFG1 register */       \
    0x0A050603u,                /* Expert value for SNR CFG1 register */       \
    0x06781430u,                /* Expert value for SNR CFG2 register */       \
    0x00040004u,                /* Expert value for GNM CFG0 register */       \
    0x00020002u,                /* Expert value for GNM CFG1 register */       \
    0x000A000Au,                /* Expert value for GNM CFG2 register */       \
    0x001E001Eu                 /* Expert value for GNM CFG3 register */       \
}


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/*
 *  struct VpsHal_DeihExpertConfig
 *  \brief Structure containing the default values for the expert registers.
 */
typedef struct
{
    UInt32      mdtCfg0;
    /**< MDT CFG0 register */
    UInt32      mdtCfg1;
    /**< MDT CFG1 register */
    UInt32      mdtCfg2;
    /**< MDT CFG2 register */
    UInt32      mdtCfg3;
    /**< MDT CFG3 register */
    UInt32      mdtCfg4;
    /**< MDT CFG4 register */
    UInt32      mdtCfg5;
    /**< MDT CFG5 register */
    UInt32      mdtCfg6;
    /**< MDT CFG6 register */
    UInt32      mdtCfg7;
    /**< MDT CFG7 register */
    UInt32      mdtCfg8;
    /**< MDT CFG8 register */
    UInt32      mdtCfg9;
    /**< MDT CFG9 register */
    UInt32      ediCfg1;
    /**< EDI CFG1 register */
    UInt32      ediCfg2;
    /**< EDI CFG2 register */
    UInt32      ediCfg3;
    /**< EDI CFG3 register */
    UInt32      ediCfg4;
    /**< EDI CFG4 register */
    UInt32      ediCfg5;
    /**< EDI CFG5 register */
    UInt32      ediCfg6;
    /**< EDI CFG6 register */
    UInt32      ediCfg7;
    /**< EDI CFG7 register */
    UInt32      tnrCfg1;
    /**< TNR CFG1 register */
    UInt32      tnrCfg2;
    /**< TNR CFG2 register */
    UInt32      tnrCfg3;
    /**< TNR CFG3 register */
    UInt32      tnrCfg4;
    /**< TNR CFG4 register */
    UInt32      tnrCfg5;
    /**< TNR CFG5 register */
    UInt32      tnrCfg6;
    /**< TNR CFG6 register */
    UInt32      fmdCfg1;
    /**< FMD CFG1 register */
    UInt32      snrCfg1;
    /**< SNR CFG1 register */
    UInt32      snrCfg2;
    /**< SNR CFG2 register */
    UInt32      gnmCfg0;
    /**< GNM CFG0 register */
    UInt32      gnmCfg1;
    /**< GNM CFG1 register */
    UInt32      gnmCfg2;
    /**< GNM CFG2 register */
    UInt32      gnmCfg3;
    /**< GNM CFG3 register */
} VpsHal_DeihExpertConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHALDEIHDEFAULTS_H */
