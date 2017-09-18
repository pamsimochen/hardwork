/*
 *  @file   GateMP_qnx.c
 *
 *  @brief      Multi-processor Gate
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

/* Utilities headers */
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/inc/IGateMPSupport.h>
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/osal/OsalProcess.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/GateProcess.h>
#include <ti/syslink/utils/IpcMemMgr.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/inc/_Ipc.h>

/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_GateMP_qnx.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/GateMPDefs.h>
#include <ti/syslink/inc/_NameServer.h>

#define SysLink_CFGNAMESERVER   "ti.syslink.cfg"
#define GateMP_NUMRESOURCES     "GateMP.numResources"
#define GateMP_DEFPROTECT       "GateMP.defaultProtection"
#define GateMP_MAXNAMELEN       "GateMP.maxNameLen"
#define GateMP_MAXRUNTIME       "GateMP.maxRunTimeEntries"


/* -----------------------------------------------------------------------------
 * Macros
 * -----------------------------------------------------------------------------
 */

#if 0 /* TODO -rvh */
#define GateMP_MODULEID     (UInt16) 0x6a88

#define GateMP_MAKE_MAGICSTAMP(x) ((GateMP_MODULEID << 12u) | (x))
#endif

/* VERSION */
#define GateMP_VERSION (1)

/* CREATED */
#define GateMP_CREATED (0x11202009)

/* ProxyOrder_SYSTEM */
#define GateMP_ProxyOrder_SYSTEM  (0)

/* ProxyOrder_CUSTOM1 */
#define GateMP_ProxyOrder_CUSTOM1 (1)

/* ProxyOrder_CUSTOM2 */
#define GateMP_ProxyOrder_CUSTOM2 (2)

/* ProxyOrder_NUM */
#define GateMP_ProxyOrder_NUM     (3)

/* Helper macros */
#define GETREMOTE(mask) ((GateMP_RemoteProtect)(mask >> 8))
#define GETLOCAL(mask)  ((GateMP_LocalProtect)(mask & 0xFF))
#define SETMASK(remoteProtect, localProtect) \
                        ((Bits32)(remoteProtect << 8 | localProtect))


/* Values used to populate the resource 'inUse' arrays */
#define UNUSED          ((UInt8)0)
#define USED            ((UInt8)1)
#define RESERVED        ((UInt8)-1)

/*!
 *  @brief  Name of the reserved NameServer used for GateMP.
 */
#define GateMP_NAMESERVER  "GateMP"

/* -----------------------------------------------------------------------------
 * Structs & Enums
 * -----------------------------------------------------------------------------
 */
/* Attrs */
typedef struct GateMP_Attrs {
    Bits16 mask;
    Bits16 creatorProcId;
    Bits32 arg;
    Bits32 status;
    Bits32 gateShMem;           /* gate offset in ShMem */
} GateMP_Attrs;

/* GateMP instance object */
typedef struct {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
    IOBJECT_SuperObject;       /* For inheritance for IObject */
    GateMP_RemoteProtect    remoteProtect;
    GateMP_LocalProtect     localProtect;
    Ptr                     nsKey;
    Int                     numOpens;
    Bool                    cacheEnabled;
    GateMP_Attrs *          attrs;
    UInt16                  regionId;
    SizeT                   allocSize;
    Ptr                     proxyAttrs;
    UInt                    resourceId;
    IGateProvider_Handle    gateHandle;
    Ipc_ObjType             objType; /* from shared region? */
    IGateProvider_Handle    localGate;
} GateMP_Object;

/* GateMP Module Global State */
typedef struct {
    Int32                   refCount;                           // done
    UInt32                  cfgOff;                             // done
    Int                     numRemoteSystem;                    // done
    Int                     numRemoteCustom1;                   // done
    Int                     numRemoteCustom2;                   // done
    IGateProvider_Handle    gateHwi;
    IGateProvider_Handle    gateNull;
    UInt32                  gateProcOffset;                     // done
    Int                     proxyMap[GateMP_ProxyOrder_NUM];    // done
    UInt32                  gateMPSharedOff;
    Bool                    started;
} GateMP_Module_GlobalState;

/* GateMP Module Local State */
typedef struct {
    Int32                       refCount;
    GateMP_Module_GlobalState * global;
    GateMP_Config *             cfg;
    Int                         attachRefCount;
    NameServer_Handle           nameServer;
    UInt8 *                     remoteSystemInUse;
    UInt8 *                     remoteCustom1InUse;
    UInt8 *                     remoteCustom2InUse;
    GateMP_Object **            remoteSystemGates;  /* GateMP_Object*[]  */
    GateMP_Object **            remoteCustom1Gates; /* GateMP_Object*[]  */
    GateMP_Object **            remoteCustom2Gates; /* GateMP_Object*[]  */
    IGateProvider_Handle        gateMutex;
    IGateProvider_Handle        gateProcess;
    GateMP_Handle               defaultGate;
} GateMP_Module_State;

/* Reserved */
typedef struct GateMP_Reserved {
    Bits32  version;
} GateMP_Reserved;

/* Localgate */
typedef struct GateMP_LocalGate {
    IGateProvider_Handle localGate;
    Int                  refCount;
} GateMP_LocalGate;

/*!
 *  @brief  Structure defining parameters for the GateMP module.
 */
typedef struct _GateMP_Params {
    String name;
    UInt16 regionId;
    Ptr sharedAddr;
    GateMP_LocalProtect localProtect;
    GateMP_RemoteProtect remoteProtect;
    UInt32 resourceId;
    Bool openFlag;
    UInt32 gateShMem;   /* offset into shared memory */
} _GateMP_Params;

/* -----------------------------------------------------------------------------
 * Forward declaration
 * -----------------------------------------------------------------------------
 */
Int GateMP_createLocal(GateMP_LocalProtect lp, IGateProvider_Handle *gh);
Void GateMP_setRegion0Reserved  (Ptr sharedAddr);
Void GateMP_clearRegion0Reserved  (Void);
Void GateMP_openRegion0Reserved (Ptr sharedAddr);
Void GateMP_closeRegion0Reserved (Ptr sharedAddr);
UInt GateMP_getFreeResource(UInt8 *inUse, Int num);
GateMP_Handle _GateMP_create (const _GateMP_Params * params);

/* -----------------------------------------------------------------------------
 * Globals
 * -----------------------------------------------------------------------------
 */
static
GateMP_Module_State GateMP_state =
{
    .refCount           = 0,
    .global             = NULL,
    .cfg                = NULL,
    .attachRefCount     = 0,
    .nameServer         = NULL,
    .remoteSystemInUse  = NULL,
    .remoteCustom1InUse = NULL,
    .remoteCustom2InUse = NULL,
    .remoteSystemGates  = NULL,
    .remoteCustom1Gates = NULL,
    .remoteCustom2Gates = NULL,
    .gateMutex          = NULL,
    .gateProcess        = NULL
};

static
GateMP_Module_State *GateMP_module = &GateMP_state;

static
GateMP_Object *GateMP_firstObject = NULL;

static
GateMP_Params GateMP_defInstParams =
{
    .name           = NULL,
    .regionId       = 0,
    .sharedAddr     = NULL,
    .localProtect   = GateMP_LocalProtect_INTERRUPT,    /* TODO Process? */
    .remoteProtect  = GateMP_RemoteProtect_SYSTEM
};


/* -----------------------------------------------------------------------------
 * APIs
 * -----------------------------------------------------------------------------
 */

#if 0
/*
 *  ======== GateMP_getConfig ========
 */
Void GateMP_getConfig(GateMP_Config *cfg)
{
    Int     status = GateMP_S_SUCCESS;
    IArg    key;

    GT_1trace(curTrace, GT_ENTER, "GateMP_getConfig: cfg=0x%08x", cfg);
    GT_assert(curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        status = GateMP_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_getConfig",
            GateMP_E_INVALIDARG, "arg is null");
    }
#endif

    key = Gate_enterSystem();

    if (status == GateMP_S_SUCCESS) {
        /* if not setup, return default config */
        if (GateMP_module->refCount == 0) {
            Memory_copy(cfg, &GateMP_defaultConfig, sizeof(GateMP_Config));
        }
        else {
            /* return the runtime config */
            Memory_copy(cfg, &GateMP_module->global->cfg,
                sizeof(GateMP_Config));
        }
    }

    Gate_leaveSystem(key);

    GT_0trace(curTrace, GT_LEAVE, "GateMP_getConfig");
}
#endif

/*
 *  ======== GateMP_setup ========
 */
Int GateMP_setup(Void)
{
    Int32                   status = GateMP_S_SUCCESS;
    Error_Block             eb;
    IArg                    key;
    Int                     i;
    NameServer_Params       params;
    GateMutex_Handle        gateMutex;
    Ptr                     shMemAddr;
    GateProcess_Handle      gateProcess;
    SizeT                   shMemSize;
    Bool                    newGlobal;
    SizeT                   size;

    GT_0trace(curTrace, GT_ENTER, "GateMP_setup");

    Error_init (&eb);

//  key = Gate_enterSystem();
    key = IpcMemMgr_enterGate();

    GateMP_module->refCount++;

    /* if already setup, nothing more to do */
    if (GateMP_module->refCount > 1) {
        status = GateMP_S_ALREADYSETUP;
        GT_1trace(curTrace, GT_2CLASS,
            "GateMP module already initialized, refCount=%d",
            GateMP_module->refCount);
    }

    /* acquire the global structure in shared memory */
    if (status == GateMP_S_SUCCESS) {
        size = sizeof(GateMP_Module_GlobalState);

        status = IpcMemMgr_acquire(GateMP_GLOBAL, size,
            (Ptr *)&GateMP_module->global, &newGlobal);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                status, "IpcMemMgr_acquire(GateMP_GLOBAL) failed");
            status = GateMP_E_MEMORY;
        }
        else {
            status = GateMP_S_SUCCESS;
        }
    }

    /* update the global structure ref count */
    if (status == GateMP_S_SUCCESS) {
        if (newGlobal) {
            GateMP_module->global->refCount = 1;
            GateMP_module->global->gateMPSharedOff = 0;
            GateMP_module->global->started = FALSE;
        }
        else {
            GateMP_module->global->refCount++;
        }
    }

    /* cache the config params in global structure */
    if (status == GateMP_S_SUCCESS) {
        if (newGlobal) {
            size = sizeof(GateMP_Config);

            status = IpcMemMgr_acquire(GateMP_CONFIG, size,
                (Ptr *)&GateMP_module->cfg, NULL);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "IpcMemMgr_acquire(GateMP_CONFIG) failed");
                status = GateMP_E_FAIL;
            }
            else {
                GateMP_module->global->cfgOff =
                    IpcMemMgr_getOffset(GateMP_module->cfg);
                status = GateMP_S_SUCCESS;
            }
        }
        else {
            GateMP_module->cfg =
                IpcMemMgr_getPtr(GateMP_module->global->cfgOff);
        }
    }

    /* create the thread gate */
    if (status == GateMP_S_SUCCESS) {

        gateMutex = GateMutex_create(NULL, &eb);

        if (gateMutex == NULL) {
            status = GateMP_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                status, "GateMutex_create failed");
        }
        else {
            GateMP_module->gateMutex = (IGateProvider_Handle)gateMutex;
        }
    }

    /* setup the proxy map */
    if (status == GateMP_S_SUCCESS) {
        if (newGlobal) {
            for (i = 0; i < GateMP_ProxyOrder_NUM; i++) {
                GateMP_module->global->proxyMap[i] = i;
            }
            if ((Ptr)GateMP_RemoteCustom1Proxy_create ==
                (Ptr)GateMP_RemoteSystemProxy_create) {

                GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1] =
                    GateMP_module->global->proxyMap[GateMP_ProxyOrder_SYSTEM];
            }
            if ((Ptr)GateMP_RemoteSystemProxy_create ==
                (Ptr)GateMP_RemoteCustom2Proxy_create) {

                GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] =
                    GateMP_module->global->proxyMap[GateMP_ProxyOrder_SYSTEM];
            }
            else if ((Ptr)GateMP_RemoteCustom2Proxy_create ==
                     (Ptr) GateMP_RemoteCustom1Proxy_create) {

                GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] =
                    GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1];
            }
        }
        else {
            /* TODO: validate proxy settings against global struct */
        }
    }

    /* setup resource counts */
    if (status == GateMP_S_SUCCESS) {
        if (newGlobal) {
            /* get number of configured instances */
            GateMP_module->global->numRemoteSystem =
                    GateMP_RemoteSystemProxy_getNumInstances();
            GateMP_module->global->numRemoteCustom1 =
                    GateMP_RemoteCustom1Proxy_getNumInstances();
            GateMP_module->global->numRemoteCustom2 =
                    GateMP_RemoteCustom2Proxy_getNumInstances();
        }
        else {
            /* TODO: validate num instances against global struct */
        }
    }

    /* setup the process gate */
    if (status == GateMP_S_SUCCESS) {
        if (newGlobal) {
            /* create the process gate instance */
            shMemSize = GateProcess_shMemSize();
            shMemAddr = IpcMemMgr_alloc(shMemSize);
            GateMP_module->global->gateProcOffset =
                IpcMemMgr_getOffset(shMemAddr);

            gateProcess = GateProcess_create(shMemAddr, NULL);

            if (gateProcess == NULL) {
                status = GateMP_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "GateProcess_create failed");
            }
            else {
                shMemAddr =
                    IpcMemMgr_getPtr(GateMP_module->global->gateProcOffset);
                GateMP_module->gateProcess = (IGateProvider_Handle)gateProcess;
            }
        }
        else {
            /* open the process gate instance */
            shMemAddr = IpcMemMgr_getPtr(GateMP_module->global->gateProcOffset);

            gateProcess = GateProcess_open(shMemAddr);

            if (gateProcess == NULL) {
                status = GateMP_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "GateProcess_open failed");
            }
            else {
                GateMP_module->gateProcess = (IGateProvider_Handle)gateProcess;
            }
        }
    }

    /* setup the name server */
    if (status == GateMP_S_SUCCESS) {
        if (newGlobal) {
            NameServer_Params_init(&params);
            params.maxRuntimeEntries = GateMP_module->cfg->maxRunTimeEntries;
            params.maxNameLen = GateMP_module->cfg->maxNameLen;
            params.maxValueLen = 2 * sizeof(UInt32);

            GateMP_module->nameServer =
                NameServer_create(GateMP_NAMESERVER, &params);

            if (GateMP_module->nameServer == NULL) {
                status = GateMP_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "NameServer_create failed");
            }
        }
        else {
            status = NameServer_open(GateMP_NAMESERVER,
                &GateMP_module->nameServer);

            if (status < 0) {
                status = GateMP_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "NameServer_open(\"GateMP\") failed");
            }
            else {
                status = GateMP_S_SUCCESS;
            }
        }
    }

    /* allocate memory for remote system gate handles */
    if (status == GateMP_S_SUCCESS) {
        if (GateMP_module->global->numRemoteSystem > 0) {

            GateMP_module->remoteSystemGates = Memory_calloc(NULL,
                GateMP_module->global->numRemoteSystem *
                sizeof(IGateProvider_Handle), 0, NULL);

            if (GateMP_module->remoteSystemGates == NULL) {
                status = GateMP_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "remoteSystemGates allocation failed");
            }
        }
        else {
            GateMP_module->remoteSystemGates = NULL;
        }
    }

    /* allocate memory for remote custom1 gate handles */
    if (status == GateMP_S_SUCCESS) {
        if ((GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
            GateMP_ProxyOrder_CUSTOM1) &&
            (GateMP_module->global->numRemoteCustom1 > 0)) {

            GateMP_module->remoteCustom1Gates = Memory_calloc(NULL,
                    GateMP_module->global->numRemoteCustom1 *
                    sizeof(IGateProvider_Handle), 0, NULL);

            if (GateMP_module->remoteCustom1Gates == NULL) {
                status = GateMP_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "remoteCustom1Gates allocation failed");
            }
        }
        else {
            GateMP_module->remoteCustom1Gates = NULL;
        }
    }

    /* allocate memory for remote custom2 gate handles */
    if (status == GateMP_S_SUCCESS) {
        if ((GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
            GateMP_ProxyOrder_CUSTOM2) &&
            (GateMP_module->global->numRemoteCustom2 > 0)) {

            GateMP_module->remoteCustom2Gates = Memory_calloc(NULL,
                GateMP_module->global->numRemoteCustom2 *
                sizeof(IGateProvider_Handle), 0, NULL);

            if (GateMP_module->remoteCustom2Gates == NULL) {
                status = GateMP_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "remoteCustom2Gates allocation failed");
            }
        }
        else {
            GateMP_module->remoteCustom2Gates = NULL;
        }
    }

    /* in failure case, release acquired resources in reverse order */
    if (status < 0) {
        if (GateMP_module->remoteCustom2Gates != NULL) {
            Memory_free(NULL, GateMP_module->remoteCustom2Gates,
                    GateMP_module->global->numRemoteCustom2 *
                    sizeof(IGateProvider_Handle));
            GateMP_module->remoteCustom2Gates = NULL;
        }
        if (GateMP_module->remoteCustom1Gates != NULL) {
            Memory_free(NULL, GateMP_module->remoteCustom1Gates,
                    GateMP_module->global->numRemoteCustom1 *
                    sizeof(IGateProvider_Handle));
            GateMP_module->remoteCustom1Gates = NULL;
        }
        if (GateMP_module->remoteSystemGates != NULL) {
            Memory_free(NULL, GateMP_module->remoteSystemGates,
                    GateMP_module->global->numRemoteSystem *
                    sizeof(IGateProvider_Handle));
            GateMP_module->remoteSystemGates = NULL;
        }
        if (GateMP_module->nameServer != NULL) {
            if (newGlobal) {
                NameServer_delete(&GateMP_module->nameServer);
                GateMP_module->nameServer = NULL;  /* TODO, fix delete() */
            }
            else {
                NameServer_close(&GateMP_module->nameServer);
                GateMP_module->nameServer = NULL;  /* TODO, fix close() */
            }
        }
        if (GateMP_module->gateProcess != NULL) {
            gateProcess = (GateProcess_Handle)(GateMP_module->gateProcess);
            if (newGlobal) {
                GateProcess_delete(&gateProcess);
            }
            else {
                GateProcess_close(&gateProcess);
            }
            GateMP_module->gateProcess = NULL;
        }

        /* release the config shared memory */
        if (GateMP_module->cfg != NULL) {
            if (newGlobal) {
                IpcMemMgr_release(GateMP_CONFIG);
                GateMP_module->global->cfgOff = 0;
            }
            GateMP_module->cfg = NULL;
        }

        /* decrement the global structure reference count */
        GateMP_module->global->refCount--;

        /* release the global structure shared memory */
        if (GateMP_module->global != NULL) {
            IpcMemMgr_release(GateMP_GLOBAL);
            GateMP_module->global = NULL;
        }

        if (GateMP_module->gateMutex != NULL) {
            GateMutex_delete((GateMutex_Handle *)&GateMP_module->gateMutex);
        }

        /* decrement the module reference count */
        GateMP_module->refCount--;
    }

    IpcMemMgr_leaveGate(key);
//  Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_setup: [0x%08x]", status);

    return(status);
}

/*
 *  ======== destroy ========
 */
Int GateMP_destroy(Void)
{
    Int                 status = GateMP_S_SUCCESS;
    IArg                key;
    Bool                deleteGlobal;
    GateProcess_Handle  gateProcess;

    GT_0trace(curTrace, GT_ENTER, "GateMP_destroy");

    key = IpcMemMgr_enterGate();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateMP_module->refCount <= 0) {
        status = GateMP_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_destroy",
            status, "module is not initialized");
    }
#endif

    /* dec module ref count, do nothing if not last user */
    if (status == GateMP_S_SUCCESS) {
        GateMP_module->refCount--;

        if (GateMP_module->refCount > 0) {
            status = GateMP_S_BUSY;
        }
    }

    /* dec global struct ref count */
    if (status == GateMP_S_SUCCESS) {
        GateMP_module->global->refCount--;
        deleteGlobal = GateMP_module->global->refCount > 0 ? FALSE : TRUE;
    }

    /* free gate handle arrays */
    if (status == GateMP_S_SUCCESS) {

        /* if custom2 gates not mapped, free the gate array */
        if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] !=
            GateMP_ProxyOrder_CUSTOM2) {
            GateMP_module->remoteCustom2Gates = NULL;
        }
        else if (GateMP_module->remoteCustom2Gates != NULL) {
            Memory_free(NULL, GateMP_module->remoteCustom2Gates,
                    GateMP_module->global->numRemoteCustom2 *
                    sizeof(IGateProvider_Handle));
            GateMP_module->remoteCustom2Gates = NULL;
        }

        /* if custom1 gates not mapped, free the gate array */
        if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1] !=
            GateMP_ProxyOrder_CUSTOM1) {
            GateMP_module->remoteCustom1Gates = NULL;
        }
        else if (GateMP_module->remoteCustom1Gates != NULL) {
            Memory_free(NULL, GateMP_module->remoteCustom1Gates,
                    GateMP_module->global->numRemoteCustom1 *
                    sizeof(IGateProvider_Handle));
            GateMP_module->remoteCustom1Gates = NULL;
        }

        /* free system gate array */
        if (GateMP_module->remoteSystemGates != NULL) {
            Memory_free(NULL, GateMP_module->remoteSystemGates,
                    GateMP_module->global->numRemoteSystem *
                    sizeof(IGateProvider_Handle));
            GateMP_module->remoteSystemGates = NULL;
        }
    }

    /* finalize the name server instance */
    if (status == GateMP_S_SUCCESS) {
        if (GateMP_module->nameServer != NULL) {
            if (deleteGlobal) {
                NameServer_delete(&GateMP_module->nameServer);
                GateMP_module->nameServer = NULL;  /* TODO, fix delete() */
            }
            else {
                NameServer_close(&GateMP_module->nameServer);
                GateMP_module->nameServer = NULL;  /* TODO, fix close() */
            }
        }
    }

    /* finalize the process gate */
    if (status == GateMP_S_SUCCESS) {
        if (GateMP_module->gateProcess != NULL) {
            gateProcess = (GateProcess_Handle)(GateMP_module->gateProcess);
            if (deleteGlobal) {
                GateProcess_delete(&gateProcess);
            }
            else {
                GateProcess_close(&gateProcess);
            }
            GateMP_module->gateProcess = NULL;
        }
    }

    /* finalize the global structure */
    if (status == GateMP_S_SUCCESS) {
        IpcMemMgr_release(GateMP_NAMESERVER);
        GateMP_module->global = NULL;
    }

    /* finalize the thread gate */
    if (status == GateMP_S_SUCCESS) {
        if (GateMP_module->gateMutex != NULL) {
            GateMutex_delete((GateMutex_Handle *)&GateMP_module->gateMutex);
        }
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_destroy", status);

    return(status);
}

/*
 *  ======== GateMP_getSharedParams ========
 */
static Void GateMP_getSharedParams(
        GateMP_Params *         sparams,
        const _GateMP_Params *  params)
{
    GT_2trace(curTrace, GT_ENTER,
            "GateMP_getSharedParams: sparams=0x%08x, params=0x%08x",
            sparams, params);

    sparams->name           = params->name;
    sparams->regionId       = params->regionId;
    sparams->sharedAddr     = params->sharedAddr;
    sparams->localProtect   = (GateMP_LocalProtect)params->localProtect;
    sparams->remoteProtect  = (GateMP_RemoteProtect)params->remoteProtect;

    GT_0trace(curTrace, GT_LEAVE, "GateMP_getSharedParams");
}

/*
 *  ======== GateMP_Params_init ========
 */
Void GateMP_Params_init(GateMP_Params *params)
{
    GT_1trace(curTrace, GT_ENTER, "GateMP_Params_init: params=0x%08x", params);
    GT_assert(curTrace, (params != NULL));

    Memory_copy(params, &GateMP_defInstParams, sizeof(GateMP_Params));

    GT_0trace(curTrace, GT_LEAVE, "GateMP_Params_init");
}

/*
 *  ======== GateMP_Instance_init ========
 */
Int GateMP_Instance_init(GateMP_Object *obj, const _GateMP_Params *params)
{
    GateMP_RemoteSystemProxy_Params     systemParams;
    GateMP_RemoteCustom1Proxy_Params    custom1Params;
    GateMP_RemoteCustom2Proxy_Params    custom2Params;
    SizeT                               cacheLine, offset;
    SharedRegion_SRPtr                  sharedShmBase;
    GateMP_Params                       sparams;
    UInt32                              nsValue[3];
    Int                                 status;
    IHeap_Handle                        regionHeap;
    Ptr                                 shMemAddr;
    SizeT                               shMemSize;

    GT_2trace(curTrace, GT_ENTER, "GateMP_Instance_init", obj, params);

    status = 0;
    shMemAddr = 0;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateMP_module->refCount <= 0) {
        status = GateMP_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_Instance_init",
            status, "module is not initialized");
        return(status);
    }
#endif

    obj->resourceId = (UInt)-1;
    obj->localGate = NULL;

    /* create/open the local gate instance */
    if ((params->openFlag) && (params->gateShMem != 0)) {
        /* use existing gate shared memory */
        shMemAddr = IpcMemMgr_getPtr(params->gateShMem);

        obj->localGate = (IGateProvider_Handle)GateProcess_open(shMemAddr);

        if (obj->localGate == NULL) {
            return(9);
        }
    }
    else {
        /* create a new local gate instance */
        switch (params->localProtect) {
            case GateMP_LocalProtect_NONE:
                obj->localGate = IGateProvider_NULL;
                break;

            case GateMP_LocalProtect_INTERRUPT:
                obj->localGate = Gate_systemHandle;
                break;

            /* TODO: always create a new local gate intance but
             * it should be of the correct type */
            case GateMP_LocalProtect_TASKLET:
            case GateMP_LocalProtect_THREAD:
            case GateMP_LocalProtect_PROCESS:
                shMemSize = GateProcess_shMemSize();
                shMemAddr = IpcMemMgr_alloc(shMemSize);

                obj->localGate = (IGateProvider_Handle)
                    GateProcess_create(shMemAddr, NULL);

                if (obj->localGate == NULL) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "GateMP_Instance_init", status,
                        "GateProcess_create failed");
                    return(8);
                }
                break;

            default:
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_Instance_init",
                    status, "invalid local protect type");
                    return(11);
                break;
        }
    }

    /* open GateMP instance */
    if (params->openFlag == TRUE) {
        /* all open work done here except for remote gateHandle */
        obj->localProtect  = params->localProtect;
        obj->remoteProtect = params->remoteProtect;
        obj->nsKey         = 0;
        obj->numOpens      = 1;
        obj->attrs         = (GateMP_Attrs *)params->sharedAddr;
        obj->regionId      = SharedRegion_getId((Ptr)obj->attrs);

        /* assert that attrs is in a valid shared region */
        if (obj->regionId == SharedRegion_INVALIDREGIONID) {
            return(1);  /* TODO: can this be passed in? */
        }

        obj->cacheEnabled  = SharedRegion_isCacheEnabled(obj->regionId);
        obj->objType       = Ipc_ObjType_OPENDYNAMIC;

        obj->allocSize = 0;

        offset = Memory_getMaxDefaultTypeAlign();
        cacheLine = SharedRegion_getCacheLineSize(obj->regionId);
        offset = (cacheLine > offset ? cacheLine : offset);
        offset = _Ipc_roundup(sizeof(GateMP_Attrs), offset);
        obj->proxyAttrs = (Ptr)((UInt32)obj->attrs + offset);
    }

    /* create GateMP instance */
    else {
        obj->localProtect  = params->localProtect;
        obj->remoteProtect = params->remoteProtect;
        obj->nsKey         = 0;
        obj->numOpens      = 0;

        if (obj->remoteProtect == GateMP_RemoteProtect_NONE) {
            /* creating a local gate (GateMP_Attrs is in local memory) */
            obj->gateHandle = obj->localGate;

            if (params->sharedAddr != NULL) {
                /* create a local gate using shared memory */
                obj->objType = Ipc_ObjType_CREATEDYNAMIC;
                obj->attrs = params->sharedAddr;
                obj->regionId = SharedRegion_getId((Ptr)obj->attrs);
                obj->cacheEnabled = SharedRegion_isCacheEnabled(obj->regionId);
            }
            else {
                /* create a local gate allocating from the local heap */
                obj->objType = Ipc_ObjType_LOCAL;
                obj->regionId = SharedRegion_INVALIDREGIONID;
                obj->cacheEnabled = FALSE; /* local */
                obj->attrs = Memory_calloc(NULL, sizeof(GateMP_Attrs), 0, NULL);
                if (obj->attrs == NULL) {
                    return (2);
                }
            }

            obj->attrs->arg = (Bits32)obj;
            obj->attrs->mask = SETMASK(obj->remoteProtect, obj->localProtect);
            obj->attrs->creatorProcId = MultiProc_self();
            obj->attrs->status = GateMP_CREATED;
            obj->attrs->gateShMem = 0;

            if (obj->cacheEnabled) {
                /*  Need to write back memory if cache is enabled because
                 *  cache will be invalidated during openByAddr
                 */
                Cache_wbInv(obj->attrs, sizeof(GateMP_Attrs), Cache_Type_ALL,
                        TRUE);
            }

            if (params->name != NULL) {
                /*  nsv[0]       : address of GateMP_Attrs (SR or heap)
                 *  nsv[1](31:16): creator procId
                 *  nsv[1](15:0) : 0 = local gate, 1 = remote gate
                 *  nsv[2]       : creator processId (pid)
                 */
                nsValue[0] = (UInt32)obj->attrs;
                nsValue[1] = MultiProc_self() << 16;
                nsValue[2] = OsalProcess_getPid();
                obj->nsKey = NameServer_add(GateMP_module->nameServer,
                        params->name, &nsValue, sizeof(nsValue));
            }

            /* nothing else to do for local gates */
            return(0);
        }

        if (params->sharedAddr == NULL) {
            /* allocate shared data from region heap */
            obj->objType = Ipc_ObjType_CREATEDYNAMIC_REGION;
            obj->regionId = params->regionId;
            GateMP_getSharedParams(&sparams, params);
            obj->allocSize = GateMP_sharedMemReq(&sparams);
            obj->cacheEnabled = SharedRegion_isCacheEnabled(obj->regionId);

            /* the region heap will do the alignment */
            regionHeap = SharedRegion_getHeap(obj->regionId);
            GT_assert(curTrace, (regionHeap != NULL));

            obj->attrs = Memory_calloc(regionHeap, obj->allocSize, 0, NULL);

            if (obj->attrs == NULL) {
                return(3);
            }
        }
        else {
            /* creating using sharedAddr */
            obj->regionId = SharedRegion_getId(params->sharedAddr);

            /* assert that the buffer is in a valid shared region */
            if (obj->regionId == SharedRegion_INVALIDREGIONID) {
                return(4);
            }

            /* assert that sharedAddr is cache aligned */
            cacheLine = SharedRegion_getCacheLineSize(obj->regionId);
            if (((UInt32)params->sharedAddr % cacheLine) != 0) {
                return(5);
            }

            obj->attrs = (GateMP_Attrs *)params->sharedAddr;
            obj->cacheEnabled = SharedRegion_isCacheEnabled(obj->regionId);
            obj->objType = Ipc_ObjType_CREATEDYNAMIC;
        }

        /* compute the address of the proxy attrs */
        offset = Memory_getMaxDefaultTypeAlign();
        cacheLine = SharedRegion_getCacheLineSize(obj->regionId);
        offset = (cacheLine > offset ? cacheLine : offset);
        offset = _Ipc_roundup(sizeof(GateMP_Attrs), offset);
        obj->proxyAttrs = (Ptr)((UInt32)obj->attrs + offset);
    }

    /* proxy work for open and create done here */
    switch (obj->remoteProtect) {

        case GateMP_RemoteProtect_SYSTEM:
            if (obj->objType == Ipc_ObjType_OPENDYNAMIC) {
                /* resourceId set by open call */
                obj->resourceId = params->resourceId;
            }
            else {
                /* created instance */
                obj->resourceId = GateMP_getFreeResource(
                        GateMP_module->remoteSystemInUse,
                        GateMP_module->global->numRemoteSystem);
                if (obj->resourceId == -1) {
                    return(6);
                }
            }

            /* create the proxy object */
            GateMP_RemoteSystemProxy_Params_init(&systemParams);
            systemParams.resourceId = obj->resourceId;
            systemParams.openFlag = (obj->objType == Ipc_ObjType_OPENDYNAMIC);
            systemParams.sharedAddr = obj->proxyAttrs;
            systemParams.regionId = obj->regionId;

            obj->gateHandle = (IGateProvider_Handle)
                GateMP_RemoteSystemProxy_create(obj->localGate, &systemParams);

            if (obj->gateHandle == NULL) {
                return(7);
            }

            /* store the object handle in the gate array */
            GateMP_module->remoteSystemGates[obj->resourceId] = obj;
            break;

        case GateMP_RemoteProtect_CUSTOM1:
            if (obj->objType == Ipc_ObjType_OPENDYNAMIC) {
                /* resourceId set by open call */
                obj->resourceId = params->resourceId;
            }
            else {
                /* created instance */
                obj->resourceId = GateMP_getFreeResource(
                        GateMP_module->remoteCustom1InUse,
                        GateMP_module->global->numRemoteCustom1);
                if (obj->resourceId == -1) {
                    return(6);
                }
            }

            /* create the proxy object */
            GateMP_RemoteCustom1Proxy_Params_init(&custom1Params);
            custom1Params.resourceId = obj->resourceId;
            custom1Params.openFlag = (obj->objType == Ipc_ObjType_OPENDYNAMIC);
            custom1Params.sharedAddr = obj->proxyAttrs;
            custom1Params.regionId = obj->regionId;

            obj->gateHandle = (IGateProvider_Handle)
                GateMP_RemoteCustom1Proxy_create(obj->localGate,
                    &custom1Params);

            if (obj->gateHandle == NULL) {
                return(7);
            }

            /* store the object handle in the gate array */
            GateMP_module->remoteCustom1Gates[obj->resourceId] = obj;
            break;

        case GateMP_RemoteProtect_CUSTOM2:
            if (obj->objType == Ipc_ObjType_OPENDYNAMIC) {
                /* resourceId set by open call */
                obj->resourceId = params->resourceId;
            }
            else {
                /* created instance */
                obj->resourceId = GateMP_getFreeResource(
                        GateMP_module->remoteCustom2InUse,
                        GateMP_module->global->numRemoteCustom2);
                if (obj->resourceId == -1) {
                    return(6);
                }
            }

            /* create the proxy object */
            GateMP_RemoteCustom2Proxy_Params_init(&custom2Params);
            custom2Params.resourceId = obj->resourceId;
            custom2Params.openFlag = (obj->objType == Ipc_ObjType_OPENDYNAMIC);
            custom2Params.sharedAddr = obj->proxyAttrs;
            custom2Params.regionId = obj->regionId;

            obj->gateHandle = (IGateProvider_Handle)
                GateMP_RemoteCustom2Proxy_create(
                    obj->localGate, &custom2Params);

            if (obj->gateHandle == NULL) {
                return(7);
            }

            /* store the object handle in the gate array */
            GateMP_module->remoteCustom2Gates[obj->resourceId] = obj;
            break;

        default:
            break;
    }

    /* add name/attrs to NameServer table */
    if (obj->objType != Ipc_ObjType_OPENDYNAMIC) {
        obj->attrs->arg = obj->resourceId;
        obj->attrs->mask = SETMASK(obj->remoteProtect, obj->localProtect);
        obj->attrs->creatorProcId = MultiProc_self();
        obj->attrs->status = GateMP_CREATED;
        if (shMemAddr == 0) {
            obj->attrs->gateShMem = 0;
        }
        else {
            obj->attrs->gateShMem = IpcMemMgr_getOffset(shMemAddr);
        }

        if (obj->cacheEnabled) {
            Cache_wbInv(obj->attrs, sizeof(GateMP_Attrs), Cache_Type_ALL, TRUE);
        }

        if (params->name != NULL) {
            sharedShmBase = SharedRegion_getSRPtr(obj->attrs, obj->regionId);
            /*  nsv[0]       : address of GateMP_Attrs (SR or heap)
             *  nsv[1](31:16): creator procId
             *  nsv[1](15:0) : 0 = local gate, 1 = remote gate
             *  nsv[2]       : creator processId (pid)
             */
            nsValue[0] = (UInt32)sharedShmBase;
            nsValue[1] = MultiProc_self() << 16 | 1;
            nsValue[2] = OsalProcess_getPid();

            obj->nsKey = NameServer_add(GateMP_module->nameServer,
                    params->name, &nsValue, sizeof(nsValue));

            if (obj->nsKey == NULL) {
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_Instance_init",
                        8, "NameServer_add Failed");
                return(8);
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_Instance_init: [0x%08x]", status);
    return(status);
}

/*
 *  ======== GateMP_Instance_finalize ========
 *  Internal function.
 */
Void GateMP_Instance_finalize(GateMP_Object *obj, Int status)
{
    IArg                                key;
//  GateMP_RemoteCustom1Proxy_Handle    custom1Handle;
//  GateMP_RemoteCustom2Proxy_Handle    custom2Handle;
    Int                                 _status;
    GateMP_Object **                    remoteGates;
    UInt8 *                             inUseArray;
    UInt                                numResources = 0;
//  Ptr                                 proxyAttrs;
//  GateMP_Handle                       defGate;

    GT_2trace(curTrace, GT_ENTER,
            "GateMP_Instance_finalize: obj=0x%08x, status=%d", obj, status);

    _status = 0;
    inUseArray = NULL;
    remoteGates = NULL;
    key = 0;

    /* TODO: make this into an assert */
    /* cannot call when numOpen is non-zero */
//  if (obj->numOpens != 0) {
//      _status = GateMP_E_INVALIDSTATE;
//  }

    /* remove from NameServer */
    if (obj->nsKey != 0) {
        NameServer_removeEntry(GateMP_module->nameServer, obj->nsKey);
        obj->nsKey = 0;
    }

    /* clear the attr status flag */
    if (obj->objType != Ipc_ObjType_OPENDYNAMIC) {
        obj->attrs->status = 0;
        if (obj->cacheEnabled) {
            Cache_wbInv(obj->attrs, sizeof(GateMP_Attrs), Cache_Type_ALL, TRUE);
        }
    }

    /*  If ObjType_LOCAL, memory was allocated from the local system heap.
     *  obj->attrs might be NULL if the Memory_alloc failed in Instance_init
     */
    if ((obj->objType == Ipc_ObjType_LOCAL) && (obj->attrs != NULL)) {
        Memory_free(NULL, obj->attrs, sizeof(GateMP_Attrs));
        obj->attrs = NULL;
    }

    /* delete the remote gate */
    switch (obj->remoteProtect) {

        case GateMP_RemoteProtect_SYSTEM:
            if (obj->gateHandle != NULL) {
                GateMP_RemoteSystemProxy_delete(
                        (GateMP_RemoteSystemProxy_Handle *)&obj->gateHandle);
            }
            inUseArray = GateMP_module->remoteSystemInUse;
            remoteGates = GateMP_module->remoteSystemGates;
            numResources = GateMP_module->global->numRemoteSystem;
            break;

        case GateMP_RemoteProtect_CUSTOM1:
            if (obj->gateHandle != NULL) {
                GateMP_RemoteCustom1Proxy_delete(
                        (GateMP_RemoteCustom1Proxy_Handle *)&obj->gateHandle);
            }
            inUseArray = GateMP_module->remoteCustom1InUse;
            remoteGates = GateMP_module->remoteCustom1Gates;
            numResources = GateMP_module->global->numRemoteCustom1;
            break;

        case GateMP_RemoteProtect_CUSTOM2:
            if (obj->gateHandle != NULL) {
                GateMP_RemoteCustom2Proxy_delete(
                        (GateMP_RemoteCustom2Proxy_Handle *)&obj->gateHandle);
            }
            inUseArray = GateMP_module->remoteCustom2InUse;
            remoteGates = GateMP_module->remoteCustom2Gates;
            numResources = GateMP_module->global->numRemoteCustom2;
            break;

        case GateMP_RemoteProtect_NONE:
            /*  nothing else to finalize */
            return;  /* TODO: eliminate this return */

        default:
            /* Nothing to do */
            break;  /* TODO: should this be an assert? */
    }

    /* close/delete local gate */
    switch (obj->localProtect) {

        case GateMP_LocalProtect_TASKLET:
        case GateMP_LocalProtect_THREAD:
        case GateMP_LocalProtect_PROCESS:
            if (obj->objType == Ipc_ObjType_OPENDYNAMIC) {
                GateProcess_close((GateProcess_Handle *)(&obj->localGate));
            }
            else {
                GateProcess_delete((GateProcess_Handle *)(&obj->localGate));
            }
            break;

        default:
            /* nothing to be done */
            break;
    }

    /* clear the handle array entry in local memory */
    if (obj->resourceId != (UInt)-1) {
        remoteGates[obj->resourceId] = NULL;
    }

    if ((obj->objType != Ipc_ObjType_OPENDYNAMIC)
        && (obj->resourceId != (UInt)-1)) {

        /* use default gate if not deleting it */
        if (obj != (GateMP_Object *)(GateMP_module->defaultGate)) {
            key = GateMP_enter(GateMP_module->defaultGate);
        }

        /* clear the resource used flag in shared memory */
        inUseArray[obj->resourceId] = UNUSED;
        if (obj->cacheEnabled) {
            Cache_wbInv(inUseArray, numResources * sizeof(UInt8),
                    Cache_Type_ALL, TRUE);
        }

        /* use default gate if not deleting it */
        if (obj != (GateMP_Object *)(GateMP_module->defaultGate)) {
            GateMP_leave(GateMP_module->defaultGate, key);
        }
    }

    /* free memory allocated from the region heap */
    if (obj->objType == Ipc_ObjType_CREATEDYNAMIC_REGION) {
        if (obj->attrs != NULL) {
            Memory_free(SharedRegion_getHeap(obj->regionId), obj->attrs,
                    obj->allocSize);
        }
    }

    GT_0trace(curTrace, GT_LEAVE, "GateMP_Instance_finalize");
}


/*
 *  ======== GateMP_enter ========
 */
IArg GateMP_enter(GateMP_Handle handle)
{
    GateMP_Object * obj;
    IArg            key;

    GT_1trace(curTrace, GT_ENTER, "GateMP_enter: handle=0x%08x", handle);

    obj = (GateMP_Object *)handle;
    key = IGateProvider_enter(obj->gateHandle);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_enter: key=0x%08x", key);

    return(key);
}

/*
 *  ======== GateMP_leave ========
 */
Void GateMP_leave(GateMP_Handle handle, IArg key)
{
    GateMP_Object *obj;

    GT_2trace(curTrace, GT_ENTER,
            "GateMP_leave: handle=0x%08x, key=0x%08x", handle, key);

    obj = (GateMP_Object *)handle;
    IGateProvider_leave(obj->gateHandle, key);

    GT_0trace(curTrace, GT_LEAVE, "GateMP_leave");
}

/*
 *  ======== GateMP_open ========
 */
Int GateMP_open(String name, GateMP_Handle *handle)
{
    SharedRegion_SRPtr  sharedShmBase;
    Int                 status;
    UInt32              len;
    Ptr                 sharedAddr;
    UInt32              nsValue[3];

    GT_1trace(curTrace, GT_ENTER, "GateMP_open:", name);

    status = GateMP_S_SUCCESS;

    /* assert that a pointer has been supplied */
    if (handle == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_open",
                status, "handle cannot be null");
        status = GateMP_E_INVALIDARG;
    }

    /* retrieve attrs from name server, search all processors */
    if (status == GateMP_S_SUCCESS) {
        len = sizeof(nsValue);

        status = NameServer_get(GateMP_module->nameServer, name, &nsValue,
            &len, NULL);

        if (status < 0) {
            *handle = NULL;
            status = GateMP_E_NOTFOUND;
        }
        else if ((nsValue[1] >> 16) == MultiProc_self()) {
            GT_assert(curTrace, (len == sizeof(nsValue)));
        }
        status = GateMP_S_SUCCESS;
    }

    if (status == GateMP_S_SUCCESS) {
        /*
         * The least significant bit of nsValue[1] == 0 means its a
         * local (private) GateMP, otherwise its a remote (shared) GateMP.
         */
        if ((nsValue[1] & 0x1) == 0) {
            if ((nsValue[1] >> 16) != MultiProc_self()) {
                /* error: trying to open another processor's private gate */
                *handle = NULL;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_open",
                    status, "cannot open private gate from another processor");
                status = GateMP_E_FAIL;
            }
            else if (nsValue[2] != OsalProcess_getPid()) {
                /* error: trying to open another process's private gate */
                *handle = NULL;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_open",
                    status, "cannot open private gate from another process");
                status = GateMP_E_FAIL;
            }
        }
    }

    if (status == GateMP_S_SUCCESS) {
        if ((nsValue[1] & 0x1) == 0) {
            /*
             * Opening a local GateMP locally. The GateMP is created
             * from a local heap so don't do SharedRegion Ptr conversion.
             */
            sharedAddr = (Ptr)nsValue[0];
        }
        else {
            /* Opening a remote GateMP. Must do SharedRegion Ptr conversion. */
            sharedShmBase = (SharedRegion_SRPtr)nsValue[0];
            sharedAddr = SharedRegion_getPtr(sharedShmBase);
        }

        status = GateMP_openByAddr(sharedAddr, handle);
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_open:", status);

    return(status);
}

/*
 *  ======== GateMP_openByAddr ========
 */
Int GateMP_openByAddr(Ptr sharedAddr, GateMP_Handle *handle)
{
    Int                     status;
    IArg                    key;
    GateMP_Object *         obj = NULL;
    _GateMP_Params          params;
    GateMP_Attrs *          attrs;
    UInt16                  regionId;


    GT_2trace(curTrace, GT_ENTER, "GateMP_openByAddr: ", sharedAddr, handle);

    status = GateMP_S_SUCCESS;
    attrs = (GateMP_Attrs *)sharedAddr;

    /* get the region id and invalidate attrs is needed */
    regionId = SharedRegion_getId(sharedAddr);

    if ((regionId != SharedRegion_INVALIDREGIONID)
        && SharedRegion_isCacheEnabled(regionId)) {

        Cache_inv(attrs, sizeof(GateMP_Attrs), Cache_Type_ALL, TRUE);
    }

    if (attrs->status != GateMP_CREATED) {
        status = GateMP_E_FAIL;
    }
    else {
        /* local gate */
        if (GETREMOTE(attrs->mask) == GateMP_RemoteProtect_NONE) {
            if (attrs->creatorProcId != MultiProc_self()) {
                status = GateMP_E_LOCALGATE;
            }
            else {
                key = Gate_enterSystem();
                *handle = (GateMP_Handle)attrs->arg;
                obj = (GateMP_Object *)(*handle);
                obj->numOpens++;
                Gate_leaveSystem(key);
            }
        }
        else {
            /* remote case */
            switch (GETREMOTE(attrs->mask)) {
                case GateMP_RemoteProtect_SYSTEM:
                    obj = GateMP_module->remoteSystemGates[attrs->arg];
                    break;

                case GateMP_RemoteProtect_CUSTOM1:
                    obj = GateMP_module->remoteCustom1Gates[attrs->arg];
                    break;

                case GateMP_RemoteProtect_CUSTOM2:
                    obj = GateMP_module->remoteCustom2Gates[attrs->arg];
                    break;

                default:
                    status = GateMP_E_FAIL;
                    /* TODO: add failure */
                    break;
            }

            /*  If the object is NULL, then it must have been created
             *  on a remote processor or in another process on the
             *  local processor. Need to create a local object. This is
             *  accomplished by setting the openFlag to TRUE.
             */
            if ((status == GateMP_S_SUCCESS) && (obj == NULL)) {
                /* create a GateMP object with the openFlag set to true */
                params.name = NULL;
                params.openFlag = TRUE;
                params.sharedAddr = sharedAddr;
                params.resourceId = attrs->arg;
                params.localProtect = GETLOCAL(attrs->mask);
                params.remoteProtect = GETREMOTE(attrs->mask);
                params.gateShMem = attrs->gateShMem;

                obj = (GateMP_Object *)_GateMP_create(&params);

                if (obj == NULL) {
                    status = GateMP_E_FAIL;
                }
            }
            else {
                obj->numOpens++;
            }

            /* Return the "opened" GateMP instance  */
            *handle = (GateMP_Handle)obj;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_openByAddr:", status);

    return (status);
}

/*
 *  ======== GateMP_close ========
 */
Int GateMP_close(GateMP_Handle *handle)
{
    GateMP_Object * obj;
    IArg            key;
    Int             status;

    GT_1trace (curTrace, GT_ENTER, "GateMP_close", handle);

    status = GateMP_S_SUCCESS;
    obj = (GateMP_Object *)(*handle);

    key = Gate_enterSystem();

    /*  Cannot call with the numOpens equal to zero.  This is either
     *  a created handle or been closed already.
     */
    if (obj->numOpens == 0) {
        status = GateMP_E_INVALIDSTATE;
    }

    if (status == GateMP_S_SUCCESS) {
        obj->numOpens--;

        /*  If the count is zero and the gate is opened, then this
         *  object was created in the open (i.e. the create happened
         *  on a remote processor or another process).
         */
        if ((obj->numOpens == 0) && (obj->objType == Ipc_ObjType_OPENDYNAMIC)) {
            GateMP_delete(handle);
        }
        else {
            *handle = NULL;
        }
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_close: [0x%0x8]", status);

    return(status);
}

/*
 *  ======== GateMP_getSharedAddr ========
 */
SharedRegion_SRPtr GateMP_getSharedAddr(GateMP_Handle handle)
{
    GateMP_Object *     obj;
    SharedRegion_SRPtr  srPtr;

    obj = (GateMP_Object *)handle;
    srPtr = SharedRegion_getSRPtr(obj->attrs, obj->regionId);
    return(srPtr);
}

/*
 *  ======== GateMP_query ========
 */
Bool GateMP_query(Int qual)
{
    return (FALSE);
}

/*
 *  ======== GateMP_getDefaultRemote ========
 */
GateMP_Handle GateMP_getDefaultRemote()
{
    return(GateMP_module->defaultGate);
}

/*
 *  ======== GateMP_getLocalProtect ========
 */
GateMP_LocalProtect GateMP_getLocalProtect(GateMP_Handle handle)
{
    GateMP_Object *obj;

    obj = (GateMP_Object *)handle;
    return(obj->localProtect);
}

/*
 *  ======== GateMP_getRemoteProtect ========
 */
GateMP_RemoteProtect GateMP_getRemoteProtect(GateMP_Handle handle)
{
    GateMP_Object *obj;

    obj = (GateMP_Object *)handle;
    return (obj->remoteProtect);
}

/*
 *  ======== GateMP_createLocal ========
 */
Int GateMP_createLocal(GateMP_LocalProtect lp, IGateProvider_Handle *gh)
{
    Int                 status;

    GT_2trace(curTrace, GT_ENTER, "GateMP_createLocal:", lp, gh);

    status = GateMP_S_SUCCESS;

    switch (lp) {
        case GateMP_LocalProtect_NONE:
            *gh = IGateProvider_NULL;
            break;

        case GateMP_LocalProtect_INTERRUPT:
            *gh = Gate_systemHandle;
            break;

        case GateMP_LocalProtect_TASKLET:
        case GateMP_LocalProtect_THREAD:
//          *gh = GateMP_module->gateMutex;     // TODO: hack for default
            *gh = GateMP_module->gateProcess;   // gate fix.
            break;

        case GateMP_LocalProtect_PROCESS:
            *gh = GateMP_module->gateProcess;
            break;

        default:
            status = GateMP_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_createLocal",
                status, "invalid local protect type");
            break;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_createLocal:", status);

    return (status);
}

/*
 *  ======== GateMP_sharedMemReq ========
 *  Internal function
 */
SizeT GateMP_sharedMemReq(const GateMP_Params *params)
{
    SizeT                               memReq, cacheLine;
    UInt16                              regionId;
    GateMP_RemoteSystemProxy_Params     systemParams;
    GateMP_RemoteCustom1Proxy_Params    custom1Params;
    GateMP_RemoteCustom2Proxy_Params    custom2Params;

    GT_1trace(curTrace, GT_ENTER, "GateMP_sharedMemReq: params=0x%08x", params);

    if (params->sharedAddr != NULL) {
        regionId = SharedRegion_getId(params->sharedAddr);
    }
    else {
        regionId = params->regionId;
    }

    memReq = Memory_getMaxDefaultTypeAlign();
    cacheLine = SharedRegion_getCacheLineSize(regionId);
    memReq = cacheLine > memReq ? cacheLine : memReq;
    memReq = _Ipc_roundup(sizeof(GateMP_Attrs), memReq);

    /* add the amount of shared memory required by proxy */
    if (params->remoteProtect == GateMP_RemoteProtect_SYSTEM) {
        GateMP_RemoteSystemProxy_Params_init(&systemParams);
        systemParams.regionId = regionId;
        memReq += GateMP_RemoteSystemProxy_sharedMemReq(
                (IGateMPSupport_Params *)&systemParams);
    }
    else if (params->remoteProtect == GateMP_RemoteProtect_CUSTOM1) {
        GateMP_RemoteCustom1Proxy_Params_init(&custom1Params);
        custom1Params.regionId = regionId;
        memReq += GateMP_RemoteCustom1Proxy_sharedMemReq(
                (IGateMPSupport_Params *)&custom1Params);
    }
    else if (params->remoteProtect == GateMP_RemoteProtect_CUSTOM2) {
        GateMP_RemoteCustom2Proxy_Params_init(&custom2Params);
        custom2Params.regionId = regionId;
        memReq += GateMP_RemoteCustom2Proxy_sharedMemReq(
                (IGateMPSupport_Params *)&custom2Params);
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_sharedMemReq: [%d]", memReq);

    return(memReq);
}

Void GateMP_setReserved(UInt32 remoteProtectType,UInt32 lockNum)
{
    if (remoteProtectType == GateMP_RemoteProtect_SYSTEM) {
        GateMP_RemoteSystemProxy_setReserved(lockNum);
    }
    else if (remoteProtectType == GateMP_RemoteProtect_CUSTOM1) {
        GateMP_RemoteCustom1Proxy_setReserved(lockNum);
    }
    else if(remoteProtectType == GateMP_RemoteProtect_CUSTOM2) {
        GateMP_RemoteCustom2Proxy_setReserved(lockNum);
    }
}

/*
 *  ======== GateMP_getRegion0ReservedSize ========
 */
SizeT GateMP_getRegion0ReservedSize(Void)
{
    SizeT reserved, minAlign;

    GT_0trace (curTrace, GT_ENTER, "GateMP_getRegion0ReservedSize");

    minAlign = Memory_getMaxDefaultTypeAlign();

    if (SharedRegion_getCacheLineSize(0) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize(0);
    }

    reserved = _Ipc_roundup(sizeof(GateMP_Reserved), minAlign);

    reserved += _Ipc_roundup(GateMP_module->global->numRemoteSystem, minAlign);

    if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
        GateMP_ProxyOrder_CUSTOM1) {

        reserved += _Ipc_roundup(GateMP_module->global->numRemoteCustom1,
            minAlign);
    }

    if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
        GateMP_ProxyOrder_CUSTOM2) {
        reserved += _Ipc_roundup(GateMP_module->global->numRemoteCustom2,
            minAlign);
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_getRegion0ReservedSize", reserved);

    return (reserved);
}

/*
 *  ======== GateMP_setRegion0Reserved ========
 */
Void GateMP_setRegion0Reserved(Ptr sharedAddr)
{
    GateMP_Reserved *reserve;
    SizeT minAlign, offset;
    UInt i;
    Bits32 *delegateReservedMask;

    GT_1trace (curTrace, GT_ENTER, "GateMP_setRegion0Reserved", sharedAddr);

    minAlign = Memory_getMaxDefaultTypeAlign();
    if (SharedRegion_getCacheLineSize(0) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize(0);
    }

    /* setup GateMP_Reserved fields */
    reserve = (GateMP_Reserved *)sharedAddr;
    reserve->version = GateMP_VERSION;

    if (SharedRegion_isCacheEnabled(0)) {
        Cache_wbInv(sharedAddr, sizeof(GateMP_Reserved), Cache_Type_ALL, TRUE);
    }

    /*
     *  Initialize the in-use array in shared memory for the system gates.
     */
    offset = _Ipc_roundup(sizeof(GateMP_Reserved), minAlign);
    GateMP_module->remoteSystemInUse = (Ptr)((UInt32)(sharedAddr) + offset);

    Memory_set(GateMP_module->remoteSystemInUse, 0,
            GateMP_module->global->numRemoteSystem);

    delegateReservedMask = GateMP_RemoteSystemProxy_getReservedMask();

    if (delegateReservedMask != NULL) {
        for (i = 0; i < GateMP_module->global->numRemoteSystem; i++) {
            if (delegateReservedMask[i >> 5] & (1 << (i % 32))) {
                GateMP_module->remoteSystemInUse[i] = RESERVED;
            }
        }
    }

    if (SharedRegion_isCacheEnabled(0)) {
        Cache_wbInv(GateMP_module->remoteSystemInUse,
                GateMP_module->global->numRemoteSystem * sizeof(UInt8),
                Cache_Type_ALL, TRUE);
    }

    /*
     *  initialize the in-use array in shared memory for the custom1 gates.
     *  Need to check if this proxy is the same as system
     */
    offset = _Ipc_roundup(GateMP_module->global->numRemoteSystem, minAlign);
    if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
        GateMP_ProxyOrder_CUSTOM1) {
        if (GateMP_module->global->numRemoteCustom1 != 0) {
            GateMP_module->remoteCustom1InUse =
                GateMP_module->remoteSystemInUse + offset;
        }

        Memory_set(GateMP_module->remoteCustom1InUse, 0,
                GateMP_module->global->numRemoteCustom1);

        delegateReservedMask = GateMP_RemoteCustom1Proxy_getReservedMask();

        if (delegateReservedMask != NULL) {
            for (i = 0; i < GateMP_module->global->numRemoteCustom1; i++) {
                if (delegateReservedMask[i >> 5] & (1 << (i % 32))) {
                    GateMP_module->remoteCustom1InUse[i] = RESERVED;
                }
            }
        }
        if (SharedRegion_isCacheEnabled(0)) {
            Cache_wbInv(GateMP_module->remoteCustom1InUse,
                    GateMP_module->global->numRemoteCustom1 * sizeof(UInt8),
                    Cache_Type_ALL, TRUE);
        }
    }
    else {
        GateMP_module->remoteCustom1InUse = GateMP_module->remoteSystemInUse;
        GateMP_module->remoteCustom1Gates = GateMP_module->remoteSystemGates;
    }

    /*
     *  initialize the in-use array in shared memory for the custom2 gates.
     *  Need to check if this proxy is the same as system or custom1
     */
    offset = _Ipc_roundup(GateMP_module->global->numRemoteCustom1, minAlign);
    if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
        GateMP_ProxyOrder_CUSTOM2) {
        if (GateMP_module->global->numRemoteCustom2 != 0) {
            GateMP_module->remoteCustom2InUse =
                GateMP_module->remoteCustom1InUse + offset;
        }

        Memory_set(GateMP_module->remoteCustom2InUse, 0,
            GateMP_module->global->numRemoteCustom2);

        delegateReservedMask = GateMP_RemoteCustom2Proxy_getReservedMask();

        if (delegateReservedMask != NULL) {
            for (i = 0; i < GateMP_module->global->numRemoteCustom2; i++) {
                if (delegateReservedMask[i >> 5] & (1 << (i % 32))) {
                    GateMP_module->remoteCustom2InUse[i] = RESERVED;
                }
            }
        }
        if (SharedRegion_isCacheEnabled(0)) {
            Cache_wbInv(GateMP_module->remoteCustom2InUse,
                    GateMP_module->global->numRemoteCustom2 * sizeof(UInt8),
                    Cache_Type_ALL, TRUE);
        }
    }
    else if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
             GateMP_ProxyOrder_CUSTOM1) {
        GateMP_module->remoteCustom2InUse = GateMP_module->remoteCustom1InUse;
        GateMP_module->remoteCustom2Gates = GateMP_module->remoteCustom1Gates;
    }
    else {
        GateMP_module->remoteCustom2InUse = GateMP_module->remoteSystemInUse;
        GateMP_module->remoteCustom2Gates = GateMP_module->remoteSystemGates;
    }

    GT_0trace (curTrace, GT_LEAVE, "GateMP_setRegion0Reserved");
}

/*
 *  ======== GateMP_clearRegion0ReservedSize ========
 */
Void GateMP_clearRegion0Reserved(Void)
{
    GT_0trace (curTrace, GT_ENTER, "GateMP_clearRegion0Reserved");


    GT_0trace (curTrace, GT_LEAVE, "GateMP_clearRegion0Reserved");
}


/*
 *  ======== GateMP_openRegion0Reserved ========
 */
Void GateMP_openRegion0Reserved(Ptr sharedAddr)
{
    GateMP_Reserved *reserve;
    UInt32 minAlign, offset;

    GT_1trace (curTrace, GT_ENTER, "GateMP_openRegion0Reserved", sharedAddr);

    minAlign = Memory_getMaxDefaultTypeAlign();
    if (SharedRegion_getCacheLineSize(0) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize(0);
    }

    /* setup GateMP_Reserved fields */
    reserve = (GateMP_Reserved *)sharedAddr;

    if (reserve->version != GateMP_VERSION) {
        /* TBD */
        return;
    }

    offset = _Ipc_roundup(sizeof(GateMP_Reserved), minAlign);
    GateMP_module->remoteSystemInUse = (Ptr)((UInt32)(sharedAddr) + offset);

    /*
     *  initialize the in-use array in shared memory for the custom1 gates.
     *  Need to check if this proxy is the same as system
     */
    offset = _Ipc_roundup(GateMP_module->global->numRemoteSystem, minAlign);
    if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
        GateMP_ProxyOrder_CUSTOM1) {
        if (GateMP_module->global->numRemoteCustom1 != 0) {
            GateMP_module->remoteCustom1InUse =
                GateMP_module->remoteSystemInUse + offset;
        }
    }
    else {
        GateMP_module->remoteCustom1InUse = GateMP_module->remoteSystemInUse;
        GateMP_module->remoteCustom1Gates = GateMP_module->remoteSystemGates;
    }

    offset = _Ipc_roundup(GateMP_module->global->numRemoteCustom1, minAlign);
    if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
        GateMP_ProxyOrder_CUSTOM2) {
        if (GateMP_module->global->numRemoteCustom2 != 0) {
            GateMP_module->remoteCustom2InUse =
                GateMP_module->remoteCustom1InUse + offset;
        }
    }
    else if (GateMP_module->global->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
             GateMP_ProxyOrder_CUSTOM1) {
        GateMP_module->remoteCustom2InUse = GateMP_module->remoteCustom1InUse;
        GateMP_module->remoteCustom2Gates = GateMP_module->remoteCustom1Gates;
    }
    else {
        GateMP_module->remoteCustom2InUse = GateMP_module->remoteSystemInUse;
        GateMP_module->remoteCustom2Gates = GateMP_module->remoteSystemGates;
    }

    GT_0trace (curTrace, GT_LEAVE, "GateMP_openRegion0Reserved");
}

/*
 *  ======== GateMP_closeRegion0Reserved ========
 */
Void GateMP_closeRegion0Reserved(Ptr sharedAddr)
{

    GT_1trace (curTrace, GT_ENTER, "GateMP_closeRegion0Reserved", sharedAddr);

    GT_0trace (curTrace, GT_LEAVE, "GateMP_closeRegion0Reserved");
}


/*
 *  ======== GateMP_start ========
 */
Int GateMP_start(Ptr sharedAddr)
{
    IArg                key;
    SharedRegion_Entry  entry;
    GateMP_Params       params;
    Int                 status;

    GT_1trace(curTrace, GT_ENTER, "GateMP_start:", sharedAddr);

    status = GateMP_S_SUCCESS;

    key = IpcMemMgr_enterGate();

    /* check if GateMP already started */
    if (GateMP_module->global->started) {
        status = GateMP_S_ALREADYSETUP; // TODO: is this an error?
    }

    /* if default owner, do nothing (too early) */
    if (status == GateMP_S_SUCCESS) {
        SharedRegion_getEntry(0, &entry);
        if (entry.ownerProcId == MultiProc_INVALIDID) {
            status = GateMP_S_ALREADYSETUP;  // TODO: new status code?
        }
    }

    /* if not owner, do nothing */
    if (status == GateMP_S_SUCCESS) {
        if (entry.ownerProcId != MultiProc_self()) {
            status = GateMP_S_ALREADYSETUP;  // TODO: new status code?
        }
    }

    /* do startup task */
    if (status == GateMP_S_SUCCESS) {

        /* intialize the locks */
        GateMP_RemoteSystemProxy_locksinit();
        GateMP_RemoteCustom1Proxy_locksinit();
        GateMP_RemoteCustom2Proxy_locksinit();

        /* initialize GateMP reserved header in SR#0 */
        GateMP_setRegion0Reserved(sharedAddr);

        /* init params for default gate */
        GateMP_Params_init(&params);
        params.sharedAddr = (Ptr)((UInt32)sharedAddr +
                GateMP_getRegion0ReservedSize());
//      params.localProtect = GateMP_LocalProtect_PROCESS;
        params.localProtect = GateMP_LocalProtect_TASKLET;      /* TODO */

        if (MultiProc_getNumProcessors() > 1) {
            params.remoteProtect = GateMP_RemoteProtect_SYSTEM;
        }
        else {
            params.remoteProtect = GateMP_RemoteProtect_NONE;
        }

        /* create default GateMP instance */
        GateMP_module->defaultGate = GateMP_create(&params);

        /* set the default GateMP for creator */
        if (GateMP_module->defaultGate == NULL) {
            status = GateMP_E_FAIL;
        }
    }

    /* cache the GateMP SR#0 header offset in global structure */
    if (status == GateMP_S_SUCCESS) {
        GateMP_module->global->started = TRUE;
        GateMP_module->global->gateMPSharedOff = sharedAddr - entry.base;
        GateMP_module->attachRefCount = 1;
    }

    IpcMemMgr_leaveGate(key);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_start: [0x%08x]", status);

    return (status);
}

/*
 *  ======== GateMP_stop ========
 *  Internal
 */
Int GateMP_stop(Void)
{
    Int status = GateMP_S_SUCCESS;

    GT_0trace(curTrace, GT_ENTER, "GateMP_stop:");

    GT_assert(curTrace, (GateMP_module->global->started));

    /* delete the default gate instance */
    status = GateMP_delete(&GateMP_module->defaultGate);

    /* clear GateMP reserved header */
    GateMP_clearRegion0Reserved();

    /* clear the started flag */
    GateMP_module->global->started = FALSE;

    GT_1trace(curTrace, GT_LEAVE, "GateMP_stop:", status);

    return(status);
}


/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */
UInt GateMP_getFreeResource(UInt8 *inUse, Int num)
{
    IArg key = 0;
    Bool flag = FALSE;
    UInt resourceId;
    GateMP_Handle defaultGate;

    GT_2trace (curTrace, GT_ENTER, "GateMP_getFreeResource", inUse, num);

    /* Need to look at shared memory. Enter default gate */
    defaultGate = GateMP_getDefaultRemote();

    if (defaultGate != NULL){
        key = GateMP_enter(defaultGate);
    }

    /* Invalidate cache before looking at the in-use flags */
    if (SharedRegion_isCacheEnabled(0)) {
        Cache_inv(inUse, num * sizeof(UInt8), Cache_Type_ALL, TRUE);
    }

    /*  Find a free resource id. Note: zero is reserved in the
     *  system proxy for the default gate.
     */
    for (resourceId = 0; resourceId < num; resourceId++) {
        /*
         *  If not in-use, set the inUse to TRUE to prevent other
         *  creates from getting this one.
         */
        if (inUse[resourceId] == UNUSED) {

           flag = TRUE;

           /* Denote in shared memory that the resource is used */
           inUse[resourceId] = USED;
           break;
       }
    }

    /* Write-back if a in-use flag was changed */
    if (flag == TRUE && SharedRegion_isCacheEnabled(0)) {
        Cache_wbInv(inUse, num * sizeof(UInt8), Cache_Type_ALL, TRUE);
    }

    /* Done with the critical section */
    if (defaultGate) {
        GateMP_leave(defaultGate, key);
    }

    if (flag == FALSE) {
        resourceId = -1;
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_getFreeResource", resourceId);

    return (resourceId);
}

/*
 *  ======== GateMP_create ========
 */
GateMP_Handle GateMP_create(const GateMP_Params *params)
{
    _GateMP_Params      _params;
    GateMP_Handle       handle;

    GT_1trace(curTrace, GT_ENTER, "GateMP_create:", params);

    Memory_set(&_params, 0, sizeof(_GateMP_Params));
    Memory_copy(&_params, (Ptr)params, sizeof(GateMP_Params));

    handle = _GateMP_create(&_params);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_create:", handle);

    return(handle);
}

/*
 *  ======== _GateMP_create ========
 */
GateMP_Handle _GateMP_create(const _GateMP_Params *params)
{
    GateMP_Object *obj;
    IArg key;

    GT_1trace(curTrace, GT_ENTER, "_GateMP_create: params=0x%08x", params);

    /* allocate the instance object */
    obj = (GateMP_Object *)Memory_calloc(NULL, sizeof(GateMP_Object), 0, NULL);

    if (obj != NULL) {
        obj->status = GateMP_Instance_init(obj, params);

        if (obj->status == 0) {
            key = Gate_enterSystem();
            if (GateMP_firstObject == NULL) {
                GateMP_firstObject = obj;
                obj->next = NULL;
            }
            else {
                obj->next = GateMP_firstObject;
                GateMP_firstObject = obj;
            }
            Gate_leaveSystem(key);
        }
        else {
            GateMP_Instance_finalize(obj, obj->status);
            Memory_free(NULL, obj, sizeof(GateMP_Object));
            obj = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "_GateMP_create: [0x%08x]", obj);

    return((GateMP_Handle)obj);
}

/*
 *  ======== GateMP_delete ========
 */
Int GateMP_delete(GateMP_Handle *handle)
{
    IArg            key;
    GateMP_Object * tmp;
    Int             status;

    GT_1trace (curTrace, GT_ENTER, "GateMP_delete", handle);

    status = GateMP_S_SUCCESS;

    if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
    }
    else if (*handle == NULL) {
        status = GateMP_E_INVALIDARG;
    }

    key = Gate_enterSystem();

    if (status == GateMP_S_SUCCESS) {
        if (GateMP_firstObject == NULL) {
            status = GateMP_E_INVALIDARG;
        }
    }

    if (status == GateMP_S_SUCCESS) {
        if ((Ptr)(*handle) == (Ptr)GateMP_firstObject) {
            GateMP_firstObject = GateMP_firstObject->next;
        }
        else {
            for (tmp = GateMP_firstObject; tmp != NULL; tmp = tmp->next) {
                if (tmp->next == *handle) {
                    tmp->next = ((GateMP_Object *)(*handle))->next;
                    break;
                }
            }
            if (tmp == NULL) {
                status = GateMP_E_INVALIDARG;
            }
        }
    }

    Gate_leaveSystem(key);

    if (status == GateMP_S_SUCCESS) {
        GateMP_Instance_finalize((GateMP_Object *)(*handle), 0);
        Memory_free(NULL, (*handle), sizeof (GateMP_Object));
        *handle = NULL;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_delete: [0x%08x]", status);

    return(status);
}

/*
 *  ======== GateMP_attach ========
 */
Int GateMP_attach(UInt16 remoteProcId, Ptr sharedAddr)
{
    Int                 status;
    IArg                key;
    Ptr                 gateMPsharedAddr;
    SharedRegion_Entry  entry;
    GateMP_Handle       defaultGate;

    status = GateMP_S_SUCCESS;

    key = IpcMemMgr_enterGate();

    /* TODO: assert that GateMP is started */

    GateMP_module->attachRefCount++;

    if (GateMP_module->attachRefCount > 1) {
        status = GateMP_S_ALREADYSETUP;
    }

    /* TODO: assert that default gate is NULL */

    /* if sharedAddr null, compute from global offset */
    if (sharedAddr == NULL) {
        SharedRegion_getEntry(0, &entry);
        sharedAddr = entry.base + GateMP_module->global->gateMPSharedOff;
    }

    if (status == GateMP_S_SUCCESS) {

        /* get region 0 information */
//      SharedRegion_getEntry(0, &entry);

//      if ((entry.ownerProcId != MultiProc_self()) &&
//          (entry.ownerProcId != MultiProc_INVALIDID)) {

//          GateMP_module->global->isOwner = FALSE;

            /* if not the owner of the SharedRegion */
            GateMP_openRegion0Reserved(sharedAddr);

            /* open the gate by address */
            gateMPsharedAddr = (Ptr)((UInt32)sharedAddr +
                    GateMP_getRegion0ReservedSize());
            status = GateMP_openByAddr(gateMPsharedAddr, &defaultGate);

            if (status >= 0) {
                /* set the default GateMP for opener */
                GateMP_module->defaultGate = defaultGate;
            }
//      }
    }

    IpcMemMgr_leaveGate(key);

    return(status);
}


/*
 *  ======== GateMP_detach ========
 */
Int GateMP_detach(UInt16 remoteProcId, Ptr sharedAddr)
{
//  GateMP_Handle       defGate;
    IArg                key;
    Int                 status;
    SharedRegion_Entry  entry;

    status = GateMP_S_SUCCESS;

    key = IpcMemMgr_enterGate();

    GateMP_module->attachRefCount--;
    if (GateMP_module->attachRefCount == 0) {

        /* if sharedAddr null, compute from global offset */
        if (sharedAddr == NULL) {
            SharedRegion_getEntry(0, &entry);
            sharedAddr = entry.base + GateMP_module->global->gateMPSharedOff;
        }

        /* if entry owner proc is not specified return */
//      if (GateMP_module->global->isOwner == FALSE) {
            GateMP_closeRegion0Reserved(sharedAddr);
            status = GateMP_close(&GateMP_module->defaultGate);

            /* set the default GateMP for opener */
//          GateMP_module->defaultGate = NULL;
//      }
    }

    IpcMemMgr_leaveGate(key);

    return(status);
}
