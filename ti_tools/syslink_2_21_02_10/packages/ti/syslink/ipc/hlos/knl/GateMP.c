/*
 *  @file   GateMP.c
 *
 *  @brief      gate wrapper implementation
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
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/inc/_Ipc.h>

/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/GateMPDefs.h>


/* -----------------------------------------------------------------------------
 * Macros
 * -----------------------------------------------------------------------------
 */
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
} GateMP_Attrs;

/* Structure defining state of GateMP Module */
typedef struct GateMP_Module_State {
    NameServer_Handle      nameServer;
    Int                    numRemoteSystem;
    Int                    numRemoteCustom1;
    Int                    numRemoteCustom2;
    UInt8 *                remoteSystemInUse;
    UInt8 *                remoteCustom1InUse;
    UInt8 *                remoteCustom2InUse;
    IGateProvider_Handle * remoteSystemGates;
    IGateProvider_Handle * remoteCustom1GatesAlloc;
    IGateProvider_Handle * remoteCustom1Gates;
    IGateProvider_Handle * remoteCustom2GatesAlloc;
    IGateProvider_Handle * remoteCustom2Gates;
    IGateProvider_Handle   gateHwi;
    IGateProvider_Handle   gateMutex;
    IGateProvider_Handle   gateNull;
    GateMP_Handle          defaultGate;
    Int                    proxyMap[GateMP_ProxyOrder_NUM];
    Int32                  refCount;
    GateMP_Config          cfg;
    /* Current config values */
    GateMP_Config          defaultCfg;
    /* default config values */
    GateMP_Params          defInstParams;
    /* default instance params */
    Bool                   isOwner;
    /* Indicates if this processor is the owner */
    Int32                  attachRefCount;
    /* Attach/detach reference count */
} GateMP_Module_State;

/* Structure defining instance of GateMP Module */
typedef struct GateMP_Object {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
    IOBJECT_SuperObject;       /* For inheritance for IObject */
    GateMP_RemoteProtect   remoteProtect;
    GateMP_LocalProtect    localProtect;
    Ptr                    nsKey;
    Int                    numOpens;
    UInt16                 creatorProcId;
    Bool                   cacheEnabled;
    GateMP_Attrs *         attrs;
    UInt16                 regionId;
    SizeT                  allocSize;
    Ptr                    proxyAttrs;
    UInt                   resourceId;
    IGateProvider_Handle   gateHandle;
    Ipc_ObjType            objType; /* from shared region? */
} GateMP_Object;

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
} _GateMP_Params;

/* -----------------------------------------------------------------------------
 * Forward declaration
 * -----------------------------------------------------------------------------
 */
IGateProvider_Handle GateMP_createLocal (GateMP_LocalProtect localProtect);
Void GateMP_setRegion0Reserved  (Ptr sharedAddr);
Void GateMP_clearRegion0Reserved  (Void);
Void GateMP_openRegion0Reserved (Ptr sharedAddr);
Void GateMP_closeRegion0Reserved (Ptr sharedAddr);
Void GateMP_setDefaultRemote    (GateMP_Handle handle);
UInt GateMP_getFreeResource(UInt8 *inUse, Int num);
GateMP_Handle _GateMP_create (const _GateMP_Params * params);

/* -----------------------------------------------------------------------------
 * Globals
 * -----------------------------------------------------------------------------
 */
static GateMP_Module_State GateMP_state =
{
    .defaultCfg.numResources       = 32,
    .defaultCfg.defaultProtection  = GateMP_LocalProtect_INTERRUPT,
    .defaultCfg.maxRunTimeEntries  = NameServer_ALLOWGROWTH,
    .defInstParams.sharedAddr      = 0x0,
    .defInstParams.regionId        = 0x0,
    .refCount                      = 0x0,
    .attachRefCount                = 0x0,
    .defaultGate                   = NULL
};

static GateMP_Module_State *    GateMP_module = &GateMP_state;
static GateMP_Object *          GateMP_firstObject = NULL;

/* -----------------------------------------------------------------------------
 * APIs
 * -----------------------------------------------------------------------------
 */

/*
 *  ======== GateMP_getSharedParams ========
 */
Void GateMP_getConfig(GateMP_Config *cfgParams)
{
    IArg        key;
    Int32       refCount;

    GT_1trace(curTrace, GT_ENTER,
        "GateMP_getConfig: cfgParams=0x%08x", cfgParams);

    GT_assert(curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_getConfig",
            GateMP_E_INVALIDARG,
            "Argument of type (GateMP_Config *) passed is null!");
        return;
    }
#endif

    /* acquire the module reference count */
    key = Gate_enterSystem();
    refCount = GateMP_module->refCount;
    Gate_leaveSystem(key);

    if (refCount == 0) {
        Memory_copy((Ptr)cfgParams, (Ptr)&GateMP_module->defaultCfg,
            sizeof(GateMP_Config));
    }
    else {
        Memory_copy((Ptr)cfgParams, (Ptr)&GateMP_module->cfg,
            sizeof (GateMP_Config));
    }

    GT_0trace(curTrace, GT_LEAVE, "GateMP_getConfig");
}

/*
 *  ======== GateMP_setup ========
 */
Int32 GateMP_setup(const GateMP_Config *cfg)
{
    Int32               status = GateMP_S_SUCCESS;
    Error_Block         eb;
    IArg                key;
    Int                 i;
    NameServer_Params   params;

    GT_1trace(curTrace, GT_ENTER, "GateMP_setup: cfg=0x%08x", cfg);
    Error_init(&eb);

    key = Gate_enterSystem();
    GateMP_module->refCount++;

    /* if already setup, then leave gate and return (success) */
    if (GateMP_module->refCount > 1) {
        Gate_leaveSystem(key);
        status = GateMP_S_ALREADYSETUP;
        GT_0trace(curTrace, GT_2CLASS, "GateMP Module already initialized!");
        goto leave;
    }

    /* save given config or use default config */
    if (cfg != NULL) {
        Memory_copy((Ptr)&GateMP_module->cfg, (Ptr)cfg, sizeof(GateMP_Config));
    }
    else {
        Memory_copy((Ptr)&GateMP_module->cfg, (Ptr)&GateMP_module->defaultCfg,
            sizeof(GateMP_Config));
    }

    /* initialize module state */
    GateMP_module->defaultGate = NULL;

    for (i = 0; i < GateMP_ProxyOrder_NUM; i++) {
        GateMP_module->proxyMap[i] = i;
    }

    if ((Ptr)GateMP_RemoteCustom1Proxy_create ==
        (Ptr)GateMP_RemoteSystemProxy_create) {

        GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM1] =
            GateMP_module->proxyMap[GateMP_ProxyOrder_SYSTEM];
    }

    if ((Ptr)GateMP_RemoteSystemProxy_create ==
        (Ptr)GateMP_RemoteCustom2Proxy_create) {

        GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] =
            GateMP_module->proxyMap[GateMP_ProxyOrder_SYSTEM];
    }
    else if ((Ptr)GateMP_RemoteCustom2Proxy_create ==
        (Ptr)GateMP_RemoteCustom1Proxy_create) {

        GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] =
            GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM1];
    }

    /* get number of configured instances from the plugged in proxy gates */
    GateMP_module->numRemoteSystem = GateMP_RemoteSystemProxy_getNumInstances();
    GateMP_module->numRemoteCustom1 =
        GateMP_RemoteCustom1Proxy_getNumInstances();
    GateMP_module->numRemoteCustom2 =
        GateMP_RemoteCustom2Proxy_getNumInstances();

    /* create mutex gate */
    GateMP_module->gateMutex = (IGateProvider_Handle)
        GateMutex_create((GateMutex_Params*)NULL, &eb);

    if (GateMP_module->gateMutex == NULL) {
        status = GateMP_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
            status, "GateMutex_create failed!");
    }

    /* create name server */
    if (status >= 0) {
        NameServer_Params_init(&params);
        params.maxRuntimeEntries = GateMP_module->cfg.maxRunTimeEntries;
        params.maxNameLen = GateMP_module->cfg.maxNameLen;
        params.maxValueLen = 2 * sizeof(UInt32);

        GateMP_module->nameServer = NameServer_create("GateMP", &params);

        if (GateMP_module->nameServer == NULL) {
            status = GateMP_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                status, "NameServer_create failed!");
        }
    }

    /* allocate memory for remote system gate handles */
    if (status >= 0) {
        if (GateMP_module->numRemoteSystem > 0) {
            GateMP_module->remoteSystemGates = Memory_calloc(NULL,
                GateMP_module->numRemoteSystem * sizeof(IGateProvider_Handle),
                0, NULL);

            if (GateMP_module->remoteSystemGates == NULL) {
                status = GateMP_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "remoteSystemGates allocation failed!");
            }
        }
        else {
            GateMP_module->remoteSystemGates = NULL;
        }
    }

    /* allocate memory for remote custom1 gate handles */
    if (status >= 0) {
        if ((GateMP_module->numRemoteCustom1 > 0) &&
           (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
            GateMP_ProxyOrder_CUSTOM1)) {

            GateMP_module->remoteCustom1GatesAlloc =
            GateMP_module->remoteCustom1Gates = Memory_calloc(NULL,
                GateMP_module->numRemoteCustom1 * sizeof(IGateProvider_Handle),
                0, NULL);

            if (GateMP_module->remoteCustom1GatesAlloc == NULL) {
                status = GateMP_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "remoteCustom1Gates allocation failed!");
            }
        }
        else {
            GateMP_module->remoteCustom1GatesAlloc =
            GateMP_module->remoteCustom1Gates = NULL;
        }
    }

    /* allocate memory for remote custom2 gate handles */
    if (status >= 0) {
        if ((GateMP_module->numRemoteCustom2 > 0) &&
           (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
            GateMP_ProxyOrder_CUSTOM2)) {

            GateMP_module->remoteCustom2GatesAlloc =
            GateMP_module->remoteCustom2Gates = Memory_calloc(NULL,
                GateMP_module->numRemoteCustom2 * sizeof(IGateProvider_Handle),
                0, NULL);

            if (GateMP_module->remoteCustom2GatesAlloc == NULL) {
                status = GateMP_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "remoteCustom2Gates allocation failed!");
            }
        }
        else {
            GateMP_module->remoteCustom2GatesAlloc =
            GateMP_module->remoteCustom2Gates = NULL;
        }
    }

    /* in failure case, release acquired resources in reverse order */
    if (status < 0) {
        if (GateMP_module->remoteCustom2GatesAlloc != NULL) {
            Memory_free (NULL, GateMP_module->remoteCustom2GatesAlloc,
                GateMP_module->numRemoteCustom2 * sizeof(IGateProvider_Handle));
            GateMP_module->remoteCustom2GatesAlloc = NULL;
        }
        if (GateMP_module->remoteCustom1GatesAlloc != NULL) {
            Memory_free(NULL, GateMP_module->remoteCustom1GatesAlloc,
                GateMP_module->numRemoteCustom1 * sizeof(IGateProvider_Handle));
            GateMP_module->remoteCustom1GatesAlloc = NULL;
        }
        if (GateMP_module->remoteSystemGates != NULL) {
            Memory_free(NULL, GateMP_module->remoteSystemGates,
                GateMP_module->numRemoteSystem * sizeof(IGateProvider_Handle));
            GateMP_module->remoteSystemGates = NULL;
        }
        if (GateMP_module->nameServer != NULL) {
            NameServer_delete(&GateMP_module->nameServer);
        }
        if (GateMP_module->gateMutex != NULL) {
            GateMutex_delete((GateMutex_Handle *)&GateMP_module->gateMutex);
        }
    }

    Gate_leaveSystem(key);

leave:
    GT_1trace(curTrace, GT_LEAVE, "GateMP_setup: [0x%08x]", status);

    /*! @retval GateMP_S_SUCCESS Operation successful */
    return(status);
}

/*
 *  ======== GateMP_destroy ========
 */
Int32 GateMP_destroy(Void)
{
    Int32       status = GateMP_S_SUCCESS;
    IArg        key;

    GT_0trace(curTrace, GT_ENTER, "GateMP_destroy");

    key = Gate_enterSystem();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateMP_module->refCount <= 0) {
        Gate_leaveSystem(key);
        /*! @retval GateMP_E_INVALIDSTATE Module was not initialized */
        status = GateMP_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_destroy",
            status, "Module was not initialized!");
        goto leave;
    }
#endif

    GateMP_module->refCount--;

    /* if not last user, leave gate and return (success) */
    if (GateMP_module->refCount > 0) {
        Gate_leaveSystem(key);
        status = GateMP_S_SUCCESS;
        goto leave;
    }

    /* release all resources */
    if (GateMP_module->remoteCustom2GatesAlloc != NULL) {
        Memory_free(NULL, GateMP_module->remoteCustom2GatesAlloc,
            GateMP_module->numRemoteCustom2 * sizeof(IGateProvider_Handle));
        GateMP_module->remoteCustom2GatesAlloc = NULL;
    }
    if (GateMP_module->remoteCustom1GatesAlloc != NULL) {
        Memory_free(NULL, GateMP_module->remoteCustom1GatesAlloc,
            GateMP_module->numRemoteCustom1 * sizeof(IGateProvider_Handle));
        GateMP_module->remoteCustom1GatesAlloc = NULL;
    }
    if (GateMP_module->remoteSystemGates != NULL) {
        Memory_free(NULL, GateMP_module->remoteSystemGates,
            GateMP_module->numRemoteSystem * sizeof(IGateProvider_Handle));
        GateMP_module->remoteSystemGates = NULL;
    }
    if (GateMP_module->nameServer != NULL) {
        NameServer_delete(&GateMP_module->nameServer);
    }
    if (GateMP_module->gateMutex != NULL) {
        GateMutex_delete((GateMutex_Handle *)&GateMP_module->gateMutex);
    }

    /* clear cfg area */
    Memory_set((Ptr)&GateMP_module->cfg, 0, sizeof(GateMP_Config));

    GateMP_module->isOwner = FALSE;
    Gate_leaveSystem (key);

leave:
    GT_1trace(curTrace, GT_LEAVE, "GateMP_destroy: [0x%08x]", status);

    /*! @retval GateMP_S_SUCCESS Operation successful */
    return(status);
}

/*
 *  ======== GateMP_getSharedParams ========
 */
static Void GateMP_getSharedParams(      GateMP_Params *  sparams,
                                   const _GateMP_Params * params)
{
    GT_2trace (curTrace, GT_ENTER, "GateMP_getSharedParams", sparams, params);

    sparams->name             = params->name;
    sparams->regionId         = params->regionId;
    sparams->sharedAddr       = params->sharedAddr;
    sparams->localProtect     = (GateMP_LocalProtect) params->localProtect;
    sparams->remoteProtect    = (GateMP_RemoteProtect) params->remoteProtect;

    GT_0trace (curTrace, GT_LEAVE, "GateMP_getSharedParams");
}

/*
 *  ======== GateMP_Params_init ========
 */
Void GateMP_Params_init(GateMP_Params *params)
{
    GT_1trace(curTrace, GT_ENTER, "GateMP_Params_init", params);
    params->name = NULL;
    params->regionId = 0;
    params->sharedAddr = NULL;
    params->localProtect = GateMP_LocalProtect_INTERRUPT;
    params->remoteProtect = GateMP_RemoteProtect_SYSTEM;

    GT_0trace(curTrace, GT_LEAVE, "GateMP_Params_init");
}

/*
 *  ======== GateMP_Instance_init ========
 */
Int GateMP_Instance_init(GateMP_Object *obj, const _GateMP_Params *params)
{
    IArg                             key;
    IGateProvider_Handle             remoteHandle;
    GateMP_RemoteSystemProxy_Params  systemParams;
    GateMP_RemoteCustom1Proxy_Params custom1Params;
    GateMP_RemoteCustom2Proxy_Params custom2Params;
    UInt32                           minAlign;
    UInt32                           offset;
    SharedRegion_SRPtr               sharedShmBase;
    GateMP_Params                    sparams;
    UInt32                           nsValue[2];
    Int                              status = 0;
    IHeap_Handle                     regionHeap;
    IGateProvider_Handle             localHandle;

    GT_2trace(curTrace, GT_ENTER, "GateMP_Instance_init", obj, params);

    /* Initialize resourceId to an invalid value */
    obj->resourceId = (UInt)-1;
    localHandle = GateMP_createLocal(params->localProtect);

    /* Open GateMP instance */
    if (params->openFlag == TRUE) {
        /* all open work done here except for remote gateHandle */
        obj->localProtect  = params->localProtect;
        obj->remoteProtect = params->remoteProtect;
        obj->nsKey         = 0;
        obj->numOpens      = 1;
        obj->creatorProcId = MultiProc_INVALIDID;
        obj->attrs         = (GateMP_Attrs *)params->sharedAddr;
        obj->regionId      = SharedRegion_getId ((Ptr)obj->attrs);
        obj->cacheEnabled  = SharedRegion_isCacheEnabled (obj->regionId);
        obj->objType       = Ipc_ObjType_OPENDYNAMIC;

        /* Assert that the buffer is in a valid shared region */
        if (obj->regionId == SharedRegion_INVALIDREGIONID) {
            status = 1;
        }

        if (status == 0) {
            obj->allocSize = 0;

            minAlign = Memory_getMaxDefaultTypeAlign ();

            if (SharedRegion_getCacheLineSize (obj->regionId) > minAlign) {
                    minAlign = SharedRegion_getCacheLineSize (obj->regionId);
            }

            offset = _Ipc_roundup (sizeof (GateMP_Attrs), minAlign);

            obj->proxyAttrs = (Ptr)((UInt32)obj->attrs + offset);
        }
    }
    /* Create GateMP instance */
    else {
        obj->localProtect  = params->localProtect;
        obj->remoteProtect = params->remoteProtect;
        obj->nsKey         = 0;
        obj->numOpens      = 0;
        obj->creatorProcId = MultiProc_self ();

        if (obj->remoteProtect == GateMP_RemoteProtect_NONE) {
            /* Creating a local gate (Attrs is in local memory) */
            /* all work done here and return */
            obj->gateHandle = GateMP_createLocal (obj->localProtect);


            if (params->sharedAddr != NULL) {
                obj->attrs = params->sharedAddr;
                obj->objType = Ipc_ObjType_CREATEDYNAMIC;
                obj->attrs = params->sharedAddr;
                /* Need cache settings since attrs is in shared memory */
                obj->regionId = SharedRegion_getId((Ptr)obj->attrs);
                obj->cacheEnabled = SharedRegion_isCacheEnabled(obj->regionId);
            }
            else {
                obj->objType = Ipc_ObjType_LOCAL;
                obj->regionId = SharedRegion_INVALIDREGIONID;
                obj->cacheEnabled  = FALSE; /* local */
                /* Using default target alignment */
                obj->attrs = Memory_calloc(NULL, sizeof(GateMP_Attrs), 0, NULL);
                if (obj->attrs == NULL) {
                    return (2);
                }
            }
            if (status == 0) {
                obj->attrs->arg = (Bits32)obj;
                obj->attrs->mask = SETMASK (obj->remoteProtect,
                                            obj->localProtect);
                obj->attrs->creatorProcId = obj->creatorProcId;
                obj->attrs->status = GateMP_CREATED;
                if (obj->cacheEnabled) {
                    /*
                     *  Need to write back memory if cache is enabled because
                     *  cache will be invalidated during openByAddr
                     */
                    Cache_wbInv(obj->attrs,
                                sizeof(GateMP_Attrs),
                                Cache_Type_ALL,
                                TRUE);
                }
                if (params->name != NULL) {
                    nsValue[0] = (UInt32)obj->attrs;
                    /*
                     *  Top 16 bits = procId of creator
                     *  Bottom 16 bits = '0' if local, '1' otherwise
                     */
                    nsValue[1] = MultiProc_self () << 16;
                    obj->nsKey = NameServer_add (GateMP_module->nameServer,
                                                 params->name,
                                                 &nsValue,
                                                 2 * sizeof (UInt32));
                }
            }
        }
        else {
            if (params->sharedAddr == NULL) {
                /* If sharedAddr = NULL we are creating dynamically from the heap */
                obj->objType = Ipc_ObjType_CREATEDYNAMIC_REGION;
                                obj->regionId = params->regionId;
                GateMP_getSharedParams(&sparams, params);
                obj->allocSize = GateMP_sharedMemReq(&sparams);
                obj->cacheEnabled  = SharedRegion_isCacheEnabled(obj->regionId);

                /* The region heap will do the alignment */
                regionHeap = SharedRegion_getHeap(obj->regionId);
                GT_assert (curTrace, (regionHeap != NULL));

                obj->attrs = Memory_calloc(regionHeap, obj->allocSize, 0, NULL);
                if (obj->attrs == NULL) {
                    status = 3;
                }

                if (status == 0) {
                    minAlign = Memory_getMaxDefaultTypeAlign();

                    if (  SharedRegion_getCacheLineSize (obj->regionId)
                        > minAlign) {
                        minAlign = SharedRegion_getCacheLineSize(obj->regionId);
                    }

                    offset = _Ipc_roundup(sizeof(GateMP_Attrs), minAlign);

                    obj->proxyAttrs = (Ptr)((UInt32)obj->attrs + offset);
                }
            }
            else {
                /* creating using sharedAddr */
                obj->regionId = SharedRegion_getId(params->sharedAddr);
                /* Assert that the buffer is in a valid shared region */
                if (obj->regionId == SharedRegion_INVALIDREGIONID) {
                    status = 4;
                }

                /* Assert that sharedAddr is cache aligned */
                if (   (status == 0)
                    && (  ((UInt32)params->sharedAddr
                        % SharedRegion_getCacheLineSize(obj->regionId)) != 0)) {
                    status = 5;
                }

                if (status == 0) {
                    obj->objType = Ipc_ObjType_CREATEDYNAMIC;
                    obj->attrs = (GateMP_Attrs *)params->sharedAddr;
                    obj->cacheEnabled  =
                                    SharedRegion_isCacheEnabled (obj->regionId);
                    minAlign = Memory_getMaxDefaultTypeAlign();
                    if (  SharedRegion_getCacheLineSize(obj->regionId)
                        > minAlign) {
                        minAlign = SharedRegion_getCacheLineSize(obj->regionId);
                    }
                    offset = _Ipc_roundup(sizeof(GateMP_Attrs), minAlign);
                    obj->proxyAttrs = (Ptr)((UInt32)obj->attrs + offset);
                }
            }
        }
    }

    /* Proxy work for open and create done here */
    switch (obj->remoteProtect) {
        case GateMP_RemoteProtect_SYSTEM:
            if (obj->objType != Ipc_ObjType_OPENDYNAMIC) {/* Created Instance */
                obj->resourceId = GateMP_getFreeResource(
                    GateMP_module->remoteSystemInUse,
                    GateMP_module->numRemoteSystem);
                if (obj->resourceId == -1) {
                    status = 6;
                }
            }
            else {
                /* resourceId set by open call */
                obj->resourceId = params->resourceId;
            }

            if (status == 0) {
                /* Create the proxy object */
                GateMP_RemoteSystemProxy_Params_init(&systemParams);
                systemParams.resourceId = obj->resourceId;
                systemParams.openFlag = (obj->objType == Ipc_ObjType_OPENDYNAMIC);
                systemParams.sharedAddr = obj->proxyAttrs;
                systemParams.regionId = obj->regionId;
                remoteHandle = (IGateProvider_Handle)
                                GateMP_RemoteSystemProxy_create(
                                                                 localHandle,
                                                                 &systemParams);

                if (remoteHandle == NULL) {
                    status = 7;
                }

                if (status == 0) {
                    /* Finish filling in the object */
                    obj->gateHandle = (IGateProvider_Handle)(remoteHandle);

                    /* Fill in the local array because it is cooked */
                    key = Gate_enterSystem();
                    GateMP_module->remoteSystemGates[obj->resourceId] =
                                                      (IGateProvider_Handle)obj;
                    Gate_leaveSystem(key);
                }
            }
            break;

        case GateMP_RemoteProtect_CUSTOM1:
            if (obj->objType != Ipc_ObjType_OPENDYNAMIC) {/* Created Instance */
                obj->resourceId = GateMP_getFreeResource(
                                             GateMP_module->remoteCustom1InUse,
                                             GateMP_module->numRemoteCustom1);
                if (obj->resourceId == -1) {
                    status = 6;
                }
            }
            else {
                /* resourceId set by open call */
                obj->resourceId = params->resourceId;
            }

            if (status == 0) {
                /* Create the proxy object */
                GateMP_RemoteCustom1Proxy_Params_init(&custom1Params);
                custom1Params.resourceId = obj->resourceId;
                custom1Params.openFlag = (obj->objType == Ipc_ObjType_OPENDYNAMIC);
                custom1Params.sharedAddr = obj->proxyAttrs;
                custom1Params.regionId = obj->regionId;
                remoteHandle = (IGateProvider_Handle)
                                    GateMP_RemoteCustom1Proxy_create(
                                                             localHandle,
                                                             &custom1Params);
                if (remoteHandle == NULL) {
                    status = 7;
                }

                if (status == 0) {
                    /* Finish filling in the object */
                    obj->gateHandle = (IGateProvider_Handle)(remoteHandle);

                    /* Fill in the local array because it is cooked*/
                    key = Gate_enterSystem();
                    GateMP_module->remoteCustom1Gates[obj->resourceId] =
                                                      (IGateProvider_Handle)obj;

                    Gate_leaveSystem(key);
                }
            }
            break;

        case GateMP_RemoteProtect_CUSTOM2:
            if (obj->objType != Ipc_ObjType_OPENDYNAMIC) {/* Created Instance */
                obj->resourceId = GateMP_getFreeResource(
                                         GateMP_module->remoteCustom2InUse,
                                         GateMP_module->numRemoteCustom2);
                if (obj->resourceId == -1) {
                    status = 6;
                }
            }
            else {
                   /* resourceId set by open call */
                obj->resourceId = params->resourceId;
            }

            if (status == 0) {
                /* Create the proxy object */
                GateMP_RemoteCustom2Proxy_Params_init(&custom2Params);
                custom2Params.resourceId = obj->resourceId;
                custom2Params.openFlag = (obj->objType == Ipc_ObjType_OPENDYNAMIC);
                custom2Params.sharedAddr = obj->proxyAttrs;
                custom2Params.regionId = obj->regionId;
                remoteHandle = (IGateProvider_Handle)
                                    GateMP_RemoteCustom2Proxy_create(
                                                             localHandle,
                                                             &custom2Params);
                if (remoteHandle == NULL) {
                    status = 7;
                }

                if (status == 0) {
                    /* Finish filling in the object */
                    obj->gateHandle = (IGateProvider_Handle)(remoteHandle);

                    /* Fill in the local array because it is cooked*/
                    key = Gate_enterSystem();
                    GateMP_module->remoteCustom2Gates[obj->resourceId] =
                                                      (IGateProvider_Handle)obj;

                    Gate_leaveSystem(key);
                }
            }
            break;

        default:
            break;
    }

    /* Place Name/Attrs into NameServer table */
    if ((obj->objType != Ipc_ObjType_OPENDYNAMIC) && (status == 0)) {
        /* Fill in the attrs */
        obj->attrs->arg = obj->resourceId;
        obj->attrs->mask = SETMASK(obj->remoteProtect, obj->localProtect);
        obj->attrs->creatorProcId = obj->creatorProcId;
        obj->attrs->status = GateMP_CREATED;

        if (obj->cacheEnabled) {
            Cache_wbInv(obj->attrs, sizeof(GateMP_Attrs), Cache_Type_ALL, TRUE);
        }

        if (params->name != NULL) {
            sharedShmBase = SharedRegion_getSRPtr(obj->attrs, obj->regionId);
            nsValue[0] = (UInt32)sharedShmBase;
            /*
             *  Top 16 bits = procId of creator
             *  Bottom 16 bits = '0' if local, '1' otherwise
             */
            nsValue[1] = MultiProc_self() << 16 | 1;
            obj->nsKey = NameServer_add(GateMP_module->nameServer,
                                params->name, &nsValue, 2 * sizeof(UInt32));
            if (obj->nsKey == NULL) {
                status = 8;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "GateMP_Instance_init",
                                     status,
                                     "NameServer_add Failed!");
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_Instance_init: [0x%08x]", status);

    return status;
}


/*
 *  ======== GateMP_Instance_finalize ========
 */
Void GateMP_Instance_finalize (GateMP_Object *obj, Int status)
{
    IArg systemKey = (IArg) 0;
    GateMP_RemoteSystemProxy_Handle systemHandle;
    GateMP_RemoteCustom1Proxy_Handle custom1Handle;
    GateMP_RemoteCustom2Proxy_Handle custom2Handle;
    Int _status = 0;
    GateMP_Handle *remoteHandles = NULL;
    UInt8 *inUseArray = NULL;
    UInt numResources = 0;

    GT_2trace (curTrace, GT_ENTER, "GateMP_Instance_finalize", obj, status);

    /* Cannot call when numOpen is non-zero. */
    if (obj->numOpens != 0) {
        _status = GateMP_E_INVALIDSTATE;
    }

    if (_status == 0) {
        /* Removed from NameServer */
        if (obj->nsKey != 0) {
            NameServer_removeEntry(GateMP_module->nameServer, obj->nsKey);
        }

        /* Set the status to 0 */
        if (obj->objType != Ipc_ObjType_OPENDYNAMIC) {
            obj->attrs->status = 0;
            if (obj->cacheEnabled) {
                Cache_wbInv(obj->attrs,
                            sizeof(GateMP_Attrs),
                            Cache_Type_ALL,
                            TRUE);
            }
        }

        /*
         *  If ObjType_LOCAL, memory was allocated from the local system heap.
         *  obj->attrs might be NULL if the Memory_alloc failed in Instance_init
         */
        if (obj->objType == Ipc_ObjType_LOCAL && obj->attrs != NULL) {
            Memory_free(NULL, obj->attrs, sizeof(GateMP_Attrs));
        }

        /* Delete if a remote gate */
        switch (obj->remoteProtect) {
            /* Delete proxy instance... need to downCast */
            case GateMP_RemoteProtect_SYSTEM:
                if (obj->gateHandle) {
                    systemHandle =
                        (GateMP_RemoteSystemProxy_Handle)(obj->gateHandle);
                    GateMP_RemoteSystemProxy_delete(&systemHandle);
                }

                inUseArray = GateMP_module->remoteSystemInUse;
                remoteHandles = (GateMP_Handle *)
                        GateMP_module->remoteSystemGates;
                numResources = GateMP_module->numRemoteSystem;

                break;

            case GateMP_RemoteProtect_CUSTOM1:
                if (obj->gateHandle) {
                        custom1Handle =
                        (GateMP_RemoteCustom1Proxy_Handle)(obj->gateHandle);
                        GateMP_RemoteCustom1Proxy_delete(&custom1Handle);
                }

                inUseArray = GateMP_module->remoteCustom1InUse;
                remoteHandles = (GateMP_Handle *)
                                          GateMP_module->remoteCustom1Gates;
                numResources = GateMP_module->numRemoteCustom1;
                break;


            case GateMP_RemoteProtect_CUSTOM2:
                if (obj->gateHandle) {
                    custom2Handle =
                        (GateMP_RemoteCustom2Proxy_Handle)(obj->gateHandle);
                        GateMP_RemoteCustom2Proxy_delete(&custom2Handle);
                }

                inUseArray = GateMP_module->remoteCustom2InUse;
                remoteHandles = (GateMP_Handle *)
                                          GateMP_module->remoteCustom2Gates;
                numResources = GateMP_module->numRemoteCustom2;
                break;
            case GateMP_RemoteProtect_NONE:
            /*
             *  Nothing else to finalize. Any alloc'ed memory has already been
             *  freed
             */
            return;

            default:
                /* Nothing to do */
                break;
        }
        /* Clear the handle array entry in local memory */
        if (obj->resourceId != (UInt)-1) {
            remoteHandles[obj->resourceId] = NULL;
        }

        if ((obj->objType != Ipc_ObjType_OPENDYNAMIC)
            && (obj->resourceId != (UInt)-1)) {

            /* Only enter default gate if not deleting default gate. */
            if (obj != GateMP_module->defaultGate) {
                systemKey = GateMP_enter(GateMP_module->defaultGate);
            }

            /* clear the resource used flag in shared memory */
            inUseArray[obj->resourceId] = UNUSED;
            if (obj->cacheEnabled) {
                Cache_wbInv(inUseArray, numResources, Cache_Type_ALL, TRUE);
            }

            /* Only enter default gate if not deleting default gate. */
            if (obj != GateMP_module->defaultGate) {
                GateMP_leave(GateMP_module->defaultGate, systemKey);
            }
        }

        if (obj->objType == Ipc_ObjType_CREATEDYNAMIC_REGION) {
            /* Free memory allocated from the region heap */
            if (obj->attrs) {
                Memory_free(SharedRegion_getHeap(obj->regionId), obj->attrs,
                            obj->allocSize);
            }
        }
    }
    GT_0trace (curTrace, GT_LEAVE, "GateMP_Instance_finalize");
}


/*
 *  ======== GateMP_enter ========
 */
IArg GateMP_enter (GateMP_Object * obj)
{
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "GateMP_enter", obj);
    key = IGateProvider_enter(obj->gateHandle);
    GT_1trace (curTrace, GT_LEAVE, "GateMP_enter", key);

    return (key);
}

/*
 *  ======== GateMP_leave ========
 */
Void GateMP_leave (GateMP_Object * obj, IArg key)
{
    GT_2trace (curTrace, GT_ENTER, "GateMP_leave", obj, key);
    IGateProvider_leave(obj->gateHandle, key);
    GT_0trace (curTrace, GT_LEAVE, "GateMP_leave");
}

/*
 *  ======== GateMP_open ========
 */
Int GateMP_open(String name, GateMP_Handle *handle)
{
    SharedRegion_SRPtr sharedShmBase;
    Int status;
    UInt32 len;
    Ptr sharedAddr;
    UInt32 nsValue[2];

    GT_2trace (curTrace, GT_ENTER, "GateMP_open", name, handle);

    /* Assert that a pointer has been supplied */
    if (handle == NULL) {
        status = GateMP_E_INVALIDARG;
    }
    else {
        len = sizeof(nsValue);
        /*
         *  Get the Attrs out of the NameServer instance.
         *  Search all processors.
         */
        status = NameServer_get (GateMP_module->nameServer,
                                 name,
                                 &nsValue,
                                 &len,
                                 NULL);

        if (status < 0) {
            *handle = NULL;
            return (GateMP_E_NOTFOUND);
        }

        /*
         * The least significant bit of nsValue[1] == 0 means its a
         * local GateMP, otherwise its a remote GateMP.
         */
        if (((nsValue[1] & 0x1) == 0) &&
            ((nsValue[1] >> 16) != MultiProc_self())) {
            /* Trying to open a local GateMP remotely */
            *handle = NULL;
            return (GateMP_E_FAIL);
        }

        if ((nsValue[1] & 0x1) == 0) {
            /*
             * Opening a local GateMP locally. The GateMP is created
             * from a local heap so don't do SharedRegion Ptr conversion.
             */
            sharedAddr = (Ptr)nsValue[0];
        }
        else {
            /* Opening a remote GateMP. Need to do SharedRegion Ptr conversion. */
            sharedShmBase = (SharedRegion_SRPtr)nsValue[0];
            sharedAddr = SharedRegion_getPtr(sharedShmBase);
        }

        status = GateMP_openByAddr(sharedAddr, handle);
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_open", status);

    return (status);
}

/*
 *  ======== GateMP_openByAddr ========
 */
Int GateMP_openByAddr(Ptr sharedAddr, GateMP_Handle *handle)
{
    Int status = GateMP_S_SUCCESS;
    IArg key;
    GateMP_Object *obj = NULL;
    _GateMP_Params params;
    GateMP_Attrs *attrs;
    UInt16 regionId;

    GT_2trace (curTrace, GT_ENTER, "GateMP_openByAddr", sharedAddr, handle);

    attrs = (GateMP_Attrs *)sharedAddr;

    /* get the region id and invalidate attrs is needed */
    regionId = SharedRegion_getId(sharedAddr);
    if (regionId != SharedRegion_INVALIDREGIONID) {
        if (SharedRegion_isCacheEnabled(regionId)) {
            Cache_inv(attrs, sizeof(GateMP_Attrs), Cache_Type_ALL, TRUE);
        }
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
                obj = (GateMP_Handle)attrs->arg;
                *handle = obj;
                obj->numOpens++;
                Gate_leaveSystem(key);
            }
        }
        else {
            /* remote case */
            switch (GETREMOTE(attrs->mask)) {
                case GateMP_RemoteProtect_SYSTEM:
                    obj = (GateMP_Object *)
                                   GateMP_module->remoteSystemGates[attrs->arg];
                    break;

                case GateMP_RemoteProtect_CUSTOM1:
                    obj = (GateMP_Object *)
                                  GateMP_module->remoteCustom1Gates[attrs->arg];
                    break;

                case GateMP_RemoteProtect_CUSTOM2:
                    obj = (GateMP_Object *)
                                  GateMP_module->remoteCustom2Gates[attrs->arg];
                    break;

                default:
                    status = GateMP_E_FAIL;
                    /* TODO: add failure */
                    break;
            }

            /*
             *  If the object is NULL, then it must have been created on a remote
             *  processor. Need to create a local object. This is accomplished
             *  by setting the openFlag to TRUE.
             */
            if ((status == GateMP_S_SUCCESS) && (obj == NULL)) {
                /* create a GateMP object with the openFlag set to true */
                params.name = NULL;
                params.openFlag = TRUE;
                params.sharedAddr = sharedAddr;
                params.resourceId = attrs->arg;
                params.localProtect = GETLOCAL(attrs->mask);
                params.remoteProtect = GETREMOTE(attrs->mask);

                obj = _GateMP_create(&params);
                if (obj == NULL) {
                    status = GateMP_E_FAIL;
                }
            }
            else {
                obj->numOpens++;
            }

            /* Return the "opened" GateMP instance  */
            *handle = obj;
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
    IArg key;
    GateMP_Handle gateHandle = *handle;
    Int count;
    Int status = GateMP_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "GateMP_close", handle);

    /*
     *  Cannot call with the numOpens equal to zero.  This is either
     *  a created handle or been closed already.
     */
    if (gateHandle->numOpens == 0) {
        status = GateMP_E_INVALIDSTATE;
    }
    else {
        key = Gate_enterSystem();
        count = --gateHandle->numOpens;
        Gate_leaveSystem(key);

        /*
         *  if the count is zero and the gate is opened, then this
         *  object was created in the open (i.e. the create happened
         *  on a remote processor.
         */
        if ((count == 0) && (gateHandle->objType == Ipc_ObjType_OPENDYNAMIC)) {
            GateMP_delete(&gateHandle);
        }
        *handle = NULL;
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_close", status);

    return (status);
}

/*
 *  ======== GateMP_getSharedAddr ========
 */
SharedRegion_SRPtr GateMP_getSharedAddr(GateMP_Object *obj)
{
    SharedRegion_SRPtr srPtr;

    GT_1trace (curTrace, GT_ENTER, "GateMP_getSharedAddr", obj);

    srPtr = SharedRegion_getSRPtr(obj->attrs, obj->regionId);

    GT_1trace (curTrace, GT_LEAVE, "GateMP_getSharedAddr", srPtr);

    return (srPtr);
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
    return (GateMP_module->defaultGate);
}

/*
 *  ======== GateMP_getLocalProtect ========
 */
GateMP_LocalProtect GateMP_getLocalProtect(GateMP_Object *obj)
{
    GT_assert (curTrace, (obj != NULL));

    return ((GateMP_LocalProtect)obj->localProtect);
}

/*
 *  ======== GateMP_getRemoteProtect ========
 */
GateMP_RemoteProtect GateMP_getRemoteProtect(GateMP_Object *obj)
{
    GT_assert (curTrace, (obj != NULL));

    return ((GateMP_RemoteProtect)obj->remoteProtect);
}

/*
 *  ======== GateMP_createLocal ========
 */
IGateProvider_Handle GateMP_createLocal(GateMP_LocalProtect localProtect)
{
    IGateProvider_Handle gateHandle = IGateProvider_NULL;

    GT_1trace(curTrace, GT_ENTER, "GateMP_createLocal: localProtect=%d", localProtect);

    /* Create the local gate. */
    switch (localProtect) {
        case GateMP_LocalProtect_NONE:
            /* Plug with the GateNull singleton */
            gateHandle = IGateProvider_NULL;
            break;

        case GateMP_LocalProtect_INTERRUPT:
            /* Plug with the GateHwi singleton */
            gateHandle = Gate_systemHandle;
            break;

        case GateMP_LocalProtect_TASKLET:
            /* Plug with the GateSwi singleton */
            gateHandle = GateMP_module->gateMutex;
            break;

        case GateMP_LocalProtect_THREAD:
        case GateMP_LocalProtect_PROCESS:
            /* Plug with the GateMutexPri singleton */
            gateHandle = GateMP_module->gateMutex;
            break;

        default:
            break;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_createLocal: [0x%08x]", gateHandle);

    return (gateHandle);
}

/*
 *  ======== GateMP_sharedMemReq ========
 */
SizeT GateMP_sharedMemReq(const GateMP_Params *params)
{
    SizeT memReq, minAlign;
    UInt16 regionId;
    GateMP_RemoteSystemProxy_Params systemParams;
    GateMP_RemoteCustom1Proxy_Params custom1Params;
    GateMP_RemoteCustom2Proxy_Params custom2Params;

    GT_1trace (curTrace, GT_ENTER, "GateMP_sharedMemReq", params);

    if (params->sharedAddr) {
        regionId = SharedRegion_getId(params->sharedAddr);
    }
    else {
        regionId = params->regionId;
    }

    minAlign = Memory_getMaxDefaultTypeAlign();
    if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize(regionId);
    }

    memReq = _Ipc_roundup(sizeof(GateMP_Attrs), minAlign);

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

    GT_1trace (curTrace, GT_LEAVE, "GateMP_sharedMemReq", memReq);

    return (memReq);
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

    reserved += _Ipc_roundup(GateMP_module->numRemoteSystem, minAlign);

    if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
        GateMP_ProxyOrder_CUSTOM1) {
        reserved += _Ipc_roundup(GateMP_module->numRemoteCustom1, minAlign);
    }

    if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
        GateMP_ProxyOrder_CUSTOM2) {
        reserved += _Ipc_roundup(GateMP_module->numRemoteCustom2, minAlign);
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_getRegion0ReservedSize", reserved);

    return (reserved);
}

/*
 *  ======== GateMP_setRegion0ReservedSize ========
 */
Void GateMP_setRegion0Reserved(Ptr sharedAddr)
{
    GateMP_Reserved *reserve;
    SizeT minAlign, offset;
    UInt i;
    Bits32 *delegateReservedMask;

    GT_1trace(curTrace, GT_ENTER,
        "GateMP_setRegion0Reserved: sharedAddr=0x%08x", sharedAddr);

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
    GateMP_module->remoteSystemInUse = (Ptr)((UInt32)sharedAddr + offset);

    memset(GateMP_module->remoteSystemInUse, 0, GateMP_module->numRemoteSystem);
    delegateReservedMask = GateMP_RemoteSystemProxy_getReservedMask();
    if (delegateReservedMask != NULL) {
        for (i = 0; i < GateMP_module->numRemoteSystem; i++) {
            if (delegateReservedMask[i >> 5] & (1 << (i % 32))) {
                GateMP_module->remoteSystemInUse[i] = RESERVED;
            }
        }
    }
    if (SharedRegion_isCacheEnabled(0)) {
        Cache_wbInv(GateMP_module->remoteSystemInUse,
                    GateMP_module->numRemoteSystem, Cache_Type_ALL, TRUE);
    }

    /*
     *  initialize the in-use array in shared memory for the custom1 gates.
     *  Need to check if this proxy is the same as system
     */
    offset = _Ipc_roundup(GateMP_module->numRemoteSystem, minAlign);
    if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
        GateMP_ProxyOrder_CUSTOM1) {
        if (GateMP_module->numRemoteCustom1 != 0) {
            GateMP_module->remoteCustom1InUse =
                GateMP_module->remoteSystemInUse + offset;
        }

        memset(GateMP_module->remoteCustom1InUse, 0,
                GateMP_module->numRemoteCustom1);
        delegateReservedMask = GateMP_RemoteCustom1Proxy_getReservedMask();
        if (delegateReservedMask != NULL) {
            for (i = 0; i < GateMP_module->numRemoteCustom1; i++) {
                if (delegateReservedMask[i >> 5] & (1 << (i % 32))) {
                    GateMP_module->remoteCustom1InUse[i] = RESERVED;
                }
            }
        }
        if (SharedRegion_isCacheEnabled(0)) {
            Cache_wbInv(GateMP_module->remoteCustom1InUse,
                     GateMP_module->numRemoteCustom1, Cache_Type_ALL, TRUE);
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
    offset = _Ipc_roundup(GateMP_module->numRemoteCustom1, minAlign);
    if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
        GateMP_ProxyOrder_CUSTOM2) {
        if (GateMP_module->numRemoteCustom2 != 0) {
                GateMP_module->remoteCustom2InUse =
                    GateMP_module->remoteCustom1InUse + offset;
        }
        memset(GateMP_module->remoteCustom2InUse, 0,
            GateMP_module->numRemoteCustom2);
        delegateReservedMask = GateMP_RemoteCustom2Proxy_getReservedMask();
        if (delegateReservedMask != NULL) {
            for (i = 0; i < GateMP_module->numRemoteCustom2; i++) {
                if (delegateReservedMask[i >> 5] & (1 << (i % 32))) {
                    GateMP_module->remoteCustom2InUse[i] = RESERVED;
                }
            }
        }
        if (SharedRegion_isCacheEnabled(0)) {
            Cache_wbInv(GateMP_module->remoteCustom2InUse,
                 GateMP_module->numRemoteCustom2, Cache_Type_ALL, TRUE);
        }
    }
    else if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
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
    GateMP_module->remoteSystemInUse = (Ptr)((UInt32)sharedAddr + offset);

    offset = _Ipc_roundup(GateMP_module->numRemoteSystem, minAlign);
    if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM1] ==
        GateMP_ProxyOrder_CUSTOM1) {
        if (GateMP_module->numRemoteCustom1 != 0) {
            GateMP_module->remoteCustom1InUse =
                GateMP_module->remoteSystemInUse + offset;
        }
    }
    else {
        GateMP_module->remoteCustom1InUse = GateMP_module->remoteSystemInUse;
        GateMP_module->remoteCustom1Gates = GateMP_module->remoteSystemGates;
    }

    offset = _Ipc_roundup(GateMP_module->numRemoteCustom1, minAlign);
    if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
        GateMP_ProxyOrder_CUSTOM2) {
        if (GateMP_module->numRemoteCustom2 != 0) {
            GateMP_module->remoteCustom2InUse =
                GateMP_module->remoteCustom1InUse + offset;
        }
    }
    else if (GateMP_module->proxyMap[GateMP_ProxyOrder_CUSTOM2] ==
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
 *  ======== GateMP_setDefaultRemote() ========
 */
Void GateMP_setDefaultRemote(GateMP_Handle handle)
{
    GT_1trace (curTrace, GT_ENTER, "GateMP_setDefaultRemote", handle);

    GateMP_module->defaultGate = handle;
    GT_0trace (curTrace, GT_LEAVE, "GateMP_setDefaultRemote");
}

/*
 *  ======== GateMP_start ========
 */
Int GateMP_start(Ptr sharedAddr)
{
    SharedRegion_Entry entry;
    GateMP_Params      gateMPParams;
    GateMP_Handle      defaultGate;
    Int                status = GateMP_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "GateMP_start", sharedAddr);

    /* get region 0 information */
    SharedRegion_getEntry(0, &entry);

    /* if entry owner proc is not specified return */
    if (entry.ownerProcId != MultiProc_INVALIDID) {

        if (entry.ownerProcId == MultiProc_self()) {
            /* Intialize the locks if ncessary.*/
            GateMP_RemoteSystemProxy_locksinit ();
            GateMP_RemoteCustom1Proxy_locksinit ();
            GateMP_RemoteCustom2Proxy_locksinit ();
        }

        /* Init params for default gate */
        GateMP_Params_init(&gateMPParams);
        gateMPParams.sharedAddr = (Ptr)((UInt32)sharedAddr +
                                        GateMP_getRegion0ReservedSize());
        gateMPParams.localProtect  = GateMP_LocalProtect_TASKLET;

        if (MultiProc_getNumProcessors() > 1) {
            gateMPParams.remoteProtect = GateMP_RemoteProtect_SYSTEM;
        }
        else {
            gateMPParams.remoteProtect = GateMP_RemoteProtect_NONE;
        }

        if (entry.ownerProcId == MultiProc_self()) {
            GateMP_module->isOwner = TRUE;
            /* if owner of the SharedRegion */
            GateMP_setRegion0Reserved(sharedAddr);

            /* create default GateMP */
            defaultGate = GateMP_create(&gateMPParams);

            if (defaultGate != NULL) {
                /* set the default GateMP for creator */
                GateMP_setDefaultRemote(defaultGate);
            }
            else {
                status = GateMP_E_FAIL;
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_start", status);

    return (status);
}

/*
 *  ======== GateMP_stop ========
 */
Int GateMP_stop(Void)
{
    Int status = GateMP_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "GateMP_stop");

    /* if entry owner proc is not specified return */
    if (GateMP_module->isOwner == TRUE) {
        /* if owner of the SharedRegion */
        GateMP_clearRegion0Reserved ();

        GateMP_delete (&GateMP_module->defaultGate);

        /* set the default GateMP for creator */
        GateMP_setDefaultRemote(NULL);
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_stop", status);

    return (status);
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

    if (defaultGate){
        key = GateMP_enter(defaultGate);
    }

    /* Invalidate cache before looking at the in-use flags */
    if (SharedRegion_isCacheEnabled(0)) {
        Cache_inv(inUse, num * sizeof(UInt8), Cache_Type_ALL, TRUE);
    }

    /*
     *  Find a free resource id. Note: zero is reserved on the
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

    return(resourceId);
}


/*
 *  ======== GateMP_create ========
 */
GateMP_Handle GateMP_create (const GateMP_Params * params)
{
    _GateMP_Params _params;
    GateMP_Handle  handle;

    GT_1trace(curTrace, GT_ENTER, "GateMP_create: gateMutex=0x%08x", params);

    memset (&_params, 0, sizeof (_GateMP_Params));
    memcpy (&_params, params, sizeof (GateMP_Params));

    handle =  _GateMP_create (&_params);

    GT_1trace (curTrace, GT_LEAVE, "GateMP_create", handle);

    return handle;
}

/*
 *  ======== _GateMP_create ========
 */
GateMP_Handle _GateMP_create(const _GateMP_Params *params)
{
    GateMP_Object *     obj;
    IArg                key;

    GT_1trace(curTrace, GT_ENTER, "_GateMP_create: params=0x%08x", params);

    obj = (GateMP_Object *)Memory_calloc(NULL, sizeof(GateMP_Object), 0, NULL);

    if (obj != NULL) {
        obj->status = GateMP_Instance_init(obj, params);

        if (obj->status == 0) {
            key = Gate_enterSystem ();
            if (GateMP_firstObject == NULL) {
                GateMP_firstObject = obj;
                obj->next = NULL;
            }
            else {
                obj->next = GateMP_firstObject;
                GateMP_firstObject = obj;
            }
            Gate_leaveSystem (key);
        }
        else {
            Memory_free (NULL, obj, sizeof (GateMP_Object));
            obj = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "_GateMP_create: [0x%08x]", obj);

    return(GateMP_Handle)obj;
}

/*
 *  ======== GateMP_delete ========
 */
Int GateMP_delete (GateMP_Handle * handle)
{
    IArg key;
    GateMP_Object * temp;
    Int status  = GateMP_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "GateMP_delete", handle);

    if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
    }
    if (*handle == NULL) {
        status = GateMP_E_INVALIDARG;
    }

    if (status >= 0) {
        key = Gate_enterSystem ();
        if ((GateMP_Object *)*handle == GateMP_firstObject) {
            GateMP_firstObject = (*handle)->next;
        }
        else {
            temp = GateMP_firstObject;
            while (temp) {
                if (temp->next == (*handle)) {
                    temp->next = (*handle)->next;
                    break;
                }
                else {
                    temp = temp->next;
                }
            }
            if (temp == NULL) {
                Gate_leaveSystem (key);
                status = GateMP_E_INVALIDARG;
            }
        }

        if (status >= 0) {
            Gate_leaveSystem (key);
            GateMP_Instance_finalize (*handle, (*handle)->status);
            Memory_free (NULL, (*handle), sizeof (GateMP_Object));
            *handle = NULL;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_delete", status);

    return GateMP_S_SUCCESS;
}


/*
 *  ======== GateMP_attach ========
 */
Int GateMP_attach(UInt16 remoteProcId, Ptr sharedAddr)
{
    Int                status = GateMP_S_SUCCESS;
    Ptr                gateMPsharedAddr;
    SharedRegion_Entry entry;
    GateMP_Handle      defaultGate;

    GateMP_module->attachRefCount++;
    if (GateMP_module->attachRefCount > 1) {
        status = GateMP_S_ALREADYSETUP;
    }
    else {
        /* get region 0 information */
        SharedRegion_getEntry (0, &entry);

        gateMPsharedAddr = (Ptr)((UInt32)sharedAddr +
                                 GateMP_getRegion0ReservedSize());

        if ((entry.ownerProcId != MultiProc_self()) &&
            (entry.ownerProcId != MultiProc_INVALIDID)) {
            GateMP_module->isOwner = FALSE;

            /* if not the owner of the SharedRegion */
            GateMP_openRegion0Reserved(sharedAddr);

            /* open the gate by address */
            status = GateMP_openByAddr(gateMPsharedAddr, &defaultGate);

            if (status >= 0) {
                /* set the default GateMP for opener */
                GateMP_setDefaultRemote(defaultGate);
            }
        }
    }

    return (status);
}


/*
 *  ======== GateMP_detach ========
 */
Int GateMP_detach (UInt16 remoteProcId, Ptr sharedAddr)
{
    Int status = GateMP_S_SUCCESS;

    GateMP_module->attachRefCount--;
    if (GateMP_module->attachRefCount == 0) {
        /* if entry owner proc is not specified return */
        if (GateMP_module->isOwner == FALSE) {
            GateMP_closeRegion0Reserved (sharedAddr);

            status = GateMP_close (&GateMP_module->defaultGate);

            /* set the default GateMP for opener */
            GateMP_setDefaultRemote(NULL);
        }
    }

    return (status);
}
