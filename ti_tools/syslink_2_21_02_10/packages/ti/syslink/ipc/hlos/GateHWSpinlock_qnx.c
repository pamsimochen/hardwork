/*
 *  @file   GateHWSpinlock_qnx.c
 *
 *  @brief      Gate based on Hardware SpinLock.
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

/* Utilities & OSAL headers */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/IpcMemMgr.h>
#include <ti/syslink/inc/IGateMPSupport.h>
#include <ti/syslink/inc/_GateMP_qnx.h>
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>

/* Module level headers */
#include <ti/syslink/utils/String.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/inc/GateHWSpinlock_qnx.h>


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/* GateHWSpinlock Module Global State */
typedef struct {
    Int32       refCount;
    UInt32      cfgOff;                 /* shmem offset to cfg struct */
    UInt32      reservedMaskAryOff;     /* track reserved locks */
} GateHWSpinlock_GlobalState;

/* GateHWSpinlock Module Local State */
typedef struct {
    Int32                           refCount;   /* reference count */
    GateHWSpinlock_GlobalState *    global;     /* va addr */
    GateHWSpinlock_Config *         cfg;        /* va addr */
    Bits32 *                        reservedMaskAry;
    UInt32 *                        baseAddr;   /* base addr lock registers */
} GateHWSpinlock_Module_State;

/* GateHWSpinlock instance object */
struct GateHWSpinlock_Object {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
    IOBJECT_SuperObject;       /* For inheritance for IObject */
    UInt                        lockNum;
    UInt                        nested;
    IGateProvider_Handle        localGate;
};


/* =============================================================================
 * Globals
 * =============================================================================
 */

static
GateHWSpinlock_Module_State GateHWSpinlock_state =
{
    .refCount = 0,
    .global = NULL,
    .cfg = NULL,
    .reservedMaskAry = NULL,
    .baseAddr = NULL,
};

static
GateHWSpinlock_Module_State *Mod = &GateHWSpinlock_state;

static
GateHWSpinlock_Params GateHWSpinlock_defInstParams =
{
    .resourceId = 0,
    .openFlag   = FALSE,
    .regionId   = 0,
    .sharedAddr = NULL
};

/* =============================================================================
 * APIS
 * =============================================================================
 */

/*!
 *  @brief      Setup the GateHWSpinlock module.
 *
 *              This function sets up the GateHWSpinlock module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then GateHWSpinlock_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call GateHWSpinlock_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional GateHWSpinlock module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         GateHWSpinlock_destroy, GateHWSpinlock_getConfig
 */
Int GateHWSpinlock_setup(Void)
{
    Int32               status = GateHWSpinlock_S_SUCCESS;
    IArg                key;
    Ptr                 ptr;
    SizeT               size;
    Bool                newGlobal;
    Memory_MapInfo      minfo;
    Memory_UnmapInfo    uminfo;

    GT_0trace(curTrace, GT_ENTER, "GateHWSpinlock_setup");

    key = IpcMemMgr_enterGate();

    Mod->refCount++;

    /* if already setup, then leave gate and return (success) */
    if (Mod->refCount > 1) {
        status = GateHWSpinlock_S_ALREADYSETUP;
        GT_1trace(curTrace, GT_2CLASS,
            "GateHWSpinlock module already initialized, refCount=%d",
            Mod->refCount);
    }

    /* acquire the global structure in shared memory */
    if (status == GateHWSpinlock_S_SUCCESS) {
        size = sizeof(GateHWSpinlock_GlobalState);

        status = IpcMemMgr_acquire(GateHWSpinlock_GLOBAL, size,
            &ptr, &newGlobal);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_setup",
                status, "IpcMemMgr_acquire(GateHWSpinlock_GLOBAL) failed");
            status = GateHWSpinlock_E_MEMORY;
        }
        else {
            Mod->global = ptr;
            status = GateHWSpinlock_S_SUCCESS;
        }
    }

    /* update the global structure reference count */
    if (status == GateHWSpinlock_S_SUCCESS) {
        if (newGlobal) {
            Mod->global->refCount = 1;
        }
        else {
            Mod->global->refCount++;
        }
    }

    /* cache the config params in global structure */
    if (status == GateHWSpinlock_S_SUCCESS) {
        if (newGlobal) {
            size = sizeof(GateHWSpinlock_Config);

            status =
                IpcMemMgr_acquire(GateHWSpinlock_CONFIG, size, &ptr, NULL);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_setup",
                    status, "IpcMemMgr_acquire(GateHWSpinlock_CONFIG) failed");
                status = GateHWSpinlock_E_FAIL;
            }
            else {
                Mod->cfg = ptr;
                Mod->global->cfgOff = IpcMemMgr_getOffset(ptr);
                status = GateHWSpinlock_S_SUCCESS;
            }
        }
        else {
            Mod->cfg = IpcMemMgr_getPtr(Mod->global->cfgOff);
        }
    }

    /* acquire the array of masks for tracking reserved locks */
    if (status == GateHWSpinlock_S_SUCCESS) {
        if (newGlobal) {
            size = ((Mod->cfg->numLocks/32) +
                (Mod->cfg->numLocks % 32 ? 1 : 0)) * sizeof(Bits32);

            Mod->reservedMaskAry = IpcMemMgr_alloc(size);

            if (Mod->reservedMaskAry == NULL) {
                status = GateHWSpinlock_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_setup",
                    status, "IpcMemMgr_alloc failed");
            }
            else {
                Mod->global->reservedMaskAryOff =
                    IpcMemMgr_getOffset(Mod->reservedMaskAry);
            }
        }
        else {
            Mod->reservedMaskAry =
                IpcMemMgr_getPtr(Mod->global->reservedMaskAryOff);
        }
    }

    /* map the hardware lock registers into the local address space */
    if (status == GateHWSpinlock_S_SUCCESS) {
        minfo.src = Mod->cfg->baseAddr;
        minfo.size = Mod->cfg->size;
        minfo.isCached = FALSE;

        status = Memory_map(&minfo);

        if (status < 0) {
           GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_setup",
                status, "Memory_map failed");
            status = GateHWSpinlock_E_OSFAILURE;
        }
        else {
            Mod->baseAddr = (UInt32 *)minfo.dst;
            status = GateHWSpinlock_S_SUCCESS;
        }
    }

    /* in failure case, release acquired resources in reverse order */
    if (status < 0) {

        /* release lock register mapping */
        if (Mod->baseAddr != NULL) {
            uminfo.addr = (UInt32)Mod->baseAddr;
            uminfo.size = Mod->cfg->size;
            uminfo.isCached = FALSE;
            Memory_unmap(&uminfo);
        }

        /* release mask array */
        if (Mod->reservedMaskAry != NULL) {
            if (newGlobal) {
                size = ((Mod->cfg->numLocks/32) +
                    (Mod->cfg->numLocks % 32 ? 1 : 0)) * sizeof(Bits32);
                IpcMemMgr_free(Mod->reservedMaskAry, size);
                Mod->global->reservedMaskAryOff = 0;
            }
            Mod->reservedMaskAry = NULL;
        }

        /* release the config shared memory */
        if (Mod->cfg != NULL) {
            if (newGlobal) {
                IpcMemMgr_release(GateHWSpinlock_CONFIG);
                Mod->global->cfgOff = 0;
            }
            Mod->cfg = NULL;
        }

        /* decrement the global structure reference count */
        Mod->global->refCount--;

        /* release the global structure shared memory */
        if (Mod->global != NULL) {
            IpcMemMgr_release(GateHWSpinlock_GLOBAL);
            Mod->global = NULL;
        }

        /* decrement the module reference count */
        Mod->refCount--;
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "GateHWSpinlock_setup: [0x%08x]", status);

    return(status);
}

/*!
 *  @brief      Function to destroy the GateHWSpinlock module.
 *
 *  @sa         GateHWSpinlock_setup
 */
Int GateHWSpinlock_destroy(Void)
{
    Int32               status = GateHWSpinlock_S_SUCCESS;
    IArg                key;
    SizeT               size;
    Bool                deleteGlobal;
    Memory_UnmapInfo    uminfo;

    GT_0trace(curTrace, GT_ENTER, "GateHWSpinlock_destroy");

    key = IpcMemMgr_enterGate();

    if (Mod->refCount <= 0) {
        status = GateHWSpinlock_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_destroy",
            status, "Module was not initialized");
    }

    /* dec module ref count, do nothing if not last user */
    if (status == GateHWSpinlock_S_SUCCESS) {
        Mod->refCount--;

        if (Mod->refCount > 0) {
            status = GateHWSpinlock_S_INUSE;
        }
    }

    if (status == GateHWSpinlock_S_SUCCESS) {

        /* decrement the global structure reference count */
        Mod->global->refCount--;
        deleteGlobal = Mod->global->refCount > 0 ? FALSE : TRUE;

        /* release lock register mapping */
        if (Mod->baseAddr != NULL) {
            uminfo.addr = (UInt32)Mod->baseAddr;
            uminfo.size = Mod->cfg->size;
            uminfo.isCached = FALSE;
            Memory_unmap(&uminfo);
        }

        /* release mask array */
        if (Mod->reservedMaskAry != NULL) {
            if (deleteGlobal) {
                size = ((Mod->cfg->numLocks/32) +
                    (Mod->cfg->numLocks % 32 ? 1 : 0)) * sizeof(Bits32);
                IpcMemMgr_free(Mod->reservedMaskAry, size);
                Mod->global->reservedMaskAryOff = 0;
            }
            Mod->reservedMaskAry = NULL;
        }

        /* release the config shared memory */
        if (Mod->cfg != NULL) {
            if (deleteGlobal) {
                IpcMemMgr_release(GateHWSpinlock_CONFIG);
                Mod->global->cfgOff = 0;
            }
            Mod->cfg = NULL;
        }

        /* release the global structure shared memory */
        if (Mod->global != NULL) {
            IpcMemMgr_release(GateHWSpinlock_GLOBAL);
            Mod->global = NULL;
        }
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "GateHWSpinlock_destroy: [0x%08x]", status);

    return(status);
}

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 */
UInt GateHWSpinlock_getNumInstances(Void)
{
    return(Mod->cfg->numLocks);
}

/*
 *  ======== GateHWSpinlock_locksinit ========
 *  Initialize the locks
 */
Void GateHWSpinlock_locksinit(Void)
{
    Int  i;

    for (i = 0; i < Mod->cfg->numLocks; i++) {
        if (Mod->reservedMaskAry[i >> 5] & (1 << (i % 32))) {
            /* don't reset reserved spinlocks */
            continue;
        }
        Mod->baseAddr[i] = 0;
    }
}

/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GateHWSpinlock_create
 */
/*
 *  ======== GateHWSpinlock_Params_init ========
 */
Void GateHWSpinlock_Params_init(GateHWSpinlock_Params *params)
{

    GT_1trace(curTrace, GT_ENTER,
        "GateHWSpinlock_Params_init: params=0x%08x", params);
    GT_assert(curTrace, (params != NULL));

    Memory_copy(params, &GateHWSpinlock_defInstParams,
        sizeof(GateHWSpinlock_Params));

    GT_0trace(curTrace, GT_LEAVE, "GateHWSpinlock_Params_init");
}

/*
 *  ======== GateHWSpinlock_Instance_init ========
 */
Int GateHWSpinlock_Instance_init(
        GateHWSpinlock_Object *         obj,
        IGateProvider_Handle            localGate,
        const GateHWSpinlock_Params *   params)
{
    Int status = 0;

    GT_0trace(curTrace, GT_ENTER, "GateHWSpinlock_Instance_init");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* check that module has been initialized */
    if (Mod->refCount <= 0) {
        status = GateHWSpinlock_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_Instance_init",
            status, "module is not initialized");
    }
#endif

    /* assert that params->resourceId is valid */
    if (params->resourceId >= Mod->cfg->numLocks) {
        status = 1;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_Instance_init",
            GateHWSpinlock_E_FAIL, "resourceId >= GateHWSpinlock_numLocks");
    }

    /* assert that localGate is non-null */
    if ((status == 0) && (localGate == NULL)) {
        status = 2;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_Instance_init",
            GateHWSpinlock_E_FAIL, "local gate must not be null");
    }

    /* create the local gate */
    if (status == 0) {
        IGateProvider_ObjectInitializer(obj, GateHWSpinlock);
        obj->localGate = localGate;
        obj->lockNum = params->resourceId;
        obj->nested = 0;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateHWSpinlock_Instance_init: [%d]", status);

    return(status);
}

/*
 *  ======== GateHWSpinlock_Instance_finalize ========
 */
Void GateHWSpinlock_Instance_finalize(GateHWSpinlock_Object *obj, Int status)
{
}

/*
 *  ======== GateHWSpinlock_enter ========
 */
IArg GateHWSpinlock_enter(GateHWSpinlock_Object *obj)
{
    volatile UInt32 *baseAddr = Mod->baseAddr;
    IArg key;

    key = IGateProvider_enter(obj->localGate);

    /* if gate already entered, just return with current key */
    obj->nested++;
    if (obj->nested > 1) {
        return(key);
    }

    /* enter the spinlock */
    while (1) {
        /* read the spinlock, returns non-zero when we get it */
        if (baseAddr[obj->lockNum] == 0) {
            break;
        }
        obj->nested--; /* restore state of delegate object */
        IGateProvider_leave(obj->localGate, key);

        /*  Need to leave and re-enter the local gate because it might
         *  be a Hwi gate which holds off interrupts while in the gate.
         *  This allows interrupts to be taken while we are waiting for
         *  the hardware spinlock to become available. Once we get the
         *  spinlock, then interrupts will be held off until client calls
         *  GateHWSpinlock_leave. Presumably, they keep the gate only for
         *  a short while. For example, an ISR wants to enter a GateMP
         *  instance to set a flag in SR#0, or walk a linked list stored
         *  in a shared region.
         */

        key = IGateProvider_enter(obj->localGate);
        obj->nested++; /* re-nest the gate */
    }

    return (key);
}

/*
 *  ======== GateHWSpinlock_setReserved ========
 */
Void GateHWSpinlock_setReserved(UInt32 lockNum)
{
    if (lockNum >= Mod->cfg->numLocks) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSpinlock_setReserved",
                             GateHWSpinlock_E_FAIL,
                             "lockNum is greater than the configured locks!");
    }
    else {
        Mod->reservedMaskAry[lockNum >> 5] |=
                                                         (1 << (lockNum % 32));
    }
}

/*
 *  ======== GateHWSpinlock_leave ========
 */
Void GateHWSpinlock_leave(GateHWSpinlock_Object *obj, IArg key)
{
    volatile UInt32 *baseAddr = Mod->baseAddr;

    obj->nested--;

    /* release the spinlock if not nested */
    if (obj->nested == 0) {
        baseAddr[obj->lockNum] = 0;
    }

    IGateProvider_leave(obj->localGate, key);
}

#if 0 /* depricated, -rvh */
/*
 *  ======== GateHWSpinlock_getResourceId ========
 */
Bits32 GateHWSpinlock_getResourceId(GateHWSpinlock_Object *obj)
{
    return (obj->lockNum);
}
#endif

/*
 *  ======== GateHWSpinlock_sharedMemReq ========
 *  This is SharedRegion memory, not Posix shared memory. Inherited from
 *  ti.sdo.ipc.interfaces.IGateMPSupport.
 */
SizeT GateHWSpinlock_sharedMemReq(const IGateMPSupport_Params *params)
{
    return(0);
}


/*
 *************************************************************************
 *                       Module functions
 *************************************************************************
 */

/*
 *  ======== GateHWSpinlock_getReservedMask ========
 */
Bits32 *GateHWSpinlock_getReservedMask(Void)
{
    return(Mod->reservedMaskAry);
}

/*
 *  ======== GateHWSpinlock_query ========
 */
Bool GateHWSpinlock_query(Int qual)
{
    Bool rc;

    switch (qual) {
        case IGateProvider_Q_BLOCKING:
            /* depends on gate proxy? */
            rc = FALSE;
            break;

        case IGateProvider_Q_PREEMPTING:
            /* depends on gate proxy? */
            rc = TRUE;
            break;

        default:
            rc = FALSE;
            break;
    }
    return(rc);
}

/* Override the IObject interface to define craete and delete APIs */
IOBJECT_CREATE1(GateHWSpinlock, IGateProvider_Handle);
