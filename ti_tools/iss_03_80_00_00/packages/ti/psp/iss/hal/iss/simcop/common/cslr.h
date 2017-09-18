/** ==================================================================
 *  @file   cslr.h                                                  
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
/******************************************************************************/
/* TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION */
/* Created 2008, (C) Copyright 2008 Texas Instruments.  All rights reserved. */
/* */
/* FILE : cslr.h */
/* PURPOSE : Linker file */
/* PROJECT : OMAP4 ISS */
/* AUTHOR : Anandhi */
/* DATE : Jul-02-2008 */
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Register layer central -- contains field-manipulation macro definitions */

#ifndef _CSLR_H_
#define _CSLR_H_
#include "csl_types.h"

/* the "expression" macros */
// #ifdef __NEW_CSL_MACROS__
/* the Field MaKe macro */
#define CSL_FMK(PER_REG_FIELD, val)                                         \
    (((val) << CSL_##PER_REG_FIELD##_SHIFT) & CSL_##PER_REG_FIELD##_MASK)

/* the Field EXTract macro */
#define CSL_FEXT(reg, PER_REG_FIELD)                                        \
    (((reg) & CSL_##PER_REG_FIELD##_MASK) >> CSL_##PER_REG_FIELD##_SHIFT)

/* the Field INSert macro */
#define CSL_FINS(reg, PER_REG_FIELD, val)                                   \
    ((reg) = ((reg) & ~CSL_##PER_REG_FIELD##_MASK)                          \
    | CSL_FMK(PER_REG_FIELD, val))

/* the Register INSert macro */
#define CSL_RINS(reg, val)                                   \
    ((reg) = (CSL_BitMask32)(val))

/* the Register EXTract macro */
#define CSL_REXT(reg)   (*(CSL_BitMask32 *)(reg))

/* the "token" macros */

/* the Field MaKe (Token) macro */
#define CSL_FMKT(PER_REG_FIELD, TOKEN)                                      \
    CSL_FMK(PER_REG_FIELD, CSL_##PER_REG_FIELD##_##TOKEN)

/* the Field INSert (Token) macro */
#define CSL_FINST(reg, PER_REG_FIELD, TOKEN)                                \
    CSL_FINS((reg), PER_REG_FIELD, CSL_##PER_REG_FIELD##_##TOKEN)

/* the "raw" macros */

/* the Field MaKe (Raw) macro */
#define CSL_FMKR(msb, lsb, val)                                             \
    (((val) & ((1 << ((msb) - (lsb) + 1)) - 1)) << (lsb))

/* the Field EXTract (Raw) macro */
#define CSL_FEXTR(reg, msb, lsb)                                            \
    (((reg) >> (lsb)) & ((1 << ((msb) - (lsb) + 1)) - 1))

/* the Field INSert (Raw) macro */
#define CSL_FINSR(reg, msb, lsb, val)                                       \
    ((reg) = ((reg) &~ (((1 << ((msb) - (lsb) + 1)) - 1) << (lsb)))         \
    | CSL_FMKR(msb, lsb, val))

#define CSL_EXIT_IF(_Cond,_ErrorCode) { \
    if ((_Cond)) { \
        status = _ErrorCode; \
        SIMCOP_PRINT ("Error :: %s:: %s : %s : %d :: Exiting because : %s\n", \
                #_ErrorCode,__FILE__, __FUNCTION__, __LINE__, #_Cond); \
        goto EXIT; \
    } \
}

#endif                                                     /* _CSLR_H_ */
#ifdef __cplusplus
}
#endif /* __cplusplus */
