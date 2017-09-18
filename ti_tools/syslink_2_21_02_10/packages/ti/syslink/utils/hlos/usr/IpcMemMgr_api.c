/*
 *  @file   IpcMemMgr_api.c
 *
 *  @brief      IPC memory manager API front-end
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
#include <ti/syslink/osal/OsalProcess.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateProcess.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>

/* Module headers */
#include <ti/syslink/utils/IpcMemMgr.h>
#include <ti/syslink/inc/IpcMemMgrDrvDefs.h>
#include <ti/syslink/inc/usr/IpcMemMgrDrv.h>


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


#define IpcMemMgr_MAX_ALLOCATIONS   256u /* Number of name based allocations */
//#define IpcMemMgr_MAX_NAMELEN       80u
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
    volatile UInt32 used;
    /* Flag to indicate free block */
    volatile UInt32 offset;
    /* offset within the inter-process shared memory heap buffer area */
    Char   name [IpcMemMgr_MAX_NAMELEN];
    /* Name of the memory chunk */
    volatile UInt32 size;
    /* Size of the memory chunk */
    volatile Int32  refCount;
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

#if 0
/* Global module state structure visible to all processes */
typedef struct IpcMemMgr_Module_GlobalState_tag {
    volatile UInt32                  refCount;
    /* Global refcount */
    volatile UInt32                  regionSize;
    /* inter-process shared memory area size */
    volatile UInt32                  reservedSize;
    /* Reserved size at the beginning of the inter-process shared memory area */
    volatile UInt32                  daemonPid;
    /* PID of the master process for inter-process shared memory area*/
    IpcMemMgr_Config                cfg;
    /* Config information of the module */
    volatile UInt32                  minAlign;
    volatile UInt32                  maxAllocIndex;
    /* Max index id of a named object in the named list array */
    volatile UInt32                  minFreeIndex;
    /* Min index id at which a free named object element exists */
    IpcMemMgr_ShmemChunk            allocNamedList [IpcMemMgr_MAX_ALLOCATIONS];
    /* List of allocated chunk blocks */
    volatile UInt32                  freeChunk;
    /* Index of largest free chunk */
    volatile UInt32                  freeSize;
    /* Size of largest free chunk */
    volatile IpcMemMgr_AllocChunk * allocator;
    /* Array of bit map for memory allocator */

    IpcMemMgr_SHPtr  bufPtr;
    /* Inter process shared memory managed by the module  */
    IpcMemMgr_Header    head;
    /* HeapMemMP      head */

} IpcMemMgr_Module_GlobalState;
#endif

/* Module state structure visible to local processes */
typedef struct IpcMemMgr_Module_State_tag {
    UInt32                          refCount;   /* module reference count   */
    GateProcess_Handle              gate;       /* module gate */
    Int                             shmfd;      /* shared memory object     */
    Ptr                             shMemArea;  /* shared memory address    */
    UInt32                          pageSize;
    IpcMemMgr_Config                cfg;        /* runtime config           */
//  IpcMemMgr_Module_GlobalState *  global;
    Char                         *  buf;
    UInt32                          bufSize;
    UInt32                          pid;
} IpcMemMgr_Module_State;


/* =============================================================================
 * Globals
 * =============================================================================
 */

/* static */
IpcMemMgr_Module_State   IpcMemMgr_moduleState =
{
    .refCount = 0,
    .pid = 0u
};

/* static */
IpcMemMgr_Module_State * const IpcMemMgr_module = &IpcMemMgr_moduleState;


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
    Int                     status = IpcMemMgr_S_SUCCESS;
    IpcMemMgrDrv_CmdArgs    cmdArgs;
    IArg                    key;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_getConfig:", cfg);
    GT_assert(curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_getConfig",
            IpcMemMgr_E_FAIL, "cfg is null");
        return;
    }
#endif
    key = Gate_enterSystem();

    if (IpcMemMgr_module->refCount == 0) {
        /* temporarily open the handle to get the configuration */
        status = IpcMemMgrDrv_open();

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_getConfig",
                status, "Failed to open driver handle!");
        }
        else {
            /* get runtime config from the driver */
            cmdArgs.args.getConfig.config = cfg;
            status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_GETCONFIG, &cmdArgs);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS,
                    "IpcMemMgr_getConfig", status,
                    "API (through IOCTL) failed on kernel-side!");
            }
        }

        /* close the driver handle regardless of error */
        IpcMemMgrDrv_close();
    }
    else {
        /* already have runtime config in module */
        Memory_copy(cfg, &IpcMemMgr_module->cfg, sizeof(IpcMemMgr_Config));
    }

    Gate_leaveSystem(key);

    GT_0trace(curTrace, GT_LEAVE, "IpcMemMgr_getConfig");
}

/*
 *  ======== IpcMemMgr_setup ========
 */
Int IpcMemMgr_setup(IpcMemMgr_Config *config)
{
    Int                     status = IpcMemMgr_S_SUCCESS;
    IpcMemMgrDrv_CmdArgs    cmdArgs;
    IArg                    key;
    Ptr                     shMemAddr;
    GateProcess_Handle      gate;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_setup:", config);

    key = Gate_enterSystem();
    IpcMemMgr_module->refCount++;

    /* if already setup, return runtime config */
    if (IpcMemMgr_module->refCount > 1) {
        status = IpcMemMgr_S_ALREADYSETUP;
        GT_1trace(curTrace, GT_1CLASS,
            "IpcMemMgr module already initialized, refCount=%d",
            IpcMemMgr_module->refCount);

        /* return the rutime config */
        if (config != NULL) {
            IpcMemMgr_getConfig(config);
        }
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        /* open the driver */
        status = IpcMemMgrDrv_open();

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "Failed to open driver handle!");
        }
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        /* setup driver with requested config, returns actual runtime config */
        cmdArgs.args.setup.config = &IpcMemMgr_module->cfg;
        status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_SETUP, &cmdArgs);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "API (through IOCTL) failed on kernel-side!");
        }
        else if (status == IpcMemMgr_S_ALREADYSETUP) {
            GT_0trace(curTrace, GT_1CLASS,
                "IpcMemMgr driver module already initialized");
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    /* return runtime config to caller */
    if (status == IpcMemMgr_S_SUCCESS) {
        if (config != NULL) {
            IpcMemMgr_getConfig(config);
        }
    }

//  IpcMemMgr_module->pageSize = PG_SIZE_4K;
//  IpcMemMgr_module->pid = OsalProcess_getPid();

    /* open the shared memory object */
    if (status == IpcMemMgr_S_SUCCESS) {
        IpcMemMgr_module->shmfd = shm_open(IpcMemMgr_SHAREDMEMNAME,
            O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH);

        if (IpcMemMgr_module->shmfd < 0) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "Failed to open the shared memory!");
        }
    }

#if 0
    if (status == IpcMemMgr_S_SUCCESS) {
        if (ftruncate(IpcMemMgr_module->shmfd, cfg.size) != 0) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "Failed to set size of shared memory!");
        }
    }
#endif

    /* map shared memory into local address space */
    if (status == IpcMemMgr_S_SUCCESS) {
        IpcMemMgr_module->shMemArea = (UInt *)mmap(0,
            IpcMemMgr_module->cfg.size, PROT_READ | PROT_WRITE, MAP_SHARED,
            IpcMemMgr_module->shmfd, 0);

        if (IpcMemMgr_module->shMemArea == MAP_FAILED) {
            status = IpcMemMgr_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "Failed to map shared memory!");
        }
    }

#if 0
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
#endif

    /* create the process gate */
    if (status == IpcMemMgr_S_SUCCESS) {
        shMemAddr = IpcMemMgr_getPtr(IpcMemMgr_module->cfg.gateOffset);

        gate = GateProcess_open(shMemAddr);

        if (gate == NULL) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_setup",
                status, "GateProcess_open failed");
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
    Int    status = IpcMemMgr_S_SUCCESS;
    IArg   key;

    GT_0trace(curTrace, GT_ENTER, "IpcMemMgr_destroy:");

    key = Gate_enterSystem();

    if (IpcMemMgr_module->refCount <= 0) {
        Gate_leaveSystem(key);
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_destroy",
            status, "Module was not initialized!");
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        /* dec refCount, do nothing if not last user */
        IpcMemMgr_module->refCount--;

        if (IpcMemMgr_module->refCount > 0) {
            status = IpcMemMgr_S_NOTLAST;
        }
    }

    /* close the process gate */
    if (status == IpcMemMgr_S_SUCCESS) {
        status = GateProcess_close(&IpcMemMgr_module->gate);

        if (status <= 0) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_destroy",
                status, "GateProcess_close failed");
        }
        else {
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        /* destroy the driver */
        status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_DESTROY, NULL);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_destroy",
                status, "API (through IOCTL) failed on kernel-side!");
        }
        else if (status == IpcMemMgr_S_NOTLAST) {
            GT_0trace(curTrace, GT_1CLASS,
                "IpcMemMgr driver module still in use");
            status = IpcMemMgr_S_SUCCESS;
        }

        /* close the driver handle. */
        IpcMemMgrDrv_close();
    }

    if (status == IpcMemMgr_S_SUCCESS) {

        IpcMemMgr_module->pid = 0;
        IpcMemMgr_module->pageSize = 0;

        /* unmap shared memory from process address space */
        if (IpcMemMgr_module->shMemArea != NULL) {
            munmap(IpcMemMgr_module->shMemArea, IpcMemMgr_module->cfg.size);
            IpcMemMgr_module->shMemArea = NULL;
        }

//      IpcMemMgr_module->global = NULL;

        /* dec ref count on shmdf, don't use shm_unlink */
        close(IpcMemMgr_module->shmfd);
        IpcMemMgr_module->shmfd = -1;
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_destroy", status);

    return(status);
}


#if 0 /* TODO */
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
 *  Allocate anonymous shared memory. Front-end proxy.
 */
Ptr IpcMemMgr_alloc(SizeT size)
{
    IpcMemMgrDrv_CmdArgs    cmdArgs;
    Int                     status;
    IArg                    key;
    Ptr                     buf;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_alloc:", size);

    status = IpcMemMgr_S_SUCCESS;

    key = IpcMemMgr_enterGate();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_alloc",
                status, "Module is in invalid state");
    }
    else if (size == 0) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_alloc",
                status, "Size passed is invalid");
    }
#endif

    /* marshal the arguments into the message */
    if (status == IpcMemMgr_S_SUCCESS) {
        cmdArgs.args.alloc.size = size;

        /* send message to SysLink Driver */
        status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_ALLOC, &cmdArgs);

        if (status < 0) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_alloc",
                    status, "IpcMemMgrDrv_ioctl() failed");
        }
        else {
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    /* unmarshal arguments from message */
    if (status == IpcMemMgr_S_SUCCESS) {
        buf = IpcMemMgr_getPtr(cmdArgs.args.alloc.offset);
    }
    else {
        buf = NULL;
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_alloc", buf);

    return(buf);
}

/*
 *  ======== IpcMemMgr_free ========
 *  Free anonymous memory. Front-end proxy.
 */
Void IpcMemMgr_free(Ptr addr, SizeT size)
{
    IpcMemMgrDrv_CmdArgs    cmdArgs;
    Int                     status;
    IArg                    key;

    GT_2trace(curTrace, GT_ENTER, "IpcMemMgr_free:", addr, size);

    status = IpcMemMgr_S_SUCCESS;

    key = IpcMemMgr_enterGate();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_free",
                status, "Module is in invalid state");
    }
    else if (addr == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_free",
                status, "Pointer passed is NULL");
    }
    else if (size == 0) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_free",
                status, "Size is invalid");
    }
#endif

    if (status == IpcMemMgr_S_SUCCESS) {
        cmdArgs.args.free.offset = IpcMemMgr_getOffset(addr);
        cmdArgs.args.free.size = size;

        status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_FREE, &cmdArgs);

        if (status < 0) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_free",
                    status, "IpcMemMgrDrv_ioctl() failed");
        }
        else {
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_free", status);

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
 *  Alloc memory by name. Front-end proxy.
 */
Int IpcMemMgr_acquire(String name, SizeT size, Ptr *pAddr, Bool *pNewAlloc)
{
    IpcMemMgrDrv_CmdArgs    cmdArgs;
    Int                     status;
    IArg                    key;

    GT_2trace(curTrace, GT_ENTER, "IpcMemMgr_acquire:", name, size);

    status = IpcMemMgr_S_SUCCESS;

    key = IpcMemMgr_enterGate();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
                status, "Module is in invalid state");
    }
    else if (name == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
                status, "Name passed is NULL");
    }
#endif

    if (status == IpcMemMgr_S_SUCCESS) {
        cmdArgs.args.acquire.name = name;
        cmdArgs.args.acquire.nameSize = String_len(name) + 1;
        cmdArgs.args.acquire.size = size;

        /* send message to SysLink Driver */
        status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_ACQUIRE, &cmdArgs);

        if (status < 0) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_acquire",
                    status, "IpcMemMgrDrv_ioctl() failed");
        }
        else {
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    if (status == IpcMemMgr_S_SUCCESS) {
        *pAddr = IpcMemMgr_getPtr(cmdArgs.args.acquire.offset);
        if (pNewAlloc != NULL) {
            *pNewAlloc = cmdArgs.args.acquire.newAlloc;
        }
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_acquire", status);

    return(status);
}

/*
 *  ======== IpcMemMgr_release ========
 *  Free memory by name. Front-end proxy.
 */
Int IpcMemMgr_release(String name)
{
    IpcMemMgrDrv_CmdArgs    cmdArgs;
    Int                     status;
    IArg                    key;

    GT_1trace(curTrace, GT_ENTER, "IpcMemMgr_release:", name);

    status = IpcMemMgr_S_SUCCESS;

    key = IpcMemMgr_enterGate();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (IpcMemMgr_module->refCount <= 0) {
        status = IpcMemMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_release",
                status, "Module is in invalid state");
    }
    else if (name == NULL) {
        status = IpcMemMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_release",
                status, "Name passed is NULL");
    }
#endif

    if (status == IpcMemMgr_S_SUCCESS) {
        cmdArgs.args.release.name = name;
        cmdArgs.args.release.nameSize = String_len(name) + 1;

        status = IpcMemMgrDrv_ioctl(CMD_IPCMEMMGR_RELEASE, &cmdArgs);

        if (status < 0) {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgr_release",
                    status, "IpcMemMgrDrv_ioctl() failed");
        }
        else {
            status = IpcMemMgr_S_SUCCESS;
        }
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgr_release:", status);

    return(status);
}

/*
 *  ======== IpcMemMgr_enterGate ========
 */
IArg IpcMemMgr_enterGate(Void)
{
    IArg key = 0;

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
