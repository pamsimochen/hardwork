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
 *  \file vps_cfgCsc.h
 *
 *  \brief HD-VPSS - CSC Config API
 */

#ifndef _VPS_CFG_CSC_H
#define _VPS_CFG_CSC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Num of coefficients in each set */
#define VPS_CSC_NUM_COEFF           (3u)

/**
 *  \brief Enum for specifying per-defined CSC co-effs
 */
typedef enum
{
    VPS_CSC_MODE_SDTV_VIDEO_R2Y = 0,
    /**< Select coefficient for SDTV Video */
    VPS_CSC_MODE_SDTV_VIDEO_Y2R,
    /**< Select coefficient for SDTV Video */
    VPS_CSC_MODE_SDTV_GRAPHICS_R2Y,
    /**< Select coefficient for SDTV Graphics */
    VPS_CSC_MODE_SDTV_GRAPHICS_Y2R,
    /**< Select coefficient for SDTV Graphics */
    VPS_CSC_MODE_HDTV_VIDEO_R2Y,
    /**< Select coefficient for HDTV Video */
    VPS_CSC_MODE_HDTV_VIDEO_Y2R,
    /**< Select coefficient for HDTV Video */
    VPS_CSC_MODE_HDTV_GRAPHICS_R2Y,
    /**< Select coefficient for HDTV Graphics */
    VPS_CSC_MODE_HDTV_GRAPHICS_Y2R,
    /**< Select coefficient for HDTV Graphics */
    VPS_CSC_MODE_MAX,
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
    VPS_CSC_MODE_NONE = 0xFFFFu
    /**< Used when coefficients are provided */
} Vps_CscMode;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_CscCoeff
 * \brief Set the Coefficients for Color Space Conversion.
 */
typedef struct
{
    UInt32              mulCoeff[VPS_CSC_NUM_COEFF][VPS_CSC_NUM_COEFF];
    /**< Multiplication coefficients in the format A0, B0, C0 in the first row,
         A1, B1, C1 in the second row and A2, B2, C2 in the third row. */
    UInt32              addCoeff[VPS_CSC_NUM_COEFF];
    /**< Addition Coefficients. */
} Vps_CscCoeff;

/**
 *  struct Vps_CscConfig
 *  \brief Configuration parameters for CSC.
 */
typedef struct
{
    UInt32              bypass;
    /**< Flag to indicate whether CSC to be bypassed or not. */
    UInt32              mode;
    /**< Used to select one of pre-calculated coefficient sets. Used only
         if coeff is null. For valid values see #Vps_CscMode. */
    Vps_CscCoeff       *coeff;
    /**< Set of user provided coefficients. Null if pre-calculated
         coefficients is to be used. */
} Vps_CscConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#endif

/* @} */
