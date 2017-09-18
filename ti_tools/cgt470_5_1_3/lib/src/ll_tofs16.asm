;******************************************************************************
;* LL_TOFS16.ASM  - 16 BIT STATE -  v5.1.3                                    *
;*                                                                            *
;* Copyright (c) 1996-2013 Texas Instruments Incorporated                     *
;* http://www.ti.com/                                                         *
;*                                                                            *
;*  Redistribution and  use in source  and binary forms, with  or without     *
;*  modification,  are permitted provided  that the  following conditions     *
;*  are met:                                                                  *
;*                                                                            *
;*     Redistributions  of source  code must  retain the  above copyright     *
;*     notice, this list of conditions and the following disclaimer.          *
;*                                                                            *
;*     Redistributions in binary form  must reproduce the above copyright     *
;*     notice, this  list of conditions  and the following  disclaimer in     *
;*     the  documentation  and/or   other  materials  provided  with  the     *
;*     distribution.                                                          *
;*                                                                            *
;*     Neither the  name of Texas Instruments Incorporated  nor the names     *
;*     of its  contributors may  be used to  endorse or  promote products     *
;*     derived  from   this  software  without   specific  prior  written     *
;*     permission.                                                            *
;*                                                                            *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
;*                                                                            *
;******************************************************************************

;****************************************************************************
;* LL$TOFS - CONVERT A 64 BIT SIGNED INTEGER TO AN IEEE 754 FORMAT 
;*           SINGLE PRECISION FLOATING POINT NUMBER
;****************************************************************************
;*
;*   o INPUT OP IS IN R0:R1 (r1:r0 IF LITTLE ENDIAN)
;*   o RESULT IS RETURNED IN R0
;*   o THE VALUE IN R1 IS DESTROYED
;*
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;****************************************************************************
;*
;* +--------------------------------------------------------------+
;* | SINGLE PRECISION FLOATING POINT FORMAT                       |
;* |								  |
;* |   31 30    23 22                    0			  |
;* |   +-+--------+-----------------------+			  |
;* |   |S|    E   |           M           +			  |
;* |   +-+--------+-----------------------+			  |
;* |								  |
;* |   <S>  SIGN FIELD    :        0 - POSITIVE VALUE		  |
;* |			           1 - NEGATIVE VALUE		  |
;* |								  |
;* |   <E>  EXPONENT FIELD:       00 - ZERO IFF M == 0		  |
;* |			     01...FE - EXPONENT VALUE (127 BIAS)  |
;* |				  FF - INFINITY			  |
;* |								  |
;* |   <M>  MANTISSA FIELD:  FRACTIONAL MAGNITUDE WITH IMPLIED 1  |
;* +--------------------------------------------------------------+
;*
;****************************************************************************

	.state16

        .if __TI_EABI_ASSEMBLER         ; ASSIGN EXTERNAL NAMES BASED ON
        .asg __aeabi_l2f, __TI_LL$TOFS  ; RTS BEING BUILT
        .else
	.clink
        .asg LL$TOFS, __TI_LL$TOFS 
        .endif

e0	.set	r2
tmp	.set	r3
	.if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
sign	.set	lr
	.else
sign	.set	r4
	.endif
		
	.if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
	.thumbfunc __TI_LL$TOFS
	.endif

	.global __TI_LL$TOFS
__TI_LL$TOFS: .asmfunc stack_usage(12)
	.if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        PUSH	{r2, r3, lr}		; SAVE CONTEXT
	.else
	PUSH	{r2-r4}
	.endif

	; IN LITTLE ENDIAN MODE THE INPUT LONG LONG VALUE IS IN R1:R0. SWAP THE
	; WORDS SO THAT WE HAVE THE LONG LONG VAULUE IN R0:R1.

	.if .TMS470_LITTLE
	MOV	tmp, r0			;
	MOV	r0, r1			;
	MOV	r1, tmp			;
	.endif

	MOV	e0, #0xBE		; SET THE EXPONENT FIELD
        MOV     sign, r0                ; SAVE THE SIGN

	CMP	r0, #0			; IF ZERO, RETURN ZERO
	BMI	$1			;
	BNE	loop			;
        CMP     r1, #0                  ;
	BNE	loop            	;
	.if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
	POP	{r2, r3, pc}		;
	.else
	POP	{r2-r4}
	BX	lr
	.endif

$1:	
        MOV     tmp, #0                 ;
        NEG     r1, r1                  ;
        SBC     tmp, r0                 ;
        MOV     r0, tmp                 ;
	BMI	cont			;

loop:   SUB     e0, #1                  ; NORMALIZE THE MANTISSA
        LSL     r0, r0, #1              ; ADJUSTING THE EXPONENT, ACCORDINGLY
        LSR     tmp, r1, #31            ;
        ORR     r0, tmp                 ;
        LSL     r1, r1, #1              ;
        CMP     r0, #0                  ;
        BPL     loop                    ;

cont:	ADD	r0, #0x80		; ADD 1/2 TO ROUND
	BCC	$2			;
	ADD	e0, #0x1		; AND ADJUST THE EXPONENT ACCORDINGLY
	B	$3			;

$2:	LSL	r0, r0, #1		; MASK IMPLIED 1 OUT OF THE MANTISSA

$3:	LSR	r0, r0, #9		; PACK THE MANTISSA
	LSL	e0, e0, #23		;
	ORR	r0, e0			; PACK THE EXPONENT
	MOV	tmp, sign		; IF THE INPUT WAS NEGATIVE
        CMP     tmp, #0                 ;
	BPL	$4			;
	MOV	e0, #1			;
	LSL	e0, e0, #31		;
	ORR	r0, e0			;  THEN SET THE SIGN FIELD
$4:	
	.if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
	POP 	{r2, r3, pc}		; RESTORE CONTEXT
	.else
	POP	{r2-r4}
	BX	lr
	.endif

	.endasmfunc

	.end
