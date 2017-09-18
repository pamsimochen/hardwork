/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_utils.h
*          This file contains the bit macros for seting fields withinin specified register
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

#ifndef CSL_UTILS_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define CSL_UTILS_H

#undef CSL_DEBUG
// #define CSL_DEBUG

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "csl_types.h"

/* ================================================================ */
/* 
 *================================================================== */
#ifndef CSL_DEBUG
#define REG_SET32(reg,val,shift,mask)    \
	((reg)=(((reg)&(~mask))|(val<<shift)))

#else
#define REG_SET32(reg,val,shift,mask)   REG_SET32_debug(&reg,val,shift,mask)
#endif

/* ================================================================ */
/* 
 *================================================================== */
#ifndef CSL_DEBUG
#define REG_WRITE32(reg,val)   \
	( (reg)=(uint32)(val))

#else
#define REG_WRITE32(reg,val)    REG_WRITE32_debug(&reg,val)

#endif

#ifndef CSL_DEBUG
#define REG_WRITE16(reg,val)   \
	( (reg)=(uint16)(val))

#else
#define REG_WRITE32(reg,val)    REG_WRITE16_debug(&reg,val)

#endif

#ifndef CSL_DEBUG
#define REG_WRITE8(reg,val)   \
	( (reg)=(uint8)(val))

#else
#define REG_WRITE32(reg,val)    REG_WRITE8_debug(&reg,val)

#endif
/* ================================================================ */
/* 
 *================================================================== */
#define REG_READ32(reg_addr)	\
(*(uint32 *) reg_addr)

/* ================================================================ */
/* 
 *================================================================== */
#define REG_FEXT(reg,shift,mask)	\
    (((reg) & mask) >> shift)

/* ================================================================ */
/* 
 *================================================================== */
#define register_write(add,val)		\
	*(volatile uint32*)(add)=val
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
