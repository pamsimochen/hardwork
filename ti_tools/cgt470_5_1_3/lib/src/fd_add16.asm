;******************************************************************************
;* FS_ADD16.ASM  - 16 BIT STATE -  v5.1.3                                     *
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

;******************************************************************************
;* FD_ADD/FD_SUB - ADD / SUBTRACT TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*   	           POINT NUMBERS.
;******************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1
;*   o INPUT OP2 IS IN r2:r3
;*   o RESULT IS RETURNED IN r0:r1
;*   o INPUT OP2 IN r2:r3 IS PRESERVED
;*
;*   o SUBTRACTION, OP1 - OP2, IS IMPLEMENTED WITH ADDITION, OP1 + (-OP2)
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST (TIE TO EVEN)
;*
;*   o IF OPERATION INVOLVES INFINITY AS AN INPUT, THE FOLLOWING SUMMARIZES
;*     THE RESULT:
;*                   +----------+----------+----------+ 
;*         ADDITION  + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   -INF   +   +INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*
;*                   +----------+----------+----------+ 
;*       SUBTRACTION + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   +INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*
;****************************************************************************
*
;* +------------------------------------------------------------------+
;* | DOUBLE PRECISION FLOATING POINT FORMAT                           |
;* |   64-bit representation                                          |
;* |   31 30      20 19                  0                            |
;* |   +-+----------+---------------------+                           |
;* |   |S|     E    |        M1           |                           |
;* |   +-+----------+---------------------+                           |
;* |                                                                  |
;* |   31                                0                            |
;* |   +----------------------------------+                           |
;* |   |             M2                   |                           |
;* |   +----------------------------------+                           |
;* |                                                                  |
;* |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE               |
;* |                                 1 - NEGATIVE VALUE               |
;* |                                                                  |
;* |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0              |
;* |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)    |
;* |                        1111111111 - INFINITY                     |
;* |                                                                  |
;* |   <M1:M2>  MANTISSA FIELDS:  FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
;* +------------------------------------------------------------------+
;*
;****************************************************************************

	.state16

        .if __TI_EABI_ASSEMBLER            ; ASSIGN EXTERNAL NAMES BASED ON
        .asg __aeabi_dadd, __TI_FD$ADD     ; RTS BEING BUILT
        .asg __aeabi_dsub, __TI_FD$SUB     
        .else
	.clink
        .asg FD$ADD, __TI_FD$ADD
        .asg FD$SUB, __TI_FD$SUB
        .endif

	.global	__TI_FD$ADD
	.global	__TI_FD$SUB

	.if .TMS470_BIG_DOUBLE

rp1_hi	.set	r0	; High word of regpair 1
rp1_lo	.set	r1	; Low word of regpair 1
rp2_hi	.set	r2	; High word of regpair 2
rp2_lo	.set	r3	; Low word of regpair 2
shift	.set	r0	; OVERLOADED WITH INPUT #1
op1e	.set	r1	; OVERLOADED WITH INPUT #1
op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2

	.else

rp1_hi	.set	r1	; High word of regpair 1
rp1_lo	.set	r0	; Low word of regpair 1
rp2_hi	.set	r3	; High word of regpair 2
rp2_lo	.set	r2	; Low word of regpair 2
shift	.set	r1	; OVERLOADED WITH INPUT #1
op1e	.set	r0	; OVERLOADED WITH INPUT #1
op1m1	.set	r3	; OVERLOADED WITH INPUT #2
op1m2	.set	r2	; OVERLOADED WITH INPUT #2

	.endif

tmp	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7

	.if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
	.thumbfunc __TI_FD$SUB, __TI_FD$ADD
	.endif
	
__TI_FD$SUB:	.asmfunc stack_usage(24)

	PUSH	{r2-r7}			;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	EOR	rp2_hi, tmp		; NEGATE INPUT #2
	B	ct			;

__TI_FD$ADD:	PUSH	{r2-r7}			;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;

ct:	PUSH	{rp2_hi}		; SAVE MODIFIED INPUT #2

	LSL	op2m1, rp2_hi, #12	; BUILD INPUT #2 MANTISSA
	LSR	op2m1, op2m1, #3	;
	LSR	op2m2, rp2_lo, #23	;
	ORR	op2m1, op2m2		;
	LSL	op2m2, rp2_lo, #9	;
	LSL	op2e, rp2_hi, #1	; BUILD INPUT #2 EXPONENT
	LSR	op2e, op2e, #21		;
	BNE	$1			;

	MOV	tmp, op2m1		; IF DENORMALIZED NUMBER (op2m != 0 AND
        ORR     tmp, op2m2		; op2e == 0), THEN UNDERFLOW
	BNE	unfl			;
	ADD	sp, #4			;
        POP	{r2-r7}		        ; ELSE IT IS ZERO SO RETURN INPUT #1
	BX	lr			;

$1:	LSR	tmp, tmp, #2		; SET IMPLIED ONE IN MANTISSA
	ORR	op2m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op2e, tmp		; IF op2e == 0x7FF, THEN OVERFLOW
	BNE	$2			;

	MOV	rp1_lo, #0x0		;
	LSR	rp1_hi, rp2_hi, #20	; PICK UP SIGN AND EXP FROM R2
	LSL	rp1_hi, rp1_hi, #20
	ADD	sp, #4			;
        POP     {r2-r7}		        ;
	BX	lr			;

$2:     CMP     rp2_hi, #0              ;
	BPL	$3			; IF INPUT #2 IS NEGATIVE,
	MOV	tmp, #0			;  THEN NEGATE THE MANTISSA
        SUB     op2m2, tmp, op2m2	;
        SBC     tmp, op2m1	        ;
	MOV	op2m1, tmp		;

$3:	LSL	op1m1, rp1_hi, #12	; BUILD INPUT #1 MANTISSA
	LSR	op1m1, op1m1, #3	;
	LSR	op1m2, rp1_lo, #23	;
	ORR	op1m1, op1m2		;
	LSL	op1m2, rp1_lo, #9	;
	LSL	op1e, rp1_hi, #1	; BUILD INPUT #1 EXPONENT
	LSR	op1e, op1e, #21		;
	BNE	$4			;

	MOV	tmp, op1m1		; IF DENORMALIZED NUMBER (op1m != 0 AND
        ORR     tmp, op1m2		; op1e == 0), THEN UNDERFLOW
	BNE	unfl			;
	POP	{rp1_hi, rp2_hi, rp2_lo}		; ELSE IT IS ZERO SO RETURN INPUT #2
	MOV 	rp1_lo, rp2_lo		;
        POP     {r4-r7}  	        ;
	BX	lr			;

$4:	MOV	tmp, #0x1		; SET IMPLIED ONE IN MANTISSA
	LSL	tmp, tmp, #29		;
	ORR	op1m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op1e, tmp		; IF op1e == 0x7FF, THEN OVERFLOW
	BNE	$5

	MOV	rp1_lo, #0x0		;
	LSR	rp1_hi, rp1_hi, #20	; ISOLATE SIGN AND EXP IN R0
	LSL	rp1_hi, rp1_hi, #20
	ADD	sp, #4			;
        POP	{r2-r7}		        ;
	BX	lr			;

$5:     CMP     rp1_hi, #0              ;
	BPL	$6			; IF INPUT #1 IS NEGATIVE,
	MOV	tmp, #0			;  THEN NEGATE THE MANTISSA
	SUB	op1m2, tmp, op1m2	;
        SBC     tmp, op1m1	        ;
	MOV	op1m1, tmp		;

$6:     SUB     shift, op1e, op2e       ; GET THE SHIFT AMOUNT
	BPL	norm			;
        MOV     tmp, op1m1              ; IF THE SHIFT AMOUNT IS NEGATIVE, THEN
        MOV     op1m1, op2m1            ;  SWAP THE TWO MANTISSA SO THAT op1m
	MOV  	op2m1, tmp		;  CONTAINS THE LARGER VALUE,
	MOV	tmp, op1m2		;
	MOV	op1m2, op2m2		;
	MOV	op2m2, tmp		;
	MOV	tmp, #0			;
        SUB     shift, tmp, shift       ;  AND NEGATE THE SHIFT AMOUNT,
        MOV     op1e, op2e              ;  AND ENSURE THE LARGER EXP. IS IN op1e

norm:   CMP     shift, #54              ; IF THE SECOND MANTISSA IS SIGNIFICANT,
	BPL	no_add			;

sticky .set op2e                        ; USE op2e TO HOLD STICKY BIT
        PUSH    {shift}                 ; NEED A REGISTER 
        SUB     shift, #57              ; DETERMINE NUMBER OF BITS TO LEFT SHIFT
        RSB     shift, shift, #0        ;
        MOV     tmp, shift              ; PERFORM LONG LONG LSL BY shift
        SUB     tmp, #32                ;
        BCS     $s_1                    ; IF tmp IS POSITIVE
        MOV     sticky, op2m1           ; tmp IS NEGATIVE
        LSL     sticky, shift           ; SHIFT HI MANTISSA LEFT
        ORR     sticky, op2m2           ; OR IN ALL OF LO MANTISSA
        B       $s_2
$s_1:   MOV     sticky, op2m2           ; ONLY NEED LO MANTISSA
        LSL     sticky, tmp             
$s_2:   CMP     sticky, #0              ; IF STICKY IS SET
        BEQ     $s_3
        MOV     shift, #0x40            ; STORE STICKY BIT IN OP2M2
        ORR     op2m2, shift            ; THIS SAVES A REGISTER FROM BEGIN USED
$s_3    POP     {shift}                 ; RESTORE SHIFT

	CMP	shift, #32		; figure out if shift is "big" per algorithm 
	BPL	sl_lit
	
	CMP	shift, #0		; ADJUST THE SECOND MANTISSA, BASED
	BEQ	no_sft			; UPON ITS EXPONENT.

        
	NEG	tmp, shift
	ADD	tmp, #32		; tmp := 32 - shift
	PUSH	{op2m1}
	LSL	op2m1, tmp		; set tmp to op2m1 shifted left by _shift_ places
	MOV	tmp, op2m1
	POP	{op2m1}
	ASR	op2m1, shift
	LSR	op2m2, shift
	ADD	op2m2, tmp		; op2m2 is zero everywhere tmp isn't and vice versa
	B	no_sft

sl_lit:	CMP	shift, #0		; reset condition codes
	MOV	op2m2, op2m1
	ASR	op2m1, shift 
	SUB	shift, #32
	ASR	op2m2, shift
	
no_sft:	ADD	op1m2, op1m2, op2m2	; ADD IT TO THE FIRST MANTISSA
	ADC	op1m1, op2m1		;

no_add: MOV	tmp, op1m1		;
	ORR     tmp, op1m2		;
	BEQ	unfl			;

        MOV	tmp, #0x0               ;
	CMP	op1m1, #0		; IF THE RESULT IS POSITIVE, NOTE SIGN
	BPL	nloop			;
        MOV     tmp, #0x1               ; IF THE RESULT IS NEGATIVE, THEN
	MOV	shift, #0		;  NOTE THE SIGN AND
        SUB	op1m2, shift, op1m2     ;  NEGATE THE RESULT
	SBC	shift, op1m1		;
	MOV	op1m1, shift		;
 
nloop:  SUB     op1e, op1e, #1          ; NORMALIZE THE RESULTING MANTISSA
	LSL     op1m2, op1m2, #1 	; ADJUSTING THE EXPONENT AS NECESSARY
	ADC	op1m1, op1m1		; 
        BPL     nloop                   ;
 
	MOV	shift, #0x1		;
	LSL	shift, shift, #10	;
        MOV     op2m1, op1m2            ; COPY LOWER MANTISSA 
        AND     op2m1, shift            ; DETERMINE GUARD BIT
        BEQ     no_round                ; IF GUARD BIT 0, DO NOT ROUND
        ADD     op1m2, op1m2, shift     ; ROUND THE MANTISSA TO THE NEAREST
	MOV	op2e, #0x0		;
	ADC	op1m1, op2e		;
	BCC	$7			;
        ADD     op1e, op1e, #1          ; ADJUST EXPONENT IF AN OVERFLOW OCCURS
	B	$8			; IF OVERFLOW, RESULT IS ALREADY EVEN
$7:     
        MOV     shift, #7               ; IF NORMALIZATION OCCURRED, STICKY
        LSL     shift, shift, #7        ;  MAY HAVE BEEN SHIFTED RIGHT BY ONE
                                        ;  GET EXTRA BIT IN CASE  
        MOV     op2m1, op1m2            ; COPY LOWER MANTISSA
        AND     op2m1, shift            ; DETERMINE (ROUND + STICKY)
        BNE     no_round                ; IF (ROUND + STICKY) == 1, SKIP
        LSL     shift, shift, #2        ; PREPARE TO CLEAR LAST BIT
        BIC     op1m2, shift            ; WE HAVE A TIE, MAKE RESULT EVEN

no_round:
        MOV     shift, #0x7
        LSL     shift, shift, #8
        BIC     op1m2, shift            ; CLEAR GUARD, ROUND, AND STICKY BITS
        LSL     op1m2, op1m2, #1 	; REMOVE THE IMPLIED ONE
	ADC	op1m1, op1m1		;
 
$8:     ADD     op1e, op1e, #2          ; NORMALIZE THE EXPONENT
	BLE	unfl			; CHECK FOR UNDERFLOW
	MOV	shift, #0x7		;
	LSL	shift, shift, #8	;
	ADD	shift, #0xFF		;
        CMP     op1e, shift             ; CHECK FOR OVERFLOW
	BCC	$9			;

	POP	{rp1_hi, rp2_hi, rp2_lo}; OVERFLOW
	MOV	rp1_lo, #0x0		;
	LSR	rp1_hi, rp1_hi, #31
	LSL	rp1_hi, rp1_hi, #8
	ADD	rp1_hi, #0xFF
	LSL	rp1_hi, rp1_hi, #3
	ADD	rp1_hi, rp1_hi, #7
	LSL	rp1_hi, rp1_hi, #20
        POP     {r4-r7}	                ;
	BX	lr			;
 
$9:	LSR	rp1_hi, op1m1, #12	; REPACK THE MANTISSA INTO rp1_hi:rp1_lo
	LSL	op1e, op1e, #20		;
        ORR     rp1_hi, op1e		; REPACK THE EXPONENT INTO rp1_hi
	LSL	tmp, tmp, #31		;
        ORR     rp1_hi, tmp 		; REPACK THE SIGN INTO rp1_hi
        LSL	rp1_lo, op1m1, #20	;
	LSR	op1m2, op1m2, #12	;
	ORR	rp1_lo, op1m2		;

	ADD	sp, #4			;
        POP	{r2-r7}	        	;
	BX	lr			;

unfl:   MOV     rp1_hi, #0              ; UNDERFLOW, SO RETURN ZERO
	MOV	rp1_lo, #0		;
	ADD	sp, #4			;
        POP     {r2-r7}			;
	BX	lr			;

	.endasmfunc

	.end
