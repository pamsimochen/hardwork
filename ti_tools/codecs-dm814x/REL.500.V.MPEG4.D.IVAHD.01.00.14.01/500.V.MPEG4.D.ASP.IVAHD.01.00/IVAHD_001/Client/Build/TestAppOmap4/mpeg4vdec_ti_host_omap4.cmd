/****************************************************************************/
/*  lnk.cmd   v1.01                                                         */
/*  Copyright (c) 1996-1997  Texas Instruments Incorporated                 */
/****************************************************************************/
/* -cr */
-c
-heap  0x6000000  
-stack 0x4000
-l rtsv7M3_T_le_eabi.lib              /* GET RUN-TIME SUPPORT             */

__S_STACK_SIZE = 0x600;                 /* SUPERVISOR Stack     */
__F_STACK_SIZE = 0x800;                 /* FIQ Stack            */

MEMORY
{
    I_MEM         : o = 0x00000000     l = 0x00000100  /* PROGRAM MEMORY (ROM) */
    EXTMEM        : o = 0x80000000     l = 0x06F90100
    EXTMEM_CONST  : o = 0x86F90100     l = 0x00070000
}



SECTIONS
{
    .intvecs        >     I_MEM 
    .text    : {}   >     EXTMEM
    .stack          >     EXTMEM
    .far            >     EXTMEM
    .switch         >     EXTMEM
    .tables         >     EXTMEM
    .data           >     EXTMEM
    .bss            >     EXTMEM
    .cinit          >     EXTMEM
    .cio            >     EXTMEM
    .sysmem         >     EXTMEM
    .input         : {} > EXTMEM
    .pci_stat      : {} > EXTMEM

    .fileIOSys	   : {} > EXTMEM
    .const          >     EXTMEM_CONST
    .const:statictable  >       EXTMEM_CONST
    .const:MPEG4VDEC_TI_ICONT >       EXTMEM_CONST
 }
 

