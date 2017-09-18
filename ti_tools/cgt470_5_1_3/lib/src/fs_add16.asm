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

;*****************************************************************************
;* FS$ADD/FS$SUB - ADD/SUBTRACT TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	           POINT NUMBERS.
;*****************************************************************************
;*
;*   o INPUT OP1 IS IN r0
;*   o INPUT OP2 IS IN r1
;*   o RESULT IS RETURNED IN r0
;*   o FOR SUBTRACTION, INPUT OP2 IN r1 IS KILLED 
;*
;*   o SUBTRACTION, OP1 - OP2, IS IMPLEMENTED WITH ADDITION, OP1 + (-OP2)
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7f800000/ff800000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
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

        .if __TI_EABI_ASSEMBLER            ; ASSIGN EXTERNAL NAMES BASED ON
        .asg __aeabi_fadd, __TI_FS$ADD     ; RTS BEING BUILT
        .asg __aeabi_fsub, __TI_FS$SUB
        .else
	.clink
        .asg FS$ADD, __TI_FS$ADD
        .asg FS$SUB, __TI_FS$SUB
        .endif

	.global __TI_FS$ADD
	.global __TI_FS$SUB


m0	.set	r2
e0	.set	r3
m1	.set	r4
e1	.set	r5
shift	.set	r6
tmp	.set	r7

	.if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
	.thumbfunc __TI_FS$ADD
	.thumbfunc __TI_FS$SUB
	.endif

__TI_FS$SUB:	.asmfunc stack_usage(24)
	PUSH	{r2-r7}  		;
	MOV	tmp, #1			;
	LSL	tmp, tmp, #31		;
	EOR	r1, tmp			; NEGATE INPUT #2
	B	$1			;

__TI_FS$ADD:	PUSH	{r2-r7}	        ;
	MOV	tmp, #1			;
	LSL	tmp, tmp, #31		;

$1:	LSL	m1, r1, #8		; PUT INPUT #2 MANTISSA IN m1
	LSL	e1, r1, #1		; PUT INPUT #2 EXPONENT IN e1
	LSR	e1, e1, #24		;
	BNE	$2			;

	CMP	m1, #0			; IF DENORMALIZED NUMBER (m0 != 0 AND
	BNE	unfl			; e1 == 0), THEN UNDERFLOW
	POP	{r2-r7} 		; ELSE IT IS ZERO SO RETURN INPUT #1
	BX	lr			;

$2:	ORR	m1, tmp			; SET IMPLIED ONE IN MANTISSA

	CMP	e1, #0xFF		; IF e1 == 0xFF, THEN OVERFLOW
	BEQ	ovfl1			;

	LSR	m1, m1, #2		; ADJUST THE MANTISSA
	CMP	r1, #0			; IF INPUT #2 IS NEGATIVE,
	BPL	$3			;
	NEG	m1, m1 			;  THEN NEGATE THE MANTISSA

$3:	LSL	m0, r0, #8		; PUT INPUT #1 MANTISSA IN m0
	LSL	e0, r0, #1		; PUT INPUT #1 EXPONENT IN e0
	LSR	e0, e0, #24		;
	BNE	$4			;

	CMP	m0, #0			; IF A DENORMALIZED NUMBER 
	BNE	unfl			; (m0 != 0 AND e0 == 0), THEN UNDERFLOW
	MOV	r0, r1			;  ELSE IT IS ZERO SO RETURN INPUT #2
	POP	{r2-r7} 		; 
	BX	lr			;

$4:	ORR	m0, tmp			; SET IMPLIED ONE IN MANTISSA

	CMP	e0, #0xFF		; IF e0 == 0xFF, THEN OVERFLOW
	BEQ	ovfl0			;

	LSR	m0, m0, #2		; ADJUST THE MANTISSA
	CMP	r0, #0			; IF INPUT #1 IS NEGATIVE,
	BPL	$5			;
	NEG	m0, m0			;  THEN NEGATE THE MANTISSA
	

$5:	SUB	shift, e0, e1		; GET THE SHIFT AMOUNT
	BPL	$6			;
	MOV	tmp, m0			; IF THE SHIFT AMOUNT IS NEGATIVE, THEN
	MOV	m0, m1			;  SWAP THE TWO MANTISSA SO THAT m0
	MOV	m1, tmp			;  CONTAINS THE LARGER VALUE,
	NEG	shift, shift		;  AND NEGATE THE SHIFT AMOUNT,
	MOV	e0, e1			;  AND ENSURE THE LARGER EXP. IS IN e0

$6:	CMP	shift, #25		; IF THE SECOND MANTISSA IS SIGNIFICANT,
	BPL	$7			;
        MOV     tmp, m1
	ASR	tmp, shift		;  ADD IT TO THE FIRST MANTISSA
	ADD	m0, tmp			;

sticky  .set    m1                      ; USE M1 TO HOLD THE STICKY BIT
        MOV     tmp, #28                ; CALCULATE STICKY BIT
        SUB     tmp, shift, tmp         ;
        RSB     tmp, tmp, #0            ;
        LSL     sticky, tmp             ; GET THE BITS THAT WERE SHIFTED OUT

$7:	CMP	m0, #0x0		; IF THE RESULT IS ZERO, 
	BEQ	unfl 			;  THEN UNDERFLOW
	BPL	$8			;
	NEG	m0, m0			; IF THE RESULT IS NEGATIVE, THEN
	MOV	tmp, #0x1		;  NEGATE THE RESULT AND
	B	loop			;

$8:	MOV	tmp, #0x0		; NOTE THE SIGN

loop:	SUB	e0, #1			; NORMALIZE THE RESULTING MANTISSA
	LSL	m0, m0, #1		; ADJUSTING THE EXPONENT AS NECESSARY
	BPL	loop			;

        MOV     e1, m0                  ; COPY MANTISSA
        MOV     shift, #0x80            ; 
        AND     e1, shift               ; CHECK IF GUARD BIT IS SET
        BEQ     $9                      ; IF GUARD BIT == 0, DO NOT ROUND
        MOV     e1, m0                  ; COPY MANTISSA
        MOV     shift, #0x20            ;
        AND     e1, shift               ; IF RESULT REQUIRED NORMALIZATION
        ORR     sticky, e1              ;  BIT 26 MUST BE ADDED TO STICKY
	ADD	m0, #0x80		; ROUND THE MANTISSA TO THE NEAREST
	BCS	$9a			; OVERFLOW
        MOV     e1, m0                  ; COPY MANTISSA
        MOV     shift, #0x40            ;
        AND     e1, shift               ; GET ROUND BIT
        ORR     sticky, e1              ; (ROUND + STICKY)
        BNE     $9                      ; IF (ROUND + STICKY)==1, M0 IS CORRECT
        MOV     shift, #0x1             ; IF (ROUND + STICKY)==0, 
        LSL     shift, shift, #8        ;  M0 MUST BE EVEN
        BIC     m0, shift               ; CLEAR LAST BIT
        B       $9

$9a:   	ADD	e0, e0, #1		; ADJUST EXPONENT IF AN OVERFLOW OCCURS

$9:	LSL	m0, m0, #1		; REMOVE THE IMPLIED ONE
        ADD	e0, #2			; NORMALIZE THE EXPONENT        
	BLE	unfl			; CHECK FOR UNDERFLOW
	CMP	e0, #0xFF		;
	BCS	ovfl			; CHECK FOR OVERFLOW

	LSR	r0, m0, #9		; REPACK THE MANTISSA INTO r0
	LSL	e0, e0, #23		;
	ORR	r0, e0			; REPACK THE EXPONENT INTO r0
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; REPACK THE SIGN INTO r0

	POP	{r2-r7} 		;
	BX	lr			;
	
unfl:	MOV	r0, #0			; UNDERFLOW
	POP	{r2-r7}		        ;
	BX	lr			;

					; OVERFLOW
ovfl1:  MOV	r0, r1			; SIGN BIT in R1
ovfl0:	LSR	tmp, r0, #31 		; ISOLATE SIGN BIT
ovfl:	LSL	tmp, tmp, #31		; 
	MOV	r0, #0ffh		; EXPONENT = Emax = 255
	LSL	r0, r0, #23		; SHIFT EXPONENT INTO PLACE
	ORR	r0, tmp			; COMBINE WITH SIGN BIT
	POP	{r2-r7} 		;
	BX	lr			;

	.endasmfunc

	.end
