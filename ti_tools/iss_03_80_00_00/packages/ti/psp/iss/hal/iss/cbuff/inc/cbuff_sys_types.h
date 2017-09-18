/** ==================================================================
 *  @file   cbuff_sys_types.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/cbuff/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef CBUFF_SYS_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define CBUFF_SYS_TYPES_H

#include <xdc/std.h>
#include <ti/psp/iss/hal/iss/common/csl_utils/csl_types.h>
#include <ti/psp/iss/hal/iss/common/csl_utils/csl_utils.h>

/* ISP RETURN TYPES */

#define CBUFF_SUCCESS 0
#define CBUFF_FAILURE 1
#define CBUFF_RESOURCE_UNAVAILABLE 2
#define CBUFF_BUSY 3
#define CBUFF_INVALID_INPUT 4
typedef int CBUFF_RETURN;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
