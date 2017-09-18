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
 *  \file iss_isif.h
 *
 *  \brief ISS ISIF - ISIF API
 */

#ifndef _ISS_ISP_ISIF_H
#define _ISS_ISP_ISIF_H


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

/** Maximum pixel position */
#define ISS_ISP_ISIF_MAX_PIX_POS                (4u)

/**
 * enum Iss_IspIsifCfaPattern
 * \brief Enum for selecting cfa pattern
 */
typedef enum
{
    ISP_ISIF_CGA_PATTERN_MOSAIC = 0,
    /**< CFA Pattern is Mosaic */
    ISP_ISIF_CGA_PATTERN_STRIPE
    /**< CFA Pattern is Stripe */
} Iss_IspIsifCfaPattern;

typedef enum
{
    ISS_ISIF_COLOR_COMP_R_YE = 0,
    /**< Color Component is R or Y */
    ISS_ISIF_COLOR_COMP_GR_CY,
    /**< Color Component is Gr or Cy */
    ISS_ISIF_COLOR_COMP_GB_G,
    /**< Color Component is Gb or G */
    ISS_ISIF_COLOR_COMP_B_MG
    /**< Color Component is B or Mg */
} Iss_IspIsifColorComp;

typedef enum
{
    ISS_ISIF_SYNC_POLARITY_POSITIVE = 0,
    /**< Polarity is Positive */
    ISS_ISIF_SYNC_POLARITY_NEGATIVE
    /**< Polarity is Nagative */
} Iss_IspIsifSyncPol;

typedef enum
{
    ISS_ISIF_BAYER_MSB_POS_BIT15 = 0,
    ISS_ISIF_BAYER_MSB_POS_BIT14,
    ISS_ISIF_BAYER_MSB_POS_BIT13,
    ISS_ISIF_BAYER_MSB_POS_BIT12,
    ISS_ISIF_BAYER_MSB_POS_BIT11,
    ISS_ISIF_BAYER_MSB_POS_BIT10,
    ISS_ISIF_BAYER_MSB_POS_BIT9,
    ISS_ISIF_BAYER_MSB_POS_BIT8,
    ISS_ISIF_BAYER_MSB_POS_BIT7
} Iss_IspIsifBayerMsbPos;

typedef enum
{
    ISS_ISIF_SRC_PACK_16BITS = 0,
    ISS_ISIF_SRC_PACK_12BITS,
    ISS_ISIF_SRC_PACK_8BITS
} Iss_IspIsifSrcPack;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/**
 * struct Iss_IspRszParams
 * \brief Structure for ISP resizer parameters like input source,
 *        input/output dataformat, scalar algorithm to be used etc.
 */
typedef struct
{
    UInt32 hdVdDir;
    /**< Direction of hte HD_VD signal, 0 for input and 1 for output */

    Iss_IspIsifBayerMsbPos msbBitPos;
    /**< MSB Bit Position */

    Iss_IspIsifSyncPol  hdPolarity;
    /**< HD sync Polarity */
    Iss_IspIsifSyncPol  vdPolarity;
    /**< VD sync Polarity */

    UInt32 hdWidth;
    /***< Width of the HD Pulse, required only of HD VD is set to output */
    UInt32 vdWidth;
    /**< Width of the VD Pulse, required only of HD VD is set to output */
    UInt32 width;
    /**< Number of pixels per line, required only of HD VD is set to output */
    UInt32 height;
    /**< Number of lines per frame, required only of HD VD is set to output */

    UInt32 startX;
    /**< Horizontal start position for the output */
    UInt32 outWidth;
    /**< Number of output pixels per line */
    UInt32 outHeight;
    /**< Number of output lines per frame */

    Iss_IspIsifCfaPattern cfaPattern;
    /**< Used to configre CFA pattern, which can either mosaic or stripes */

    UInt32 blankDcOffset;
    /**< DC Offset for Blank Clamp */

    Iss_IspIsifColorComp        colorPattern[FVID2_MAX_FIELDS]
                                    [ISS_ISP_ISIF_MAX_PIX_POS];
    /**< Color pattern */

    UInt32  horzOffset;
    /**< Horizontal Offset */
    UInt32  enableDecrAddr;
    /**< Flag to enable decrement of address */

    Iss_IspIsifSrcPack  srcPack;

    //LSC
} Iss_IspIsifCfg;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ISS_ISP_ISIF_H */

/* @} */

