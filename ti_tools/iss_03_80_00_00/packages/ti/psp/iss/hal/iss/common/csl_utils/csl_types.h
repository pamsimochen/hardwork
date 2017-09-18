
/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file sys_types.h
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
#ifndef SYS_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define SYS_TYPES_H

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

#define uint64 UInt64
#define uint32 UInt32
#define uint16 UInt16
#define uint8 UInt8
#define int64 Int64
#define int32 Int32
#define int16 Int16
#define int8 Int8

#if 0

/* ================================================================ */
/* 
 *================================================================== */
// typedef unsigned int UInt;

typedef unsigned char u8;

typedef unsigned short u16;

typedef short s16;

typedef unsigned int U32;

typedef long s64;

// typedef int s32;

typedef unsigned long u64;

typedef unsigned long ulong;

// typedef unsigned int UInt;
// typedef int Int;

// #define Void void 

// typedef unsigned char Uint8;
typedef unsigned char uint8;

// typedef unsigned short Uint16;
typedef unsigned short uint16;

typedef signed short int16;

// typedef unsigned int Uint32;
typedef unsigned long uint32;

typedef unsigned int int32;

#endif

/* ================================================================ */
/* 
 *================================================================== */
typedef enum {
    _TRUE = 1,
    _FALSE = 0
} BOOL;

/* ================================================================ */
/* 
 *================================================================== */

#define CSL_SUCCESS 0
#define CSL_FAILURE 1

#define CSL_RESOURCE_UNAVAILABLE     2
#define CSL_BUSY                     3
#define CSL_INVALID_INPUT            4
#define	CSL_CONFIGURE_ERROR	         5
#define CSL_NOT_CONFIGURED           6
#define CSL_HANDLER_NOT_REGISTERED   7
#define	CSL_NULL_PTR			     8
#define	CSL_DRV_BUSY			     9
#define	CSL_DMA_NOT_ALLOWED		    10

#define	CSL_GENERIC_MAX		        11

typedef int CSL_RETURN;

#define CSL_PRINT System_printf
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
