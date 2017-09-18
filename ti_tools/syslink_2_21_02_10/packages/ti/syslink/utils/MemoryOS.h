/** 
 *  @file   MemoryOS.h
 *
 *  @brief      Memory abstraction APIs for local memory allocation.
 *
 *              This provides a direct access to local memory allocation, which
 *              does not require creation of a Heap.
 *
 *
 */
/* 
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
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



#ifndef MEMORYOS_H_0x97D2
#define MEMORYOS_H_0x97D2

#include <ti/syslink/utils/MemoryDefs.h>
/* OSAL and utils */
#include <ti/syslink/utils/_MemoryOS.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 *  See MemoryDefs.h
 * =============================================================================
 */
/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Initialize the memory os module. */
Int32 MemoryOS_setup (void);

/* Finalize the memory os module. */
Int32 MemoryOS_destroy (void);

/* Allocates the specified number of bytes of type specified through flags. */
Ptr MemoryOS_alloc (UInt32 size, UInt32 align, UInt32 flags);

/* Allocates the specified number of bytes and memory is set to zero. */
Ptr MemoryOS_calloc (UInt32 size, UInt32 align, UInt32 flags);

/* Frees local memory */
Void MemoryOS_free (Ptr ptr, UInt32 size, UInt32 flags);

/* Copies the data between memory areas. Returns the number of bytes copied. */
Ptr MemoryOS_copy (Ptr dst, Ptr src, UInt32 len);

/* Set the specified values to the allocated  memory area */
Ptr MemoryOS_set (Ptr buf, Int value, UInt32 len);

/* Translate API */
Ptr MemoryOS_translate (Ptr srcAddr, Memory_XltFlags flags);

/* TBD: Add APIs for Memory_move, Scatter-Gather & translateAddr. */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* ifndef MEMORYOS_H_0x97D2 */
