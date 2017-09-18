/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2013 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_VPS_COMMON HD-VPSS - Common API
 *
 * @{
 */

/**
 *  \file iss_h3a.h
 *
 *  \brief ISS H3A - H3A API
 */

#ifndef _ISS_ISP_H3A_H
#define _ISS_ISP_H3A_H


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/fvid2.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


#define ISS_ISP_H3A_AF_IIR_COEFF_MAX        (11)

#define ISS_ISP_H3A_AF_FIR_COEFF_MAX        (5)


/**
 *  enum Iss_IspH3aAfMode
 *  \brief Enum for selecting AF mode
 */
typedef enum
{
    ISP_H3A_AF_MODE_NORMAL = 0,
    /**< Normal AF Mode */
    ISP_H3A_AF_MODE_ADVANCED
    /**< Advanced AF Mode */
} Iss_IspH3aAfMode;

/**
 *  enum Iss_IspH3aAfRgbPos
 *  \brief Enum for differnet layout for RGB in bayer format
 */
typedef enum
{
    ISP_H3A_AF_RGBPOS_GR_GB = 0,
    /**< RGB Postion 0 */
    ISP_H3A_AF_RGBPOS_RG_GB,
    /**< RGB Postion 1 */
    ISP_H3A_AF_RGBPOS_GR_BG,
    /**< RGB Postion 2 */
    ISP_H3A_AF_RGBPOS_RG_BG,
    /**< RGB Postion 3 */
    ISP_H3A_AF_RGBPOS_GG_RB,
    /**< RGB Postion 4 */
    ISP_H3A_AF_RGBPOS_RB_GG,
    /**< RGB Postion 5 */
} Iss_IspH3aAfRgbPos;


/**
 *  enum Iss_IspH3aAfFvMode
 *  \brief Enum for selecting FV Mode
 */
typedef enum
{
    ISP_H3A_AF_FV_MODE_SUM = 0,
    /**< FV Accumulation mode is sum mode */
    ISP_H3A_AF_FV_MODE_PEAK
    /**< FV Accumulation mode is Peak mode */
} Iss_IspH3aAfFvMode;

/**
 *  enum Iss_IspH3aAfVfMode
 *  \brief Enum for selecting Vf Mode
 */
typedef enum
{
    ISP_H3A_AF_VF_HORZ_ONLY = 0,
    /**< Vertical Focus mode is 4 color horizontal FV Only */
    ISP_H3A_AF_VF_HORZ_VERT
    /**< Vertical Focus mode is 1 color horizontal and 1 color vertical */
} Iss_IspH3aAfVfMode;

/**
 *  enum Iss_IspH3aAewbOutMode
 *  \brief Enum for output mode
 */
typedef enum
{
    ISP_H3A_AEWB_OUT_FMT_SUM_SQR = 0,
    /**< Output format is sum of square */
    ISP_H3A_AEWB_OUT_FMT_MIN_MAX,
    /**< Output format is Min and Max Value */
    ISP_H3A_AEWB_OUT_FMT_SUM_ONLY
    /**< Output format is sum only */
} Iss_IspH3aAewbOutMode;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    UInt32          coeff[ISS_ISP_H3A_AF_IIR_COEFF_MAX];
    UInt32          threshold;
} Iss_IspH3aAfIirFiltCfg;

typedef struct
{
    UInt32          coeff[ISS_ISP_H3A_AF_FIR_COEFF_MAX];
    UInt32          threshold;
} Iss_IspH3aAewbFirFilrCfg;

typedef struct
{
    UInt32              startX;
    UInt32              startY;
    /**< Paxel Start position */

    UInt32              width, height;
    /**< Paxel Size */

    UInt32              horzCount, vertCount;
    /**< horizonntal and vertical paxel count */

    UInt32              horzIncr, vertIncr;
    /**< Line and Column increments in Paxel */

    UInt32              iirFiltStartPos;
    /**< IIR filter start position */
} Iss_IspH3aAfPaxelCfg;

/**
 * struct Iss_IspH3aAfCfg
 * \brief Structure for ISP H3A AF engine parameters.
 */
typedef struct
{
    UInt32                  enable;
    /**< Flag to enable/disalbe AF engine */

    Iss_IspH3aAfMode        mode;
    /**< AF Mode */

    UInt32                  enableALowComp;
    /**< Flag to enable A Low Compression */

    UInt32                  enableMedFilt;
    /**< Flag to enable/disable Medial Filter, to reduce Temperatur Induced Noise */
    UInt32                  midFiltThreshold;
    /**< Modian Filter Threshold */

    Iss_IspH3aAfRgbPos      rgbPos;
    /**< RGB layout in bayer format */

    Iss_IspH3aAfPaxelCfg    paxelCfg;
    /**< Paxel configuration */

    Iss_IspH3aAfFvMode      fvMode;
    /**< Defines type of accumulation for FV to be done */

    Iss_IspH3aAfVfMode      vfMode;
    /**< Vertical Focus Mode */

    Iss_IspH3aAfIirFiltCfg      iirCfg1, iirCfg2;
    /**< IIR parameters */
} Iss_IspH3aAfCfg;

/**
 * struct Iss_IspH3aAewbCfg
 * \brief Structure for ISP H3A AEWB engine parameters.
 */
typedef struct
{
    UInt32                  enable;
    /**< Flag to enable/disalbe AF engine */

    UInt32                  enableALowComp;
    /**< Flag to enable A Low Compression */

    UInt32                  enableMedFilt;
    /**< Flag to enable/disable Medial Filter, to reduce Temperatur Induced Noise */
    UInt32                  midFiltThreshold;
    /**< Modian Filter Threshold */

    Iss_IspH3aAfPaxelCfg    winCfg;
    /**< Paxel configuration */

    UInt32                  blackRowStartX, blackRowStartY;
    /**< Start position of the additional black lines used for calculating DC offset */

    Iss_IspH3aAewbOutMode   outMode;
    /**< Output Format */

    UInt32                  sumShift;
    /**< Sum Shift Value */

    UInt32                  satMax;
    /**< Saturation Limit */

    Iss_IspH3aAewbFirFilrCfg    firCfg1, firCfg2;
} Iss_IspH3aAewbCfg;

/**
 * struct Iss_IspH3aCfg
 * \brief Structure for ISP resizer parameters like input source,
 *        input/output dataformat, scalar algorithm to be used etc.
 */
typedef struct
{
    UInt32                      startX, startY;
    /**< Start position for H3A module */

    Iss_IspH3aAfCfg             afCfg;
    /**< AF Configuration */

    Iss_IspH3aAewbCfg           aewbCfg;
    /**< AE/AWB configuration */
}Iss_IspH3aCfg;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ISS_ISP_H3A_H */

/* @} */

