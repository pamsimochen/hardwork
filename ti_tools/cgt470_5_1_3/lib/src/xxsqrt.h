/****************************************************************************/
/*  xxsqrt.h           v5.1.3                                               */
/*                                                                          */
/* Copyright (c) 2010-2013 Texas Instruments Incorporated                   */
/* http://www.ti.com/                                                       */
/*                                                                          */
/*  Redistribution and  use in source  and binary forms, with  or without   */
/*  modification,  are permitted provided  that the  following conditions   */
/*  are met:                                                                */
/*                                                                          */
/*     Redistributions  of source  code must  retain the  above copyright   */
/*     notice, this list of conditions and the following disclaimer.        */
/*                                                                          */
/*     Redistributions in binary form  must reproduce the above copyright   */
/*     notice, this  list of conditions  and the following  disclaimer in   */
/*     the  documentation  and/or   other  materials  provided  with  the   */
/*     distribution.                                                        */
/*                                                                          */
/*     Neither the  name of Texas Instruments Incorporated  nor the names   */
/*     of its  contributors may  be used to  endorse or  promote products   */
/*     derived  from   this  software  without   specific  prior  written   */
/*     permission.                                                          */
/*                                                                          */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS   */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT   */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT   */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT   */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT   */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    */
/*                                                                          */
/****************************************************************************/
/* xxsqrt.h -- common sqrt[fl] functionality */
#include "xmath.h"
_STD_BEGIN

FTYPE (FFUN(sqrt))(FTYPE x)
	{	/* compute x^(1/2) */
#if !(defined(__TI_COMPILER_VERSION__) && HAS_BF_SQRT)
	short xexp;

	switch (FNAME(Dunscale)(&xexp, &x))
		{	/* test for special codes */
	case _NANCODE:
	case 0:
		return (x);
	case _INFCODE:
		if (!FISNEG(x))
			return (x);	/* INF */
	default:	/* -INF or finite */
		if (FISNEG(x))
			{	/* sqrt undefined for reals */
			_Feraise(_FE_INVALID);
			return (FCONST(Nan));
			}
	                {
	                FTYPE y;
			if ((unsigned int)xexp & 1)
				x *= FLIT(2.0), --xexp;
			y = (FLIT(-0.09977) * x + FLIT(0.71035)) * x
				+ FLIT(0.38660);	/* 6 bits */
			y += x / y;
			y = FLIT(0.25) * y + x / y;	/* 27 bits */

 #if   FBITS <= 27

 #elif FBITS <= 56
			y = FLIT(0.5) * (y + x / y);	/* 56 bits */

 #elif FBITS <= 113
			y += x / y;
			y = FLIT(0.25) * y + x / y;	/* 113 bits */

 #else /* FBITS */
  #error sqrt has insufficient precision
 #endif /* FBITS */

			FNAME(Dscale)(&y, xexp / 2);
			return (y);
                        }
                }
#else /* !(defined(__TI_COMPILER_VERSION__) && HAS_BF_SQRT) */
        if (x < FLIT(0.0))
        {
            _Feraise(_FE_INVALID);
            return FCONST(Nan);
        }
	
	/* Call builtin sqrt function. x can be real, +inf, or NaN */
        return TYPED_SQRT(x);

#endif /* !(defined(__TI_COMPILER_VERSION__) && HAS_BF_SQRT) */

	}
_STD_END

/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V4.02:1476 */
