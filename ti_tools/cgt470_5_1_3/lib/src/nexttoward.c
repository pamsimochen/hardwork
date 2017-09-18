/* nexttoward function -- IEEE version */
#include "xmath.h"
_STD_BEGIN

double (nexttoward)(double x, long double y)
	{	/* compute next value after x going toward y */
	unsigned short *const px = (unsigned short *)(char *)&x;

	if (0 < _Dtest(&x) || _LDtest(&y) == _NANCODE || x == y)
		;
	else if (x < y && 0.0 < x || y < x && x < 0.0)
		{	/* increment magnitude of x */
                if (
#if !defined(_32_BIT_DOUBLE)
                    (++px[_D3] & 0xffff) == 0 &&
                    (++px[_D2] & 0xffff) == 0 &&
#endif
			   (++px[_D1] & 0xffff) == 0
			&& (++px[_D0] & ~_DSIGN) == _DMAX << _DOFF)
			{	/* raise overflow and inexact */
			_Feraise(_FE_OVERFLOW);
			_Feraise(_FE_INEXACT);
			}
		}
	else
		{	/* decrement magnitude of x */
		if (x == 0.0)
			{	/* change zero to tiny of opposite sign */
#if defined(_32_BIT_DOUBLE)
		        px[_D1] = 1;
#else
			px[_D3] = 1;
#endif
			px[_D0] ^= _DSIGN;
			}
		else if (
#if !defined(_32_BIT_DOUBLE)
                    (--px[_D3] & 0xffff) == 0xffff &&
                    (--px[_D2] & 0xffff) == 0xffff &&
#endif
			   (--px[_D1] & 0xffff) == 0xffff)
			--px[_D0];
		if ((px[_D0] & _DMASK) == 0)
			{	/* zero or denormalized, raise underflow and inexact */
			_Feraise(_FE_UNDERFLOW);
			_Feraise(_FE_INEXACT);
			}
		}
	return (x);
	}
_STD_END

/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V4.02:1476 */
