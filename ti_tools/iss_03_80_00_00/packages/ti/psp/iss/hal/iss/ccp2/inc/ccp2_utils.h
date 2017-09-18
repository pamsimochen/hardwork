/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csi2_utils.h
*
* This file contains utilities and macros used in CSI2 driver code for MONICA/ OMAP-4 Ducati. 
*
* @path drv_csi2/
*
* @rev 1.0
*
* @developer: 
*/
/*========================================================================*/

#ifndef _CSLR_CCP2_UTILS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_CCP2_UTILS_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#define CSI2_SUCCESS 0
#define CSI2_FAILURE 1
#define CSI2_BUSY 2
#define CSI2_INVALID_INPUT 3

#include "ti/psp/iss/hal/iss/simcop/common/csl_types.h"
#include "ti/psp/iss/hal/iss/common/csl_utils/csl_utils.h"
/* ================================================================ */
/* Macro to set a bit-field of a register to a given value
 * ================================================================== */
#define CCP2_SET32  REG_SET32

/* ================================================================ */
/* Macro to write a register with a given value
 * ================================================================== */
#define CCP2_WRITE32 REG_WRITE32

/* ================================================================ */
/* Macro to read a register value
 * ================================================================== */
#define CCP2_READ32 REG_READ32

/* ================================================================ */
/* Macro to extract a bit-field of a register
 * ================================================================== */
#define CCP2_FEXT REG_FEXT

#define CCP2_SUCCESS CSL_SUCCESS
#define CCP2_FAILURE CSL_FAILURE
#define CCP2_RESOURCE_UNAVAILABLE CSL_RESOURCE_UNAVAILABLE
#define CCP2_BUSY CSL_BUSY
#define CCP2_INVALID_INPUT CSL_INVALID_INPUT
typedef CSL_RETURN CCP2_RETURN;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
