;******************************************************************************
;* FD_DIV16.ASM  - 16 BIT STATE -  v5.1.3                                     *
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
;* FD$DIV - DIVIDES TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	    POINT NUMBERS.
;****************************************************************************
;*
;*   o DIVIDEND INPUT OP1 IS IN r0:r1
;*   o DIVISOR INPUT OP2 IS IN r2:r3
;*   o QUOTIENT IS RETURNED IN r0:r1
;*   o INPUT OP2 IN r2:r3 IS PRESERVED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*   o DIVIDE BY ZERO RETURNS ZERO
;*
;*   o IF THE OPERATION INVOLVES INFINITY AS AN INPUT, UNLESS THE OTHER INPUT
;*     IS ZERO, THE RESULT IS INFINITY WITH THE SIGN DETERMINED IN THE USUAL
;*     FASHION.
;*
;****************************************************************************
;*
;* +--------------------------------------------------------------+
;* | DOUBLE PRECISION FLOATING POINT FORMAT                       |
;* |   64-bit representation                                      |
;* |   31 30      20 19                  0                        |
;* |   +-+----------+---------------------+                       |
;* |   |S|     E    |        M1           |                       |
;* |   +-+----------+---------------------+                       |
;* |                                                              |
;* |   31                                0                        |
;* |   +----------------------------------+                       |
;* |   |             M2                   |                       |
;* |   +----------------------------------+                       |
;* |                                                              |
;* |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE           |
;* |                                 1 - NEGATIVE VALUE           |
;* |                                                              |
;* |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0          |
;* |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)|
;* |                        1111111111 - INFINITY                 |
;* |                                                              |
;* |   <M1:M2>  MANTISSA FIELDS:                                  |
;* |                          FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
;* +--------------------------------------------------------------+
;*
;****************************************************************************

	.state16

        .if __TI_EABI_ASSEMBLER            ; ASSIGN EXTERNAL NAMES BASED ON
        .asg __aeabi_ddiv, __TI_FD$DIV     ; RTS BEING BUILT
        .else
	.clink
        .asg FD$DIV, __TI_FD$DIV
        .endif

	.global	__TI_FD$DIV

	.if .TMS470_BIG_DOUBLE

rp1_hi	.set	r0	; High word of regpair 1
rp1_lo	.set	r1	; Low word of regpair 1
rp2_hi	.set	r2	; High word of regpair 2
rp2_lo	.set	r3	; Low word of regpair 2
res1	.set	r0	; OVERLOADED WITH INPUT #1
op1e	.set	r1	; OVERLOADED WITH INPUT #1
op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2

	.else

rp1_hi	.set	r1	; High word of regpair 1
rp1_lo	.set	r0	; Low word of regpair 1
rp2_hi	.set	r3	; High word of regpair 2
rp2_lo	.set	r2	; Low word of regpair 2
res1	.set	r1	; OVERLOADED WITH INPUT #1
op1e	.set	r0	; OVERLOADED WITH INPUT #1
op1m1	.set	r3	; OVERLOADED WITH INPUT #2
op1m2	.set	r2	; OVERLOADED WITH INPUT #2

	.endif

tmp	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7
res2	.set	r7	; OVERLOADED WITH op2e


	.if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
	.thumbfunc __TI_FD$DIV
	.endif

__TI_FD$DIV:	.asmfunc	stack_usage(28)
	PUSH	{r2-r7}			;

	MOV	tmp, rp1_hi		; SET THE SIGN OF THE RESULT
        EOR     tmp, rp2_hi             ;
	LSR	tmp, tmp, #31		;
	LSL	tmp, tmp, #31		;
	PUSH	{tmp}			;

	LSL	op2m1, rp2_hi, #12	; BUILD INPUT #2 MANTISSA
	LSR	op2m1, op2m1, #12	;
	LSL	op2m2, rp2_lo, #1	;
	ADC	op2m1, op2m1		;
	LSL	op2e, rp2_hi, #1	; BUILD INPUT #2 EXPONENT
	LSR	op2e, op2e, #21		;
	BEQ	unfl			;

	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #21		; SET IMPLIED ONE IN MANTISSA
	ORR	op2m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op2e, tmp		; IF op2e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

	LSL	op1m1, rp1_hi, #12	; BUILD INPUT #1 MANTISSA
	LSR	op1m1, op1m1, #12	;
	LSL	op1m2, rp1_lo, #1	;
	ADC	op1m1, op1m1		;
	LSL	op1e, rp1_hi, #1	; BUILD INPUT #1 EXPONENT
	LSR	op1e, op1e, #21		;
	BEQ	unfl			;

	MOV	tmp, #0x1		; SET IMPLIED ONE IN MANTISSA
	LSL	tmp, tmp, #21		;
	ORR	op1m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op1e, tmp		; IF op1e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

        SUB     op1e, op1e, op2e        ; SUBTRACT EXPONENTS
 
        ; DIVIDE THE MANTISAE:  op1m / op2m => res1:res2
        MOV     res1, #0x0              ; INITIALIZE THE QUOTIENT
	MOV	res2, #0x0		;
        MOV     tmp, #54                ; INITIALIZE THE SHIFT COUNTER
 
fdivb:  CMP     op1m1, op2m1            ; IF DIVIDEND IS LARGER THAN DIVISOR,
	BNE	$1			;
	CMP	op1m2, op2m2		;
$1:	BCC	$2			;
        ADC     res2, res2              ;  SHIFT A 1 INTO THE QUOTIENT
	ADC	res1, res1		;
	SUB	op1m2, op1m2, op2m2	;  AND SUBTRACT THE DIVISOR FROM
	SBC	op1m1, op2m1		;  THE DIVIDEND
	B	$3			;
$2:	ADC     res2, res2		; ELSE, SHIFT A 0 INTO THE QUOTIENT
	ADC	res1, res1		;
$3:	SUB	tmp, tmp, #1		; UPDATE THE SHIFT COUNTER
	BEQ	fdive			; EXIT IF OUT OF SHIFTS
	
	CMP	op1m1, #0		; EXIT IF NO DIVIDEND LEFT
	BNE	$4			;
	CMP	op1m2, #0		;
	BEQ	fdive			;
$4:	LSL	op1m2, op1m2, #1	; IF DIVIDEND STILL THERE,
	ADC	op1m1, op1m1		;   UPDATE DIVIDEND AND
	B	fdivb			;   CONTINUE
 
fdive:  CMP	tmp, #32		; ADJUST THE QUOTIENT AS NECESSARY
	BCS	low			;
	CMP	tmp, #22		;
	BCC	high			;

mid:	MOV	res1, res2		;
	LSL	res2, tmp		;
	MOV	op2m2, #32		;
	SUB	op2m2, op2m2, tmp	;
	LSR	res1, op2m2		;
	B	cont			;

low:	SUB	tmp, #32		;
	MOV	res1, res2		;
	LSL	res1, tmp		;
	MOV	res2, #0		;
	B	cont			;

high:	LSL	res1, tmp		;
	MOV	op2m2, #32		;
	SUB	op2m2, op2m2, tmp	;
	MOV	op2m1, res2		;
	LSR	op2m1, op2m2		;
	ORR	res1, op2m1		;
	LSL	res2, tmp		;

cont:   MOV	tmp, #0x1		;
	LSL	tmp, tmp, #21		;
	TST     res1, tmp               ; ALIGN THE QUOTIENT
	BNE	$5			;
	LSL	res2, res2, #1		;
	ADC	res1, res1		;
        SUB     op1e, op1e, #0x1        ;
 
$5:     MOV	tmp, #0x0		;
	ADD     res2, res2, #0x00000001 ; 1/2 ADJUST FOR ROUNDING
	ADC	res1, tmp		;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #22		;
	TST	res1, tmp		;
	BEQ	$6			;
        ADD     op1e, op1e, #0x1        ;
	BIC	res1, tmp		;
	B	$7			;
$6:	LSR	tmp, tmp, #1		;
	BIC	res1, tmp		;

$7:	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	ADD     op1e, op1e, tmp         ; ADJUST FOR BIAS
	BLE	unfl			; AND CHECK FOR UNDERFLOW

	ADD	tmp, tmp, tmp		;
        CMP     op1e, tmp	        ; AND CHECK FOR EXPONENT OVERFLOW 
	BHI	ovfl			;

	LSL	tmp, res1, #31		;
	LSR	res1, res1, #1		; REPACK THE QUOTIENT INTO rp1_hi:rp1_lo
	LSL	op1e, op1e, #20		;
        ORR     res1, op1e              ; REPACK THE EXPONENT INTO rp1_hi
	LSR	rp1_lo, res2, #1	;
	ORR	rp1_lo, tmp		;
	POP	{tmp}			;
        ORR     res1, tmp               ; REPACK THE SIGN INTO rp1_hi
 
	POP	{r2-r7}		        ;
	BX	lr			;

ovfl:	POP	{rp1_hi}		; POP BACK THE SIGN.             
	MOV	rp1_lo, #0x0		; RETURN OVERFLOW VALUE, WHICH IS
	LSR	rp1_hi, rp1_hi, #31	; BASED ON THE SIGN OF THE RESULT
	LSL	rp1_hi, rp1_hi, #8
	MOV	rp1_hi, #0xFF
	LSL	rp1_hi, rp1_hi, #3
	ADD	rp1_hi, rp1_hi, #7
	LSL	rp1_hi, rp1_hi, #20	; 
        POP     {r2-r7}		        ;
	BX	lr			;

unfl:   MOV     rp1_hi, #0              ; UNDERFLOW, SO RETURN ZERO
	MOV	rp1_lo, #0		;
	ADD	sp, #4			;
        POP     {r2-r7}			;
	BX	lr			;

	.endasmfunc

	.end
