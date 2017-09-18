/** 
 *  @file   IpcMemMgr.h
 *
 *  @brief      IPC Memory Manager
 *
 *              This abstracts the IPC Memory Management interface. Supported
 *              functions include alloc, free, and copy. Additional features:
 *
 *              1. Process wide shared memory allocation.
 *              2. Provides APIs to allocate kmalloc/vmalloc memory.
 *              3. mmap/unmap.
 *              4. translate
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


#ifndef ti_syslink_utils_IpcMemMgr__include
#define ti_syslink_utils_IpcMemMgr__include

#include <ti/syslink/osal/OsalTypes.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*!
 *  @brief  Unique module ID.
 */
#define IpcMemMgr_MODULEID               (0x434D)

#define IpcMemMgr_S_NOTLAST              2
#define IpcMemMgr_S_ALREADYSETUP         1
#define IpcMemMgr_S_SUCCESS              0
#define IpcMemMgr_E_FAIL                -1
#define IpcMemMgr_E_INVALIDSTATE        -2
#define IpcMemMgr_E_INVALIDARG          -3
#define IpcMemMgr_E_MEMORY              -4
#define IpcMemMgr_E_OSFAILURE           -5
#define IpcMemMgr_E_TOOMANYNAMEDALLOCS  -6
#define IpcMemMgr_E_NOTFOUND            -7
#define IpcMemMgr_E_WRONGSIZE           -8

#define IpcMemMgr_SHAREDMEMNAME         "/syslink_shm"
#define IpcMemMgr_MAXNAMEDBUFFERS       256     /* max named buffers    */
#define IpcMemMgr_MAX_NAMELEN           63      /* max name length      */


/* =============================================================================
 * Struct & Enums
 * =============================================================================
 */
/* Memory type */
typedef enum {
    IpcMemMgr_ALLOCTYPE_PSHMEM          = 0x00000,
    IpcMemMgr_ALLOCTYPE_PHYS_CACHE      = 0x00001,
    IpcMemMgr_ALLOCTYPE_PHYS_NOCACHE    = 0x00002,
    IpcMemMgr_ALLOCTYPE_END             = 0x00003,
} IpcMemMgr_MemType;

/*!
 *  @brief  Structure defining config parameters for overall System.
 */
typedef struct IpcMemMgr_Config_t {
    UInt32      size;
    /*!< Size of the shared memory to create */
    UInt32      minAlign;
    /* minimum alignement for the buffer allocations */
    UInt32      gateOffset;
    /* shared memory offset for the process gate */
} IpcMemMgr_Config;

/*!
 *  @brief      SharedRegion pointer type
 */
typedef UInt32 IpcMemMgr_SHPtr;

/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to setup the system memory maanger */
Void IpcMemMgr_getConfig(IpcMemMgr_Config * config);

/* setup the ipc memory manager module */
Int IpcMemMgr_setup(IpcMemMgr_Config *cfg);

/* destroy the ipc memory manager module */
Int IpcMemMgr_destroy(Void);

/* allocate anonymous shared memory */
// Ptr IpcMemMgr_alloc(UInt32 size, UInt32 reqAlign, IpcMemMgr_MemType memType);
Ptr IpcMemMgr_alloc(SizeT size);

/* allocate memory by name */
//Int IpcMemMgr_allocByName(String name, SizeT size, Ptr *addr, Bool *newAlloc);
Int IpcMemMgr_acquire(String name, SizeT size, Ptr *addr, Bool *newAlloc);

/* free memory anonymous memory */
Void IpcMemMgr_free(Ptr adr, SizeT size);

/* free memory by name */
// Int IpcMemMgr_freeByName(String name);
Int IpcMemMgr_release(String name);

/* map memory based on kernel virtual address */
/* Ptr IpcMemMgr_map(Ptr hint, Ptr buf, UInt32 size); */

/* unmap memory */
/* Int IpcMemMgr_unmap(Ptr buf, UInt32 size); */

/*
 *  ======== IpcMemMgr_getPtr ========
 *  Convert the given shared memory offset into a local address pointer.
 */
Ptr IpcMemMgr_getPtr(UInt32 offset);

/*
 *  ======== IpcMemMgr_getOffset ========
 *  Convert the given local address pointer into an offset from the
 *  shared memory base address.
 */
UInt32 IpcMemMgr_getOffset(Ptr vaddr);

/* allocate inter-process shared memory */
Ptr IpcMemMgr_allocShmemHeap(String name, UInt32 size, UInt32 align);

/* free inter-process shared memory */
Int IpcMemMgr_freeShmemHeap(Ptr buf, UInt32 size);

/* free inter-process shared memory by name */
Int IpcMemMgr_freeShmemHeapByName(String name);

IArg IpcMemMgr_enterGate(Void);

Void IpcMemMgr_leaveGate(IArg key);


#if defined (__cplusplus)
}
#endif

#endif /* ti_syslink_utils_IpcMemMgr__include */
