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
/*       platform_supportM3bios.c -- Platform dependent functions for Cortex M3*/
/*                                                                          */
/*     DESCRIPTION                                                          */
/*       This file implements platform dependent functions such as          */
/*       interrupt setup, memory allocation, timer functions using DSP/BIOS */
/*                                                                          */
/*     REV                                                                  */ 
/*                                                                          */
/*        version 0.0.1:  12th May                                          */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2011 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

/* Standard header file */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/hal/unicache/Cache.h>

/* private header file */
#include "platform_support.h"

#define DDR_SIZE (0x10000000) /* 256 MB */

// Taken from gel file
#define OSC_0                    20
#define PLL_BASE_ADDRESS         0x481C5000
#define ISS_PLL_BASE            (PLL_BASE_ADDRESS+0x140)
#define M2NDIV                  0x10
#define MN2DIV                  0x14
#define	M2N_PLL                 (volatile Uint32*)(ISS_PLL_BASE+M2NDIV)
#define	M_PLL                   (volatile Uint32*)(ISS_PLL_BASE+MN2DIV)

// The timer is driven by clock at OSC_0 Mhz
#define TIOCP_CFG   ((volatile Uint32*)0x48042010)
#define TCLR        ((volatile Uint32*)0x48042038)
#define TCRR        ((volatile Uint32*)0x4804203C)

#define CACHE_REG           0x55080000
#define CACHE_CFG           ((volatile Uint32*)(CACHE_REG+0x4))
#define CACHE_MAINT         ((volatile Uint32*)(CACHE_REG+0x10))
#define CACHE_MTSTART       ((volatile Uint32*)(CACHE_REG+0x14))
#define CACHE_MTEND         ((volatile Uint32*)(CACHE_REG+0x18))

#define CACHE_MMU_MAINT     ((volatile Uint32*)(CACHE_REG+0xCA8))
#define CACHE_MMU_MTSTART   ((volatile Uint32*)(CACHE_REG+0xCAC))
#define CACHE_MMU_MTEND     ((volatile Uint32*)(CACHE_REG+0xCB0))
#define CACHE_MMU_MAINTST   ((volatile Uint32*)(CACHE_REG+0xCB4))

static unsigned long long freq;
static Uint32 ratioVicpArm;
static Uint16 armMhz, vicpMhz;

//static U64 freq;

/* 
 intSetup() plugs the VICP ISR isr_func to the event EVENDID_VICP 
 Note that the dm648.tcf file must contain the following lines:
 bios.ECM.ENABLE = 1;
 bios.HWI.instance("HWI_INT8").interruptSelectNumber = 0;
 */
Int32 intSetup(void(*function)()){
    //intcSetup(function);
    return 0;

}

/* Enable VICP interrupt */
Int32 intEnable(){
    //intcEvtEnable(CSL_INTC_EVENTID_ASQINT);
    return 0;
}

/* Disable VICP interrupt */
Int32 intDisable(){
    //intcEvtDisable(CSL_INTC_EVENTID_ASQINT);
    return 0;
}

/* Initialize memory module */
Int32 memInit(){
    return 0;
}

/* Allocate memory region */
void *memAlloc(Uint32 size){
  /* sIMCOP requires all memory that it needs to DMA to be 16 bytes aligned
   * and cache requires 128 bytes alignment
   * */
  void *ptr;
  Error_Block eb;
  Uint8 unBlockAlignment;

  unBlockAlignment = 128; // 128 bytes alignment for cache reason
  ptr = (void *)Memory_alloc((xdc_runtime_IHeap_Handle)NULL, size,
						   unBlockAlignment, &eb);

  if(ptr == NULL)
	  printf("Error during Memory_alloc");

  return(ptr);
}

/* Free memory region */
Int32 memFree(void *ptr, Uint32 size){
    Memory_free(NULL, ptr, size);
    return 0;
}

/* Cache write back and invalidate all */
Int32 cacheWbInvAll(){
   //Cache_invAll();
   Cache_wbInv((xdc_Ptr)0x80000000, DDR_SIZE, Cache_Type_ALL, TRUE);
   return 0;
}

/* Cache write back */
void cacheWb(void * ptr, Uint32 size, Uint16 wait){
    Cache_wb(ptr, size, Cache_Type_ALL, wait);
    return;
}

/* Initialize timer and start it */
Int32 timerInit(){

	Types_FreqHz temp;
    Timestamp_getFreq(&temp);
    freq = temp.hi;
    freq = (freq<<32)|temp.lo;

    freq = freq/1000;

    armMhz=getARMmhz();
    vicpMhz=getVICPmhz();

    ratioVicpArm= (vicpMhz<<3)/armMhz;

    return 0;
}

/* Set counter to 0 and start timer */
Uint32 timerReadStart(){
	Types_Timestamp64 temp;
	long long temp64;
	
    Timestamp_get64(&temp);

    temp64= (long long)8*temp.lo;
    return ( (Uint32)(temp64 / ratioVicpArm));
}

/* Stop timer and returns counter at end of benchmark region, scaled at ARM frequency */
Uint32 timerReadEnd(){

	Types_Timestamp64 temp;
	long long temp64;

    Timestamp_get64(&temp);

    temp64= (long long)8*temp.lo;
    return ( (Uint32)(temp64 / ratioVicpArm));
}

Uint16 getARMmhz(){

    Uint16 M,N,M2;

    M= *M_PLL;
    N= *M2N_PLL  & 0xFFFF;
    M2= (*M2N_PLL >> 16)  & 0xFFFF;
    return (((OSC_0*M)/(N+1))/(2*M2));
}

Uint16 getVICPmhz(){

    Uint16 M,N,M2;

    M= *M_PLL;
    N= *M2N_PLL  & 0xFFFF;
    M2= (*M2N_PLL >> 16)  & 0xFFFF;
    return(((OSC_0*M)/(N+1))/M2);

}

static Uint16 testId;

void testPrintHeader(char *title, Uint16 width, Uint16 height){

    printf("               %s testing starts with frame dimension %dx%d= %d points.\n", title, width, height, width*height);
    printf("               M3 freq: %d Mhz  coproc freq: %d Mhz\n", armMhz, vicpMhz);
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| No | Status | Setup M3 | Reset M3 | Exec M3 | Exec coproc | Description\n");
    printf("|    |        |  cycles  |  cycles  |  cycles |  cyc/point  |            \n");
    printf("------------------------------------------------------------------------------------------------------------------------------\n");

    testId= 0;
}

#define MAX_LINELEN  60
#define MAX_NUMLINES 3

void testPrintResults(Int32 errorFlag, Uint32 setupTimerDiff, Uint32 resetTimerDiff, Uint32 execTimerDiff, char *desc, Uint32 size){

    Uint16 index, maxIndex;
    char *descString;
    char *curStr;
    char *line[MAX_NUMLINES+1];
    char *emptyString="";

    index= 1;

    if (desc== NULL) {
        descString= emptyString;
        line[0]= descString;
        }
    else {
        descString= desc;
/* Code below is to break up a long description string into multiple lines
   so everything fits nicely in the display.
   The algorithm looks for last space before the line exceeeds
   MAX_LINELEN characters to determine end of each line
   */
        curStr= descString;
        line[0]= descString;
        index= 1;
        while ((strlen(curStr) > MAX_LINELEN) && (index < MAX_NUMLINES)){
           curStr+= MAX_LINELEN;
           while (*curStr!= ' ' && curStr != descString)
              curStr--;
           if (curStr != descString)
              line[index++]= curStr;
           else
              line[index++]= descString + strlen(descString);
           curStr++;
        }

    }

    maxIndex= index;
    line[index]= descString + strlen(descString);

    index= 0;

    if (!errorFlag)
        printf("|%3d | PASS", testId++);
    else
        printf("|%3d | FAIL", testId++);

    *(line[index+1])= 0; // we set the end of line to '0'
    printf("   | %8d | %5d    |%8d | %6.2f      | %s\n", setupTimerDiff, resetTimerDiff,  execTimerDiff, (float)(ratioVicpArm*execTimerDiff)/(8*size), line[0]);
    if (index != maxIndex - 1)
       *(line[index+1])= ' '; // we set back the end of line to ' ' if not last line
    index++;
    line[index]++; // advance pointer line[index] by 1 to skip the 0

    while (index != maxIndex) {
     *(line[index+1])= 0;
     printf("|    |        |          |          |         |             | %s\n", line[index]);
     if (index != maxIndex - 1)
       *(line[index+1])= ' ';
     index++;
     line[index]++; // advance pointer line[index] by 1 to skip the 0
    }
}

void testPrintFooter(char *title){
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
    printf("               %s testing ends\n\n", title);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2009 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

