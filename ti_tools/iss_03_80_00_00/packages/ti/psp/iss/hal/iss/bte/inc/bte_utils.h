/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file bte_utils.h
*
* This file contains utilities and macros used in BTE driver code for MONICA/ OMAP-4 Ducati. 
*
* @path 
*
* @rev 1.0
*
* @developer: 
*/
/*========================================================================*/

#ifndef _CSLR_BTE_UTILS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_BTE_UTILS_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include <ti/psp/iss/hal/iss/common/csl_utils/csl_types.h>
#include <ti/psp/iss/hal/iss/common/csl_utils/csl_utils.h>
/* ================================================================ */
/* Macro to set a bit-field of a register to a given value
 * ================================================================== */
#define BTE_SET32  REG_SET32

/* ================================================================ */
/* Macro to write a register with a given value
 * ================================================================== */
#define BTE_WRITE32 REG_WRITE32

/* ================================================================ */
/* Macro to read a register value
 * ================================================================== */
#define BTE_READ32 REG_READ32

/* ================================================================ */
/* Macro to extract a bit-field of a register
 * ================================================================== */
#define BTE_FEXT REG_FEXT

#define BTE_SUCCESS CSL_SUCCESS
#define BTE_FAILURE CSL_FAILURE
#define BTE_RESOURCE_UNAVAILABLE CSL_RESOURCE_UNAVAILABLE
#define BTE_BUSY CSL_BUSY
#define BTE_INVALID_INPUT CSL_INVALID_INPUT
typedef CSL_RETURN BTE_RETURN;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
