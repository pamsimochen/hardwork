/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
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
 *
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

/*
*  @file timm_osal_types.h
*  The timm_osal_types header file defines the primative osal type definitions.
*  @path
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 0.2: Ported to BIOS 6, gaurav.a@ti.com
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

#ifndef _TIMM_OSAL_TYPES_H_
#define _TIMM_OSAL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>


typedef unsigned char       u8;
typedef unsigned short      u16;
typedef short               s16;
typedef unsigned int        u32;
typedef int                 s32;
typedef unsigned long long  u64;
typedef long long           s64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned short ushort;
//typedef unsigned long uint32;
//typedef unsigned long long uint64;
typedef unsigned int OSALBool;
                           /* --- Signed types --- */
typedef signed char int8;
typedef signed short int16;
//typedef signed long int32;
typedef long long int64;

typedef unsigned int uint;
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

typedef   Int8   TIMM_OSAL_S8;
typedef   Int16  TIMM_OSAL_S16;
typedef   signed long TIMM_OSAL_S32;

#define TIMM_OSAL_INT8_MIN 0xFF
#define TIMM_OSAL_INT8_MAX 0x7F

#define TIMM_OSAL_INT16_MIN 0xFFFF
#define TIMM_OSAL_INT16_MAX 0x7FFF

#define TIMM_OSAL_INT32_MIN 0xFFFFFFFF
#define TIMM_OSAL_INT32_MAX 0x7FFFFFFF

typedef   UInt8   TIMM_OSAL_U8;
typedef   UInt16  TIMM_OSAL_U16;
typedef  unsigned long  TIMM_OSAL_U32;

#define TIMM_OSAL_UINT8_MIN 0
#define TIMM_OSAL_UINT8_MAX 0xFF

#define TIMM_OSAL_UINT16_MIN 0
#define TIMM_OSAL_UINT16_MAX 0xFFFF

#define TIMM_OSAL_UINT32_MIN 0
#define TIMM_OSAL_UINT32_MAX 0xFFFFFFFF


typedef   char    TIMM_OSAL_CHAR;

typedef   void *  TIMM_OSAL_HANDLE;
typedef   void *  TIMM_OSAL_PTR;
#if 0
typedef enum TIMM_OSAL_BOOL {
    TIMM_OSAL_FALSE = 0,
    TIMM_OSAL_TRUE = !TIMM_OSAL_FALSE,
    TIMM_OSAL_BOOL_MAX = 0x7FFFFFFF
} TIMM_OSAL_BOOL;
#endif
#define TIMM_OSAL_SUSPEND     (0xFFFFFFFF)
#define TIMM_OSAL_NO_SUSPEND  (0)

#define TIMM_OSAL_NULL 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_TYPES_H_ */


