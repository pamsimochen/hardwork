;******************************************************************************
;* FD_TOS16.ASM  - 16 BIT STATE -  v5.1.3                                     *
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
;* FD$TOFS - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	     POINT NUMBER TO 754 FORMAT SINGLE PRECISION FLOATING 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0
;*   o r1 IS DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7F800000/FF800000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;****************************************************************************
;*
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
;*
;* +--------------------------------------------------------------+
;* | SINGLE PRECISION FLOATING POINT FORMAT                       |
;* |   32-bit representation                                      |
;* |   31 30    23 22                    0                        |
;* |   +-+--------+-----------------------+                       |
;* |   |S|    E   |           M           +                       |
;* |   +-+--------+-----------------------+                       |
;* |                                                              |
;* |   <S>  SIGN FIELD    :        0 - POSITIVE VALUE             |
;* |                               1 - NEGATIVE VALUE             |
;* |                                                              |
;* |   <E>  EXPONENT FIELD:       00 - ZERO IFF M == 0            |
;* |                         01...FE - EXPONENT VALUE (127 BIAS)  |
;* |                              FF - INFINITY                   |
;* |                                                              |
;* |   <M>  MANTISSA FIELD:  FRACTIONAL MAGNITUDE WITH IMPLIED 1  |
;* +--------------------------------------------------------------+
;*
;****************************************************************************

	.state16

        .if __TI_EABI_ASSEMBLER            ; ASSIGN EXTERNAL NAMES BASED ON
        .asg __aeabi_d2f, __TI_FD$TOFS     ; RTS BEING BUILT
        .else
	.clink
        .asg FD$TOFS, __TI_FD$TOFS
        .endif

	.global	__TI_FD$TOFS

	.if .TMS470_BIG_DOUBLE
rp1_hi	.set	r0	; High word of regpair 1
rp1_lo	.set	r1	; Low word of regpair 1
	.else
rp1_hi	.set	r1	; High word of regpair 1
rp1_lo	.set	r0	; Low word of regpair 1
	.endif

tmp	.set	r2

	.if __TI_ARM9ABI_ASSEMBLER  | __TI_EABI_ASSEMBLER
	.thumbfunc __TI_FD$TOFS
	.endif

__TI_FD$TOFS: .asmfunc stack_usage(4)
	PUSH	{r2}			; SAVE CONTEXT

	LSR	rp1_lo, rp1_lo, #20	; SETUP MANTISSA INTO rp1_lo
	LSL	tmp, rp1_hi, #12	;
	ORR	rp1_lo, tmp		;

	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #8		;
	ADD	rp1_lo, rp1_lo, tmp	; ADD 1/2 FOR ROUNDING
	BCC	$1			;
	LSR	rp1_lo, rp1_lo, #1	; IF OVERFLOW, ADJUST MANTISSA AND
	LSL	tmp, tmp, #12		; EXPONENT
	ADD	rp1_hi, rp1_hi, tmp	;

$1:	LSR	rp1_lo, rp1_lo, #9	; ALIGN MANTISSA
	CMP	rp1_hi, #0		; AND ADD SIGN
	BPL	$2			;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	ORR	rp1_lo, tmp		;

$2:	LSL	rp1_hi, rp1_hi, #1	; SETUP EXPONENT
	LSR	rp1_hi, rp1_hi, #21	;

	MOV	tmp, #0x38		;
	LSL	tmp, tmp, #4		;
	SUB	rp1_hi, rp1_hi, tmp	; ADJUST FOR THE BIAS
	BMI	unfl			; CHECK FOR UNDERFLOW / ZERO

	CMP	rp1_hi, #0xFF		; CHECK FOR OVERFLOW
	BCS	ovfl			;

	LSL	rp1_hi, rp1_hi, #23	;
	ORR	r0, r1			; ADD EXPONENT INTO RESULT
	POP	{r2}			;
	BX	lr			;
	
unfl:	MOV	r0, #0
	POP	{r2}			; IF UNDERFLOW, RETURN 0
	BX	lr			;

ovfl:	LSR	r1, rp1_hi, #31		; OVERFLOW, RETURN +/- INF
	LSL	r1, r1, #31
	MOV	r0, #0xFF
	LSL	r0, r0, #23
	ORR	r0, r1			;
	POP	{r2}			;
	BX	lr			;

	.endasmfunc

	.end
