/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  VICP  Signal Processing Library                                         */
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
/*        platform_support.h -- Platform dependent functions                */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file declares the prototypes of platform dependent functions */
/*		  such as interrupt setup, memory allocation, timer functions       */
/*                                                                          */
/*     REV                                                                  */ 
/*                                                                          */
/*        version 0.0.1:  12th March                                        */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2009 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _PLAT_SUPPORT_H
#define _PLAT_SUPPORT_H

#ifdef __cplusplus
    extern "C" {
#endif
 
#include <tistdtypes.h>

#ifdef _BIOS_PROJECT
#define main dummyMain
#endif
/* 
 intSetup() plugs the VICP ISR isr_func to the system 
*/
Int32 intSetup(void(*isr_func)());

/* Enable VICP interrupt */
Int32 intEnable();

/* Disable VICP interrupt */
Int32 intDisable();

/* Initialize memory module */
Int32 memInit();

/* Allocate memory region */
void *memAlloc(Uint32 size);

/* Free memory region */
Int32 memFree(void *ptr, Uint32);

/* Cache write back and invalidate all */
Int32 cacheWbInvAll();

/* Cache write back */
void cacheWb(void * ptr, Uint32 size, Uint16 wait);

/* Initialize timer */
Int32 timerInit();

/* Read timer and returns counter at start of benchmark region */
Uint32 timerReadStart();

/* Read timer and returns counter at end of benchmark region */
Uint32 timerReadEnd();

Uint16 getARMmhz();

Uint16 getVICPmhz();

void testPrintHeader(char *title, Uint16 width, Uint16 height);

void testPrintResults(Int32 errorFlag, Uint32 setupTimerDiff, Uint32 resetTimerDiff, Uint32 execTimerDiff, char *desc, Uint32 size);

void testPrintFooter(char *title);

#ifdef __cplusplus
 }
#endif

#endif /* #define _CPISLIB_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2009 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
