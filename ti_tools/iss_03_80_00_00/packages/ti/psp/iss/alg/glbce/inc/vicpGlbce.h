/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  VICP Signal Processing Library                                          */
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
/*        vicpGlbce.h -- Signal Processing Library Interface header file     */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces supported   */
/*        by the Library                                                    */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.0.1:  22nd Sep                                          */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _CPISLIBGLBCE_H
#define _CPISLIBGLBCE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <ti/psp/iss/alg/evf/inc/tistdtypes.h>
//#include "vicplib.h"
#include "ti/psp/iss/alg/evf/inc/cpisCore.h"
  
typedef struct {
    Uint8 UL;
    Uint8 UR;
    Uint8 LL;
    Uint8 LR;
} CPIS_GlbceWeightStruct;

/* Glbce function's parameter structure */
typedef struct {
    CPIS_GlbceWeightStruct *weightPtr;
    Uint8 generateWeight; /* 1: generate weight 0: don't generate weight */ 
    Uint8 *toneCurvesPtr;
    Uint16 numHorzCurveTables;
    Uint16 numVertCurveTables;
    Uint16 localBlockWidth;
    Uint16 localBlockHeight;
    Uint8 downsampleRatio;
    Uint8 boxcarDownsampleRatio;
    Uint8 tonesCurvesDownsampleRatio;
    Uint8 qFormat;
    Uint8 sensorType;
    Uint16 externalGamma;
    Uint8 *gammaTable;
    Uint8 *invGammaTable;
    Uint16 gammaTableSize;
    Uint8 numBits;
	Uint32 dirtyBit;
} CPIS_GlbceParms;

typedef struct {
    Uint16 dwn_sample_factor;
    Uint16 boxcarPattern;
    Int16 rgb2rgbMat[9];
    Uint8 numInBits;
    Uint8 numOutBits;
	Uint32 dirtyBit;
} CPIS_BoxcarParms;

/* Glbce function interface */
Int32 CPIS_glbce(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_GlbceParms *params,
    CPIS_ExecType execType
);

/* Set best ROI dimension and processing block width and height
   The function CPIS_setBestRoiAndProcBlockDim just needs the members downsampleRatio, tonesCurvesDownsampleRatio
   externalGamma to be set to function 
*/
Int32 CPIS_setBestRoiAndProcBlockDim(CPIS_BaseParms *base, CPIS_GlbceParms *params);

/* Return in *blockWidth and *blockHeight the optimum dimensions for the given ROI dimensions and number of tone curves */
Int32 CPIS_getGlbceBlockDim(Uint32 roiWidth, Uint32 roiHeight, CPIS_GlbceParms *params, Uint32 *blockWidth, Uint32 *blockHeight);

/* Boxcar function interface */
Int32 CPIS_boxcar(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_BoxcarParms *params,
    CPIS_ExecType execType
);

char * getVersion_GLBCE();

#endif /* #define _CPISLIBCUST_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
