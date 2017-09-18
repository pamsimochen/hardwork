/**
 *  @file   Std.h
 *
 *  @brief      This will have definitions of standard data types for
 *              platform abstraction.
 *
 *
 */
/*
 *  ============================================================================
 *
 *  Copyright (c) 2008-2013, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information:
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *
 */


#if !defined(STD_H)
#define STD_H

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
#else /* #if defined(SYSLINK_BUILD_RTOS) */

#ifdef SYSLINK_BUILDOS_LINUX
#if defined (__KERNEL__)
#include <ti/syslink/inc/knl/Linux/std_linux.h>
#else
#include <ti/syslink/inc/usr/Linux/std_linux.h>
#endif
#elif defined(SYSLINK_BUILDOS_QNX)
#include <ti/syslink/inc/knl/Qnx/std_qnx.h>
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#define Void              void

typedef char              Char;
typedef unsigned char     UChar;
typedef short             Short;
typedef unsigned short    UShort;
typedef int               Int;
typedef unsigned int      UInt;
typedef long              Long;
typedef unsigned long     ULong;
typedef float             Float;
typedef double            Double;
typedef long double       LDouble;


typedef unsigned short    Bool;
typedef void            * Ptr;       /* data pointer */
typedef char            * String;    /* null terminated string */
typedef const char      * CString;   /* null terminated constant string */


typedef int            *  IArg;
typedef unsigned int   *  UArg;
typedef char              Int8;
typedef short             Int16;
typedef int               Int32;

typedef unsigned char     UInt8;
typedef unsigned short    UInt16;
typedef unsigned int      UInt32;
typedef unsigned int      SizeT;
typedef unsigned char     Bits8;
typedef unsigned short    Bits16;
typedef UInt32            Bits32;

#ifdef SYSLINK_BUILDOS_QNX
typedef void             * HANDLE;
typedef unsigned int     atomic_t;
typedef unsigned long     ULONG;
typedef unsigned long     DWORD;
typedef unsigned int      uint;
typedef unsigned int      UINT32;
typedef unsigned short    bool;
#endif /* #ifdef SYSLINK_BUILDOS_QNX */

#define TRUE              1
#define FALSE             0

/*! Data type for errors */
typedef UInt32            Error_Block;

/*! Initialize error block */
#define Error_init(eb) *eb = 0

#if defined (__cplusplus)
}
#endif

#endif /* #if defined(SYSLINK_BUILD_RTOS) */

#endif
