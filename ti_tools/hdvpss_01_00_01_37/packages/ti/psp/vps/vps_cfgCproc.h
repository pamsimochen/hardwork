/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_VPS_COMMON
 * \addtogroup VPS_DRV_FVID2_VPS_COMMON_CSC HD-VPSS - CSC Config API
 *
 * @{
 */

/**
 *  \file vps_cfgCproc.h
 *
 *  \brief HD-VPSS - CPROC Config API
 */

#ifndef _VPS_CFG_CPROC_H
#define _VPS_CFG_CPROC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/** \brief Num of coefficients in each set */
#define VPS_CPROC_NUM_COEFF           (3u)

/**
 *  \brief Enum for specifying per-defined CSC co-effs
 */
typedef enum
{
    VPS_CPROC_CIECAM_MODE_BT601 = 0,
    /**< BT601 CIECAM and Inverse CIECAM coefficient */

    VPS_CPROC_CIECAM_MODE_BT709,
    /**< BT709 CIECAM and Inverse CIECAM coefficient */

    VPS_CPROC_CIECAM_MODE_NONE
    /**< User provided CIECAM and Inverse CIECAM coefficient */
} Vps_CprocCiecamMode;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_CprocCiecamCfg
 *  \brief Configuration parameters for Cproc ciecam.
 */
typedef struct
{
    UInt32              multCoeff[VPS_CPROC_NUM_COEFF][VPS_CPROC_NUM_COEFF];
    /**< Multiplication coefficients in the format A0, B0, C0 in the first row,
         A1, B1, C1 in the second row and A2, B2, C2 in the third row. */

    UInt32             S;
    /**< Cprocs Ciecam parameters - S */

    UInt32             T;
    /**< Cprocs Ciecam parameters - T */

    UInt32             A;
    /**< Cprocs Ciecam parameters - A */

    UInt32             CZ;
    /**< Cprocs Ciecam parameters - CZ*/

    UInt32              GY;
    /**< Cprocs Ciecam parameters - GY */

    UInt32              GUB;
    /**< Cprocs Ciecam parameters - GUB */

    UInt32              GUG;
    /**< Cprocs Ciecam parameters - GUG */

    UInt32              GVG;
    /**< Cprocs Ciecam parameters - GVG */

    UInt32              GVR;
    /**< Cprocs Ciecam parameters - GVR */

} Vps_CprocCiecamCfg;

/**
 *  struct Vps_CprocICiecamCfg
 *  \brief Configuration parameters for Cproc Inverse ciecam.
 */
typedef struct
{
    UInt32              multCoeff[VPS_CPROC_NUM_COEFF][VPS_CPROC_NUM_COEFF];
    /**< Multiplication coefficients in the format A0, B0, C0 in the first row,
         A1, B1, C1 in the second row and A2, B2, C2 in the third row. */

    UInt32              A;
    /**< Cproc Inverse ciecam parameters - A*/

    UInt32              CZ;
    /**< Cproc Inverse ciecam parameters - CZ*/

    UInt32              S;
    /**< Cproc Inverse ciecam parameters - S */

    UInt32              X0;
    /**< Cproc Inverse ciecam parameters - X0*/

} Vps_CprocICiecamCfg;

/**
 *  struct Vps_CprocConfig
 *  \brief Configuration parameters for Cproc.
 */
typedef struct
{
    UInt32             ciecamMode;
    /**< Check #Vps_CprocCiecamMode to get the right value*/

    Vps_CprocCiecamCfg ciecamCfg;
    /**< ciecam configuration*/

    Vps_CprocICiecamCfg iciecamCfg;
    /**< Inverse ciecam configuration*/

} Vps_CprocConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#endif /* #ifndef _VPS_CFG_CPROC_H */

/* @} */
