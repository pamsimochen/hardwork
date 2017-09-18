
/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file isp5_csl_types.h
* This file contains the macros and datatypes used in configuration of ISP5
* 
*
* @path Centaurus\drivers\drv_isp\inc\csl
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! 
*========================================================================= */
#ifndef _ISP5_SYS_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISP5_SYS_TYPES_H

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "../../common/csl_utils/csl_types.h"

#define ISP_SUCCESS CSL_SUCCESS
#define ISP_FAILURE CSL_FAILURE
#define ISP_RESOURCE_UNAVAILABLE CSL_RESOURCE_UNAVAILABLE
#define ISP_BUSY CSL_BUSY
#define ISP_INVALID_INPUT CSL_INVALID_INPUT

typedef CSL_RETURN ISP_RETURN;

#define ISP_PRINT CSL_PRINT

#define ISP_ASSERT(c,v)	{	\
	if(c==v)	\
		{	\
		return ISP_FAILURE;	\
		}	\
		}	\


typedef Void(*ISP_Callback_Fxn) (int, SizeT, UArg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
