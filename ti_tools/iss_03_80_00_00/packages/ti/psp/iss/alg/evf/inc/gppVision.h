/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  CPIS layer prototypes                                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*     NAME                                                                 */
/*        gppVision.h --                                                    */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces supported   */
/*        by the general purpose C vision functions belonging to the        */
/*        CoProcessor InfraStructure (CPIS)'s gpp visson library            */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.1  6 June , 2011                                        */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2011 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _GPPVISION_H
#define _GPPVISION_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <tistdtypes.h>
#include "cpisCore.h"
#include "cpisVision.h"

Int32 GPP_CPIS_updateEWRMeanVar16(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_UpdateEWRMeanVar16Parms *params,
    CPIS_ExecType execType
);

/* Return optimum block dimensions used by the background update algorithm based on roi's width and roi's height) */
Int32 GPP_CPIS_getUpdateEWRMeanVar16blockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_segmentFG(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_SegmentFGParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_fgSegment(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_FgSegmentParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getSegmentFGblockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_initMeanVarMHI(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_InitMeanVarMHIParms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions  */
Int32 GPP_CPIS_getCalcSobelEdgeMapBlockDim(CPIS_Format srcFormat, Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_calcSobelEdgeMap(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_CalcSobelEdgeMapParms *params,
    CPIS_ExecType execType
);

/* 2-D Gradient filtering

The first order 3×3 gradient filter calculates the first derivative in both the horizontal and vertical directions,
Gx and Gy, respectively. So for the image pixel I(x,y), we calculate the gradients as shown here:
Gx = I(x+1,y) - I(x-1,y)
Gy = I(x,y+1) - I(x,y-1)
The gradient magnitude can be calculated in three ways depending on the paramater
params->magnitudeType:
- params->magnitudeType== DO_NOT_COMPUTE : no magnitude values are produced
- params->magnitudeType== SUM_ABS_XY : magnitude values are computed using the formula
Gmag = (|Gx| + |Gy|)
- params->magnitudeType== SUM_SQUARE_XY : magnitude values are computed using the formula
Gmag= Gx^2 + Gy^2
- params->magnitudeType== SQRT_SUM_SQUARE_XY : Not supported currently
*/
Int32 GPP_CPIS_xyGradientsAndMagnitude(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_XyGradientsAndMagnitudeParms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getXyGradientsAndMagnitudeBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

/*nms*/

Int32 GPP_CPIS_getNMSBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_nms(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_NMSParms *params,
    CPIS_ExecType execType
);



Int32 GPP_CPIS_getYCbCr2HueBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);
Int32 GPP_CPIS_getHue2MaskBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);
Int32 GPP_CPIS_YCbCr2Hue(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_YCbCr2HueParms *params,
        CPIS_ExecType execType
);
Int32 GPP_CPIS_Hue2Mask(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_Hue2MaskParms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getDoubleThreshBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_doubleThresholding(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_doubleThresholdingParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getMorphologyBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_morphology(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_morphologyParms *params,
    CPIS_ExecType execType
);



Int32 GPP_CPIS_integralImage(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_IntegralImageParms *params,
    CPIS_ExecType execType
);

/*ComputeCorners */
Int32 GPP_CPIS_computeCorners(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ComputeCornersParms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getComputeCornersBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

/*Compute Block Maxima */
Int32 GPP_CPIS_computeBlockMaxima(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ComputeBlockMaximaParms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getComputeBlockMaximaBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

/*NMS Step1 */
Int32 GPP_CPIS_fastNmsStep1(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_FastNmsStep1Parms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getFastNmsStep1BlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_twoLevelsPyramid(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_TwoLevelsPyramidParms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getTwoLevelsPyramidBlockAndBorderDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight, Uint16 *borderWidth, Uint16 *borderHeight);

Int32 GPP_CPIS_countNumONpixels(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_CountNumONpixelsParms *params,
    CPIS_ExecType execType
);

#ifdef __cplusplus
 }
#endif

#endif /* #define _CPISLIBCUST_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
