/*
 *  @file   IpcMemMgr.c
 *
 *  @brief      IPC memory manager
 *
 *              This abstracts the Memory management interface in the user-side.
 *              Allocation, Freeing-up, copy are supported for the user memory
 *              management.
 *              It supports:
 *                  1. Process wide shared memory allocation.
 *
 *
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


/* Standard headers */
#include <ti/syslink/Std.h>

/* LIBC headers */
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>



/* Utilities headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateProcess.h>
#include <ti/syslink/osal/OsalProcess.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/osal/Atomic.h>
#include <ti/syslink/utils/String.h>

/* Module headers */
#include <ti/syslink/utils/IpcMemMgr.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/* Name of the memory allocation where ipcmemmgr's state object is located */
#define IpcMemMgr_STATEOBJECTNAME   "ipcmemmgr"

/* Name of the memory allocation where shared heap */
#define IpcMemMgr_SHAREDMEMNAME     "/syslink_shm"

/* Size of shared memory heap (2MB) */
#define IpcMemMgr_SHAREDHEAP_SIZE   (16*1024*1024)

/* Macro for memory alignment */
//#define IpcMemMgr_ALIGN(x,y)        (((x) + (y) - 1) & ~((y) - 1))

#define IpcMemMgr_ALIGN(x, y) \
    (UInt32)((((UInt32)(x)) + (((UInt32)(y)) - 1)) & ~((UInt32)(y) - 1))

/* Macros to help traversing the shared memory chunk list */
#define IpcMemMgr_O2CHUNK(x) ((IpcMemMgr_ShmemChunk*)IpcMemMgr_getPtr ((x)))

#define IpcMemMgr_CHUNK2O(x)  IpcMemMgr_getOffset ((x))

#define IpcMemMgr_SHP2CHUNK(x) ((IpcMemMgr_AllocChunk*)IpcMemMgr_getPtr ((x)))

#define IpcMemMgr_CHUNK2SHP(x) IpcMemMgr_getOffset ((x))


//#define PG_SIZE_4K                  4096u
#define PG_SIZE_4K                  2048u

/*! @brief Macro to make a correct module magic number with refCount */
#define IpcMemMgr_MAKE_MAGICSTAMP(x)    ((IpcMemMgr_MODULEID << 12u) | (x))

/* IOCTL for the IpcMemMgr driver */
/* Alloc types */
#define IpcMemMgr_ALLOC_NORMAL      0x00000
#define IpcMemMgr_ALLOC_PHYS        0x00001
/* Free types */
#define IpcMemMgr_FREE_NORMAL       0x10000
#define IpcMemMgr_FREE_PHYS         0x10001

/* =============================================================================
 * Structs & Enums
 * =============================================================================
 */
/* Struct for inter-process shared memory chunks */
typedef struct IpcMemMgr_ShmemChunk_tag {
    UInt32      used;           /* TODO: remove */
    Int32       refCount;       /* number of users of block         */
    UInt32      size;           /* size of the memory block         */
    UInt32      offset;         /* buffer offset from shMemBase     */
    UInt32      nameLen;        /* length of name */
    Char        name[IpcMemMgr_MAX_NAMELEN+1]; /* name of the memory chunk */
} IpcMemMgr_ShmemChunk;

typedef struct IpcMemMgr_AllocChunk_tag {
    volatile UInt32 inUse       :1;
    /* Flag to indicate free block */
    volatile UInt32 chunkSizePg :19;
    /* Size of chunk in pages */
    volatile UInt32 allocIndex  :12;
    /* Index into the array of allocated names */
} IpcMemMgr_AllocChunk;

typedef struct IpcMemMgr_Header_tag {
    IpcMemMgr_SHPtr    next;
    /* SHPtr to next header (Header *)    */
    UInt32             size;
    /* Size of this segment (Memory.size) */
} IpcMemMgr_Header;

/* Global module state structure visible to all processes */
typedef struct IpcMemMgr_Module_GlobalState_tag {
    UInt32                  regionSize;
    /* inter-process shared memory area size */
    UInt32                  reservedSize;
    /* Reserved size at the beginning of the inter-process shared memory area */
    GateProcess_ShMem       gateShMem;
    UInt32                  daemonPid;
    /* PID of the master process for inter-process shared memory area*/
    UInt32                  minAlign;
    UInt32                  freeChunk;
    /* Index of largest free chunk */
    UInt32                  freeSize;
    /* Size of largest free chunk */
    IpcMemMgr_AllocChunk * allocator;
    /* Array of bit map for memory allocator */

    IpcMemMgr_SHPtr  bufPtr;
    /* Inter process shared memory managed by the module  */
    IpcMemMgr_Header    head;
    /* HeapMemMP      head */

} IpcMemMgr_Module_GlobalState;

/* Module state structure visible to local processes */
typedef struct IpcMemMgr_Module_State_tag {
    Int32                           refCount;
    GateProcess_Handle              gate;       /* module gate */
    Int                             shmfd;
    Ptr                             shMemArea;
    UInt32                          pageSize;
    IpcMemMgr_Config                defCfg;
    IpcMemMgr_Module_GlobalState *  global;
    Char                         *  buf;        /* local pointer to buf */
    UInt32                          bufSize;    /* Buffer Size */
    UInt32                          pid;
    IpcMemMgr_Config                cfg; /* Config information of the module */
    IpcMemMgr_ShmemChunk            allocNamedList[IpcMemMgr_MAXNAMEDBUFFERS];
    /* list of allocated named blocks */
} IpcMemMgr_Module_State;


/* =============================================================================
 * Globals
 * =============================================================================
 */
/* State objects */
static IpcMemMgr_Module_State   IpcMemMgr_moduleState =
{
    .refCount = 0,
    .shMemArea = NULL,
    .pid = 0u
};

/* static */
IpcMemMgr_Module_State * IpcMemMgr_module = &IpcMemMgr_moduleState;

/* static */
IpcMemMgr_Config IpcMemMgr_defaultCfg =
{
    .size = IpcMemMgr_SHAREDHEAP_SIZE,
    .minAlign = 128,
    .gateOffset = 0
};

/* =============================================================================
 * Forward
 * =============================================================================
 */
/* Returns the requested free chunk  */
// static IpcMemMgr_AllocChunk * _IpcMemMgr_getFreeChunk(UInt32 size);


/* =============================================================================
 * APIs
 * =============================================================================
 */

/*
 *  ======== IpcMemMgr_getConfig ========
 */
Void IpcMemMgr_getConfig(IpcMemMgr_Config *cfg)
{
    Int     status = IpcMemMgr_S_SUCCESS;
    IArg    key;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_getConfig: cfg=0x%08x", cfg);
    GT_assert(curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_getConfig",
            IpcMemMgr_E_INVALIDARG, "arg is null");
    }
#endif

    key = Gate_enterSystem();

    if (status == IpcMemMgr_S_SUCCESS) {
        /* if not setup, return default config */
        if (IpcMemMgr_module->refCount == 0) {
            Memory_copy(cfg, &IpcMemMgr_defaultCfg, sizeof(IpcMemMgr_Config));
        }
        else {
            /* return the runtime config */
            Memory_copy(cfg, &IpcMemMgr_module->cfg, sizeof(IpcMemMgr_Config));
        }
    }

    Gate_leaveSystem(key);

    GT_0trace(curTrace, GT_LEAVE, "IpcMemMgr_getConfig");
}

/*
 *  ======== IpcMemMgr_setup ========
 */
Int IpcMemMgr_setup(IpcMemMgr_Config *cfg)
{
    Int                             status = IpcMemMgr_S_SUCCESS;
    IArg                            key;
//  UInt32                          allocatorSize;
    UInt32                          chunkOffset;
    IpcMemMgr_Module_GlobalState  * gModule         = NULL;
//  IpcMemMgr_AllocChunk          * chunk           = NULL;
    IpcMemMgr_Header * begHeader = NULL;
    Void *                          addr;
    UInt32                          size;
    GateProcess_Handle              gate;
    Ptr                             shMemAddr;

    GT_0trace(curTrace, GT_ENTER, "IpcMemMgr_setup");

    /* TODO: Check for 4K aligned parameter size */

    key = Gate_enterSystem();

#if 0
    /* Handle fork processes */
    if (IpcMemMgr_module->pid == 0) {
        IpcMemMgr_module->pid = OsalProcess_getId();
    } else if (IpcMemMgr_module->pid != OsalProcess_getId()) {
        size = IpcMemMgr_ALIGN(sizeof(IpcMemMgr_Module_GlobalState),
                       IpcMemMgr_module->pageSize);
        /* Free the shared region */
        _IpcMemMgrD_unmap(IpcMemMgr_module->shMemArea,
                  IpcMemMgrD_SHAREDHEAP_SIZE);
        _IpcMemMgrD_unmap(IpcMemMgr_module->global, size);
        /* re-init the state variable */
        memset((Ptr)IpcMemMgr_module, 0, sizeof(IpcMemMgrD_Module_State));
        IpcMemMgr_module->drvHandle = -1;
    }
#endif

    /* When creating the IpcMemMgr module, following steps are done:
     * 1. Create the IPC Shared Memory.
     * 2. Configure the global module state.
     * 3. Allocate shared memory for IpcMemMgrD state object by name.
     * 4. Map the shared memory to the current user space.
     * 5. Allocate a region for inter-process shared memory heap.
     * 6. Allocate shared memory for the inter-process shared memory heap.
     * 7. Map the inter-process shared memory heap memory to the region in
     *    step5.
     * 8. If this is first time System memory manager is created, then
     *    create the shared heap.
     */

    IpcMemMgr_module->refCount++;

    /* if already setup, just return the runtime config */
    if (IpcMemMgr_module->refCount > 1) {
        status = IpcMemMgr_S_ALREADYSETUP;
        GT_1trace(curTrace, GT_1CLASS,
            "IpcMemMgr module already initialized, refCount=%d",
            IpcMemMgr_module->refCount);

        /* return the rutime config */
        if (cfg != NULL) {
            IpcMemMgr_getConfig(cfg);
        }
    }

    /* save given config or use default config */
    if (status == IpcMemMgr_S_SUCCESS) {
        if (cfg != NULL) {
            Memory_copy((Ptr)&IpcMemMgr_module->cfg, (Ptr)cfg,
                sizeof(IpcMemMgr_Config));
        }
        else {
            Memory_copy((Ptr)&IpcMemMgr_module->cfg,
                (Ptr)&IpcMemMgr_defaultCfg, sizeof(IpcMemMgr_Config));
        }
    }

    /* delete existing shared memory object */
    if (status == IpcMemMgr_S_SUCCESS) {
        IpcMemMgr_module->shmfd = shm_open(IpcMemMgr_SHAREDMEMNAME,
            O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH);

        if (IpcMemMgr_module->shmfd >= 0) {
            /* delete the shared memory object */
            shm_unlink(IpcMemMgr_SHAREDMEMNAME);
        }
    }

    /* create the shared memory object */
    if (status == IpcMemMgr_S_SUCCESS) {
//      IpcMemMgr_module->pageSize = sysconf(_SC_PAGESIZE);
        IpcMemMgr_module->pageSize = PG_SIZE_4K;
        IpcMemMgr_module->pid = OsalProcess_getPid();

        IpcMemMgr_module->shmfd = shm_open(IpcMemMgr_SHAREDMEMNAME,
            O_CREAT | O_RDWR | O_TRUNC | O_EXCL, S_IRUSR | S_IWUSR | S_IWGRP |
            S_IRGRP | S_IWOTH | S_IROTH);

        if (IpcMemMgr_module->shmfd < 0) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
            status, "Failed to open the shared memory");
        }
    }

    /* allocate backing store for shared memory object */
    if (status == IpcMemMgr_S_SUCCESS) {
        status = ftruncate(IpcMemMgr_module->shmfd, IpcMemMgr_module->cfg.size);

        if (status < 0) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
            status, "Failed to set size of shared memory");
        }
    }

    /* map the shared memory into the current process address space */
    if (status == IpcMemMgr_S_SUCCESS) {
        addr = (UInt *)mmap(0, IpcMemMgr_module->cfg.size,
            PROT_READ | PROT_WRITE, MAP_SHARED, IpcMemMgr_module->shmfd, 0);

        if (addr == MAP_FAILED) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "Failed to map shared memory");
        }
        else {
            IpcMemMgr_module->shMemArea = (Ptr)addr;
        }
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        gModule = (IpcMemMgr_Module_GlobalState *)(IpcMemMgr_module->shMemArea);
//      gModule->refCount++;

        /* Set the following only for the daemon process */
        gModule->daemonPid = IpcMemMgr_module->pid;
        gModule->regionSize = IpcMemMgr_module->cfg.size;

//      chunkOffset = sizeof(IpcMemMgr_Module_GlobalState);
//      chunkOffset = sizeof(IpcMemMgr_AllocChunk *);

#if 0
        chunkOffset = (sizeof(IpcMemMgr_Module_GlobalState)) -
            (sizeof(IpcMemMgr_AllocChunk *));
        chunkOffset = (UInt32 ) IpcMemMgr_ALIGN(chunkOffset, PG_SIZE_4K);

        Memory_set(IpcMemMgr_module->allocNamedList, 0,
            sizeof(IpcMemMgr_ShmemChunk)* IpcMemMgr_MAXNAMEDBUFFERS);

        allocatorSize = IpcMemMgr_ALIGN((IpcMemMgr_module->cfg.size - \
                         sizeof(IpcMemMgr_Module_GlobalState) - \
                         sizeof(IpcMemMgr_AllocChunk *)), PG_SIZE_4K);
        allocatorSize = (allocatorSize / PG_SIZE_4K) * \
                sizeof(IpcMemMgr_AllocChunk);

        allocatorSize = IpcMemMgr_ALIGN(allocatorSize, PG_SIZE_4K);

        chunk = (IpcMemMgr_AllocChunk *)(IpcMemMgr_module->shMemArea +
                            chunkOffset);

        Memory_set(chunk, 0, allocatorSize);

        gModule->allocator = (IpcMemMgr_AllocChunk *)IpcMemMgr_getOffset(chunk);
#endif

        gModule->minAlign = IpcMemMgr_module->cfg.minAlign;
        chunkOffset = IpcMemMgr_ALIGN(sizeof(IpcMemMgr_Module_GlobalState),
            IpcMemMgr_module->cfg.minAlign);

        gModule->reservedSize = chunkOffset;
        gModule->freeSize = (gModule->regionSize - gModule->reservedSize);
        gModule->freeChunk = 0;
//      chunk->inUse = 0;
//      chunk->chunkSizePg = (gModule->freeSize / PG_SIZE_4K);

#if 0 /* replaced below, -rvh */
        IpcMemMgr_module->buf = (Ptr)((UInt32)IpcMemMgr_module->shMemArea
                                          + chunkOffset);
        IpcMemMgr_module->buf = (Ptr)IpcMemMgr_ALIGN((IpcMemMgr_module->buf),
            (gModule->minAlign));
#else

        addr = (Ptr)((UInt32)(IpcMemMgr_module->shMemArea) + chunkOffset);
        IpcMemMgr_module->buf = (Ptr)IpcMemMgr_ALIGN(addr, gModule->minAlign);
#endif


#if 0 /* replaced below, -rvh */
        IpcMemMgr_module->bufSize = gModule->regionSize - gModule->reservedSize;
        IpcMemMgr_module->bufSize =
           (IpcMemMgr_module->bufSize/ gModule->minAlign) * gModule->minAlign;
#else
        size = gModule->regionSize - gModule->reservedSize;
        IpcMemMgr_module->bufSize = (size/gModule->minAlign)*gModule->minAlign;
#endif

        gModule->bufPtr = IpcMemMgr_getOffset(IpcMemMgr_module->buf);
        gModule->head.size = IpcMemMgr_module->bufSize;

        memset(IpcMemMgr_module->buf, 0, IpcMemMgr_module->bufSize);

        /*
         *  Fill in the top of the memory block
         *  next: pointer will be NULL (end of the list)
         *  size: size of this block
         *  NOTE: no need to Cache_inv because obj->attrs->bufPtr
         *  should be const
         */
        begHeader = (IpcMemMgr_Header *)IpcMemMgr_module->buf;
        begHeader->next = (UInt32)-1;
        begHeader->size = IpcMemMgr_module->bufSize;

        gModule->head.next = gModule->bufPtr;

        IpcMemMgr_module->global = gModule;
    }

    /* create the process gate */
    if (status == IpcMemMgr_S_SUCCESS) {
        shMemAddr = &IpcMemMgr_module->global->gateShMem;
        IpcMemMgr_module->cfg.gateOffset = IpcMemMgr_getOffset(shMemAddr);

        gate = GateProcess_create(shMemAddr, NULL);

        if (gate == NULL) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "GateProcess_create failed");
        }
        else {
            IpcMemMgr_module->gate = gate;
        }
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_setup", status);

    return(status);
}


/*
 *  ======== IpcMemMgr_destroy ========
 */
Int IpcMemMgr_destroy(Void)
{
    Int                             status = IpcMemMgr_S_SUCCESS;
    IArg                            key;
//  IpcMemMgr_Module_GlobalState  * gModule         = NULL;
    IpcMemMgr_ShmemChunk          * allocNamedList;
//  IpcMemMgr_AllocChunk          * allocator;
//  UInt32                          max;
    UInt32                          i;
//  UInt32                          next;

    GT_0trace(curTrace, GT_ENTER, "IpcMemMgr_destroy");

    key = Gate_enterSystem();

    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_destroy",
            status, "Module was not initialized");
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        /* dec refCount, do nothing if not last user */
        IpcMemMgr_module->refCount--;

        if (IpcMemMgr_module->refCount > 0) {
            status = IpcMemMgr_S_NOTLAST;
        }
    }

    /* delete the process gate */
    if (status == IpcMemMgr_S_SUCCESS) {
        status = GateProcess_delete(&IpcMemMgr_module->gate);

        if (status <= 0) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_destroy",
                status, "GateProcess_delete failed");
        }
        else {
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        /* check and free any named memory objects */
        allocNamedList = IpcMemMgr_module->allocNamedList;

        for (i = 0; i < IpcMemMgr_MAXNAMEDBUFFERS; i++) {
            if (allocNamedList[i].used == 1) {
                allocNamedList[i].refCount = 1;
//              IpcMemMgr_free(allocNamedList [i].name, 0,
//                         allocNamedList[i].size);
                IpcMemMgr_release(allocNamedList[i].name);
                Memory_set((Void *)&allocNamedList[i], 0,
                           sizeof(IpcMemMgr_ShmemChunk));
            }
        }

/* TODO */
#if 0
        /* check for any unfreed allocator elements */
        max = (gModule->regionSize - gModule->reservedSize) / PG_SIZE_4K;
        allocator = (IpcMemMgr_AllocChunk *)IpcMemMgr_getPtr(
                    (UInt32)gModule->allocator);

        for (i = 0; i < max; i = next) {
            if (allocator[i].inUse == 1) {
                next = i + allocator[i].chunkSizePg;
                IpcMemMgr_free(NULL, gModule->reservedSize + (PG_SIZE_4K *
                        ((i - (UInt32)gModule->allocator))),
                        allocator[i].chunkSizePg * PG_SIZE_4K);
            }
        }
#endif

        /* unmap shared memory from process address space */
        if (IpcMemMgr_module->shMemArea != NULL) {
            munmap(IpcMemMgr_module->shMemArea, IpcMemMgr_module->cfg.size);
            IpcMemMgr_module->shMemArea = NULL;
        }

        /* delete the shared memory object */
        shm_unlink(IpcMemMgr_SHAREDMEMNAME);
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_destroy: [0x%08x]", status);

    return(status);
}


#if 0
/* Function to setup the system memory manager in the user process*/
Int
IpcMemMgr_setup(Void) {
    Int                 status  = IpcMemMgr_S_SUCCESS;
    IArg                key;
    IpcMemMgr_Config    cfg;

    GT_0trace(curTrace, GT_ENTER, "IpcMemMgr_setup");

    /* TODO: Check for 4K aligned parameter size */

    /* If this is the first this api is called in a process */
    key = Gate_enterSystem();

    /* When creating the System memory manager module, following steps are done:
     * 1. Create the IPC Shared Memory.
     * 2. Configure the global module state.
     * 3. Allocate shared memory for IpcMemMgrD state object by name.
     * 4. Map the shared memory to the current user space.
     * 5. Allocate a region for inter-process shared memory heap.
     * 6. Allocate shared memory for the inter-process shared memory heap.
     * 7. Map the inter-process shared memory heap memory to the region in step5.
     * 8. If this is first time System memory manager is created, then
     *    create the shared heap.
     */
    IpcMemMgr_getConfig(&cfg);
    IpcMemMgr_module->refCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (IpcMemMgr_module->refCount > 1) {
        status = IpcMemMgr_S_ALREADYSETUP;
        GT_1trace(curTrace,
              GT_1CLASS,
              "IpcMemMgr module has been already setup in this process.\n"
              "    RefCount: [%d]\n",
              IpcMemMgr_module->refCount);
    } else {

        IpcMemMgr_module->pageSize = PG_SIZE_4K;
        IpcMemMgr_module->pid = OsalProcess_getPid();

        /* Open the Shared Memory */
        IpcMemMgr_module->shmfd = shm_open(IpcMemMgr_SHAREDMEMNAME,
                           O_RDWR,
                           S_IRUSR | S_IWUSR | S_IWGRP |
                           S_IRGRP | S_IWOTH | S_IROTH);
        if (IpcMemMgr_module->shmfd < 0) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace,
                        GT_4CLASS,
                        "IpcMemMgr_setup",
                        status,
                        "Failed to open the shared memory!");
        }

        if (status >= 0) {
            if (ftruncate(IpcMemMgr_module->shmfd, cfg.size) != 0) {
                status = IpcMemMgr_E_OSFAILURE;
                GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "IpcMemMgr_setup",
                            status,
                            "Failed to set size of shared memory!");
            } else {
                IpcMemMgr_module->shMemArea = (UInt *)mmap(0, cfg.size,
                                  PROT_READ | PROT_WRITE, MAP_SHARED,
                                  IpcMemMgr_module->shmfd, 0);
                if (IpcMemMgr_module->shMemArea == MAP_FAILED) {
                    status = IpcMemMgr_E_OSFAILURE;
                    GT_setFailureReason(curTrace,
                                GT_4CLASS,
                                "IpcMemMgr_setup",
                                status,
                                "Failed to map shared memory!");
                }
            }
        }

        if (status >= 0) {
            IpcMemMgr_module->global = (IpcMemMgr_Module_GlobalState *)
                           (IpcMemMgr_module->shMemArea);
            IpcMemMgr_module->global->refCount++;
            if (IpcMemMgr_module->global->refCount <= 1) {
                status = IpcMemMgr_E_INVALIDSTATE;
                GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "IpcMemMgr_setup",
                            status,
                            "Failed to map shared memory!");
            } else {
                Osal_printf("IpcMemMgr_setup successful.\n");
                IpcMemMgr_module->buf = (Ptr)IpcMemMgr_module->global->bufPtr;
                IpcMemMgr_module->bufSize =IpcMemMgr_module->global->head.size;

            }
        }
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_setup", status);

    return status;
}
#endif


#if 0
/* Function to destroy the system memory manager module */
Int
IpcMemMgr_destroy(Void) {
    Int    status = IpcMemMgr_S_SUCCESS;
    IArg   key;

    GT_0trace(curTrace, GT_ENTER, "IpcMemMgr_destroy");

    key = Gate_enterSystem();

    if (IpcMemMgr_module->refCount == 1) {
        IpcMemMgr_module->pid = 0;
        IpcMemMgr_module->pageSize = 0;
        IpcMemMgr_module->refCount = 0;
        IpcMemMgr_module->global->refCount--;
        if (IpcMemMgr_module->global->refCount <= 1) {
            GT_setFailureReason(curTrace,
                        GT_4CLASS,
                        "IpcMemMgr_destroy",
                        IpcMemMgr_E_INVALIDSTATE,
                        "IpcMemMgr Global state has gone to Invalid"
                        " state!!");
        } else {
            if (IpcMemMgr_module->shMemArea) {
                munmap(IpcMemMgr_module->shMemArea,
                       IpcMemMgr_module->global->regionSize);
                IpcMemMgr_module->shMemArea = NULL;
            }
            IpcMemMgr_module->global = NULL;
            close(IpcMemMgr_module->shmfd);
        }
    } else {
        IpcMemMgr_module->refCount--;
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_destroy", status);

    return status;
}
#endif


#if 0
/* Returns the requested free chunk  */
static IpcMemMgr_AllocChunk *
_IpcMemMgr_getFreeChunk(UInt32 size) {
    UInt32                 id          = 0;
    UInt32                 chunkSize   = 0;
    UInt32                 nextId      = 0;
    IpcMemMgr_AllocChunk * allocator   = NULL;
    IpcMemMgr_AllocChunk * freeChunk   = NULL;
    static UInt32          tableSizePg = 0;

    /* Not doing any state error checking since this is an internal function */
    GT_assert(curTrace, (size % PG_SIZE_4K == 0));

    allocator = (IpcMemMgr_AllocChunk *)
            IpcMemMgr_SHP2CHUNK((UInt32)IpcMemMgr_module->global->allocator);
    if (tableSizePg == 0) {
        tableSizePg = ((IpcMemMgr_module->global->regionSize - \
                IpcMemMgr_module->global->reservedSize) / PG_SIZE_4K);
    }

    if (size > IpcMemMgr_module->global->freeSize) {
        /* Find the largest free region (coalesce during the traversal) */
        while (id < tableSizePg) {
            chunkSize = allocator [id].chunkSizePg;
            nextId = id + chunkSize;
            if (allocator [id].inUse == 0) {
                /* Coalesce, if possible */
                if (nextId < tableSizePg && allocator [nextId].inUse == 0) {
                    allocator [id].chunkSizePg += allocator [nextId].chunkSizePg;
                    allocator [nextId].chunkSizePg = 0;
                    allocator [nextId].allocIndex = 0;
                    continue;
                }
                chunkSize *= PG_SIZE_4K;
                if (chunkSize > IpcMemMgr_module->global->freeSize) {
                    IpcMemMgr_module->global->freeChunk = id;
                    IpcMemMgr_module->global->freeSize = chunkSize;
                }
            }
            id = nextId;
        }
    }
    if (size <= IpcMemMgr_module->global->freeSize) {
        freeChunk = allocator + IpcMemMgr_module->global->freeChunk;
        IpcMemMgr_module->global->freeChunk += (size / PG_SIZE_4K);
        IpcMemMgr_module->global->freeSize -= size;
    }

    return freeChunk;
}
#endif


/*
 *  ======== IpcMemMgr_alloc ========
 *  Allocate anonymous shared memory.
 */
Ptr IpcMemMgr_alloc(SizeT size)
{
    Int                     status = IpcMemMgr_S_SUCCESS;
//  Ptr                     buf = NULL;
//  IpcMemMgr_AllocChunk  * allocatedChunk = NULL;
    IArg                    key;
//  UInt32                  reqSize;
//  UInt32                  reqSizePg;
//  Ptr virtAddrAllocator;

    Char             * allocAddr = NULL;
    IpcMemMgr_Header * prevHeader;
    IpcMemMgr_Header * newHeader;
    IpcMemMgr_Header * curHeader;
    UInt32           curSize;
    UInt32           adjSize;
    UInt32           remainSize; /* free memory after allocated memory */
    UInt32           adjAlign;
    UInt32           offset;
//  UInt32           defMinAlign;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_alloc", size);

//    key = Gate_enterSystem();

    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_alloc",
                    status,
                    "Module is in invalid state!");
    }
#if 0
    else if (IpcMemMgr_module->global->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_alloc",
                    status,
                    "Global Module is in invalid state!");
    }
#endif
    else if (size == 0) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_alloc",
                    status,
                    "Size passed is invalid!");
    }
#if 0
    else if (memType != IpcMemMgr_ALLOCTYPE_PSHMEM) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_alloc",
                    status,
                    "Only Linux Shared Memory supported currently!");
    }
#endif
    else {

#if 0
        reqSize = IpcMemMgr_ALIGN(size, PG_SIZE_4K);
        allocatedChunk = _IpcMemMgr_getFreeChunk(reqSize);
        if (allocatedChunk != NULL) {

            virtAddrAllocator = IpcMemMgr_SHP2CHUNK(
                                   (UInt32)IpcMemMgr_module->global->allocator);
            printf("virtAddrAllocator 0x%x \n",virtAddrAllocator);
            printf("IpcMemMgr_module->global->reservedSize 0x%x \n", IpcMemMgr_module->global->reservedSize);

            buf = (Ptr)(((Ptr)allocatedChunk - /*IpcMemMgr_module->global->allocator*/
                        virtAddrAllocator)
                    * PG_SIZE_4K) + IpcMemMgr_module->global->reservedSize;
            /* Calculate the number entries to use */
            reqSizePg = reqSize / PG_SIZE_4K;
            if (reqSize < allocatedChunk->chunkSizePg) {
                /* Mark remainder of free region */
                allocatedChunk[reqSizePg].inUse = 0;
                allocatedChunk[reqSizePg].chunkSizePg = \
                                    allocatedChunk->chunkSizePg - reqSizePg;
                allocatedChunk[reqSizePg].allocIndex = 0xFFF;
            }
            /*  get_region will return first fit chunk. But we only use what
            is requested. */
            allocatedChunk->inUse = 1;
            allocatedChunk->chunkSizePg = reqSizePg;
            allocatedChunk->allocIndex = 0xFFF;
            /* Return the chunk's starting address */
            buf = IpcMemMgr_getPtr((UInt32)buf);
        }
#endif
        adjSize = (UInt32) size;

        /* Make size requested a multiple of obj->minAlign */
        if ((offset = (adjSize & (IpcMemMgr_module->global->minAlign - 1)))
            != 0) {
            adjSize = adjSize + (IpcMemMgr_module->global->minAlign - offset);
        }

        /*
         *  Make sure the alignment is at least as large as obj->minAlign
         *  Note: adjAlign must be a power of 2 (by function constraint) and
         *  obj->minAlign is also a power of 2,
         */
//      adjAlign = reqAlign;
        adjAlign = 0; /* TODO */
        if (adjAlign == 0) {
            adjAlign =  IpcMemMgr_module->global->minAlign;
        }

        if (adjAlign & (IpcMemMgr_module->global->minAlign - 1)) {
            /* adjAlign is less than obj->minAlign */
            adjAlign = IpcMemMgr_module->global->minAlign;
        }

        /* No need to Cache_inv Attrs- 'head' should be constant */
        prevHeader = (IpcMemMgr_Header *) &(IpcMemMgr_module->global->head);

        key = Gate_enterSystem ();

        /*
         *  The block will be allocated from curHeader. Maintain a pointer to
         *  prevHeader so prevHeader->next can be updated after the alloc.
         */
        curHeader = (IpcMemMgr_Header *) IpcMemMgr_getPtr (prevHeader->next);
        /* A1 */

        /* Loop over the free list. */
        while (curHeader != NULL) {

            curSize = curHeader->size;

            /*
             *  Determine the offset from the beginning to make sure
             *  the alignment request is honored.
             */
            offset = (UInt32)curHeader & (adjAlign - 1);
            if (offset) {
                offset = adjAlign - offset;
            }

            /* Internal Assert that offset is a multiple of obj->minAlign */
            if (((offset & (IpcMemMgr_module->global->minAlign - 1)) != 0)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "IpcMemMgr_alloc",
                                     IpcMemMgr_E_FAIL,
                                     "offset is not a multiple of"
                                     " IpcMemMgr_module->global->minAlign!");
            }
            else {
                /* big enough? */
                if (curSize >= (adjSize + offset)) {
                    /* Set the pointer that will be returned.
                     * Alloc from front
                     */
                    allocAddr = (Char *) ((UInt32) curHeader + offset);

                    /*
                     *  Determine the remaining memory after the allocated block.
                     *  Note: this cannot be negative because of above
                     *  comparison.
                     */
                    remainSize = curSize - adjSize - offset;

                    /* Internal Assert that remainSize is a multiple of
                     * obj->minAlign
                     */
                    if (((   remainSize
                           & (IpcMemMgr_module->global->minAlign - 1)) != 0)) {
                        allocAddr = (UInt32) NULL;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "IpcMemMgr_alloc",
                                             IpcMemMgr_E_FAIL,
                                             "remainSize is not a multiple of"
                                             "IpcMemMgr_module->global->minAlign!");
                        break;
                    }
                    else {
                        /*
                         *  If there is memory at the beginning (due to alignment
                         *  requirements), maintain it in the list.
                         *
                         *  offset and remainSize must be multiples of
                         *  sizeof(HeapMemMP_Header). Therefore the address of the newHeader
                         *  below must be a multiple of the sizeof(HeapMemMP_Header), thus
                         *  maintaining the requirement.
                         */
                        if (offset) {
                            /* Adjust the curHeader size accordingly */
                            curHeader->size = offset; /* B2 */
                            /* Cache wb at end of this if block */

                            /*
                             *  If there is remaining memory, add into the free list.
                             *  Note: no need to coalesce and we have HeapMemMP locked so
                             *        it is safe.
                             */
                            if (remainSize) {
                                newHeader = (IpcMemMgr_Header *)
                                            ((UInt32) allocAddr + adjSize);

                                /* curHeader has been inv at top of 'while' loop */
                                newHeader->next = curHeader->next;  /* B1 */
                                newHeader->size = remainSize;       /* B1 */

                                curHeader->next = IpcMemMgr_getOffset
                                                                (newHeader);
                                GT_assert (curTrace,
                                           (    curHeader->next
                                            !=  NULL));
                            }
                        }
                        else {
                            /*
                             *  If there is any remaining, link it in,
                             *  else point to the next free block.
                             *  Note: no need to coalesce and we have HeapMemMP locked so
                             *        it is safe.
                             */
                            if (remainSize) {
                                newHeader = (IpcMemMgr_Header *)
                                            ((UInt32) allocAddr + adjSize);

                                newHeader->next  = curHeader->next; /* A2, B3  */
                                newHeader->size  = remainSize;      /* B3      */

                                /* B4 */
                                prevHeader->next = IpcMemMgr_getOffset (
                                                                    newHeader);
                            }
                            else {
                                /* curHeader has been inv at top of 'while' loop */
                                prevHeader->next = curHeader->next; /* A2, B4 */
                            }
                        }
                    }

                    /* Success, return the allocated memory */
                    break;

                }
                else {
                    prevHeader = curHeader;
                    curHeader = IpcMemMgr_getPtr (curHeader->next);
                }
            }
        }

        memset(allocAddr, 0,size);

        Gate_leaveSystem(key);
    }

    GT_1trace (curTrace, GT_LEAVE, "IpcMemMgr_alloc", allocAddr);

    return allocAddr;
}

/*
 *  ======== IpcMemMgr_free ========
 *  Free anonymous memory.
 */
Void IpcMemMgr_free(Ptr addr, SizeT size)
{
    Int               status = IpcMemMgr_S_SUCCESS;
    IArg              key;
    IpcMemMgr_Header * curHeader = NULL;
    IpcMemMgr_Header * newHeader = NULL;
    IpcMemMgr_Header * nextHeader= NULL;
    SizeT              offset;

//  GT_3trace(curTrace, GT_ENTER, "IpcMemMgr_free", addr, type, size);
    GT_2trace(curTrace, GT_ENTER, "IpcMemMgr_free: addr=0x%08x, size=0x%x",
        addr, size);

    key = Gate_enterSystem();

    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_free",
                    status,
                    "Module is in invalid state!");
    }
#if 0
    else if (IpcMemMgr_module->global->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_free",
                    status,
                    "Module is in invalid state!");
    }
#endif
    else if (addr == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_free",
                    status,
                    "Pointer passed is NULL!");
    } else if (size == 0) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace,
                    GT_4CLASS,
                    "IpcMemMgr_free",
                    status,
                    "Size is invalid!");
    } else {
#if 0
        /* Type translation */
        if (type == IpcMemMgr_FREETYPE_PSHMEM) {
            status = IpcMemMgr_freeShmemHeap(buf, size);
            if (status < 0) {
                status = IpcMemMgr_E_FAIL;
                GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "IpcMemMgr_free",
                            status,
                            "IpcMemMgr_freeShMemHeap failed!");
            }
        } else {
            args.func.free.size = size;
            args.func.free.vaddr = buf;
            ret = ioctl(IpcMemMgr_module->drvHandle, type, &args);
            if (ret < 0) {
                status = IpcMemMgr_E_OSFAILURE;
                GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "IpcMemMgr_free",
                            status,
                            "A OS call failed!");
            }
        }
#endif

        GT_assert (curTrace, ((UInt32)addr %
                               IpcMemMgr_module->global->minAlign == 0));

        /*
         * obj->attrs never changes, doesn't need Gate protection
         * and Cache invalidate
         */
        curHeader = (IpcMemMgr_Header *) &(IpcMemMgr_module->global->head);

        /* Restore size to actual allocated size */
        offset = size & (IpcMemMgr_module->global->minAlign - 1);
        if (offset != 0) {
            size += IpcMemMgr_module->global->minAlign - offset;
        }

        newHeader = (IpcMemMgr_Header *) addr;

        nextHeader = IpcMemMgr_getPtr (curHeader->next);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Make sure the entire buffer is in the range of the heap. */
        if (EXPECT_FALSE (!(    ((SizeT) newHeader >= (SizeT) IpcMemMgr_module->buf)
                            && (   (SizeT) newHeader + size
                                <= (SizeT) IpcMemMgr_module->buf + IpcMemMgr_module->bufSize)))) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "IpcMemMgr_free",
                             status,
                             "Entire buffer is not in the range of the heap!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Go down freelist and find right place for buf */
            while ((nextHeader != NULL) && (nextHeader < newHeader)) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                /* Make sure the addr is not in this free block */
                if (EXPECT_FALSE (    (SizeT) newHeader
                                  < ((SizeT) nextHeader + nextHeader->size))) {
                    /* A2 */
                    status = IpcMemMgr_E_INVALIDSTATE;
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "IpcMemMgr_free",
                                     status,
                                     "Address is in this free block");
                    break;
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    curHeader = nextHeader;
                    /* A2 */
                    nextHeader = IpcMemMgr_getPtr (nextHeader->next);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_TRUE (status >= 0)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* B2 */
                newHeader->next = IpcMemMgr_getOffset (nextHeader);
                newHeader->size = size;

                /* B1, A1 */
                curHeader->next = IpcMemMgr_getOffset (newHeader);

                /* Join contiguous free blocks */
                if (nextHeader != NULL) {
                    /*
                     *  Verify the free size is not overlapping. Not all cases
                     *  are detectable, but it is worth a shot. Note: only do
                     *  this assert if nextHeader is non-NULL.
                     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (  ((SizeT) newHeader + size)
                                      > (SizeT) nextHeader)) {
                        /* A2 */
                        status = IpcMemMgr_E_INVALIDSTATE;
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "IpcMemMgr_free",
                                         status,
                                         "Free size is overlapping");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Join with upper block */
                        if (((UInt32) newHeader + size) == (UInt32)nextHeader) {
                            newHeader->next = nextHeader->next; /* A2, B2 */
                            newHeader->size += nextHeader->size; /* A2, B2 */
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_TRUE (status >= 0)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /*
                     *  Join with lower block. Make sure to check to see if not the
                     *  first block. No need to invalidate attrs since head shouldn't change.
                     */
                    if (   (curHeader != &IpcMemMgr_module->global->head)
                        && (    ((UInt32) curHeader + curHeader->size)
                            ==  (UInt32) newHeader)) {
                        /*
                         * Don't Cache_inv newHeader since newHeader has
                         * data that hasn't been written back yet (B2)
                         */
                        curHeader->next = newHeader->next; /* B1, B2 */
                        curHeader->size += newHeader->size; /* B1, B2 */
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    Gate_leaveSystem (key);

//  GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_free", status);
    GT_0trace(curTrace, GT_LEAVE, "IpcMemMgr_free");

//  return status;
    return;
}

/*
 *  ======== IpcMemMgr_getPtr ========
 *  Convert the given shared memory offset into a local address pointer.
 */
Ptr IpcMemMgr_getPtr(UInt32 offset)
{
    return((Ptr)((UInt32)(IpcMemMgr_module->shMemArea) + offset));
}

/*
 *  ======== IpcMemMgr_getOffset ========
 *  Convert the given local address pointer into an offset from the
 *  shared memory base address. This must be implemented in the local module.
 */
UInt32 IpcMemMgr_getOffset(Ptr vaddr)
{
    return((UInt32)(vaddr) - (UInt32)(IpcMemMgr_module->shMemArea));
}

/*
 *  ======== IpcMemMgr_acquire ========
 *  Allocate memory by name.
 */
Int IpcMemMgr_acquire(String name, SizeT size, Ptr *addr, Bool *newAlloc)
{
    Int                     status;
    Ptr                     buf;
    Int                     i;
    IpcMemMgr_ShmemChunk *  entry;
    Bool                    found;
    IArg                    key;
    SizeT                   nameLen;
    SizeT                   len;

    GT_2trace(curTrace, GT_ENTER, "IpcMemMgr_acquire", name, size);

    status = IpcMemMgr_S_SUCCESS;
    buf = NULL;
    found = FALSE;

    key = Gate_enterSystem();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
            status, "invalid module state");
    }
    else if (size == 0) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
            status, "invalid size, must be greater then zero");
    }
    else if (name == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
            status, "name is NULL");
    }
    else if (String_len(name) >= IpcMemMgr_MAX_NAMELEN) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
            status, "name too long, must be <= IpcMemMgr_MAX_NAMELEN");
    }
    else if (addr == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
            status, "addr is NULL");
    }
#endif

    /* check if named buffer already exists */
    if (status == IpcMemMgr_S_SUCCESS) {
        nameLen = String_len(name);

        for (i = 0; i < IpcMemMgr_MAXNAMEDBUFFERS; i++) {
            entry = &IpcMemMgr_module->allocNamedList[i];

            if (entry->used == 1) {
                len = (nameLen > entry->nameLen ? nameLen : entry->nameLen);
                
                if (String_ncmp(name, entry->name, len) == 0) {
                    if (entry->size == size) {
                        entry->refCount++;
                        *addr = IpcMemMgr_getPtr(entry->offset);
                        if (newAlloc != NULL) {
                            *newAlloc = FALSE;
                        }
                        found = TRUE;
                    }
                    else {
                        status = IpcMemMgr_E_WRONGSIZE;
                    }
                    /* must stop looking because name matched */
                    break;
                }
            }
        }

        if (i == IpcMemMgr_MAXNAMEDBUFFERS) {
            found = FALSE;
        }
    }

    /* find empty slot for new buffer */
    if ((status == IpcMemMgr_S_SUCCESS) && !found) {
        for (i = 0; i < IpcMemMgr_MAXNAMEDBUFFERS; i++) {
            entry = &IpcMemMgr_module->allocNamedList[i];
            if (entry->used == 0) {
                break;
            }
        }

        if (i == IpcMemMgr_MAXNAMEDBUFFERS) {
            status = IpcMemMgr_E_TOOMANYNAMEDALLOCS;
        }
    }

    /* allocate buffer from free list */
    if ((status == IpcMemMgr_S_SUCCESS) && !found) {
        buf = IpcMemMgr_alloc(size);

        if (buf == NULL) {
            status = IpcMemMgr_E_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
                status, "alloc call failed");
        }
    }

    /* store buffer in named list */
    if ((status == IpcMemMgr_S_SUCCESS) && !found) {
        entry->used = 1;
        entry->refCount = 1;
        entry->size = size;
        entry->offset = IpcMemMgr_getOffset(buf);
        entry->nameLen = nameLen;
        String_cpy(entry->name, name);

        /* set return values */
        *addr = buf;
        if (newAlloc != NULL) {
            *newAlloc = TRUE;
        }
    }

    Gate_leaveSystem(key);

    GT_3trace(curTrace, GT_LEAVE, "IpcMemMgr_acquire", status, addr, newAlloc);

    return(status);
}

/*
 *  ======== IpcMemMgr_release ========
 *  Free memory by name.
 */
Int IpcMemMgr_release(String name)
{
    Int                     status;
    IpcMemMgr_ShmemChunk *  entry;
    Ptr                     buf;
    Int                     i;
    SizeT                   nameLen;
    SizeT                   len;
//  IArg key;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_release", name);

    status = IpcMemMgr_S_SUCCESS;

//  key = Gate_enterSystem();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_release",
            status, "invalid module state");
    }
    else if (name == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_release",
            status, "name is NULL");
    }
#endif

    /* search for named buffer */
    if (status == IpcMemMgr_S_SUCCESS) {
        nameLen = String_len(name);

        for (i = 0; i < IpcMemMgr_MAXNAMEDBUFFERS; i++) {
            entry = &IpcMemMgr_module->allocNamedList[i];

            if (entry->used == 1) {
                len = (nameLen > entry->nameLen ? nameLen : entry->nameLen);

                if (String_ncmp(name, entry->name, len) == 0) {
                    break;
                }
            }
        }

        if (i == IpcMemMgr_MAXNAMEDBUFFERS) {
            status = IpcMemMgr_E_NOTFOUND;
        }
    }

    /* decrement ref count and free if zero */
    if (status == IpcMemMgr_S_SUCCESS) {
        entry->refCount--;

        if (entry->refCount == 0) {
            buf = IpcMemMgr_getPtr(entry->offset);
            IpcMemMgr_free(buf, entry->size);
            entry->used = 0;
        }
    }

//  Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_release: [0x%08x]", status);

    return(status);
}

/*
 *  ======== IpcMemMgr_enterGate ========
 */
IArg IpcMemMgr_enterGate(Void)
{
    IArg key;

    key = GateProcess_enter(IpcMemMgr_module->gate);

    return(key);
}

/*
 *  ======== IpcMemMgr_leaveGate ========
 */
Void IpcMemMgr_leaveGate(IArg key)
{

    GateProcess_leave(IpcMemMgr_module->gate, key);

    return;
}
