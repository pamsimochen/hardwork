/** ==================================================================
 *  @file   csl_types.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
\*****************************************************/

#ifndef _CSL_TYPES_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_TYPES_H_

#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

typedef Int16 CSL_Uid;

typedef Int16 CSL_ModuleId;

typedef Uint32 CSL_Xio;

typedef Uint16 CSL_BitMask16;

typedef Uint32 CSL_BitMask32;

typedef volatile Uint8 CSL_Reg8;

typedef volatile Uint16 CSL_Reg16;

typedef volatile Uint32 CSL_Reg32;

typedef long CSL_Status;

typedef Int16 CSL_InstNum;

typedef Int16 CSL_ChaNum;

typedef enum {
    CSL_EXCLUSIVE = 0,
    CSL_SHARED = 1
} CSL_OpenMode;

typedef enum {
    CSL_FAIL = 0,
    CSL_PASS = 1
} CSL_Test;

typedef Uint8 Bits2;

typedef Uint8 Bits3;

typedef Uint8 Bits4;

typedef Uint8 Bits5;

typedef Uint8 Bits6;

typedef Uint8 Bits7;

typedef Uint16 Bits9;

typedef Uint16 Bits10;

typedef Uint16 Bits11;

typedef Uint16 Bits12;

typedef Uint16 Bits13;

typedef Uint16 Bits14;

typedef Uint16 Bits15;

#define MAX_32BIT 0xFFFFFFFF
#define MAX_24BIT 16777216
#define MAX_20BIT 1048576
#define MAX_16BIT 65535
#define MAX_14BIT 16383
#define MAX_13BIT 8191
#define MAX_12BIT 4095
#define MAX_10BIT 1023
#define MAX_9BIT 511
#define MAX_7BIT 127
#define MAX_4BIT 15
#define MAX_3BIT 7
#define MAX_2BIT 3

#define MIN_14BIT -16384
#define MIN_13BIT -8192

#ifdef __STANDALONE_TESTING__
#define SIMCOP_PRINT printf
#else
#define SIMCOP_PRINT System_printf
#endif

#ifndef NULL
#define NULL            (0)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _CSL_TYPES_H_ */
