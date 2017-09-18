/*==========================================================================*/
/*   LNK.CMD   - v2.50a COMMAND FILE FOR LINKING TMS470 32BIS C/C++ PROGRAMS*/
/*                                                                          */
/*   Usage:  lnk470 <obj files...>    -o <out file> -m <map file> lnk.cmd   */
/*           cl470 <src files...>  -z -o <out file> -m <map file> lnk.cmd   */
/*                                                                          */
/*   Description: < Empty >                                                 */
/*                                                                          */
/*   Notes: (1)   You must specify the directory in which run-time support  */
/*                library is located.  Either add a "-i<directory>" line to */
/*                this file, or use the system environment variable C_DIR   */
/*                to specify a search path for libraries.                   */
/*                                                                          */
/*          (2)   If the run-time support library you are using is not      */
/*                named below, be sure to use the correct name here.        */
/*                                                                          */
/*==========================================================================*/

-c                                      /* LINK USING C CONVENTIONS         */
-stack  0x4000                          /* SOFTWARE STACK SIZE              */
-heap   0x4000000                        /* HEAP AREA SIZE                   */
__S_STACK_SIZE = 0x200;                  /* SUPERVISOR Stack                 */
__F_STACK_SIZE = 0x400;                  /* FIQ Stack                        */
__I_STACK_SIZE = 0x200;                  /* IRQ Stack                        */
__U_STACK_SIZE = 0x100;                  /* UNDEF Stack                      */
__Y_STACK_SIZE = 0x100;                  /* SYSYEM Stack                     */
__A_STACK_SIZE = 0x100;                  /* ABORT Stack                      */

/*-l rtsv7M3_T_le_tiarm9.lib        */  /* GET RUN-TIME SUPPORT             */
-l rtsv7M3_T_le_eabi.lib
/*--------------------------------------------------------------------------*/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/*--------------------------------------------------------------------------*/
MEMORY
{                                                                        
    VECS        :                   o = 0x00000000     l = 0x00000100
    ITCM        :                   o = 0x00000100     l = 0x0002FF00
    DTCM        :                   o = 0x00400000     l = 0x00800000
    EXTMEM_SHARED :                 o = 0x94000000     l = 0x00100000 
    EXTMEM_IO          :            o = 0x94100000     l = 0x04000000
    EXTMEM             :            o = 0x98100000     l = 0x07600000
}                           
                                                                          
/*--------------------------------------------------------------------------*/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/*--------------------------------------------------------------------------*/
SECTIONS                                                                 
{                                                                        


    .intvecs        >       VECS 
    .intc           >       EXTMEM   /*Code                               */
    .text           >       EXTMEM   /*Code                               */
    .stack          >       EXTMEM   /*Data                               */
    .far            >       EXTMEM   /*Data--                             */
    .switch         >       EXTMEM   /*                                   */
    .tables         >       EXTMEM   /*Data--tables not standard          */
    .data           >       EXTMEM   /*Data                               */
    .cinit          >       EXTMEM   /*Global Variables which are initialized  */
    .cio            >       EXTMEM   /*Standard IO                             */
    .scratch        >       EXTMEM   /*Non-Standard                            */
    .handler        >       EXTMEM   /*??NS                                    */
    .sstack         >       EXTMEM   /*Data--S                                 */
    .fstack         >       EXTMEM   /*                                        */
    .istack  		> 		EXTMEM              /* INTERRUPT RELATED                  */
    .ystack   		> 		EXTMEM              /* INTERRUPT RELATED                  */
    .ustack   		> 		EXTMEM              /* INTERRUPT RELATED                  */
    .astack   		> 		EXTMEM              /* INTERRUPT RELATED                  */

    .ch             >       EXTMEM   /*                                        */
    .const          >       EXTMEM /*Gloabal Const data                      */
    .bss            >       EXTMEM /*Global Variables which are un-initiazed */
    .sysmem         >       EXTMEM /*Mem-alloc                               */

    .inputbuffer     >       EXTMEM_IO /*                                    */
	.outputbuffer    >       EXTMEM_IO /*                                    */

	.const:MPEG4ENC_TI_StaticTableSection > EXTMEM PALIGN(16)

	.MPEG4VENC_TI_ICONT1_DataSect0 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT1_DataSect1 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT1_DataSect2 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT1_DataSect3 > EXTMEM PALIGN(16)

	.MPEG4VENC_TI_ICONT2_DataSect0 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT2_DataSect1 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT2_DataSect2 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT2_DataSect3 > EXTMEM PALIGN(16)

	.MPEG4VENC_TI_ICONT1_DEBUGTRACE_LEVEL1_DataSect0 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT1_DEBUGTRACE_LEVEL1_DataSect1 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT1_DEBUGTRACE_LEVEL1_DataSect2 > EXTMEM PALIGN(16)

	.MPEG4VENC_TI_ICONT2_DEBUGTRACE_LEVEL1_DataSect0 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT2_DEBUGTRACE_LEVEL1_DataSect1 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT2_DEBUGTRACE_LEVEL1_DataSect2 > EXTMEM PALIGN(16)
	.MPEG4VENC_TI_ICONT2_DEBUGTRACE_LEVEL1_DataSect3 > EXTMEM PALIGN(16)

    .ConcScheduleGVar   >   EXTMEM   /* Contains all the ConcSchedule related   */
                                   /*   Global Variables                        */
    .ConstEXTMEM          >   EXTMEM   /*Constant Data in EXTMEM                    */


    

	.RawInput  > EXTMEM
    .dmm_param > EXTMEM
    .analyticinfo_sect > EXTMEM PALIGN(16)
    .InternalMemory > EXTMEM PALIGN(16)
   .fileIOSys         >    EXTMEM_SHARED
}                                 