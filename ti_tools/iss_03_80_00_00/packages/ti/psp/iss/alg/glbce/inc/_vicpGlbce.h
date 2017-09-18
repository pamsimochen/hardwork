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
/*        _vicplib.h -- Signal Processing Library Interface header file     */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces required    */
/*        by the internal layer of Library                                  */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.0.5:  5th Jan, 2009                                     */
/*        Added 2-D median filtering                                        */                            
/*                                                                          */
/*        version 0.0.4:  10th Dec                                          */
/*        Added recursive filtering                                         */                            
/*                                                                          */
/*        version 0.0.3:  26th Nov                                          */
/*        Removed constrain mat_width= mat_height for CPIS_matMul() function*/                            
/*                                                                          */
/*        version 0.0.2:  27th Oct                                          */
/*        Adding kernels for release 2.0                                    */
/*                                                                          */
/*        version 0.0.1:  22nd Sep                                          */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


#ifndef __CPIS_LIBCUST_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __CPIS_LIBCUST_H

//#include "vicplib.h"
#include "vicpGlbce.h"
#include "cpisSched.h"
//#include "_vicplib.h"
#include "_cpisCore.h"


Int32 _CPIS_checkGlbceParams(
    CPIS_BaseParms *base,
    void *params);

Int32 _CPIS_resetGlbce(CPIS_IpRun *ipRun);
  
Int32 _CPIS_setGlbceProcessing(
    CPIS_IpRun *ipRun,
    CPIS_BaseParms *base,
    void *p);

Int32 _CPIS_setGlbceDmaIn(
    CPIS_IpRun *ipRun,
    CPIS_BaseParms *base,
    void *p);

Int32 _CPIS_checkBoxcarParams(
    CPIS_BaseParms *base,
    void *params);
  
Int32 _CPIS_setBoxcarProcessing(
    CPIS_IpRun *ipRun,
    CPIS_BaseParms *base,
    void *p);

Int32 _CPIS_setBoxcarDmaOut(
    CPIS_IpRun *ipRun,
    CPIS_BaseParms *base,
    void *p);

Uint16 imxenc_calcCurrTonesCurvePtr(Uint16* pcurrToneCurveXY, Uint16*piY_tmp, Uint16*piX_tmp, Uint16* ptoneCurvesPtr, Uint16 *pTempCoefWord, Int16* cmdptr);

Uint16 imxenc_updateCounters( \
	Uint8 numBlockInHalfBigBlock_x, \
	Uint8 numBlksH, \
    Uint16 numX, \
	Uint8 numBlockInHalfBigBlock_y, \
	Int16 *px, \
	Int16 *pIncForHalfBigBlock_x, \
	Int16 *pHalfBigBlock_x,
	Int16 *pX,
	Int16 *pmaskX,
	Int16 *py, \
	Int16 *pIncForHalfBigBlock_y, \
	Int16 *pHalfBigBlock_y,
	Int16 *pIncFory,
	Int16 *pOne, \
	Int16 *pPermScratch1, \
    Int16 *pTCrowLen, \
    Int16 *pIncForTCofst,
    Int16 *pTcSliceOfst,
    Int16 *pToneCurvePtr,
    Int16 *pWriteToneCurvePtr,
    Uint8 tferTCflag,
	Int16 *cmdptr);

Uint16 imxenc_lookupToneCurves(
	Int16 *currY, Int16 *piUr_tmp, Int16 *piDr_tmp, Int16 *piLr_tmp, Int16 *piRr_tmp, \
	Int16 *newYUL, Int16 *newYUR, Int16 *newYLL, Int16 *newYLR, \
    Int16 *toneCurvesPtr, Int16 *currY_currToneCurveIndexTemp, Int16 *currY_currToneCurveIndex,\
    Uint16 quarterSize, Uint16 tluLen, Int16 *cmdptr);

Uint16 imxenc_lookupToneCurvesCol(
	Int16 *currY, Int16 *piUr_tmp, Int16 *piDr_tmp, Int16 *piLr_tmp, Int16 *piRr_tmp, \
	Int16 *toneCurvesPtr, Int16 *pTcSliceOfst, Int16 *newYUL, Int16 *newYUR, Int16 *newYLL, Int16 *newYLR, \
    Int16 *currY_currToneCurveIndexTemp, Int16 *currY_currToneCurveIndex,\
    Uint16 quarterSize, Uint16 tluLen, Int16 *cmdptr);

Uint16 imxenc_lookupToneCurvesRow(
	Int16 *currY, Int16 *piUr_tmp, Int16 *piDr_tmp, Int16 *piLr_tmp, Int16 *piRr_tmp, \
	Int16 *toneCurvesPtr, Int16 *pTcSliceOfst, Int16 *newYUL, Int16 *newYUR, Int16 *newYLL, Int16 *newYLR, \
    Int16 *currY_currToneCurveIndexTemp, Int16 *currY_currToneCurveIndex,\
    Uint16 quarterSize, Uint16 tluLen, Int16 *cmdptr);

void _CPIS_glbceJamTCinWeightMapRow(CPIS_BaseParms *base, CPIS_GlbceParms *params, Uint16 numHorzBlocks, Uint16 numVertBlocks, Uint16 ds_width, Uint16 ds_height, Uint16 numTCbytesPerBlock);

void _CPIS_glbceJamTCinWeightMapCol(CPIS_BaseParms *base, CPIS_GlbceParms *params, Uint16 numHorzBlocks, Uint16 numVertBlocks, Uint16 ds_width, Uint16 ds_height, Uint16 numTCbytesPerBlock);



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  /* #define __CPIS_LIB_H */
  
/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


