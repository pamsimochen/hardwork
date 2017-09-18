/********************************************************/
/* ************************************************************************** */
/*   lnk.cmd   v1.01                                                          */
/*   Copyright (c) 1996-1997  Texas Instruments Incorporated                  */
/* ************************************************************************** */
/*  -cr                                                                       */
 
                                                                          
                                                                          
                                                                          
-c                                                                       
-heap  0x3000000                                                          
-stack 0x4000       
-l rtsv7M3_T_le_eabi.lib              /* GET RUN-TIME SUPPORT             */
/*-l ../lib/mpeg4venc_ti_host.lib*/

__F_STACK_SIZE = 0x400;                  /*  IRQ Stack                    */
__S_STACK_SIZE = 0x200;                   /* SUPERVISOR Stack              */
__A_STACK_SIZE = 0x100;                   /*  IRQ Stack                    */
__U_STACK_SIZE = 0x100;                   /* SUPERVISOR Stack              */
__Y_STACK_SIZE = 0x100;                   /*  IRQ Stack                    */
__I_STACK_SIZE = 0x200;                   /*  IRQ Stack                    */

/*--------------------------------------------------------------------------*/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/*--------------------------------------------------------------------------*/
                                                                          
MEMORY                                                                   
{                                                                        
    VECS          :     o = 0x00000000     l = 0x00000100
    ITCM          :     o = 0x00000100     l = 0x0002FF00
    EXTMEM        :     o = 0x8C000000     l = 0x04000000
    EXTMEM_1_PCI  :     o = 0xB8000000     l = 0x00000100
    EXTMEM_1      :     o = 0xB8000100     l = 0x04000000
    OCMC          :     o = 0x00300000     l = 0x0003FFFF
}                           

/*--------------------------------------------------------------------------*/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/*--------------------------------------------------------------------------*/

SECTIONS                                                                 
{                                                                        


    .intvecs        >       VECS 
    .intc           >       OCMC   /*Code                               */
    .text           >       OCMC   /*Code                               */
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
    .istack         >       EXTMEM   /* INTERRUPT RELATED                  */
    .ystack         >       EXTMEM   /* INTERRUPT RELATED                  */
    .ustack         >       EXTMEM   /* INTERRUPT RELATED                  */
    .astack         >       EXTMEM   /* INTERRUPT RELATED                  */

    .ch             >       EXTMEM   /*                                        */
    .const          >       EXTMEM /*Gloabal Const data                      */
    .bss            >       EXTMEM /*Global Variables which are un-initiazed */
    .sysmem         >       EXTMEM /*Mem-alloc                               */

    .inputbuffer     >       EXTMEM_1 /*                             */
    .outputbuffer    >       EXTMEM_1 /*                                       */

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

    /*.mycode {
     mpeg4venc_ti_host.lib(.text)
    } > EXTMEM*/
    
   	.rts_lib {
         rtsv7M3_T_le_eabi.lib  (.text)
    } > EXTMEM   

    .InternalMemory    >    ITCM PALIGN(16)
	.RawInput  > EXTMEM
	.pci_stat: {} > EXTMEM_1_PCI
	.pci_buf : {} > EXTMEM_1
	
	.analyticinfo_sect > EXTMEM_1 PALIGN(16)
  

}                                 