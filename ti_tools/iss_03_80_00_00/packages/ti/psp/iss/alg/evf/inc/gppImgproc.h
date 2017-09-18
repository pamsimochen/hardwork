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
/*        gppImgproc.h --                                                   */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces supported   */
/*        by the general purpose C img proc functions belonging to the      */
/*        CoProcessor InfraStructure (CPIS)'s gpp imgproc library           */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.1  6 June , 2011                                        */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2011 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _GPP_IMGPROCLIB_H

#define _GPP_IMGPROCLIB_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <tistdtypes.h>
#include "cpisCore.h"
#include "cpisImgproc.h"

Int32 GPP_CPIS_initAffineTransform(CPIS_BaseParms *base,
    CPIS_AffineTransformParms *params, CPIS_AffineTransformOutputROI *outputROI );

Int32 GPP_CPIS_getColorSpcConvBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_colorSpcConv(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_ColorSpcConvParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_arrayOp(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_ArrayOpParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getArrayOpBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_arrayScalarOp(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_ArrayScalarOpParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getArrayScalarOpBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_alphaBlend(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_AlphaBlendParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_alphaBlendRGB(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_AlphaBlendParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getRotationBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_rotation(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_RotationParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getFillMemBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_fillMem(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_FillMemParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_arrayCondWrite(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_ArrayCondWriteParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getYCbCrPackBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_YCbCrPack(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_YCbCrPackParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getYCbCrUnPackBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_YCbCrUnpack(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_YCbCrUnpackParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getMatMulBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_matMul(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_MatMulParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_sum(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_SumParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_sumCFA(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_SumCFAParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getLUTBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_table_lookup(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_LUTParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_blkAverage(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_BlkAverageParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_medianFilterRow(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_MedianFilterRowParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_medianFilterCol(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_MedianFilterColParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getMedian2DBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_median2D(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_Median2DParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getFilterBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_filter(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_FilterParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_RGBPack(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_RGBPackParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_RGBUnpack(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_RGBUnpackParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_recursiveFilter(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_RecursiveFilterParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getSobelBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_sobel(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_SobelParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getPyramidBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_pyramid(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_PyramidParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_affineTransform(
     CPIS_Handle *handle,
     CPIS_BaseParms *base,
     CPIS_AffineTransformParms *params,
     CPIS_ExecType execType
);
Int32 GPP_CPIS_getSadBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_sad(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_SadParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_deInterleave(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_DeInterleaveParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_semiPlanarToPlanar(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_SemiPlanarToPlanarParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_fft(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_FFTParms *params,
    CPIS_ExecType execType
);


Int32 GPP_CPIS_getFFTBlockDim(CPIS_Format srcFormat, CPIS_Format dstFormat, Uint32 roiWidth, Uint32 roiHeight, CPIS_FFTParms *params, Uint32 *blockWidth, Uint32 *blockHeight);

/*
 * CPIS_convertMap()
 *
 * srcMap points to an input array of (x,y) coordinates in float
 * dstMapStruct points to a structure CPIS_RemapMaps
 */
Int32 GPP_CPIS_convertMap(CPIS_RemapMaps *maps);

/* CPIS_remap() function interface */
Int32 GPP_CPIS_remap(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_RemapParms *params,
    CPIS_ExecType execType
);

Int32 GPP_CPIS_getRecFiltHorzBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_recursiveFilterHorz(
        CPIS_Handle *handle,
        CPIS_BaseParms *baseRef,
        CPIS_recursiveFilterHorzParms *params,
        CPIS_ExecType execType
);

Int32 GPP_CPIS_getRecFiltVertBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 GPP_CPIS_recursiveFilterVert(
        CPIS_Handle *handle,
        CPIS_BaseParms *baseRef,
        CPIS_recursiveFilterVertParms *params,
        CPIS_ExecType execType
);

#ifdef __cplusplus
 }
#endif

#endif/* #define _GPP_IMGPROCLIB_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


