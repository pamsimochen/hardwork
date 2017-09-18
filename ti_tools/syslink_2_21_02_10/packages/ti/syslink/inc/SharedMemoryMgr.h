/** 
 *  @file   SharedMemoryMgr.h
 *
 *  @brief      Header file implementing Memory  manager on sharedmemory.
 *              Works on 1-D shared memory
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




#ifndef SHAREDMEMORYMGR_H
#define SHAREDMEMORYMGR_H

#include <ti/syslink/utils/SyslinkMemMgr.h>
#include <ti/syslink/inc/_SyslinkMemMgr.h>

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/runtime/IHeap.h>
#else
#include <ti/syslink/utils/IHeap.h>
#endif

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining config parameters to create the instance.
 */
typedef struct SyslinkMemMgr_SharedMemory_Params_Tag {
     SyslinkMemMgr_CreateParams commonCreateParams;
     /* Common create params across all implementations */
     IHeap_Handle heapHandle;
     /* Heap handle to allocate/ Free memory */
} SyslinkMemMgr_SharedMemory_Params;

/*!
 *  @brief  Forward declaration of structure defining object for the
 *          SyslinkMemMgr_SharedMemory.
 */
typedef struct SyslinkMemMgr_SharedMemory_Object SyslinkMemMgr_SharedMemory_Object;

/*!
 *  @brief  Handle for the FrameQBufMgr instance.
 */
typedef SyslinkMemMgr_SharedMemory_Object *SyslinkMemMgr_SharedMemory_Handle;


/* =============================================================================
 * APIs
 * =============================================================================
 */
void SyslinkMemMgr_SharedMemory_Params_init(
        SyslinkMemMgr_SharedMemory_Params *params);

/* Function to create a SyslinkMemMgr instance */
SyslinkMemMgr_SharedMemory_Handle SyslinkMemMgr_SharedMemory_create(
        Ptr params);

/* Function to delete the created Memory Manager  instance*/
Int32 SyslinkMemMgr_SharedMemory_delete(
        SyslinkMemMgr_SharedMemory_Handle *pHandle);


/* Function to allocate memory from the SyslinkMemMgr */
Ptr SyslinkMemMgr_SharedMemory_alloc(SyslinkMemMgr_SharedMemory_Handle handle,
        SyslinkMemMgr_AllocParams *params);

/* Function to allocate memory from the SyslinkMemMgr */
Int32 SyslinkMemMgr_SharedMemory_free(SyslinkMemMgr_SharedMemory_Handle handle,
        Ptr ptr, UInt32 size);

/* Function to allocate memory from the SyslinkMemMgr. not used for shared memory*/
Ptr SyslinkMemMgr_SharedMemory_map(SyslinkMemMgr_SharedMemory_Handle handle,
        Ptr arg);

/* Function to allocate memory from the SyslinkMemMgr */
Int32 SyslinkMemMgr_SharedMemory_unmap(
        SyslinkMemMgr_SharedMemory_Handle handle, Ptr arg);

/* Function to allocate memory from the SyslinkMemMgr.Calls sharedregion API to do
 * address translations
 */
Ptr SyslinkMemMgr_SharedMemory_translate(
        SyslinkMemMgr_SharedMemory_Handle handle, Ptr buf,
        SyslinkMemMgr_AddrType srcAddrType,
        SyslinkMemMgr_AddrType desAddrType);


extern SyslinkMemMgr_Fxns SyslinkMemMgr_SharedMemory_fxns;

#if defined (__cplusplus)
}
#endif

#endif
