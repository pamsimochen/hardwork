/*
********************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
********************************************************************************
*/

/**  
*******************************************************************************
 * @file mpeg4vdec_ti_host_netrasim.cmd                       
 *                                       
 * @brief  Linker command file for the Test application. 
 *
 *         This File contains the Memory map and linker secctions used by the 
 *         test application for DM816x
 *
 * @author:  Ashish Singh <ashish.singh@ti.com>
 *
 * @version 0.1 (Oct 2011)  : Base version 

 ******************************************************************************
*/

-c                                   /* LINK USING C CONVENTIONS         */
-stack  0x4000                       /* SOFTWARE STACK SIZE              */
-heap   0x2000000                    /* HEAP AREA SIZE                   */
-l rtsv7M3_T_le_eabi.lib             /* GET RUN-TIME SUPPORT             */

/*--------------------------------------------------------------------------*/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/*--------------------------------------------------------------------------*/
MEMORY
{
    I_MEM       : o = 0x00000000     l = 0x00000100  /* PROGRAM MEMORY (ROM) */
    SL2MEM      : o = 0x59000000     l = 0x00080000
    INTMEM      : o = 0x00300000     l = 0x00040000
    EXTMEM_PCI  : o = 0x8C000000     l = 0x00000100
    EXTMEM_SYS  : o = 0x8C000100     l = 0x02000000  
    INPUTBUF    : o = 0x8e000100     l = 0x00400000
    EXTMEM      : o = 0x8e400100     l = 0x01C00000
}


/*--------------------------------------------------------------------------*/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/*--------------------------------------------------------------------------*/

SECTIONS
{
    .rts_lib {
         rtsv7M3_T_le_eabi.lib  (.text)
                } > EXTMEM
    .mpeg4vdec_ti_text {
            .\..\..\..\..\lib\mpeg4vdec_ti_host.lib(.text)           
                } > EXTMEM
    .intvecs : {} > I_MEM        /* INTERRUPT VECTORS                 */
    .bss     : {} > EXTMEM       /* GLOBAL & STATIC VARS              */
    .sysmem  : {} > EXTMEM_SYS   /* DYNAMIC MEMORY ALLOCATION AREA    */
    .stack   : {} > EXTMEM       /* SOFTWARE SYSTEM STACK             */
    .data    : {} > EXTMEM       /* Global varibale spece             */
    .text    : {} > EXTMEM       /* CODE                              */
    .cinit   : {} > EXTMEM       /* INITIALIZATION TABLES             */
    .const   : {} > EXTMEM       /* CONSTANT DATA                     */
    .pinit   : {} > EXTMEM       /* C++ CONSTRUCTOR TABLES            */
    .input   : {} > INPUTBUF     /* Input buffer                      */
    .output  : {} > EXTMEM       /* Output Buffer                     */
    .mytemp  : {} > EXTMEM       /* temp buffer in case we use        */
    .pci_stat: {} > EXTMEM_PCI   /* PCI stuffs space                  */
    .fileIOSys : {} > EXTMEM     /* File IO lib space                 */
}

