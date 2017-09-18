/* xxacos.h -- common acos[fl] functionality */
#include "xmath.h"
_STD_BEGIN

FTYPE (FFUN(acos))(FTYPE x)
	{	/* compute acos(x) */
#if defined(__TI_COMPILER_VERSION__)
	static const _DATA_ACCESS FTYPE rthalf = FLIT(0.70710678118654752440084436210484905);
#else
	static const FTYPE rthalf = FLIT(0.70710678118654752440084436210484905);
#endif /* defined(__TI_COMPILER_VERSION__) */
	unsigned short hex;

	switch (FNAME(Dtest)(&x))
		{	/* test for special codes */
	case _NANCODE:
		return (x);
	default:	/* 0, FINITE, or INF */
		if (x < FLIT(0.0))
			x = -x, hex = 0x4;
		else
			hex = 0x2;
		if (x <= rthalf)
			return (FNAME(Atan)(x / FFUN(sqrt)((FLIT(1.0) - x)
				* (FLIT(1.0) + x)), hex));
		else if (x <= FLIT(1.0))
			return (FNAME(Atan)(FFUN(sqrt)((FLIT(1.0) - x)
				* (FLIT(1.0) + x)) / x, hex ^ 0x2));
		else
			{	/* 1 < |x| */
			_Feraise(_FE_INVALID);
			return (FCONST(Nan));
			}
		}
	}
_STD_END

/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V4.02:1476 */
