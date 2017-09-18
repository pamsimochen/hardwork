;/** ==========================================================================
; *   @file   h264vdec_ti_intvecs.asm
; *
; *   @path   $(PROJDIR)\src\asm\arm968
; *
; *   @brief  This File contains vector table for ARM968. 
; * ===========================================================================
; * Copyright (c) Texas Instruments Inc 2006, 2007
; *
; * Use of this software is controlled by the terms and conditions found
; * in the license agreement under which this software has been supplied
; * =========================================================================*/
;
;/* -------------------- compilation control switches -----------------------*/
;
;
;/****************************************************************************
;*   INCLUDE FILES
;*****************************************************************************/
;
;/* -------------------- system and platform files ------------------------- */
;
;/* ------------------------- program files -------------------------------- */
;
;/****************************************************************************
;*   EXTERNAL REFERENCES NOTE : only use if not found in header file
;*****************************************************************************/
;
;/* ------------------------ data declarations ----------------------------- */
;
;/* ----------------------- function prototypes ---------------------------- */
;
;
;/****************************************************************************
;*   PUBLIC DECLARATIONS Defined here, used elsewhere
;*****************************************************************************/
;
;/* ----------------------- data declarations ------------------------------ */
;
;/* ---------------------- function prototypes ----------------------------- */
;
;
;/****************************************************************************
;*   PRIVATE DECLARATIONS Defined here, used only here
;*****************************************************************************/
;
;/* ---------------------- data declarations ------------------------------- */
;

;/* ----------------------------------------------------------------------- */
;/*             VECTOR TABLE                                                */
;/* To know more about vector table, refer to ARM926 TRM : chapter 2        */
;/* ----------------------------------------------------------------------- */

;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg    dummy,   DUMMY_FUNC
   .elseif __TI_ARM9ABI_ASSEMBLER | .TMS470_32BIS
        .asg    _dummy,   DUMMY_FUNC
   .else
        .asg    $dummy,   DUMMY_FUNC
   .endif


    .state32
        .global _c_init_Host
        .global _arm_irq
        .global _arm_fiq
        .global _arm_swi

        .global DUMMY_FUNC


;/* Interrupt vectors definition */

        .sect ".intvecs"
        .global _vec_table

_vec_table:
  B _c_init_Host; reset interrupt
 .word 0xEAFFFFFE ; undefined instruction interrupt
  B _arm_swi ; software interrupt
 .word 0xEAFFFFFE ; abort (prefetch) interrupt
 .word 0xEAFFFFFE ; abort (data) interrupt
 .word 0xEAFFFFFE ; reserved
 .word 0xEAFFFFFE  ; IRQ interrupt
  B _arm_fiq  ; FIQ interrupt

; This function is just for the sake of linker in ARM EABI mode.
; Source code has to call this function once so that this file will 
; get included in the build
    .armfunc DUMMY_FUNC

DUMMY_FUNC: .asmfunc
    NOP
    NOP
    NOP
    BX      lr



