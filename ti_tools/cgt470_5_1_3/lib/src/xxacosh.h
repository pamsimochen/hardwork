/* xxacosh.h -- common acosh[fl] functionality */
#include "xmath.h"
_STD_BEGIN

FTYPE FFUN(log1p)(FTYPE);
#if defined(__TI_COMPILER_VERSION__)
static const _DATA_ACCESS FTYPE ln2 = FLIT(0.69314718055994530941723212145817658);
#else
static const FTYPE ln2 = FLIT(0.69314718055994530941723212145817658);
#endif /* defined(__TI_COMPILER_VERSION__) */
FTYPE (FFUN(acosh))(FTYPE x)
	{	/* compute acosh(x) */
	switch (FNAME(Dtest)(&x))
		{	/* test for special codes */
	case _NANCODE:
		return (x);
	case _INFCODE:
		if (!FISNEG(x))
			return (x);
	case 0:
	default:	/* -INF or finite */
		if (x < FLIT(1.0))
			{	/* defined only for 1 <= x */
			_Feraise(_FE_DIVBYZERO);
			return (FCONST(Nan));
			}
		else if (x == FLIT(1.0))
			return (FLIT(0.0));
		else if (x < FLIT(2.0) / FCONST(Eps))
			{	/* not big, compute carefully */
			FTYPE z = x - FLIT(1.0);

			return (FFUN(log1p)(z
				+ FFUN(sqrt)(z * (x + FLIT(1.0)))));
			}
		else
			return (FFUN(log)(x) + ln2);	/* x big, compute log(x+x) */
		}
	}
_STD_END

/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V4.02:1476 */
