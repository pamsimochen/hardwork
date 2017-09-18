/*
*******************************************************************************
 * HDVICP2.0 Based H.264SVC Decoder
 *
 * "HDVICP2.0 Based H.264SVC Decoder" is a software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of decoding a compressed
 *  baseline profile SVC bit-stream into a YUV 4:2:0 Raw video.
 *  Based on "Annex G ISO/IEC 14496-10".
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
*******************************************************************************
*/
/**  
*******************************************************************************
 * @file Host_Omap4.cmd                        
 *                                       
 * @brief  Linker command file for the Test application. 
 *
 *         This File contains the Memory map and linker secctions used by the 
 *         test application for Omap4 
 *
 * @author:  Keshava Prasad <prasadk@ti.com>
 *
 * @version 0.1 (Jan 2008) : Base version 
 ******************************************************************************
*/

-c                                      /* LINK USING C CONVENTIONS         */
-stack  0x4000                          /* SOFTWARE STACK SIZE              */
-heap   0x06000000                      /* HEAP AREA SIZE                   */
-l rtsv7M3_T_le_eabi.lib                /* GET RUN-TIME SUPPORT             */

/*--------------------------------------------------------------------------*/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/*--------------------------------------------------------------------------*/
MEMORY
{
    I_MEM          : o = 0x00000000      l = 0x00000100 /*PROGRAM MEMORY (ROM)*/
    EXTMEM_SHARED  : o = 0x94000000      l = 0x00100000 
    EXTMEM_CODE    : o = 0x94100000      l = 0x00100000
    EXTMEM         : o = 0x94200000      l = 0x07E90000
    EXTMEM_CONST   : o = 0x9C090000      l = 0x70000                      
}
/*--------------------------------------------------------------------------*/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/*--------------------------------------------------------------------------*/

SECTIONS
{
   .intvecs : {} >      I_MEM          /* INTERRUPT VECTORS                 */
   .text    : {} >      EXTMEM_CODE   /* CODE                              */
   .stack   : {} >      EXTMEM        /* SOFTWARE SYSTEM STACK             */
   .far     : {} >      EXTMEM
   .switch  : {} >      EXTMEM
   .tables  : {} >      EXTMEM
   .data    : {} >      EXTMEM
   .bss     : {} >      EXTMEM        /* GLOBAL & STATIC VARS              */
   .cinit   : {} >      EXTMEM        /* INITIALIZATION TABLES             */
   .pinit   : {} >      EXTMEM        /* C++ CONSTRUCTOR TABLES            */
   .const   : {} >      EXTMEM_CONST  /* CONSTANT DATA                     */
   .cio     : {} >      EXTMEM
   .sysmem  : {} >      EXTMEM       /* DYNAMIC MEMORY ALLOCATION AREA    */
   .pci_stat    : {} > EXTMEM
   .input  : {} > EXTMEM
   .fileIOSys	   : {} > EXTMEM_SHARED
  
}

