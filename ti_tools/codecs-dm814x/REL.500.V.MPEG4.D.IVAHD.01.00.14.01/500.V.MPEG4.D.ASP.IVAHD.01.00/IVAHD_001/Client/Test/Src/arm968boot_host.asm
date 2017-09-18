;******************************************************************************
;* BOOT  v4.5.0                                                               *
;* Copyright (c) 1996-2008 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* BOOT.ASM
;*
;* THIS IS THE INITAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
;* IT MUST BE LINKED AND LOADED WITH ALL C++ PROGRAMS.
;* 
;* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
;*   1) ALLOCATES THE STACK AND INITIALIZES THE STACK POINTER
;*   2) CALLS AUTO-INITIALIZATION ROUTINE
;*   3) CALLS THE FUNCTION MAIN TO START THE C++ PROGRAM
;*   4) CALLS THE STANDARD EXIT ROUTINE
;*
;* THIS MODULE DEFINES THE FOLLOWING GLOBAL SYMBOLS:
;*   1) __stack     STACK MEMORY AREA
;*   2) _c_init_Host    BOOT ROUTINE
;*
;****************************************************************************

;***************************************************************
;* DEFINE THE DIFFERENT STACK SECTIONS (C STACK should be 
;* defined in RTSx.lib)
;***************************************************************
__fstack:       .usect  ".fstack", 0x800,  4        
__sstack:       .usect  ".sstack", 0x600 , 4        
__istack:       .usect  ".istack", 0x400,  4
__ustack:       .usect  ".ustack", 0x50,   4
__ystack:       .usect  ".ystack", 0x50,   4
__astack:       .usect  ".astack", 0x50,   4


   .if  __TI_TMS470_V7M3__
    .thumbfunc _c_init_Host
   .else
    .armfunc _c_init_Host
   .endif

;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg    main,   ARGS_MAIN_RTN
        .asg    _goto_WFI,         EXIT_RTN
;        .asg    main_func_sp, MAIN_FUNC_SP
   .elseif __TI_ARM9ABI_ASSEMBLER | .TMS470_32BIS
        .asg    _main,   ARGS_MAIN_RTN
        .asg    __goto_WFI,         EXIT_RTN
;        .asg    _main_func_sp, MAIN_FUNC_SP
   .else
        .asg    $main,   ARGS_MAIN_RTN
        .asg    $_goto_WFI,         EXIT_RTN
;        .asg    _main_func_sp, MAIN_FUNC_SP
   .endif

   .if .TMS470_16BIS

;****************************************************************************
;*  16 BIT STATE BOOT ROUTINE                                               *
;****************************************************************************

   .if __TI_TMS470_V7M3__
    .state16
   .else
    .state32
   .endif

    .global __stack
    .global _vec_table

;***************************************************************
;* DEFINE THE USER MODE STACK (DEFAULT SIZE IS 512)               
;***************************************************************
__stack:.usect  ".stack", 0, 4

    .global _c_init_Host

;***************************************************************
;* FUNCTION DEF: _c_init_icont                                      
;***************************************************************
_c_init_Host: .asmfunc

    .if !__TI_TMS470_V7M3__
    .if __TI_NEON_SUPPORT__ | __TI_VFP_SUPPORT__
        ;*------------------------------------------------------
    ;* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
    ;* 10 AND 11, REQUIRED TO ENABLE NEON/VFP      
    ;* COPROCESSOR ACCESS CONTROL REG 
    ;* BITS [23:22] - CP11, [21:20] - CP10
    ;* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
        ;*------------------------------------------------------
        MRC      p15,#0x0,r0,c1,c0,#2
        MOV      r3,#0xf00000
        ORR      r0,r0,r3
        MCR      p15,#0x0,r0,c1,c0,#2

        ;*------------------------------------------------------
    ; SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
        ;*------------------------------------------------------
        MOV      r0,#0x40000000
        FMXR     FPEXC,r0
    .endif

    ;------------------------------------------------------
    ;* SET TO USER MODE
        ;*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0x1F  ; CLEAR MODES
        ORR     r0, r0, #0x10  ; SET USER MODE
        MSR     cpsr, r0

        ;*------------------------------------------------------
    ;* CHANGE TO 16 BIT STATE
        ;*------------------------------------------------------
        ADD r0, pc, #1
        BX  r0

    .state16
    .endif

    ;*------------------------------------------------------
        ;* INITIALIZE THE USER MODE STACK                      
        ;*------------------------------------------------------
    LDR     r0, c_stack
    MOV sp, r0
        LDR     r0, c_STACK_SIZE
    ADD sp, r0

    ;*-----------------------------------------------------
    ;* ALIGN THE STACK TO 64-BITS IF EABI.
    ;*-----------------------------------------------------
    .if __TI_EABI_ASSEMBLER
    MOV r7, sp
    MOV r0, #0x07
    BIC     r7, r0         ; Clear upper 3 bits for 64-bit alignment.
    MOV sp, r7
    .endif

    ;*-----------------------------------------------------
    ;* SAVE CURRENT STACK POINTER FOR SDP ANALYSIS
    ;*-----------------------------------------------------
;   LDR r0, c_mf_sp
;   MOV r7, sp
;   STR r7, [r0]

        ;*------------------------------------------------------
        ;* Perform all the required initilizations:
        ;*   - Process BINIT Table
        ;*   - Perform C auto initialization
        ;*   - Call global constructors 
        ;*------------------------------------------------------
        BL      __TI_auto_init

        ;*------------------------------------------------------
    ;* CALL APPLICATION                                     
        ;*------------------------------------------------------
        BL      ARGS_MAIN_RTN

        ;*------------------------------------------------------
    ;* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT(1)
        ;*------------------------------------------------------
        MOV     r0, #1
        BL      EXIT_RTN

        ;*------------------------------------------------------
    ;* DONE, LOOP FOREVER
        ;*------------------------------------------------------
L1:     B   L1
        B   _vec_table
    .endasmfunc

   .else           ; !.TMS470_16BIS

;****************************************************************************
;*  32 BIT STATE BOOT ROUTINE                                               *
;****************************************************************************

    .global __stack
    .global _vec_table

;***************************************************************
;* DEFINE THE USER MODE STACK (DEFAULT SIZE IS 512)            
;***************************************************************
__stack:.usect  ".stack", 0, 4

    .global _c_init_Host
;***************************************************************
;* FUNCTION DEF: _c_init_Host                                      
;***************************************************************
_c_init_Host: .asmfunc

    .if __TI_NEON_SUPPORT__ | __TI_VFP_SUPPORT__
        ;*------------------------------------------------------
    ;* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
    ;* 10 AND 11, REQUIRED TO ENABLE NEON/VFP      
    ;* COPROCESSOR ACCESS CONTROL REG 
    ;* BITS [23:22] - CP11, [21:20] - CP10
    ;* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
        ;*------------------------------------------------------
        MRC      p15,#0x0,r0,c1,c0,#2
        MOV      r3,#0xf00000
        ORR      r0,r0,r3
        MCR      p15,#0x0,r0,c1,c0,#2

        ;*------------------------------------------------------
        ; SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
        ;*------------------------------------------------------
        MOV      r0,#0x40000000
        FMXR     FPEXC,r0
    .endif

        BL  _initStack


    ;*------------------------------------------------------
    ;* Enable IRQ and FIQ
    ;*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0xC0  ; Enable IRQ and FIQ to work with WFI and with interrupt
        NOP                    ; IF you want to work with out WFI and with interrupt
        NOP                    ; Enable FIQ by replacing 0x00 with 0x40
        MSR     cpsr, r0


        ;*------------------------------------------------------
        ;* SET TO USER MODE
        ;*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0x1F  ; CLEAR MODES
        ORR     r0, r0, #0x10  ; SET USER MODE
        MSR     cpsr, r0

        ;*------------------------------------------------------
        ;* INITIALIZE THE USER MODE STACK                      
        ;*------------------------------------------------------
        LDR     sp, c_stack
        LDR     r0, c_STACK_SIZE
        ADD sp, sp, r0

    ;*-----------------------------------------------------
    ;* ALIGN THE STACK TO 64-BITS IF EABI.
    ;*-----------------------------------------------------
        .if __TI_EABI_ASSEMBLER
        BIC     sp, sp, #0x07  ; Clear upper 3 bits for 64-bit alignment.
        .endif

    ;*-----------------------------------------------------
    ;* SAVE CURRENT STACK POINTER FOR SDP ANALYSIS
    ;*-----------------------------------------------------
;   LDR r0, c_mf_sp
;   STR sp, [r0]
                
        ;*------------------------------------------------------
        ;* Perform all the required initilizations:
        ;*   - Process BINIT Table
        ;*   - Perform C auto initialization
        ;*   - Call global constructors 
        ;*------------------------------------------------------
        BL      __TI_auto_init

        ;*------------------------------------------------------
    ;* CALL APPLICATION                                     
        ;*------------------------------------------------------    
        BL      ARGS_MAIN_RTN

        ;*------------------------------------------------------
    ;* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT(1)
        ;*------------------------------------------------------
        MOV     R0, #1
        BL      EXIT_RTN

        ;*------------------------------------------------------
    ;* DONE, LOOP FOREVER
        ;*------------------------------------------------------
L1:     B   L1
        B   _vec_table
    .endasmfunc

   .endif    ; !.TMS470_16BIS

;***************************************************************
;* CONSTANTS USED BY THIS MODULE
;***************************************************************
c_stack     .long    __stack
c_STACK_SIZE    .long    __STACK_SIZE
;c_mf_sp            .long    MAIN_FUNC_SP

    .if __TI_EABI_ASSEMBLER
        .data
        .align 4
_stkchk_called:
        .field          0,32
        .else
        .sect   ".cinit"
        .align  4
        .field          4,32
        .field          _stkchk_called+0,32
        .field          0,32

        .bss    _stkchk_called,4,4
        .symdepend ".cinit", ".bss"
        .symdepend ".cinit", ".text"
        .symdepend ".bss", ".text"
    .endif

;******************************************************
;* UNDEFINED REFERENCES                               *
;******************************************************
    .global _stkchk_called
    .global __STACK_SIZE
    .global ARGS_MAIN_RTN
;   .global MAIN_FUNC_SP
    .global EXIT_RTN
;        .global __TI_auto_init

        .sect   ".text"
;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg    memcpy,  COPY_IN_RTN
   .elseif __TI_ARM9ABI_ASSEMBLER | .TMS470_32BIS
        .asg    C_MEMCPY, COPY_IN_RTN
   .else
        .asg    $memcpy, COPY_IN_RTN
   .endif

;   .global __TI_auto_init
        
;****************************************************************************
; 16-BIS ROUTINE
;****************************************************************************
   .if .TMS470_16BIS

        .state16

        .thumbfunc __TI_auto_init

__TI_auto_init:    .asmfunc stack_usage(20)
        PUSH    {r4-r7, lr}
        ;*------------------------------------------------------
        ;* PROCESS BINIT LINKER COPY TABLE.  IF BINIT IS -1, THEN
    ;* THERE IS NONE.
        ;*------------------------------------------------------
        LDR r0, c_binit
        MOV r7, #1 
        CMN r0, r7
        BEQ _b1_
        BL      COPY_IN_RTN

    .if __TI_EABI_ASSEMBLER
        ;*------------------------------------------------------
        ;* If eabi, process the compressed cinit table. The format
        ;* is as follows:                                      
    ;* |4-byte load addr|4-byte run addr|
    ;* |4-byte load addr|4-byte run addr|
        ;*                                                     
    ;* Processing steps:
        ;*   1. Read load and run address.                     
        ;*   2. Read one byte at load address, say idx.
        ;*   3. Get pointer to handler at handler_start[idx]
        ;*   4. call handler(load_addr + 1, run_addr)
        ;*------------------------------------------------------
_b1_:
        LDR     r5, c_cinit_start
        LDR     r7, c_cinit_end
        LDR     r6, handler_start
_b1_loop_:   
        CMP     r5,r7
        BCS     _b1_loop_end_
        LDMIA   r5!, {r0,r1}
        LDRB    r4, [r0]
        LSL     r4, r4, #2
        LDR     r4, [r6,r4]
        ADD     r0, r0, #1
        .if __TI_TMS470_V7M3__
        BLX     r4
        .else
        BL      IND$CALL
        .endif
        B       _b1_loop_
_b1_loop_end_:
        .else
        ;*------------------------------------------------------
        ;* PERFORM AUTO-INITIALIZATION.  IF CINIT IS -1, THEN
    ;* THERE IS NONE.
        ;*------------------------------------------------------
_b1_:   LDR r0, c_cinit
        MOV r7, #1 
        CMN r0, r7
        BEQ _c1_
        BL      perform_cinit
    .endif

_c1_:   LDR r5, c_pinit
        .if (!__TI_EABI_ASSEMBLER)
        ;*------------------------------------------------------
        ;* IN NON-EABI MODE, THERE IS NO INITIALIZATION ROUTINE
        ;* IF PINIT IS -1. ALSO, PINT IS NULL TERMINATED. ITERATE
        ;* OVER THE PINIT TABLE AND CALL THE INITIALIZATION ROUTINE
        ;* FOR CONSTRUCTORS.
    ;* NOTE THAT r7 IS PRESERVED ACROSS AUTO-INITIALIZATION.
        ;*------------------------------------------------------
        CMN r5, r7
        BEQ _c3_
        B   _c2_
_loop_: 
        .if __TI_TMS470_V7M3__
        BLX     r4
        .else
        BL      IND$CALL
        .endif
_c2_:   LDMIA   r5!, {r4}
        CMP r4, #0
        BNE _loop_
_c3_:
        .else
        ;*------------------------------------------------------
        ;* IN EABI MODE, INIT_ARRAY IS NOT NULL TERMINATED. START
        ;* FROM THE INIT_ARRAY START (C_PINIT) AND ITERATE TILL
        ;* INIT_ARRAY END (C_PINT_END)
        ;*------------------------------------------------------
        LDR    r7, c_pinit_end
_loop_:
        CMP     r5, r7
        BCS     _loop_end_      ; If r5 is GE r7, we have reached the end.
        LDMIA   r5!, {r4}
        .if __TI_TMS470_V7M3__
        BLX     r4
        .else
        BL      IND$CALL
        .endif
        B       _loop_
_loop_end_:
        .endif

    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        POP     {r4-r7, pc}             ;
        .else
        POP     {r4-r7}                 ;
        POP {r0}
        MOV lr, r0
        BX      lr
        .endif

    .endasmfunc


;***************************************************************************
;*  PROCESS INITIALIZATION TABLE.
;*
;*  THE TABLE CONSISTS OF A SEQUENCE OF RECORDS OF THE FOLLOWING FORMAT:
;*                                                                          
;*       .word  <length of data (bytes)>
;*       .word  <address of variable to initialize>                         
;*       .word  <data>
;*                                                                          
;*  THE INITIALIZATION TABLE IS TERMINATED WITH A ZERO LENGTH RECORD.
;*                                                                          
;***************************************************************************

tbl_addr  .set    r0
var_addr  .set    r1
tmp   .set    r2
length:   .set    r3
data:     .set    r4
three:    .set    r5

perform_cinit: .asmfunc
        MOV three, #3          ;
        B   rec_chk            ;

        ;*------------------------------------------------------
    ;* PROCESS AN INITIALIZATION RECORD
        ;*------------------------------------------------------
record: LDR var_addr, [tbl_addr, #4]   ;
        ADD tbl_addr, #8           ;

        ;*------------------------------------------------------
    ;* COPY THE INITIALIZATION DATA
        ;*------------------------------------------------------
        MOV tmp, var_addr          ; DETERMINE ALIGNMENT
        AND tmp, three         ; AND COPY BYTE BY BYTE
        BNE _bcopy             ; IF NOT WORD ALIGNED

        MOV tmp, length        ; FOR WORD COPY, STRIP
        AND tmp, three         ; OUT THE NONWORD PART
        BIC length, three          ; OF THE LENGTH
        BEQ _wcont             ;        

_wcopy: LDR data, [tbl_addr]       ;
        ADD tbl_addr, #4           ;
        STR data, [var_addr]       ; COPY A WORD OF DATA
        ADD var_addr, #4           ;
        SUB length, #4         ;
        BNE _wcopy                     ;
_wcont: MOV length, tmp        ;
        BEQ _cont              ;

_bcopy: LDRB    data, [tbl_addr]       ;
        ADD tbl_addr, #1           ;
        STRB    data, [var_addr]       ; COPY A BYTE OF DATA
        ADD var_addr, #1           ;
        SUB length, #1         ;
        BNE _bcopy                     ;

_cont:  MOV tmp, tbl_addr              ;
        AND tmp, three             ; MAKE SURE THE ADDRESS
        BEQ rec_chk            ; IS WORD ALIGNED
        BIC tbl_addr, three        ;
        ADD tbl_addr, #0x4             ;

rec_chk:LDR length, [tbl_addr]         ; PROCESS NEXT
        CMP length, #0                 ; RECORD IF LENGTH IS
        BNE record                     ; NONZERO 

        BX  lr
    .endasmfunc

   .else    ; !.TMS470_16BIS

    .armfunc __TI_auto_init

        .state32

;****************************************************************************
;*  AUTO INIT ROUTINE                                                       *
;****************************************************************************

;   .global __TI_auto_init
;***************************************************************
;* FUNCTION DEF: _c_int00                                      
;***************************************************************
__TI_auto_init: .asmfunc stack_usage(20)

        STMFD   sp!, {r4-r7, lr}
        ;*------------------------------------------------------
        ;* PROCESS BINIT LINKER COPY TABLE.  IF BINIT IS -1, THEN
    ;* THERE IS NONE.
        ;*------------------------------------------------------
        LDR r0, c_binit
        CMN r0, #1
            BLNE    COPY_IN_RTN

    .if __TI_EABI_ASSEMBLER
        ;*------------------------------------------------------
        ;* If eabi, process the compressed cinit table. The format
        ;* is as follows:                                      
    ;* |4-byte load addr|4-byte run addr|
    ;* |4-byte load addr|4-byte run addr|
        ;*                                                     
    ;* Processing steps:
        ;*   1. Read load and run address.                     
        ;*   2. Read one byte at load address, say idx.
        ;*   3. Get pointer to handler at handler_start[idx]
        ;*   4. call handler(load_addr + 1, run_addr)
        ;*------------------------------------------------------
        LDR     r5, c_cinit_start
        LDR     r7, c_cinit_end
        LDR     r6, handler_start
_b1_loop_:   
        CMP     r5,r7
        BCS     _b1_loop_end_
        LDMIA   r5!, {r0,r1}
        LDRB    r4, [r0]
        MOV     r4, r4, LSL #2
        LDR     r4, [r6,r4]
        ADD     r0, r0, #1
        BL      IND_CALL
        B       _b1_loop_
_b1_loop_end_:
        .else
        ;*------------------------------------------------------
        ;* PERFORM COFF MODE AUTO-INITIALIZATION.  IF CINIT IS -1, THEN
    ;* THERE IS NO CINIT RECORDS TO PROCESS.
        ;*------------------------------------------------------
        LDR r0, c_cinit
        CMN r0, #1
        BLNE    perform_cinit
        .endif

        ;*------------------------------------------------------
    ;* CALL INITIALIZATION ROUTINES FOR CONSTRUCTORS. IF
    ;* PINIT IS -1, THEN THERE ARE NONE.
        ;*------------------------------------------------------
        LDR r5, c_pinit
        .if (!__TI_EABI_ASSEMBLER)
        ;*------------------------------------------------------
        ;* IN NON-EABI MODE, THERE IS NO INITIALIZATION ROUTINE
        ;* IF PINIT IS -1. ALSO, PINT IS NULL TERMINATED. ITERATE
        ;* OVER THE PINIT TABLE AND CALL THE INITIALIZATION ROUTINE
        ;* FOR CONSTRUCTORS.
        ;*------------------------------------------------------
        CMN r5, #1
        BEQ _c2_
        B   _c1_
_loop_: BL  IND_CALL
_c1_:   LDR r4, [r5], #4
        CMP r4, #0
        BNE _loop_
_c2_:
        .else
        ;*------------------------------------------------------
        ;* IN EABI MODE, INIT_ARRAY IS NOT NULL TERMINATED. START
        ;* FROM THE INIT_ARRAY START (C_PINIT) AND ITERATE TILL
        ;* INIT_ARRAY END (C_PINT_END)
        ;*------------------------------------------------------
        LDR    r7, c_pinit_end
_loop_:
        CMP     r5, r7
        BCS     _loop_end_      ; If r5 is GE r7, we have reached the end.
        LDR     r4, [r5], #4
        BL      IND_CALL
        B       _loop_
_loop_end_:
        .endif

    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMFD   sp!, {r4-r7, pc}    ;
    .else
        LDMFD   sp!, {r4-r7, lr}
        BX  lr
    .endif

    .endasmfunc

;***************************************************************************
;*  PROCESS INITIALIZATION TABLE.
;*
;*  THE TABLE CONSISTS OF A SEQUENCE OF RECORDS OF THE FOLLOWING FORMAT:
;*                                                                          
;*       .word  <length of data (bytes)>
;*       .word  <address of variable to initialize>                         
;*       .word  <data>
;*                                                                          
;*  THE INITIALIZATION TABLE IS TERMINATED WITH A ZERO LENGTH RECORD.
;*                                                                          
;***************************************************************************

tbl_addr: .set    R0
var_addr: .set    R1
length:   .set    R2
data:     .set    R3

perform_cinit: .asmfunc
        B   rec_chk

        ;*------------------------------------------------------
    ;* PROCESS AN INITIALIZATION RECORD
        ;*------------------------------------------------------
record: LDR var_addr, [tbl_addr], #4   ;

        ;*------------------------------------------------------
    ;* COPY THE INITIALIZATION DATA
        ;*------------------------------------------------------
        TST var_addr, #3           ; SEE IF DEST IS ALIGNED
        BNE     _bcopy             ; IF NOT, COPY BYTES
        SUBS    length, length, #4     ; IF length <= 3, ALSO
        BMI     _bcont             ; COPY BYTES

_wcopy: LDR data, [tbl_addr], #4       ;
        STR data, [var_addr], #4       ; COPY A WORD OF DATA
        SUBS    length, length, #4     ;
        BPL _wcopy             ;
_bcont: ADDS    length, length, #4     ;
        BEQ _cont              ;

_bcopy: LDRB    data, [tbl_addr], #1       ;
        STRB    data, [var_addr], #1       ; COPY A BYTE OF DATA
        SUBS    length, length, #1     ;
        BNE _bcopy                     ;

_cont:  ANDS    length, tbl_addr, #0x3     ; MAKE SURE THE ADDRESS
        RSBNE   length, length, #0x4       ; IS WORD ALIGNED
        ADDNE   tbl_addr, tbl_addr, length ;

rec_chk:LDR length, [tbl_addr], #4     ; PROCESS NEXT
        CMP length, #0                 ; RECORD IF LENGTH IS
        BNE record                     ; NONZERO

        BX  LR
    .endasmfunc

   .endif    ; !.TMS470_16BIS

;***************************************************************
;* CONSTANTS USED BY THIS MODULE
;***************************************************************
c_binit         .long    binit

   .if __TI_EABI_ASSEMBLER
c_pinit         .long    SHT$$INIT_ARRAY$$Base
c_pinit_end     .long    SHT$$INIT_ARRAY$$Limit
c_cinit_start   .long    __TI_CINIT_Base
c_cinit_end     .long    __TI_CINIT_Limit
handler_start   .long    __TI_Handler_Table_Base
   .else
c_pinit         .long    pinit
c_cinit         .long    cinit
   .endif


;******************************************************
;* UNDEFINED REFERENCES                               *
;******************************************************
    .global binit
    .global cinit
    .global COPY_IN_RTN

   .if .TMS470_16BIS
    .global IND$CALL
   .else
    .global IND_CALL
   .endif

   .if __TI_EABI_ASSEMBLER
        .weak   SHT$$INIT_ARRAY$$Base
        .weak   SHT$$INIT_ARRAY$$Limit
        .weak   __TI_CINIT_Base
        .weak   __TI_CINIT_Limit
        .weak   __TI_Handler_Table_Base
   .else 
    .global pinit
   .endif

;****************************************************************************
;* IND_CALL - PERFORM INDIRECT CALL, SUPPORTING DUAL STATE INTERWORKING
;*
;****************************************************************************
;*
;*   o ADDRESS OF CALLED FUNCTION IS IN r4
;*   o r4 GETS DESTROYED
;*
;****************************************************************************
    .state32
    
        .if !__TI_EABI_ASSEMBLER
    .clink
    .endif
    
    .if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
    .armfunc IND_CALL
    .endif
    
    .global IND_CALL
IND_CALL: .asmfunc
        TST r4, #1          ; TEST STATE OF DESTINATION
        BNE ep_16                   ;
        BX  r4                      ; INDIRECT CALL TO 32BIS ENTRY POINT

ep_16:  MOV ip, r4          ; FREE r4 BY STORING ITS VALUE IN ip
        MOV r4, lr          ; STORE RETURN ADDRESS IN r4
        ADD lr, pc, #1      ; SETUP NEW RETURN ADDRESS
        BX  ip          ; INDIRECT CALL TO 16BIS ENTRY POINT
        .state16
        BX  r4          ; RETURN
        NOP             ;
        .state32

    .endasmfunc






   .if .TMS470_16BIS

;******************************************************************************
;* MEMCPY16.ASM  - 16 BIT STATE -  v4.5.0                                     *
;* Copyright (c) 1996-2008 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* C$MEMCPY - COPY CHARACTERS FROM SOURCE TO DEST
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o SOURCE LOCATION IS IN r1
;*   o NUMBER OF CHARACTERS TO BE COPIED IS IN r2
;****************************************************************************
    ; IF BUILDING EABI RTS, INCLUDE THUMB VERSION OF RTS FUNCTION
    ; ONLY IF BUILDING 32BIS RTS LIBRARY
    .if (!__TI_EABI_ASSEMBLER | .TMS470_16BIS) & !.TMS470_T2IS

        .state16
 
        .if __TI_EABI_ASSEMBLER 
        .asg memcpy, __TI_C$MEMCPY
    .thumbfunc __aeabi_memcpy
    .thumbfunc __aeabi_memcpy4
    .thumbfunc __aeabi_memcpy8
    .global __aeabi_memcpy
    .global __aeabi_memcpy4
    .global __aeabi_memcpy8
__aeabi_memcpy:
__aeabi_memcpy4:
__aeabi_memcpy8:
        .else
    .clink
        .asg C$MEMCPY, __TI_C$MEMCPY
        .endif
 
    .if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
    .thumbfunc __TI_C$MEMCPY
    .endif

        .global __TI_C$MEMCPY
__TI_C$MEMCPY: .asmfunc stack_usage(0)
        CMP     r2, #0                  ; CHECK FOR n == 0
        BEQ     _ret2_          ;
 
        MOV r12, r0         ; SAVE RETURN VALUE

        LSL r3, r1, #30     ; CHECK ADDRESS ALIGNMENT
        BNE _unaln          ; IF NOT WORD ALIGNED, HANDLE SPECIALLY
        LSL r3, r0, #30     ;
        BNE _saln           ;

_aln:   CMP r2, #16         ; ADDRESSES AND LENGTH ARE WORD ALIGNED
        BCC _udr16          ; IF n < 16, SKIP 16 BYTE COPY CODE

_ovr16: PUSH    {r4 - r6}       ; COPYING 16 BYTES OR MORE.
        SUB r2, #16         ;
_lp16:  LDMIA   r1!, {r3 - r6}      ;
        STMIA   r0!, {r3 - r6}      ;
        SUB r2, #16         ;
        BCS _lp16           ;
        POP {r4 - r6}       ; RESTORE THE SAVED REGISTERS AND
        ADD r2, #16         ; CONTINUE THE COPY IF THE REMAINDER
        BEQ _ret_           ; IS NONZERO.
    
_udr16: CMP r2, #4          ; IF UNDER 16, THEN COMPUTE 
        BCC _off1           ; THE COPY CODE TO EXECUTE, AND
        CMP r2, #8          ; GO THERE
        BCC _c4         ;
        CMP r2, #12         ;
        BCC _c8         ;

_c12:   LDMIA   r1!, {r3}       ; COPY 12 BYTES
        STMIA   r0!, {r3}       ; 
_c8:    LDMIA   r1!, {r3}       ; COPY 8 BYTES
        STMIA   r0!, {r3}       ;
_c4:    LDMIA   r1!, {r3}       ; COPY 4 BYTES
        STMIA   r0!, {r3}       ;

_oddsz: LSL r2, r2, #30     ; HANDLE THE TRAILING BYTES
        BEQ _ret_           ;
        LSR r2, r2, #30     ;
        B   _lp1            ;

_unaln: LDRB    r3, [r1]        ; THE ADDRESSES ARE NOT WORD ALIGNED.
        STRB    r3, [r0]        ; COPY BYTES UNTIL THE SOURCE IS
        ADD r1, r1, #1      ;
        ADD r0, r0, #1      ;
        SUB r2, r2, #1      ; WORD ALIGNED OR THE COPY SIZE
        BEQ _ret_           ; BECOMES ZERO
        LSL r3, r1, #30     ;
        BNE _unaln          ;

_saln:  LSL r3, r0, #31     ; IF THE ADDRESSES ARE OFF BY 1 BYTE
        BNE _off1           ; JUST BYTE COPY

        LSL r3, r0, #30     ; IF THE ADDRESSES ARE NOW WORD ALIGNED
        BEQ _aln            ; GO COPY.  ELSE THEY ARE OFF BY 2, SO
                    ; GO SHORT WORD COPY

_off2:  SUB r2, r2, #4      ; COPY 2 BYTES AT A TIME...
        BCC _oddb           ;
_cp4s:  LDMIA   r1!, {r3}       ; LOAD IN CHUNKS OF 4
    .if .TMS470_BIG
        STRH    r3, [r0, #2]        ;
        LSR r3, r3, #16     ;
        STRH    r3, [r0]        ;
    .else
        STRH    r3, [r0]        ;
        LSR r3, r3, #16     ;
        STRH    r3, [r0, #2]        ;
    .endif
        ADD r0, r0, #4      ;
        SUB r2, r2, #4      ;
        BCS _cp4s           ;
        B   _oddb           ;

_cp4:   LDMIA   r1!, {r3}       ; COPY 1 BYTE AT A TIME, IN CHUNKS OF 4
    .if .TMS470_BIG
        STRB    r3, [r0, #3]        ;
        LSR r3, r3, #8      ;
        STRB    r3, [r0, #2]        ;
        LSR r3, r3, #8      ;
        STRB    r3, [r0, #1]        ;
        LSR r3, r3, #8      ;
        STRB    r3, [r0]        ;
    .else
        STRB    r3, [r0]        ;
        LSR r3, r3, #8      ;
        STRB    r3, [r0, #1]        ;
        LSR r3, r3, #8      ;
        STRB    r3, [r0, #2]        ;
        LSR r3, r3, #8      ;
        STRB    r3, [r0, #3]        ;
    .endif
        ADD r0, r0, #4      ;
_off1:  SUB r2, r2, #4      ;
        BCS _cp4            ;

_oddb:  ADD r2, r2, #4      ; THEN COPY THE ODD BYTES.
        BEQ _ret_           ;
    
_lp1:   LDRB    r3, [r1]        ;
        STRB    r3, [r0]        ;
        ADD r1, r1, #1      ;
        ADD r0, r0, #1      ;
        SUB r2, r2, #1      ;
        BNE _lp1            ;
_ret_:  MOV r0, r12         ;
_ret2_: BX  lr

    .endasmfunc

    .endif                          ; !__TI_EABI_ASSEMBLER | .TMS470_16BIS





    .else ;   .if .TMS470_16BIS

;******************************************************************************
;* MEMCPY32.ASM  - 32 BIT STATE -  v4.5.0                                     *
;* Copyright (c) 1996-2008 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* C_MEMCPY - COPY CHARACTERS FROM SOURCE TO DEST
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o SOURCE LOCATION IS IN r1
;*   o NUMBER OF CHARACTERS TO BE COPIED IS IN r2
;****************************************************************************
    ; IF BUILDING EABI RTS, INCLUDE ARM VERSION OF RTS FUNCTION
    ; ONLY IF BUILDING 32BIS RTS LIBRARY
    .if !__TI_EABI_ASSEMBLER | .TMS470_32BIS

        .state32
 
        .if __TI_EABI_ASSEMBLER 
        .asg memcpy, __TI_C_MEMCPY
    .armfunc __aeabi_memcpy
    .armfunc __aeabi_memcpy4
    .armfunc __aeabi_memcpy8
    .global __aeabi_memcpy
    .global __aeabi_memcpy4
    .global __aeabi_memcpy8
__aeabi_memcpy:
__aeabi_memcpy4:
__aeabi_memcpy8:
        .else
    .clink
        .asg C_MEMCPY, __TI_C_MEMCPY
        .endif

    .if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
    .armfunc __TI_C_MEMCPY
    .endif

        .global __TI_C_MEMCPY
__TI_C_MEMCPY: .asmfunc stack_usage(12)
        CMP r2, #0          ; CHECK FOR n == 0
        BXEQ    lr          ;

        STMFD   sp!, {r0, lr}       ; SAVE RETURN VALUE AND ADDRESS

        TST r1, #0x3        ; CHECK ADDRESS ALIGNMENT
        BNE _unaln          ; IF NOT WORD ALIGNED, HANDLE SPECIALLY
        TST r0, #0x3        ;
        BNE _saln           ;

_aln:   CMP r2, #16         ; CHECK FOR n >= 16
        BCC _l16            ;

        STMFD   sp!, {r4}       ;
        SUB r2, r2, #16     ;
_c16:   LDMIA   r1!, {r3, r4, r12, lr}  ; COPY 16 BYTES
        STMIA   r0!, {r3, r4, r12, lr}  ;
        SUBS    r2, r2, #16     ;
        BCS _c16            ;
        LDMFD   sp!, {r4}       ;
        ADDS    r2, r2, #16     ; RETURN IF DONE
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ;
    .else
        LDMEQFD sp!, {r0, lr}
        BXEQ    lr
    .endif
    
_l16:   ANDS    r3, r2, #0xC        ;
        BEQ _cp1            ;
        BICS    r2, r2, #0xC        ;
        ADR r12, _4line - 16    ;
        ADD pc, r12, r3, LSL #2 ;

_4line: LDR r3, [r1], #4        ; COPY 4 BYTES
        STR r3, [r0], #4        ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ; CHECK FOR n == 0
    .else
        BEQ _ret
    .endif
        B   _cp1            ;

        LDMIA   r1!, {r3, r12}      ; COPY 8 BYTES
        STMIA   r0!, {r3, r12}      ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ; CHECK FOR n == 0
    .else
        BEQ _ret
    .endif
        B   _cp1            ;

        LDMIA   r1!, {r3, r12, lr}  ; COPY 12 BYTES
        STMIA   r0!, {r3, r12, lr}  ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ; CHECK FOR n == 0
    .else
        LDMEQFD sp!, {r0, lr}
        BXEQ    lr
    .endif

_cp1:   SUBS    r2, r2, #1      ;
        ADRNE   r3, _1line - 4      ; SETUP TO COPY 1 - 3 BYTES...
        ADDNE   pc, r3, r2, LSL #4  ;

_1line: LDRB    r3, [r1], #1        ; COPY 1 BYTE
        STRB    r3, [r0], #1        ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMFD   sp!, {r0, pc}       ;
    .else
        B   _ret
    .endif

        LDRH    r3, [r1], #2        ; COPY 2 BYTES
        STRH    r3, [r0], #2        ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMFD   sp!, {r0, pc}       ;
        NOP
    .else
        LDMFD   sp!, {r0, lr}
        BX  lr
    .endif

        LDRH    r3, [r1], #2        ; COPY 3 BYTES
        STRH    r3, [r0], #2        ;
        LDRB    r3, [r1], #1        ;
        STRB    r3, [r0], #1        ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMFD   sp!, {r0, pc}       ;
    .else
        LDMFD   sp!, {r0, lr}
        BX  lr
    .endif

_unaln: LDRB    r3, [r1], #1        ; THE ADDRESSES ARE NOT WORD ALIGNED.
        STRB    r3, [r0], #1        ; COPY BYTES UNTIL THE SOURCE IS
        SUBS    r2, r2, #1      ; WORD ALIGNED OR THE COPY SIZE
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ; BECOMES ZERO
    .else
        LDMEQFD sp!, {r0, lr}
        BXEQ    lr
    .endif
        TST r1, #0x3        ;
        BNE _unaln          ;

_saln:  TST r0, #0x1        ; IF THE ADDRESSES ARE OFF BY 1 BYTE
        BNE _off1           ; JUST BYTE COPY

        TST r0, #0x2        ; IF THE ADDRESSES ARE NOW WORD ALIGNED
        BEQ _aln            ; GO COPY.  ELSE THEY ARE OFF BY 2, SO
                    ; GO SHORT WORD COPY

_off2:  SUBS    r2, r2, #4      ; COPY 2 BYTES AT A TIME...
        BCC _c1h            ;
_c2:    LDR r3, [r1], #4        ; START BY COPYING CHUNKS OF 4,
    .if .TMS470_BIG
        STRH    r3, [r0, #2]        ;
        MOV r3, r3, LSR #16     ;
        STRH    r3, [r0], #4        ;
        .else
        STRH    r3, [r0], #4        ;
        MOV r3, r3, LSR #16     ;
        STRH    r3, [r0, #-2]       ;
    .endif
        SUBS    r2, r2, #4      ;
        BCS _c2         ;
        CMN r2, #4          ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ;
    .else
        LDMEQFD sp!, {r0, lr}
        BXEQ    lr
    .endif

_c1h:   ADDS    r2, r2, #2      ; THEN COPY THE ODD BYTES.
        LDRCSH  r3, [r1], #2        ;
        STRCSH  r3, [r0], #2        ;
        SUBCS   r2, r2, #2      ;
        ADDS    r2, r2, #1      ;
        LDRCSB  r3, [r1], #1        ;
        STRCSB  r3, [r0], #1        ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMFD   sp!, {r0, pc}       ;
    .else
        LDMFD   sp!, {r0, lr}
    BX  lr
    .endif

_off1:  SUBS    r2, r2, #4      ; COPY 1 BYTE AT A TIME...
        BCC _c1b            ;
_c1:    LDR r3, [r1], #4        ; START BY COPYING CHUNKS OF 4,
    .if .TMS470_BIG
        STRB    r3, [r0, #3]        ;
        MOV r3, r3, LSR #8      ;
        STRB    r3, [r0, #2]        ;
        MOV r3, r3, LSR #8      ;
        STRB    r3, [r0, #1]        ;
        MOV r3, r3, LSR #8      ;
        STRB    r3, [r0], #4        ;
    .else
        STRB    r3, [r0], #4        ;
        MOV r3, r3, LSR #8      ;
        STRB    r3, [r0, #-3]       ;
        MOV r3, r3, LSR #8      ;
        STRB    r3, [r0, #-2]       ;
        MOV r3, r3, LSR #8      ;
        STRB    r3, [r0, #-1]       ;
    .endif
        SUBS    r2, r2, #4      ;
        BCS _c1         ;

_c1b:   ADDS    r2, r2, #4      ; THEN COPY THE ODD BYTES.
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        LDMEQFD sp!, {r0, pc}       ;
    .else
        LDMEQFD sp!, {r0, lr}
        BXEQ    lr
    .endif
_lp1:   LDRB    r3, [r1], #1        ;
        STRB    r3, [r0], #1        ;
        SUBS    r2, r2, #1      ;
        BNE _lp1            ;
    .if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
_ret:   LDMFD   sp!, {r0, pc}       ;
    .else
_ret:   LDMFD   sp!, {r0, lr}
        BX  lr
    .endif

    .endasmfunc

    .endif                          ; !__TI_EABI_ASSEMBLER | .TMS470_32BIS


    .endif ;   .if .TMS470_16BIS



;/* ===========================================================================
; *  @fn     goto_WFI()
; *      
; *  @desc     Function make the ARM968 to go inot low power sleep state
; *            For more details  refer to ARM Assembler guide
; *            LN# SPNU118   - TMS470R1x Assembly Language Tool User's Guide  
; *
; *  @param    None
; *
; *  @return   None
; *  ==========================================================================
; */
    .global EXIT_RTN
    .armfunc EXIT_RTN
    .state32

    .sect ".text"   ;keeping in text should be OK


EXIT_RTN: 
    
        MOV r0,#0
        MCR p15, #0, R0, c7, c10, #4    ;Drain write buffers
        MCR p15, #0, R0, c7, c0, #4     ;Enter wait-for-interrupt state
    ;/*===================================================================*/
    ;/* If interrupts are disabled, This statemnt gets executed
    ;/*  just after an interrupts happens
    ;/*  If interrupts are enable, ARM will service the interrupt
    ;/*  and then reach the below statement 
    ;/*===================================================================*/
        MOVS    pc, lr          ; return from WFI





     .global _arm_fiq
     .global EXIT_RTN
     .global _arm_swi

;/* ===========================================================================
; *  @fn     arm_swi()
; *      
; *  @desc     Function is the low level SWI handler. This function gets called
; *            whenever a SWI is raised. It enters into WFI whenever SWI is 
; *            raised . There is no other action performed in the SWI call.
; *            For details of assemply intruction, refer
; *            to ARM Assembler guide
; *            LN# SPNU118   - TMS470R1x Assembly Language Tool User's Guide  
; *
; *  @param    None
; *
; *  @return   None
; *  ==========================================================================
; */
  
    .align  4
    .armfunc _arm_swi
; .global _fiq_irq_enable
    .state32

    .sect ".text"   ;keeping in text should be OK


_arm_swi: 
        STMFD   sp!, {r0-r12, lr} ; store user's gp registers
; B     _fiq_irq_enable

;/* Handler code enter into  WFI mode */
;
; MOV r0,#0
; MCR p15, #0, R0, c7, c10, #4  ;Drain write buffers
; MCR p15, #0, R0, c7, c0, #4   ;Enter wait-for-interrupt state
;
        LDMFD   sp!, {r0-r12, pc}^ ; unstack user's registers 


;/* ===========================================================================
; *  @fn     arm_fiq()
; *      
; *  @desc     Function is the low level FIQ handler. This function gets called
; *            whenever a FIQ is raised. When Host ARM926 wants to wake up 
; *            ARM968 to perform frame encode/decode tasks, it send an FIQ. This
; *            FIQ handler updates a global register and them comes out. FIQ 
; *            also results in ARM coming out of WFI mode and start the 
; *            frame processing.
; *            For more details  refer to ARM Assembler guide
; *            LN# SPNU118   - TMS470R1x Assembly Language Tool User's Guide  
; *
; *  @param    None
; *
; *  @return   None
; *  ==========================================================================
; */


    .armfunc _arm_fiq
    .state32

    .sect ".text"   ;keeping in text should be OK
;poll_reg .field 0x00400000,32 ;This field is used to store the poll register
;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg    arm_isr,   EXT_ISR
   .elseif __TI_ARM9ABI_ASSEMBLER | .TMS470_32BIS
        .asg    _arm_isr,   EXT_ISR
   .else
        .asg    $arm_isr,   EXT_ISR
   .endif

    .global EXT_ISR


_arm_fiq: 
        SUBS    lr, lr, #4  ; return from handler
        STMFD   sp!, {r0-r12, lr} ; store user's gp registers
;   LDR     r1, EXT_ISR
;   BLX    r1
        BL  EXT_ISR
        LDMFD   sp!, {r0-r12, pc}^ ; unstack user's registers 

;*------------------------------------------------------
;*  STACK INITIALIZE (see .CMD file)       
;*------------------------------------------------------
    .global __S_STACK_SIZE
    .global __I_STACK_SIZE
    .global __U_STACK_SIZE
    .global __F_STACK_SIZE
    .global __Y_STACK_SIZE
    .global __A_STACK_SIZE

;   .global __IRQ_STACK
;   .global __FIQ_STACK
;   .global __UNDEF_STACK
;   .global __SWI_STACK


    .state32

_initStack:
;*------------------------------------------------------
;* SET TO IRQ  MODE
;*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0x1F  ; CLEAR MODES
        ORR     r0, r0, #0x12  ; SET  IRQ MODE
        MSR     cpsr, r0

;*------------------------------------------------------
;* INITIALIZE THE IRQ  MODE STACK                      
;*------------------------------------------------------
        LDR     SP, i_stack
        LDR     R0, i_STACK_SIZE
        ADD SP, SP, R0
        BIC     sp, sp, #0x07  ; Clear lower 3 bits for 64-bit alignment.

;*------------------------------------------------------
;* SET TO FIQ  MODE
;*------------------------------------------------------
        MRS r0, cpsr
        BIC r0, r0, #0x1F   ; CLEAR MODES
        ORR r0, r0, #0x11   ; SET FIQ mode
        MSR cpsr, r0

;*------------------------------------------------------
;* INITIALIZE THE FIQ  MODE STACK                      
;*------------------------------------------------------
        LDR     SP, f_stack
        LDR     R0, f_STACK_SIZE
        ADD SP, SP, R0
        BIC     sp, sp, #0x07  ; Clear lower 3 bits for 64-bit alignment.
    
;*------------------------------------------------------
;* SET TO UNDEF  MODE
;*------------------------------------------------------
        MRS r0, cpsr
        BIC r0, r0, #0x1F   ; CLEAR MODES
        ORR r0, r0, #0x1B   ; SET UNDEF mode
        MSR cpsr, r0

;*------------------------------------------------------
;* INITIALIZE THE UNDEF  MODE STACK                      
;*------------------------------------------------------
        LDR     SP, u_stack
        LDR     R0, u_STACK_SIZE
        ADD SP, SP, R0
        BIC     sp, sp, #0x07  ; Clear lower 3 bits for 64-bit alignment.
                                
;*------------------------------------------------------
;* SET TO SYSTEM  MODE
;*------------------------------------------------------
        MRS r0, cpsr
        BIC r0, r0, #0x1F   ; CLEAR MODES
        ORR r0, r0, #0x1F   ; SET SYSTEM mode
        MSR cpsr, r0       
        
;*------------------------------------------------------
;* INITIALIZE THE SYSTEM MODE STACK                      
;*------------------------------------------------------
        LDR     SP, y_stack
        LDR     R0, y_STACK_SIZE
        ADD SP, SP, R0
        BIC     sp, sp, #0x07  ; Clear lower 3 bits for 64-bit alignment.

;*------------------------------------------------------
;* SET TO ABORT  MODE
;*------------------------------------------------------
        MRS r0, cpsr
        BIC r0, r0, #0x1F   ; CLEAR MODES
        ORR r0, r0, #0x17   ; SET ABORT mode
        MSR cpsr, r0       
        
;*------------------------------------------------------
;* INITIALIZE THE ABORT MODE STACK                      
;*------------------------------------------------------
        LDR     SP, a_stack
        LDR     R0, a_STACK_SIZE
        ADD SP, SP, R0
        BIC     sp, sp, #0x07  ; Clear lower 3 bits for 64-bit alignment.


;*------------------------------------------------------
;* SET TO SUPERVISOR  MODE
;*------------------------------------------------------
        MRS r0, cpsr
        BIC r0, r0, #0x1F   ; CLEAR MODES
        ORR r0, r0, #0x13   ; SET SUPERVISOR mode
        MSR cpsr, r0       
        
;*------------------------------------------------------
;* INITIALIZE THE SUPERVISOR  MODE STACK                      
;*------------------------------------------------------
        LDR     SP, s_stack
        LDR     R0, s_STACK_SIZE
        ADD SP, SP, R0
        BIC     sp, sp, #0x07  ; Clear lower 3 bits for 64-bit alignment.

        MOV PC, LR
       
      
s_stack         .long    __sstack
s_STACK_SIZE    .long    __S_STACK_SIZE
i_stack         .long    __istack
i_STACK_SIZE    .long    __I_STACK_SIZE  
f_stack         .long    __fstack
f_STACK_SIZE    .long    __F_STACK_SIZE  
u_stack         .long    __ustack
u_STACK_SIZE    .long    __U_STACK_SIZE  
y_stack         .long    __ystack
y_STACK_SIZE    .long    __Y_STACK_SIZE  
a_stack         .long    __astack
a_STACK_SIZE    .long    __A_STACK_SIZE  


_tc_pattern:
  NOP
_tc_passed:
  NOP
_tc_failed:
  NOP


    .end

