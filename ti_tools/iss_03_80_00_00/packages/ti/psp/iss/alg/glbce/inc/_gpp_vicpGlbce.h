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
/*        _vicpGlbce.h -- Signal Processing Library Interface header file    */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces required    */
/*        by the internal layer of Library                                  */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.0.1:  22nd Sep                                          */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


#ifndef __GPP_CPIS_LIBGLBCE_H
#define __GPP_CPIS_LIBGLBCE_H

#ifdef __cplusplus
	extern "C" {
#endif

//#include "gpp_vicplib.h"
#include "_cpisCore.h"
#include "_gppCore.h"
#include "gpp_vicpGlbce.h"

Int32 _GPP_CPIS_checkGlbceParams(
    CPIS_BaseParms *base,
    void *params);
  
Int32 _GPP_CPIS_setGlbceProcessing(
    CPIS_IpRun *ipRun,
    CPIS_BaseParms *base,
    void *p,
    GPP_CPIS_Func *func);

Int32 _GPP_CPIS_setGlbceDmaIn(
 CPIS_IpRun *ipRun,
 CPIS_BaseParms *base,
 void *p);

void _GPP_CPIS_glbceJamTCinWeightMapRow(CPIS_BaseParms *base, CPIS_GlbceParms *params, Uint16 numHorzBlocks, Uint16 numVertBlocks, Uint16 ds_width, Uint16 ds_height, Uint16 numTCbytesPerBlock);

void _GPP_CPIS_glbceJamTCinWeightMapCol(CPIS_BaseParms *base, CPIS_GlbceParms *params, Uint16 numHorzBlocks, Uint16 numVertBlocks, Uint16 ds_width, Uint16 ds_height, Uint16 numTCbytesPerBlock);

#ifdef __cplusplus
 }
#endif


#endif  /* #define __CPIS_LIB_H */
  
/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


