/*
 *  @file   FrameQBufMgr_ShMem.c
 *
 *  @brief  Implementation of FrameQBufMgr ShMem module on RTOS
 *          side(DSP/BIOS).
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



/*-------------------------    XDC  specific includes ------------------------*/
#include <xdc/std.h>
#include <ti/syslink/utils/Trace.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <ti/sysbios/gates/GateSwi.h>

#include <xdc/runtime/IHeap.h>
#include <ti/sdo/utils/List.h>

/*-------------------------    Generic includes  -----------------------------*/
#include <string.h>

/*-------------------------    BIOS includes    ------------------------------*/
#include <ti/sysbios/gates/GateMutex.h>

/*-------------------------    IPC module specific includes ------------------*/
#include <ti/sdo/ipc/Ipc.h>

#include <ti/syslink/inc/SyslinkMemMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>

#ifdef xdc_target__isaCompatible_64P
#include <ti/sysbios/family/c64p/Cache.h>
#else
#include <ti/sysbios/hal/Cache.h>
#endif

#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/NameServer.h>
#define ti_sdo_ipc_GateMP__nolocalnames
#include <ti/sdo/ipc/GateMP.h>
#undef ti_sdo_ipc_GateMP__nolocalnames
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/HeapMemMP.h>

#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr.h>

#include <ti/syslink/ipc/_FrameQBufMgr_ShMem.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>

#include <ti/syslink/utils/SyslinkMemMgr.h>
#include <ti/syslink/inc/SharedMemoryMgr.h>
#include <ti/syslink/inc/TilerMemoryMgr.h>

#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

#define FRAMEQBUFMGR_NAMESERVERNAME          "FrameQBufMgr"

/*TODO: Move these defines to xdc interface file */
#define FRAMEQBUFMGR_INSTNO_BITS             8u
#define FRAMEQBUFMGR_INSTNO_MASK             0xFF
#define FRAMEQBUFMGR_INSTNO_BITOFFSET        0u
#define FRAMEQBUFMGR_CREATORPROCID_BITS      8u
#define FRAMEQBUFMGR_CREATORPROCID_MASK      0xFF
#define FRAMEQBUFMGR_CREATORPROCID_BITOFFSET 8u


extern Ptr gFrameQBufMgr_nsHandle;

/* ============================================================================
 * Forward declarations of internal Functions
 * ============================================================================
 */
static inline
Int32
_FrameQBufMgr_ShMem_create(FrameQBufMgr_ShMem_Handle handle,
        const FrameQBufMgr_ShMem_Params *params, Ptr arg);
static inline
Int32
_FrameQBufMgr_ShMem_open(FrameQBufMgr_ShMem_Handle handle,
        const FrameQBufMgr_ShMem_Params *params, Ptr arg);

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr
_FrameQBufMgr_ShMem_getCliNotifyMgrShAddr (FrameQBufMgr_ShMem_Handle handle);

Ptr
_FrameQBufMgr_ShMem_getCliNotifyMgrGateShAddr (FrameQBufMgr_ShMem_Handle handle);

Int32
 _FrameQBufMgr_ShMem_setNotifyId (FrameQBufMgr_ShMem_Handle handle,
                                  UInt32 notifyId);

Int32
 _FrameQBufMgr_ShMem_resetNotifyId (FrameQBufMgr_ShMem_Handle handle,
                                    UInt32 notifyId);
/*
 *  ======== _FrameQBufMgr_ShMem_cleanupCreatedInstance ========
 * Function to cleanup the instance if create failed.
 */
static inline  void
_FrameQBufMgr_ShMem_cleanupCreatedInstance(FrameQBufMgr_ShMem_Handle  handle);
/*
 *  ======== FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf ========
 * Function to find out if cache is enabled for header buffers.
 */
Bool FrameQBufMgr_ShMem_isCacheEnabledForHeaderBuf(FrameQBufMgr_ShMem_Handle handle);
/*
 *  ======== FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf ========
 * Function to find out if cache is enabled for frame buffers.
 */
Bool FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf(FrameQBufMgr_ShMem_Handle handle,
                                                  UInt8 frameBufIndex);
Int32
FrameQBufMgr_ShMem_getNumFreeFrames (FrameQBufMgr_ShMem_Handle handle,
                                     UInt32                    * numFreeFrames);
Int32
FrameQBufMgr_ShMem_getvNumFreeFrames (
                                FrameQBufMgr_ShMem_Handle   handle,
                                UInt32                        numFreeFrames[],
                                UInt8                         freeFramePoolNo[],
                                UInt8                         numFreeFramePools);

/*
 * Interface functions.Structure defining Interface functions for FrameQBufMgr.
 */
FrameQBufMgr_Fxns FrameQBufMgr_ShMem_fxns = {
    (fqbm_create)FrameQBufMgr_ShMem_create,
    (fqbm_delete)FrameQBufMgr_ShMem_delete,
    (fqbm_open)FrameQBufMgr_ShMem_open,
    NULL,//FrameQBufMgr_ShMem_openByAddress
    (fqbm_close)FrameQBufMgr_ShMem_close,
    (fqbm_alloc)FrameQBufMgr_ShMem_alloc,
    (fqbm_allocv)FrameQBufMgr_ShMem_allocv,
    (fqbm_free)FrameQBufMgr_ShMem_free,
    (fqbm_freev)FrameQBufMgr_ShMem_freev,
    (fqbm_add)FrameQBufMgr_ShMem_add,
    (fqbm_remove)FrameQBufMgr_ShMem_remove,
    (fqbm_dup)FrameQBufMgr_ShMem_dup,
    (fqbm_dupv)FrameQBufMgr_ShMem_dupv,
    (fqbm_registerNotifier)FrameQBufMgr_ShMem_registerNotifier,
    (fqbm_unregisterNotifier)FrameQBufMgr_ShMem_unregisterNotifier,
    (fqbm_writeBack)FrameQBufMgr_ShMem_writeBack,
    (fqbm_invalidate)FrameQBufMgr_ShMem_invalidate,
    (fqbm_writeBackHeaderBuf)FrameQBufMgr_ShMem_writeBackHeaderBuf,
    (fqbm_invalidateHeaderBuf)FrameQBufMgr_ShMem_invalidateHeaderBuf,
    (fqbm_writeBackFrameBuf)FrameQBufMgr_ShMem_writeBackFrameBuf,
    (fqbm_invalidateFrameBuf)FrameQBufMgr_ShMem_invalidateFrameBuf,
    (fqbm_translateAddr)FrameQBufMgr_ShMem_translateAddr,
    (fqbm_getId)FrameQBufMgr_ShMem_getId,
    (fqbm_control)FrameQBufMgr_ShMem_control,
    (fqbm_getCliNotifyMgrShAddr) _FrameQBufMgr_ShMem_getCliNotifyMgrShAddr,
    (fqbm_getCliNotifyMgrGateShAddr) _FrameQBufMgr_ShMem_getCliNotifyMgrGateShAddr,
    (fqbm_setNotifyId) _FrameQBufMgr_ShMem_setNotifyId,
    (fqbm_resetNotifyId) _FrameQBufMgr_ShMem_resetNotifyId,
    (fqbm_isCacheEnabledForHeaderBuf)FrameQBufMgr_ShMem_isCacheEnabledForHeaderBuf,
    (fqbm_isCacheEnabledForFrameBuf)FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf,
    (fqbm_getNumFreeFrames)FrameQBufMgr_ShMem_getNumFreeFrames,
    (fqbm_getvNumFreeFrames)FrameQBufMgr_ShMem_getvNumFreeFrames,
};

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*
 * @brief Structure for FrameQBufMgr module state
 */
typedef struct FrameQBufMgr_ShMem_ModuleObject_Tag {
    UInt32              refCount;
    /* TBD : can not be atomic */
    NameServer_Handle           nameServer;
    /*!< Handle to the local NameServer used for stoting FrameQBufMgrBufMgr instance
     *   information.
     */
    UInt32                      nameServerType;
    /*!< Name server create type. Dynamic or static*/
    IGateProvider_Handle            gate;
    /*!< Handle for module wide protection*/
    UInt32                      gateType;
    /*!< Flag denotes type of gate */
    Ptr                         defaultInstGate;
    /*!< Holds default instance gate if user sets it */
    List_Handle                 objList;
    /* List holding created objects */
    IGateProvider_Handle        listLock;
    FrameQBufMgr_Config         cfg;
    /*!<  Current module wide config values*/
    FrameQBufMgr_Config         defaultCfg;
    /*!< Default config values*/
    FrameQBufMgr_ShMem_Params   defaultInstParams;
    /*!< Default instance creation parameters */
     Bool isInstNoValid[ClientNotifyMgr_maxInstances];
    /*  Create call will check this flags and sets first invalid index to valid
     *  and  assigns this index to the first byte of obj->Id
     */
} FrameQBufMgr_ShMem_ModuleObject;

/*
 * @brief Structure defining internal object for the FrameQBufMgrBufMgr
 */
typedef struct FrameQBufMgr_ShMem_Obj_Tag {
    List_Elem                    listElem;
    /*!< Used for creating a linked list */
    Ptr                     nsKey;
     /*!< NameServer key */
    UInt32                  regionId;
    /*!< Region id of the shared region. Used for instance control structure */

    SizeT                   allocSize;
    /*!< Shared memory allocated   from regionId*/
    UInt32                  hdrBuf_regionId;
    /*!< Region id of the shared region. Used for Header buffers */
    Ptr                     hdrBufPtr;
    /*!< Pointer to hold the header buffer chunk  */

    SizeT                   hdrBufAllocSize;
    /*!<  Size of the memory allocate for  header buffers*/

    UInt32                  frmBuf_regionId;
    /*!< Region id of the shared region. Used for frame buffers */
    Ptr                     frmBufPtr;
    /*!< Pointer to hold the frame buffer chunk  */

    SizeT                   frmBufAllocSize;
    /*!<  Size of the memory allocate for  frame buffers*/

    GateMP_Handle               gate ;
    /*!< Instance gate */

    Bool                         creGate;
    /*!< Denotes if gate is created internally or not */

    UInt32                       gateMPAllocSize;
    /*! Size of  the memory allocated for instance gate if gate is created
     * internally.
     */
    Ptr                          instGateSharedAddr;
    /*!Shared address of the clientNotifyMgr  gate in knl virtual foramat */

    UInt32                       instId;
    /*!< Id of the instance*/

    UInt32                       notifyId;
    /*!<  Location to  store the key returned by the
     *    ClientNotiftmgr_registerClient call
     */
    UInt32                       objType;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32                       ctrlInterfaceType;
    /* Type of interface for the shared control structures */

    UInt32                       headerInterfaceType;
    /* Type of interface for the frame headers */
    UInt32                       bufInterfaceType;
    /* Type of interface for the frame buffers*/
    UInt32                       minAlign;
    /* Alignment of control structure */
    Ptr                          pnameserverEntry;
    /*!Pointer to the nameserver entry located at the start of the instance
     * shared  address
     */
    volatile   FrameQBufMgr_ShMem_Attrs             *attrs;
    /* Pointer to the shared attrs */
    volatile   FrameQBufMgr_ShMem_FreeFramePoolObj  **freeFramePool;
    /* Arrey of  pointers to the free frame pools*/

    UInt32                       cliGateMPShAddrSize;
    /*!< Size of the ClientNotifyMgr gate  if created internally */

    ClientNotifyMgr_Handle       clientNotifyMgrHandle;
    /*!< Handle to the client  notify mgr instance */

    Ptr                          clientNotifyMgrGate;
    /*!< Handle to the client  notify mgr instance */
    UInt32                       refCount;
    /*!< */
    UInt32                       memMgrType_HdrBufs;
    /*!< Type of the memmgr for header bufs*/
    Ptr                          memMgrAllocater_hdrBufs;
    /*!< Handle to the SyslinkMemMgr for header bufs */

    IHeap_Handle                 heapMemMPHandle_HdrBufs;
    /*!< Handle of heapMemMP from which header buffers gets allocated */

    UInt32                       memMgrType_FrmBufs;
    /*!< Type of the memmgr for frame bufs*/
    Ptr                          memMgrAllocater_frmBufs;
    /*!< Handle to the SyslinkMemMgr for frame bufs */

    IHeap_Handle                 heapMemMPHandle_FrmBufs;
    /*!< Handle of heapMemMP from which frame buffers gets allocated */

    UInt8                        name[FrameQBufMgr_ShMem_MAXNAMELEN];
    /* Name of the instance */

    Bool                         ctrlStructCacheFlag ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    instance control structures
     */
    Bool                         frmHdrBufCacheFlag ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    frame header buffers.
     */
    Bool                         frmBufCacheFlag[8] ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    frame buffers
     */
    Bool                         bufCpuAccessFlag[8];
    /*!<  Flag indicating whether APP uses CPU to access these frame buffers.
     */

    Bool                         isRegistered;
    /*!< Indicates if client has registered a call back function */

    UInt32                       notifyRegId;
    /*!<  Id returned by the registerClient API of ClientNotifyMgr*/
    Ptr                          top;
    /* Pointer to the top Object */

    Ptr cliNotifyMgrShAddr;
    /*!Shared address of the clientNotify Mgr in knl virtual foramat */
    Ptr cliNotifyMgrGateShAddr;
    /*!Shared address of the clientNotifyMgr  gate in knl virtual foramat */
    FrameQBufMgr_ShMem_Params    params;
    /*!< Instance  config params */
    Bool creCliGate;
    /*!< Denotes if gate is created internally or not for clientNotifyMgr */
} FrameQBufMgr_ShMem_Obj;

/*
 * @brief Structure defining object for the FrameQBufMgr.
 */
typedef struct FrameQBufMgr_ShMem_Object_tag {
    FrameQBufMgr_Fxns  *fxns;
    /*!< Pointer to the function table interface that this instance supports*/
    FrameQBufMgr_ShMem_Obj *obj;
    /* Pointer to the internal object of type FrameQBufMgr_ShMem_Obj*/
}FrameQBufMgr_ShMem_Object;



static inline
Int32
_FrameQBufMgr_ShMem_openHdrBufMemMgr(
                                FrameQBufMgr_ShMem_Obj *obj,
                                FrameQBufMgr_NameServerEntry *pnameserverEntry);

static inline
Int32
_FrameQBufMgr_ShMem_openFrmBufMemMgr(
                                FrameQBufMgr_ShMem_Obj *obj,
                                FrameQBufMgr_NameServerEntry *pnameserverEntry);

Void FrameQBufMgr_Shmem_updateCacheFlags(FrameQBufMgr_ShMem_Obj *obj,
                                         UInt32 cacheFlags);

/*=============================================================================
 * Globals
 *=============================================================================
 */
/*!
 *  @var    FrameQBufMgr_ShMem_module_obj
 *
 *  @brief  FrameQBufMgr_ShMem_module state variable.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
FrameQBufMgr_ShMem_ModuleObject FrameQBufMgr_ShMem_module_obj = {
    .nameServer = NULL,
    .defaultCfg.eventNo = FrameQBufMgr_NOTIFY_RESERVED_EVENTNO,
    .defaultCfg.usedefaultgate = FALSE,
    .defaultInstParams.commonCreateParams.size
                                      = sizeof(FrameQBufMgr_ShMem_Params),
    .defaultInstParams.commonCreateParams.ctrlInterfaceType
                                      = FrameQBufMgr_INTERFACE_SHAREDMEM,
    .defaultInstParams.headerInterfaceType = FrameQBufMgr_HDRINTERFACE_SHAREDMEM,
    .defaultInstParams.bufInterfaceType = FrameQBufMgr_BUFINTERFACE_SHAREDMEM,
    .defaultInstParams.commonCreateParams.name = NULL,
    .defaultInstParams.commonCreateParams.openFlag = FALSE,
    .defaultInstParams.gate = NULL,
    .defaultInstParams.localProtect = GateMP_LocalProtect_INTERRUPT,
    .defaultInstParams.remoteProtect = GateMP_RemoteProtect_SYSTEM,
    .defaultInstParams.sharedAddr = NULL,
    .defaultInstParams.sharedAddrSize = 0,
    .defaultInstParams.sharedAddrHdrBuf = NULL,
    .defaultInstParams.sharedAddrHdrBufSize = 0,
    .defaultInstParams.sharedAddrFrmBuf = NULL,
    .defaultInstParams.sharedAddrFrmBufSize = 0,
    .defaultInstParams.numFreeFramePools = 0,
    .defaultInstParams.cpuAccessFlags = 0,
    .defaultInstParams.regionId = 0,
    .defaultInstParams.hdrBuf_regionId = 0,
    .defaultInstParams.frmBuf_regionId = 0,
};


/*!
 *  @var    FrameQBufMgr_ShMem_module
 *
 *  @brief  Pointer to FrameQBufMgr_ShMem_module_obj .
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
FrameQBufMgr_ShMem_ModuleObject* FrameQBufMgr_ShMem_module =
                                                 &FrameQBufMgr_ShMem_module_obj;

/*=============================================================================
 * Forward declarations of internal Functions
 *=============================================================================
 */
/* Post initialization function in instance create. */
static inline
Int32
FrameQBufMgr_ShMem_postInit (FrameQBufMgr_ShMem_Obj *     obj,
                              const FrameQBufMgr_ShMem_Params *  params,
                              Ptr arg);

/*=============================================================================
 * APIs
 *=============================================================================
 */
/*
 *  ======== FrameQBufMgr_ShMem_getConfig ========
 *  API to get the module wide config params.
 */
Int32 FrameQBufMgr_ShMem_getConfig(FrameQBufMgr_ShMem_Config *cfg)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;

    GT_0trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_getConfig");

    GT_assert(curTrace, (cfg != NULL));

    if (NULL != cfg) {
        /* (If setup has not yet been called) */
        if (FrameQBufMgr_ShMem_module->refCount == 0) {

            /* Return the default static config values */
            memcpy(cfg, &FrameQBufMgr_ShMem_module->defaultCfg,
                    sizeof(FrameQBufMgr_ShMem_Config));
            /*
             * cfg->eventNo        = FrameQBufMgr_ShMem_NOTIFY_RESERVED_EVENTNO;
             * cfg->usedefaultgate = FALSE;
             * cfg->maxInstances   = FrameQBufMgr_MAXINSTANCES;
             */
        }
        else {
            /* Return the currently configured parameters */
            memcpy(cfg, &FrameQBufMgr_ShMem_module->cfg,
                    sizeof(FrameQBufMgr_ShMem_Config));
        }
    }
    else {
        status = FrameQBufMgr_E_INVALIDARG;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_getConfig", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_setup ========
 *  Setup the FrameQBufMgr_ShMem module
 */
Int32 FrameQBufMgr_ShMem_setup(FrameQBufMgr_Config *cfg)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    List_Params listParams;
    IArg key;
    Error_Block eb;
    FrameQBufMgr_Config tmpCfg;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_setup", cfg);
    Error_init(&eb);

    key = Gate_enterSystem();
    FrameQBufMgr_ShMem_module->refCount++;
    if (FrameQBufMgr_ShMem_module->refCount > 1) {
        status = FrameQBufMgr_S_ALREADYSETUP;
        Gate_leaveSystem(key);
    }
    else {
        Gate_leaveSystem(key);

        if (cfg == NULL) {
            FrameQBufMgr_ShMem_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }

        FrameQBufMgr_ShMem_module->defaultInstGate = NULL;
        /* Create a locate gate */
        FrameQBufMgr_ShMem_module->gate = (IGateProvider_Handle)
                GateMutex_create((GateMutex_Params*)NULL, &eb);

        if (FrameQBufMgr_ShMem_module->gate == NULL) {
            status = FrameQBufMgr_E_CREATE_GATEMP;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQBufMgr_ShMem_setup", status,
                    "GateMutex_create()Failed to create module gate!");
        }
        else {
           /* Check the params required to create the name server instance */
            if (FrameQBufMgr_ShMem_module->nameServer == NULL) {
//                FrameQBufMgr_ShMem_module->nameServer =
//                                                    NameServer_getHandle(name);
                FrameQBufMgr_ShMem_module->nameServer = gFrameQBufMgr_nsHandle;

                if (FrameQBufMgr_ShMem_module->nameServer == NULL) {
                    status = FrameQBufMgr_E_OPEN_NAMESERVER;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "FrameQBufMgr_setup", status,
                            "Failed to create NameServer!");
                    /* Decrement the reference count */
//TODO                    Atomic_dec_return (&FrameQBufMgr_ShMem_module->refCount);
                }
                else {
                    /* Set the Nameserver create type */
                    FrameQBufMgr_ShMem_module->nameServerType =
                            FrameQBufMgr_DYNAMIC_CREATE;
                }
            }
        }

        if (status >= 0) {
            /* Construct the list object */
            List_Params_init(&listParams);
            FrameQBufMgr_ShMem_module->objList = List_create(&listParams, &eb);
            GT_assert(curTrace, (NULL != FrameQBufMgr_ShMem_module->objList));

            /* Copy the cfg */
            memcpy((Ptr)&FrameQBufMgr_ShMem_module->cfg, (Ptr)cfg,
                    sizeof(FrameQBufMgr_ShMem_Config));
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_setup", status);

    return (status);
}


/*
 *  ======== FrameQBufMgr_ShMem_destroy ========
 *  Destroy (finalize) the FrameQBufMgr_ShMem module.
 */
Int32 FrameQBufMgr_ShMem_destroy()
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    IArg key;

    GT_0trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_destroy");

    key = Gate_enterSystem();
    FrameQBufMgr_ShMem_module->refCount--;
    if (FrameQBufMgr_ShMem_module->refCount == 0) {
        Gate_leaveSystem(key);
        FrameQBufMgr_ShMem_module->nameServer = NULL;
        FrameQBufMgr_ShMem_module->defaultInstGate = NULL;

        /* Destroy the list object */
        List_delete(&FrameQBufMgr_ShMem_module->objList);

        if (FrameQBufMgr_ShMem_module->gate != NULL) {
            /* Delete the module gate. */
            GateMutex_delete(
                    (GateMutex_Handle *)&FrameQBufMgr_ShMem_module->gate);
        }
        /* Clear cfg area */
        memset((Ptr)&FrameQBufMgr_ShMem_module->cfg, 0,
                sizeof(FrameQBufMgr_ShMem_Config));
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_destroy", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_sharedMemReq ========
 *  Find shared memory requirements for the instance
 */
UInt32 FrameQBufMgr_ShMem_sharedMemReq(const FrameQBufMgr_ShMem_Params *params,
        UInt32 *frmHdrBufChunkSize, UInt32 *frmBufChunkSize)
{
    UInt32                  totalSize        = 0;
    UInt32                  bufSize          = 0;
    UInt32                  frmBufSizesInFrm = 0;
    ClientNotifyMgr_Params  clientMgrParams;
    GateMP_Params           gateParams;
    UInt32                  i;
    UInt32                  j;
    SizeT                   minAlign;
    SizeT                   hdrBufMinAlign;
    SizeT                   frmBufMinAlign;
    UInt16                  regionId;
    UInt16                  hdrBuf_regionId;
    UInt16                  frmBuf_regionId;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_sharedMemReq", params);

    GT_assert(curTrace, (params != NULL));

    if (NULL != params) {
        GT_assert(curTrace,
                (params->numFreeFramePools <= FrameQBufMgr_ShMem_MAX_POOLS));

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

        /* Header buffers */
        if (params->sharedAddrHdrBuf) {
            hdrBuf_regionId = SharedRegion_getId(params->sharedAddrHdrBuf);
        }
        else {
            hdrBuf_regionId = params->hdrBuf_regionId;
        }
        GT_assert(curTrace, (hdrBuf_regionId != SharedRegion_INVALIDREGIONID));

        hdrBufMinAlign = Memory_getMaxDefaultTypeAlign();
        if (SharedRegion_getCacheLineSize(hdrBuf_regionId) > hdrBufMinAlign) {
            hdrBufMinAlign = SharedRegion_getCacheLineSize(hdrBuf_regionId);
        }

        /* FrameQ buffers */
        if (params->sharedAddrFrmBuf) {
            frmBuf_regionId = SharedRegion_getId(params->sharedAddrFrmBuf);
        }
        else {
            frmBuf_regionId = params->frmBuf_regionId;
        }
        GT_assert(curTrace, (frmBuf_regionId != SharedRegion_INVALIDREGIONID));

        frmBufMinAlign = Memory_getMaxDefaultTypeAlign();
        if (SharedRegion_getCacheLineSize(frmBuf_regionId) > frmBufMinAlign) {
            frmBufMinAlign = SharedRegion_getCacheLineSize(frmBuf_regionId);
        }


        totalSize = ROUND_UP(sizeof (FrameQBufMgr_NameServerEntry), minAlign);

        /* Calculate the memory required for the instance
         * shared control structure
         */
        totalSize += ROUND_UP(sizeof (FrameQBufMgr_ShMem_Attrs), minAlign);

        totalSize += ROUND_UP(sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                               minAlign) * params->numFreeFramePools;

        if (FrameQBufMgr_ShMem_module->cfg.usedefaultgate == FALSE) {
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
        /* Calculate the memory required for the instance's
         * ClientNotifyMgr handle
         */
        ClientNotifyMgr_Params_init (&clientMgrParams);
        clientMgrParams.regionId = regionId;
        clientMgrParams.numNotifyEntries = params->numNotifyEntries;
        clientMgrParams.numSubNotifyEntries = params->numFreeFramePools;

        totalSize +=  ROUND_UP(ClientNotifyMgr_sharedMemReq (
                                    (ClientNotifyMgr_Params*)&clientMgrParams ),
                                   minAlign);

        /* Calculate buffer Chunk needed for frame header buffers */

        for (i = 0; i < params->numFreeFramePools; i++) {
            bufSize += (   (    params->numFreeFramesInPool[i]
                              + params->numFreeHeaderBufsInPool[i])
                        * (  ROUND_UP(params->frameHeaderBufSize[i],
                             hdrBufMinAlign) ));
        }

        if (frmHdrBufChunkSize != NULL) {
            *frmHdrBufChunkSize = bufSize;
        }

        /* Calculate buffer Chunk needed for frame buffers */
        /* Reset bufSize */
        bufSize = 0;
        for (i = 0; i < params->numFreeFramePools; i++) {
            for (j = 0; j < params->numFrameBufsInFrame[i]; j++ ) {
                if (params->bufInterfaceType ==
                        FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM) {
                    frmBufSizesInFrm +=
                            ROUND_UP((UInt32)params->frameBufParams[i][j].size,
                            frmBufMinAlign);
                }
                if (params->bufInterfaceType ==
                    FrameQBufMgr_ShMem_BUFINTERFACE_TILERMEM) {
                        frmBufSizesInFrm = 0;
                      /* TBD */
                }
            }
            bufSize += (frmBufSizesInFrm * params->numFreeFramesInPool[i]);
            frmBufSizesInFrm = 0;
        }

        if (frmBufChunkSize != NULL) {
            *frmBufChunkSize = bufSize;
        }
    }

    GT_3trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_sharedMemReq", totalSize,
            *frmHdrBufChunkSize, *frmBufChunkSize);

    /*! @retval Shared memory required for  the instance */
    return (totalSize);
}


/*
 *  ======== FrameQBufMgr_ShMem_Params_init ========
 *  Initialize params struct
 */
Void FrameQBufMgr_ShMem_Params_init(FrameQBufMgr_ShMem_Params *params)
{
    UInt32 i;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_Params_init", params);

    GT_assert(curTrace, (params != NULL));

    if (params == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS,
                "FrameQBufMgr_ShMem_Params_init", FrameQBufMgr_E_FAIL,
                "params is NULL!");
    }
    else {
        /* Returning default instance params */
        memcpy(params,
                &(FrameQBufMgr_ShMem_module->defaultInstParams),
                sizeof(FrameQBufMgr_ShMem_Params));

        params->commonCreateParams.size =  sizeof(FrameQBufMgr_ShMem_Params);
        params->commonCreateParams.ctrlInterfaceType  =
                                               FrameQBufMgr_INTERFACE_SHAREDMEM;
        params->commonCreateParams.name            = NULL;
        params->commonCreateParams.openFlag        = FALSE;
        params->gate                               = NULL;
        params->sharedAddr                         = NULL;
        params->sharedAddrSize                     = 0;
        params->sharedAddrHdrBuf                   = NULL;
        params->sharedAddrHdrBufSize               = 0;
        params->sharedAddrFrmBuf                   = NULL;
        params->sharedAddrFrmBufSize               = 0;
        params->numFreeFramePools                  = 0;
        params->regionId                = 0;
        params->hdrBuf_regionId         = 0;
        params->frmBuf_regionId         = 0;
        params->cpuAccessFlags          = 0;
        params->numNotifyEntries        = 1;

        for (i = 0; i < FrameQBufMgr_ShMem_MAX_POOLS; i++) {
            params->numFreeFramesInPool[i] = 0;
            params->numFreeHeaderBufsInPool[i] = 0;
            params->frameHeaderBufSize[i] = 0;
            params->numFrameBufsInFrame[i] = 0;

            params->frameBufParams[i] = NULL;
        }
    }

    GT_0trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_Params_init");
}

/*
 *  ======== FrameQBufMgr_ShMem_create ========
 *  Create a FrameQBufMgr instance
 */
FrameQBufMgr_ShMem_Handle FrameQBufMgr_ShMem_create(
        FrameQBufMgr_ShMem_Params *params)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    UInt32                        tempHdrChunk;
    UInt32                        tempBufChunk;
    UInt32                        align          = 4;
    UInt32                        i              = 0;
    UInt32                        j;
    IArg                          key0;
    FrameQBufMgr_ShMem_Handle     handle         = NULL;
    FrameQBufMgr_ShMem_Obj        *obj            = NULL;
    Error_Block ebObj;
    Error_Block *eb = &ebObj;  /* locally used Error_Block */

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_ShMem_create", params);

    GT_assert (curTrace, (params != NULL));

    Error_init(eb);

    if (params == NULL) {
         status = FrameQBufMgr_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_ShMem_create",
                             status,
                             "Params pointer is NULL!");
    }
    else {
        /* Create the handle */
        /* Allocate memory for handle */
        handle = (FrameQBufMgr_ShMem_Object*)Memory_calloc(NULL,
                sizeof (FrameQBufMgr_ShMem_Object), 0u, eb);
        if (handle == NULL) {
            status = FrameQBufMgr_E_ALLOC_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQBufMgr_ShMem_create", status,
                    "Memory allocation failed for handle!");
        }
        else {
            obj = (FrameQBufMgr_ShMem_Obj *)Memory_calloc(NULL,
                    sizeof (FrameQBufMgr_ShMem_Obj), 0u, eb);

            if (obj == NULL) {
                status = FrameQBufMgr_E_ALLOC_MEMORY;
                Memory_free (NULL, handle, sizeof (FrameQBufMgr_ShMem_Object));
                handle = NULL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_ShMem_create",
                                     FrameQBufMgr_E_ALLOC_MEMORY,
                                     "Memory allocation failed for internal "
                                     "object!");
            }
        }
    }

    if (status >= 0) {
        if (params->commonCreateParams.openFlag == TRUE) {
            if ((params->commonCreateParams.name == NULL) && (NULL == params->sharedAddr)) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_ShMem_create",
                                     status,
                                     "Name and shared Addr provided are null!");
            }
            else {
                handle->obj = obj;

                status = _FrameQBufMgr_ShMem_open (handle, params, NULL);
            }
        }
        else {
            /* Validate the params */
            if ((params->sharedAddr == NULL) &&
                (params->regionId == SharedRegion_INVALIDREGIONID)) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                               GT_4CLASS,
                               "FrameQBufMgr_ShMem_create",
                               status,
                               "params->sharedAddr buffer is null.!");
            }
            else if (params->sharedAddrSize <
                     FrameQBufMgr_ShMem_sharedMemReq(params,
                                                     &tempHdrChunk,
                                                     &tempBufChunk)) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                               GT_4CLASS,
                               "FrameQBufMgr_ShMem_create",
                               status,
                               "Size of the sharedAddr buffer is  less than "
                               "the memory required for the instance.!");
            }
            else if (params->numFreeFramePools == 0) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                               GT_4CLASS,
                               "FrameQBufMgr_ShMem_create",
                               status,
                               "Size of the sharedAddr buffer is  less than "
                               "the memory required for the instance.!");
            }
            else {
#if 0
                /* Use the min alignment,
                 * unless an alignment has been requested.
                 */
                if (params->align == 0) {
                    align = Memory_getMaxDefaultTypeAlign();
                }
                else {
                    align = params->align;
                    /* Assert valid align parameter (must be power of 2) */

                }
#endif
                /* Check if Buffer address is given to create buffer pools*/
                if (params->sharedAddrHdrBuf != NULL) {
                    if (params->sharedAddrHdrBufSize < tempHdrChunk) {
                        status = FrameQBufMgr_E_INVALIDARG;
                        GT_setFailureReason (curTrace,
                                       GT_4CLASS,
                                       "FrameQBufMgr_ShMem_create",
                                       status,
                                       "sharedAddrHdrBufSize provided is zero.!");
                    }
                    else {
                        if ((  ((UInt32)params->sharedAddrHdrBuf)
                             & (align-1)) != 0) {
                            status = FrameQBufMgr_E_INVALIDARG;
                            GT_setFailureReason (curTrace,
                                           GT_4CLASS,
                                           "FrameQBufMgr_ShMem_create",
                                           status,
                                           "sharedAddrHdrBuf provided  is not"
                                           "aligned.!");
                        }
                    }
                }

                if (params->sharedAddrFrmBuf != NULL) {
                    if (params->sharedAddrFrmBufSize < tempBufChunk) {
                        status = FrameQBufMgr_E_INVALIDARG;
                        GT_setFailureReason (curTrace,
                                       GT_4CLASS,
                                       "FrameQBufMgr_ShMem_create",
                                       status,
                                       "sharedAddrFrmBufSize provided is not "
                                       "sufficient!");

                    }
                    else {
                        if ((  ((UInt32)params->sharedAddrFrmBuf)
                             & (align-1)) != 0) {
                            status = FrameQBufMgr_E_INVALIDARG;
                            GT_setFailureReason (curTrace,
                                           GT_4CLASS,
                                           "FrameQBufMgr_ShMem_create",
                                           status,
                                           "sharedAddrHdrBuf provided  is not"
                                           "aligned.!");

                        }
                    }
                }

                if (status >= 0) {
                    /* Ensure buffer Sizes specified in the bufSizes array
                     * is multiple of align
                     */
                    for ( i= 0; i < params->numFreeFramePools; i++ ) {
                        /* Ensure bufSizes[i] is a multiple of cache align.  */
                        if ((((UInt32)params->frameHeaderBufSize[i])
                             % FrameQBufMgr_ShMem_CACHE_LINESIZE) != 0) {
                            status = FrameQBufMgr_E_INVALIDARG;
                            GT_setFailureReason (curTrace, GT_4CLASS,
                                    "FrameQBufMgr_ShMem_create", status,
                                    "frameHeaderBufSize provided is not cache"
                                    "aligned!");
                            break;
                        }
                    }
                }

                if (status >= 0) {
                    /* Ensure buffer Sizes specified in the bufSizes array
                     * is multiple of align.
                     */
                    for ( i= 0;
                         (i < params->numFreeFramePools)&& (status >= 0); i++ ) {
                        for (j = 0; j < params->numFrameBufsInFrame[i]; j++) {
                            /* Ensure bufSizes[i] is cache aligned */
                            if ((((UInt32)params->frameBufParams[i][j].size)
                                 % FrameQBufMgr_ShMem_CACHE_LINESIZE) != 0) {
                                status = FrameQBufMgr_E_INVALIDARG;
                                GT_setFailureReason(curTrace, GT_4CLASS,
                                        "FrameQBufMgr_ShMem_create", status,
                                        "frameBufSizes provided is not cache"
                                        "aligned!");
                                break;
                            }
                        }
                    }
                }
                if (status >= 0) {
                    for ( i = 0;
                         (i < params->numFreeFramePools) && (status >= 0);
                         i++ ) {
                        GT_assert(curTrace,
                                  (   params->numFreeFramesInPool[i]
                                   <= FrameQBufMgr_ShMem_POOL_MAXFRAMES));
                        if (params->numFreeFramesInPool[i]
                            > FrameQBufMgr_ShMem_POOL_MAXFRAMES) {
                            status = FrameQBufMgr_E_INVALIDARG;
                            GT_setFailureReason(curTrace, GT_4CLASS,
                                    "_FrameQBufMgr_ShMem_create", status,
                                    "numFreeFramesInPool[] > "
                                    "FrameQBufMgr_ShMem_POOL_MAXFRAMES!");
                            break;
                        }
                        GT_assert(curTrace,
                                  (   params->numFreeHeaderBufsInPool[i]
                                   <= FrameQBufMgr_ShMem_POOL_MAXHDRS));
                        if ((  params->numFreeHeaderBufsInPool[i]
                             > FrameQBufMgr_ShMem_POOL_MAXHDRS)) {
                            status = FrameQBufMgr_E_INVALIDARG;
                            GT_setFailureReason(curTrace, GT_4CLASS,
                                    "_FrameQBufMgr_ShMem_create", status,
                                    "numFreeHeaderBufsInPool[] > "
                                    "FrameQBufMgr_ShMem_POOL_MAXHDRS!");
                            break;
                        }
                    }
                }
                if (status >= 0) {
                    handle->obj = obj;
                    status = _FrameQBufMgr_ShMem_create(handle, params, NULL);
                }
            }
        }

        if (status >= 0) {
            /* Plugin the function table */
            handle->fxns = &FrameQBufMgr_ShMem_fxns;

            /* Populate the params member */
            memcpy((Ptr)&obj->params, (Ptr)params,
                    sizeof(FrameQBufMgr_Params));

             /* Put in the local list */
             key0 = IGateProvider_enter (FrameQBufMgr_ShMem_module->gate);
             List_elemClear (&obj->listElem);
             List_put ((List_Handle)FrameQBufMgr_ShMem_module->objList,
                       &(obj->listElem));
             IGateProvider_leave (FrameQBufMgr_ShMem_module->gate, key0);
        }
    }

    /* Failed to create /open the instance. Do clean up*/
    if (status < 0) {
        if (obj != NULL ) {
            Memory_free (NULL, obj, sizeof (FrameQBufMgr_ShMem_Obj));
        }
        if (handle != NULL) {
            Memory_free (NULL, handle, sizeof (FrameQBufMgr_ShMem_Object));
            handle = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_create", handle);

    return (handle);
}


/*!
 * @brief  Function to delete the created FrameQBufMgr_ShMem instance
 *
 * @param  pHandle Pointer to the handle
 */
Int32 FrameQBufMgr_ShMem_delete(FrameQBufMgr_ShMem_Handle *pHandle)
{
    Int32                         status = FrameQBufMgr_S_SUCCESS;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    IArg                          key;
    FrameQBufMgr_ShMem_Handle     handle;
    FrameQBufMgr_ShMem_Obj      * obj;
    IArg                          key0;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_ShMem_delete", pHandle);

    GT_assert (curTrace, (pHandle != NULL));

    if (pHandle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_ShMem_delete",
                             status,
                             "pHandle passed is NULL!");
    }
    else if (*pHandle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_ShMem_delete",
                             status,
                             "*pHandle passed is NULL!");
    }
    else {
        handle = *pHandle;

        obj = (FrameQBufMgr_ShMem_Obj *) handle->obj;
        GT_assert (curTrace, (NULL != obj));

        pAttrs = obj->attrs;

        switch (obj->objType) {

        case FrameQBufMgr_DYNAMIC_OPEN:
        {
            key0 = IGateProvider_enter (FrameQBufMgr_ShMem_module->gate);
            /* Remove it from the local list */
            List_remove ((List_Handle)FrameQBufMgr_ShMem_module->objList,
                         &obj->listElem);
            IGateProvider_leave (FrameQBufMgr_ShMem_module->gate, key0);

            key = GateMP_enter (obj->gate);
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv ((Ptr)pAttrs,
                           sizeof (FrameQBufMgr_ShMem_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }
            GateMP_leave(obj->gate, key);

            /*Free the allocated memory for obj->freeFramePool pointers */
            if (obj->freeFramePool != NULL) {
                Memory_free (NULL,
                             obj->freeFramePool,
                             sizeof(UInt32)* pAttrs->numFreeFramePools);
            }

            /*Unregister the notification with the clientNotifyMgr */
            if (obj->isRegistered == TRUE) {
                ClientNotifyMgr_unregisterClient (obj->clientNotifyMgrHandle,
                                                  obj->notifyId);
                obj->isRegistered = FALSE;
            }

            if (obj->clientNotifyMgrHandle != NULL) {
                ClientNotifyMgr_close (&obj->clientNotifyMgrHandle);
            }

            if (   (obj->creCliGate == TRUE)
                && (obj->clientNotifyMgrGate != NULL) ) {
                /* Close GateMP instance of ClientNotifyMgr.*/
                GateMP_close ((GateMP_Handle*)&obj->clientNotifyMgrGate);
            }

            /* Close the instance gate if it is opened internally
             * in the instance.
             */
            if ((obj->creGate == TRUE) && (obj->gate != NULL)) {
                GateMP_close ((GateMP_Handle*)&obj->gate);
            }
            /* Delete create SyslinkMemMgrs of FrameHeaders anf Frame Buffers.*/
            if (obj->memMgrAllocater_hdrBufs != NULL) {
                SyslinkMemMgr_delete ((SyslinkMemMgr_Handle*)&obj->memMgrAllocater_hdrBufs);
            }

            if (obj->memMgrAllocater_frmBufs != NULL) {
                SyslinkMemMgr_delete ((SyslinkMemMgr_Handle*)&obj->memMgrAllocater_frmBufs);
            }

            /* Now free the handle */
            Memory_free (NULL, obj, sizeof (FrameQBufMgr_ShMem_Obj));
            obj = NULL;
            Memory_free (NULL, handle, sizeof (FrameQBufMgr_ShMem_Object));
            handle = NULL;
            break;
        }

        case FrameQBufMgr_ShMem_DYNAMIC_CREATE:
        case FrameQBufMgr_ShMem_DYNAMIC_CREATE_USEDREGION:
        {
            key0 = IGateProvider_enter (FrameQBufMgr_ShMem_module->gate);
            /* Remove it from the local list */
            List_remove ((List_Handle)FrameQBufMgr_ShMem_module->objList,
                         &obj->listElem);
            IGateProvider_leave (FrameQBufMgr_ShMem_module->gate, key0);
            _FrameQBufMgr_ShMem_cleanupCreatedInstance(*pHandle);
#if 0
            key = GateMP_enter (obj->gate);

            if (pAttrs != NULL) {
                if (obj->ctrlStructCacheFlag == TRUE) {
                    Cache_inv ((Ptr)pAttrs,
                               sizeof (FrameQBufMgr_ShMem_Attrs),
                               Cache_Type_ALL,
                               TRUE);
                }
                for (i = 0; i < pAttrs->numFreeFramePools ; i++) {
                    freeFramePoolObj = obj->freeFramePool[i];
                    memset((Ptr)freeFramePoolObj,
                            sizeof(FrameQBufMgr_ShMem_FreeFramePoolObj),
                            0);
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv ((Ptr)freeFramePoolObj,
                                     sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                                     Cache_Type_ALL,
                                     TRUE);
                    }
                }

                /* Free the allocated memory for obj->freeFramePool pointers */
                Memory_free (NULL,
                             obj->freeFramePool,
                             sizeof(UInt32)* pAttrs->numFreeFramePools);

                /* Delete the created ListMP objects  for bufpools */
                pAttrs->status = !FrameQBufMgr_ShMem_CREATED;
                if (obj->ctrlStructCacheFlag == TRUE) {
                    Cache_wbInv ((Ptr)pAttrs,
                                 sizeof (FrameQBufMgr_ShMem_Attrs),
                                 Cache_Type_ALL,
                                 TRUE);
                }

                /* Delete id from the module wide shared info */
                if (obj->instId != (UInt32)-1) {

                    instId = obj->instId & 0xFFFF;
                    /* Find out the creator procid */
                    creProcId = (  (instId >> FRAMEQBUFMGR_CREATORPROCID_BITOFFSET)
                                  & FRAMEQBUFMGR_CREATORPROCID_MASK);
                    GT_assert (curTrace, (creProcId < MultiProc_getNumProcessors()));

                    instNo = ( (instId >> FRAMEQBUFMGR_INSTNO_BITOFFSET)
                              & FRAMEQBUFMGR_INSTNO_MASK);
                    GT_assert(curTrace, (instNo < FrameQBufMgr_MAXINSTANCES));
                    FrameQBufMgr_freeInstNo(instNo);
                }
            }

            /* Remove entry from NameServer */
            if (   (NULL != FrameQBufMgr_ShMem_module->nameServer)
                && (obj->nsKey != 0)) {
                NameServer_removeEntry(FrameQBufMgr_ShMem_module->nameServer,
                                       obj->nsKey);
            }

            /*Unregister the notification with the clientNotifyMgr */
            if (obj->isRegistered == TRUE) {
                ClientNotifyMgr_unregisterClient (obj->clientNotifyMgrHandle,
                                                  obj->notifyId);
            }

            if (obj->clientNotifyMgrHandle != NULL) {
                ClientNotifyMgr_delete (&(obj->clientNotifyMgrHandle));
            }

            /* Delete GateMP instance of ClientNotifyMgr.*/
            if (   (obj->creCliGate == TRUE)
                && (obj->clientNotifyMgrGate != NULL) ) {
                GateMP_delete ((GateMP_Handle*)&obj->clientNotifyMgrGate);
            }

            GateMP_leave (obj->gate, key);

            /* Free the allocated memory for the instance control structure
             */
            if (obj->objType == FrameQBufMgr_ShMem_DYNAMIC_CREATE_USEDREGION) {
                Memory_free(SharedRegion_getHeap(obj->regionId),
                            obj->pnameserverEntry,
                            obj->allocSize);
            }
            /* Free up the allocated frame header buffers and
             * frame buffers if they are allocated from shared region.
             */
            if ((obj->hdrBufPtr != NULL) && (obj->hdrBufAllocSize > 0)) {
                Memory_free(SharedRegion_getHeap(obj->hdrBuf_regionId),
                            obj->hdrBufPtr,
                            obj->hdrBufAllocSize);
            }
            if ((obj->frmBufPtr != NULL) && (obj->frmBufAllocSize > 0)) {
                Memory_free(SharedRegion_getHeap(obj->frmBuf_regionId),
                            obj->frmBufPtr,
                            obj->frmBufAllocSize);
            }
            /* Delete instance gate if it is opened internally
             * in the instance.
             */
            if ((obj->creGate == TRUE) && (obj->gate != NULL)) {
                GateMP_delete ((GateMP_Handle*)&obj->gate);
                /* Free the allocated memory for the instance control
                 * structure.
                 */
                Memory_free(SharedRegion_getHeap(obj->regionId),
                            obj->instGateSharedAddr,
                            obj->gateMPAllocSize);
            }


            /* Delete created SyslinkMemMgrs */
            if (obj->memMgrAllocater_hdrBufs != NULL) {
                SyslinkMemMgr_delete ((SyslinkMemMgr_Handle*)&obj->memMgrAllocater_hdrBufs);
            }

            if (obj->memMgrAllocater_frmBufs != NULL) {
                SyslinkMemMgr_delete ((SyslinkMemMgr_Handle*)&obj->memMgrAllocater_frmBufs);
            }
#endif
            /* TODO: Close the heapMemMPinstances/Tiler Mangers opened for SyslinkMemMgrs
             */
            /* Now free the handle */
            Memory_free (NULL, obj, sizeof (FrameQBufMgr_ShMem_Obj));
            obj = NULL;
            Memory_free (NULL, handle, sizeof (FrameQBufMgr_ShMem_Object));
            handle = NULL;
            break;
        }

        default:
            status = FrameQBufMgr_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_ShMem_delete",
                                 status,
                                 "Invalid Handle!");
            break;
        }
    }
    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_delete", status);

    return (status);
}


/*
 *  ======== FrameQBufMgr_ShMem_open ========
 *  Open the created the created instance
 */
Int32
FrameQBufMgr_ShMem_open(FrameQBufMgr_ShMem_Handle *handlePtr,
        FrameQBufMgr_ShMem_OpenParams *openParams)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Params params;
    Error_Block                 eb;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_open", handlePtr,
            openParams);

    GT_assert(curTrace, (handlePtr != NULL));
    GT_assert(curTrace, (openParams != NULL));

    /* Reject  if both name and shared Addr is not known */
    if ((handlePtr == NULL) && (openParams == NULL)) {
        status = FrameQBufMgr_E_INVALIDARG;
    } else {
        /* Get the default params */
        if (strlen((String)openParams->commonOpenParams.name)
             > (FrameQBufMgr_MAXNAMELEN - 1)) {

        }
        else {
            Error_init(&eb);
            FrameQBufMgr_ShMem_Params_init(&params);
            /* Name is string pointer in CreateParams*/
            params.commonCreateParams.name = (String)
                                             openParams->commonOpenParams.name;
            params.commonCreateParams.openFlag = TRUE;
            params.sharedAddr = openParams->commonOpenParams.sharedAddr;
            /* Opening the instance  */
            params.cpuAccessFlags = openParams->commonOpenParams.cpuAccessFlags;

            *handlePtr = FrameQBufMgr_ShMem_create(&params);
            if (*handlePtr == NULL) {
                status = FrameQBufMgr_E_FAIL;
            }
        }
    }

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_close ========
 *  Close the opened instance
 */
Int32 FrameQBufMgr_ShMem_close(FrameQBufMgr_ShMem_Handle *handlePtr)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_close", handlePtr);

    GT_assert(curTrace, (handlePtr != NULL));
    GT_assert(curTrace, (*handlePtr != NULL));

    status = FrameQBufMgr_ShMem_delete(handlePtr);

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_close", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_alloc ========
 *  Allocate a frame
 */
Int32 FrameQBufMgr_ShMem_alloc(FrameQBufMgr_ShMem_Handle handle,
        FrameQBufMgr_ShMem_Frame *framePtr)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    Bool                               found            = FALSE;
    FrameQBufMgr_ShMem_FrameBufInfo   *frameBufInfo     = NULL;
    UInt32                             freeFramePoolIndex = 0;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj     *freeFramePoolObj;
    UInt32                             i;
    UInt32                             key;
    Ptr                                portableAddr;
    FrameQBufMgr_ShMem_Frame           frame;
    Ptr                                virtPtr;
    UInt8                              j;
    FrameQBufMgr_ShMem_Obj                   *obj ;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_alloc", handle,
            framePtr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    pAttrs = obj->attrs;

    GT_assert(curTrace, (pAttrs != NULL));

   *framePtr = NULL;
    /* Acquire lock */
    key = GateMP_enter(obj->gate);
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)pAttrs,
                   sizeof (FrameQBufMgr_ShMem_Attrs),
                   Cache_Type_ALL,
                   TRUE);
    }

    /* Alloc allocates frame from the free Frame Pool 0*/
    freeFramePoolIndex = 0;
    freeFramePoolObj = obj->freeFramePool[freeFramePoolIndex];
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)freeFramePoolObj,
                   sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                   Cache_Type_ALL,
                   TRUE);
    }

    if (freeFramePoolObj->freeFrms > 0) {
        /* Find out the free frame by checking the reference counts of the
         * frames.
         */
        for (i = 0; i < freeFramePoolObj->numFrms; i++) {
            if(freeFramePoolObj->frmRefCnt[i] == 0) {
                /* free frame found*/
                found = TRUE;
                break;
            }
        }

        if (found == TRUE) {
            freeFramePoolObj->freeFrms -=1u;
            freeFramePoolObj->frmRefCnt[i] += 1u;

            portableAddr = (Ptr)
            (freeFramePoolObj->frmStartAddrp +
             (  (   ( (freeFramePoolObj->frmEndAddrp)
                  -  (freeFramePoolObj->frmStartAddrp))
               /freeFramePoolObj->numFrms)* i) );

                frame = SyslinkMemMgr_translate (obj->memMgrAllocater_hdrBufs,
                                        (Ptr)portableAddr,
                                         SyslinkMemMgr_AddrType_Portable,
                                         SyslinkMemMgr_AddrType_Virtual);
                GT_assert(curTrace, (NULL != frame));

               /* Convert frame buffer addresses from portable format to virtual
                * address before returning free frame to the caller.
                */
                frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo *)
                                                     &(frame->frameBufInfo[0]);
                for (j = 0; j < frame->numFrameBuffers; j++) {
                    if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM) {
                        portableAddr = (Ptr)frameBufInfo[j].bufPtr;
                        virtPtr = SyslinkMemMgr_translate(
                                obj->memMgrAllocater_hdrBufs,
                                (Ptr)portableAddr,
                                SyslinkMemMgr_AddrType_Portable,
                                SyslinkMemMgr_AddrType_Virtual);
                        GT_assert(curTrace, (virtPtr != NULL));
                        frameBufInfo[j].bufPtr = (UInt32)virtPtr;
                    }
                }
                *framePtr = frame;
        }
        else {
            status = FrameQBufMgr_E_ALLOC_FRAME;
        }
    }
    else if (freeFramePoolObj->freeDynamicFrames > 0){
        /* TODO:Allocates from the dynamically added frames */
        status = FrameQBufMgr_E_ALLOC_FRAME;
    }
    else {
        status = FrameQBufMgr_E_ALLOC_FRAME;
    }

    if (obj->ctrlStructCacheFlag == TRUE) {
       Cache_wbInv ((Ptr)freeFramePoolObj,
                    sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                    Cache_Type_ALL,
                    TRUE);
    }

    if (((status < 0)||(found == FALSE))&& (obj->isRegistered == TRUE)) {
        ClientNotifyMgr_enableNotification (obj->clientNotifyMgrHandle,
                                            obj->notifyId,
                                            freeFramePoolIndex);
    }

    GateMP_leave(obj->gate, key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_alloc", status);

    return (status);
}


/*
 *  ======== FrameQBufMgr_ShMem_allocv ========
 *  Allocate multiple frames from the FrameQbufMgr instance
 */
Int32 FrameQBufMgr_ShMem_allocv(FrameQBufMgr_ShMem_Handle handle,
                           FrameQBufMgr_ShMem_Frame   framePtr[],
                           UInt32                     freeQId[],
                           UInt8                     *numFrames)
{
    Int32                              status           = FrameQBufMgr_S_SUCCESS;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    Bool                               found            = FALSE;
    FrameQBufMgr_ShMem_FrameBufInfo   *frameBufInfo = NULL;
    UInt32                             lframes      = 0;
    UInt32                             freeFramePoolIndex;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj     *freeFramePoolObj;
    UInt32                             i;
    UInt32                             key;
    Ptr                                portableAddr;
    FrameQBufMgr_ShMem_Frame           frame;
    Ptr                                virtPtr;
    UInt8                              j;
    Int32                              tmpStatus;
    /* TO find out the duplicate freeQ ids and frames requested in that free Q*/
    UInt32                             pfreeQIds[FrameQBufMgr_MAX_FRAMESINVAPI] = {0};/* TBD: 64 needs to be a macro*/
    UInt32                             pNumFrames[FrameQBufMgr_MAX_FRAMESINVAPI] = {0};/* TBD: 64 needs to be a macro*/
    UInt32                             count = 0;
    UInt32                             pNotifySubIds[FrameQBufMgr_MAX_FRAMESINVAPI];/* TBD: 64 needs to be a macro*/
    UInt32                             numSubEntries = 0;
    FrameQBufMgr_ShMem_Obj             *obj;
    Error_Block                        eb;

    GT_4trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_allocv", handle,
            framePtr, freeQId, numFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));
    GT_assert(curTrace, (freeQId != NULL));
    GT_assert(curTrace, (numFrames != NULL));
    GT_assert(curTrace, (FrameQBufMgr_ShMem_MAX_POOLS >= *numFrames));
    GT_assert(curTrace, (FrameQBufMgr_MAX_FRAMESINVAPI >= *numFrames));

    Error_init(&eb);

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    pAttrs = obj->attrs;
    GT_assert(curTrace, (pAttrs != NULL));

    /* Find out the duplicate entries in freeQids specified */
    for (i = 0 ; i < *numFrames ; i++) {
        if (pfreeQIds [i] != (UInt32) -1) {
            pfreeQIds [count] = freeQId[i];
            pNumFrames [count] += 1;

            for (j = i + 1 ; j < *numFrames ; j++) {
                if (pfreeQIds [count] ==  freeQId[j]) {
                    pNumFrames [count] += 1;
                    pfreeQIds [j] = (UInt32) -1 ;
                }
            }
            count++ ;
        }
    }

    /* Acquire lock */
    key = GateMP_enter (obj->gate);

    /*  Invalidate attrs if required */
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)pAttrs,
                   sizeof (FrameQBufMgr_ShMem_Attrs),
                   Cache_Type_ALL,
                   TRUE);
    }

    for (i = 0; i < count; i++) {
        freeFramePoolIndex = pfreeQIds[i];
        freeFramePoolObj = obj->freeFramePool[freeFramePoolIndex];
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv((Ptr)freeFramePoolObj,
                    sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                    Cache_Type_ALL, TRUE);
        }
        if (  ( (freeFramePoolObj->freeFrms)
                +(freeFramePoolObj->freeDynamicFrames ))
              < pNumFrames [i]) {
             status = FrameQBufMgr_E_ALLOC_FRAME;
             /* We are using subNotifyentry 0 for framePool 0 and on */
             pNotifySubIds[numSubEntries] = freeFramePoolIndex;
             numSubEntries += 1;

        }
    }

    if (status >= 0) {
        for (i = 0;  i < *numFrames; i++) {
            /* Allocates frame from the  Frame Pool*/
            freeFramePoolIndex = freeQId[i];
            freeFramePoolObj = obj->freeFramePool[freeFramePoolIndex];
            if (freeFramePoolObj->freeFrms > 0) {
                /* Find out the free frame by checking the reference
                 * counts of the frames.
                 */
                for (j = 0;
                     j < freeFramePoolObj->numFrms;
                     j++) {
                    if(   freeFramePoolObj->frmRefCnt[j]
                        == 0) {
                        /* free frame found*/
                        found = TRUE;
                        break;
                    }
                }

                if (found == TRUE) {
                    freeFramePoolObj->freeFrms -=1u;
                    freeFramePoolObj->frmRefCnt[j] += 1u;
                    portableAddr = (Ptr)
                    (freeFramePoolObj->frmStartAddrp +
                     (  (   ( (freeFramePoolObj->frmEndAddrp)
                          -  (freeFramePoolObj->frmStartAddrp))
                       /freeFramePoolObj->numFrms)* j) );

                    frame = SyslinkMemMgr_translate (
                                               obj->memMgrAllocater_hdrBufs,
                                               (Ptr)portableAddr,
                                               SyslinkMemMgr_AddrType_Portable,
                                               SyslinkMemMgr_AddrType_Virtual);
                    /* Convert frame buffer addresses from portable
                     * format to virtual address before returning free
                     * frame to the caller.
                     */
                    frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo *)
                                   &(frame->frameBufInfo[0]);
                    for(j = 0; j < frame->numFrameBuffers; j++) {
                        if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM){
                            portableAddr = (Ptr)frameBufInfo[j].bufPtr;
                            virtPtr = SyslinkMemMgr_translate(
                                    obj->memMgrAllocater_hdrBufs,
                                    (Ptr)portableAddr,
                                    SyslinkMemMgr_AddrType_Portable,
                                    SyslinkMemMgr_AddrType_Virtual);
                            GT_assert(curTrace, (virtPtr != NULL));
                            frameBufInfo[j].bufPtr = (UInt32)virtPtr;
                        }
                    }
                    framePtr[i] = frame;
                    lframes += 1;
                }
            }
            else if (freeFramePoolObj->freeDynamicFrames > 0){
                /* TODO:Allocates from the dynamically added frames */
                status        = FrameQBufMgr_E_ALLOC_FRAME;
                framePtr[i]  = NULL;
            }
            else {
                status        = FrameQBufMgr_E_ALLOC_FRAME;
                framePtr[i]  = NULL;

            }

           if (obj->ctrlStructCacheFlag == TRUE) {
               Cache_wbInv ((Ptr)freeFramePoolObj,
                            sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                            Cache_Type_ALL,
                            TRUE);
            }

        }
        if ((lframes > 0) && (lframes < *numFrames)) {
            /* Set the status indicating all the frames are not allocated. Only able
             * allocate few frames
             */
            status = FrameQBufMgr_E_ALLOC_ALLFRAMES;
        }
    }

    if (((status == FrameQBufMgr_E_ALLOC_FRAME) ||
            (status == FrameQBufMgr_E_ALLOC_ALLFRAMES)) &&
            (obj->isRegistered == TRUE)) {
        tmpStatus = ClientNotifyMgr_waitOnMultiSubEntryNotify(
                obj->clientNotifyMgrHandle,
                obj->notifyId,
                pNotifySubIds,
                numSubEntries);
        if (tmpStatus < 0) {
            status = FrameQBufMgr_E_FAIL;
        }
    }

    if (lframes == 0) {
        for (i = 0; i < *numFrames; i++) {
            framePtr[i]  = NULL;
        }
    }

    /* Update the *numFrames with the  number of frames allocated */
    *numFrames = lframes;

    GateMP_leave(obj->gate, key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_allocv", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_free ========
 *  Free frame
 */
Int32 FrameQBufMgr_ShMem_free(FrameQBufMgr_ShMem_Handle handle,
        FrameQBufMgr_ShMem_Frame frame)
{
    Int32                         status        = FrameQBufMgr_S_SUCCESS;
    Bool                          toNotify      = FALSE;
#if defined (SYSLINK_FRAMEQBUFMGR_SUPPORT_DYN_ADD_FRAMES)
    Bool                          freed         = FALSE;
#endif
    UInt32                        index         = 0;
    UInt32                        i             = 0;
    UInt32                        totalFreeFrames ;
    FrameQBufMgr_ShMem_FrameBufInfo              *frameBufInfo  = NULL;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj *freeFramePoolObj;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs  = NULL;
    UInt32                        portablePtr;
    UInt32                        key;
    UInt32                        refIndex;
    FrameQBufMgr_ShMem_Obj             *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_free", handle, frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame != NULL));

    if (frame == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQBufMgr_ShMem_free",
                status, "frame passed is NULL!");
    }
    else {
        obj = handle->obj;
        GT_assert(curTrace, (obj != NULL));

        pAttrs = obj->attrs;

        key = GateMP_enter (obj->gate);

        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv ((Ptr)pAttrs,
                       sizeof (FrameQBufMgr_ShMem_Attrs),
                       Cache_Type_ALL,
                       TRUE);
        }

        index = frame->freeFrmQueueNo;

        if (index >= pAttrs->numFreeFramePools) {
            status = FrameQBufMgr_E_FAIL;
        }
        else {

            /* Convert the frame addr to portable address to check to which
             * frame pool it belongs.
             */
            portablePtr = (UInt32)SyslinkMemMgr_translate(
                    obj->memMgrAllocater_hdrBufs, (Ptr)frame,
                    SyslinkMemMgr_AddrType_Virtual,
                    SyslinkMemMgr_AddrType_Portable);
            GT_assert(curTrace,(portablePtr != (UInt32)NULL));

            freeFramePoolObj = obj->freeFramePool[index];
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv ((Ptr)freeFramePoolObj,
                           sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                           Cache_Type_ALL,
                           TRUE);
            }

            /* Check if frame  belongs to the frames created during create time
             */
            if (  (portablePtr >= freeFramePoolObj->frmStartAddrp)
                &&(portablePtr <  freeFramePoolObj->frmEndAddrp)) {
                /* Buffer  needs to freed static frame  list*/

                /* Find out the offset of the buffer in static frames to be able
                 * to reduce the reference count
                 */

                i = ( (portablePtr - (freeFramePoolObj->frmStartAddrp))/
                      freeFramePoolObj->frmHdrBufSize);
                GT_assert (curTrace, (freeFramePoolObj->frmRefCnt[i] != 0));
                freeFramePoolObj->frmRefCnt[i]--;
                if (freeFramePoolObj->frmRefCnt[i] == 0) {
                    /* Free buffer is availabe for subsequnet allocs */
                    toNotify = TRUE;
                    freeFramePoolObj->freeFrms++;
                }
                /* convert the frame buffer pointer in this frame to
                 * portable pointers as this  frame header belongs to free frames
                 * and not  belongs to heade buffers.
                 */
                 frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo*)
                                                     &(frame->frameBufInfo[0]);
                 for (i = 0; i < frame->numFrameBuffers; i++) {
                    if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM){
                        frameBufInfo[i].bufPtr = (UInt32)SyslinkMemMgr_translate(
                            obj->memMgrAllocater_hdrBufs,
                            (Ptr)frameBufInfo[i].bufPtr,
                            SyslinkMemMgr_AddrType_Virtual,
                            SyslinkMemMgr_AddrType_Portable);
                        GT_assert (curTrace, ((Ptr)frameBufInfo[i].bufPtr != NULL));
                    }
                 }
                /* WriteBack frame contents to memory as next alloc should
                 * see the portable frame pointers of Frame buffers.
                 */
                 FrameQBufMgr_ShMem_writeBackHeaderBuf(handle, frame);

            }
            else if ((portablePtr >= freeFramePoolObj->hdrBufStartAddrp)
                &&(portablePtr <  freeFramePoolObj->hdrBufEndAddrp)) {
                /* Check if it is a duped frame */
                i = ( (portablePtr - (freeFramePoolObj->hdrBufStartAddrp))/
                  freeFramePoolObj->frmHdrBufSize);
                /* Make the frame header is free */
                freeFramePoolObj->hdrBufIsUsed[i] = FALSE;

                refIndex = freeFramePoolObj->hdrBuf_refCntIndex[i];

                GT_assert(curTrace,
                        (freeFramePoolObj->frmRefCnt[refIndex] != 0));

                freeFramePoolObj->frmRefCnt[refIndex]--;
                if (freeFramePoolObj->frmRefCnt[refIndex] == 0) {
                    /* Now frame becomes free.So notify */
                    toNotify = TRUE;
                    freeFramePoolObj->freeFrms++;
                }
                freeFramePoolObj->freeFrmHdrBufs++;
            }
            else {
#if defined (SYSLINK_FRAMEQBUFMGR_SUPPORT_DYN_ADD_FRAMES)
                /* Check in dynamic frame list */
                for ( i = 0;
                      (   (i < freeFramePoolObj->numDynamicFrames)
                       && (freed == FALSE));
                      i++) {
                    if (portablePtr ==  freeFramePoolObj->dynFrame[i]) {
                        /* Index of the Buffer to be freed is found. */
                        freeFramePoolObj->dynFrmRefCnt[i] -= 1;
                        if (freeFramePoolObj->dynFrmRefCnt[i] == 0) {
                            toNotify = TRUE;
                            freed    = TRUE;
                            freeFramePoolObj->freeDynamicFrames++;
                        }
                        /*convert the frame buffer pointer in this frame to
                         * portable pointers as this  frame header belongs to
                         * dynamic free frames and not belongs to dynamic header
                         * buffers.
                         */
                        frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo*)
                                &(frame->frameBufInfo[0]);
                         for (i = 0; i < frame->numFrameBuffers; i++) {
                            if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM){
                                frameBufInfo[i].bufPtr = (UInt32)
                                        SyslinkMemMgr_translate(
                                        obj->memMgrAllocater_hdrBufs,
                                        (Ptr)frameBufInfo[i].bufPtr,
                                        SyslinkMemMgr_AddrType_Virtual,
                                        SyslinkMemMgr_AddrType_Portable);
                                GT_assert(curTrace,
                                          (frameBufInfo[i].bufPtr != (UInt32)NULL));
                            }

                         }

                       break;
                    }
                }

                if (freed == FALSE) {
                    /*Check if buffer is duped with dynamic hdrbufs */
                    /* TODO */
                }
#endif
            }
        }

        totalFreeFrames =  (  freeFramePoolObj->freeFrms
                            + freeFramePoolObj->freeDynamicFrames);
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_wbInv ((Ptr)freeFramePoolObj,
                         sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);

        if (toNotify == TRUE) {
            /* Sub notify entry no and pool index no are one to one mapped.*/
            ClientNotifyMgr_sendNotificationMulti (obj->clientNotifyMgrHandle,
                    obj->notifyId, index, totalFreeFrames, (UInt32)-1);

        }
    }

     GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_free", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_freev ========
 *  Free multiple frames
 */
Int32
FrameQBufMgr_ShMem_freev (FrameQBufMgr_ShMem_Handle  handle,
                          FrameQBufMgr_ShMem_Frame   framePtr[],
                          UInt32                     numFrames)
{
    Int32                         status        = FrameQBufMgr_S_SUCCESS;
    Bool                          toNotify      = FALSE;
    Bool                          freed         = FALSE;
    UInt32                        index         = 0;
    UInt32                        i             = 0;
    UInt32                            totalFreeFrames = 0;
    FrameQBufMgr_ShMem_FrameBufInfo  *frameBufInfo  = NULL;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj     *freeFramePoolObj;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    UInt32                        portablePtr;
    UInt32                        key;
    UInt32                        refIndex;
    UInt32                        frmCnt;
    FrameQBufMgr_ShMem_Obj        *obj;

    GT_3trace ( curTrace,
                GT_ENTER,
                "FrameQBufMgr_ShMem_freev",
                handle,
                framePtr,
                numFrames);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (framePtr != NULL));
    GT_assert (curTrace, (numFrames != 0));

    obj = handle->obj;
    GT_assert (curTrace, (obj != NULL));

    pAttrs = obj->attrs;

    key = GateMP_enter (obj->gate);

    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv((Ptr)pAttrs, sizeof(FrameQBufMgr_ShMem_Attrs),
                Cache_Type_ALL, TRUE);
    }

    /* Free frames by looking at to which pool no it belongs. */
    for (frmCnt = 0; frmCnt < numFrames; frmCnt++) {
        index = framePtr[frmCnt]->freeFrmQueueNo;

        if (index >= pAttrs->numFreeFramePools) {
            status = FrameQBufMgr_E_FAIL;
        }
        else {
            freeFramePoolObj = obj->freeFramePool[index];
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv ((Ptr)freeFramePoolObj,
                           sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                           Cache_Type_ALL,
                           TRUE);
            }
            /* Convert frame addr to portable address to check  reference
             * counts etc.
             */
            portablePtr = (UInt32)SyslinkMemMgr_translate (
                                                   obj->memMgrAllocater_hdrBufs,
                                                   (Ptr)framePtr[frmCnt],
                                                   SyslinkMemMgr_AddrType_Virtual,
                                                   SyslinkMemMgr_AddrType_Portable);
            GT_assert(curTrace, (portablePtr != NULL));

            /* Check if frame  belongs to the frames created during create time
             */
            if (  (portablePtr >= freeFramePoolObj->frmStartAddrp)
                &&(portablePtr <  freeFramePoolObj->frmEndAddrp)) {
                /* Buffer  needs to freed static frame  list*/

                /* Find out the offset of the buffer in static frames to be able
                 * to reduce the reference count
                 */

                i = ( (portablePtr - (freeFramePoolObj->frmStartAddrp))/
                      freeFramePoolObj->frmHdrBufSize);
                GT_assert(curTrace, (freeFramePoolObj->frmRefCnt[i] != 0));

                freeFramePoolObj->frmRefCnt[i]--;
                if (freeFramePoolObj->frmRefCnt[i] == 0) {
                    /* Free buffer is availabe for subsequent allocs */
                    toNotify = TRUE;
                    freeFramePoolObj->freeFrms++;
                }

                /* Convert the frame buffer pointer in this frame to
                 * portable pointers as this frame header belongs to free frames
                 * and not belongs to header buffers.
                 */
                 frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo*)
                                           &(framePtr[frmCnt]->frameBufInfo[0]);
                 for (i = 0; i < framePtr[frmCnt]->numFrameBuffers; i++) {
                    if(obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM){
                        frameBufInfo[i].bufPtr = (UInt32) SyslinkMemMgr_translate (
                                                   obj->memMgrAllocater_hdrBufs,
                                                  (Ptr)frameBufInfo[i].bufPtr,
                                                       SyslinkMemMgr_AddrType_Virtual,
                                                       SyslinkMemMgr_AddrType_Portable);
                        GT_assert(curTrace, (NULL != frameBufInfo[i].bufPtr));
                    }
                 }
                /* WriteBack frame contents to memory  as next alloc should
                 * see the portable frame pointers of Frame buffers.
                 */
                 FrameQBufMgr_ShMem_writeBackHeaderBuf(handle, framePtr[frmCnt]);
            }
            else if ((portablePtr >= freeFramePoolObj->hdrBufStartAddrp)
                &&(portablePtr <  freeFramePoolObj->hdrBufEndAddrp)) {
                /* Check if it is a duped frame */
                i = ( (portablePtr - (freeFramePoolObj->hdrBufStartAddrp))/
                  freeFramePoolObj->frmHdrBufSize);
                /* Make the frame header is free */
                freeFramePoolObj->hdrBufIsUsed[i] = FALSE;

                refIndex = freeFramePoolObj->hdrBuf_refCntIndex[i];
                GT_assert(curTrace,
                        (freeFramePoolObj->frmRefCnt[refIndex] != 0));
                freeFramePoolObj->frmRefCnt[refIndex]--;
                if (freeFramePoolObj->frmRefCnt[refIndex] == 0) {
                    /* Now frame becomes free.So notify */
                    toNotify = TRUE;
                    freeFramePoolObj->freeFrms++;
                }
                freeFramePoolObj->freeFrmHdrBufs++;
            }
            else {
#if defined (SYSLINK_FRAMEQBUFMGR_SUPPORT_DYN_ADD_FRAMES)
                /* Check in dynamic frame list */
                for ( i = 0;
                      (   (i < freeFramePoolObj->numDynamicFrames)
                       && (freed == FALSE));
                      i++) {
                    if (portablePtr ==  freeFramePoolObj->dynFrame[i]) {
                        /* Index of the Buffer to be freed is found. */
                        freeFramePoolObj->dynFrmRefCnt[i] -= 1;
                        if (freeFramePoolObj->dynFrmRefCnt[i] == 0) {
                            toNotify = TRUE;
                            freed    = TRUE;
                            freeFramePoolObj->freeDynamicFrames++;
                        }
                        /*convert the frame buffer pointer in this frame to
                         * portable pointers as this  frame header belongs to
                         * dynamic free frames and not belongs to dynamic header
                         * buffers.
                         */
                         frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo*)
                                           &(framePtr[frmCnt]->frameBufInfo[0]);
                         for (i = 0;
                              i < framePtr[frmCnt]->numFrameBuffers;
                              i++) {
                            if(obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM){
                            frameBufInfo[i].bufPtr = (UInt32)
                                       SyslinkMemMgr_translate (
                                                    obj->memMgrAllocater_hdrBufs,
                                                    (Ptr)frameBufInfo[i].bufPtr,
                                                        SyslinkMemMgr_AddrType_Virtual,
                                                        SyslinkMemMgr_AddrType_Portable);
                            GT_assert(curTrace, (NULL != frameBufInfo[i].bufPtr));
                         }
                         }

                       break;
                    }
                }
#endif /*#if defined (SYSLINK_FRAMEQBUFMGR_SUPPORT_DYN_ADD_FRAMES)*/
                if (freed == FALSE) {
                    /*Check if buffer is duped with dynamic hdrbufs */
                    /* TODO */
                }

            }
        }

        totalFreeFrames =  (   freeFramePoolObj->freeFrms
                             + freeFramePoolObj->freeDynamicFrames);
        if (obj->ctrlStructCacheFlag == TRUE) {
           Cache_wbInv ((Ptr)freeFramePoolObj,
                        sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                        Cache_Type_ALL,
                        TRUE);
        }

        if (toNotify == TRUE) {
            /* Release lock */
           GateMP_leave(obj->gate, key);

            /* Send notification to all the other clients of this instance
             * on this framePool identified by the index.
             */
            ClientNotifyMgr_sendNotificationMulti(obj->clientNotifyMgrHandle,
                                                   obj->notifyId,
                                                   index,
                                                   totalFreeFrames,
                                                   (UInt32)-1 );
            key = GateMP_enter (obj->gate);

        }
        toNotify = FALSE;
    }

    GateMP_leave (obj->gate, key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_freev", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_add ========
 *  Add free frames dynamically
 */
Ptr FrameQBufMgr_ShMem_add(FrameQBufMgr_ShMem_Handle handle, UInt8 freeQId)
{
    Ptr ptr = NULL;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_add", handle, freeQId);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (FrameQBufMgr_MAX_POOLS > freeQId));

    /* Currently not implemented */

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_add (NOT IMPLEMENTED!)",
            ptr);

    return (ptr);
}

/*
 *  ======== FrameQBufMgr_ShMem_remove ========
 *  Remove frame from the FrameQBufMgr
 */
Int32
FrameQBufMgr_ShMem_remove(FrameQBufMgr_ShMem_Handle handle,
        UInt8 freeQId, FrameQBufMgr_ShMem_Frame framePtr)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "FrameQBufMgr_ShMem_remove", handle,
            freeQId, framePtr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (FrameQBufMgr_MAX_POOLS > freeQId));
    GT_assert(curTrace, (NULL != framePtr));

    /* Currently not implemented */
    status = FrameQBufMgr_E_NOTIMPLEMENTED;

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_remove", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_registerNotifier ========
 * Function to Register notification with the instance.
 *
 * It internally registers notification with the individual free queues(free frame pools).
 * If alloc on free queue 0 failed,Notification will be generated if free frames
 * in that queue becomes more than watermark(all the free frames in that queue).
 */
Int32
FrameQBufMgr_ShMem_registerNotifier (
                                  FrameQBufMgr_ShMem_Handle        handle,
                                  FrameQBufMgr_ShMem_NotifyParams *notifyParams)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    ClientNotifyMgr_registerNoitifyParams regParams;
    UInt32 lNotifyId;
    FrameQBufMgr_ShMem_Obj  *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_registerNotifier",
               handle,
               notifyParams);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (notifyParams != NULL));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));
    /* Register call back function with the clientNotifyMgr.Pass the
     * cbClientHandle which is passed by the caller of this API to
     * clientNotifyMgr so that it will be passed as first arg to the call back
     * function during notification.
     */
    regParams.clientHandle   = notifyParams->cbClientHandle;
    regParams.procId         = MultiProc_self();
    regParams.notifyType     = notifyParams->notifyType;
    regParams.fxnPtr         = (ClientNotifyMgr_FnCbck)(notifyParams->cbFxnPtr);
    regParams.cbContext      = notifyParams->cbContext;
    regParams.watermarkCond1 = notifyParams->watermark;
    regParams.watermarkCond2 = (UInt32)-1;

    /*Call ClientNotifyMgr_registerClient API */
    status = ClientNotifyMgr_registerClient(obj->clientNotifyMgrHandle,
            &regParams, &lNotifyId);
    if (status >= 0) {
        obj->isRegistered = TRUE;
        obj->notifyId = lNotifyId;
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_registerNotifier",
            status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_unregisterNotifier ========
 *  Unregister the notification call back function
 */
Int32 FrameQBufMgr_ShMem_unregisterNotifier(FrameQBufMgr_ShMem_Handle handle)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_unregisterNotifier",
            handle);

    GT_assert(curTrace, (handle != NULL));

    if (handle == NULL) {
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);

        status = ClientNotifyMgr_unregisterClient(obj->clientNotifyMgrHandle,
                obj->notifyId);
        if (status >= 0) {
            obj->isRegistered = FALSE;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_unregisterNotifier",
            status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_dup ========
 *  Duplicate the given frame
 */
Int32
FrameQBufMgr_ShMem_dup (FrameQBufMgr_ShMem_Handle        handle,
                        FrameQBufMgr_ShMem_Frame   frame,
                        FrameQBufMgr_ShMem_Frame   dupedFramePtr[],
                        UInt32                     numDupedFrames)
{
    Int32   status = FrameQBufMgr_S_SUCCESS;
    Bool    bDuped = FALSE;
    UInt32  ldupedFrames = 0;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj *freeFramePoolObj;
    FrameQBufMgr_ShMem_Frame lframe;
    UInt32  portablePtr;
    UInt8   index;
    UInt32  lFrameIndex;
    UInt8   indexType;
    UInt32  refIndex;
    UInt32  i;
    UInt32  k;
    Int32   key;
    FrameQBufMgr_ShMem_Obj    *obj;

    GT_4trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_dup", handle, frame,
            dupedFramePtr, numDupedFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame != NULL));
    GT_assert(curTrace, (dupedFramePtr != NULL));
    GT_assert(curTrace, (numDupedFrames != 0));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    index = frame->freeFrmQueueNo;
    /* Reset numDupedFramePtr*/
    for (i = 0; i < numDupedFrames; i++) {
        dupedFramePtr[i] = NULL;
    }

    pAttrs = obj->attrs;
    GT_assert(curTrace, (pAttrs != NULL));

    key = GateMP_enter(obj->gate);

    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)pAttrs,
                   sizeof (FrameQBufMgr_ShMem_Attrs),
                   Cache_Type_ALL,
                   TRUE);
    }

    /* Check if the frame is static frame or duped frame created from the static
     * headers or dynamic frame or frame with duped dynamic header.
     */
    if (index >= pAttrs->numFreeFramePools) {
        status = FrameQBufMgr_E_FAIL;
    }
    else {
        freeFramePoolObj = obj->freeFramePool[index];
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv ((Ptr)freeFramePoolObj,
                       sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                       Cache_Type_ALL,
                       TRUE);
        }
        /* Check if this queue has sufficient number of free header bufs to dup
         * the given frame.
         */
         if (    numDupedFrames
              > (  freeFramePoolObj->freeFrmHdrBufs
                 + freeFramePoolObj->freeDynamicHdrBufs)) {
             /* Can not satisfy the requirement with the available free header
              * buffers.
              */
             status = FrameQBufMgr_E_INSUFFICIENT_HDRBUFS;
         }
         else {
            /* Convert frame addr to portable address to check  reference
             * counts etc.
             */
            portablePtr = (UInt32)SyslinkMemMgr_translate (
                                            obj->memMgrAllocater_hdrBufs,
                                            (Ptr)frame,
                                            SyslinkMemMgr_AddrType_Virtual,
                                            SyslinkMemMgr_AddrType_Portable);
            GT_assert(curTrace, (NULL != portablePtr));

            /* Check if frame  belongs to the frames created during create time
             */
            if (  (portablePtr >= freeFramePoolObj->frmStartAddrp)
                &&(portablePtr <  freeFramePoolObj->frmEndAddrp)) {
                /* Given frame belons to static frame.*/

                /* Find out the offset of the buffer in static frames to be able
                 * to update the reference count of the given frame.
                 */
                 i = ( (portablePtr - (freeFramePoolObj->frmStartAddrp))/
                      freeFramePoolObj->frmHdrBufSize);
                 /* Increment reference count by numDupedFrames */
                 freeFramePoolObj->frmRefCnt[i] += numDupedFrames;
                 lFrameIndex = i;
                /* Indicates original frame that is pointed to is
                 * static frame.
                 */
                indexType = FrameQBufMgr_ShMem_STATIC_FRAME;

            }
            else if (  (portablePtr >= freeFramePoolObj->hdrBufStartAddrp)
                     &&(portablePtr <  freeFramePoolObj->hdrBufEndAddrp))
                {
                /* Check if the given frame is a duped frame  with static
                 * header .
                 */
                i = ( (portablePtr - (freeFramePoolObj->hdrBufStartAddrp))/
                  freeFramePoolObj->frmHdrBufSize);

                /* Find out the original frame to which the given duped frame
                 * is referring to.
                 */
                refIndex = freeFramePoolObj->hdrBuf_refCntIndex[i];
                lFrameIndex = refIndex;
                /* Check if this Header is pointing to dynamic frame or
                 * static frame.
                 */
                if (freeFramePoolObj->hdrBuf_refCntIndexType[i] ==
                                              FrameQBufMgr_ShMem_STATIC_FRAME) {
                    freeFramePoolObj->frmRefCnt[refIndex] += numDupedFrames;
                    /* Indicates original frame that is pointed to is
                     * static frame.
                     */
                    indexType = FrameQBufMgr_ShMem_STATIC_FRAME;
                }
                else {
                    /* This header buffers is actually duped frame for frame
                     * whcih belongs dynamic frames of this queue
                     */
                    freeFramePoolObj->dynFrmRefCnt[refIndex] += numDupedFrames;
                    /* Indicates original frame that is pointed to is
                     * dynamic frame.
                     */
                    indexType = FrameQBufMgr_ShMem_DYNAMIC_FRAME;
                }

            }
            else {
                /* Check in dynamic frame list */
                for ( i = 0;
                      (i < freeFramePoolObj->numDynamicFrames);
                      i++) {
                    if (portablePtr ==  freeFramePoolObj->dynFrame[i]) {
                        /* Index of the Buffer to be freed is found. */
                        freeFramePoolObj->dynFrmRefCnt[i] += numDupedFrames;
                        bDuped = TRUE;
                        lFrameIndex = i;
                        /* Indicates original frame that is pointed to is
                         * dynamic frame.
                         */
                        indexType = FrameQBufMgr_ShMem_DYNAMIC_FRAME;

                    }
                }

                if (bDuped == FALSE) {
                    /* Check if given frame header is from dynamic frame
                     * headers
                     */
                    for ( i = 0;
                          (i < freeFramePoolObj->numDynamicHdrBufs);
                          i++) {
                        if (    portablePtr
                             == freeFramePoolObj->dynHdrBufs[i]) {
                            /* Find out the original frame to which the given duped frame
                             * is referring to.
                             */
                            refIndex =
                               freeFramePoolObj->dynHdrBuf_refCntIndex[i];
                            lFrameIndex = refIndex;
                            /* Check if this Header is pointing to dynamic frame or
                             * static frame and increment ref count of static
                             * frame or dynamic frame.
                             */
                            if (   (freeFramePoolObj->
                                    dynHdrBuf_refCntIndexType[i])
                                ==  (FrameQBufMgr_ShMem_STATIC_FRAME)) {
                                freeFramePoolObj->frmRefCnt[refIndex] +=
                                                                 numDupedFrames;
                                /* Indicates original frame that is pointed to
                                 * is static frame.
                                 */
                                indexType = FrameQBufMgr_ShMem_STATIC_FRAME;

                            }
                            else {
                                /* This header buffers is actually duped frame for frame
                                 * whcih belongs dynamic frames of this queue
                                 */
                                freeFramePoolObj->dynFrmRefCnt[refIndex]
                                                              += numDupedFrames;
                                /* Indicates original frame that is pointed to
                                 * is dynamic frame.
                                 */
                                indexType = FrameQBufMgr_ShMem_DYNAMIC_FRAME;

                            }

                        }
                    }
                } /*if (bDuped == FALSE) {*/
            }
            /* Now Allocate duped headers from the quque and copy the contents
             * of the orginal frame to duped frames  and  return
             */
            if (status >= 0) {
                for (k = 0;
                     (  (k < freeFramePoolObj->numFrmHdrBufs)
                      &&(ldupedFrames < numDupedFrames)
                      &&(freeFramePoolObj->freeFrmHdrBufs > 0));
                     k++) {
                    if (freeFramePoolObj->hdrBufIsUsed[k] != TRUE){
                        /* Found free header */
                        freeFramePoolObj->hdrBufIsUsed[k] = TRUE;

                        freeFramePoolObj->hdrBuf_refCntIndex[k] =
                                                                   lFrameIndex;
                        /* Set if it is pointing to static frame or
                         * dynamic frame.
                         */
                        freeFramePoolObj->hdrBuf_refCntIndexType[k] =
                                                                     indexType;
                        freeFramePoolObj->freeFrmHdrBufs--;
                        portablePtr = (UInt32)
                          (   freeFramePoolObj->hdrBufStartAddrp
                            + (  (   (  (freeFramePoolObj->hdrBufEndAddrp)
                                       -(freeFramePoolObj->hdrBufStartAddrp))
                                   / (freeFramePoolObj->numFrmHdrBufs))
                                * k) );

                        lframe = SyslinkMemMgr_translate(
                                obj->memMgrAllocater_hdrBufs,
                                (Ptr)portablePtr,
                                SyslinkMemMgr_AddrType_Portable,
                                SyslinkMemMgr_AddrType_Virtual);
                        /* Copy given frame header contens to this frame*/
                        memcpy(lframe, frame,  frame->headerSize);
                        dupedFramePtr[ldupedFrames] = lframe;
                        ldupedFrames++;
                    }

                }

                if (ldupedFrames < numDupedFrames) {
                    /* If still  need to allocate */
                    for (k = 0;
                        (  (k < freeFramePoolObj->numDynamicHdrBufs)
                         && (ldupedFrames < numDupedFrames)
                         && (freeFramePoolObj->freeDynamicHdrBufs > 0));
                        k++) {
                        if (freeFramePoolObj->dynHdrBufIsUsed[k] != TRUE){
                            /* Found free header */
                            freeFramePoolObj->dynHdrBufIsUsed[k] = TRUE;

                            freeFramePoolObj->dynHdrBuf_refCntIndex[k] =
                                                                    lFrameIndex;
                            /* Set if it is pointing to static frame or
                             * dynamic frame.
                             */
                            freeFramePoolObj->dynHdrBuf_refCntIndexType[k]
                                                                   = indexType;
                            freeFramePoolObj->freeDynamicHdrBufs--;
                            portablePtr = (UInt32)
                                          freeFramePoolObj->dynHdrBufs[k];

                            lframe = SyslinkMemMgr_translate(
                                    obj->memMgrAllocater_hdrBufs,
                                    (Ptr)portablePtr,
                                    SyslinkMemMgr_AddrType_Portable,
                                    SyslinkMemMgr_AddrType_Virtual);
                            /* Copy given frame header contens to this frame*/
                            memcpy(lframe, frame,  frame->headerSize);
                            dupedFramePtr[ldupedFrames] = lframe;
                            ldupedFrames++;
                        }
                    }
                }
            }
        }

        /* Writeback contents of the frame pool */
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_wbInv ((Ptr)freeFramePoolObj,
                         sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                         Cache_Type_ALL,
                         TRUE);
        }

    }

    /* Release lock */
    GateMP_leave(obj->gate, key);

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_dup", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_dupv ========
 *  Duplicate multiple frames
 */
Int32
FrameQBufMgr_ShMem_dupv (FrameQBufMgr_ShMem_Handle   handle,
                         FrameQBufMgr_ShMem_Frame    framePtr[],
                         FrameQBufMgr_ShMem_Frame  **dupedFramePtr,
                         UInt32                      numDupedFrames,
                         UInt32                      numFrames)
{
    Int32       status = FrameQBufMgr_S_SUCCESS;
    Bool        bDuped = FALSE;
    UInt32      ldupedFrames = 0;
    UInt8       indexType    = 0xFF;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj *freeFramePoolObj;
    FrameQBufMgr_ShMem_Frame lframe;
    UInt32      portablePtr;
    UInt8       index;
    UInt32      lFrameIndex;
    UInt32      lFrmCnt;
    UInt32      refIndex;
    UInt32      i;
    UInt32      k;
    Int32       key;
    FrameQBufMgr_ShMem_Obj    *obj;

    GT_5trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_dupv",
               handle,
               framePtr,
               dupedFramePtr,
               numDupedFrames,
               numFrames);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (framePtr != NULL));
    GT_assert (curTrace, (dupedFramePtr != NULL));
    GT_assert (curTrace, (numDupedFrames != 0));
    GT_assert (curTrace, (numFrames != 0));

    obj = handle->obj;
    GT_assert(curTrace, (obj != NULL));

    pAttrs = obj->attrs;
    /* Acquire lock */
    key = GateMP_enter(obj->gate);

    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)pAttrs,
                   sizeof (FrameQBufMgr_ShMem_Attrs),
                   Cache_Type_ALL,
                   TRUE);
    }

    /* Return error if header buffers  available are not sufficient*/
    for (lFrmCnt = 0; (lFrmCnt < numFrames) && (status >= 0); lFrmCnt++) {
        index = framePtr[lFrmCnt]->freeFrmQueueNo;
        /* Check if the frame is static frame or duped frame created from the static
         * headers or dynamic frame or frame with duped dynamic header.
         */
        if (index >= pAttrs->numFreeFramePools) {
            status = FrameQBufMgr_E_FAIL;
        }
        else {
            freeFramePoolObj = obj->freeFramePool[index];
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv ((Ptr)freeFramePoolObj,
                           sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                           Cache_Type_ALL,
                           TRUE);
            }
            /* Check if this queue has sufficient number of free header bufs to dup
             * the given frame.
             */
            if (    numDupedFrames
                  > (  freeFramePoolObj->freeFrmHdrBufs
                     + freeFramePoolObj->freeDynamicHdrBufs)) {
                 /* Can not satisfy the requirement with the available free header
                  * buffers.
                  */
                status = FrameQBufMgr_E_INSUFFICIENT_HDRBUFS;
            }
        }
    }

    if (status >= 0) {
        for (lFrmCnt = 0; lFrmCnt < numFrames; lFrmCnt++) {
            index = framePtr[lFrmCnt]->freeFrmQueueNo;
            ldupedFrames = 0;

            /* Convert frame addr to portable address to check  reference
             * counts etc.
             */
            portablePtr = (UInt32)SyslinkMemMgr_translate (
                                                   obj->memMgrAllocater_hdrBufs,
                                                   (Ptr)framePtr[lFrmCnt],
                                                    SyslinkMemMgr_AddrType_Virtual,
                                                    SyslinkMemMgr_AddrType_Portable);
            GT_assert(curTrace, (portablePtr != NULL));

            /* Check if frame  belongs to the frames created during create time
             */
            if (  (portablePtr >= freeFramePoolObj->frmStartAddrp)
                &&(portablePtr <  freeFramePoolObj->frmEndAddrp)) {
                /* Given frame belons to static frame.*/

                /* Find out the offset of the buffer in static frames to be able
                 * to update the reference count of the given frame.
                 */
                 i = ( (portablePtr - (freeFramePoolObj->frmStartAddrp))/
                      freeFramePoolObj->frmHdrBufSize);
                 /* Increment reference count by numDupedFrames */
                 freeFramePoolObj->frmRefCnt[i] += numDupedFrames;
                 lFrameIndex = i;
                /* Indicates original frame that is pointed to is
                 * static frame.
                 */
                indexType = FrameQBufMgr_ShMem_STATIC_FRAME;

            }
            else if (  (portablePtr >= freeFramePoolObj->hdrBufStartAddrp)
                     &&(portablePtr <  freeFramePoolObj->hdrBufEndAddrp))
                {
                /* Check if the given frame is a duped frame  with static
                 * header .
                 */
                i = ( (portablePtr - (freeFramePoolObj->hdrBufStartAddrp))/
                  freeFramePoolObj->frmHdrBufSize);

                /* Find out the original frame to which the given duped frame
                 * is referring to.
                 */
                refIndex = freeFramePoolObj->hdrBuf_refCntIndex[i];
                lFrameIndex = refIndex;
                /* Check if this Header is pointing to dynamic frame or
                 * static frame.
                 */
                if (freeFramePoolObj->hdrBuf_refCntIndexType[i] ==
                                              FrameQBufMgr_ShMem_STATIC_FRAME) {
                    freeFramePoolObj->frmRefCnt[refIndex] +=
                                                                numDupedFrames;
                    /* Indicates that original frame is
                     * static frame.
                     */
                    indexType = FrameQBufMgr_ShMem_STATIC_FRAME;
                }
                else {
                    /* This header buffers is actually duped frame for frame
                     * whcih belongs dynamic frames of this queue
                     */
                    freeFramePoolObj->dynFrmRefCnt[refIndex] +=
                                                               numDupedFrames;
                    /* Indicates that original frame  is
                     * dynamic frame.
                     */
                    indexType = FrameQBufMgr_ShMem_DYNAMIC_FRAME;
                }

            }
            else {
                /* Check in dynamic frame list */
                for ( i = 0;
                      (i < freeFramePoolObj->numDynamicFrames);
                      i++) {
                    if (portablePtr ==  freeFramePoolObj->dynFrame[i]) {
                        /* Index of the Buffer to be freed is found. */
                        freeFramePoolObj->dynFrmRefCnt[i] += numDupedFrames;
                        bDuped = TRUE;
                        lFrameIndex = i;
                        indexType = FrameQBufMgr_ShMem_DYNAMIC_FRAME;

                    }
                }

                if (bDuped == FALSE) {
                    /* Check if given frame header is from dynamic frame
                     * headers
                     */
                    for ( i = 0;
                          (i < freeFramePoolObj->numDynamicHdrBufs);
                          i++) {
                        if (    portablePtr
                             == freeFramePoolObj->dynHdrBufs[i]) {
                            /* Find out the original frame to which the given
                             * duped frame is referring to.
                             */
                            refIndex =
                                     freeFramePoolObj->dynHdrBuf_refCntIndex[i];
                            lFrameIndex = refIndex;
                            /* Check if this Header is pointing to dynamic frame
                             * or static frame and increment ref count of static
                             * frame or dynamic frame.
                             */
                            if (   (freeFramePoolObj->
                                    dynHdrBuf_refCntIndexType[i])
                                ==  (FrameQBufMgr_ShMem_STATIC_FRAME)) {
                                freeFramePoolObj->frmRefCnt[refIndex] +=
                                                                 numDupedFrames;
                                indexType = FrameQBufMgr_ShMem_STATIC_FRAME;

                            }
                            else {
                                /* This header buffers is actually duped frame for frame
                                 * whcih belongs dynamic frames of this queue
                                 */
                                freeFramePoolObj->dynFrmRefCnt[refIndex]
                                                              += numDupedFrames;
                                indexType = FrameQBufMgr_ShMem_DYNAMIC_FRAME;

                            }

                        }
                    }
                } /*if (bDuped == FALSE) {*/
            }
            /* Now Allocate  duped headers  from the quque and copy the contents
             * of the orginal frame to duped frames  and  return
             */
            if (status >= 0) {
                for (k = 0;
                     (  (k < freeFramePoolObj->numFrmHdrBufs)
                      &&(ldupedFrames < numDupedFrames)
                      &&(freeFramePoolObj->freeFrmHdrBufs > 0));
                     k++) {
                    if (freeFramePoolObj->hdrBufIsUsed[k] != TRUE){
                        /* Found free header */
                        freeFramePoolObj->hdrBufIsUsed[k] = TRUE;

                        freeFramePoolObj->hdrBuf_refCntIndex[k] =
                                                                   lFrameIndex;
                        /* Set if it is pointing to static frame or
                         * dynamic frame.
                         */
                        freeFramePoolObj->hdrBuf_refCntIndexType[k] =
                                                                     indexType;
                        freeFramePoolObj->freeFrmHdrBufs--;
                        portablePtr = (UInt32)
                          (   freeFramePoolObj->hdrBufStartAddrp
                            + (  (   (  (freeFramePoolObj->hdrBufEndAddrp)
                                       -(freeFramePoolObj->hdrBufStartAddrp))
                                   / (freeFramePoolObj->numFrmHdrBufs))
                                * k) );

                        lframe = SyslinkMemMgr_translate(
                                obj->memMgrAllocater_hdrBufs,
                                (Ptr)portablePtr,
                                SyslinkMemMgr_AddrType_Portable,
                                SyslinkMemMgr_AddrType_Virtual);
                        /* Copy given frame header contents to this frame*/
                        memcpy(lframe,
                               framePtr[lFrmCnt],
                               framePtr[lFrmCnt]->headerSize);
                        dupedFramePtr[lFrmCnt][ldupedFrames] = lframe;
                        ldupedFrames++;
                    }
                }

                if (ldupedFrames < numDupedFrames) {
                    /* If still  need to allocate */
                    for (k = 0;
                        (  (k < freeFramePoolObj->numDynamicHdrBufs)
                         &&(ldupedFrames < numDupedFrames)
                         &&(freeFramePoolObj->freeDynamicHdrBufs > 0));
                        k++) {
                        if (freeFramePoolObj->dynHdrBufIsUsed[k] != TRUE){
                            /* Found free header */
                            freeFramePoolObj->dynHdrBufIsUsed[k] = TRUE;

                            freeFramePoolObj->dynHdrBuf_refCntIndex[k] =
                                                                    lFrameIndex;
                            /* Set if it is pointing to static frame or
                             * dynamic frame.
                             */
                            freeFramePoolObj->dynHdrBuf_refCntIndexType[k]
                                                                   = indexType;
                            freeFramePoolObj->freeDynamicHdrBufs--;
                            portablePtr = (UInt32)
                                          freeFramePoolObj->dynHdrBufs[k];

                            lframe = SyslinkMemMgr_translate (
                                                   obj->memMgrAllocater_hdrBufs,
                                                   (Ptr)portablePtr,
                                                   SyslinkMemMgr_AddrType_Portable,
                                                   SyslinkMemMgr_AddrType_Virtual);
                            /* Copy  given frame header contens to this frame*/
                            memcpy(lframe,
                                   framePtr[lFrmCnt],
                                   framePtr[lFrmCnt]->headerSize);
                            dupedFramePtr[lFrmCnt][ldupedFrames] = lframe;
                            ldupedFrames++;
                        }

                    }/*for (k = 0;  */

                }/*if (ldupedFrames < numDupedFrames) */
            }
            /* Writeback contents of the frame pool */
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv ((Ptr)freeFramePoolObj,
                             sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                             Cache_Type_ALL,
                             TRUE);
            }
        }

    }

    /* Release lock */
    GateMP_leave ((GateMP_Handle)obj->gate, key);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_writeBack ========
 * Write back the contents  of a frame including frame buffers.
 *
 * NOTE: frame address and frame buffer address in the frame should be in
 *       virtual format.
 */
Int32
FrameQBufMgr_ShMem_writeBack(FrameQBufMgr_ShMem_Handle  handle,
                             FrameQBufMgr_ShMem_Frame   frame)
{
    Int32   status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_FrameBufInfo   *frameBufInfo;
    Ptr     addr;
    UInt32  i;
    FrameQBufMgr_ShMem_Obj  *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_writeBack",
               handle,
               frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame  != NULL));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);
    frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo*)&(frame->frameBufInfo[0]);

    for (i = 0; i < frame->numFrameBuffers; i++) {
        /* Write  back the buffer if cache flag  and cpu access flag is enabled.
         */
        if ((obj->frmBufCacheFlag[i]) && (obj->bufCpuAccessFlag[i])) {
            /* Write back only valid contents in frame buffer */
            addr = (Ptr)(   (UInt32)(frameBufInfo[i].bufPtr)
                          + (UInt32)(frameBufInfo[i].startOffset));
            Cache_wbInv(addr, frameBufInfo[i].validSize,
                    Cache_Type_ALL, TRUE);
            if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM) {
                frameBufInfo[i].bufPtr = (UInt32)SyslinkMemMgr_translate(
                        obj->memMgrAllocater_hdrBufs,
                        (Ptr)frameBufInfo[i].bufPtr,
                        SyslinkMemMgr_AddrType_Virtual,
                        SyslinkMemMgr_AddrType_Portable);
            }
        }
    }

    /* Write  back frame  if cache flag  is enabled.
     */
    if (obj->frmHdrBufCacheFlag == TRUE) {
        Cache_wbInv(frame, frame->headerSize, Cache_Type_ALL, TRUE);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_writeBack", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_invalidate ========
 *  Invalidate the contents of a frame
 */
Int32 FrameQBufMgr_ShMem_invalidate(FrameQBufMgr_ShMem_Handle handle,
        FrameQBufMgr_ShMem_Frame frame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_FrameBufInfo *frameBufInfo;
    Ptr addr;
    UInt32 i;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_invalidate",
               handle,
               frame);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frame  != NULL));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);
    frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo*) &(frame->frameBufInfo[0]);

    /* Write  back frame  if cache flag  is enabled.
     */
    if (obj->frmHdrBufCacheFlag == TRUE) {
        /* Do invalidation first one cache line */
        Cache_inv(frame,
                  FrameQBufMgr_CACHE_LINESIZE,
                  Cache_Type_ALL,
                  TRUE);
        Cache_inv(frame,
                  frame->headerSize,
                  Cache_Type_ALL,
                  TRUE);
    }

    for (i = 0; i < frame->numFrameBuffers; i++) {
        /* Write  back the buffer if cache flag  and cpu access flag is enabled.
         */
        if ((obj->frmBufCacheFlag[i]) && (obj->bufCpuAccessFlag[i])) {
            if(obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM){
                frameBufInfo[i].bufPtr = (UInt32)SyslinkMemMgr_translate (
                                     obj->memMgrAllocater_hdrBufs,
                                    (Ptr)frameBufInfo[i].bufPtr,
                                         SyslinkMemMgr_AddrType_Portable,
                                         SyslinkMemMgr_AddrType_Virtual);

                /* Write back only valid contents in frame buffer */
                addr = (Ptr)(   (UInt32)(frameBufInfo[i].bufPtr)
                              + (UInt32)(frameBufInfo[i].startOffset));
                Cache_inv(addr,
                          frameBufInfo[i].validSize,
                          Cache_Type_ALL,
                          TRUE);
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_invalidate", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_writeBackBuf ========
 *  Write back the contents of buffer
 */
Int32 FrameQBufMgr_ShMem_writeBackHeaderBuf(FrameQBufMgr_ShMem_Handle handle,
        Ptr headerBuf)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Frame frame = (FrameQBufMgr_ShMem_Frame)headerBuf;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_writeBackHeaderBuf",
               handle,
               headerBuf);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (headerBuf  != NULL));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);
    if (obj->frmHdrBufCacheFlag == TRUE) {
        Cache_wbInv(frame, frame->headerSize, Cache_Type_ALL, TRUE);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_writeBackHeaderBuf",
            status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_writeBackBuf ========
 *  Write back the contents of a buffer
 */
Int32 FrameQBufMgr_ShMem_invalidateHeaderBuf(FrameQBufMgr_ShMem_Handle handle,
        Ptr headerBuf)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Frame frame = (FrameQBufMgr_ShMem_Frame)headerBuf;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_invalidateHeaderBuf",
               handle,
               headerBuf);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (headerBuf  != NULL));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);
    if (obj->frmHdrBufCacheFlag == TRUE) {
        /* Do invalidation first one cache line */
        Cache_wbInv(frame, FrameQBufMgr_ShMem_CACHE_LINESIZE, Cache_Type_ALL,
                TRUE);
        Cache_wbInv(frame, frame->headerSize, Cache_Type_ALL, TRUE);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_invalidateHeaderBuf",
            status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_writeBackFrameBuf ========
 *  Write back the contents of a buffer
 */
Int32 FrameQBufMgr_ShMem_writeBackFrameBuf(FrameQBufMgr_ShMem_Handle handle,
        Ptr frameBuf, UInt32 size, UInt8 bufIndexInFrame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_writeBackFrameBuf",
               handle,
               frameBuf,
               size,
               bufIndexInFrame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frameBuf != NULL));
    GT_assert(curTrace, (8 > bufIndexInFrame));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);
    /* Find out the buffer type from the flags */
    if ((obj->frmBufCacheFlag[bufIndexInFrame] == TRUE)
            && (obj->bufCpuAccessFlag[bufIndexInFrame] == TRUE)) {
        Cache_wbInv(frameBuf, size, Cache_Type_ALL, TRUE);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_writeBackFrameBuf",
            status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_invalidateFrameBuf ========
 *  Invalidate the contents of a frame buffer
 */
Int32 FrameQBufMgr_ShMem_invalidateFrameBuf(FrameQBufMgr_ShMem_Handle handle,
        Ptr frameBuf, UInt32 size, UInt8 bufIndexInFrame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Obj   *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_invalidateFrameBuf",
               handle,
               frameBuf,
               size,
               bufIndexInFrame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frameBuf != NULL));
    GT_assert(curTrace, (8 > bufIndexInFrame));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);
    /* Find out the buffer type from the flags */
    if ((obj->frmBufCacheFlag[bufIndexInFrame] == TRUE)
            && (obj->bufCpuAccessFlag[bufIndexInFrame] == TRUE)) {
        Cache_inv(frameBuf, size, Cache_Type_ALL, TRUE);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_invalidateFrameBuf",
            status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_translateAddr ========
 *  Translate address between different types.
 */
Int32
FrameQBufMgr_ShMem_translateAddr(FrameQBufMgr_ShMem_Handle      handle,
                                  Ptr *                          dstAddr,
                                  FrameQBufMgr_ShMem_AddrType    dstAddrType,
                                  Ptr                            srcAddr,
                                  FrameQBufMgr_ShMem_AddrType    srcAddrType,
                                  FrameQBufMgr_ShMem_BufType     bufType)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    SyslinkMemMgr_Handle memMgrHandle = NULL;
    Ptr           addr;
    SyslinkMemMgr_AddrType        sAddrType;
    SyslinkMemMgr_AddrType        dAddrType;
    FrameQBufMgr_ShMem_Obj   *obj;

    GT_5trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_ShMem_translateAddr",
               dstAddr,
               dstAddrType,
               srcAddr,
               srcAddrType,
               bufType);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (dstAddr != NULL));
    GT_assert(curTrace, (srcAddr != NULL));

        obj = handle->obj;
        GT_assert(curTrace, obj != NULL);
    switch (bufType) {
        case FrameQBufMgr_ShMem_BUF_FRAMEHEADER:
            memMgrHandle = obj->memMgrAllocater_hdrBufs;
            break;

        case FrameQBufMgr_ShMem_BUF_FRAMEBUF:
            memMgrHandle = obj->memMgrAllocater_frmBufs;
            break;

        default:
            status = FrameQBufMgr_E_FAIL;
    }
    if ((status >= 0)) {
        if (dstAddrType == FrameQBufMgr_ShMem_AddrType_Virtual) {
            dAddrType = SyslinkMemMgr_AddrType_Virtual;
        }
        else {
            dAddrType = SyslinkMemMgr_AddrType_Portable;
        }

        if (srcAddrType == FrameQBufMgr_ShMem_AddrType_Virtual) {
            sAddrType = SyslinkMemMgr_AddrType_Virtual;
        }
        else {
            sAddrType = SyslinkMemMgr_AddrType_Portable;
        }

        addr = SyslinkMemMgr_translate (memMgrHandle,
                                        (Ptr)srcAddr,
                                        sAddrType,
                                        dAddrType);
        if (addr ==NULL) {
            status = FrameQBufMgr_E_FAIL;
            *dstAddr = (Ptr)NULL;
        }
        else {
            *dstAddr = (Ptr)addr;
        }
    }

    GT_1trace (curTrace,
              GT_LEAVE,
              "FrameQBufMgr_ShMem_translateAddr",
              status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_getId ========
 *  Get FrameQbufMgr Id
 */
UInt32 FrameQBufMgr_ShMem_getId(FrameQBufMgr_ShMem_Handle handle)
{
    GT_assert(curTrace, (handle != NULL));

    return (handle->obj->instId);
}

/*
 *  ======== FrameQBufMgr_ShMem_getHandle ========
 *  Get the FrameQBufMgr handle
 */
Ptr FrameQBufMgr_ShMem_getHandle(UInt32 Id)
{
    FrameQBufMgr_ShMem_Handle handle = NULL;
    UInt32 key;
    FrameQBufMgr_ShMem_Obj *obj;
    List_Elem *elem;
    UInt16 instId;
    UInt16 instNo;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_getHandle", Id);

    instId = Id & 0xFFFF;

    /* Sanity check the creator procid */
    GT_assert(curTrace, (((instId >> FRAMEQBUFMGR_CREATORPROCID_BITOFFSET) &
            FRAMEQBUFMGR_CREATORPROCID_MASK)
            < ti_sdo_utils_MultiProc_numProcessors));

    /* Get the instance no of the instance */
    instNo = ((instId >> FRAMEQBUFMGR_INSTNO_BITOFFSET) &
            FRAMEQBUFMGR_INSTNO_MASK);
    GT_assert(curTrace, (instNo < FrameQBufMgr_MAXINSTANCES));

    if (instNo < FrameQBufMgr_MAXINSTANCES) {
        key = IGateProvider_enter(FrameQBufMgr_ShMem_module->gate);

        for ((elem = List_next (FrameQBufMgr_ShMem_module->objList, NULL));
                elem != NULL;
                elem = List_next (FrameQBufMgr_ShMem_module->objList, elem)) {
            if (((FrameQBufMgr_ShMem_Obj *)elem)->instId == instId) {
                obj = ((FrameQBufMgr_ShMem_Obj *)elem);
                /* Check if we have created the GP or not */
                handle  = obj->top;
                break;
            }
        }

        IGateProvider_leave(FrameQBufMgr_ShMem_module->gate, key);
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_getHandle", handle);

    return (handle);
}

/*
 *  ======== FrameQBufMgr_ShMem_getNumFreeFrames ========
 *  Function to get the number of free frames available in
 *  FrameQbufMgr's FreeFramePool 0. The returned number free frame may not
 *  exist in the frameQbufMgr  after this call.This is because
 *  if any allocs are done by the other FrameQ writers on the
 *  same FrameQBufMgr.If frees are not other frameQ writers the free Frames
 *  would be even more than the value that we have got.
 *
 */
Int32 FrameQBufMgr_ShMem_getNumFreeFrames(FrameQBufMgr_ShMem_Handle handle,
        UInt32 *numFreeFrames)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    UInt32 freeFramePoolIndex = 0;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj *freeFramePoolObj;
    UInt32 key;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_assert(curTrace,(NULL != handle));
    GT_assert(curTrace, (NULL != numFreeFrames));

    obj = handle->obj;
    GT_assert(curTrace, obj != NULL);

    key = GateMP_enter(obj->gate);

    /* Alloc allocates frame from the free Frame Pool 0*/
    freeFramePoolIndex = 0;
    freeFramePoolObj =  obj->freeFramePool[freeFramePoolIndex];
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv ((Ptr)freeFramePoolObj,
                   sizeof(FrameQBufMgr_ShMem_FreeFramePoolObj),
                   Cache_Type_ALL,
                   TRUE);
    }

    * numFreeFrames = (  freeFramePoolObj->freeFrms
                       + freeFramePoolObj->freeDynamicFrames);
    GateMP_leave(obj->gate, key);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_getvNumFreeFrames ========
 * Function to get the number of free frames available in the plugged in
 * FrameQbufMgr.
 */
Int32
FrameQBufMgr_ShMem_getvNumFreeFrames (
                                FrameQBufMgr_ShMem_Handle   handle,
                                UInt32                        numFreeFrames[],
                                UInt8                         freeFramePoolNo[],
                                UInt8                         numFreeFramePools)
{
    Int32                              status           = FrameQBufMgr_S_SUCCESS;
    volatile FrameQBufMgr_ShMem_Attrs  *pAttrs          = NULL;
    UInt32                             freeFramePoolIndex;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj     *freeFramePoolObj;
    UInt32                             i;
    UInt32                             key;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != numFreeFrames));
    GT_assert(curTrace, (NULL != freeFramePoolNo));

    obj = handle->obj;
    GT_assert (curTrace, obj != NULL);

    pAttrs = obj->attrs;

    /* Acquire lock */
    key = GateMP_enter(obj->gate);

    /*  Invalidate control if required */
    if (obj->ctrlStructCacheFlag == TRUE) {
        Cache_inv((Ptr)pAttrs,
                  sizeof (FrameQBufMgr_ShMem_Attrs),
                  Cache_Type_ALL,
                  TRUE);
    }

    for (i = 0; i < numFreeFramePools; i++) {
        freeFramePoolIndex = freeFramePoolNo[i];
        /* Assert if pool no given is more than the configured pools*/
        GT_assert (curTrace,(freeFramePoolIndex < pAttrs->numFreeFramePools));
        freeFramePoolObj =  obj->freeFramePool[freeFramePoolIndex];
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv ((Ptr)freeFramePoolObj,
                       sizeof(FrameQBufMgr_ShMem_FreeFramePoolObj),
                       Cache_Type_ALL,
                       TRUE);
        }
        numFreeFrames[i] = (  freeFramePoolObj->freeFrms
                            + freeFramePoolObj->freeDynamicFrames);
    }
    /* Release lock */
    GateMP_leave(obj->gate, key);

    return (status);
}

/*
 *  ======== FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf ========
 * Function to find out if cache is enabled for header buffers.
 */
Bool FrameQBufMgr_ShMem_isCacheEnabledForHeaderBuf(
        FrameQBufMgr_ShMem_Handle handle)
{
    GT_assert (curTrace, (NULL != handle));

    return (handle->obj->frmHdrBufCacheFlag);
}

/*
 *  ======== FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf ========
 * Function to find out if cache is enabled for frame buffers.
 */
Bool FrameQBufMgr_ShMem_isCacheEnabledForFrameBuf(
        FrameQBufMgr_ShMem_Handle handle, UInt8 frameBufIndex)
{
    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (FrameQBufMgr_MAX_FRAMEBUFS > frameBufIndex));

    return (handle->obj->frmBufCacheFlag[frameBufIndex]);
}

/*
 *  ======== FrameQBufMgr_ShMem_control ========
 *  @brief  Provides a hook to perform implementation dependent operation.
 *
 */
Int32 FrameQBufMgr_ShMem_control(FrameQBufMgr_ShMem_Object *obj,
                            Int32                      cmd,
                            Ptr                        arg)
{
    Int32   status = FrameQBufMgr_S_SUCCESS;

    GT_assert (curTrace, (NULL != obj));

    status = FrameQBufMgr_E_NOTIMPLEMENTED;

    return (status);
}

#if 0
/******************************* Internal functions ***************************/
/*
 *  ======== FrameQBufMgr_ShMem_Module_startup ========
 */
Int FrameQBufMgr_ShMem_Module_startup(Int phase)
{
    Int32                        key;
    GateSwi_Handle               swiGate;
    Error_Block                  eb;

    if (FrameQBufMgr_ShMem_module->nameServer != NULL) {
        if (!ti_sdo_utils_NameServer_Module_startupDone()) {
            return (Startup_NOTDONE);
        }
    }
#if 0 // TODO - how do we handle startup in non-XDC
    /* Check the  implementation specific module setups */
    if (!FrameQBufMgr_Module_startupDone()) {
        return (Startup_NOTDONE);
    }
#endif

    Error_init(&eb);

    key = Gate_enterSystem();

    FrameQBufMgr_ShMem_module->refCount++;

    if(FrameQBufMgr_ShMem_module->refCount > 1) {
        Gate_leaveSystem(key);
        return (Startup_DONE);
    }
    else {
        /*Open the Name server created by FrameQ front end*/
        if (/*TODO??   (FrameQBufMgr_ShMem_module->cfg.useNameServer == TRUE)
                && */(FrameQBufMgr_ShMem_module->nameServer == NULL)) {
            /* Open  the Name Server instance created by Frond End FrameQBufMgr
             * module if not assigned statically.
             */
//            FrameQBufMgr_ShMem_module->nameServer = NameServer_getHandle(
//                                                   FrameQBufMgr_NAMESERVERNAME);
            FrameQBufMgr_ShMem_module->nameServer = gFrameQBufMgr_nsHandle;
            if(NULL == FrameQBufMgr_ShMem_module->nameServer) {

                /* Decrement the reference count */
                FrameQBufMgr_ShMem_module->refCount--;
                Gate_leaveSystem(key);

                return (Startup_NOTDONE);
            }
        }
    }/* if(FrameQBufMgr_ShMem_module->refCount > 1) { */

    /* Release lock */
    Gate_leaveSystem(key);

    if (FrameQBufMgr_ShMem_module->gate == NULL) {
        /*  Create local SWI gate  for module wide protection
         * if not created statically.
         */
        swiGate = GateSwi_create(NULL, &eb);
        if (swiGate != NULL) {
            FrameQBufMgr_ShMem_module->gate =
                                      GateSwi_Handle_upCast(swiGate);
            FrameQBufMgr_ShMem_module->gateType =
                                      FrameQBufMgr_ShMem_DYNAMIC_CREATE;
        }
        else {
            return (Startup_NOTDONE);
        }
    }

    return (Startup_DONE);
}
#endif

/*
 *  ======== _FrameQBufMgr_ShMem_create ========
 *  API to create/open the instance.Called internally by
 *  the FrameQBufMgr_create call.
 */
static inline
Int32
_FrameQBufMgr_ShMem_create(FrameQBufMgr_ShMem_Handle  handle,
                           const FrameQBufMgr_ShMem_Params * params,
                           Ptr arg)
{
    Int32                         status         = FrameQBufMgr_S_SUCCESS;
    UInt32                        i              = 0;
    volatile FrameQBufMgr_ShMem_Attrs * pAttrs   = NULL;
    UInt32                        addrOffset     = 0;
    UInt32                        virtAddr       = 0;
    UInt16                        instId         = 0;
    UInt                          align          = 4;
    Int32                         tmpStatus;
    ClientNotifyMgr_Params        clientMgrParams;
    ClientNotifyMgr_Handle        cliMgrHandle;
    UInt32                        regionId;
    SharedRegion_SRPtr            sharedShmBase;
    GateMP_Params                 gateParams;
    GateMP_Handle                 gateHandle;
    UInt32                        key;
    UInt8                         nameLen;
    FrameQBufMgr_NameServerEntry  nameserverEntry;
    FrameQBufMgr_NameServerEntry *pnameserverEntry;
    UInt32                        tempHdrChunk;
    UInt32                        tempBufChunk;
    UInt32                        j;
    UInt8                         instNo;
    UInt32                        gateMPsharedAddrSize;
    FrameQBufMgr_ShMem_Obj        *obj;
    Error_Block ebObj;
    Error_Block *eb = &ebObj;  /* locally used Error_Block */

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != params));

    /* Assert if shared addr provided is null and region id provide is
     * invalid.
     */
    GT_assert(curTrace, (params->sharedAddr != NULL) ||
            (params->regionId != SharedRegion_INVALIDREGIONID));

    /* Assert if FreeFramePools are zero */
    GT_assert(curTrace, (params->numFreeFramePools != 0));
    GT_assert(curTrace,
            (params->numFreeFramePools <= FrameQBufMgr_ShMem_MAX_POOLS));

    Error_init(eb);

    /* Assert if shared addr size is less than  shared memory requirement */
    FrameQBufMgr_ShMem_sharedMemReq(params,
                                    &tempHdrChunk,
                                    &tempBufChunk);
//    Assert_isTrue ( (   (params->sharedAddr != NULL)
//                    &&     params->sharedAddrSize
//                         >= FrameQBufMgr_ShMem_sharedMemReq(&sparams,
//                                                            &tempHdrChunk,
//                                                            &tempBufChunk)),
//                    FrameQBufMgr_ShMem_A_invalidParams);

    /* Validate the params */
    /* Check if Buffer address is given to create buffer pools*/
    if (params->sharedAddrHdrBuf != NULL) {
        if (params->sharedAddrHdrBufSize < tempHdrChunk) {
            status = FrameQBufMgr_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "_FrameQBufMgr_ShMem_create", status,
                    "sharedAddrHdrBufSize provided is zero!");
        }
        else {
            if ((((SizeT)params->sharedAddrHdrBuf) & (align-1)) != 0) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQBufMgr_ShMem_create", status,
                        "sharedAddrHdrBuf provided  is not aligned!");
            }
        }
    }

    if (params->sharedAddrFrmBuf != NULL) {
        if (params->sharedAddrFrmBufSize < tempBufChunk) {
            status = FrameQBufMgr_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "_FrameQBufMgr_ShMem_create", status,
                    "sharedAddrFrmBufSize provided is not sufficient!");
        }
        else {
            if ((((SizeT)params->sharedAddrFrmBuf) & (align-1)) != 0) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQBufMgr_ShMem_create", status,
                        "sharedAddrHdrBuf provided is not aligned!");
            }
        }
    }

    if (status >= 0) {
        /*
         * Ensure buffer Sizes specified in the bufSizes array
         * is multiple of align.
         */
        for (i = 0; i < params->numFreeFramePools; i++) {
            /* Ensure bufSizes[i] is a multiple of cache align.  */
            if ((((SizeT)params->frameHeaderBufSize[i])
                 % FrameQBufMgr_ShMem_CACHE_LINESIZE) != 0) {
                status = FrameQBufMgr_E_INVALIDARG;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQBufMgr_ShMem_create", status,
                        "frameHeaderBufSize provided is not cache aligned!");
                break;
            }
        }
    }

    if (status >= 0) {
        /* Ensure buffer Sizes specified in the bufSizes array
         * is multiple of align.
         */
        for (i = 0; (i < params->numFreeFramePools) && (status >= 0); i++) {
            for (j = 0; j < params->numFrameBufsInFrame[i]; j++) {
                /* Ensure bufSizes[i] is a multiple of cache align */
                if ((((SizeT)params->frameBufParams[i][j].size)
                     % FrameQBufMgr_ShMem_CACHE_LINESIZE) != 0) {
                    status = FrameQBufMgr_E_INVALIDARG;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "_FrameQBufMgr_ShMem_create", status,
                            "frameBufSizes provided is not cache aligned!");
                    break;
                }
            }
        }
    }
    if (status >= 0) {
        for (i = 0; i < params->numFreeFramePools; i++ ) {
            GT_assert(curTrace, (params->numFreeFramesInPool[i] <=
                    FrameQBufMgr_ShMem_POOL_MAXFRAMES));
            GT_assert(curTrace, (params->numFreeHeaderBufsInPool[i] <=
                    FrameQBufMgr_ShMem_POOL_MAXHDRS));
        }
    }

    if (status >= 0) {
        /*Call FrameQBufMgr API to get the unique instance number */
        status = FrameQBufMgr_getInstNo(&instNo);
        if (status >= 0) {
            obj = handle->obj;
            GT_assert (curTrace, obj != NULL);

            /* Found the  empty slot. This is the unique
             * instnace no for this instance on this processor
             * By appending creator proc id we get the unique
             * instance id for this instance in the entire system
             */
            instId = ( (   MultiProc_self()
                         & FRAMEQBUFMGR_CREATORPROCID_MASK)
                       << FRAMEQBUFMGR_CREATORPROCID_BITOFFSET)
                    + (   (instNo & FRAMEQBUFMGR_INSTNO_MASK)
                       << FRAMEQBUFMGR_INSTNO_BITOFFSET);

            /* Set the Id to default */
            obj->instId = (UInt32)-1;

            /* The memory pointed to by sharedAddr must be in shared memory and
             * the address must be mapped in all processors which require access
             * to this instance.
             */
            if (params->sharedAddr == NULL) {
                /* Allocate memory for controlStruct as user has not provided it
                 */
                /* Creating using a shared region ID and name */
                obj->regionId = params->regionId;
                obj->ctrlStructCacheFlag =
                                    SharedRegion_isCacheEnabled(obj->regionId);

                /* Need to allocate from the heap */
                obj->allocSize = FrameQBufMgr_ShMem_sharedMemReq(
                                             params,
                                             &tempHdrChunk,
                                             &tempBufChunk);
                /* HeapMemMP will do the alignment */
                virtAddr = (UInt32)Memory_calloc(
                                        SharedRegion_getHeap(obj->regionId),
                                        obj->allocSize,
                                        0,
                                        eb);
                GT_assert(curTrace, virtAddr != (UInt32)NULL);
            }
            else {
                /* Creating using sharedAddr */
                obj->regionId = SharedRegion_getId(params->sharedAddr);

                /* Assert that the buffer is in a valid shared region */
                GT_assert(curTrace, (obj->regionId != SharedRegion_INVALIDREGIONID));

                /* Assert that sharedAddr is cache aligned */
                GT_assert(curTrace, (UInt32)params->sharedAddr %
                             SharedRegion_getCacheLineSize(obj->regionId) == 0);

                /* set object's cacheEnabled, objType, and attrs  */
                obj->ctrlStructCacheFlag =
                                 SharedRegion_isCacheEnabled(obj->regionId);
                virtAddr = (UInt32)params->sharedAddr;
            }

            GT_assert(curTrace, (virtAddr != NULL));

            obj->pnameserverEntry = (FrameQBufMgr_NameServerEntry *)virtAddr;
            /*
             * Populate nameserver entry params that will be stored in
             * nameserver as well as at the start of the shared Addr .
             */
            nameserverEntry.ctrlInterfaceType =
                                   FrameQBufMgr_ShMem_INTERFACE_SHAREDMEM;
            nameserverEntry.headerInterfaceType =
                                   FrameQBufMgr_ShMem_HDRINTERFACE_SHAREDMEM;
            nameserverEntry.bufInterfaceType =
                                   params->bufInterfaceType;

            regionId = SharedRegion_getId (
                                          (Ptr)virtAddr);
            sharedShmBase = SharedRegion_getSRPtr ((Ptr)virtAddr, regionId);

            nameserverEntry.instParams.shMemParams.sharedAddr = (Ptr)
                                                              sharedShmBase;
            if (params->sharedAddr != NULL) {
                nameserverEntry.instParams.shMemParams.sharedAddrSize =
                                                         params->sharedAddrSize;
            }
            else {
                nameserverEntry.instParams.shMemParams.sharedAddrSize =
                                                         obj->allocSize;
            }

            /* Pass region id  to be used to be used
             * internally to allocate headers.
             * TODO:In case of Tiler what should  we pass here.
             */
            nameserverEntry.instParams.shMemParams.hdrBuf_regionId =
                                                        params->hdrBuf_regionId;

            /* Pass region id  to be used
             * internally to allocate  frame buffers.
             * TODO:In case of Tiler what should  we pass here.
             */
            nameserverEntry.instParams.shMemParams.frmBuf_regionId =
                                                        params->frmBuf_regionId;

            /*******************************************************************
             * Create instance gate based on the parameters if user has not
             * provided gate.
             *******************************************************************
             */
            if (params->gate != NULL) {
                obj->gate = params->gate;
                nameserverEntry.instParams.shMemParams.instGateMPAddr =
                    (Ptr)ti_sdo_ipc_GateMP_getSharedAddr(
                        (ti_sdo_ipc_GateMP_Handle)obj->gate);
                obj->creGate = FALSE;
            }
            else {
                if (FrameQBufMgr_ShMem_module->cfg.usedefaultgate == TRUE) {
                    obj->creGate = FALSE;
                    /* Check if FrameQ module has any default gate set. if not
                     * use GateMP's default gate.
                     */
                    if (FrameQBufMgr_ShMem_module->defaultInstGate != NULL) {
                        gateHandle = FrameQBufMgr_ShMem_module->defaultInstGate;
                    }
                    else {
                        /* Use the GateMP's default system gate */
                        gateHandle = GateMP_getDefaultRemote();
                        GT_assert(curTrace, (NULL != gateHandle));
                    }
                }
                else {
                    /* Set flag  indicaring user has not provided  GateMP */
                    obj->creGate = TRUE;
                    /*Create gateMP for this instance */
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
                                            obj->gateMPAllocSize,
                                            0,
                                            eb);
                    GT_assert(curTrace, (NULL != gateParams.sharedAddr));

                    gateHandle = GateMP_create (&gateParams);

                    GT_assert(curTrace, (NULL != gateHandle));
                    /* Store the instance gate control address in  name server so
                     * that openers can open it.
                     */
                }
                obj->gate = gateHandle;
                nameserverEntry.instParams.shMemParams.instGateMPAddr =
                    (Ptr)ti_sdo_ipc_GateMP_getSharedAddr(
                        (ti_sdo_ipc_GateMP_Handle)obj->gate);
            }
            /* Inserting nameserver entry params at the start of the shared
             * addr to allow open by  address.
             */
            pnameserverEntry = obj->pnameserverEntry;
            memcpy (pnameserverEntry,
                    &nameserverEntry,
                    sizeof (FrameQBufMgr_NameServerEntry));

            if (obj->ctrlStructCacheFlag == TRUE ) {
                Cache_wbInv ((Ptr)pnameserverEntry,
                             sizeof(FrameQBufMgr_NameServerEntry),
                             Cache_Type_ALL,
                             TRUE);
            }
            obj->minAlign = Memory_getMaxDefaultTypeAlign();

            if (SharedRegion_getCacheLineSize(obj->regionId) > obj->minAlign) {
                obj->minAlign = SharedRegion_getCacheLineSize(obj->regionId);
            }

            obj->ctrlInterfaceType  =
                           params->commonCreateParams.ctrlInterfaceType;
            obj->headerInterfaceType   =
                            params->headerInterfaceType;
            obj->bufInterfaceType   =
                            params->bufInterfaceType;

            obj->hdrBuf_regionId = params->hdrBuf_regionId;
            obj->frmBuf_regionId = params->frmBuf_regionId;
            /* Update cache flags based on shared region cache flags and
             * cpuAccessFlags for frame headers and frame buffers..
             */
            FrameQBufMgr_Shmem_updateCacheFlags(obj, params->cpuAccessFlags);

            /* Move addrOffset by cache aligned size of
             * FrameQ_NameServerEntry.
             */
            addrOffset += ROUND_UP(sizeof (FrameQBufMgr_NameServerEntry),
                                   obj->minAlign);

            obj->attrs = (FrameQBufMgr_ShMem_Attrs*) (virtAddr + addrOffset);

            pAttrs = obj->attrs;
            pAttrs->numNotifyEntries =  params->numNotifyEntries;

            /* Assuming FrameQBufMgr_ShMem_ControlStruct is
             * of cache aligned size
             */
            addrOffset += ROUND_UP(sizeof (FrameQBufMgr_ShMem_Attrs),
                                   obj->minAlign);
            obj->freeFramePool = Memory_alloc(NULL,
                                              (sizeof(UInt32) * params->numFreeFramePools),
                                              0,
                                              eb);
            GT_assert(curTrace, (NULL != obj->freeFramePool));

            for (i = 0; i < params->numFreeFramePools; i++) {
                obj->freeFramePool[i] = (FrameQBufMgr_ShMem_FreeFramePoolObj*)
                                                        (virtAddr + addrOffset);
                addrOffset += ROUND_UP(sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                                       obj->minAlign);
            }

            ClientNotifyMgr_Params_init(&clientMgrParams);
            clientMgrParams.numNotifyEntries = params->numNotifyEntries;
            clientMgrParams.numSubNotifyEntries = params->numFreeFramePools;
           /*
            * Pass the shared memory address to create the client
            * manager instance. This address stars right after the
            * size of (FrameQBufMgr_ShMem_ControlStruct) from the
            * params->sharedAddr.
            */
            clientMgrParams.sharedAddr = (Ptr)( virtAddr + addrOffset );
            clientMgrParams.sharedAddrSize =
                    ClientNotifyMgr_sharedMemReq (
                           (ClientNotifyMgr_Params*)&clientMgrParams );

            addrOffset += ROUND_UP(clientMgrParams.sharedAddrSize,
                                   obj->minAlign);
            if (params->gate != NULL) {
                clientMgrParams.gate = (Ptr) params->gate;
                /* Make it null so that finalization won't delete it.
                 */
                nameserverEntry.instParams.shMemParams.cliGateMPAddr =
                    (Ptr)ti_sdo_ipc_GateMP_getSharedAddr(
                        (ti_sdo_ipc_GateMP_Handle)clientMgrParams.gate);

                obj->clientNotifyMgrGate = params->gate;
                obj->creCliGate = FALSE;
            }
            else {
                if (FrameQBufMgr_ShMem_module->cfg.usedefaultgate ==
                        TRUE ) {
                    clientMgrParams.gate = (Ptr) gateHandle;
                    /* Make it null so that finalization won't delete it.
                     */
                    nameserverEntry.instParams.shMemParams.cliGateMPAddr =
                        (Ptr)ti_sdo_ipc_GateMP_getSharedAddr(
                            (ti_sdo_ipc_GateMP_Handle)clientMgrParams.gate);
                    obj->clientNotifyMgrGate = NULL;
                    obj->creCliGate = FALSE;
                }
                else {
                    /*Create gateMP for the clientNotifyMgr */
                    obj->creCliGate = TRUE;
                    GateMP_Params_init(&gateParams);
                    gateParams.localProtect = (GateMP_LocalProtect)params->localProtect;
                    gateParams.remoteProtect = (GateMP_RemoteProtect)
                                                              params->remoteProtect;
                    gateParams.regionId = obj->regionId;
                    gateParams.sharedAddr  = (Ptr)(virtAddr + addrOffset );
                    gateMPsharedAddrSize = GateMP_sharedMemReq(&gateParams);

                    addrOffset += ROUND_UP(gateMPsharedAddrSize,
                                           obj->minAlign);
                    gateHandle = GateMP_create (&gateParams);

                    GT_assert(curTrace, (NULL != gateHandle));
                    if (gateHandle == NULL) {
                        status = FrameQBufMgr_E_CREATE_GATEMP;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "_FrameQBufMgr_ShMem_create", status,
                                "Unable to create gate!");
                    }
                    else {
                        clientMgrParams.gate = (Ptr) gateHandle;
                        nameserverEntry.instParams.shMemParams.
                           cliGateMPAddr = (Ptr)SharedRegion_invalidSRPtr();
                        obj->clientNotifyMgrGate = gateHandle;
                    }
                }
            }

            if (status >= 0) {
                /* ClientNotifyMgr instance creation and  notification
                 * related stuff.
                 */
                clientMgrParams.numNotifyEntries = pAttrs->numNotifyEntries;
                /* Set the sub notify entries equqvent to number frame pools
                 */
                clientMgrParams.numSubNotifyEntries =
                                                  params->numFreeFramePools;
                clientMgrParams.name =
                                    (String)params->commonCreateParams.name;
                /* Event no to be used for this instance */
                clientMgrParams.eventNo =
                                 FrameQBufMgr_ShMem_NOTIFY_RESERVED_EVENTNO;
                /* This is actually create call */
                clientMgrParams.openFlag = FALSE;
                if (obj->ctrlStructCacheFlag == TRUE) {
                    clientMgrParams.cacheFlags =
                                               ClientNotifyMgr_CONTROL_CACHEUSE;
                }
                cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);
                GT_assert(curTrace, (NULL != cliMgrHandle));

                if (NULL == cliMgrHandle) {
                    status = FrameQBufMgr_E_FAIL_CLIENTN0TIFYMGR_CREATE;
                    GT_setFailureReason (curTrace, GT_4CLASS,
                            "_FrameQBufMgr_ShMem_create", status,
                            "ClientNotifyMgr create failed!");
                }
               else  {
                    /*********************************************************
                     *Memory manager allocaters handling
                     *********************************************************
                     */
                    tmpStatus = _FrameQBufMgr_ShMem_openHdrBufMemMgr(
                                                         obj,
                                                         pnameserverEntry);
                    if (tmpStatus < 0) {
                        status = tmpStatus;
                        /* Pass the generated error up  */
                    }
                    else {
                        tmpStatus = _FrameQBufMgr_ShMem_openFrmBufMemMgr(
                                                             obj,
                                                             pnameserverEntry);
                        if (tmpStatus < 0) {
                            status = tmpStatus;
                            /* Pass the generated error up  */
                        }
                        else {
                            obj->clientNotifyMgrHandle = cliMgrHandle;
                            if (params->sharedAddr) {
                                obj->objType   =
                                              FrameQBufMgr_ShMem_DYNAMIC_CREATE;
                            }
                            else {
                                /* set the object type indicating it is created
                                 * using Shared Region id internally.
                                 */
                                obj->objType  =
                                   FrameQBufMgr_ShMem_DYNAMIC_CREATE_USEDREGION;
                            }

                            obj->isRegistered   = FALSE;
                            obj->notifyId       = (UInt32)-1;
                            obj->instId = pAttrs->entryId = instId;
                            strcpy((String)pAttrs->name ,
                                   (String)params->commonCreateParams.name);
                            pAttrs->name[31] = '\0';

                            strcpy((Ptr)obj->name ,
                                   (const Char*)pAttrs->name);

                            pAttrs->version = FrameQBufMgr_ShMem_VERSION;
                            pAttrs->numFreeFramePools =
                                                      params->numFreeFramePools;
                            pAttrs->localProtect = params->localProtect;
                            pAttrs->remoteProtect = params->remoteProtect;
                            obj->refCount = 1;

                            /* Populate Bufpools if buffer chunks are not
                             * provided This API allocates buffers
                             */
                            status = FrameQBufMgr_ShMem_postInit (obj,
                                                                  params,
                                                                  eb);
                            if (status >= 0) {
                               /* Add name to the FrameQBufMgr name server
                                * instance.
                                * The Name server instance is created in  module
                                * initialization  phase.
                                */
                                key = GateMP_enter(obj->gate);
                                nameLen =  strlen(
                                       (String)params->commonCreateParams.name);
                                if (   (   NULL
                                       != FrameQBufMgr_ShMem_module->nameServer)
                                    && (nameLen > 0)) {
                                    /* Populate the nameserver entry  with the
                                     * params.
                                     */
                                    obj->nsKey = NameServer_add ((NameServer_Handle)
                                        FrameQBufMgr_ShMem_module->nameServer,
                                        (String)params->commonCreateParams.name,
                                        &nameserverEntry,
                                         sizeof(FrameQBufMgr_NameServerEntry));
                                    if (obj->nsKey == NULL) {
                                        status = FrameQBufMgr_E_NAMESERVERADD;
                                        GT_assert(curTrace,
                                                (NULL != obj->nsKey));
                                    }
                                }
                                GateMP_leave ((GateMP_Handle)obj->gate, key);
                            }
                        }
                    }
                }
            }
            if (status < 0) {
                status = FrameQBufMgr_freeInstNo (instNo) ;
            }
        }
    }
    return (status);
}

static inline  void
_FrameQBufMgr_ShMem_cleanupCreatedInstance(FrameQBufMgr_ShMem_Handle  handle)
{
    FrameQBufMgr_ShMem_Obj        *obj;
    FrameQBufMgr_ShMem_FreeFramePoolObj * freeFramePoolObj;
    IArg key;
    UInt16  instId;
    UInt32  i, j, k;
    UInt8   instNo;
    Ptr                              portableAddr;
    FrameQBufMgr_ShMem_Frame         frame;
    FrameQBufMgr_ShMem_FrameBufInfo *frameBufInfo  = NULL;

    if ((handle != NULL) &&(handle->obj != NULL)) {
        obj= handle->obj;

        if (obj->gate != NULL) {
            key = GateMP_enter (obj->gate);
        }

        if (obj->attrs != NULL) {
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_inv ((Ptr)obj->attrs,
                           sizeof (FrameQBufMgr_ShMem_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }
            if (obj->freeFramePool) {

                for (i = 0; i < obj->attrs->numFreeFramePools ; i++) {

                    freeFramePoolObj = (FrameQBufMgr_ShMem_FreeFramePoolObj *)
                                                      obj->freeFramePool[i];
                    if (freeFramePoolObj != NULL) {
                        /* Get the frame pointer in virtual format and
                         * free the tiler buffers in case frame buffer
                         * is  from tiler
                         */
                        for (j = 0; j < freeFramePoolObj->numFrms; j++) {

                            portableAddr = (Ptr)
                                    (freeFramePoolObj->frmStartAddrp +
                                      ( ( ( (freeFramePoolObj->frmEndAddrp)
                                       -  (freeFramePoolObj->frmStartAddrp))
                                       / freeFramePoolObj->numFrms)* j) );

                            frame = SyslinkMemMgr_translate (
                                        obj->memMgrAllocater_hdrBufs,
                                        (Ptr)portableAddr,
                            SyslinkMemMgr_AddrType_Portable,
                                        SyslinkMemMgr_AddrType_Virtual);
                            GT_assert(curTrace, (frame != NULL));

                            /* Convert frame buffer addresses from
                             * portable format to virtual address before
                             * returning free frame to the caller.
                             */
                            frameBufInfo = (FrameQBufMgr_ShMem_FrameBufInfo *)
                                    &(frame->frameBufInfo[0]);

                            for(k = 0;k < frame->numFrameBuffers; k++) {

                                if(obj->bufInterfaceType ==
                                      FrameQBufMgr_ShMem_BUFINTERFACE_TILERMEM){

                                    if (frameBufInfo[k].bufPtr != NULL) {

                                        SyslinkMemMgr_free(
                                                obj->memMgrAllocater_frmBufs,
                                                (Ptr)frameBufInfo[k].bufPtr,
                                                frameBufInfo[k].bufSize);
                                    }
                                }
                            }
                            /*Write back the contents of this frame */
                            if (obj->frmHdrBufCacheFlag == TRUE) {
                                Cache_wbInv((Ptr)frame,
                                       frame->baseHeaderSize,
                                       TRUE,
                                       NULL);
                            }
                        }
                        memset((Ptr)freeFramePoolObj,
                               sizeof(FrameQBufMgr_ShMem_FreeFramePoolObj), 0);

                        if (obj->ctrlStructCacheFlag == TRUE) {
                            Cache_wbInv ((Ptr)freeFramePoolObj,
                                         sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                                         Cache_Type_ALL,
                                         TRUE);
                        }
                    }
                }
                /* Free the allocated memory for obj->freeFramePool pointers */
                Memory_free (NULL,
                             obj->freeFramePool,
                             sizeof(UInt32)* obj->attrs->numFreeFramePools);
            }
            /* Delete the created ListMP objects  for bufpools */
            obj->attrs->status = !FrameQBufMgr_ShMem_CREATED;
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv ((Ptr)obj->attrs,
                             sizeof (FrameQBufMgr_ShMem_Attrs),
                             Cache_Type_ALL,
                             TRUE);
            }
        }
        /* Delete id from the module wide shared info */
        if (obj->instId != (UInt32)-1) {

            instId = obj->instId & 0xFFFF;

            /* Sanity check the creator procid */
            GT_assert (curTrace, ((
                    (instId >> FRAMEQBUFMGR_CREATORPROCID_BITOFFSET)
                    & FRAMEQBUFMGR_CREATORPROCID_MASK)
                    < MultiProc_getNumProcessors()));

            instNo = ( (instId >> FRAMEQBUFMGR_INSTNO_BITOFFSET)
                      & FRAMEQBUFMGR_INSTNO_MASK);
            GT_assert (curTrace, (instNo < FrameQBufMgr_MAXINSTANCES));
            FrameQBufMgr_freeInstNo(instNo);
        }
        /* Remove entry from NameServer */
        if (   (NULL != FrameQBufMgr_ShMem_module->nameServer)
            && (obj->nsKey != 0)) {
            NameServer_removeEntry(FrameQBufMgr_ShMem_module->nameServer,
                                   obj->nsKey);
        }
        /*Unregister the notification with the clientNotifyMgr */
        if (obj->isRegistered == TRUE) {
            ClientNotifyMgr_unregisterClient (obj->clientNotifyMgrHandle,
                                              obj->notifyId);
        }

        if (obj->clientNotifyMgrHandle != NULL) {
            ClientNotifyMgr_delete (&(obj->clientNotifyMgrHandle));
        }

        /* Delete GateMP instance of ClientNotifyMgr.*/
        if ((obj->clientNotifyMgrGate != NULL)&&(obj->creCliGate == TRUE)) {
            GateMP_delete ((GateMP_Handle*)&obj->clientNotifyMgrGate);
        }
        GateMP_leave (obj->gate, key);

        /* Free the allocated memory for the instance control structure
         */
        if (obj->objType == FrameQBufMgr_ShMem_DYNAMIC_CREATE_USEDREGION) {
            if ((obj->pnameserverEntry) && (obj->allocSize)) {
                Memory_free(SharedRegion_getHeap(obj->regionId),
                            obj->pnameserverEntry,
                            obj->allocSize);
            }
        }
        /* Free up the allocated frame header buffers and
         * frame buffers if they are allocated from shared region.
         */
        if ((obj->hdrBufPtr != NULL) && (obj->hdrBufAllocSize > 0)) {
            Memory_free(SharedRegion_getHeap(obj->hdrBuf_regionId),
                        obj->hdrBufPtr,
                        obj->hdrBufAllocSize);
        }
        if ((obj->frmBufPtr != NULL) && (obj->frmBufAllocSize > 0)) {
            Memory_free(SharedRegion_getHeap(obj->frmBuf_regionId),
                        obj->frmBufPtr,
                        obj->frmBufAllocSize);
        }
        /* Delete instance gate if it is opened internally
         * in the instance.
         */
        if ((obj->creGate == TRUE) && (obj->gate != NULL)) {
            GateMP_delete ((GateMP_Handle*)&obj->gate);
            /* Free the allocated memory for the instance control
             * structure.
             */
            Memory_free(SharedRegion_getHeap(obj->regionId),
                        obj->instGateSharedAddr,
                        obj->gateMPAllocSize);
        }
        /* Delete created SyslinkMemMgrs */
        if (obj->memMgrAllocater_hdrBufs != NULL) {
            SyslinkMemMgr_delete (
                          (SyslinkMemMgr_Handle*)&obj->memMgrAllocater_hdrBufs);
        }

        if (obj->memMgrAllocater_frmBufs != NULL) {
            SyslinkMemMgr_delete (
                          (SyslinkMemMgr_Handle*)&obj->memMgrAllocater_frmBufs);
        }
    }
}


/*
 *  ======== _FrameQBufMgr_ShMem_open ========
 *  Internal API to open the instance.Called internally by
 *  the FrameQBufMgr_ShMem_instance_init() call.
 */
static inline
Int32 _FrameQBufMgr_ShMem_open(FrameQBufMgr_ShMem_Handle handle,
        const FrameQBufMgr_ShMem_Params *params, Ptr arg)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    volatile FrameQBufMgr_ShMem_Attrs *pAttrs = NULL;
    UInt32                        addrOffset     = 0;
    UInt32                        virtAddr       = 0;
    Int32                         tmpStatus;
    ClientNotifyMgr_Params        clientMgrParams;
    ClientNotifyMgr_Handle        cliMgrHandle;
    UInt32                        regionId;
    GateMP_Params                 gateParams;
    GateMP_Handle                 gateHandle;
    FrameQBufMgr_NameServerEntry  nameserverEntry;
    FrameQBufMgr_NameServerEntry *pnameserverEntry;
    Ptr                           localAddr;
    UInt32                        len;
    UInt32                        key;
    UInt32                        i;
    FrameQBufMgr_ShMem_Obj        *obj = NULL;
    Error_Block ebObj;
    Error_Block *eb = &ebObj;  /* locally used Error_Block */

    GT_assert(curTrace, (NULL != (String)params->commonCreateParams.name));

    obj = handle->obj;

    /* Open the already created instance */
    if ((String)params->commonCreateParams.name != NULL) {
        GT_assert(curTrace, (FrameQBufMgr_ShMem_MAXNAMELEN
                >= (strlen((String)params->commonCreateParams.name) + 1)));
        GT_assert(curTrace, (NULL != FrameQBufMgr_ShMem_module->nameServer));

        len = sizeof (FrameQBufMgr_NameServerEntry);
        status = NameServer_get(FrameQBufMgr_ShMem_module->nameServer,
                (String)params->commonCreateParams.name, &nameserverEntry,
                &len, NULL);
        if (status == NameServer_E_NOTFOUND) {
            /* Name not found. */
            /* Failed to get the information from the name server.
             * It means it is not created.
             */
            status = FrameQBufMgr_E_NOTFOUND;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "_FrameQBufMgr_ShMem_open", status, "Name not found!");
        }
        else if (status == NameServer_E_FAIL) {
            /* Error happened in NameServer. Pass the error up. */
            status = FrameQBufMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "_FrameQBufMgr_ShMem_open", status,
                    "NameServer failure!");
        }
        else {
            virtAddr = (UInt32) SharedRegion_getPtr (
                                      (SharedRegion_SRPtr)
                 nameserverEntry.instParams.shMemParams.sharedAddr);
            pnameserverEntry = (FrameQBufMgr_NameServerEntry*)virtAddr;
            /* Assert that sharedAddr is cache aligned */
            regionId = SharedRegion_getId(pnameserverEntry);

            obj->regionId = regionId;
            /* Assert that the buffer is in a valid shared region */
            GT_assert(curTrace, (regionId != SharedRegion_INVALIDREGIONID));

            GT_assert(curTrace, ((UInt32)pnameserverEntry %
                    SharedRegion_getCacheLineSize(regionId) == 0));
        }
    }
    else if (params->sharedAddr != NULL) {
        /* Unable to get information from name server. But user has
         * provided  virtual shared address. So proceeding further.
         */
        virtAddr    = (UInt32) params->sharedAddr;
        pnameserverEntry = (FrameQBufMgr_NameServerEntry*)virtAddr;
        /* Invalidate the  FrameQBufMgr entry info located at the start of
         * the sharedAddr.
         */
        /* Assert that sharedAddr is cache aligned */
        regionId = SharedRegion_getId(pnameserverEntry);
        obj->regionId = regionId;
        /* Assert that the buffer is in a valid shared region */
        GT_assert(curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        GT_assert(curTrace, ((UInt32)pnameserverEntry %
                SharedRegion_getCacheLineSize(regionId) == 0));

        if (SharedRegion_isCacheEnabled(regionId)) {
            Cache_inv((Ptr)pnameserverEntry,
                      ROUND_UP (
                              sizeof (FrameQBufMgr_NameServerEntry),
                              SharedRegion_getCacheLineSize(obj->regionId)),
                      Cache_Type_ALL,
                      TRUE);
        }
        memcpy(&nameserverEntry,
               pnameserverEntry,
               sizeof(FrameQBufMgr_NameServerEntry));
        status      = FrameQBufMgr_S_SUCCESS;
    }
    else {
        /*params->sharedAddr is null and also virtaddr got from the
         * name server is also not valid.  So rejecting the open
         * call.
         */
        status = FrameQBufMgr_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "_FrameQBufMgr_ShMem_open", status,
                "params->sharedAddr is null!");
    }

    if (status >= 0) {
        GT_assert(curTrace, (nameserverEntry.instParams.shMemParams.
                instGateMPAddr != (Ptr)SharedRegion_invalidSRPtr()));

        if (nameserverEntry.instParams.shMemParams.instGateMPAddr !=
                (Ptr)SharedRegion_invalidSRPtr()) {
            /* Get the local address of the SRPtr */
            localAddr = SharedRegion_getPtr((SharedRegion_SRPtr)
                    nameserverEntry.instParams.shMemParams.instGateMPAddr);

            /* Open gateMP using the shared Addr of the gateMP created
             * by the  FrameQ creator
             */
            tmpStatus = GateMP_openByAddr(localAddr, &gateHandle);
            if (tmpStatus != GateMP_S_SUCCESS) {
                obj->creGate = FALSE;
                status = FrameQBufMgr_E_OPEN_GATEMP;
                GT_setFailureReason (curTrace, GT_4CLASS,
                        "_FrameQBufMgr_ShMem_open", status,
                        "GateMP_openByAddr failed!");
            }
            else {
                obj->gate    = gateHandle;
                obj->creGate = TRUE;
            }
        }
    }

    Error_init(eb);

    if (status >= 0) {
        /* Set the Id to invalid */
        obj->instId = (UInt32)-1;
        obj->minAlign = Memory_getMaxDefaultTypeAlign();

        if (SharedRegion_getCacheLineSize(obj->regionId) > obj->minAlign) {
            obj->minAlign = SharedRegion_getCacheLineSize(obj->regionId);
        }

        obj->ctrlInterfaceType     =
                       nameserverEntry.ctrlInterfaceType;
        obj->headerInterfaceType   =
                        nameserverEntry.headerInterfaceType;
        obj->bufInterfaceType      =
                        nameserverEntry.bufInterfaceType;

        obj->hdrBuf_regionId =
                        nameserverEntry.instParams.shMemParams.hdrBuf_regionId;
        obj->frmBuf_regionId =
                        nameserverEntry.instParams.shMemParams.frmBuf_regionId;
        /* Update the control cache flag */
        obj->ctrlStructCacheFlag =
                    SharedRegion_isCacheEnabled(obj->regionId);

        /* Update cache flags based on shared region cache flags and
         * cpuAccessFlags.
         */
        FrameQBufMgr_Shmem_updateCacheFlags(obj,
                                            params->cpuAccessFlags);

        /* Move offset by size of nameserverEntry  cache aligned size.
         * No need to check interface type as it is checked by front end
         * module(FrameQBufMgr).
         */
        obj->pnameserverEntry = (FrameQBufMgr_NameServerEntry*)virtAddr;

        addrOffset += ROUND_UP(sizeof (FrameQBufMgr_NameServerEntry),
                              obj->minAlign);

        obj->attrs = (FrameQBufMgr_ShMem_Attrs*) (virtAddr + addrOffset);

        addrOffset += ROUND_UP(sizeof (FrameQBufMgr_ShMem_Attrs),
                              obj->minAlign);

        /* Enter the gate */
        key = GateMP_enter(obj->gate);

         /*Invalidate the  virtAddr contents */
        pAttrs = obj->attrs;
        if (obj->ctrlStructCacheFlag == TRUE) {
            Cache_inv((Ptr)obj->pnameserverEntry,
                      sizeof (FrameQBufMgr_NameServerEntry),
                      Cache_Type_ALL,
                      TRUE);
            Cache_inv((Ptr)pAttrs,
                      sizeof (FrameQBufMgr_ShMem_Attrs),
                      Cache_Type_ALL,
                      TRUE);
        }
        /* Make sure the instance has been created */
        if (pAttrs->status != FrameQBufMgr_ShMem_CREATED) {
            status = FrameQBufMgr_E_NOTFOUND;
        }
        else {
            obj->freeFramePool = Memory_alloc(NULL,
                                              (sizeof(UInt32) * pAttrs->numFreeFramePools),
                                              0,
                                              eb);
            if (obj->freeFramePool != NULL) {
                for (i = 0; i < pAttrs->numFreeFramePools; i++) {
                    obj->freeFramePool[i] = (FrameQBufMgr_ShMem_FreeFramePoolObj*)
                            (virtAddr + addrOffset);
                    addrOffset += ROUND_UP(sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                            obj->minAlign);
                }

                ClientNotifyMgr_Params_init(&clientMgrParams);
                clientMgrParams.numNotifyEntries = pAttrs->numNotifyEntries;
                clientMgrParams.numSubNotifyEntries = pAttrs->numFreeFramePools;
                clientMgrParams.sharedAddr = (Ptr)(virtAddr + addrOffset);
                clientMgrParams.sharedAddrSize = ClientNotifyMgr_sharedMemReq(
                        (ClientNotifyMgr_Params*)&clientMgrParams);
                addrOffset += ROUND_UP(clientMgrParams.sharedAddrSize,obj->minAlign);
                /* cliGateMPAddr if it valid means user has passed the
                 * gate to create call or  default gate is used in create call.
                 */
                if (   nameserverEntry.instParams.shMemParams.cliGateMPAddr
                    != (Ptr)SharedRegion_invalidSRPtr()) {
                    /* Get the local address of the SRPtr */
                    localAddr = SharedRegion_getPtr((SharedRegion_SRPtr)
                            nameserverEntry.instParams.shMemParams.cliGateMPAddr);
                    GT_assert(curTrace, (localAddr != NULL));

                    tmpStatus = GateMP_openByAddr(localAddr, &gateHandle);
                    if (tmpStatus != GateMP_S_SUCCESS) {
                        status = FrameQBufMgr_E_OPEN_GATEMP;
                        obj->creCliGate = FALSE;
                        /* Make it null so that finalization won't delete it.
                         */
                        obj->clientNotifyMgrGate = NULL;
                    }
                    else {
                        clientMgrParams.gate = (Ptr)gateHandle;
                        obj->clientNotifyMgrGate  = gateHandle;
                        obj->creCliGate = TRUE;
                    }
                }
                else {
                    /* Open the gate created for the ClientNotifyMgr instance*/
                    obj->creCliGate = TRUE;
                    GateMP_Params_init(&gateParams);

                    gateParams.localProtect  =
                                  (GateMP_LocalProtect)pAttrs->localProtect;
                    gateParams.remoteProtect =
                                    (GateMP_RemoteProtect)pAttrs->remoteProtect;
                    gateParams.regionId = obj->regionId;
                    gateParams.sharedAddr  = (Ptr)(virtAddr + addrOffset );

                    addrOffset += ROUND_UP(GateMP_sharedMemReq(&gateParams),
                                           obj->minAlign);
                    tmpStatus = GateMP_openByAddr(gateParams.sharedAddr,
                                              &gateHandle);
                    if (tmpStatus != GateMP_S_SUCCESS) {
                        /* Error happened in GateMP */
                        obj->clientNotifyMgrGate = NULL;
                        status = FrameQBufMgr_E_OPEN_GATEMP;
                        GT_setFailureReason (curTrace, GT_4CLASS,
                                "_FrameQBufMgr_ShMem_open", status,
                                "GateMP_openByAddr failed!");
                    }
                    else {
                        clientMgrParams.gate = (Ptr)gateHandle;
                        obj->clientNotifyMgrGate  = gateHandle;
                    }
                }
                if (status >= 0) {
                    /* Open  the already created clientNotifyMgr  instance  using
                     * Shared addr
                     */
                    if (obj->ctrlStructCacheFlag == TRUE) {
                        clientMgrParams.cacheFlags =
                                            ClientNotifyMgr_CONTROL_CACHEUSE;
                    }

                    /*Open the client notifyMgr instace*/
                    clientMgrParams.openFlag = TRUE;
                    cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);
                    if (cliMgrHandle == NULL) {
                        status = FrameQBufMgr_E_CLIENTN0TIFYMGR_OPEN;
                        GT_setFailureReason (curTrace, GT_4CLASS,
                                "_FrameQBufMgr_ShMem_open", status,
                                "ClientNotifyMgr_create() failed!");
                    }
                    else {
                        obj->clientNotifyMgrHandle = (Ptr)cliMgrHandle;
                        obj->objType               =
                                                    FrameQBufMgr_ShMem_DYNAMIC_OPEN;
                        obj->isRegistered          = FALSE;
                        obj->notifyId              = (UInt32)-1;
                        obj->refCount              = 1;
                        obj->instId                = pAttrs->entryId;
                        strcpy((String)obj->name, (const Char *)pAttrs->name);
                        /*********************************************************
                         *Memory manager allocaters handling
                         *********************************************************
                         */
                        status = _FrameQBufMgr_ShMem_openHdrBufMemMgr(obj,
                                pnameserverEntry);
                        if (status >= 0) {
                            status = _FrameQBufMgr_ShMem_openFrmBufMemMgr(
                                    obj, pnameserverEntry);
                        }
                    }
                }
            }
            else {
                status  = FrameQBufMgr_E_FAIL;
            }
        }
        /* Leave the gate */
        GateMP_leave ((GateMP_Handle)obj->gate, key);
    }

    return (status);
}

/*
 *  ======== _FrameQBufMgr_ShMem_openHdrBufMemMgr ========
 *  Internal API to open the instance.Called internally.
 */
static inline
Int32 _FrameQBufMgr_ShMem_openHdrBufMemMgr(FrameQBufMgr_ShMem_Obj *obj,
        FrameQBufMgr_NameServerEntry *pnameserverEntry)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    SyslinkMemMgr_SharedMemory_Params hdrBufMemMgrParams;

    /* Memory manager creation for Header bufs */
    switch (pnameserverEntry->headerInterfaceType) {
        case FrameQBufMgr_HDRINTERFACE_SHAREDMEM:
            /* Open HeapMemMP that is needed for HeaderBuffers
             * Open only if name is provided.
             */
            if (status >= 0) {
                /* Initialize params for the shared Memory manager
                 */
                SyslinkMemMgr_SharedMemory_Params_init(&hdrBufMemMgrParams);

                /* Pass null to the MemMgr as we are explicitly
                 * providing memory for headerbufs.
                 */
                obj->heapMemMPHandle_HdrBufs = NULL;

                obj->memMgrAllocater_hdrBufs = SyslinkMemMgr_create(&hdrBufMemMgrParams);
                obj->memMgrType_HdrBufs = SyslinkMemMgr_TYPE_SHAREDMEM;
                GT_assert(curTrace, (NULL != obj->memMgrAllocater_hdrBufs));
            }
            break;

        default:
            GT_assert(curTrace, 0);
            status = FrameQBufMgr_E_INVALID_HDRINTERFACETYPE;
            break;
    }

    return (status);
}

/*
 *  ======== _FrameQBufMgr_ShMem_openFrmBufMemMgr ========
 *  Internal API to open the instance.Called internally.
 */
static inline
Int32 _FrameQBufMgr_ShMem_openFrmBufMemMgr(FrameQBufMgr_ShMem_Obj *obj,
        FrameQBufMgr_NameServerEntry *pnameserverEntry)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    SyslinkMemMgr_SharedMemory_Params frmBufMemMgrShmParams;
    SyslinkMemMgr_TilerMemory_Params frmBufMemMgrTlrParams;

    /* Memory manager creation for frame bufs */
    switch (pnameserverEntry->bufInterfaceType) {
        case FrameQBufMgr_BUFINTERFACE_SHAREDMEM:
            SyslinkMemMgr_SharedMemory_Params_init(&frmBufMemMgrShmParams);
            obj->heapMemMPHandle_FrmBufs = NULL;

            obj->memMgrAllocater_frmBufs = SyslinkMemMgr_create(&frmBufMemMgrShmParams);
            obj->memMgrType_FrmBufs = SyslinkMemMgr_TYPE_SHAREDMEM;
            GT_assert(curTrace, (NULL != obj->memMgrAllocater_frmBufs));
            break;

        case FrameQBufMgr_BUFINTERFACE_TILERMEM:
            SyslinkMemMgr_TilerMemory_Params_init(&frmBufMemMgrTlrParams);

            obj->memMgrAllocater_frmBufs = SyslinkMemMgr_create(&frmBufMemMgrTlrParams);
            obj->memMgrType_FrmBufs = SyslinkMemMgr_TYPE_TILERMEM;
            GT_assert(curTrace, (NULL != obj->memMgrAllocater_frmBufs));
            break;

        default:
            GT_assert(curTrace, 0);
            status = FrameQBufMgr_E_INVALID_BUFINTERFACETYPE;
            break;
    }

    return (status);
}


/*
 *  ======== FrameQBufMgr_Shmem_updateCacheFlags ========
 *  Internal API to populate the cache flags
 */
Void FrameQBufMgr_Shmem_updateCacheFlags(FrameQBufMgr_ShMem_Obj *obj,
        UInt32 cpuAccessFlags)
{
    UInt32  i = 0;
    UInt32  bufCacheFlag = FALSE;

    /* Derive Header buffer cache flag from the shared region id */
    obj->frmHdrBufCacheFlag = SharedRegion_isCacheEnabled(obj->hdrBuf_regionId);

    /* Derive frame buffer cache flags from the shared region id and
     * cpuAccessFlags.
     */
    if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_SHAREDMEM) {
        bufCacheFlag = SharedRegion_isCacheEnabled(obj->frmBuf_regionId);
    }
    else if (obj->bufInterfaceType == FrameQBufMgr_BUFINTERFACE_TILERMEM) {
        /* TBD: Currently Tiler buffers are from non cached memory */
        bufCacheFlag = FALSE;
    }
    else {
    }

    for (i = 0; i < FrameQBufMgr_ShMem_MAX_FRAMEBUFS; i++) {
        /* Check and update if user has provided cache flags for Frame
         * buffers.
         */
        obj->frmBufCacheFlag[i] = bufCacheFlag;

        /* Check and update if user has provided cpu access flags for Frame
         * buffers.
         */
        if (cpuAccessFlags
             & (1 << (FrameQBufMgr_ShMem_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET +i))) {
            obj->bufCpuAccessFlag[i] = TRUE;
        }
    }
}

/*
 *  ======== FrameQBufMgr_postInit ========
 *  Internal API to populate the frame pools.
 */
Int32
FrameQBufMgr_ShMem_postInit(FrameQBufMgr_ShMem_Obj *obj,
        const FrameQBufMgr_ShMem_Params *params, Ptr arg)
{
    Int32                         status     = FrameQBufMgr_S_SUCCESS;
    UInt32                        addrOffset = 0;
    FrameQBufMgr_ShMem_FrameBufInfo              *frameBufInfo  = NULL;
    volatile FrameQBufMgr_ShMem_Attrs            *pAttrs;
    volatile FrameQBufMgr_ShMem_FreeFramePoolObj *freeFramePoolobj;
    UInt32                        i;
    UInt32                        j;
    UInt32                        k;
    UInt32                        memSzForCliMgr;
    ClientNotifyMgr_Params        clientMgrParams;
    FrameQBufMgr_ShMem_Frame      frame;
    UInt32                        lhdrChunkStartAddr;
    UInt32                        lfrmChunkStartAddr;
    UInt32                        lhdrChunkSize;
    UInt32                        lfrmChunkSize;
    UInt32                        numframes;
    UInt32                        numHdrs;
    UInt32                        hdrSize;
    UInt32                        numBufsInFrm;
    UInt32                        frmbufOffset;
    UInt32                        tframbufSize;
    UInt32                        virtAddr;
    UInt32                        hdrBufMemReq;
    UInt32                        frmBufMemReq;
    SyslinkMemMgr_AllocParams     allocParams;
    SharedRegion_Entry            srEntry;

    Error_Block ebObj;
    Error_Block *eb = &ebObj;  /* locally used Error_Block */

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_ShMem_postInit", obj, params);

    GT_assert(curTrace, (obj != NULL));
    GT_assert(curTrace, (params != NULL));
    pAttrs = obj->attrs;

    Error_init(eb);

    if (params->numFreeFramePools == 0) {
        status = FrameQBufMgr_E_FAIL;
    }
    else {
        /* Start of the instance shared addr */
        virtAddr = (UInt32)obj->pnameserverEntry;

        /* ---------------------------------------------------------
         * Update the control structure for number of frame pools
         * ---------------------------------------------------------
         */
        pAttrs->numFreeFramePools = params->numFreeFramePools;

        /* To create the  list objects */
        addrOffset += ROUND_UP(sizeof (FrameQBufMgr_NameServerEntry),
                              obj->minAlign);


        addrOffset += ROUND_UP(sizeof (FrameQBufMgr_ShMem_Attrs),
                              obj->minAlign);

        addrOffset += ROUND_UP(sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                              obj->minAlign) * params->numFreeFramePools;

        addrOffset += ROUND_UP(obj->cliGateMPShAddrSize,
                              obj->minAlign);

        ClientNotifyMgr_Params_init(&clientMgrParams);
        clientMgrParams.regionId = SharedRegion_getId((Ptr)pAttrs);
        clientMgrParams.sharedAddr = (UInt32*)((UInt32)virtAddr
                                                  + addrOffset );
        clientMgrParams.numNotifyEntries = pAttrs->numNotifyEntries;
        clientMgrParams.numSubNotifyEntries = pAttrs->numFreeFramePools;

        memSzForCliMgr =  ClientNotifyMgr_sharedMemReq (
                               (ClientNotifyMgr_Params*)&clientMgrParams );

        addrOffset += ROUND_UP(memSzForCliMgr, obj->minAlign);

        FrameQBufMgr_ShMem_sharedMemReq(params, &hdrBufMemReq, &frmBufMemReq);

        if (params->sharedAddrHdrBuf == NULL) {
            /* Allocate memory from the hdrBuf_regionId */
            obj->hdrBuf_regionId = params->hdrBuf_regionId;
            obj->hdrBufAllocSize = hdrBufMemReq;
            obj->hdrBufPtr =
                    Memory_alloc(SharedRegion_getHeap(obj->hdrBuf_regionId),
                        obj->hdrBufAllocSize, 0, eb);
            if (obj->hdrBufPtr == NULL) {
                status = FrameQBufMgr_E_ALLOC_MEMORY;
                GT_setFailureReason (curTrace, GT_4CLASS,
                        "FrameQBufMgr_ShMem_postInit", status,
                        "Memory allocation failed for hdrBufPtr!");
            }
            else {
                lhdrChunkStartAddr = (UInt32)obj->hdrBufPtr;
                lhdrChunkSize = obj->hdrBufAllocSize;
            }
        }
        else {
            obj->hdrBufPtr = params->sharedAddrHdrBuf;
            obj->hdrBufAllocSize = 0;
            lhdrChunkStartAddr = (UInt32)params->sharedAddrHdrBuf;
            lhdrChunkSize = params->sharedAddrHdrBufSize;
        }

        if (params->sharedAddrFrmBuf == NULL) {
            /* Allocate memory from the hdrBuf_regionId */
            obj->frmBuf_regionId = params->frmBuf_regionId;
            obj->frmBufAllocSize = frmBufMemReq;

            if (obj->bufInterfaceType ==
                    FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM){

                SharedRegion_getEntry(obj->frmBuf_regionId, &srEntry);
                /* no need to check SharedRegion_S_SUCCESS, asserted before */

                if (srEntry.createHeap == TRUE) {

                    obj->frmBufPtr = Memory_alloc(
                            SharedRegion_getHeap(obj->frmBuf_regionId),
                            obj->frmBufAllocSize, 0, eb);
                    if (obj->frmBufPtr == NULL) {
                        status = FrameQBufMgr_E_ALLOC_MEMORY;
                        GT_setFailureReason (curTrace, GT_4CLASS,
                                "FrameQBufMgr_ShMem_postInit", status,
                                "Memory allocation failed for frmBufs!");
                    }
                    lfrmChunkSize = obj->frmBufAllocSize;

                } else {
                    /* This is the case when frame-buffer memory is not mapped
                     * to kernel virtual memory space on the HOST OS.
                     *
                     * On the RTOS side simply use the Shared Region Base
                     * address as the exclusive owner of that SR, and assign
                     * the whole region for frame buffer memory.
                     */
                    obj->frmBufPtr = (Ptr) srEntry.base;
                    obj->frmBufAllocSize = 0;
                    lfrmChunkSize =  srEntry.len;
                }
            }
            else {
                /* Will be assigned at MemMgr_alloc for tiled buffers */
                obj->frmBufPtr  = NULL;
            }
            lfrmChunkStartAddr = (UInt32)obj->frmBufPtr;
        }
        else {
            obj->frmBufPtr = params->sharedAddrFrmBuf;
            obj->frmBufAllocSize = 0;
            lfrmChunkStartAddr = (UInt32)params->sharedAddrFrmBuf;
            lfrmChunkSize = params->sharedAddrFrmBufSize;
        }

        for (i = 0; (i < params->numFreeFramePools) && (status >= 0); i++) {
            numframes = params->numFreeFramesInPool[i];
            numHdrs   = params->numFreeHeaderBufsInPool[i];
            hdrSize   = params->frameHeaderBufSize[i];
            numBufsInFrm  = params->numFrameBufsInFrame[i];
            /* Checking if hdr chunk is sufficient */
            if (  ((numframes + numHdrs) * hdrSize) + lhdrChunkStartAddr
                <= ((UInt32)obj->hdrBufPtr + lhdrChunkSize)) {
                /*  Now populate frames and header buffers */
                /* Check if frm buf chunk is sufficient */
                tframbufSize = 0;
                if (obj->bufInterfaceType == FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM){
                /*TBD:Check this condition forTilerMem*/
                    for (j = 0; j < numBufsInFrm ; j++) {
                        tframbufSize += params->frameBufParams[i][j].size;
                    }

                    if (  ((numframes * tframbufSize)+ lfrmChunkStartAddr)
                        > ((UInt32)obj->frmBufPtr + lfrmChunkSize)) {
                        status = FrameQBufMgr_E_FAIL;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "FrameQBufMgr_ShMem_postInit", status,
                                "FrameQBufMgr_ShMem_postInit Failed!");
                    }
                }
                if (status >= 0) {
                    freeFramePoolobj = obj->freeFramePool[i];
                    /* Frame header buffer size of this pool */
                    freeFramePoolobj->frmHdrBufSize = hdrSize;

                    /* Number of frame buffers in a frame
                     * belonging to this pool
                     */
                    freeFramePoolobj->numBufsInFrame = numBufsInFrm;

                    for (k = 0; k < numBufsInFrm; k++) {
                        freeFramePoolobj->frmBufSize[k] =
                                params->frameBufParams[i][k].size;
                    }

                    /* Number of Extra headers in this pool */
                    freeFramePoolobj->numFrmHdrBufs =  numHdrs;

                    freeFramePoolobj->freeFrmHdrBufs =
                                           freeFramePoolobj->numFrmHdrBufs;

                    /* Number of frames in this pool */
                    freeFramePoolobj->numFrms = numframes;

                    freeFramePoolobj->freeFrms =
                                                freeFramePoolobj->numFrms;

                    /* Allow  dynamic addition of  frames and frame buffers to
                     * this pool.
                     */
                    freeFramePoolobj->numDynamicHdrBufs  = 0;
                    freeFramePoolobj->freeDynamicHdrBufs = 0;

                    freeFramePoolobj->numDynamicFrames  = 0;
                    freeFramePoolobj->freeDynamicFrames = 0;

                    for (j = 0; j < FrameQBufMgr_ShMem_POOL_MAXFRAMES; j++) {
                        freeFramePoolobj->frmRefCnt[j] = 0;
                    }

                    for (j = 0; j < FrameQBufMgr_ShMem_POOL_DYNAMIC_MAXFRAMES; j++)
                    {
                        freeFramePoolobj->dynFrmRefCnt[j] = 0;
                    }

                    for (j = 0; j < FrameQBufMgr_ShMem_POOL_MAXHDRS; j++) {
                        /* To search for free hdr buf for dup purposes.
                         */
                        freeFramePoolobj->hdrBufIsUsed[j] = FALSE;
                        /* To keep track of hdr buf is pointing to  which
                         * reference count.
                         */
                        freeFramePoolobj->hdrBuf_refCntIndex[j] = (UInt32)-1;
                        freeFramePoolobj->hdrBuf_refCntIndexType[j] = 0;

                    }

                    for (j = 0;
                         j < FrameQBufMgr_ShMem_POOL_DYNAMIC_MAXHDRS;
                         j++) {
                        /* To search for free hdr buf for dup purposes.
                         */
                        freeFramePoolobj->dynHdrBufIsUsed[j] = FALSE;
                        /* To keep track of hdr buf is pointing to  which
                         * reference count.
                         */
                        freeFramePoolobj->dynHdrBuf_refCntIndex[j] = (UInt32)-1;
                        freeFramePoolobj->dynHdrBuf_refCntIndexType[j] = 0;
                        freeFramePoolobj->dynHdrBufs[j] = NULL;
                    }

                    /* Populate frames and headers */
                    /* Convert  address in to portable address */
                    freeFramePoolobj->frmStartAddrp = (UInt32)
                            SyslinkMemMgr_translate (obj->memMgrAllocater_hdrBufs,
                                              (Ptr) lhdrChunkStartAddr,
                                              SyslinkMemMgr_AddrType_Virtual,
                                              SyslinkMemMgr_AddrType_Portable);

                    freeFramePoolobj->frmEndAddrp = (UInt32)
                            SyslinkMemMgr_translate (obj->memMgrAllocater_hdrBufs,
                                              (Ptr)( lhdrChunkStartAddr +(numframes * hdrSize)),
                                              SyslinkMemMgr_AddrType_Virtual,
                                              SyslinkMemMgr_AddrType_Portable);

                    freeFramePoolobj->hdrBufStartAddrp = (UInt32)
                            SyslinkMemMgr_translate (obj->memMgrAllocater_hdrBufs,
                                              (Ptr)( lhdrChunkStartAddr +(numframes * hdrSize)),
                                              SyslinkMemMgr_AddrType_Virtual,
                                              SyslinkMemMgr_AddrType_Portable);

                    freeFramePoolobj->hdrBufEndAddrp = (UInt32)
                            SyslinkMemMgr_translate (obj->memMgrAllocater_hdrBufs,
                                              (Ptr)( lhdrChunkStartAddr +((numframes + numHdrs) * hdrSize)),
                                              SyslinkMemMgr_AddrType_Virtual,
                                              SyslinkMemMgr_AddrType_Portable);
                    if (obj->bufInterfaceType
                            == FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM) {

                        /*
                         * On the RTOS this translation will actually
                         * work whether allocation was from SR Heap
                         * or directly using SR->entry.base address.
                         * since virtual == physical addresses.
                         */
                        freeFramePoolobj->frmBufsStartAddrp = (UInt32)
                                SyslinkMemMgr_translate (
                                        obj->memMgrAllocater_frmBufs,
                                        (Ptr)(lfrmChunkStartAddr),
                                        SyslinkMemMgr_AddrType_Virtual,
                                        SyslinkMemMgr_AddrType_Portable);

                        freeFramePoolobj->frmBufsEndAddrp = (UInt32)
                                SyslinkMemMgr_translate (
                                        obj->memMgrAllocater_frmBufs,
                                        (Ptr)( lfrmChunkStartAddr
                                                +(numframes *tframbufSize)),
                                        SyslinkMemMgr_AddrType_Virtual,
                                        SyslinkMemMgr_AddrType_Portable);
                    }
                    frmbufOffset = 0;

                    /* Now prepare frame header for the frames in the pool*/
                    for (j = 0; j < freeFramePoolobj->numFrms; j++) {
                        frame = (FrameQBufMgr_ShMem_Frame)(lhdrChunkStartAddr
                                     +( (freeFramePoolobj->frmHdrBufSize) * j));

                        /*Size of the frame  header(total size of base header
                         *and extended header .
                         */
                        frame->headerSize =
                                         freeFramePoolobj->frmHdrBufSize;
                        /* FrameQBufMgr Id from which this frame is allocated */
                        frame->frmAllocaterId = obj->instId;
                        /* Free ququeNo from which frame is allocated in
                         * FrameQBufMgr
                         */
                        frame->freeFrmQueueNo = i;
                        /* Number of framebuffers  in frame. */
                        frame->numFrameBuffers = numBufsInFrm;
                        /*Size of the base Frame header*/
                        frame->baseHeaderSize =
                            FrameQBufMgr_getBaseHeaderSize(frame->numFrameBuffers);

                        frameBufInfo = &frame->frameBufInfo[0];

                        for (k = 0; k < numBufsInFrm;k++) {
                            /*frame buffer in portable format*/
                            if (obj->bufInterfaceType ==
                                    FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM) {

                                /*
                                 * On the RTOS this translation will actually
                                 * work whether allocation was from SR Heap
                                 * or directly using SR->entry.base address.
                                 * since virtual == physical addresses.
                                 */
                                frameBufInfo[k].bufPtr = (UInt32)
                                    SyslinkMemMgr_translate (
                                        obj->memMgrAllocater_frmBufs,
                                       (Ptr)(lfrmChunkStartAddr + frmbufOffset),
                                            SyslinkMemMgr_AddrType_Virtual,
                                            SyslinkMemMgr_AddrType_Portable);
                                /*frame buffer size */
                                frameBufInfo[k].bufSize =
                                        freeFramePoolobj->frmBufSize[k];
                                frmbufOffset +=
                                        freeFramePoolobj->frmBufSize[k];
                                /* valid data start offset from buffer start
                                 * addr */
                                frameBufInfo[k].startOffset= 0;
                                /* frame buffer valid size from the startoffset.
                                 * startoffset+validsize must be less than or
                                 * equal to bufSize
                                 */
                                frameBufInfo[k].validSize = 0;
                            }
                            if (obj->bufInterfaceType ==
                                    FrameQBufMgr_ShMem_BUFINTERFACE_TILERMEM) {
                                frameBufInfo[k].pixelFormat = allocParams.block[0].pixelFormat     = params->frameBufParams[i][k].pixelFormat;
                                frameBufInfo[k].width       = allocParams.block[0].dim.area.width  = params->frameBufParams[i][k].width;
                                frameBufInfo[k].height      = allocParams.block[0].dim.area.height = params->frameBufParams[i][k].height;
                                allocParams.tilerBlocks     = 1;

                                frameBufInfo[k].bufPtr = (UInt32)
                                                         SyslinkMemMgr_alloc(obj->memMgrAllocater_frmBufs,
                                                                             &allocParams);
                                /* Return the stride to use*/
                                frameBufInfo[k].stride = allocParams.block[0].stride;
                                /* valid data start offset from buffer start addr */
                                frameBufInfo[k].startOffset= 0;
                            }
                        }
                        /*Write back the contents of this frame to memory*/
                        if (obj->frmHdrBufCacheFlag == TRUE) {
                            Cache_wbInv((Ptr)frame,
                                        frame->baseHeaderSize,
                                        Cache_Type_ALL,
                                        TRUE);
                        }
                    }
                    /* Increment  for the next free frame pool preparation*/
                    lhdrChunkStartAddr += ((numframes + numHdrs) * hdrSize);
                    lfrmChunkStartAddr += (numframes *tframbufSize);

                    if (obj->ctrlStructCacheFlag == TRUE) {
                        Cache_wbInv((Ptr)freeFramePoolobj,
                                    sizeof(FrameQBufMgr_ShMem_FreeFramePoolObj),
                                    Cache_Type_ALL,
                                    TRUE);
                    }
                }

            }
            else {
                status = FrameQBufMgr_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "FrameQBufMgr_ShMem_postInit", status,
                        "_FrameQBufMgr_postInit Failed!");
            }
        }

        if (status >= 0) {
           /* Last thing, set the status */
            pAttrs->status = FrameQBufMgr_ShMem_CREATED;
            if (obj->ctrlStructCacheFlag == TRUE) {
                Cache_wbInv((Ptr)pAttrs,
                            sizeof(FrameQBufMgr_ShMem_Attrs),
                            Cache_Type_ALL,
                            TRUE);
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_postInit", status);

    return (status);
}


/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQBufMgr_ShMem_getCliNotifyMgrShAddr(FrameQBufMgr_ShMem_Handle handle)
{
    FrameQBufMgr_ShMem_Obj      *obj;
    Ptr                    shAddr;

    GT_1trace(curTrace, GT_ENTER, "_FrameQBufMgr_ShMem_getCliNotifyMgrShAddr",
            handle);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != handle->obj));

    obj = (FrameQBufMgr_ShMem_Obj*)handle->obj;

    GT_assert(curTrace, (obj != NULL));
    shAddr = (Ptr)(obj->cliNotifyMgrShAddr);

    GT_1trace(curTrace, GT_LEAVE, "_FrameQBufMgr_ShMem_getCliNotifyMgrShAddr",
            shAddr);

    return (shAddr);
}

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQBufMgr_ShMem_getCliNotifyMgrGateShAddr (FrameQBufMgr_ShMem_Handle handle)
{
    FrameQBufMgr_ShMem_Obj *obj;
    Ptr shAddr;

    GT_1trace(curTrace, GT_ENTER, "_FrameQBufMgr_ShMem_getCliNotifyMgrShAddr",
            handle);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != handle->obj));

    obj = (FrameQBufMgr_ShMem_Obj*)handle->obj;

    shAddr = (Ptr)(obj->cliNotifyMgrGateShAddr);;

    GT_1trace(curTrace, GT_LEAVE, "_FrameQBufMgr_ShMem_getCliNotifyMgrShAddr",
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
Int32 _FrameQBufMgr_ShMem_setNotifyId(FrameQBufMgr_ShMem_Handle handle,
        UInt32 notifyId)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Obj *obj;

    GT_2trace (curTrace, GT_ENTER, "_FrameQBufMgr_ShMem_setNotifyId", handle,
            notifyId);

    GT_assert(curTrace, (NULL != handle));

    obj = (FrameQBufMgr_ShMem_Obj*)handle->obj;

    obj->notifyId = notifyId;
    obj->isRegistered  = TRUE;

    GT_1trace(curTrace, GT_LEAVE, "_FrameQBufMgr_ShMem_setNotifyId", status);

    return (status);
}

/*
 *  ======== _FrameQBufMgr_ShMem_resetNotifyId ========
 *  Reset the notifyid received in userspace
 */
Int32 _FrameQBufMgr_ShMem_resetNotifyId(FrameQBufMgr_ShMem_Handle handle,
        UInt32 notifyId)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
//    FrameQBufMgr_ShMem_Obj *obj;

    GT_2trace (curTrace, GT_ENTER, "_FrameQBufMgr_ShMem_resetNotifyId", handle,
            notifyId);

    GT_assert (curTrace, (NULL != handle));

//    obj = (FrameQBufMgr_ShMem_Obj*)handle->obj;

    handle->obj->notifyId = notifyId;
    handle->obj->isRegistered  = FALSE;

    GT_1trace(curTrace, GT_LEAVE, "_FrameQBufMgr_ShMem_resetNotifyId", status);

    return (status);
}
