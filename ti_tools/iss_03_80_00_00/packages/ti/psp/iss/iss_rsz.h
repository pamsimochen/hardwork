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
 *  \file iss_rsz.h
 *
 *  \brief ISS RSZ - Resizer API
 */

#ifndef _ISS_ISP_RSZ_H
#define _ISS_ISP_RSZ_H

/*
    TODO: - How to program Threshold High and Low Value
          - FRACDIV?
          - DMA_RSZ and DMA_RZB
          - Do we need to configure IRQ_RSZA, i think this is used for Circular
            buffer output
          - YUV_PHS.PoS
          - Luma and Chroma Vertical Start Phase
          - Scaling ratio in V_DIFF and H_DIFF
          - H_PHS and H_PHS_ADJ pending
 */

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

#define ISS_ISP_RSZ_MAX_RESIZER          (2)


/**
 *  enum Iss_IspRszOpMode
 *  \brief Enum for Resizer operating modes
 */
typedef enum
{
    ISP_RSZ_OP_MODE_BYPASS = 0,
    /**< Resizer bypass mode */
    ISP_RSZ_OP_MODE_PASSTHRU,
    /**< Resizer Pass Thru mode */
    ISP_RSZ_OP_MODE_RESIZING
    /**< Resizing mode */
} Iss_IspRszOpMode;

/**
 *  enum Iss_IspRszInpSrc
 *  \brief Enum for Input source for the Resizer
 */
typedef enum
{
    ISP_RSZ_INP_SRC_IPIPE = 0,
    /**< Resizer Input source is IPIPE */
    ISP_RSZ_INP_SRC_IPIPEIF,
    /**< Resizer Input source is IPIPE Interface */
    ISP_RSZ_INP_SRC_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} Iss_IspRszInpSrc;


/**
 *  enum Iss_IspRszStorageMode
 *  \brief Enum for Input source for the Resizer
 */
typedef enum
{
    ISP_RSZ_STR_MODE_NORMAL = 0,
    /**< Normal Storage Mode */
    ISP_RSZ_STR_MODE_HFLIP,
    /**< Output is fliped in horizontal direction */
    ISP_RSZ_STR_MODE_VFLIP,
    /**< Output is fliped in vertical direction */
    ISP_RSZ_STR_MODE_HFLIP_VFLIP,
    /**< Output is fliped in horizontal and vertical direction */
} Iss_IspRszStorageMode;


/**
 *  enum Iss_IspRszFiltType
 *  \brief Enum for Selecting Filter type
 */
typedef enum
{
    ISP_RSZ_FILT_4TAP_CUBIC = 0,
    /**< 4 Tap Cubic Convolution algorithm for vertical resizing */
    ISP_RSZ_FILT_2TAP_LINEAR,
    /**< 2 Tap Linear Interpolation algorithm for vertical resizing */
} Iss_IspRszFiltType;


/**
 *  enum Iss_IspRszRescaleMode
 *  \brief Enum for Scaling mode, it can be either normal in which
 *         it supports 1/16x to 16x or downscale mode in which allows
 *         scaling factor from 1/4096x to 1x.
 */
typedef enum
{
    ISP_RSZ_RESCALE_MODE_NORMAL = 0,
    /**< Normal Scaling mode in which up and down scaling is allowed */
    ISP_RSZ_RESCALE_MODE_DOWNSCALE,
    /**< Down scaling mode */
} Iss_IspRszRescaleMode;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    Iss_IspRszFiltType      lumaVsType, chromaVsType;
    /**< Vertical Scaling algorithm to be used for Luma and Chroma */
    Iss_IspRszFiltType      lumaHsType, chromaHsType;
    /**< Horizontal Scaling algorithm to be used for Luma and Chroma */

    UInt32                  lumaVsLpfInts, chromaVsLpfInts;
    /**< Vertical Luma and chroma LPF Intensity */
    UInt32                  lumaHsLpfInts, chromaHsLpfInts;
    /**< Horizontal Luma and chroma LPF Intensity */

    UInt32                  alpha;
    /**< Alpha value used in 32 bit RGBA output */

    /* Phase information, Do we really need in user interface */
    UInt32                  lumaVsInitPhase, chromaVsInitPhase;
    UInt32                  hsInitPhase, hsPhaseAdj;
    /**< Phase information */
} Iss_IspRszAdvParams;

/**
 * struct Iss_IspRszCfg
 * \brief Structure for ISP individual resizer configuration like output size,
 *        format, etc.
 */
typedef struct
{
    Iss_IspRszStorageMode   storageMode;
    /**< Different Storage Modes Please see #Isp_RszStogeMode for
         the valid values */

    Iss_PosConfig           posPrms;
    /**< Resizer operation start position in the frame.
         Resizer operation starts after leaving these many pixels */
    UInt32                  outWidth, outHeight;
    /**< Output Frame Size */

    Iss_IspRszAdvParams    *advCfg;
    /**< Resizer Advanced Configuration, Not supported */
} Iss_IspRszParams;

/**
 * struct Iss_IspRszCfg
 * \brief Structure for ISP resizer parameters like input source,
 *        input/output dataformat, scalar algorithm to be used etc.
 */
typedef struct
{
    Iss_IspRszOpMode            opMode;
    /**< Resizer operating mode */

    Iss_CropConfig              cropPrms;
    /**< Crop Parameters, This crop parameres are used for global
         cropping, cropped image from here goes into the scalar
         module for scaling */

    //Iss_ClipParams              clipPrms;
    /**< Clips the Luma and Chroma value to the range given in this param */

    UInt32                      numOutput;
    /**< Number of outputs from the Resizer, At max, it could be
         ISS_ISP_RSZ_MAX_RESIZER, this also is used to identify valid entries
         in the array outputCfg */

    Iss_IspRszParams            rPrms[ISS_ISP_RSZ_MAX_RESIZER];
    /**< Individual Resizer configuration */
} Iss_IspResizerParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ISS_ISP_RSZ_H */

/* @} */

