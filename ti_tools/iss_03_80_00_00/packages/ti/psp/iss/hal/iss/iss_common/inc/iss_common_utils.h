/** ==================================================================
 *  @file   iss_common_utils.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/iss_common/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _ISS_COMMON_UTILS_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_COMMON_UTILS_H

#include <ti/psp/iss/hal/iss/common/csl_utils/csl_types.h>
#include <ti/psp/iss/hal/iss/common/csl_utils/csl_utils.h>
/* ================================================================ */
/* Macro to set a bit-field of a register to a given value
 * ================================================================== */
#define ISS_SET32  REG_SET32

/* ================================================================ */
/* Macro to write a register with a given value
 * ================================================================== */
#define ISS_WRITE32 REG_WRITE32

/* ================================================================ */
/* Macro to read a register value
 * ================================================================== */
#define ISS_READ32 REG_READ32

/* ================================================================ */
/* Macro to extract a bit-field of a register
 * ================================================================== */
#define ISS_FEXT REG_FEXT

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
