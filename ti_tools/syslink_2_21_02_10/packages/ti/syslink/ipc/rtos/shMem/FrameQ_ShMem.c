/*
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
 */

/**
 *  @file   ti/syslink/ipc/rtos/shMem/FrameQ_ShMem.c
 *
 *  @brief  Implements FrameQ on Shared memory created out of shared region.
 */

/*-------------------------    XDC  specific includes ----------------------  */
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>

#include <ti/sdo/utils/List.h>

/*-------------------------  BIOS includes ---------------------------------- */
#include <ti/sysbios/gates/GateMutex.h>

/*-------------------------    Generic includes  ---------------------------- */
#include <string.h>

/*-------------------------    IPC module specific includes ------------------*/
#include <ti/sdo/ipc/Ipc.h>

#include <ti/syslink/inc/ClientNotifyMgr.h>

#ifdef xdc_target__isaCompatible_64P
#include <ti/sysbios/family/c64p/Cache.h>
#else
#include <ti/sysbios/hal/Cache.h>
#endif

//#include "package/internal/FrameQ_ShMem.xdc.h"
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/ListMP.h>

#define ti_sdo_ipc_GateMP__nolocalnames
#include <ti/sdo/ipc/GateMP.h>
#undef ti_sdo_ipc_GateMP__nolocalnames

#include <ti/syslink/utils/Trace.h>


//#include <ti/syslink/ipc/rtos/_FrameQBufMgr.h>
#include <ti/syslink/ipc/_FrameQBufMgr.h>

#include <ti/syslink/inc/_FrameQ_ShMem.h>
#include <ti/syslink/inc/_FrameQ.h>


#define FRAMEQ_NAMESERVERNAME  "FrameQ"

#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

extern Ptr gFrameQ_nsHandle;
/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*
 * Interface functions.Structure defining Interface functions for FrameQBufMgr.
 */
FrameQ_Fxns FrameQ_ShMem_fxns = {
    (fq_create) FrameQ_ShMem_create,
    (fq_delete) FrameQ_ShMem_delete,
    (fq_open)   FrameQ_ShMem_open,
     NULL,//FrameQ_ShMem_openByAddress
    (fq_close)  FrameQ_ShMem_close,
    (fq_alloc)  FrameQ_ShMem_alloc,
    (fq_allocv) FrameQ_ShMem_allocv,
    (fq_free)   FrameQ_ShMem_free,
    (fq_freev)  FrameQ_ShMem_freev,
    (fq_put)    FrameQ_ShMem_put,
    (fq_putv)   FrameQ_ShMem_putv,
    (fq_get)    FrameQ_ShMem_get,
    (fq_getv)   FrameQ_ShMem_getv,
    (fq_dup)    FrameQ_ShMem_dup,
    (fq_registerNotifier)       FrameQ_ShMem_registerNotifier,
    (fq_unregisterNotifier)     FrameQ_ShMem_unregisterNotifier,
    (fq_sendNotify)             FrameQ_ShMem_sendNotify,
    (fq_getNumFrames)           FrameQ_ShMem_getNumFrames,
    (fq_getvNumFrames)          FrameQ_ShMem_getvNumFrames,
    (fq_control)                FrameQ_ShMem_control,
    (fq_getCliNotifyMgrHandle) _FrameQ_ShMem_getCliNotifyMgrHandle,
    (fq_getCliNotifyMgrGate)   _FrameQ_ShMem_getCliNotifyMgrGate,
    (fq_getCliNotifyMgrShAddr)  _FrameQ_ShMem_getCliNotifyMgrShAddr,
    (fq_getCliNotifyMgrGateShAddr) _FrameQ_ShMem_getCliNotifyMgrGateShAddr,
    (fq_setNotifyId)               _FrameQ_ShMem_setNotifyId,
    (fq_resetNotifyId)             _FrameQ_ShMem_resetNotifyId,
    (fq_getNumFreeFrames)           FrameQ_ShMem_getNumFreeFrames,
    (fq_getvNumFreeFrames)          FrameQ_ShMem_getvNumFreeFrames
};

/*
 * @brief Structure for FrameQ module state
 */
typedef struct FrameQ_ShMem_ModuleObject_Tag {
    UInt32              refCount;
    /* TBD : can not be atomic */
    NameServer_Handle       nameServer;
    /*!< Handle to the local NameServer used for storing FrameQ_ShMem instance
     *   information.
     */
    List_Handle             objList;
    /* List holding created objects */
    IGateProvider_Handle    gate;
    IGateProvider_Handle    listLock;
    /* Handle to lock for protecting objList */
    FrameQ_ShMem_Config     cfg;
    /*!<  Current module wide config values*/
    FrameQ_ShMem_Config     defaultCfg;
    /*!< Default config values*/
    FrameQ_ShMem_Params     defInstParams;
    /*!< Default instance configuration values*/
    Ptr                   defaultInstGate;
    /*!< Gate used for instances if useDefaultgate is set.
     * if defaultInstGate is null, module uses GateMP's
     * default gate
     */
} FrameQ_ShMem_ModuleObject;

/*
 * @brief Structure defining internal object for the FrameQ_ShMem
 */
typedef struct FrameQ_ShMem_Obj_Tag {
    List_Elem                            listElem;
    String                               name;
    /*!< Name of the instance.*/
    Ptr                                  nsKey;
    /*!< Nameserver key*/
    UInt32                               regionId;
    /*!< region id of the shared region  */
    UInt32                               allocSize;
    /*!< Shared memory allocated       */

    GateMP_Handle                        gate;
    /*!< Gate handle for protection */
    Bool                                 creGate;
    /*!< Gate handle for protection */
    UInt32                               gateMPAllocSize;
    /*!< Gate handle for protection */
    Ptr                                  instGateSharedAddr;
    /*!< Gate handle for protection */
    UInt32                               objType;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32                               interfaceType;
    /* Type of interface */
    UInt32                               objMode;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32                                minAlign;
    /* cache line alignment */
    Ptr                                   pnameserverEntry;
    /* Nameserverentry in the object */
    volatile FrameQ_ShMem_Attrs          *attrs;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    volatile FrameQ_ShMem_WriterClient   *writer;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    volatile FrameQ_ShMem_ReaderClient   **reader;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    volatile FrameQ_ShMem_FrameQueue     **frmQueue;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    /*!< Mode of the obejct. FrameQ_ShMem_MODE_READER  or FrameQ_ShMem_MODE_WRITER*/
    volatile Ptr                          virtClientHandle;
    /*!< Pointer to the shared client information*/
    ClientNotifyMgr_Handle                clientNotifyMgrHandle;
    /*!< Handle to the clientNotifyMgr handke used for this instance */
    GateMP_Handle                         clientNotifyMgrGate;
    /* Gate created  to pass to clientNotifyMgr*/
    FrameQBufMgr_Handle                   frameQBufMgrHandle ;
    /*!< Handle of FrameBufMgr that this instance is using.*/
    UInt32                                frameQBufMgrId;
    /*!< Id of the FrameBufMgr that this instance is using.*/
    UInt32                                notifyRegId;
    /*!< Id of the FrameBufMgr that this instance is using.*/
    Bool                                  ctrlStructCacheFlag ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    instance control structures
     */
    Bool                                  frmHdrBufCacheFlag ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    frame header buffers.
     */
    Bool                                  frmBufCacheFlag[8] ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    frame buffers
     */
    Bool                                  bufCpuAccessFlag[8];
    /*!<  Flag indicating whether APP uses CPU to access these frame buffers.
     */
    ListMP_Handle **                      frameList;
    /*!< list of Frame queues */
    FrameQ_ShMem_Params     params;
    /*!< Instance  config params */
    IGateProvider_Handle    cliNotifyMgrGate;
    /* Gate created  to pass to clientNotifyMgr*/
    Ptr cliNotifyMgrShAddr;
    /*!Shared address of the clientNotify Mgr in knl virtual foramat */
    Ptr cliNotifyMgrGateShAddr;
    /*!Shared address of the clientNotifyMgr  gate in knl virtual format */
    Bool creCliGate;
    /*!< Denotes if gate is created internally or not for clientNotifyMgr */
} FrameQ_ShMem_Obj;

/*
 * @brief Structure defining object for the FrameQ_ShMem.
 */
typedef struct FrameQ_ShMem_Object_tag {
    FrameQ_Fxns *fxns;
    /* Function table interface of ShMem implementation */
    FrameQ_ShMem_Obj *obj;
    /*!< Pointer to the FrameQ_ShMem internal object*/
}FrameQ_ShMem_Object;


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    FrameQ_state
 *
 *  @brief  FrameQ_state state variable.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
//static
#endif
FrameQ_ShMem_ModuleObject FrameQ_ShMem_module_obj =
{
    .defaultCfg.eventNo           = FrameQ_NOTIFY_RESERVED_EVENTNO,
    .defaultCfg.usedefaultgate    = FALSE,
    .defInstParams.commonCreateParams.size = sizeof (FrameQ_ShMem_Params),
    .defInstParams.commonCreateParams.name           = NULL,
    .defInstParams.commonCreateParams.openFlag       = FALSE,
    .defInstParams.commonCreateParams.ctrlInterfaceType  =
                                              FrameQ_ShMem_INTERFACE_SHAREDMEM,
    .defInstParams.gate           = NULL,
    .defInstParams.gate           = NULL,
    .defInstParams.localProtect   = GateMP_LocalProtect_INTERRUPT,
    .defInstParams.remoteProtect  = GateMP_RemoteProtect_SYSTEM,

    .defInstParams.regionId       = 0,
    .defInstParams.sharedAddr     = 0x0,
    .defInstParams.sharedAddrSize = 0u,
    .defInstParams.numReaders     = 1u,
    .defInstParams.numQueues      = 1u,
    .defInstParams.frameQBufMgrName = NULL,
    .defInstParams.frameQBufMgrSharedAddr      = NULL,
    .defInstParams.cpuAccessFlags     = 0u,
};

/*!
 *  @var    FrameQBufMgr_ShMem_module
 *
 *  @brief  Pointer to FrameQBufMgr_ShMem_module_obj .
 */
FrameQ_ShMem_ModuleObject* FrameQ_ShMem_module = &FrameQ_ShMem_module_obj;

/*=============================================================================
 * Forward declarations of internal Functions
 *=============================================================================
 */
FrameQ_ShMem_Handle _FrameQ_ShMem_open(FrameQ_ShMem_Params *params);
static inline Int32
FrameQ_ShMem_readerPutv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 filledQueueId[],
        UInt8 numFrames);
static inline Int32 FrameQ_ShMem_writerPutv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 filledQueueId[],
        UInt8 numFrames);

static Int32 _FrameQ_ShMem_setOpenFlags(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_OpenParams *openParams);

Void FrameQ_Shmem_updateCacheFlags(FrameQ_ShMem_Obj *obj,
        UInt32 cpuAccessFlags);

/* Called by creator to flush all the remaining frames during delete time */
Int32 FrameQ_ShMem_flushAll(FrameQ_ShMem_Handle handle);

/** ============================================================================
 *  External APIs
 *  ============================================================================
 */

/*
 *  ======== FrameQ_ShMem_getConfig ========
 *  API to get the module wide config params.
 */
Void FrameQ_ShMem_getConfig(FrameQ_ShMem_Config *cfg)
{
    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_getConfig", cfg);

    if (cfg == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_getConfig",
                FrameQ_E_INVALIDARG, "cfg passed is null!");
    }
    else {
        if (FrameQ_ShMem_module->refCount == 0) {
            /* default config */
            memcpy(cfg, &(FrameQ_ShMem_module->defaultCfg),
                    sizeof(FrameQ_ShMem_Config));
        }
        else {
            /* current config */
            memcpy(cfg, &(FrameQ_ShMem_module->cfg),
                    sizeof(FrameQ_ShMem_Config));
        }
    }

    GT_0trace(curTrace, GT_LEAVE, "FrameQ_ShMem_getConfig");
}


/*
 *  ======== FrameQ_ShMem_setup ========
 *  API to initialize FrameQ_ShMem module
 */
Int32 FrameQ_ShMem_setup(FrameQ_ShMem_Config *cfg)
{
    Int32 status = FrameQ_S_SUCCESS;
    List_Params listParams;
    IArg key;
    Error_Block eb;
    FrameQ_ShMem_Config tmpCfg;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_setup", cfg);

    key = Gate_enterSystem();
    FrameQ_ShMem_module->refCount++;
    if (FrameQ_ShMem_module->refCount > 1) {
        status = FrameQ_S_ALREADYSETUP;
        GT_0trace(curTrace, GT_2CLASS,
                "FrameQ_ShMem Module already initialized!");
    }
    else {
        if (cfg == NULL) {
            FrameQ_ShMem_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }

        /* Check the params required to create the name server instance */
        if (FrameQ_ShMem_module->nameServer == NULL) {
            /* Open  the Name Server instance  created by Frond End FrameQBufMgr
             * module if not  assigned statically.
             */
//            FrameQ_ShMem_module->nameServer =
 //                             NameServer_getHandle(FRAMEQ_NAMESERVERNAME);
            FrameQ_ShMem_module->nameServer = gFrameQ_nsHandle;
            if (NULL == FrameQ_ShMem_module->nameServer) {
                status = FrameQ_E_OPEN_NAMESERVER;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_setup",
                        status, "Failed to open FrameQ NameServer");
                /* Decrement the reference count */
                FrameQ_ShMem_module->refCount--;
            }
            else {

                Error_init(&eb);

                /* Create the list */
                List_Params_init(&listParams);
                FrameQ_ShMem_module->objList = List_create(&listParams, &eb);
                GT_assert(curTrace, (NULL != FrameQ_ShMem_module->objList));

                FrameQ_ShMem_module->defaultInstGate = NULL;
                /* Create a lock for protecting list object */
                FrameQ_ShMem_module->listLock = (IGateProvider_Handle)
                        GateMutex_create ((GateMutex_Params*)NULL, &eb);
                if (FrameQ_ShMem_module->listLock == NULL) {
                    FrameQ_ShMem_module->nameServer = NULL;
                    status = FrameQ_E_FAIL;
                    GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_ShMem_setup",
                            status, "Failed to create the listLock!");
                    /* Decrement the reference count */
                    FrameQ_ShMem_module->refCount--;
                }
            }
        }
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_setup", status);

    return (status);
}


/*
 *  ======== FrameQ_ShMem_destroy ========
 *  API to finalize the module.
 */
Int32 FrameQ_ShMem_destroy()
{
    Int32 status = FrameQ_S_SUCCESS;
    IArg key;

    GT_0trace(curTrace, GT_ENTER, "FrameQ_ShMem_destroy");

    key = Gate_enterSystem();

    FrameQ_ShMem_module->refCount--;

    if (FrameQ_ShMem_module->refCount == 0) {

        /* Delete the list object */
        List_delete(&FrameQ_ShMem_module->objList);

        /* Defensively zero out a few key fields */
        FrameQ_ShMem_module->nameServer = NULL;

        memset((Ptr)&FrameQ_ShMem_module->cfg, 0,
                sizeof(FrameQ_ShMem_Config));
   }

    /* Release lock */
    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_destroy", status);

    return (status);
}



/*
 *  ======== FrameQ_ShMem_Params_init ========
 *  Shim for function to get default initialization param for the FrameQ shMem
 *  instance.
 */
Void FrameQ_ShMem_Params_init(FrameQ_ShMem_Params *params)
{
    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_Params_init", params);

    GT_assert(curTrace, (params != NULL));

    memcpy(params, &(FrameQ_ShMem_module->defInstParams),
            sizeof(FrameQ_ShMem_Params));

    GT_0trace (curTrace, GT_LEAVE, "FrameQ_ShMem_Params_init");
}

/*
 *  ======== FrameQ_sharedMemReq ========
 *  Function to return the amount of shared memory required for
 *  creation of each instance of the ShMem type.
 */
UInt32 FrameQ_ShMem_sharedMemReq(const FrameQ_ShMem_Params *params)
{
    UInt32                 totalSize     = 0;
    ListMP_Params          listMPParams;
    ClientNotifyMgr_Params clientMgrParams;
    GateMP_Params          gateParams;
    SizeT minAlign;
    UInt16 regionId;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_ShMem_sharedMemReq", params);

    GT_assert (curTrace, (params != NULL));

    if (NULL != params) {
//        Assert_isTrue(params->numReaders >= FrameQ_MAX_INST_READERS,
//                        FrameQ_ShMem_A_invalidParams);
        if (params->sharedAddr != NULL) {
            regionId = SharedRegion_getId(params->sharedAddr);
        }
        else {
            regionId = params->regionId;
        }
        GT_assert(curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        minAlign = Memory_getMaxDefaultTypeAlign();
        if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize(regionId);
        }


        totalSize = ROUND_UP(sizeof (FrameQ_NameServerEntry), minAlign);
        /* Attrs size */
        totalSize += ROUND_UP(sizeof (FrameQ_ShMem_Attrs), minAlign);
        /* Writer client */
        totalSize += ROUND_UP(sizeof (FrameQ_ShMem_WriterClient), minAlign);
        /* Reader Clients */
        totalSize += (ROUND_UP(sizeof (FrameQ_ShMem_ReaderClient),minAlign))
                      * params->numReaders;
        /* Reader queues */
        totalSize += (ROUND_UP(sizeof (FrameQ_ShMem_FrameQueue),minAlign))
                     * params->numReaders;

        GateMP_Params_init(&gateParams);
        gateParams.remoteProtect = params->remoteProtect;
        gateParams.localProtect  = params->localProtect;

        if (FrameQ_ShMem_module->cfg.usedefaultgate == FALSE) {
            if(params->gate == NULL) {
                GateMP_Params_init(&gateParams);
                gateParams.regionId = regionId;
                gateParams.remoteProtect =
                                  (GateMP_RemoteProtect) params->remoteProtect;

                gateParams.localProtect  =
                                      (GateMP_LocalProtect)params->localProtect;
                totalSize += ROUND_UP(GateMP_sharedMemReq(&gateParams),
                                       minAlign);/* for self */
                gateParams.localProtect  =
                             (GateMP_LocalProtect)params->localProtect;
                totalSize += ROUND_UP(GateMP_sharedMemReq(&gateParams),
                                       minAlign);/* For ClientNotifyMgr */
            }
        }
        /* Get the default params from the client notify manager */
        ClientNotifyMgr_Params_init(&clientMgrParams);
        clientMgrParams.regionId = regionId;
        /* Number of notify entries is equal to number of readers*/
        clientMgrParams.numNotifyEntries = params->numReaders;
        clientMgrParams.numSubNotifyEntries = params->numQueues;
        totalSize += ROUND_UP(ClientNotifyMgr_sharedMemReq (
                                   (ClientNotifyMgr_Params*)&clientMgrParams ),
                              minAlign);

        ListMP_Params_init (&listMPParams);

         /* Set the region id in order to calculate the
          * shared mem required.
          */
        listMPParams.regionId = regionId;

        /* Calculate the shared memory required for the ListMP objects  that are
         * required for all configured readers.
         */
        totalSize += (  ROUND_UP(ListMP_sharedMemReq(&listMPParams),
                                 minAlign)
                      * params->numReaders
                      * params->numQueues);
    }

    GT_0trace(curTrace, GT_LEAVE, "FrameQ_ShMem_sharedMemReq");

    return (totalSize);
}

/*
 *  ======== FrameQ_ShMem_create ========
 *  API to create the instance
 */
FrameQ_ShMem_Handle FrameQ_ShMem_create(FrameQ_ShMem_Params *params)
{
    FrameQ_ShMem_Handle     handle= NULL;
    FrameQ_ShMem_Obj       *obj = NULL;
    Int32                   status      = FrameQ_S_SUCCESS;
    UInt32                  virtAddr    = 0;
    UInt32                  addrOffset   = 0;
    Int32                   tmpStatus;
    volatile FrameQ_ShMem_Attrs  *pAttrs;
    volatile FrameQ_ShMem_WriterClient  * writer;
    ListMP_Params           listMPParams;
    UInt32                  i;
    SharedRegion_SRPtr      sharedShmBase;
    GateMP_Params           gateParams;
    GateMP_Handle           gateHandle;
    ClientNotifyMgr_Params  clientMgrParams;
    ClientNotifyMgr_Handle  cliMgrHandle;
    FrameQ_NameServerEntry  nameserverEntry;
    FrameQ_NameServerEntry  *pnameserverEntry;
    UInt32                  k;
    FrameQBufMgr_OpenParams frmQBufMgrOpenParams;
    FrameQBufMgr_Handle     bufMgrHandle;
    UInt32                  listMPsharedAddrSize;
    UInt32                  gateMPsharedAddrSize;
    Int32                   index;
    UInt32                  len;
    IArg                    key0;
    Error_Block ebObj;
    Error_Block *eb = &ebObj;  /* locally used Error_Block */

    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_create", params);

    GT_assert(curTrace, (params != NULL));

    /* Assert if invalid region id. */
    GT_assert(curTrace, (SharedRegion_INVALIDREGIONID != params->regionId));

    Error_init(eb);

    /* Create the handle */
    /* Allocate memory for handle */
    handle = (FrameQ_ShMem_Handle)Memory_calloc(NULL,
            sizeof(FrameQ_ShMem_Object), 0u, eb);
    if (handle == NULL) {
        status = FrameQ_E_MEMORY;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_create",
                status, "Memory allocation failed for handle!");
    }
    else {
        obj = (FrameQ_ShMem_Obj *)Memory_calloc(NULL,
                sizeof(FrameQ_ShMem_Obj), 0u, eb);
        if (obj == NULL) {
            status = FrameQ_E_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_create",
                    status, "Memory alloc failed for internal object!");

            Memory_free(NULL, handle, sizeof (FrameQ_ShMem_Object));
            handle = NULL;
        }
    }

    if (status >= 0) {
        if (params->commonCreateParams.openFlag == TRUE) {
            /* Is this right? */
        }
        else {
            /* Check if name exists in nameServer  */
            if (strlen((String)params->commonCreateParams.name) > 0) {
                /* Ensure NameServer is enabled */
                len = sizeof (FrameQ_NameServerEntry);
                status = NameServer_get(FrameQ_ShMem_module->nameServer,
                        (String)params->commonCreateParams.name,
                        &nameserverEntry, &len, NULL);
                if (status > 0) {
                    /* Already instance with the name exists */
                    status = FrameQ_E_INST_EXISTS;
                    GT_setFailureReason (curTrace, GT_4CLASS,
                            "FrameQ_ShMem_create", status,
                            "Instance with this name already exists");
                }
                else if (status == NameServer_E_FAIL) {
                    /* Error happened in NameServer. Pass the error up. */
                    status = FrameQ_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "FrameQ_ShMem_create", status,
                            "Error happened in NameServer_get!");
                }
                else  {
                    /* Instance with this name is not there in the NameServer.
                     * So create can proceed.
                     */
                     /* Reinitialize the error block */
                     Error_init(eb);
                     status = FrameQ_S_SUCCESS;
                }
            }

            if (status >= 0) {
                if (params->numReaders > FrameQ_MAX_INST_READERS ) {
                    status = FrameQ_E_INVALIDARG;
                    GT_setFailureReason (curTrace, GT_4CLASS,
                            "FrameQ_ShMem_create", status,
                            "params->numReaders > max");
                }
                else if ((params->frameQBufMgrName == NULL) &&
                         (params->frameQBufMgrSharedAddr == NULL)) {
                    status = FrameQ_E_INVALIDARG;
                    GT_setFailureReason (curTrace, GT_4CLASS,
                            "FrameQ_ShMem_create", status,
                            "MgrName and SharedAddr both NULL!");
                }
            }

            if (status >= 0) {
                handle->obj = obj;
                if (params->sharedAddr == NULL) {
                    /* Creating using a shared region ID and name */
                    /* Need to allocate from the heap */
                    obj->allocSize = FrameQ_ShMem_sharedMemReq(params);

                    /* HeapMemMP will do the alignment */
                    virtAddr = (UInt32)Memory_calloc(
                            SharedRegion_getHeap(params->regionId),
                                obj->allocSize, 0, eb);
                }
                else {
                    virtAddr = (UInt32)params->sharedAddr;
                }

                /* Creating using sharedAddr */
                obj->regionId = SharedRegion_getId((Ptr)virtAddr);

                /* Assert that the buffer is in a valid shared region */
                GT_assert(curTrace,
                        (obj->regionId != SharedRegion_INVALIDREGIONID));

                /* Assert that sharedAddr is cache aligned */
                GT_assert(curTrace, ((UInt32)virtAddr %
                        SharedRegion_getCacheLineSize(obj->regionId) == 0));

                /* set object's cacheEnabled, objType, and attrs  */
                obj->ctrlStructCacheFlag =
                        SharedRegion_isCacheEnabled(obj->regionId);

                obj->pnameserverEntry = (FrameQ_NameServerEntry *)virtAddr;

                sharedShmBase = SharedRegion_getSRPtr((Ptr)virtAddr,
                        obj->regionId);
                nameserverEntry.interfaceType =
                        params->commonCreateParams.ctrlInterfaceType;
                nameserverEntry.instParams.shMemParams.sharedAddr =
                        (Ptr)sharedShmBase;
                nameserverEntry.instParams.shMemParams.sharedAddrSize =
                        params->sharedAddrSize;
                if (params->sharedAddr != NULL) {
                    nameserverEntry.instParams.shMemParams.sharedAddrSize =
                            params->sharedAddrSize;
                }
                else {
                    nameserverEntry.instParams.shMemParams.sharedAddrSize =
                            obj->allocSize;
                }

                if (params->frameQBufMgrName != NULL) {
                    strcpy((String)nameserverEntry.instParams.shMemParams
                            .frameQBufMgrName, params->frameQBufMgrName);
                }
                else {
                    nameserverEntry.instParams.shMemParams.frameQBufMgrName[0] = '\0';
                }

                if (params->frameQBufMgrSharedAddr != NULL) {
                    index = SharedRegion_getId(params->frameQBufMgrSharedAddr);
                    nameserverEntry.instParams.shMemParams.frameQBufMgrSharedAddr =
                            (Ptr)SharedRegion_getSRPtr(
                                params->frameQBufMgrSharedAddr, index);
                }
                else {
                    nameserverEntry.instParams.shMemParams.frameQBufMgrSharedAddr =
                            NULL;
                }

                /***********************************************************************
                 * Create instance gate based on the parameters if user has not provided
                 * gate.
                 ***********************************************************************
                 */
                if (params->gate != NULL) {
                    obj->gate = params->gate;
                    nameserverEntry.instParams.shMemParams.instGateMPAddr =
                            (Ptr)ti_sdo_ipc_GateMP_getSharedAddr(
                            (ti_sdo_ipc_GateMP_Handle)obj->gate);
                    obj->creGate = FALSE;
                }
                else {
                    /* Set flag  indicating user has not provided  GateMP */
                    if (FrameQ_ShMem_module->cfg.usedefaultgate == TRUE) {
                        obj->creGate = FALSE;
                        /* Check if FrameQ module has any default gate set. if not use
                         * GateMP's default gate.
                         */
                        if (FrameQ_ShMem_module->defaultInstGate != NULL) {
                            gateHandle = FrameQ_ShMem_module->defaultInstGate;
                        }
                        else {
                            gateHandle = GateMP_getDefaultRemote();
                            GT_assert(curTrace, (NULL != gateHandle));
                        }
                    }
                    else {
                        /* Set the flag indicating that gate is created internally */
                        obj->creGate = TRUE;
                        /*Create gateMP for this instance using the params provided */
                        GateMP_Params_init(&gateParams);
                        gateParams.localProtect = (GateMP_LocalProtect)
                                params->localProtect;
                        gateParams.remoteProtect = (GateMP_RemoteProtect)
                                params->remoteProtect;
                        gateParams.regionId = obj->regionId;
                        obj->gateMPAllocSize = GateMP_sharedMemReq(&gateParams);

                        obj->instGateSharedAddr =
                        gateParams.sharedAddr = Memory_calloc(
                                SharedRegion_getHeap(obj->regionId),
                                    obj->gateMPAllocSize, 0, eb);
                        gateHandle = GateMP_create(&gateParams);

                        GT_assert(curTrace, (NULL != gateHandle));
                    }

                    /* Store instance gate address for openers */
                    obj->gate = gateHandle;
                    nameserverEntry.instParams.shMemParams.instGateMPAddr =
                            (Ptr)ti_sdo_ipc_GateMP_getSharedAddr(
                            (ti_sdo_ipc_GateMP_Handle)obj->gate);
                }
                if (status >= 0) {
                    /* Inserting nameserver entry params at the start of the
                     * shared addr to allow open by address.
                     */
                    pnameserverEntry = obj->pnameserverEntry;
                    memcpy(pnameserverEntry, (Ptr)&nameserverEntry,
                            sizeof(FrameQ_NameServerEntry));
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv((Ptr)pnameserverEntry,
                                sizeof(FrameQ_NameServerEntry),
                                Cache_Type_ALL, TRUE);
                    }

                    obj->minAlign = Memory_getMaxDefaultTypeAlign();

                    if (SharedRegion_getCacheLineSize(obj->regionId) >
                            obj->minAlign) {
                        obj->minAlign =
                                SharedRegion_getCacheLineSize(obj->regionId);
                    }

                    /* Move addrOffset by cache aligned size of
                     * FrameQ_NameServerEntry.
                     */
                    addrOffset += ROUND_UP(sizeof (FrameQ_NameServerEntry),
                            obj->minAlign);

                    /***********************************************************
                     * Open FrameQBufMgr instance using the provided
                     * FrameQBufMgr Name. It Must created before creating FrameQ
                     * with this FrameQBufMgr name.
                     **************************************************************
                     */
                    frmQBufMgrOpenParams.commonOpenParams.name =
                            params->frameQBufMgrName;
                    frmQBufMgrOpenParams.commonOpenParams.sharedAddr =
                            params->frameQBufMgrSharedAddr;
                    frmQBufMgrOpenParams.commonOpenParams.cpuAccessFlags  =
                            params->cpuAccessFlags;
                    tmpStatus = FrameQBufMgr_open(&bufMgrHandle,
                            &frmQBufMgrOpenParams);
                    if (tmpStatus < 0){
                        status = FrameQ_E_FRAMEQBUFMGROPEN;
                        GT_setFailureReason (curTrace, GT_4CLASS,
                                "FrameQ_ShMem_create", status,
                                "FrameQBufMgr_open() failed!");
                    }
                    else {
                        /* Successfully opened the FrameQBufMgr instance
                         */
                        obj->frameQBufMgrHandle = bufMgrHandle;
                        obj->frameQBufMgrId = FrameQBufMgr_getId(
                                obj->frameQBufMgrHandle);

                        /* Update the cache flags */
                        FrameQ_Shmem_updateCacheFlags(obj,
                                params->cpuAccessFlags);

                        obj->attrs = (FrameQ_ShMem_Attrs *)
                                (virtAddr + addrOffset);

                        /* Populate the shared struct */
                        pAttrs =  obj->attrs;

                        pAttrs->status       = FrameQ_ShMem_CREATED;
                        pAttrs->version      = FrameQ_ShMem_VERSION; /* version info */

                        pAttrs->numReaders         = params->numReaders;
                        /* Number of queues for each reader */
                        pAttrs->numQueues          = params->numQueues;
                        pAttrs->primaryQueueId     = 0;
                        pAttrs->numActiveReaders   = 0;
                        pAttrs->frmQBufMgrId = obj->frameQBufMgrId;
                        pAttrs->localProtect = params->localProtect;
                        pAttrs->remoteProtect = params->remoteProtect;

                        addrOffset +=  ROUND_UP(sizeof (FrameQ_ShMem_Attrs),
                                obj->minAlign);

                        /* Populate the obj with the writer client control info pointer
                         * in shared memory
                         */
                        obj->writer =  (FrameQ_ShMem_WriterClient *)
                                (virtAddr + addrOffset);

                        addrOffset +=
                                ROUND_UP(sizeof (FrameQ_ShMem_WriterClient),
                                    obj->minAlign);
                        /* Update the readers information */
                        obj->reader = Memory_alloc(NULL,
                                sizeof(UInt32) * pAttrs->numReaders, 0, eb);
                        if (obj->reader == NULL) {
                            status = FrameQ_E_ALLOC_MEMORY;
                            GT_setFailureReason (curTrace, GT_4CLASS,
                                    "FrameQr_ShMem_create", status,
                                    "Memory_alloc failed!");
                        }
                        else {
                            for (i = 0; i <  pAttrs->numReaders; i++) {
                                obj->reader[i] = (FrameQ_ShMem_ReaderClient *)
                                        (virtAddr + addrOffset);

                                addrOffset +=
                                        ROUND_UP(sizeof(FrameQ_ShMem_ReaderClient),
                                            obj->minAlign);
                            }
                        }

                        if (status >= 0) {
                            /* Update the Queue information */
                            obj->frmQueue = Memory_alloc(NULL,
                                    (sizeof(UInt32) * pAttrs->numReaders *
                                    pAttrs->numQueues), 0, eb);

                            if (obj->frmQueue == NULL) {
                                status = FrameQ_E_ALLOC_MEMORY;
                                GT_setFailureReason (curTrace, GT_4CLASS,
                                        "FrameQr_ShMem_create", status,
                                        "Memory_alloc failed!");
                            }
                            else {
                                for (i = 0; i < (pAttrs->numReaders); i++) {
                                    obj->frmQueue[i] =
                                            (FrameQ_ShMem_FrameQueue *)
                                                (virtAddr + addrOffset);

                                    addrOffset += ROUND_UP(
                                            sizeof(FrameQ_ShMem_FrameQueue),
                                            obj->minAlign);
                                }
                            }
                        }

                        /***********************************************************
                         * Create ClientNotifyMgr instance used by this FrameQ
                         * instance.Create a gateMP using same protection parameters
                         * as specfied in FrameQ create and  pass  it to
                         * ClientNotifyMgr_create.
                         ***********************************************************
                         */

                        ClientNotifyMgr_Params_init(&clientMgrParams);
                        clientMgrParams.numNotifyEntries = pAttrs->numReaders;
                        clientMgrParams.numSubNotifyEntries = pAttrs->numQueues;
                        clientMgrParams.sharedAddr =
                                (Ptr)(virtAddr + addrOffset);

                        clientMgrParams.sharedAddrSize =
                                ClientNotifyMgr_sharedMemReq (
                                    (ClientNotifyMgr_Params*)&clientMgrParams);

                        addrOffset += ROUND_UP(clientMgrParams.sharedAddrSize,
                                obj->minAlign);

                        if (params->gate != NULL) {
                            clientMgrParams.gate = params->gate;
                nameserverEntry.instParams.shMemParams.cliGateMPAddr =
                (Ptr) ti_sdo_ipc_GateMP_getSharedAddr((ti_sdo_ipc_GateMP_Handle)clientMgrParams.gate);

                            /* Make it null so that finalization won't delete it.
                             */
                            obj->clientNotifyMgrGate = params->gate;
                            obj->creCliGate = FALSE;
                        }
                        else {
                            if (FrameQ_ShMem_module->cfg.usedefaultgate
                                    == TRUE) {
                                clientMgrParams.gate = (Ptr)gateHandle;
                                nameserverEntry.instParams.shMemParams.
                                        cliGateMPAddr = (Ptr)
                                        ti_sdo_ipc_GateMP_getSharedAddr(
                                        (ti_sdo_ipc_GateMP_Handle)
                                        clientMgrParams.gate);

                                /* Make it null so that finalization won't delete it.
                                 */
                                obj->clientNotifyMgrGate = NULL;
                                obj->creCliGate = FALSE;
                            }
                            else {
                                /*Create gateMP for the clientNotifyMgr */
                                obj->creCliGate = TRUE;
                                GateMP_Params_init(&gateParams);

                                /* Since ClientNotifyMgr uses this gate in ISR
                                 * context, local protection should be
                                 * GateMP_LocalProtect_INTERRUPT.
                                 */

                                gateParams.localProtect = (GateMP_LocalProtect)
                                        params->localProtect;
                                gateParams.remoteProtect = (GateMP_RemoteProtect)
                                        params->remoteProtect;
                                gateParams.sharedAddr =
                                        (Ptr)(virtAddr + addrOffset);
                                gateParams.regionId = obj->regionId;
                                gateMPsharedAddrSize =
                                        GateMP_sharedMemReq(&gateParams);

                                addrOffset += ROUND_UP(gateMPsharedAddrSize,
                                        obj->minAlign);
                                gateHandle = GateMP_create(&gateParams);

                                if (gateHandle == NULL) {
                                    status = FrameQBufMgr_E_CREATE_GATEMP;
                                    GT_setFailureReason(curTrace, GT_4CLASS,
                                            "FrameQ_ShMem_create", status,
                                            "Failed to create GateMP instance.");
                                }
                                else {
                                    clientMgrParams.gate = (Ptr) gateHandle;
                                    nameserverEntry.instParams.shMemParams.
                                            cliGateMPAddr = (Ptr)
                                            SharedRegion_invalidSRPtr();
                                    /* set to NULL so that finalization won't
                                     * delete it.
                                     */
                                    obj->clientNotifyMgrGate = gateHandle;
                                }
                            }
                        }

                        if (status >= 0) {
                            /* Create the client notifyMgr for this instance */
                            /* Set the sub notify entries equivalent to
                             * number queues
                             */
                            clientMgrParams.numSubNotifyEntries =
                                    params->numQueues;

                            /* Pass the same name for ClientNotifyMgr
                            */
                            clientMgrParams.name =
                                    (String)params->commonCreateParams.name;
                            /* Event no to be used for this instance */
                            clientMgrParams.eventNo =
                                    FrameQ_ShMem_NOTIFY_RESERVED_EVENTNO;
                            /* This is actually create call */
                            clientMgrParams.openFlag = FALSE;
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                clientMgrParams.cacheFlags =
                                        ClientNotifyMgr_CONTROL_CACHEUSE;
                            }

                            cliMgrHandle =
                                    ClientNotifyMgr_create(&clientMgrParams);
                            if (NULL == cliMgrHandle) {
                                status = FrameQ_E_FAIL_CLIENTN0TIFYMGR_CREATE;
                                GT_setFailureReason(curTrace, GT_4CLASS,
                                        "FrameQ_ShMem_create", status,
                                        "Failed to create ClientNotifyMgr "
                                        "instance.");
                                obj->clientNotifyMgrHandle = NULL;
                            }
                            else {
                                obj->clientNotifyMgrHandle = cliMgrHandle;
                            }

                            if (status >= 0) {
                                obj->frameList = Memory_alloc(NULL,
                                        sizeof(UInt32) * pAttrs->numReaders,
                                        0, eb);
                                if (obj->frameList  == NULL) {
                                    status = FrameQ_E_MEMORY;
                                    GT_setFailureReason (curTrace, GT_4CLASS,
                                            "FrameQ_ShMem_create", status,
                                            "Memory allocation failed "
                                            "obj->frameList!");
                                }
                                else {
                                    for (k = 0; k < pAttrs->numReaders; k++) {
                                        obj->frameList[k] = Memory_alloc(NULL,
                                                sizeof(UInt32) * pAttrs->numQueues,
                                                0, eb);
                                        if (obj->frameList[k] == NULL) {
                                            status = FrameQ_E_MEMORY;
                                            GT_setFailureReason(curTrace, GT_4CLASS,
                                                    "FrameQ_ShMem_create", status,
                                                    "Memory allocation "
                                                    "failed obj->frameList!");
                                        }
                                    }
                                }
                            }

                            if  (status >= 0) {
                                /* Prepare  frame queues.
                                 * Pass shared Addr to all the ListMPs
                                 * that FrameQ is creating.
                                 */
                                ListMP_Params_init(&listMPParams);
                                /* Set the region id in order to calculate the
                                 * shared mem required.
                                 */
                                listMPParams.regionId =
                                        SharedRegion_getId((Ptr)virtAddr);
                                listMPParams.gate = obj->gate;
                                listMPsharedAddrSize =
                                        ListMP_sharedMemReq(&listMPParams);
                                /*
                                 * Each frmQueue entry in attrs maintains info
                                 * about queues that belongs  one reader.
                                 */
                                for (i = 0; i < pAttrs->numReaders; i++) {
                                    obj->frmQueue[i]->isAllocated = FALSE;
                                    obj->frmQueue[i]->readerIndex = (UInt32)-1;
                                    for (k = 0; k < pAttrs->numQueues; k++) {
                                        /* Initialize the numframes in each reader
                                         * queue to zero
                                         */
                                        obj->frmQueue[i]->numFrames[k] = 0;
                                        listMPParams.sharedAddr = (UInt32 *)
                                                  ( (UInt32) virtAddr
                                                   + addrOffset);
                                        obj->frameList[i][k] = ListMP_create(
                                                &listMPParams);
                                        addrOffset += listMPsharedAddrSize;
                                    }
                                    if (obj->ctrlStructCacheFlag == TRUE) {
                                        Cache_wbInv((Ptr)obj->frmQueue[i],
                                                sizeof(FrameQ_ShMem_FrameQueue),
                                                Cache_Type_ALL, TRUE);

                                    }
                                }
                            }
                        }

                        if (status >= 0) {
                            /* Initialize writer client structure */
                            writer = (FrameQ_ShMem_WriterClient *)obj->writer;
                            writer->isValid     = FALSE;
                            writer->procId      = (UInt32)-1;
                            writer->cacheFlags  = 0;
                            writer->isRegistered = FALSE;

                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_wbInv((Ptr)writer,
                                             sizeof(FrameQ_ShMem_WriterClient),
                                             Cache_Type_ALL, TRUE);

                            }
                            /* Initialize reader clients control structures */
                            for (i = 0; i < pAttrs->numReaders; i++) {
                                obj->reader[i]->isValid      = FALSE;
                                obj->reader[i]->procId       = (UInt32)-1;
                                obj->reader[i]->frameQIndex  = (UInt32)-1;
                                obj->reader[i]->cacheFlags   = 0;
                                obj->reader[i]->isRegistered = FALSE;
                                obj->reader[i]->notifyId     = (UInt32)-1;
                                if (obj->ctrlStructCacheFlag == TRUE) {
                                    Cache_wbInv((Ptr)obj->reader[i],
                                         sizeof(FrameQ_ShMem_WriterClient),
                                         Cache_Type_ALL,
                                         TRUE);

                                }
                            }

                            /* Write back Attrs structure */
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_wbInv ((Ptr)pAttrs,
                                             sizeof(FrameQ_ShMem_Attrs),
                                             Cache_Type_ALL,
                                             TRUE);
                            }

                            if (params->sharedAddr) {
                                obj->objType   = FrameQ_ShMem_DYNAMIC_CREATE;
                            }
                            else {
                                /* set the object type indicating it is created
                                 * using Shared Region id internally.
                                 */
                                obj->objType =
                                        FrameQ_ShMem_DYNAMIC_CREATE_USEDREGION;
                            }
                            obj->interfaceType =
                                    params->commonCreateParams.ctrlInterfaceType;
                            obj->virtClientHandle = NULL;
                            /***************************************************
                             * Add nameserverEntry in to the name server instance
                             ***************************************************
                             */
                            if ((FrameQ_ShMem_module->nameServer != NULL) &&
                                (strlen((String)params->commonCreateParams.name) > 0)) {
                                /* FrameQ_ShMem_module->nameServer is the same nameserver
                                 * instance created by the front end FrameQ module.
                                 */
                                obj->nsKey = NameServer_add (
                                       FrameQ_ShMem_module->nameServer,
                                       (String)params->commonCreateParams.name,
                                       &nameserverEntry,
                                       sizeof(FrameQ_NameServerEntry));
                                if (obj->nsKey == NULL) {
                                    status = FrameQ_E_NAMESERVERADD;
                                    GT_assert(curTrace, (NULL != obj->nsKey));
                                }
                            }
                        }
                    }
                }
            }
        }
        if ((strlen((String)params->commonCreateParams.name) > 0) &&
                (status >= 0)) {
            obj->name = Memory_alloc(NULL, FrameQ_ShMem_MAX_NAMELEN, 0, eb);
            if (obj->name == NULL) {
                status = FrameQ_E_MEMORY;
                GT_setFailureReason (curTrace, GT_4CLASS,
                        "FrameQ_ShMem_create", status,
                        "Failed to allocate memory for obj->name!");
            }
            else {
                if (strlen((String)params->commonCreateParams.name)
                        < FrameQ_ShMem_MAX_NAMELEN) {
                    strcpy(obj->name, (String)params->commonCreateParams.name);
                }
            }
        }

        if (status >= 0) {
            /* Put in the local list */
            /* Populate the params member */
            memcpy((Ptr)&obj->params, (Ptr)params,
                    sizeof(FrameQ_ShMem_Params));
            key0 = IGateProvider_enter(FrameQ_ShMem_module->listLock);
            List_elemClear (&obj->listElem);
            List_put ((List_Handle)FrameQ_ShMem_module->objList,
                      &obj->listElem);
            IGateProvider_leave (FrameQ_ShMem_module->listLock, key0);
        }

    }

    /* Failed to create/open the instance. Do clean up*/
    if (status < 0) {
        if (obj != NULL ) {
            if (obj->name != NULL) {
                Memory_free (NULL, obj->name, FrameQ_ShMem_MAXNAMELEN);
            }
            Memory_free (NULL, obj, sizeof (FrameQ_ShMem_Obj));
        }
        if (handle != NULL) {
            Memory_free (NULL, handle, sizeof (FrameQ_ShMem_Object));
            handle = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_create", handle);

    return (handle);
}

/*
 *  ======== _FrameQ_ShMem_open ========
 *  Internal API to open the instance.
 */
FrameQ_ShMem_Handle _FrameQ_ShMem_open(FrameQ_ShMem_Params *params)
{
    FrameQ_ShMem_Handle     handle= NULL;
    FrameQ_ShMem_Obj       *obj = NULL;
    Int32                   status      = FrameQ_S_SUCCESS;
    UInt32                  virtAddr    = 0;
    UInt32                  addrOffset   = 0;
    Ptr                     localAddr = NULL;
    Int32                   tmpStatus;
    volatile FrameQ_ShMem_Attrs  *pAttrs;
    ListMP_Params           listMPParams;
    UInt32                  i;
    GateMP_Params           gateParams;
    GateMP_Handle           gateHandle;
    ClientNotifyMgr_Params  clientMgrParams;
    ClientNotifyMgr_Handle  cliMgrHandle;
    FrameQ_NameServerEntry  nameserverEntry;
    FrameQ_NameServerEntry  *pnameserverEntry;
    UInt32                  k;
    FrameQBufMgr_OpenParams frmQBufMgrOpenParams;
    FrameQBufMgr_Handle     bufMgrHandle;
    UInt32                  listMPsharedAddrSize;
    Ptr                     listMPsharedAddr;
    Int32                   index;
    UInt32                  len;
    IArg                    key;
    IArg                    key0;
    Error_Block ebObj;
    Error_Block *eb = &ebObj;  /* locally used Error_Block */

    GT_1trace(curTrace, GT_ENTER, "_FrameQ_ShMem_open", params);

    GT_assert(curTrace, (params != NULL));

    Error_init(eb);

    /* Create the handle */
    /* Allocate memory for handle */
    handle = (FrameQ_ShMem_Handle)Memory_calloc(NULL,
            sizeof(FrameQ_ShMem_Object), 0u, eb);
    if (handle == NULL) {
        status = FrameQ_E_MEMORY;
        GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_ShMem_open",
                status, "Memory allocation failed for handle!");
    }
    else {
        obj = (FrameQ_ShMem_Obj *)Memory_calloc(NULL,
                sizeof(FrameQ_ShMem_Obj), 0u, eb);
        if (obj == NULL) {
            Memory_free(NULL, handle, sizeof(FrameQ_ShMem_Object));
            handle = NULL;
            status = FrameQ_E_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_ShMem_open",
                    status, "Memory alloc failed for internal object!");
        }
    }

    if (status >= 0) {
        /* Get the shared control addr, if NameServer is configured  and
         * name is provided in the params.
         */
        if ((strlen((String)params->commonCreateParams.name) > 0) &&
                (FrameQ_ShMem_module->nameServer != NULL)) {

            len = sizeof(FrameQ_NameServerEntry);
            status = NameServer_get ((NameServer_Handle)FrameQ_ShMem_module->nameServer,
                    (String)(params->commonCreateParams.name),
                    &nameserverEntry, &len, NULL);
            /* TODO: check need to pass multiproc info */
            if (status == NameServer_E_NOTFOUND) {
                /* Failed to get the information from the name server.
                 * It means it is not created.
                 */
                if (params->sharedAddr == NULL) {
                    status = FrameQ_E_NOTFOUND;
                    GT_setFailureReason (curTrace, GT_4CLASS,
                            "_FrameQ_ShMem_open", status,
                            "Instance not found!");
                }
            }
            else if (status == NameServer_E_FAIL) {
                /* Error happened in NameServer. Pass the error up. */
                status = FrameQ_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQ_ShMem_open", status,
                        "Failure in NameServer_get!");
            }
            else if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQ_ShMem_open", status,
                        "Failure in NameServer_get!");
            }
            else {
                virtAddr = (UInt32)SharedRegion_getPtr (
                        (SharedRegion_SRPtr)
                        nameserverEntry.instParams.shMemParams.sharedAddr);
                pnameserverEntry = &nameserverEntry;
                /* Assert that sharedAddr is cache aligned */
                index = SharedRegion_getId((Ptr)virtAddr);

                obj->regionId = index;
                /* Assert that the buffer is in a valid shared region */
                GT_assert(curTrace, (index != SharedRegion_INVALIDREGIONID));

                GT_assert(curTrace, ((UInt32)virtAddr %
                        SharedRegion_getCacheLineSize(index) == 0));

                if (SharedRegion_isCacheEnabled(index)) {
                    obj->ctrlStructCacheFlag = TRUE;
                    Cache_inv((Ptr)virtAddr,
                            ROUND_UP(sizeof(FrameQ_NameServerEntry),
                                obj->minAlign), Cache_Type_ALL, TRUE);
                }
            }
        }

        if (virtAddr == 0 ) {
            if (params->sharedAddr != NULL) {
                /* Unable to get information from name server. But user has
                 * provided  virtual shared address. So proceeding further.
                 */
                virtAddr    = (UInt32) params->sharedAddr;
                pnameserverEntry = (FrameQ_NameServerEntry*)virtAddr;
                /* Assert that sharedAddr is cache aligned */
                index = SharedRegion_getId(pnameserverEntry);

                /* Assert that the buffer is in a valid shared region */
                GT_assert(curTrace, index != SharedRegion_INVALIDREGIONID);

                GT_assert(curTrace, ((UInt32)pnameserverEntry %
                              SharedRegion_getCacheLineSize(index) == 0));

                /* Invalidate FrameQ enry info located at the start of
                 * the sharedAddr.
                 */
                if (SharedRegion_isCacheEnabled(index)) {
                    Cache_inv((Ptr)pnameserverEntry,
                              ROUND_UP (
                                      sizeof (FrameQ_NameServerEntry),
                                      obj->minAlign),
                              Cache_Type_ALL,
                              TRUE);
                    obj->ctrlStructCacheFlag = TRUE;
                }
                status  = FrameQ_S_SUCCESS;
            }
            else {
                /*
                 * params->sharedAddr is null and also virtaddr got from the
                 * name server is  also not valid. So rejecting the  open
                 * call.
                 */
                status = FrameQ_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQ_ShMem_open", status,
                        "params->sharedAddr is null!");
            }
        }
        /* In shared memory  lay out  for FrameQ instance is
         * FrameQ_NameServerEntry - Cache aligned size.
         * FrameQ conrol struct
         * ClientNoitifyMgr gate
         * ClientNotifyMgr control struct
         * listMp objects (numreaders * numqueues)
         */

        if (status >= 0) {
            handle->obj = obj;
            GT_assert(curTrace,
                    (nameserverEntry.instParams.shMemParams.instGateMPAddr !=
                    (Ptr)SharedRegion_invalidSRPtr()));
            if (nameserverEntry.instParams.shMemParams.instGateMPAddr !=
                    (Ptr)SharedRegion_invalidSRPtr()) {

                /* Get the local address of the SRPtr */
                localAddr = SharedRegion_getPtr((SharedRegion_SRPtr)
                        nameserverEntry.instParams.shMemParams.instGateMPAddr);
                GT_assert(curTrace, (localAddr != NULL));

                tmpStatus = GateMP_openByAddr(localAddr, &gateHandle);
                if (tmpStatus != GateMP_S_SUCCESS) {
                    obj->creGate = FALSE;
                    status = FrameQ_E_OPEN_GATEMP;
                }
                else {
                    obj->gate = gateHandle;
                    obj->creGate = TRUE;
                }
            }
        }

        if (status >= 0) {
            obj->minAlign = Memory_getMaxDefaultTypeAlign();

            obj->regionId = SharedRegion_getId((Ptr)virtAddr);

            if (SharedRegion_getCacheLineSize(obj->regionId) > obj->minAlign) {
                obj->minAlign = SharedRegion_getCacheLineSize(obj->regionId);
            }

            /* Move offset by size of nameserverEntry cache aligned size.
             * No need to check interface type as it is checked by front end
             * module(FrameQ).
             */
            obj->pnameserverEntry = (FrameQ_NameServerEntry*)(virtAddr);
            addrOffset += ROUND_UP(sizeof (FrameQ_NameServerEntry), obj->minAlign);

            obj->attrs = (FrameQ_ShMem_Attrs *)(virtAddr + addrOffset);

            addrOffset += ROUND_UP(sizeof(FrameQ_ShMem_Attrs), obj->minAlign);

            pAttrs       = obj->attrs;

            /***************************************************
             * Open the FrameQBufMgr instance using the provided
             * FrameQBufMgr Name.
             ***************************************************
             */
            frmQBufMgrOpenParams.commonOpenParams.name = (String)
                pnameserverEntry->instParams.shMemParams.frameQBufMgrName;
            frmQBufMgrOpenParams.commonOpenParams.sharedAddr =
                    pnameserverEntry->instParams.shMemParams.
                        frameQBufMgrSharedAddr;
            frmQBufMgrOpenParams.commonOpenParams.cpuAccessFlags  =
                    params->cpuAccessFlags;
            tmpStatus = FrameQBufMgr_open(&bufMgrHandle,
                    &frmQBufMgrOpenParams);
            if (tmpStatus < 0) {
                status = FrameQ_E_FRAMEQBUFMGROPEN;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQ_ShMem_open", status,
                        "FrameQBufMgr_open() failed!");
            }
            else {
                obj->frameQBufMgrHandle = bufMgrHandle;
                obj->frameQBufMgrId = FrameQBufMgr_getId(
                        obj->frameQBufMgrHandle);
                /* Update cache flags */
                FrameQ_Shmem_updateCacheFlags(obj,params->cpuAccessFlags);

                key = GateMP_enter (obj->gate);

                if (obj->ctrlStructCacheFlag == TRUE) {
                    Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                            Cache_Type_ALL, TRUE);
                }

                if (pAttrs->status != FrameQ_ShMem_CREATED) {
                    status = FrameQ_E_NOTFOUND;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "_FrameQ_ShMem_open", status,
                            "Instance not created!");
                }
                else {
                    /* Update the writer information */
                    obj->writer =  (FrameQ_ShMem_WriterClient *)(virtAddr + addrOffset);

                    addrOffset += ROUND_UP(sizeof (FrameQ_ShMem_WriterClient),
                            obj->minAlign);

                    /* Update the readers information */
                    obj->reader = Memory_alloc(NULL,
                            sizeof(UInt32) * pAttrs->numReaders, 0, eb);
                    if (obj->reader == NULL) {
                        status = FrameQ_E_ALLOC_MEMORY;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "_FrameQ_ShMem_open", status,
                                "Memory_alloc failed!");
                    }
                    else {
                        for (i = 0; i <  pAttrs->numReaders; i++) {
                            obj->reader[i] = (FrameQ_ShMem_ReaderClient *)(virtAddr + addrOffset);

                            addrOffset += ROUND_UP(sizeof(FrameQ_ShMem_ReaderClient),
                                    obj->minAlign);
                        }
                    }

                    if (status >= 0) {
                        /* Update the Queue information */
                        obj->frmQueue = Memory_alloc(NULL,
                                (sizeof(UInt32) * pAttrs->numReaders *
                                pAttrs->numQueues), 0, eb);

                        if (obj->frmQueue == NULL) {
                            status = FrameQ_E_ALLOC_MEMORY;
                            GT_setFailureReason(curTrace, GT_4CLASS,
                                    "_FrameQ_ShMem_open", status,
                                    "Memory_alloc failed!");
                        }
                        else {
                            for (i = 0; i < (pAttrs->numReaders); i++) {
                                obj->frmQueue[i] = (FrameQ_ShMem_FrameQueue *)
                                                            (virtAddr + addrOffset);

                                addrOffset += ROUND_UP(
                                                   sizeof (FrameQ_ShMem_FrameQueue),
                                                   obj->minAlign);
                            }
                        }
                    }

                    /* Get the default params from the client notify manager */
                    ClientNotifyMgr_Params_init(&clientMgrParams);
                    clientMgrParams.numNotifyEntries = pAttrs->numReaders;
                    clientMgrParams.numSubNotifyEntries = pAttrs->numQueues;
                    clientMgrParams.sharedAddr = (Ptr) (virtAddr + addrOffset);
                    clientMgrParams.sharedAddrSize =
                            ClientNotifyMgr_sharedMemReq (
                               (ClientNotifyMgr_Params*)&clientMgrParams );

                    addrOffset += ROUND_UP(clientMgrParams.sharedAddrSize,
                                          obj->minAlign);
                    /* cliGateMPAddr if it valid means user has passed the
                     * gate to create call or  default gate is used in create
                     * call.
                     */
                    if (nameserverEntry.instParams.shMemParams.cliGateMPAddr
                        != (Ptr)SharedRegion_invalidSRPtr()) {
                        /* Get the local address of the SRPtr */
                        localAddr = SharedRegion_getPtr((SharedRegion_SRPtr)
                                 nameserverEntry.instParams.shMemParams.cliGateMPAddr);
                        GT_assert(curTrace, (localAddr != NULL));

                        tmpStatus = GateMP_openByAddr(localAddr, &gateHandle);
                        if (tmpStatus != GateMP_S_SUCCESS) {
                            status = FrameQ_E_OPEN_GATEMP;
                            obj->creCliGate = FALSE;
                            /* Make it null so that finalization won't delete it.
                             */
                            obj->clientNotifyMgrGate = NULL;
                        }
                        else {
                            clientMgrParams.gate = (Ptr)gateHandle;
                            obj->clientNotifyMgrGate = gateHandle;
                            obj->creCliGate = TRUE;
                        }
                    }
                    else {
                        /* Open the gate created for the ClientNotifyMgr
                         * instance.
                         */
                        obj->creCliGate = TRUE;
                        GateMP_Params_init(&gateParams);

                        gateParams.localProtect  =
                                (GateMP_LocalProtect)pAttrs->localProtect;
                        gateParams.remoteProtect =
                                (GateMP_RemoteProtect)pAttrs->remoteProtect;

                        gateParams.sharedAddr = (Ptr)(virtAddr + addrOffset);
                        gateParams.regionId = obj->regionId;

                        addrOffset += ROUND_UP(GateMP_sharedMemReq(&gateParams),
                                               obj->minAlign);
                        tmpStatus = GateMP_openByAddr(gateParams.sharedAddr,
                                                  &gateHandle);
                        if (tmpStatus != GateMP_S_SUCCESS) {
                            status = FrameQ_E_OPEN_GATEMP;
                        }
                        else {
                            clientMgrParams.gate = (Ptr)gateHandle;
                            obj->clientNotifyMgrGate = gateHandle;
                        }
                    }

                    if (status >= 0) {
                        if (obj->ctrlStructCacheFlag == TRUE) {
                            clientMgrParams.cacheFlags =
                                               ClientNotifyMgr_CONTROL_CACHEUSE;
                        }

                        /* Open the client notifyMgr instance*/
                        clientMgrParams.openFlag = TRUE;

                        cliMgrHandle =
                                ClientNotifyMgr_create(&clientMgrParams);
                        if (cliMgrHandle == NULL) {
                            status = FrameQ_E_FAIL_CLIENTN0TIFYMGR_OPEN;
                            GT_setFailureReason(curTrace, GT_4CLASS,
                                    "_FrameQ_ShMem_open", status,
                                    "Failed to open the clientNotifyMgr "
                                    "instance!");
                            obj->clientNotifyMgrHandle = NULL;
                        }
                        else {
                            obj->clientNotifyMgrHandle = cliMgrHandle;
                            obj->objType       = FrameQ_ShMem_DYNAMIC_OPEN;
                            obj->interfaceType =
                                    params->commonCreateParams.ctrlInterfaceType;
                            obj->frameList = Memory_alloc(NULL,
                                    sizeof(UInt32) * pAttrs->numReaders, 0,
                                    eb);
                            if (obj->frameList == NULL) {
                                status = FrameQ_E_MEMORY;
                                GT_setFailureReason(curTrace, GT_4CLASS,
                                        "_FrameQ_ShMem_open", status,
                                        "Failed to allocate memory for "
                                        "frameList!");
                            }
                            else {
                                for (k = 0; k < pAttrs->numReaders; k++) {
                                    obj->frameList[k] = Memory_alloc(NULL,
                                            sizeof(UInt32) * pAttrs->numQueues,
                                            0, eb);
                                    if (obj->frameList[k] == NULL) {
                                        status = FrameQ_E_MEMORY;
                                        GT_setFailureReason(curTrace, GT_4CLASS,
                                                "_FrameQ_ShMem_open", status,
                                                "Failed to allocate memory for "
                                                "frameList!");
                                        /* TODO - how to clean up */
                                        break;
                                   }
                                }
                            }

                            if  (status >= 0) {
                                ListMP_Params_init(&listMPParams);
                                /* Set the region id in order to calculate the
                                 * shared mem required.
                                 */
                                listMPParams.regionId =
                                        SharedRegion_getId((Ptr)virtAddr);
                                listMPParams.gate = obj->gate;
                                listMPsharedAddrSize =
                                ListMP_sharedMemReq(&listMPParams);

                                /*
                                 * Each frmQueue entry in attrs maintains info
                                 * about queues that belongs  one reader.
                                 * Open all the listMPs created for this frameQ.
                                 */
                                for (i = 0; i < pAttrs->numReaders; i++) {
                                    for (k = 0; k < pAttrs->numQueues; k++) {
                                        /* Open the ListMP by using sharedAddr
                                         * as code will become complex if we
                                         * start using names for all the listMPs
                                         */
                                        listMPsharedAddr = (Ptr)
                                                ((UInt32)virtAddr + addrOffset);
                                        tmpStatus = ListMP_openByAddr(
                                                listMPsharedAddr,
                                                &(obj->frameList[i][k]));
                                        if (tmpStatus < 0){
                                            status = FrameQ_E_LISTMPOPEN;
                                            GT_setFailureReason(curTrace, GT_4CLASS,
                                                      "_FrameQ_ShMem_open", status,
                                                      "Failed to allocate memory for "
                                                      "frameList!");

                                            break;
                                        }
                                        addrOffset += ROUND_UP(listMPsharedAddrSize,
                                                              obj->minAlign);
                                    }
                                }
                            }
                        }
                    }
                }

                GateMP_leave(obj->gate, key);
            }
        }

        if ((strlen((String)params->commonCreateParams.name) > 0) &&
                (status >= 0)) {
            obj->name = Memory_alloc(NULL, FrameQ_ShMem_MAX_NAMELEN, 0, eb);
            if (obj->name == NULL) {
                status = FrameQ_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQ_ShMem_open", status,
                        "Failed to allocate memory for obj->name!");
            }
            else {
                if (strlen((String)params->commonCreateParams.name) <
                        FrameQ_ShMem_MAX_NAMELEN) {
                    strcpy(obj->name, (String)params->commonCreateParams.name);
                }
            }
        }

        if (status >= 0) {
            /* Put in the local list */
            /* Populate the params member */
            memcpy((Ptr)&obj->params, (Ptr)params,
                    sizeof(FrameQ_ShMem_Params));
            key0 = IGateProvider_enter (FrameQ_ShMem_module->listLock);
            List_elemClear(&obj->listElem);
            List_put((List_Handle)FrameQ_ShMem_module->objList,
                    &obj->listElem);
            IGateProvider_leave(FrameQ_ShMem_module->listLock, key0);
        }
    }

    /* Failed to crate /open the instance. Do clean up*/
    if (status < 0) {
        if (obj != NULL ) {
            if (obj->name != NULL) {
                Memory_free(NULL, obj->name, FrameQ_ShMem_MAXNAMELEN);
            }
            Memory_free(NULL, obj, sizeof (FrameQ_ShMem_Obj));
        }
        if (handle != NULL) {
            Memory_free(NULL, handle, sizeof (FrameQ_ShMem_Object));
            handle = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "_FrameQ_ShMem_open", handle);

    return (handle);
}

/*
 *  ======== _FrameQ_ShMem_setOpenFlags ========
 *  API to set the mode for the created client
 */
static Int32 _FrameQ_ShMem_setOpenFlags(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_OpenParams *openParams)
{
    Int32                   status      = FrameQ_S_SUCCESS;
    volatile FrameQ_ShMem_Attrs      *pAttrs;
    volatile FrameQ_ShMem_WriterClient *writer;
    volatile FrameQ_ShMem_ReaderClient *reader;
    volatile FrameQ_ShMem_FrameQueue   *frmQueue;
    UInt32                  key;
    UInt32                  i;
    UInt32                  index;
    UInt32                  primQId;
    FrameQ_ShMem_Obj        *obj;

    GT_2trace(curTrace, GT_ENTER, "_FrameQ_ShMem_setOpenFlags", handle,
               openParams);

    /* assert as the caller should have already checked these */
    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (openParams != NULL));

    obj = handle->obj;
    pAttrs = obj->attrs;
    writer = obj->writer;

        /*Invalidate  Control */
    GT_assert(curTrace, (obj->gate != NULL));
    key = GateMP_enter(obj->gate);
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                Cache_Type_ALL, TRUE);
        Cache_inv((Ptr)writer, sizeof(FrameQ_ShMem_WriterClient),
                Cache_Type_ALL, TRUE);
    }

    if (openParams->commonOpenParams.openMode == FrameQ_ShMem_MODE_WRITER) {
        /* Checking if writer is already  opened  */
        if (writer->isValid == TRUE) {
            status = FrameQ_E_WRITER_EXISTS;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "_FrameQ_ShMem_setOpenFlags", status,
                    "FrameQ writer already exists");
        }
        else {
            writer->isValid = TRUE;
            /* Get the caller processor id from the MultiProc module */
            writer->procId  = MultiProc_self();
            writer->cacheFlags = openParams->commonOpenParams.cpuAccessFlags;
            /*TODO: Add cache flags for controlstruct and frame header buffer in writer->cacheFlags */

            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv((Ptr)writer, sizeof(FrameQ_ShMem_WriterClient),
                        Cache_Type_ALL, TRUE);
            }

            obj->virtClientHandle = (void*)writer;
            obj->objMode          = FrameQ_ShMem_MODE_WRITER;
        }

    }
    else if (openParams->commonOpenParams.openMode ==
            FrameQ_ShMem_MODE_READER) {

        /* Check in  Reader clients to know which one is free */
        for (i = 0; i < pAttrs->numReaders; i++) {
            reader = obj->reader[i];
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv((Ptr)reader, sizeof(FrameQ_ShMem_ReaderClient),
                        Cache_Type_ALL, TRUE);
            }
            if (reader->isValid != TRUE) {
                break;
            }
        }

        if (i == pAttrs->numReaders) {
            /* Empty slot is not found in the array of readers
             * No more  additional readers*/
            status = FrameQ_E_MAX_READERS;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "_FrameQ_ShMem_setOpenFlags", status,
                    "no more empty reader slots");
        }
        else {
            /* Unused slot found in the reader list */
            reader->isValid    = TRUE;
            reader->procId     = MultiProc_self();
            reader->cacheFlags = openParams->commonOpenParams.cpuAccessFlags;
            pAttrs->numActiveReaders++;

            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                        Cache_Type_ALL, TRUE);
            }

            /* If it is the first reader and writer is still not active
             * set  queue of this reader as primay queue
             */
            if (pAttrs->numActiveReaders == 1) {
                primQId = pAttrs->primaryQueueId;

                frmQueue = obj->frmQueue[primQId];
                if (obj->ctrlStructCacheFlag == TRUE) {
                    Cache_inv((Ptr)frmQueue, sizeof(FrameQ_ShMem_FrameQueue),
                            Cache_Type_ALL, TRUE);
                }

                /*Check if  writer is in active state */
                if (writer->isValid == TRUE) {
                    /* No need to change the primary queue
                     * instead point this reader to the  primary
                     * queque.
                     */
                    reader->frameQIndex = primQId;
                    frmQueue->isAllocated = TRUE;
                    frmQueue->readerIndex = i;
                }
                else {
                    /* If writer is not active,
                     * Then also point this reader to the primary queue
                     */
                    reader->frameQIndex = primQId;
                    frmQueue->isAllocated = TRUE;
                    frmQueue->readerIndex = i;
                }

                if (obj->ctrlStructCacheFlag == TRUE) {
                    Cache_wbInv((Ptr)reader,
                            sizeof(FrameQ_ShMem_ReaderClient),
                            Cache_Type_ALL, TRUE);
                    Cache_wbInv((Ptr)frmQueue,
                            sizeof(FrameQ_ShMem_FrameQueue),
                            Cache_Type_ALL, TRUE);
                }
            }
            else {
                /* Assign empty queue for this reader */
                for (index = 0; index < pAttrs->numReaders; index++) {
                    frmQueue = obj->frmQueue[index];
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_inv((Ptr)frmQueue,
                                sizeof(FrameQ_ShMem_FrameQueue),
                                Cache_Type_ALL, TRUE);
                    }

                    if (frmQueue->isAllocated != TRUE) {
                        break;
                    }
                }

                if (index == pAttrs->numReaders) {
                    status = FrameQ_E_FAIL;
                }
                else {
                    reader->frameQIndex =  index;
                    frmQueue->isAllocated = TRUE;
                    frmQueue->readerIndex = i;

                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv((Ptr)reader,
                                sizeof(FrameQ_ShMem_ReaderClient),
                                Cache_Type_ALL, TRUE);
                        Cache_wbInv((Ptr)frmQueue,
                                sizeof(FrameQ_ShMem_FrameQueue),
                                Cache_Type_ALL, TRUE);
                    }
                }
            }
            if (status >= 0) {
                /* Point to the obj->virtControlHandle to this reader */
                obj->virtClientHandle = (void*)reader;
                obj->objMode          = FrameQ_ShMem_MODE_READER;
            }
        }
    }
    else {
        /*Invalid mode specified.Return error */
        status = FrameQ_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "_FrameQ_ShMem_setOpenFlags", status,
                "Invalid Open mode specified");
    }

    GateMP_leave(obj->gate, key);

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_setOpenFlags", status);

    return (status);
}


/*
 *  ======== FrameQ_ShMem_open ========
 *  Open a FrameQ instance
 */
Int32 FrameQ_ShMem_open(FrameQ_ShMem_OpenParams *openParams,
        FrameQ_ShMem_Handle *handlePtr)
{
    FrameQ_ShMem_Handle  handle    = NULL;
    Int32                status    = FrameQ_S_SUCCESS;
    Int32                tmpStatus = FrameQ_S_SUCCESS;
    FrameQ_ShMem_Params  params ;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_open", handlePtr, openParams);

    GT_assert(curTrace, (handlePtr != NULL));
    GT_assert(curTrace, (openParams != NULL));

    FrameQ_ShMem_Params_init(&params);

    /* This is open call . So Updating the params accordingly */
    params.commonCreateParams.openFlag       = TRUE ;
    /* Copy name */
    GT_assert (curTrace, (strlen((String)openParams->commonOpenParams.name)
            < FrameQ_ShMem_MAX_NAMELEN));

    /* Name is string pointer in CreateParams*/
    params.commonCreateParams.name = (String)
                                            openParams->commonOpenParams.name;
    /* Not passing shared addr */
    params.sharedAddr = openParams->commonOpenParams.sharedAddr;

    params.cpuAccessFlags = openParams->commonOpenParams.cpuAccessFlags;

    handle = _FrameQ_ShMem_open(&params);

    if (handle == NULL) {
        /*! @retval FrameQ_E_FAIL Failed to open the instance*/
        status = FrameQ_E_FAIL;
        *handlePtr = NULL;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_open",
                FrameQ_E_FAIL, "Failed to open the instance.");
    }
    else {
        /* Prepare this handle for the specified openParams */
        tmpStatus = _FrameQ_ShMem_setOpenFlags(handle, openParams);
        if (tmpStatus < 0) {
            status = tmpStatus;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_open",
                    status, "Failed to set Open Flags in the handle.");

            /*Delete the created FrameQ */
            FrameQ_ShMem_delete(&handle);
            *handlePtr = NULL;
        }
        else {
            *handlePtr = handle;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_open", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_delete ========
 *  Delete a FrameQ instance
 */
Int32 FrameQ_ShMem_delete(FrameQ_ShMem_Handle *pHandle)
{
    Int32 status = FrameQ_S_SUCCESS;
    volatile FrameQ_ShMem_Attrs *pAttrs     = NULL;
    UInt32                      i          = 0;
    volatile FrameQ_ShMem_WriterClient  *writer;
    FrameQ_ShMem_ReaderClient  *reader;
    volatile FrameQ_ShMem_FrameQueue    *frmQueue = NULL;
    IArg                        key;
    UInt32                      k;
    IArg                        key0;
    FrameQ_ShMem_Object        *handle;
    FrameQ_ShMem_Obj           *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_delete", pHandle);

    GT_assert(curTrace, (pHandle != NULL));

    if (pHandle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_delete", status,
                "pHandle passed is NULL!");
    }
    else if (*pHandle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_delete", status,
                "*pHandle passed is NULL!");
    }
    else {

        handle = *pHandle;
        obj = (FrameQ_ShMem_Obj *) handle->obj;
        GT_assert(curTrace, (obj != NULL));

        if (NULL != obj) {
            switch (obj->objType) {

                case FrameQ_ShMem_DYNAMIC_OPEN:
                    key0 = IGateProvider_enter (FrameQ_ShMem_module->listLock);
                    /* Remove it from the local list */
                    List_remove ((List_Handle)FrameQ_ShMem_module->objList,
                                 &obj->listElem);
                    IGateProvider_leave (FrameQ_ShMem_module->listLock, key0);
                    /*
                     *  Update  the shared control info corresponding to this
                     *  client as invalid and Update the other shared control
                     *  info.
                     */
                    pAttrs = obj->attrs;
                    key = GateMP_enter (obj->gate);

                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_inv((Ptr)pAttrs,
                                  sizeof (FrameQ_ShMem_Attrs),
                                  Cache_Type_ALL,
                                  TRUE);
                    }
                    if (pAttrs != NULL) {
                       /* Close frame queue lists */
                        for (i = 0; i < pAttrs->numReaders; i++ ) {
                            for (k = 0; k < pAttrs->numQueues; k++ ) {
                                if (obj->frameList[i][k] != NULL) {
                                    ListMP_close (
                                        (ListMP_Handle*)&(obj->frameList[i][k]));
                                }
                            }
                        }
                        if (obj->objMode ==  (UInt32)FrameQ_ShMem_MODE_WRITER) {
                            writer = obj->writer;
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_inv((Ptr)writer,
                                          sizeof (FrameQ_ShMem_WriterClient),
                                          Cache_Type_ALL,
                                          TRUE);
                            }
                            /*  Set the writer shared control info as invalid  */
                            writer->isValid    = FALSE;
                            writer->cacheFlags = 0u;
                            writer->procId     = MultiProc_INVALIDID;

                            if (writer->isRegistered == TRUE) {
                                FrameQBufMgr_unregisterNotifier(
                                                          obj->frameQBufMgrHandle);
                            }

                            if (obj->ctrlStructCacheFlag == TRUE) {
                                /* Write back the writer shared control info */
                                Cache_wbInv ((Ptr)writer,
                                             sizeof (FrameQ_ShMem_WriterClient),
                                             Cache_Type_ALL,
                                             TRUE);
                            }
                            obj->objType = 0;

                        }
                        else if (    obj->objMode
                                 == (UInt32)FrameQ_ShMem_MODE_READER) {
                            /* Point to the correct shared client info */
                            reader =
                                (FrameQ_ShMem_ReaderClient *) obj->virtClientHandle;
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_inv (reader,
                                           sizeof (FrameQ_ShMem_ReaderClient),
                                           Cache_Type_ALL,
                                           TRUE);
                            }
                            if (reader != NULL) {
                                reader->isValid    = FALSE;
                                reader->procId     = MultiProc_INVALIDID;
                                reader->cacheFlags = 0u;

                                frmQueue =  (FrameQ_ShMem_FrameQueue*)
                                                 obj->frmQueue[reader->frameQIndex];
                                frmQueue->isAllocated = FALSE;
                                frmQueue->readerIndex = (UInt32)-1;

                                if (reader->isRegistered == TRUE) {
                                    ClientNotifyMgr_unregisterClient (
                                                         obj->clientNotifyMgrHandle,
                                                         reader->notifyId);
                                }

                                /* Do not modify the num elements and list, as  the
                                 * queue can be reopened and used.In that  case,
                                 * the frames in the correspondig list can be
                                 * retrieved and used by the next reader.
                                 */
                                pAttrs->numActiveReaders -= 1u;
                            }

                            if (pAttrs->numActiveReaders == 0) {
                                /* TODO: Check do we need to change  primay Queue index
                                 * to zero.
                                 */

                            }
                            obj->objType = 0;
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_wbInv ((Ptr)pAttrs,
                                             sizeof (FrameQ_ShMem_Attrs),
                                             Cache_Type_ALL,
                                             TRUE);
                                Cache_wbInv ((Ptr)reader,
                                             sizeof (FrameQ_ShMem_ReaderClient),
                                             Cache_Type_ALL,
                                             TRUE);
                                Cache_wbInv ((Ptr)frmQueue,
                                             sizeof (FrameQ_ShMem_FrameQueue),
                                             Cache_Type_ALL,
                                             TRUE);
                            }

                        }
                        else {
                            /* If mode is other than read or write simply delete
                             * the obejct with out touching the  shared control
                             * structures.This case is possible when
                             * setOpenFlags API has failed.
                             */
                        }
                    }
                    /* Close client NotifyMgr handle */
                    if (obj->clientNotifyMgrHandle != NULL) {
                        ClientNotifyMgr_close (&obj->clientNotifyMgrHandle);
                    }
                    if (   (obj->creCliGate == TRUE)
                        && (obj->clientNotifyMgrGate != NULL) ) {
                        /* Close GateMP instance of ClientNotifyMgr.*/
                        GateMP_close (&obj->clientNotifyMgrGate);
                    }

                    for (i = 0; i < pAttrs->numReaders; i++ ) {
                        if (obj->frameList[i] != NULL) {
                            Memory_free(NULL,
                                       (Ptr)obj->frameList[i],
                                        sizeof(UInt32)* pAttrs->numQueues );
                        }
                    }
                    if (obj->frmQueue != NULL) {
                        Memory_free(NULL,
                                     obj->frmQueue,
                                     (  sizeof(UInt32)
                                      * pAttrs->numReaders
                                      * pAttrs->numQueues));
                    }
                    if (obj->reader != NULL) {
                        Memory_free(NULL,
                                    obj->reader,
                                    sizeof(UInt32) * pAttrs->numReaders);
                    }

                    if (obj->frameList != NULL) {
                        Memory_free(NULL,
                                   (Ptr)obj->frameList,
                                    sizeof(UInt32)* pAttrs->numReaders);
                    }

                    GateMP_leave (obj->gate, key);

                    if (obj->name != NULL) {
                        Memory_free(NULL, (Ptr)obj->name, FrameQ_ShMem_MAX_NAMELEN );
                    }
                    /* Close the instance gate if it is opened internally
                     * in the instance.
                     */
                    if ((obj->creGate == TRUE) &&(obj->gate != NULL)) {
                        GateMP_close (&obj->gate);
                    }
                    /* Close the FrameQBufMgr instance opened by this client.*/
                    if (obj->frameQBufMgrHandle != NULL) {
                        FrameQBufMgr_close(&obj->frameQBufMgrHandle);
                    }

                    /* Now free the handle */
                    Memory_free(NULL, obj, sizeof(FrameQ_ShMem_Obj));
                    obj = NULL;
                    Memory_free(NULL, handle, sizeof(FrameQ_ShMem_Object));
                    handle = NULL;
                    break;

                case FrameQ_ShMem_DYNAMIC_CREATE:
                case FrameQ_ShMem_DYNAMIC_CREATE_USEDREGION:
                    pAttrs =  obj->attrs;

                    key0 = IGateProvider_enter (FrameQ_ShMem_module->listLock);

                    /* Remove it from the local list */
                    List_remove ((List_Handle)FrameQ_ShMem_module->objList,
                                 &obj->listElem);

                    IGateProvider_leave(FrameQ_ShMem_module->listLock, key0);

                    key = GateMP_enter(obj->gate);
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                                Cache_Type_ALL, TRUE);
                    }
                    pAttrs->status = !FrameQ_ShMem_CREATED;

                    /* Flush all the remaining frames */
                    FrameQ_ShMem_flushAll(handle);

                   /* Close frame queue lists */
                    for (i = 0; i < pAttrs->numReaders; i++ ) {
                        for (k = 0; k < pAttrs->numQueues; k++ ) {
                            if (obj->frameList[i][k] != NULL) {
                                ListMP_delete(
                                      (ListMP_Handle*)&(obj->frameList[i][k]));
                            }
                        }
                    }
                    for (i = 0; i < pAttrs->numReaders; i++ ) {
                        if (obj->frameList[i] != NULL) {
                            Memory_free(NULL, (Ptr)obj->frameList[i],
                                    sizeof(UInt32) * pAttrs->numQueues);
                        }
                    }

                    if (obj->frameList != NULL) {
                        Memory_free(NULL, (Ptr)obj->frameList,
                                sizeof(UInt32) * pAttrs->numReaders);
                    }
                    if (obj->frmQueue != NULL) {
                        Memory_free(NULL, obj->frmQueue, sizeof(UInt32) *
                                pAttrs->numReaders * pAttrs->numQueues);
                    }
                    if (obj->reader != NULL) {
                        Memory_free(NULL, obj->reader,
                                sizeof(UInt32) * pAttrs->numReaders);
                    }

                    memset ((Ptr)pAttrs, 0, sizeof(FrameQ_ShMem_Attrs));
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                                Cache_Type_ALL, TRUE);
                    }

                    if (obj->clientNotifyMgrHandle != NULL) {
                        ClientNotifyMgr_delete(&(obj->clientNotifyMgrHandle));
                    }

                    /* Delete GateMP instance of ClientNotifyMgr.*/
                    if ((obj->creCliGate == TRUE) &&
                            (obj->clientNotifyMgrGate != NULL)) {
                        GateMP_delete(&obj->clientNotifyMgrGate);
                    }

                    /* Close the FrameQBufMgr instance opened by this client.*/
                    if (obj->frameQBufMgrHandle != NULL) {
                        FrameQBufMgr_close(&obj->frameQBufMgrHandle);
                    }

                    /* Remove entry from NameServer */
                    if ((NULL != FrameQ_ShMem_module->nameServer) &&
                            (obj->nsKey != 0)) {
                        NameServer_removeEntry(FrameQ_ShMem_module->nameServer,
                                obj->nsKey);
                    }

                    GateMP_leave (obj->gate, key);

                    /* Free the allocated memory for the instance control structure
                     */
                    if (obj->objType == FrameQ_ShMem_DYNAMIC_CREATE_USEDREGION) {
                        Memory_free(SharedRegion_getHeap(obj->regionId),
                                obj->pnameserverEntry, obj->allocSize);
                    }

                    /* Delete instance gate if it is opened internally
                     * in the instance.
                     */
                    if ((obj->creGate == TRUE) && (obj->gate != NULL)) {
                        GateMP_delete(&obj->gate);
                        /* Free the allocated memory for the instance control
                         * structure.
                         */
                        Memory_free(SharedRegion_getHeap(obj->regionId),
                                obj->instGateSharedAddr, obj->gateMPAllocSize);
                    }

                    if (obj->name != NULL) {
                        Memory_free(NULL, (Ptr)obj->name, FrameQ_ShMem_MAX_NAMELEN );
                    }

                    /* Now free the handle */
                    Memory_free(NULL, obj, sizeof(FrameQ_ShMem_Obj));
                    obj = NULL;
                    Memory_free(NULL, handle, sizeof(FrameQ_ShMem_Object));
                    handle = NULL;

                    break;

                default:
                    /* Object does  not have proper object type defined*/
                    status = FrameQ_E_FAIL;
                    break;
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_delete", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_close ========
 *  API to close the FrameQ.
 */
Int32 FrameQ_ShMem_close(FrameQ_ShMem_Handle *phandle)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_close", phandle);

    GT_assert(curTrace, (phandle != NULL));
    GT_assert(curTrace, (*phandle != NULL));

    status = FrameQ_ShMem_delete(phandle);

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_close", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_alloc ========
 * Function to allocate a  frame.
 */
Int32 FrameQ_ShMem_alloc(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame *framePtr)
{
    Int32 status = FrameQ_S_SUCCESS;
    Int32 tmpStatus;
    FrameQ_ShMem_Obj *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_ShMem_alloc", handle, framePtr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));

    obj = handle->obj;
    GT_assert (curTrace, (obj != NULL));

    if (obj->objMode != FrameQ_ShMem_MODE_WRITER) {
        /* If Caller is other than writer, return error. */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_alloc",
                status, "handle does not have permission to"
                        " alloc (not a writer)!");

        *framePtr = NULL;
    }
    else {
        /* Get the frame from bufMgr. */
        GT_assert (curTrace, (obj->frameQBufMgrHandle != NULL));
        tmpStatus = FrameQBufMgr_alloc(obj->frameQBufMgrHandle,
                (FrameQBufMgr_Frame *)framePtr);
        if (tmpStatus < 0) {
            /* Return FrameQ error msg */
            status = FrameQ_E_FAIL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_alloc", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_allocv ========
 *  Allocate multiple frames
 */
Int32 FrameQ_ShMem_allocv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 freeQId[], UInt8 *numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;
    Int32 tmpStatus;
    FrameQ_ShMem_Obj *obj;

    GT_4trace (curTrace, GT_ENTER, "FrameQ_ShMem_allocv", handle, framePtr,
            freeQId, *numFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));
    GT_assert(curTrace, (freeQId != NULL));
    GT_assert(curTrace, (numFrames != NULL));

    obj = handle->obj;
    GT_assert (curTrace, (obj != NULL));

    if (obj->objMode != FrameQ_ShMem_MODE_WRITER) {
        /* If Caller is other than writer, return error. */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_allocv",
                status, "The provided handle does not have permission to"
                " alloc (not writer)!");

    }
    else {
        /* Get the frame from free queue 0 of FrameQ bufMgr. */
        GT_assert (curTrace, (obj->frameQBufMgrHandle != NULL));

        /* Allocate  frames from the plugged in FrameQBufMgr instance.
         * Base frame headers need not be populated  here as FrameQBufMgr will
         * return the frames  having updated headers.
         *
         *  If Alloc fails FrameQbufMgr takes care of enablling notifications
         *  for the writer clients.
         */
        tmpStatus = FrameQBufMgr_allocv(obj->frameQBufMgrHandle, framePtr,
                freeQId, numFrames);

        if (tmpStatus < 0) {
            /* return FrameQ error */
            status = FrameQ_E_FAIL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_allocv", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_free ========
 *  Free single frame
 */
Int32 FrameQ_ShMem_free(FrameQ_ShMem_Handle handle, FrameQ_ShMem_Frame frame)
{
    Int32 status = FrameQ_S_SUCCESS;
    Int32 tmpStatus = FrameQ_S_SUCCESS;
    FrameQ_ShMem_Obj *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_free", frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame != NULL));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    GT_assert(curTrace, ((obj->objMode == FrameQ_ShMem_MODE_WRITER) ||
            (obj->objMode == FrameQ_ShMem_MODE_READER)));

    tmpStatus = FrameQBufMgr_free(obj->frameQBufMgrHandle, frame);

    if  (tmpStatus < 0) {
        status = FrameQ_E_FAIL;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_free", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_freev ========
 *  Free multiple frames
 */
Int32 FrameQ_ShMem_freev(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;
    Int32 tmpStatus = FrameQ_S_SUCCESS;
    FrameQBufMgr_Handle bufMgrHandle;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_ShMem_freev", framePtr);

    /* Note that (strangely?) this fxn doesn't use the provided handle */
    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));
    GT_assert(curTrace, (numFrames != 0));

    /* Currently assuming all frames from the same buffer manager allocater */
    /* We could assert that all framePtr's bufMgrHandles are the same */
    bufMgrHandle = FrameQBufMgr_getHandle(framePtr[0]->frmAllocaterId);
    GT_assert (curTrace, (NULL != bufMgrHandle));

    if (bufMgrHandle == NULL) {
        status = FrameQ_E_FAIL;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_ShMem_freev",
                status, "FrameQBufMgr handle is null!");
    }
    else {
        tmpStatus = FrameQBufMgr_freev(bufMgrHandle, framePtr, numFrames);
        if (tmpStatus < 0) {
            status = FrameQ_E_FAIL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_freev", status);

    return (status);
}

/*
 * ======== FrameQ_ShMem_put ========
 * Insert frame into FrameQ
 */
Int32 FrameQ_ShMem_put(FrameQ_ShMem_Handle handle, FrameQ_ShMem_Frame frame)
{
    Int32 status = FrameQ_S_SUCCESS;
    UInt32 i = 0;
    Bool tonotify = FALSE;
    volatile FrameQ_ShMem_Attrs *pAttrs = NULL;
    FrameQ_ShMem_Frame      pframe[FrameQ_MAX_INST_READERS] = {NULL};
    volatile FrameQ_ShMem_ReaderClient *reader;
    volatile FrameQ_ShMem_FrameQueue *queue;
    UInt32 counter;
    FrameQ_ShMem_Frame temp;
    UInt32 key;
    Int32 tmpStatus;
    FrameQBufMgr_Handle bufMgrHandle;
    Ptr portablePtr;
    UInt8 filledQueueNo = 0;
    UInt32 k;
    FrameQ_ShMem_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_put", handle, frame);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != frame));
    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    GT_assert(curTrace, ((obj->objMode == FrameQ_ShMem_MODE_WRITER) ||
            (obj->objMode == FrameQ_ShMem_MODE_READER)));

    pAttrs = obj->attrs;

    filledQueueNo = 0;
    key = GateMP_enter(obj->gate);

    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs), Cache_Type_ALL,
                TRUE);
    }

    if (obj->objMode == FrameQ_ShMem_MODE_READER) {
        reader = obj->virtClientHandle;
        queue = (FrameQ_ShMem_FrameQueue *)
                                         obj->frmQueue[reader->frameQIndex];
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv ((Ptr)queue,
                       sizeof (FrameQ_ShMem_FrameQueue),
                       Cache_Type_ALL,
                       TRUE);
        }
        if (queue->isAllocated != TRUE) {
            status = FrameQ_E_FAIL;
        }
        else {
            /* If client happens to be  reader, then just put the frame at
             * the head of the corresponding reader's queue
             */
            /* Convert all address to sharead region address in
             * frame header.i.e frame buffer pointers
             */
            bufMgrHandle = obj->frameQBufMgrHandle;
            GT_assert(curTrace, (NULL != bufMgrHandle));

            for (k = 0; k < frame->numFrameBuffers; k++) {
                if (   (obj->frmBufCacheFlag[k] == TRUE)
                    && (obj->bufCpuAccessFlag[k] == TRUE)) {
                status = FrameQBufMgr_writeBackFrameBuf(
                                bufMgrHandle,
                               (Ptr)
       (frame->frameBufInfo[k].bufPtr + frame->frameBufInfo[k].startOffset),
                               frame->frameBufInfo[k].validSize,
                               k);
                }
                /* Address translation to portable pointers */
                status =  FrameQBufMgr_translateAddr (
                                    bufMgrHandle,
                                    &portablePtr,
                                    FrameQBufMgr_AddrType_Portable,
                                    (Ptr)frame->frameBufInfo[k].bufPtr,
                                    FrameQBufMgr_AddrType_Virtual,
                                    FrameQBufMgr_BUF_FRAMEBUF);

                GT_assert (curTrace, (NULL != portablePtr));
                frame->frameBufInfo[k].bufPtr = (UInt32)portablePtr;
            }

            if (obj->frmHdrBufCacheFlag == TRUE) {
                status = FrameQBufMgr_writeBackHeaderBuf(bufMgrHandle,
                                                         frame);
            }
            /* Put in to filled queue 0 of the reader */
            ListMP_putHead ((ListMP_Handle)obj->frameList[reader->frameQIndex][filledQueueNo] ,
                           (ListMP_Elem *)frame);
            queue->numFrames[filledQueueNo]++;

            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv ((Ptr)queue,
                             sizeof (FrameQ_ShMem_FrameQueue),
                             Cache_Type_ALL,
                             TRUE);
            }
        }


    }
    else {
        bufMgrHandle = obj->frameQBufMgrHandle;
        GT_assert(curTrace, (NULL != bufMgrHandle));

        if (pAttrs->numActiveReaders > 1) {
            /*More than one reader exist. Need to duplicate frame to insert
             *frames in to queues other than primary queue.
             */
             /* Allocate frames  by duplicating. Need
              * to allocate as many as there are number of active readers
              * except primary queue reader.
              */

            tmpStatus = FrameQBufMgr_dup (bufMgrHandle,
                                          (FrameQBufMgr_Frame)frame,
                                          (FrameQBufMgr_Frame *)pframe,
                                          pAttrs->numActiveReaders);
            if (tmpStatus < 0) {
                status = tmpStatus;
            }
        }
        if (status >= 0) {
            /* Write back the original frame */

            /* Translate the frame buffer address to portable addreess */
            for (k = 0; k < frame->numFrameBuffers; k++) {
                if (   (obj->frmBufCacheFlag[k] == TRUE)
                    && (obj->bufCpuAccessFlag[k] == TRUE)) {
                    status = FrameQBufMgr_writeBackFrameBuf(
                               bufMgrHandle,
                               (Ptr)
       (frame->frameBufInfo[k].bufPtr + frame->frameBufInfo[k].startOffset),
                               frame->frameBufInfo[k].validSize,
                               k);
                }
                /* Address translation to portable pointers */
                status =  FrameQBufMgr_translateAddr (
                                    bufMgrHandle,
                                    &portablePtr,
                                    FrameQBufMgr_AddrType_Portable,
                                    (Ptr)frame->frameBufInfo[k].bufPtr,
                                    FrameQBufMgr_AddrType_Virtual,
                                    FrameQBufMgr_BUF_FRAMEBUF);

                GT_assert(curTrace, (NULL != portablePtr));
                frame->frameBufInfo[k].bufPtr = (UInt32)portablePtr;
            }

            if (obj->frmHdrBufCacheFlag == TRUE) {
                status = FrameQBufMgr_writeBackHeaderBuf(bufMgrHandle,
                                                         frame);
            }

            /* Copy the contents of original frame header contents to other
             * duped frame headers.
             */
            if (pAttrs->numActiveReaders > 1) {
                for (i = 0; i < pAttrs->numActiveReaders -1; i++) {
                    memcpy (pframe[i], frame, frame->headerSize);
                }
            }
        }

        if (pAttrs->numActiveReaders > 1) {
             counter = 0;

             for (  i = 0; i < pAttrs->numReaders; i++) {
                if( i != pAttrs->primaryQueueId) {
                    queue = obj->frmQueue[i];
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_inv((Ptr)queue,
                                  sizeof (FrameQ_ShMem_FrameQueue),
                                  Cache_Type_ALL,
                                  TRUE);
                    }

                    temp = (FrameQ_ShMem_Frame)pframe[i];
                }
                else {
                    /* Insert the original frame in to primary queue
                     * irrespective of numActive readers
                     */
                    queue = obj->frmQueue[pAttrs->primaryQueueId];
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_inv((Ptr)queue,
                                  sizeof (FrameQ_ShMem_FrameQueue),
                                  Cache_Type_ALL,
                                  TRUE);
                    }
                    temp = frame;

                }
                if (   (queue->isAllocated == TRUE)
                    || (i == pAttrs->primaryQueueId)) {
                    /* Allow insertion in to primary queue evene if it
                     * is not allocated to any reader.
                     */
                    counter++;
                    /* Increment the frame count of filled queue 0 of
                     * reader.
                     */
                    queue->numFrames[filledQueueNo]++;
                    /* Since frame headers are also from share region,
                     * Internal ListMP operation on list element will not be
                     * a problem(ListMP calls Shared region APIs on the list
                     * element).
                     */
                    ListMP_putTail ((ListMP_Handle)obj->frameList[i][filledQueueNo] ,
                                    (ListMP_Elem *)temp);
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv((Ptr)queue,
                                    sizeof (FrameQ_ShMem_FrameQueue),
                                    Cache_Type_ALL,
                                    TRUE);
                    }
                    /* Send Notification to the Reader if it
                     * is registered for notification
                     */
                    if (pAttrs->numActiveReaders > 0) {
                        reader = obj->reader[queue->readerIndex];
                        if (obj->ctrlStructCacheFlag == TRUE) {
                             Cache_inv((Ptr)reader,
                                       sizeof (FrameQ_ShMem_ReaderClient),
                                       Cache_Type_ALL,
                                       TRUE);
                        }
                        tonotify = TRUE;
                    }
                    if ((tonotify == TRUE)&&(reader->isRegistered == TRUE)) {
                        /* Release lock */
                        GateMP_leave ((GateMP_Handle)obj->gate, key);
                        status =
                            ClientNotifyMgr_sendNotification (
                                             (ClientNotifyMgr_Handle)obj->clientNotifyMgrHandle,
                                             reader->notifyId,
                                             filledQueueNo,
                                             queue->numFrames[filledQueueNo],
                                            (UInt32)-1);
                        key = GateMP_enter (obj->gate);

                    }
                }

                if (counter == pAttrs->numActiveReaders) {
                    break;
                }
             }
        }
        else {
           /* Insert the original frame in to primary queue
            */
            queue = obj->frmQueue[pAttrs->primaryQueueId];
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv((Ptr)queue,
                          sizeof (FrameQ_ShMem_FrameQueue),
                          Cache_Type_ALL,
                          TRUE);
            }
            temp = frame;
            queue->numFrames[filledQueueNo]++;

            /* Check if queue is allocated  to reader then only do invalidation
             * of reader to send notifications. There may be a case
             * where reader is not opened yet.
             */
            if (queue->isAllocated == TRUE) {
                reader = (FrameQ_ShMem_ReaderClient*)
                                                obj->reader[queue->readerIndex];
                if (obj->ctrlStructCacheFlag == TRUE) {
                     Cache_inv((Ptr)reader,
                               sizeof (FrameQ_ShMem_ReaderClient),
                               Cache_Type_ALL,
                               TRUE);
                }
                tonotify = TRUE;
            }

            ListMP_putTail ((ListMP_Handle)obj->frameList[pAttrs->primaryQueueId][filledQueueNo] ,
                           (ListMP_Elem *)temp);
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv((Ptr)queue, sizeof(FrameQ_ShMem_FrameQueue),
                        Cache_Type_ALL, TRUE);
            }

            /* Check and send notification to  the corresponding reader
            * only if notification is registered.
            */
            if ((tonotify == TRUE) && (reader->isRegistered == TRUE)) {
                /* Release lock */
                GateMP_leave(obj->gate, key);

                status = ClientNotifyMgr_sendNotification (
                        obj->clientNotifyMgrHandle, reader->notifyId,
                        filledQueueNo, queue->numFrames[filledQueueNo],
                        (UInt32)-1);
                return (status);

            }
        }
    }

    /* Release lock */
    GateMP_leave(obj->gate, key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_put", status);

    return (status);
}

/*
 * ======== FrameQ_ShMem_putv ========
 * Insert multiple frames into queues
 */
Int32 FrameQ_ShMem_putv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 filledQueueId[], UInt8 numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_putv", handle, framePtr);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != framePtr));
    GT_assert(curTrace, (NULL != filledQueueId));
    GT_assert (curTrace, (NULL != numFrames));
    GT_assert(curTrace, (handle->obj != NULL));

    if (handle->obj->objMode == FrameQ_ShMem_MODE_READER){
        status = FrameQ_ShMem_readerPutv(handle, framePtr, filledQueueId,
                numFrames);
    }
    else  {
        GT_assert(curTrace,
                (handle->obj->objMode == FrameQ_ShMem_MODE_WRITER));

        status = FrameQ_ShMem_writerPutv(handle, framePtr, filledQueueId,
                numFrames);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_putv", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_get ========
 *  Get frame from FrameQ
 */
Int32 FrameQ_ShMem_get(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame *framePtr)
{
    Int32                        status  = FrameQ_S_SUCCESS;
    volatile FrameQ_ShMem_Attrs          *pAttrs;
    FrameQ_ShMem_ReaderClient   *reader;
    volatile FrameQ_ShMem_FrameQueue     *queue;
    FrameQ_ShMem_Frame           frame;
    Ptr                          bufPtr;
    Int32                        key;
    UInt32                       i;
    UInt8                        filledQueueNo;
    FrameQ_ShMem_Obj            *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_ShMem_get", handle, framePtr);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != framePtr));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    if (obj->objMode != FrameQ_ShMem_MODE_READER) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_ShMem_get", status,
                "The provided handle cannot get (not reader)!");
    }
    else {
        pAttrs =  obj->attrs;

        /* Get simply retrieves frame from the filled queue 0 of the reader */
        filledQueueNo = 0;
        reader = (FrameQ_ShMem_ReaderClient *)obj->virtClientHandle;

        /* Acquire lock */
        key = GateMP_enter(obj->gate);
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv (reader,
                       sizeof (FrameQ_ShMem_ReaderClient),
                       Cache_Type_ALL,
                       TRUE);
        }

        if (reader->isValid != TRUE) {
            status = FrameQ_E_ACCESSDENIED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_get",
                    status, "The provided handle is not valid!");
        }
        else {
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                        Cache_Type_ALL, TRUE);
            }
            queue = obj->frmQueue[reader->frameQIndex];
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv((Ptr)queue, sizeof(FrameQ_ShMem_FrameQueue),
                        Cache_Type_ALL, TRUE);
            }

            if (queue->numFrames[filledQueueNo] == 0 ) {
                status = FrameQ_E_EMPTY;
            }
            else {
                frame = (FrameQ_ShMem_Frame)ListMP_getHead ((ListMP_Handle)
                            obj->frameList[reader->frameQIndex][filledQueueNo]);

                GT_assert(curTrace, (NULL != frame));

                if (frame != NULL) {
                    queue->numFrames[filledQueueNo] -= 1;
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv((Ptr)queue,
                                sizeof(FrameQ_ShMem_FrameQueue),
                                Cache_Type_ALL, TRUE);
                    }

                    /* Invalidate  frame header.
                     * Conevert portable frame pointers to virtual pointers.
                     * Then do invalidate frame buffers.
                     */
                    if (obj->frmHdrBufCacheFlag == TRUE){
                        status = FrameQBufMgr_invalidateHeaderBuf(
                                obj->frameQBufMgrHandle, frame);
                    }

                    /* Convert  portable address of framebuffers to  virtual
                     * address in the retrieved frame.
                     */
                     for (i = 0; i < frame->numFrameBuffers; i++) {
                        /* Address translation to virtual pointers */
                        status =  FrameQBufMgr_translateAddr (
                                            obj->frameQBufMgrHandle,
                                            &bufPtr,
                                            FrameQBufMgr_AddrType_Virtual,
                                            (Ptr)frame->frameBufInfo[i].bufPtr,
                                            FrameQBufMgr_AddrType_Portable,
                                            FrameQBufMgr_BUF_FRAMEBUF);

                        GT_assert (curTrace, (NULL != bufPtr));
                        frame->frameBufInfo[i].bufPtr = (UInt32)bufPtr;

                        if ((obj->frmBufCacheFlag[i] == TRUE)
                           && (obj->bufCpuAccessFlag[i] == TRUE)) {
                            /* Invalidate frame buffers */
                            status = FrameQBufMgr_invalidateFrameBuf(
                                            obj->frameQBufMgrHandle,
                                           (Ptr)(frame->frameBufInfo[i].bufPtr +
                                            frame->frameBufInfo[i].startOffset),
                                            frame->frameBufInfo[i].validSize,
                                            i);
                        }
                        *framePtr = frame;
                    }

                }
                else {
                    status = FrameQ_E_FAIL;
                }
            }
        }

        /* Release lock */
        GateMP_leave (obj->gate, key);

        if (status < 0) {
            *framePtr = NULL;
            if (    (reader->isRegistered == TRUE)
                 && (reader->notifyId != (UInt32)-1)) {
                ClientNotifyMgr_enableNotification((ClientNotifyMgr_Handle)obj->clientNotifyMgrHandle,
                                                     reader->notifyId,
                                                     filledQueueNo);
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_get", status);

    return (status);
}

/*
 * ======== FrameQ_ShMem_getv ========
 * Function to retrieve frames from  queues of a FrameQ Reader client.
 * It returns frame if it is available in the queue.
 */
Int32 FrameQ_ShMem_getv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame pframe[], UInt32 filledQueueId[], UInt8 *numFrames)
{
    Int32                        status     = FrameQ_S_SUCCESS;
    UInt32                       lNumFrames = 0;
    volatile FrameQ_ShMem_Attrs *pAttrs;
    FrameQ_ShMem_ReaderClient   *reader;
    volatile FrameQ_ShMem_FrameQueue     *queue;
    FrameQ_ShMem_Frame           frame;
    Ptr                          bufPtr;
    Int32                        key;
    UInt32                       i;
    UInt32                       j;
    UInt8                        filledQueueNo;
    Int32                        tmpStatus;
    /* TO find out the duplicate freeQ ids and frames requested in that free Q*/
    UInt32                       pfilledQIds[FrameQ_MAX_FRAMESINVAPI]= {0};
    UInt32                       pNumFrames[FrameQ_MAX_FRAMESINVAPI]= {0};
    UInt32                       count = 0;
    UInt32                       pNotifySubIds[FrameQ_MAX_FRAMESINVAPI] = {0};
    UInt32                       numSubEntries = 0;
    FrameQ_ShMem_Obj            *obj;

    GT_4trace(curTrace, GT_ENTER, "FrameQ_ShMem_getv", handle, pframe,
            filledQueueId, *numFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != pframe));
    GT_assert(curTrace, (NULL != filledQueueId));
    GT_assert(curTrace, (NULL != numFrames));
    GT_assert(curTrace, (FrameQ_MAX_FRAMESINVAPI >= *numFrames));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    if (obj->objMode != FrameQ_ShMem_MODE_READER) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_ShMem_get", status,
                "The provided handle cannot get (not reader)!");
    }
    else {
        /* Find out the duplicate entries in freeQids specified */
        for (i = 0 ; i < *numFrames ; i++) {
            if (pfilledQIds [i] != (UInt32) -1) {
                pfilledQIds [count] = filledQueueId[i];
                pNumFrames [count] += 1u;

                for (j = i + 1 ; j < *numFrames ; j++) {
                    if (pfilledQIds [count] ==  filledQueueId[j]) {
                        pNumFrames [count] += 1;
                        pfilledQIds [j] = (UInt32) -1 ;
                    }
                }
                count++ ;
            }
        }

        pAttrs =  obj->attrs;
        reader = (FrameQ_ShMem_ReaderClient *)obj->virtClientHandle;

        /* Acquire  lock */
        key = GateMP_enter (obj->gate);

        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv(reader, sizeof(FrameQ_ShMem_ReaderClient),
                    Cache_Type_ALL, TRUE);
        }


        if (reader->isValid != TRUE) {
            status = FrameQ_E_ACCESSDENIED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_ShMem_getv",
                    status, "Internal error - reader invalid!");
        }
        else {
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs),
                        Cache_Type_ALL, TRUE);
            }
            queue = obj->frmQueue[reader->frameQIndex];
            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)queue, sizeof(FrameQ_ShMem_FrameQueue),
                        Cache_Type_ALL, TRUE);
            }

            for (i = 0; i < count; i++) {
                filledQueueNo = filledQueueId[i];
                /* Check if the filledquque has sufficient frames */
                if (queue->numFrames[filledQueueNo] < pNumFrames [i]) {
                    status = FrameQ_E_INSUFFICENT_FRAMES;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "FrameQ_ShMem_getv", status,
                            "insufficient frames!");

                    /* We are using subNotifyentry 0 for framePool 0 and
                     * so on
                     */
                    pNotifySubIds[i] = filledQueueNo;
                    numSubEntries +=1;
                 }
            }

            if (status >= 0) {
                for (i = 0; i < *numFrames; i++) {
                    filledQueueNo = filledQueueId[i];
                    if (queue->numFrames[filledQueueNo] == 0 ) {
                        /* Should not be here */
                        status = FrameQ_E_EMPTY;
                        pframe[i] = NULL;
                        if ((reader->isRegistered == TRUE) &&
                                (reader->notifyId != (UInt32)-1)) {
                            /* Enable notification on the subnotifyEntry  to
                             * get notification on this filledQueueId.
                             */
                            ClientNotifyMgr_enableNotification (
                                    obj->clientNotifyMgrHandle,
                                    reader->notifyId, filledQueueNo);
                        }
                    }
                    else {
                        frame = (FrameQ_ShMem_Frame)ListMP_getHead((ListMP_Handle)
                                obj->frameList[reader->frameQIndex][filledQueueNo]);

                        GT_assert(curTrace, (NULL != frame));
                        if (frame != NULL) {
                            queue->numFrames[filledQueueNo] -= 1;
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_wbInv((Ptr)queue,
                                        sizeof(FrameQ_ShMem_FrameQueue),
                                        Cache_Type_ALL, TRUE);
                            }

                            /* Invalidate frame header.
                             * Convert portable frame pointers to virtual
                             * pointers.Then do invalidate frame buffers.
                             */
                            if (obj->frmHdrBufCacheFlag == TRUE){
                                status = FrameQBufMgr_invalidateHeaderBuf(
                                        obj->frameQBufMgrHandle, frame);
                            }

                            /* Convert portable address of framebuffers to
                             * virtual address in the retrieved frame.
                             */
                            for (j = 0; j < frame->numFrameBuffers; j++) {
                                /* Address translation to virtual pointers */
                                status =  FrameQBufMgr_translateAddr (
                                        obj->frameQBufMgrHandle,
                                        &bufPtr, FrameQBufMgr_AddrType_Virtual,
                                        (Ptr)frame->frameBufInfo[j].bufPtr,
                                        FrameQBufMgr_AddrType_Portable,
                                        FrameQBufMgr_BUF_FRAMEBUF);

                                GT_assert(curTrace, (NULL != bufPtr));
                                frame->frameBufInfo[j].bufPtr = (UInt32)bufPtr;

                                if ((obj->frmBufCacheFlag[j] == TRUE)
                                        && (obj->bufCpuAccessFlag[j] == TRUE)){
                                    /* Invalidate frame buffers */
                                    status = FrameQBufMgr_invalidateFrameBuf(
                                            obj->frameQBufMgrHandle,
                                            (Ptr)(frame->frameBufInfo[j].bufPtr +
                                            frame->frameBufInfo[j].startOffset),
                                            frame->frameBufInfo[j].validSize,
                                            j);
                                 }
                            }
                            pframe[i] = frame;
                            lNumFrames += 1;
                        }
                    }
                }

                if ((lNumFrames > 0) && (lNumFrames < *numFrames)) {
                    status = FrameQ_E_INSUFFICENT_FRAMES;
                }
            }

            if (((status == FrameQ_E_INSUFFICENT_FRAMES) ||
                    (status == FrameQ_E_EMPTY)) &&
                    ((reader->isRegistered == TRUE) &&
                    (reader->notifyId != (UInt32)-1))) {
                tmpStatus = ClientNotifyMgr_waitOnMultiSubEntryNotify(
                        obj->clientNotifyMgrHandle, reader->notifyId,
                        pNotifySubIds, numSubEntries);
                if (tmpStatus < 0) {
                    status = FrameQ_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "FrameQ_ShMem_getv", status,
                            "ClientNotifyMgr_waitOnMultiSubEntryNotify "
                            "failed!");
                }
            }
        }

        /* Release lock */
        GateMP_leave(obj->gate, key);
    }

    if (lNumFrames == 0) {
        for (i = 0; i < *numFrames; i++) {
            pframe[i] = NULL;
        }
    }

    *numFrames = lNumFrames;

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_getv", status);

    return (status);
}

/*
 * ======== FrameQ_ShMem_dup ========
 * Duplicate the given frame
 */
Int32 FrameQ_ShMem_dup(FrameQ_ShMem_Handle handle, FrameQ_ShMem_Frame frame,
        FrameQ_ShMem_Frame *dupedFrame)
{
    Int32                   status       = FrameQ_S_SUCCESS;
    FrameQBufMgr_Frame      dupedFramePtr[1];
    FrameQ_ShMem_Obj       *obj;

    GT_3trace(curTrace, GT_ENTER, "FrameQ_ShMem_dup", handle, frame,
            dupedFrame);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (NULL != dupedFrame));

        obj = handle->obj;

    /* Allocate frame header for the duplicated frame */

    status = FrameQBufMgr_dup(obj->frameQBufMgrHandle, frame,
            dupedFramePtr, 1);
    /* No Need to check status to assign dupedFramePtr.
     * dupedFramePtr[0] will be NULL if dup fails.
     */
    *dupedFrame = (FrameQ_ShMem_Frame) dupedFramePtr[0];

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_dup", status);

    /*! @retval  FrameQ_S_SUCCESS   Able to get frame from the FrameQ */
    return (status);
}

/*
 *  ======== FrameQ_ShMem_flushAll ========
 *  API to flush frames.
 *
 *  Flushes all the queus. Currently supported for creator.
 */
Int32 FrameQ_ShMem_flushAll(FrameQ_ShMem_Handle handle)
{
    Int32 status = FrameQ_S_SUCCESS;
    UInt32 count = 0;
    UInt32                      numFlushFrames = 0;
    volatile FrameQ_ShMem_FrameQueue     *queue;
    FrameQ_ShMem_Frame           frame;
    Ptr                          bufPtr;
    FrameQ_ShMem_Frame           pframe[FrameQ_MAX_FRAMESINVAPI];
    Int32                        key;
    UInt32                       i;
    UInt16                       j;
    UInt16                       k;
    UInt16                       m;
    Int32                        tmpStatus;
    UInt8                        filledQueueNo;
    FrameQ_ShMem_Obj            *obj;

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != handle->obj));
    obj = (FrameQ_ShMem_Obj *) handle->obj;

    if (obj->objMode == FrameQ_MODE_NONE) {
        /* FrameQ creator  has called the flush.*/
        /* Flush  given queue from each reader */
        /* Acquire  lock */
        key = GateMP_enter ((GateMP_Handle)obj->gate);
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv ((Ptr)obj->attrs,
                       sizeof (FrameQ_ShMem_Attrs),
                       Cache_Type_ALL,
                       TRUE);
        }
        for (k = 0; k < obj->attrs->numQueues; k++) {
            filledQueueNo = k;
            for (j = 0; j < obj->attrs->numReaders; j++) {
                /* Each queu( obj->frmQueue) is for  one reader. loop through
                 * all the queues and free it
                 */
                queue = obj->frmQueue[j];
                if (obj->ctrlStructCacheFlag == TRUE) {
                    Cache_inv ((Ptr)queue,
                               sizeof (FrameQ_ShMem_FrameQueue),
                               Cache_Type_ALL,
                               TRUE);
                }
                do {
                    count = queue->numFrames[filledQueueNo];
                    for (i = 0; i < FrameQ_MAX_FRAMESINVAPI && i < count ; i++) {
                        frame = (FrameQ_ShMem_Frame)ListMP_getHead ((ListMP_Handle)
                                    obj->frameList[j][filledQueueNo]);
                        if (frame != NULL) {
                            queue->numFrames[filledQueueNo] -= 1;
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                Cache_wbInv ((Ptr)queue,
                                             sizeof (FrameQ_ShMem_FrameQueue),
                                             Cache_Type_ALL,
                                             TRUE);
                            }

                            /* Invalidate  frame header.
                             * Conevert portable frame pointers to virtual pointers.
                             * Then do invalidate frame buffers.
                             */
                            if (obj->frmHdrBufCacheFlag == TRUE){
                                status = FrameQBufMgr_invalidateHeaderBuf(
                                                            obj->frameQBufMgrHandle,
                                                            frame);
                            }
                            /* Convert  portable address of framebuffers to  virtual
                             * address in the retrieved frame.
                             */
                             for (m = 0; m < frame->numFrameBuffers; m++) {
                                /* Address translation to virtual pointers */
                                status =  FrameQBufMgr_translateAddr (
                                                    obj->frameQBufMgrHandle,
                                                    &bufPtr,
                                                    FrameQBufMgr_AddrType_Virtual,
                                                    (Ptr)frame->frameBufInfo[m].bufPtr,
                                                    FrameQBufMgr_AddrType_Portable,
                                                    FrameQBufMgr_BUF_FRAMEBUF);

                                GT_assert(curTrace, (NULL != bufPtr));
                                frame->frameBufInfo[m].bufPtr = (UInt32)bufPtr;

                                if (   (obj->frmBufCacheFlag[m] == TRUE)
                                   && (obj->bufCpuAccessFlag[m] == TRUE)) {
                                    /* Invalidate  frame buffers pointed to by this
                                     * frame Using optimum cache coherence operations
                                     */
                                    status = FrameQBufMgr_invalidateFrameBuf(
                                                    obj->frameQBufMgrHandle,
                                                   (Ptr)(frame->frameBufInfo[m].bufPtr +
                                                    frame->frameBufInfo[m].startOffset),
                                                    frame->frameBufInfo[m].validSize,
                                                    m);
                                }
                            }
                            pframe[i]= frame;
                            numFlushFrames ++;
                        }
                    }
                    if (numFlushFrames > 0) {
                        /* Leave the lock and Call FrameQBufMgr_freeV API.*/
                        GateMP_leave ((GateMP_Handle)obj->gate , key);
                        /* Call FrameQBufMgr API to free frame */
                        tmpStatus = FrameQBufMgr_freev(obj->frameQBufMgrHandle,
                                                       (FrameQBufMgr_Frame *)pframe,
                                                       numFlushFrames);
                        GT_assert(curTrace, (tmpStatus >= 0));
                        if (tmpStatus < 0) {
                            status = FrameQ_E_FAIL;
                            GT_setFailureReason (curTrace, GT_4CLASS,
                                    "FrameQ_ShMem_flushAll", status,
                                    "FrameQBufMgr_freev failed to"
                                    "flush the remaining frames!");
                        }

                        key = GateMP_enter((GateMP_Handle)obj->gate);
                    }
                    /* Loop again if writer has put any frames  to
                     * total frames are more than pframe array size
                     */
                    count = queue->numFrames[filledQueueNo];
                } while (count > 0 );
            }
        }
        /* Release lock */
        GateMP_leave ((GateMP_Handle)obj->gate , key);
    }
    return status;
}

/*
 *  ======== FrameQ_ShMem_registerNotifier ========
 *  Register a call back function.
 */
Int32 FrameQ_ShMem_registerNotifier(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_NotifyParams *notifyParams)
{
    Int32                        status       = FrameQ_S_SUCCESS;
    ClientNotifyMgr_registerNoitifyParams  regParams;
    FrameQBufMgr_NotifyParams lnotifyParams;
    Int32                        tmpStatus;
    UInt32                       key;
    FrameQ_ShMem_ReaderClient   *reader;
    FrameQ_ShMem_WriterClient   *writer;
    UInt32                      lNotifyId;
    FrameQ_ShMem_Obj            *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_ShMem_registerNotifier", obj,
            notifyParams);

    GT_assert(curTrace, (NULL != obj));
    GT_assert(curTrace, (notifyParams));

    obj = (FrameQ_ShMem_Obj*)handle->obj;

    switch (obj->objMode) {

        case FrameQ_ShMem_MODE_READER:
        {
            reader = (FrameQ_ShMem_ReaderClient*)obj->virtClientHandle;
            if (obj->ctrlStructCacheFlag == TRUE) {
                key = GateMP_enter(obj->gate);
                Cache_inv (reader,
                           sizeof (FrameQ_ShMem_ReaderClient),
                           Cache_Type_ALL,
                           TRUE);
                GateMP_leave (obj->gate, key);
            }
            regParams.clientHandle   = notifyParams->cbClientHandle;
            regParams.procId         = MultiProc_self();
            regParams.notifyType     = notifyParams->notifyType;
            regParams.fxnPtr         = (ClientNotifyMgr_FnCbck)notifyParams->cbFxnPtr;
            regParams.cbContext      = notifyParams->cbContext;
            regParams.watermarkCond1 = notifyParams->watermark;
            regParams.watermarkCond2 = (UInt32)-1;

            tmpStatus = ClientNotifyMgr_registerClient(obj->clientNotifyMgrHandle,
                    &regParams, (Ptr)&lNotifyId);
            if (tmpStatus < 0) {
                status = FrameQ_E_CLIENTNOTIFYMGRREGCLIENT;
            }
            else {
                reader->notifyId = lNotifyId;
                reader->isRegistered = TRUE;
                if (obj->ctrlStructCacheFlag == TRUE) {
                    key = GateMP_enter (obj->gate);
                    Cache_wbInv (reader,
                                 sizeof (FrameQ_ShMem_ReaderClient),
                                 Cache_Type_ALL,
                                 TRUE);
                    GateMP_leave (obj->gate, key);
                }
           }
        }
        break;

        case FrameQ_ShMem_MODE_WRITER:
        {
            writer = (FrameQ_ShMem_WriterClient *) obj->writer;
            if (obj->ctrlStructCacheFlag == TRUE) {
                key = GateMP_enter (obj->gate);
                Cache_inv (writer,
                           sizeof (FrameQ_ShMem_WriterClient),
                           Cache_Type_ALL,
                           TRUE);
                GateMP_leave (obj->gate, key);
            }

            lnotifyParams.cbClientHandle = notifyParams->cbClientHandle;
            lnotifyParams.notifyType     = (SysLink_NotifyType)notifyParams->notifyType;
            lnotifyParams.cbFxnPtr       = (FrameQBufMgr_NotifyFunc)
                                                         notifyParams->cbFxnPtr;
            lnotifyParams.cbContext      = notifyParams->cbContext;
            lnotifyParams.watermark      = notifyParams->watermark;

            tmpStatus = FrameQBufMgr_registerNotifier(obj->frameQBufMgrHandle,
                    &lnotifyParams);
            if (tmpStatus < 0) {
                status = FrameQ_E_FRAMEQBUFMGRREGCLIENT;
            }
            else {
                writer->isRegistered = TRUE;
                if (obj->ctrlStructCacheFlag == TRUE) {
                    key = GateMP_enter (obj->gate);
                    Cache_wbInv (writer,
                                 sizeof (FrameQ_ShMem_WriterClient),
                                 Cache_Type_ALL,
                                 TRUE);
                    GateMP_leave (obj->gate, key);
                }
           }
        }
        break;

        default:
            /* Invalid mode. Return error */
            status = FrameQ_E_ACCESSDENIED;
        break;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_registerNotifier", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_unregisterNotifier ========
 *  Unregister a call back function
 */
Int32 FrameQ_ShMem_unregisterNotifier(FrameQ_ShMem_Handle handle)
{
    Int32                        status       = FrameQ_S_SUCCESS;
    Int32                        tmpStatus;
    UInt32                       key;
    FrameQ_ShMem_ReaderClient   *reader;
    FrameQ_ShMem_WriterClient   *writer;
    FrameQ_ShMem_Obj             *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_ShMem_unregisterNotifier", handle);

    GT_assert(curTrace, (NULL != handle));

    obj = (FrameQ_ShMem_Obj*)handle->obj;
    switch (obj->objMode) {

        case FrameQ_ShMem_MODE_READER:
        {
            /* For Reader  unregister the call back with the client notify
             * manager.
             */
            reader = (FrameQ_ShMem_ReaderClient*)obj->virtClientHandle;
            if (obj->ctrlStructCacheFlag == TRUE) {
                key = GateMP_enter (obj->gate);
                Cache_inv (reader,
                           sizeof (FrameQ_ShMem_ReaderClient),
                           Cache_Type_ALL,
                           TRUE);
                GateMP_leave (obj->gate, key);
            }
            tmpStatus = ClientNotifyMgr_unregisterClient (
                                                     (ClientNotifyMgr_Handle)obj->clientNotifyMgrHandle,
                                                     reader->notifyId);
            if (tmpStatus < 0) {
                status = FrameQ_E_CLIENTNOTIFYMGRUNREGCLIENT;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "FrameQ_ShMem_registerNotifier", status,
                        "ClientNotifyMgr_unregisterClient failed!");
            }
            else {
                reader->isRegistered = FALSE;
                reader->notifyId     = (UInt32)-1;
                if (obj->ctrlStructCacheFlag == TRUE) {
                    key = GateMP_enter (obj->gate);
                    Cache_wbInv (reader,
                                 sizeof (FrameQ_ShMem_ReaderClient),
                                 Cache_Type_ALL,
                                 TRUE);
                    GateMP_leave (obj->gate, key);
                }
            }
        }
        break;

        case FrameQ_ShMem_MODE_WRITER:
        {
            /* For writer  unregister the call back with the FrameQBufMgr.
             */
            writer = (FrameQ_ShMem_WriterClient *) obj->writer;
            if (obj->ctrlStructCacheFlag == TRUE) {
                key = GateMP_enter (obj->gate);
                Cache_inv (writer,
                           sizeof (FrameQ_ShMem_ReaderClient),
                           Cache_Type_ALL,
                           TRUE);
                GateMP_leave (obj->gate, key);
            }

            tmpStatus = FrameQBufMgr_unregisterNotifier(obj->frameQBufMgrHandle);
            if (tmpStatus < 0) {
                status = FrameQ_E_FRAMEQBUFMGRUNREGCLIENT;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "FrameQ_ShMem_registerNotifier", status,
                        "FrameQBufMgr_unregisterNotifier failed!");
            }
            else {
                writer->isRegistered = FALSE;
                if (obj->ctrlStructCacheFlag == TRUE) {
                    key = GateMP_enter (obj->gate);
                    Cache_wbInv (writer,
                                 sizeof (FrameQ_ShMem_WriterClient),
                                 Cache_Type_ALL,
                                 TRUE);
                    GateMP_leave (obj->gate, key);
                }
           }
        }
        break;

        default:
            /* Invalid mode. Return error */
            status = FrameQ_E_ACCESSDENIED;
        break;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_unregisterNotifier", status);

    return (status);
}

/*
 * ======== FrameQ_ShMem_sendNotify ========
 * Function to send forced notification to the reader clients.
 */
Int32 FrameQ_ShMem_sendNotify(FrameQ_ShMem_Handle handle, UInt16 msg)
{
    Int32 status = FrameQ_S_SUCCESS;
    FrameQ_ShMem_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_sendNotify", handle, msg);

    GT_assert(curTrace, (NULL != handle));

    obj = (FrameQ_ShMem_Obj*)handle->obj;
    switch (obj->objMode) {

        case FrameQ_ShMem_MODE_WRITER:
            /* Send force notification to all the registered Reader clients.
             * status  will be  success if atleast one reader is notified.
             */
            status = ClientNotifyMgr_sendForceNotificationMulti(
                    obj->clientNotifyMgrHandle, (UInt32)-1, msg);
            break;

        default:
            /* Invalid mode. Return error */
            status = FrameQ_E_ACCESSDENIED;
        break;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_sendNotify", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_getNumFrames ========
 *  Get the number of frames in a FrameQ
 */
Int32 FrameQ_ShMem_getNumFrames(FrameQ_ShMem_Handle handle, UInt32 *numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;
    volatile FrameQ_ShMem_Attrs          *pAttrs;
    FrameQ_ShMem_ReaderClient            *reader;
    volatile FrameQ_ShMem_FrameQueue     *queue;
    UInt32                                queueId;
    UInt32                                key;
    UInt8                                 filledQueueNo;
    FrameQ_ShMem_Obj                     *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_getNumFrames", handle,
            numFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != handle->obj));
    GT_assert(curTrace, (NULL != numFrames ));

    obj = (FrameQ_ShMem_Obj*)handle->obj;

    pAttrs = obj->attrs;
    /* This function returns  number of frames available in the fileld queue 0
     */
    filledQueueNo = 0;

    switch (obj->objMode)
    {
        case FrameQ_ShMem_MODE_READER:
        {
             key = GateMP_enter (obj->gate);
             reader  = obj->virtClientHandle;
            /* Return  available frames in the frame queue associated withe this
             * reader  excluding the frames that it has already acquired but not
             * freed.
             */
            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)pAttrs,
                          sizeof(FrameQ_ShMem_Attrs),
                          Cache_Type_ALL,
                          TRUE);
                Cache_inv(reader,
                          sizeof(FrameQ_ShMem_ReaderClient),
                          Cache_Type_ALL,
                          TRUE);
            }
            queueId = reader->frameQIndex;
            queue = obj->frmQueue[queueId];
            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)queue,
                          sizeof(FrameQ_ShMem_FrameQueue),
                          Cache_Type_ALL,
                          TRUE);
            }
            /* Get valid frames  */
            *numFrames = queue->numFrames[filledQueueNo];

            GateMP_leave (obj->gate, key);
        }
        break;

        case FrameQ_ShMem_MODE_WRITER:
        {
            key = GateMP_enter (obj->gate);

            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)pAttrs,
                          sizeof(FrameQ_ShMem_Attrs),
                          Cache_Type_ALL,
                          TRUE);
            }
            queueId = pAttrs->primaryQueueId;
            queue = obj->frmQueue[queueId];
            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)queue,
                          sizeof(FrameQ_ShMem_FrameQueue),
                          Cache_Type_ALL,
                          TRUE);
            }
            /* Get valid frames  */
            *numFrames = queue->numFrames[filledQueueNo];

            GateMP_leave (obj->gate, key);
        }
        break;

        default:
        {
            *numFrames = 0;
            status = FrameQ_E_FAIL;
            GT_setFailureReason (curTrace, GT_4CLASS,
                    "FrameQ_ShMem_getNumFrames", status,
                    "Invalid handle (not reader or writer)!");
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_getNumFrames", status);

    return (status);
}

/*
 *  ======== FrameQ_getvNumFrames ========
 *  Get variable number of frames
 */
Int32 FrameQ_ShMem_getvNumFrames(FrameQ_ShMem_Handle handle,
        UInt32 numFrames[], UInt8 filledQId[], UInt8 numFilledQids)
{
    Int32 status = FrameQ_S_SUCCESS;
    volatile FrameQ_ShMem_Attrs          *pAttrs;
    FrameQ_ShMem_ReaderClient            *reader;
    volatile FrameQ_ShMem_FrameQueue     *queue;
    UInt32      queueId;
    UInt32      i;
    Int32       key;
    FrameQ_ShMem_Obj                     *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_getvNumFrames", handle,
            numFrames);

    GT_assert(curTrace, (NULL != handle));

    GT_assert(curTrace, (NULL != handle->obj));
    GT_assert(curTrace, (NULL != numFrames ));
    GT_assert(curTrace, (NULL != filledQId));

    obj = (FrameQ_ShMem_Obj*)handle->obj;

    pAttrs = obj->attrs;
    /* This function returns  number of frames available in the fileld queue 0
     */
    switch (obj->objMode)
    {
        case FrameQ_ShMem_MODE_READER:
        {
            key = GateMP_enter (obj->gate);
            reader  = obj->virtClientHandle;
            /* Return  available frames in the frame queue associated withe this
             * reader  excluding the frames that it has already acquired but not
             * freed.
             */
            if (obj->ctrlStructCacheFlag  == TRUE) {
                    Cache_inv((Ptr)pAttrs,
                               sizeof(FrameQ_ShMem_Attrs),
                               Cache_Type_ALL,
                               TRUE);
                    Cache_inv(reader,
                              sizeof(FrameQ_ShMem_ReaderClient),
                              Cache_Type_ALL,
                              TRUE);
            }
            queueId = reader->frameQIndex;
            queue = obj->frmQueue[queueId];
            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)queue,
                          sizeof(FrameQ_ShMem_FrameQueue),
                          Cache_Type_ALL,
                          TRUE);
            }
            for (i = 0; i < numFilledQids; i++) {
                if (filledQId[i] > pAttrs->numQueues) {
                    status = FrameQ_E_INVALIDARG;
                }
                else {
                     /* Get valid frames  */
                    numFrames[i] = queue->numFrames[filledQId[i]];
                }

            }
            GateMP_leave (obj->gate, key);
        }
        break;

        case FrameQ_ShMem_MODE_WRITER:
        {
            key = GateMP_enter (obj->gate);

            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)pAttrs,
                          sizeof(FrameQ_ShMem_Attrs),
                          Cache_Type_ALL,
                          TRUE);
            }
            queueId = pAttrs->primaryQueueId;
            queue = obj->frmQueue[queueId];
            if (obj->ctrlStructCacheFlag  == TRUE) {
                Cache_inv((Ptr)queue,
                          sizeof(FrameQ_ShMem_FrameQueue),
                          Cache_Type_ALL,
                          TRUE);
            }
            /* Get valid frames  */
            for (i = 0; i < numFilledQids; i++) {
                if (filledQId[i] > pAttrs->numQueues) {
                    status = FrameQ_E_INVALIDARG;
                }
                else {
                     /* Get valid frames  */
                    numFrames[i] = queue->numFrames[filledQId[i]];
                }
            }
            GateMP_leave (obj->gate, key);
        }
        break;

        default:
        {
            for (i = 0; i < numFilledQids; i++) {
                 /* Get valid frames  */
                numFrames[i] = 0;
            }

            status = FrameQ_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQ_ShMem_getvNumFrames", status,
                    "Invalid handle (not reader or writer)!");
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_getvNumFrames", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_getNumFreeFrames ========
 *  Get the number of free frames available
 */
Int32 FrameQ_ShMem_getNumFreeFrames(FrameQ_ShMem_Handle handle,
        UInt32 *numFreeFrames)
{
    Int32 status = FrameQ_S_SUCCESS;
    Int32 tmpStatus;
    FrameQ_ShMem_Obj            *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_getNumFreeFrames", handle,
            numFreeFrames);

    GT_assert(curTrace, (NULL != handle));

    GT_assert(curTrace, (NULL != handle->obj));
    GT_assert(curTrace, (NULL != numFreeFrames));

    obj = handle->obj;

    GT_assert(curTrace, (NULL != obj->frameQBufMgrHandle));

    tmpStatus = FrameQBufMgr_getNumFreeFrames(obj->frameQBufMgrHandle,
            numFreeFrames);
    if (tmpStatus < 0) {
        status = FrameQ_E_FAIL;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_getNumFreeFrames", status);

    return (status);
}

/*
 *  ======== FrameQ_getvNumFreeFrames ========
 * Get the number of free frames available (for multiple frames)
 */
Int32 FrameQ_ShMem_getvNumFreeFrames(FrameQ_ShMem_Handle handle,
        UInt32 numFreeFrames[], UInt8 freeQId[], UInt8 numFreeQids)
{
    Int32 status = FrameQ_S_SUCCESS;
    Int32 tmpStatus;
    FrameQ_ShMem_Obj                     *obj;

    GT_3trace(curTrace, GT_ENTER, "FrameQ_ShMem_getvNumFreeFrames", handle,
            numFreeFrames, freeQId);

    GT_assert(curTrace, (NULL != handle));

    GT_assert(curTrace, (NULL != handle->obj));
    GT_assert(curTrace, (NULL != numFreeFrames));
    GT_assert(curTrace, (NULL != freeQId));
    obj = handle->obj;

    GT_assert(curTrace, (NULL != obj->frameQBufMgrHandle));

    tmpStatus = FrameQBufMgr_getvNumFreeFrames(obj->frameQBufMgrHandle,
            numFreeFrames, freeQId, numFreeQids);
    if (tmpStatus < 0) {
        status = FrameQ_E_FAIL;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_getvNumFreeFrames", status);

    return (status);
}

/*
 *  ======== FrameQ_ShMem_control ========
 *  Hook to perform implementation dependent operation
 */
Int32 FrameQ_ShMem_control(FrameQ_ShMem_Handle handle, Int32 cmd, Ptr arg)
{
    Int32   status = FrameQ_S_SUCCESS;

    GT_assert(curTrace, (NULL != handle));

    status = FrameQ_E_NOTIMPLEMENTED;

    return (status);
}


/*
 *  ======== FrameQ_ShMem_getNumFrameBuffers ========
 *  API to find out the number of frame buffers in a frame.
 */
UInt32 FrameQ_ShMem_getNumFrameBuffers(FrameQ_ShMem_Frame frame)
{
    UInt32 numbufs = 0;

    GT_assert(curTrace, (NULL != frame));

    numbufs = frame->numFrameBuffers;

    return (numbufs);
}

/*
 *  ======== FrameQ_ShMem_getFrameBuffer ========
 *  API to retrieve framebuffer  identified by frameBufNum from a given frame.
 * Here no validation is performed for frameBufNum. User is expected to pass the
 * right value for it.
 * ( APPs can use getNumFrameBuffers API to know the number of frame buffers
 *  associated with the frame)
 */
Ptr FrameQ_ShMem_getFrameBuffer(FrameQ_ShMem_Frame frame, UInt32 frameBufNum)
{
    Void * frmBuffer = NULL;
    FrameQ_ShMem_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_ShMem_FrameBufInfo *)&(frame->frameBufInfo[0]);

    frmBuffer = (Ptr) frameBufInfo[frameBufNum].bufPtr;

    return (frmBuffer);
}

/*
 *  ======== FrameQ_ShMem_getFrameBufSize ========
 *  API to get the size of the framebuffer.
 */
UInt32 FrameQ_ShMem_getFrameBufSize(FrameQ_ShMem_Frame frame,
        UInt32 frameBufNum)
{
    UInt32 size;
    FrameQ_ShMem_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_ShMem_FrameBufInfo *)&(frame->frameBufInfo[0]);
    size = frameBufInfo[frameBufNum].bufSize;

    return (size);
}


/*
 *  ======== FrameQ_ShMem_getFrameBufValidSize ========
 *  API to get the valid data size of a framebuffer  identified by frameBufNum
 *  in a given frame.
 */
UInt32 FrameQ_ShMem_getFrameBufValidSize(FrameQ_ShMem_Frame frame,
        UInt32 frameBufNum)
{
    UInt32 size;
    FrameQ_ShMem_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_ShMem_FrameBufInfo *)&(frame->frameBufInfo[0]);
    size = frameBufInfo[frameBufNum].validSize;

    return (size);
}

/*
 *  ======== FrameQ_ShMem_getFrameBufDataStartOffset ========
 *  API to get the valid data start offset in framebuffer  identified by
 *  frameBufNum in a given frame.
 */
UInt32 FrameQ_ShMem_getFrameBufDataStartOffset(FrameQ_ShMem_Frame frame,
        UInt32 frameBufNum)
{
    UInt32 offSet;
    FrameQ_ShMem_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_ShMem_FrameBufInfo *)&(frame->frameBufInfo[0]);
    offSet = frameBufInfo[frameBufNum].startOffset;

    return (offSet);
}

/*
 *  ======== FrameQ_ShMem_setFrameBufValidSize ========
 *  API to set the valid data size of a framebuffer  identified by
 *  frameBufNum in  given frame.
 */
Int32 FrameQ_ShMem_setFrameBufValidSize(FrameQ_ShMem_Frame frame,
        UInt32 frameBufNum, UInt32 validDataSize)
{
    Int32 status = FrameQ_S_SUCCESS;
    FrameQ_ShMem_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers > frameBufNum));

    frameBufInfo = (FrameQ_ShMem_FrameBufInfo *)&(frame->frameBufInfo[0]);

    GT_assert(curTrace, (frameBufInfo[frameBufNum].bufSize <
            (validDataSize + frameBufInfo[frameBufNum].startOffset)));

    frameBufInfo[frameBufNum].validSize = validDataSize;

    return (status);
}

/*
 *  ======== FrameQ_ShMem_setFrameBufDataStartOffset ========
 *  API to set the valid data start offset of a framebuffer  identified by
 *  frameBufNum in a given frame.
 */
Int32 FrameQ_ShMem_setFrameBufDataStartOffset(FrameQ_ShMem_Frame frame,
        UInt32 frameBufNum, UInt32 dataStartOffset)
{
    Int32 status = FrameQ_S_SUCCESS;
    FrameQ_ShMem_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers > frameBufNum));

    frameBufInfo = (FrameQ_ShMem_FrameBufInfo *)&(frame->frameBufInfo[0]);

    GT_assert(curTrace, (frameBufInfo[frameBufNum].bufSize <
            (dataStartOffset + frameBufInfo[frameBufNum].validSize)));

    frameBufInfo[frameBufNum].startOffset = dataStartOffset;

    return (status);
}


/*
 * ========================== Internal functions ============================
 */

#if 0
/*
 *  ======== FrameQ_ShMem_Module_startup ========
 */
Int FrameQ_ShMem_Module_startup(Int phase)
{
    Int32 key;

    if (FrameQ_ShMem_module->nameServer != NULL) {
        if (!ti_sdo_utils_NameServer_Module_startupDone()) {
            return (Startup_NOTDONE);
        }
    }

#if 0 // TODO - how do we handle startup in non-XDC
    /* Check the  Front end module setup is done. */
    if (!FrameQ_Module_startupDone()) {
        return (Startup_NOTDONE);
    }
#endif

    key = Gate_enterSystem();

    FrameQ_ShMem_module->refCount++;

    if(FrameQ_ShMem_module->refCount > 1) {
        Gate_leaveSystem(key);
        return (Startup_DONE);
    }
    else {
        /*Open the Name server created by FrameQ front end*/
        if (FrameQ_ShMem_module->nameServer == NULL) {
            /* Open  the Name Server instance  created by Frond End FrameQBufMgr
             * module if not  assigned statically.
             */
//          FrameQ_ShMem_module->nameServer =
//                                  NameServer_getHandle(FRAMEQ_NAMESERVERNAME);
            FrameQ_ShMem_module->nameServer = gFrameQ_nsHandle ;
            if(NULL == FrameQ_ShMem_module->nameServer) {

                /* Decrement the reference count */
                FrameQ_ShMem_module->refCount--;
                Gate_leaveSystem(key);
                return (Startup_NOTDONE);
            }
        }
    }/* if(FrameQ_ShMem_module->refCount > 1) { */

    /* Release lock */
    Gate_leaveSystem(key);

    return (Startup_DONE);
}
#endif

/*
 *  ======== FrameQ_Shmem_updateCacheFlags ========
 *  Internal API to populate the cache flags
 */
Void FrameQ_Shmem_updateCacheFlags(FrameQ_ShMem_Obj *obj,
        UInt32 cpuAccessFlags)
{
    UInt32 i = 0;

    if (FrameQBufMgr_isCacheEnabledForHeaderBuf(obj->frameQBufMgrHandle) == TRUE) {
        obj->frmHdrBufCacheFlag = TRUE;
    }
    else {
        obj->frmHdrBufCacheFlag = FALSE;
    }

    for (i = 0; i < FrameQBufMgr_MAX_FRAMEBUFS ; i++) {
        /* Check and update if user has provided cache  flags for Frame
         * buffers.
         */
        if ( FrameQBufMgr_isCacheEnabledForFrameBuf(obj->frameQBufMgrHandle, i) == TRUE) {
            obj->frmBufCacheFlag[i] = TRUE;
        }
        else {
            obj->frmBufCacheFlag[i] = FALSE;
        }
        /* Check and update if user has provided cpu access flags for Frame
         * buffers.
         */
        if (   cpuAccessFlags
             & (1 << (FrameQ_ShMem_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET +i))) {
            obj->bufCpuAccessFlag[i] = TRUE;
        }
        else {
            obj->bufCpuAccessFlag[i] = FALSE;
        }
    }
}

/*
 * ======== FrameQ_ShMem_readerPutv ========
 * Function to insert multiple frames  back in to the queue of a
 * FrameQ reader client. Internal API.
 */
static inline Int32 FrameQ_ShMem_readerPutv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 filledQueueId[],
        UInt8 numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;
    UInt32 i = 0;
    FrameQ_ShMem_Attrs *pAttrs = NULL;
    FrameQ_ShMem_ReaderClient *reader;
    FrameQ_ShMem_FrameQueue *queue;
    UInt32 key;
    FrameQBufMgr_Handle bufMgrHandle;
    Ptr portablePtr;
    UInt8 filledQueueNo;
    UInt32 k;
    FrameQ_ShMem_Obj *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_ShMem_readerPutv", handle, framePtr);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != framePtr));
    GT_assert (curTrace, (NULL != filledQueueId));

    obj     = (FrameQ_ShMem_Obj*) handle->obj;
    GT_assert (curTrace, (NULL != obj));

    pAttrs = (FrameQ_ShMem_Attrs *) obj->attrs;

    key = GateMP_enter(obj->gate);

    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)pAttrs,
                   sizeof (FrameQ_ShMem_Attrs),
                   Cache_Type_ALL,
                   TRUE);
    }

    reader = obj->virtClientHandle;
    queue = (FrameQ_ShMem_FrameQueue *)obj->frmQueue[reader->frameQIndex];
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv (queue,
                   sizeof (FrameQ_ShMem_FrameQueue),
                   Cache_Type_ALL,
                   TRUE);
    }
    if ((queue->isAllocated == TRUE)) {
        for (i = 0; i < numFrames; i++) {
            /* If client happens to be  reader, then just put the frame at
             * the head of the corresponding reader's queue
             */
            /* Convert all address to sharead region address in
             * frame header.i.e frame buffer pointers
             */
            bufMgrHandle = obj->frameQBufMgrHandle;
            GT_assert(curTrace, (NULL != bufMgrHandle));

            for (k = 0; k < framePtr[i]->numFrameBuffers; k++) {
                if ((obj->frmBufCacheFlag[k] == TRUE) &&
                        (obj->bufCpuAccessFlag[k] == TRUE)) {
                status = FrameQBufMgr_writeBackFrameBuf(bufMgrHandle,
                               (Ptr)
       (framePtr[i]->frameBufInfo[k].bufPtr + framePtr[i]->frameBufInfo[k].startOffset),
                               framePtr[i]->frameBufInfo[k].validSize,
                               k);
                }
                /* Address translation to portable pointers */
                status =  FrameQBufMgr_translateAddr (
                                    bufMgrHandle,
                                    &portablePtr,
                                    FrameQBufMgr_AddrType_Portable,
                                    (Ptr)framePtr[i]->frameBufInfo[k].bufPtr,
                                    FrameQBufMgr_AddrType_Virtual,
                                    FrameQBufMgr_BUF_FRAMEBUF);

                GT_assert(curTrace, (NULL != portablePtr));
                framePtr[i]->frameBufInfo[k].bufPtr = (UInt32)portablePtr;
            }

            if (obj->frmHdrBufCacheFlag == TRUE) {
                status = FrameQBufMgr_writeBackHeaderBuf(bufMgrHandle,
                                                         framePtr[i]);
            }

            filledQueueNo = filledQueueId[i];
            /* Put in to filled queue of the reader */
            ListMP_putHead((ListMP_Handle)obj->frameList[reader->frameQIndex][filledQueueNo],
                           (ListMP_Elem *)framePtr[i]);
            queue->numFrames[filledQueueNo]++;
        }


        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_wbInv (queue,
                         sizeof (FrameQ_ShMem_FrameQueue),
                         Cache_Type_ALL,
                         TRUE);
        }

    }
    else {
        status = FrameQ_E_FAIL;
    }

    /* Release lock*/
    GateMP_leave ((GateMP_Handle)obj->gate, key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_ShMem_readerPutv", status);

    return (status);
}

/*
 * ======== FrameQ_ShMem_writerPutv ========
 * Internal API. Function to insert multiple frames in to the queues of
 * reader client. If multiple readers are active at the same time, it dups and
 * inserts the frames. If  it able to dup successfully, then only it inserts
 * frames other wise it returns failure.
 *
 */
static inline Int32 FrameQ_ShMem_writerPutv(FrameQ_ShMem_Handle handle,
        FrameQ_ShMem_Frame framePtr[], UInt32 filledQueueId[],
        UInt8 numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;
    UInt32                      i            = 0;
    FrameQ_ShMem_Attrs         *pAttrs       = NULL;
    FrameQ_ShMem_Frame          pframe[FrameQ_MAX_FRAMESINVAPI][FrameQ_MAX_INST_READERS]  = { NULL};
    FrameQ_ShMem_Frame          temp         = NULL;
    Int32                       tmpStatus    = FrameQ_S_SUCCESS;
    FrameQ_ShMem_ReaderClient  *reader;
    FrameQ_ShMem_FrameQueue    *queue;
    FrameQBufMgr_Handle         bufMgrHandle;
    Ptr                         portablePtr;
    UInt8                       filledQueueNo;
    UInt32                      k;
    UInt32                      rdrCnt;
    UInt32                      counter;
    UInt32                      key;
    FrameQ_ShMem_Obj           *obj;
    Error_Block                 eb;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_ShMem_writerPutv", handle, framePtr);

    GT_assert(curTrace, (FrameQ_MAX_FRAMESINVAPI >= numFrames));

    obj     = (FrameQ_ShMem_Obj*) handle->obj;
    GT_assert(curTrace, (NULL != obj));

    /* Check if Frames passed are allocated from the FrameQBufMgr that this
     * writer supports.
     */
    for (i = 0; i < numFrames; i ++) {
        if (framePtr[i]->frmAllocaterId != obj->frameQBufMgrId) {
            /* Frame can not be inserted as it is allocated from different FramQ
             * BufMgr
             */
            status =  FrameQ_E_INVALID_FRAMEQBUFMGRID;
        }
    }

    if (status >= 0) {
        Error_init(&eb);

        pAttrs = (FrameQ_ShMem_Attrs *) obj->attrs;

        key = GateMP_enter((GateMP_Handle)obj->gate);

        /* Invalidate to get the updated attrs  */
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv((Ptr)pAttrs, sizeof(FrameQ_ShMem_Attrs), Cache_Type_ALL,
                    TRUE);
        }

        bufMgrHandle = obj->frameQBufMgrHandle;
        GT_assert(curTrace, (NULL != bufMgrHandle));

        if (pAttrs->numActiveReaders > 1) {
            /*More than one reader exist. Need to duplicate frames to insert
             *frames in to queues other than primary queue.
             */
            /* Exclude primary reader(first reader) in dups as frames passed by APPs
             * will be put in to the primary queue.
             */
            /* Allocate frames  by duplicating. Need
             * to allocate as many as there are number of active readers
             * except primary queue reader.
             */

            tmpStatus = FrameQBufMgr_dupv (obj->frameQBufMgrHandle,
                                           (FrameQBufMgr_Frame *)framePtr,
                                           (FrameQBufMgr_Frame **)pframe,
                                           pAttrs->numActiveReaders -1,
                                           numFrames);
            if (tmpStatus < 0) {
                status = tmpStatus;
            }
        }


        if (status >= 0) {
            /* Write back the original frames */
            for (i = 0; i < numFrames; i++) {
                /* Translate the frame buffer address to portable addreess */
                for (k = 0; k < framePtr[i]->numFrameBuffers; k++) {
                    if (   (obj->frmBufCacheFlag[k] == TRUE)
                        && (obj->bufCpuAccessFlag[k] == TRUE)) {
                        status = FrameQBufMgr_writeBackFrameBuf(
                                   bufMgrHandle,
                                   (Ptr)
           (framePtr[i]->frameBufInfo[k].bufPtr + framePtr[i]->frameBufInfo[k].startOffset),
                                   framePtr[i]->frameBufInfo[k].validSize,
                                   k);
                    }
                    /* Address translation to portable pointers */
                    status =  FrameQBufMgr_translateAddr (
                                        bufMgrHandle,
                                        &portablePtr,
                                        FrameQBufMgr_AddrType_Portable,
                                        (Ptr)framePtr[i]->frameBufInfo[k].bufPtr,
                                        (FrameQBufMgr_AddrType)FrameQBufMgr_AddrType_Virtual,
                                        FrameQBufMgr_BUF_FRAMEBUF);

                    GT_assert(curTrace, (NULL != portablePtr));
                    framePtr[i]->frameBufInfo[k].bufPtr = (UInt32)portablePtr;
                }

                if (obj->frmHdrBufCacheFlag == TRUE) {
                    status = FrameQBufMgr_writeBackHeaderBuf(bufMgrHandle,
                                                             framePtr[i]);
                }

                /*Copy the contents of original frame header contents to other
                 * duped frame headers.
                 */
                if (pAttrs->numActiveReaders > 1) {
                    for (rdrCnt = 0;
                         rdrCnt < pAttrs->numActiveReaders -1;
                         rdrCnt++) {
                        memcpy (pframe[i][rdrCnt],
                                framePtr[i], framePtr[i]->headerSize);
                    }
                }

            }
        }

        /* Now insert frames in to readers queues */
        if (status >= 0) {
            for (i = 0; i < numFrames; i++) {
                /* Retrive the filledQueue No */
                filledQueueNo = filledQueueId[i];
                counter = 0;

                 for (  k = 0; k < pAttrs->numReaders; k++) {
                    if( k != pAttrs->primaryQueueId) {
                        queue = (FrameQ_ShMem_FrameQueue *)
                                                     obj->frmQueue[k];
                        if (obj->ctrlStructCacheFlag == TRUE) {
                            Cache_inv(queue,
                                      sizeof (FrameQ_ShMem_FrameQueue),
                                      Cache_Type_ALL,
                                      TRUE);
                        }

                        temp = (FrameQ_ShMem_Frame)pframe[i][k];
                    }
                    else {
                        /* Insert the original frame in to primary queue
                         * irrespective of numActive readers
                         */
                        queue = (FrameQ_ShMem_FrameQueue *)
                                obj->frmQueue[pAttrs->primaryQueueId];
                        if (obj->ctrlStructCacheFlag == TRUE) {
                            Cache_inv(queue,
                                      sizeof (FrameQ_ShMem_FrameQueue),
                                      Cache_Type_ALL,
                                      TRUE);
                        }
                        temp = framePtr[i];

                    }
                    if (   (queue->isAllocated == TRUE)
                        || (k== pAttrs->primaryQueueId)) {
                        /* Allow insertion in to primary queue evene if it
                         * is not allocated to any reader.
                         */
                        counter++;
                        /* Increment the frame count of filled queue 0 of
                         * reader.
                         */
                        queue->numFrames[filledQueueNo]++;
                        /*TODO: what about frame header pointer.ListMP is
                         * assuming shared region for list elements.
                         * if header buffers are  not from shared region it
                         * will give problem.
                         */
                        ListMP_putTail ((ListMP_Handle)obj->frameList[k][filledQueueNo] ,
                                        (ListMP_Elem *)temp);
                        if (obj->ctrlStructCacheFlag == TRUE) {
                            Cache_wbInv(queue,
                                        sizeof (FrameQ_ShMem_FrameQueue),
                                        Cache_Type_ALL,
                                        TRUE);
                        }
                        /* Check if queue is allocated  to reader then only do
                         * invalidation of reader to send notifications. There
                         * may be a case where reader is not opened yet hence
                         * readerIndex may be -1.
                         */
                        if (queue->isAllocated == TRUE) {
                            /* if queue is allocated queue->readerIndex is a valid
                             * one
                             */
                            /* Send Notification to the Reader if it
                             * is registered for notification
                             */
                            reader = (FrameQ_ShMem_ReaderClient*)
                                    obj->reader[queue->readerIndex];
                            if (obj->ctrlStructCacheFlag == TRUE) {
                                 Cache_inv(reader,
                                           sizeof (FrameQ_ShMem_ReaderClient),
                                           Cache_Type_ALL,
                                           TRUE);
                            }
                            if (reader->isRegistered == TRUE) {
                                /* Release lock */
                                GateMP_leave ((GateMP_Handle)obj->gate, key);
                                status =
                                    ClientNotifyMgr_sendNotification (
                                                     (ClientNotifyMgr_Handle)obj->clientNotifyMgrHandle,
                                                     reader->notifyId,
                                                     filledQueueNo,
                                                     queue->numFrames[filledQueueNo],
                                                     (UInt32)-1);

                                key = GateMP_enter ((GateMP_Handle)obj->gate);

                            }
                        }
                    }

                    if (counter == pAttrs->numActiveReaders) {
                        break;
                    }
                 }
             }
        }

        /* Release lock */
        GateMP_leave ((GateMP_Handle)obj->gate, key);
    }
    return (status);
}

/*
 *  ======== _FrameQ_ShMem_getCliNotifyMgrHandle ========
 *  Get the client handle for a given FrameQ instance
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrHandle(FrameQ_ShMem_Handle handle)
{
    GT_1trace(curTrace, GT_ENTER, "_FrameQ_ShMem_getCliNotifyMgrHandle",
            handle);

    GT_assert(curTrace, ((NULL != handle) && (NULL != handle->obj)));

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_getCliNotifyMgrHandle",
               handle->obj->clientNotifyMgrHandle );

    return (handle->obj->clientNotifyMgrHandle);
}

/*
 *  ======== _FrameQ_ShMem_getCliNotifyMgrGate ========
 *  Get the gate used in client notifyMgr for a given FrameQ instance
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrGate (FrameQ_ShMem_Handle handle)
{
    GT_1trace (curTrace, GT_ENTER, "_FrameQ_ShMem_getCliNotifyMgrGate",
            handle);

    GT_assert(curTrace, ((NULL != handle) && (NULL != handle->obj)));

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_getCliNotifyMgrGate",
            handle->obj->cliNotifyMgrGate );

    return (handle->obj->cliNotifyMgrGate);
}

/*
 *  ======== _FrameQ_ShMem_getCliNotifyMgrShAddr ========
 *  Get the sharedMemBaseAddress of the clientNotifyMgr instance
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrShAddr (FrameQ_ShMem_Handle handle)
{
    Ptr shAddr;

    GT_1trace(curTrace, GT_ENTER, "_FrameQ_ShMem_getCliNotifyMgrShAddr",
            handle);

    GT_assert(curTrace, ((NULL != handle) && (NULL != handle->obj)));

    shAddr = (Ptr)(handle->obj->cliNotifyMgrShAddr);
    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_getCliNotifyMgrShAddr",
            shAddr);

    return (shAddr);
}

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrGateShAddr (FrameQ_ShMem_Handle handle)
{
    Ptr shAddr;

    GT_1trace(curTrace, GT_ENTER, "_FrameQ_ShMem_getCliNotifyMgrGateShAddr",
            handle);

    GT_assert(curTrace, ((NULL != handle) && (NULL != handle->obj)));

    shAddr = (Ptr)(handle->obj->cliNotifyMgrGateShAddr);

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_getCliNotifyMgrGateShAddr",
            shAddr);

    return (shAddr);
}

/*!
 *  @brief     Function to set the notifyid received in userspace during call to
 *             ClientNotifyMgr_register client.
 *  @param     handle  Instance handle.
 *  @param     notifyId  Id to to be set in the object.
 *
 */
Int32
 _FrameQ_ShMem_setNotifyId (FrameQ_ShMem_Handle handle, UInt32 notifyId)
{
    Int32                         status  = FrameQ_S_SUCCESS;
    FrameQ_ShMem_Obj             *obj;
    FrameQ_ShMem_ReaderClient    *reader;
    IArg                          key;

    GT_2trace (curTrace,
               GT_ENTER,
              "_FrameQ_ShMem_setNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace,
               (NULL != handle));

    obj = (FrameQ_ShMem_Obj*)handle->obj;

    if (obj->objMode != FrameQ_ShMem_MODE_READER) {
        status = FrameQ_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQ_ShMem_setNotifyId",
                             status,
                             "Api is allowed for reader client.");
    }
    else {
        key = GateMP_enter (obj->gate);
        reader  = obj->virtClientHandle;
        if (obj->ctrlStructCacheFlag  == TRUE) {
            Cache_inv(reader,
                      sizeof(FrameQ_ShMem_ReaderClient),
                      Cache_Type_ALL,
                      TRUE);
        }

        reader->notifyId      = notifyId;
        reader->isRegistered  = TRUE;

        if (obj->ctrlStructCacheFlag  == TRUE) {
            Cache_wbInv(reader,
                        sizeof(FrameQ_ShMem_ReaderClient),
                        Cache_Type_ALL,
                        TRUE);
        }

        GateMP_leave (obj->gate, key);
    }

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_setNotifyId", status);

    return (status);
}

/*!
 *  @brief     Function to reset the notifyid received in userspace during call to
 *             ClientNotifyMgr_register client.
 *  @param     handle  Instance handle.
 *  @param     reset value to set in the object.
 *
 */
Int32
 _FrameQ_ShMem_resetNotifyId (FrameQ_ShMem_Handle handle, UInt32 notifyId)
{
    Int32                   status       = FrameQ_S_SUCCESS;
    FrameQ_ShMem_Obj             *obj;
    FrameQ_ShMem_ReaderClient    *reader;
    IArg                    key;

    GT_2trace (curTrace,
               GT_ENTER,
              "_FrameQ_ShMem_resetNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace,
               (NULL != handle));

    obj = (FrameQ_ShMem_Obj*)handle->obj;

    if (obj->objMode != FrameQ_ShMem_MODE_READER) {
        status = FrameQ_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQ_ShMem_resetNotifyId",
                             status,
                             "Api is allowed only for reader client.");
    }
    else {
         key = GateMP_enter (obj->gate);
         reader  = obj->virtClientHandle;
         if (obj->ctrlStructCacheFlag  == TRUE) {
             Cache_inv(reader,
                       sizeof(FrameQ_ShMem_ReaderClient),
                       Cache_Type_ALL,
                       TRUE);
         }

         reader->notifyId      = notifyId;
         reader->isRegistered  = FALSE;

         if (obj->ctrlStructCacheFlag  == TRUE) {
             Cache_wbInv(reader,
                         sizeof(FrameQ_ShMem_ReaderClient),
                         Cache_Type_ALL,
                         TRUE);
         }

         GateMP_leave (obj->gate, key);
    }

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_ShMem_resetNotifyId", status);

    return (status);
}
