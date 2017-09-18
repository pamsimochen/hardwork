/** ==================================================================
 *  @file   cbuff_utils.h                                                  
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
#ifndef CBUFF_CSL_UTILS_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define CBUFF_CSL_UTILS_H

// #include"sys_types.h"

#define CBUFF_SET32(reg,val,shift,mask)    \
((reg)=(((reg)&(~mask))|(val<<shift)))

#define CBUFF_WRITE32(reg,val)   \
	( (reg)=(uint32)(val))

#define CBUFF_READ32(reg_addr)	\
(*(uint32 *) reg_addr)

#define CBUFF_FEXT(reg,shift,mask)	\
    (((reg) & mask) >> shift)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
