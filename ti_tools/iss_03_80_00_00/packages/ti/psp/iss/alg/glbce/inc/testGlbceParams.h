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
/*        testparams.h -- Signal Processing Library test wrapper interface  */
/*                        header file                                       */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interface support      */
/*        required by the test wrapper files                                */
/*                                                                          */
/*     REV                                                                  */
/*                                                                          */
/*        version 0.0.5: 12th June 2009                                     */
/*        Added affine and cfa                                              */
/*                                                                          */
/*        version 0.0.4: 5th Jan 2009                                       */
/*        Added median 2D                                                   */
/*                                                                          */
/*        version 0.0.3:  10th Dec                                          */
/*        Added recursive filtering                                         */
/*                                                                          */ 
/*        version 0.0.1:  27th Oct                                          */
/*        Adding kernels for rev 2.0 release                                */
/*                                                                          */
/*        version 0.0.1:  22nd Sep                                          */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _TEST_PARAMS_H

#define _TEST_PARAMS_H

//#include "vicplib.h"
#include "cpisCore.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct {
	Uint16 dwn_sample_factor;
	Uint16 boxcarPattern;
    CPIS_Format dstFormat;
} TestBoxcarParams;

typedef struct {
	Uint16 *dwn_sample_factor;
	Uint16 *boxcarPattern;
	CPIS_Format *dstFormat;    
} TestBoxcarStruct;

void initTestParams();
Int32 nextTestParams(void *test);

#ifdef __cplusplus
 }
#endif

#endif /* define _TEST_PARAMS_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


