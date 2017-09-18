/*
 *  @file   FrameQ.c
 *
 *  @brief      Implements FrameQ functions.
 *
 *              TODO: write description
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
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/inc/Bitops.h>

#if defined(SYSLINK_BUILDOS_LINUX)
#include <linux/string.h>
#else
#include <string.h>
#endif

/* Utilities & OSAL headers */
#include <ti/ipc/MultiProc.h>
/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/GatePeterson.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/ipc/ListMP.h>

#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/FrameQBufMgr.h>

#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/inc/_FrameQ.h>

#include <ti/syslink/FrameQ_ShMem.h>
#include <ti/syslink/inc/_FrameQ_ShMem.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */

/*! @brief Macro to make a correct module magic number with refCount */
#define FRAMEQ_MAKE_MAGICSTAMP(x) ((FrameQ_MODULEID << 12u) | (x))


extern FrameQ_Fxns FrameQ_ShMem_fxns;
/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*
 * @brief Structure for FrameQ module state
 */
typedef struct FrameQ_ModuleObject_Tag {
    Atomic                refCount;
    /*!< Reference count */
    NameServer_Handle     nameServer;
    /*!< Handle to the local NameServer used for stoting FrameQ instance
     *   information.
     */
    IGateProvider_Handle  gate;
    /*!< Handle to lock for protection in NameServer*/
    List_Object           objList;
    /* List holding created objects */
    IGateProvider_Handle  listLock;
    /* Handle to lock for protecting objList */
    FrameQ_Config         cfg;
    /*!<  Current module wide config values*/
    FrameQ_Config         defaultCfg;
    /*!< Default config values*/
    FrameQ_Params         defInstParams;
    /*!< Default instance configuration values*/
} FrameQ_ModuleObject;

/*
 * @brief Structure defining internal object for the FrameQ
 */
typedef struct FrameQ_Obj_Tag {
    List_Elem            listElem;
    String          name;
    /*!< Name of the instance.*/
    GateMP_Handle     gate;
    /*!< Gate handle for protection */
    Ptr                     handle;
    /* Handle to implementation specific instance */
    UInt32                  instId;
    /*!< Id of the instance*/
    UInt32                  notifyId;
    /*!<  Location to  store the key returned by the
     *    ClientNotiftmgr_registerClient call
     */
    UInt32 objType;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32 objMode;
    /*!< Mode of the obejct. FrameQ_MODE_READER  or FrameQ_MODE_WRITER*/
    UInt32                  interfaceType;
    /* Interface type */
    Bool                    ctrlStructCacheFlag ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    instance control structures
     */
    Bool                    frmHdrBufCacheFlag ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    frame header buffers.
     */
    Bool                    frmBufCacheFlag[8] ;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    frame buffers
     */
    Bool                    bufCpuAccessFlag[8];
    /*!<  Flag indicating whether APP uses CPU to access these frame buffers.
     */
    UInt32 notifyRegId;
    /*!<  Id returned by the registerClient API of ClientNotifyMgr*/
    FrameQ_Params params;
    /*!< Instance  config params */
} FrameQ_Obj;

/*
 * @brief Structure defining object for the FrameQ.
 */
struct FrameQ_Object {
    FrameQ_Fxns *fxns;
   /*Pointer to implementation specific function table interface */
    FrameQ_Obj *obj;
    /*!< Pointer to the FrameQ internal object*/
};
typedef struct FrameQ_Object    FrameQ_Object;

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
static
#endif
FrameQ_ModuleObject FrameQ_state =
{
    .defaultCfg.eventNo                              = FrameQ_NOTIFY_RESERVED_EVENTNO,
    .defInstParams.commonCreateParams.size           = sizeof(FrameQ_Params),
    .defInstParams.commonCreateParams.ctrlInterfaceType  = FrameQ_INTERFACE_NONE,
    .defInstParams.commonCreateParams.name           = NULL,
    .defInstParams.commonCreateParams.openFlag       = FALSE,
};

/*  NameServer handle for the FrameQ module */
Ptr gFrameQ_nsHandle;

/*=============================================================================
 * Forward declarations of internal Functions
 *=============================================================================
 */
Void FrameQ_Params_init(FrameQ_Handle handle,
        FrameQ_Params *params);
static inline Int32 _FrameQ_create(FrameQ_Handle handle,
        FrameQ_Params *params);

static inline Int32 _FrameQ_open(FrameQ_Handle handle, FrameQ_Params* params);

/*=============================================================================
 * Module API
 *=============================================================================
 */
/*
 *  ======== FrameQ_getConfig ========
 *  Get the configuration for the FrameQ module
 */
Void FrameQ_getConfig(FrameQ_Config *cfg)
{
    GT_1trace(curTrace, GT_ENTER, "FrameQ_getConfig", cfg);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_getConfig",
                FrameQ_E_INVALIDARG,
                "Argument of type (FrameQ_Config *) passed is null!");
    }
    else {
#endif
        if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
                FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
            Memory_copy((Ptr)cfg, (Ptr)&FrameQ_state.defaultCfg,
                    sizeof(FrameQ_Config));
        }
        else {
            Memory_copy((Ptr)cfg, (Ptr)&FrameQ_state.cfg,
                    sizeof(FrameQ_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_0trace (curTrace, GT_LEAVE, "FrameQ_getConfig");
}


/*
 *  ======== FrameQ_setup ========
 */
Int32 FrameQ_setup(FrameQ_Config *cfg)
{
    Int32 status = FrameQ_S_SUCCESS;
    Error_Block       eb;
    Int32             tmpStatus;
    FrameQ_Config     tmpCfg;
    NameServer_Params nsParams;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_setup", cfg);
    Error_init(&eb);

    if (cfg == NULL) {
        FrameQ_getConfig(&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set(&FrameQ_state.refCount,
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(0));

    if (Atomic_inc_return(&FrameQ_state.refCount) !=
            FRAMEQ_MAKE_MAGICSTAMP(1u)) {
        status = FrameQ_S_ALREADYSETUP;
        GT_0trace(curTrace, GT_2CLASS, "FrameQ Module already initialized!");
    }
    else {
        /* Get the default params for  the Name server */
        NameServer_Params_init(&nsParams);
        nsParams.maxRuntimeEntries = cfg->maxInstances; /* max instances on this processor */
        nsParams.checkExisting     = TRUE; /* Check if exists */
        /* Length of  the FrameQ instance */
        nsParams.maxNameLen        = FrameQ_MAX_NAMELEN;
        nsParams.maxValueLen       = sizeof(FrameQ_NameServerEntry);
        /* Create the Name Server instance */
        FrameQ_state.nameServer = NameServer_create (FRAMEQ_NAMESERVERNAME,
                &nsParams);

        gFrameQ_nsHandle = FrameQ_state.nameServer;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NULL == FrameQ_state.nameServer) {
            status = FrameQ_E_CREATE_NAMESERVER;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_setup", status,
                    "Failed to create FrameQ NameServer");
        }
        else {
#endif

            /* Construct the list object */
            List_construct (&FrameQ_state.objList, NULL);
            /* Copy the cfg */
            Memory_copy ((Ptr) &FrameQ_state.cfg,
                         (Ptr) cfg,
                         sizeof (FrameQ_Config));
            /* Create a lock for protection*/
            FrameQ_state.gate = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
            GT_assert(curTrace, (FrameQ_state.gate != NULL));
            /* Create a lock for protecting list object */
            FrameQ_state.listLock = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (FrameQ_state.listLock == NULL) {
                if (FrameQ_state.nameServer != NULL) {
                    status = NameServer_delete(&FrameQ_state.nameServer);
                    GT_assert (curTrace, (status >= 0));
                }

                /*! @retval FrameQ_E_FAIL Failed to create the listLock*/
                status = FrameQ_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_setup",
                        status, "Failed to create the listLock!");
                Atomic_set (&FrameQ_state.refCount,
                            FRAMEQ_MAKE_MAGICSTAMP(0));
            }
        }
#endif
        if (status >= 0) {
            /* Call all the implementation specific setups */
            tmpStatus = FrameQ_ShMem_setup(cfg);
            if (tmpStatus < 0) {
                status = tmpStatus;
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_setup", status);

    return (status);
}

/*
 *  ======== FrameQ_destroy ========
 *  API to finalize the module.
 */
Int32
FrameQ_destroy (Void)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_0trace(curTrace, GT_ENTER, "FrameQ_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_destroy", status,
                "Module was not initialized!");
    }
    else {
#endif
        if (Atomic_dec_return(&FrameQ_state.refCount)
                == FRAMEQ_MAKE_MAGICSTAMP(0)) {

            FrameQ_ShMem_destroy();

            /* Delete name server instance created for this module,
             * if it is dynamically created
             */
            if (FrameQ_state.nameServer != NULL) {
                /* Delete the nameserver for modules */
                status = NameServer_delete(&FrameQ_state.nameServer);
                GT_assert(curTrace, (status >= 0));
            }
            /* TODO: module supplied gates needs to be finalized */

            /* Destruct the list object */
            List_destruct(&FrameQ_state.objList);

            if (FrameQ_state.listLock != NULL) {
                /* Delete the list lock */
                status = GateMutex_delete((GateMutex_Handle*)&FrameQ_state.listLock);
                GT_assert(curTrace, (status >= 0));
            }
            if (FrameQ_state.gate != NULL) {
                /* Delete the list lock */
                status = GateMutex_delete((GateMutex_Handle*)&FrameQ_state.gate);
                GT_assert(curTrace, (status >= 0));
            }
            /* Clear cfg area */
            Memory_set((Ptr)&FrameQ_state.cfg, 0, sizeof(FrameQ_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_destroy", status);

    return (status);
}

/*
 *  ======== FrameQ_sharedMemReq ========
 *  API to return the shared memory required for the instance based on the type
 *  of implementation.
 */
UInt32 FrameQ_sharedMemReq(Ptr params)
{
    UInt32                 totalSize     = 0;
    FrameQ_CreateParams    *frmQCreParams;

    if (NULL != params) {
        frmQCreParams = (FrameQ_CreateParams *)params;

        switch (frmQCreParams->ctrlInterfaceType) {
            case FrameQ_INTERFACE_SHAREDMEM:
                /* Call shared memory implementation's API*/
                totalSize = FrameQ_ShMem_sharedMemReq(params);
                break;

            default:
                /* Interface type is not known.*/
                break;
        }
    }

    return (totalSize);
}

/*
 *  ======== FrameQ_create ========
 *  Create a new instance of FrameQ
 */
FrameQ_Handle FrameQ_create(Ptr vparams)
{
    Int32 status = FrameQ_S_SUCCESS;
    FrameQ_Handle handle = NULL;
    FrameQ_Obj             *obj             = NULL;
    FrameQ_Params *params = (FrameQ_Params*)vparams;
    IArg                    key0;
    Int32                   tmpStatus;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_create", vparams);

    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create", status,
                "Module was not initialized!");
    }
    else if (params == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_create", status,
                "params passed is NULL!");
    }
    else {
#endif
        /* Allocate memory for handle */
        handle = (FrameQ_Handle)Memory_calloc(NULL, sizeof(FrameQ_Object), 0,
                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle == NULL) {
            status = FrameQ_E_ALLOC_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create", status,
                    "Memory allocation failed for handle!");
        }
        else {
#endif
            obj = (FrameQ_Obj *)Memory_calloc(NULL, sizeof(FrameQ_Obj), 0u,
                    NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj == NULL) {
                status = FrameQ_E_ALLOC_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create",
                    status, "Memory allocation failed for internal object!");

                Memory_free(NULL, handle, sizeof(FrameQ_Object));
                handle = NULL;
            }
            else {
#endif
                handle->obj = obj;
                if (params->commonCreateParams.openFlag == TRUE) {
                   tmpStatus = _FrameQ_open (handle, params);
                   if (tmpStatus < 0) {
                       status = tmpStatus;
                       GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create",
                               status, "Failed to open the instance!");
                   }
                }
                else if (params->commonCreateParams.openFlag == FALSE) {
                    tmpStatus = _FrameQ_create(handle, params);
                    if (tmpStatus < 0) {
                        status = tmpStatus;
                        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create",
                                status, "Failed to create instance!");
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif
        if (status >= 0) {
            /* Populate the params member */
            Memory_copy((Ptr)&obj->params, (Ptr)params, sizeof(FrameQ_Params));

            /* Put in the local list */
            key0 = IGateProvider_enter(FrameQ_state.gate);

            List_elemClear(&obj->listElem);
            List_put((List_Handle)&FrameQ_state.objList, &(obj->listElem));

            IGateProvider_leave(FrameQ_state.gate, key0);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Failed to create/open the instance. Clean up */
        else {
            if (obj != NULL) {
                Memory_free(NULL, obj, sizeof(FrameQ_Obj));
            }
            if (handle != NULL) {
                Memory_free(NULL, handle, sizeof(FrameQ_Object));
                handle = NULL;
            }
        }
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_create", handle);

    return (handle);
}


/*
 *  ======== FrameQ_open ========
 *  Open the created FrameQ instance
 */
Int32 FrameQ_open(Ptr instOpenParams, FrameQ_Handle *handlePtr)
{
    Int32 status = FrameQ_S_SUCCESS;
    FrameQ_Handle           handle = NULL;
    FrameQ_Fxns             *fxns;
    FrameQ_Params params;
    FrameQ_OpenParams *openParams = (FrameQ_OpenParams *)instOpenParams;
    Ptr                     impHandle = NULL;
    UInt32                  len;
    FrameQ_Obj             *obj            = NULL;
    FrameQ_Object          *object         = NULL;
    FrameQ_NameServerEntry entry;
    FrameQ_NameServerEntry *pentry;
    UInt32                  interfaceType;
    UInt8                   nameLen;
    IArg                    key;
    UInt32                  index;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_open", handlePtr, instOpenParams);

    GT_assert(curTrace, (handlePtr != NULL));
    GT_assert(curTrace, (openParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_open", status,
                "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_open", status,
                "Argument of type (FrameQ_Handle *) is NULL!");
    }
    else if (instOpenParams == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_open", status,
                "instOpenParams passed is null.");
    }
    else if ((String_len(openParams->commonOpenParams.name) == 0) &&
            (openParams->commonOpenParams.sharedAddr == NULL)) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_open", status,
                "Either name or sharedaddr must be non null.");
    }
    else {
#endif
        nameLen = strlen((String)openParams->commonOpenParams.name);
        if (openParams->commonOpenParams.name != NULL) {
            /* If name is provided use name based open only */
            openParams->commonOpenParams.sharedAddr = NULL;
        }

        if (((nameLen > 0) && ( nameLen <= (FrameQ_MAX_NAMELEN - 1)) &&
                (NULL != FrameQ_state.nameServer))) {
            len = sizeof(FrameQ_NameServerEntry);
            status = NameServer_get(FrameQ_state.nameServer,
                    (String)openParams->commonOpenParams.name, &entry, &len,
                    NULL);
            if (status == NameServer_E_NOTFOUND) {
                status = FrameQ_E_NOTFOUND;
            }
            else {
                interfaceType = entry.interfaceType;
            }
        }
        else if (openParams->commonOpenParams.sharedAddr != NULL) {
            pentry = (FrameQ_NameServerEntry*)
                    openParams->commonOpenParams.sharedAddr;
            /* Assert that sharedAddr is cache aligned */
            index = SharedRegion_getId(pentry);

            /* Assert that the buffer is in a valid shared region */
            GT_assert(curTrace, (index != SharedRegion_INVALIDREGIONID));

            GT_assert(curTrace, ((UInt32)pentry %
                    SharedRegion_getCacheLineSize(index) == 0));

            if (SharedRegion_isCacheEnabled(index)) {
                /* Invalidating the entry located in shared memory. The memory
                 * provided by this entry should be in cache aligend size
                 */
                key = Gate_enterSystem();
                Cache_inv(pentry, sizeof(FrameQ_NameServerEntry),
                        Cache_Type_ALL, TRUE);
                Gate_leaveSystem(key);
            }
            interfaceType = pentry->interfaceType;
        }
        else {
            status = FrameQ_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_open", status,
                    "Either name or sharedaddr must be non null.");
        }

        if (status >= 0) {
            /* Check the interface type and if it is not supported
             * return failure.
             */
            if (!FrameQ_isSupportedInterface(interfaceType)) {
                status = FrameQ_E_INVALID_INTERFACE;
                GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_open",
                        status, "Unsupported interface type.");
            }
            else {
                FrameQ_Params_init(NULL, &params);
                params.commonCreateParams.name = (String)
                        openParams->commonOpenParams.name;
                /* Opening the instance  */
                params.commonCreateParams.openFlag = TRUE;

                handle = FrameQ_create(&params);
                if (handle == NULL) {
                    status = FrameQ_E_FAIL;
                }
                else {
                    if (openParams->commonOpenParams.name != NULL) {
                        /* If name is provided use name based  open only */
                        openParams->commonOpenParams.sharedAddr = NULL;
                    }
                    /* Open handle to the implementation specific handle
                     * and plug in to the top level FrameQ object.
                     */
                    switch (interfaceType) {
                        case FrameQ_INTERFACE_SHAREDMEM:
                            status = FrameQ_ShMem_open(
                                    (FrameQ_ShMem_OpenParams *)openParams,
                                    (FrameQ_ShMem_Handle *)&impHandle);
                            if ((status >= 0) && (impHandle != NULL)) {
                                fxns = &(FrameQ_ShMem_fxns);
                            }
                            else {
                                FrameQ_close(&handle);
                            }
                            break;

                        default:
                            /* NameServer entry does not contain the
                             * implementation type.
                             */
                            status = FrameQ_E_INVALID_INTERFACE;
                    }
                }
            }
        }

        if (status >= 0) {
            /* obj is inside FrameQBufMgr handle */
            object             = handle;
            handle->fxns   = fxns;
            obj                = object->obj;
            obj->handle        = impHandle;
            obj->interfaceType = interfaceType;
            obj->objType       = FrameQ_DYNAMIC_OPEN;
            *handlePtr         = handle;
        }
        else {
           *handlePtr         = NULL;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    return (status);
}


/*
 *  ======== FrameQ_delete ========
 *  Deletes an instance of FrameQ
 */
Int32 FrameQ_delete(FrameQ_Handle *pHandle)
{
    Int32 status = FrameQ_S_SUCCESS;
    IArg                   key0;
    FrameQ_Object          *handle;
    FrameQ_Obj             *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_delete", pHandle);

    GT_assert(curTrace, (pHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALID_INTERFACE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_delete", status,
                "Module was not initialized!");
    }
    else if (pHandle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_delete", status,
                "pHandle passed is NULL!");
    }
    else if (*pHandle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_delete", status,
                "*pHandle passed is NULL!");
    }
    else {
#endif
        handle = *pHandle;
        obj = (FrameQ_Obj *) handle->obj;
        GT_assert (curTrace, (NULL != obj));

        switch (obj->objType) {

            case FrameQ_DYNAMIC_OPEN:
                key0 = IGateProvider_enter(FrameQ_state.gate);
                /* Remove it from the local list */
                List_remove((List_Handle)&FrameQ_state.objList,
                        &obj->listElem);
                IGateProvider_leave(FrameQ_state.gate, key0);

                /* Close the handle and delete the object */
                if (obj->handle != NULL) {
                    switch (obj->interfaceType) {
                        case FrameQ_INTERFACE_SHAREDMEM:
                            FrameQ_ShMem_close(
                                    (FrameQ_ShMem_Handle *)&(obj->handle));
                            break;

                        default:
                            break;
                    }
                }
                /* Now free the handle */
                Memory_free(NULL, obj, sizeof(FrameQ_Obj));
                obj = NULL;
                Memory_free(NULL, handle, sizeof(FrameQ_Object));
                handle = NULL;
                break;

            case FrameQ_DYNAMIC_CREATE:
                key0 = IGateProvider_enter(FrameQ_state.gate);
                /* Remove it from the local list */
                List_remove((List_Handle)&FrameQ_state.objList,
                        &obj->listElem);
                IGateProvider_leave(FrameQ_state.gate, key0);
                /* Delete the handle and top level object */
                if (obj->handle != NULL) {
                    /* Call implementation delete */
                    handle->fxns->deleteInstance(&(obj->handle));
                }

                /* Now free the handle */
                Memory_free(NULL, obj, sizeof(FrameQ_Obj));
                obj = NULL;
                Memory_free(NULL, handle, sizeof(FrameQ_Object));
                handle = NULL;

                break;

            default:
                status = FrameQ_E_ACCESSDENIED;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_delete",
                        status, "Invalid Handle!");
                break;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_delete", status);

    return (status);
}

/*
 * ======== FrameQ_close ========
 * Function to close the dynamically opened shared memory FrameQ instance.
 */
Int32 FrameQ_close(FrameQ_Handle *pHandle)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_close", pHandle);
    GT_assert(curTrace, (pHandle != NULL));

    status = FrameQ_delete(pHandle);

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_close", status);

    return (status);
}

/*
 * ======== FrameQ_alloc ========
 * Function to allocate a frame.
 */
Int32 FrameQ_alloc(FrameQ_Handle handle, FrameQ_Frame *framePtr)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_alloc", handle, framePtr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_alloc", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_alloc", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_alloc", status,
                "framePtr passed is NULL!");
    }
    else {
#endif

        /* Initialize to NULL. */
        *framePtr = NULL;

        /* Call implementation specific alloc.
         * Here obj->handle is the implementation specific handle.
         */
        if (handle->fxns->alloc != NULL) {
            status = handle->fxns->alloc(handle->obj->handle, framePtr);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_alloc", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_alloc", status);

    return (status);
}

/*
 * ======== FrameQ_allocv ========
 * Function to allocate multiple frames
 */
Int32 FrameQ_allocv(FrameQ_Handle handle, FrameQ_Frame framePtr[],
        UInt32 freeQId[], UInt8 *numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_4trace(curTrace, GT_ENTER, "FrameQ_allocv", handle, framePtr, freeQId,
            *numFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_allocv", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_allocv", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_allocv", status,
                "framePtr passed is NULL!");
    }
    else {
#endif

        /* Call implementation specific allocv.
         * Here obj->handle is the implementation specific handle.
         */
        if (handle->fxns->allocv != NULL) {
            status = handle->fxns->allocv(handle->obj->handle, framePtr,
                    freeQId, numFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_allocv", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_allocv", status);

    return (status);
}

/*
 * ======== FrameQ_free ========
 * Function to free single frame
 */
Int32 FrameQ_free(FrameQ_Handle handle, FrameQ_Frame frame)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_free", handle, frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_free", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_free", status,
                "handle passed is NULL!");
    }
    else {
#endif
        /* Call implementation specific free.
         * Here obj->handle is the implementation specific handle.
         */
        if (handle->fxns->free != NULL) {
            status = handle->fxns->free(handle->obj->handle, frame);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_free", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_free", status);

    return (status);
}

/*
 * ======== FrameQ_freev ========
 * Free multiple frames
 */
Int32 FrameQ_freev(FrameQ_Handle handle, FrameQ_Frame framePtr[],
        UInt32 numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_freev", handle, framePtr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_freev", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_freev", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_freev", status,
                "framePtr passed is NULL!");
    }
    else {
#endif
        /* Call implementation specific freev.
         * Here obj->handle is the implementation specific handle.
         */
        if (handle->fxns->freev != NULL) {
            status = handle->fxns->freev(handle->obj->handle, framePtr,
                    numFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_freev", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_freev", status);

    return (status);
}


/*
 * ======== FrameQ_put ========
 * Insert frame in to FrameQ
 */
Int32 FrameQ_put(FrameQ_Handle handle, FrameQ_Frame frame)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_put", handle, frame);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != frame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_put", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_put", status,
                "handle passed is NULL!");
    }
    else if (frame == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_put", status,
                "framePtr passed is NULL!");
    }
    else {
#endif

        /* Call implementation specific put.
         * Here obj->handle is the implementation specific handle.
         */
        if (handle->fxns->put != NULL) {
            status = handle->fxns->put(handle->obj->handle, frame);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_put", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_put", status);

    return (status);
}


/*
 * ======== FrameQ_putv ========
 * Insert multiple frames
 */
Int32 FrameQ_putv(FrameQ_Handle handle, FrameQ_Frame framePtr[],
        UInt32 filledQueueId[], UInt8 numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_4trace(curTrace, GT_ENTER, "FrameQ_putv", handle, framePtr,
            filledQueueId, numFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != framePtr));
    GT_assert(curTrace, (NULL != filledQueueId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_putv", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_putv", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_putv", status,
                "framePtr passed is NULL!");
    }
    else {
#endif

        /* Call implementation specific alloc.
         * Here obj->handle is the upcasted handle.
         */
        if (handle->fxns->putv != NULL) {
            status = handle->fxns->putv(handle->obj->handle, framePtr,
                    filledQueueId, numFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_putv", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_putv", status);

    return (status);
}


/*
 * ======== FrameQ_get ========
 * Retrieve frame from FrameQ
 */
Int32 FrameQ_get(FrameQ_Handle handle, FrameQ_Frame *pframe)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_get", handle, pframe);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != pframe));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                "handle passed is NULL!");
    }
    else if (pframe == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                "framePtr passed is NULL!");
    }
    else if (handle->obj == NULL) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                "The provided handle does not have permission to"
                " do get. The handle is not a reader handle.!");
    }
    else {
#endif

        /* Initialize to NULL. */
        *pframe = NULL;

        if (handle->fxns->get != NULL) {
            status = handle->fxns->get(handle->obj->handle, pframe);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_get", status);

    return (status);
}

/*
 * ======== FrameQ_getv ========
 * Retrieve frames
 */
Int32 FrameQ_getv(FrameQ_Handle handle, FrameQ_Frame pframe[],
        UInt32 filledQueueId[], UInt8 *numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_4trace(curTrace, GT_ENTER, "FrameQ_getv", handle, pframe,
            filledQueueId, *numFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != pframe));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                "handle passed is NULL!");
    }
    else if (pframe == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                "framePtr passed is NULL!");
    }
    else if (handle->obj == NULL) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                "The provided handle does not have permission to"
                " do put. The handle is not a reader handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (handle->fxns->getv != NULL) {
            status = handle->fxns->getv(handle->obj->handle, pframe,
                    filledQueueId, numFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_getv", status);

    return (status);
}


/*
 * ======== FrameQ_registerNotifier ========
 * Register a callback function
 */
Int32 FrameQ_registerNotifier(FrameQ_Handle handle,
        FrameQ_NotifyParams *params)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_registerNotifier", handle, params);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != params));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_registerNotifier",
                status, "Module was not initialized!");
    }
    else if (handle->obj == NULL) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_registerNotifier",
                status, "handle->obj is null!");
    }
    else {
#endif
        params->cbClientHandle = handle;

        if (handle->fxns->registerNotifier != NULL) {
            status = handle->fxns->registerNotifier(handle->obj->handle,
                    params);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQ_registerNotifier", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_registerNotifier", status);

    return (status);
}

/*
 *  ======== FrameQ_unregisterNotifier ========
 * Function to unregister the call back function.
 */
Int32 FrameQ_unregisterNotifier(FrameQ_Handle handle)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_unregisterNotifier", handle);

    GT_assert(curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_unregisterNotifier",
                FrameQ_E_INVALIDSTATE, "Module was not initialized!");
    }
    else if (handle->obj == NULL) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_unregisterNotifier",
                status, "handle->obj is null.!");
    }
    else {
#endif

        if (handle->fxns->unregisterNotifier != NULL) {
            status = handle->fxns->unregisterNotifier(handle->obj->handle);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQ_unregisterNotifier", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_unregisterNotifier", status);

    return (status);
}

/*
 * ======== FrameQ_dup ========
 * Duplicate given frame
 */
Int32 FrameQ_dup(FrameQ_Handle handle, FrameQ_Frame frame,
        FrameQ_Frame * dupedFrame)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "FrameQ_dup", handle, frame, dupedFrame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_dup", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_dup", status,
                "handle passed is NULL!");
    }
    else if (frame == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_dup", status,
                "frame passed is NULL!");
    }
    else if (dupedFrame == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_dup", status,
                "dupedFrame passed is NULL!");
    }
    else if (handle->obj == NULL) {
        status = FrameQ_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_dup", status,
                "The provided handle has a NULL "
                "FrameQ object ptr!(handle->obj");
    }
    else {
#endif

        if (handle->fxns->dup != NULL) {
            status = handle->fxns->dup(handle->obj->handle, frame, dupedFrame);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_dup", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_dup", status);

    return (status);
}


/*
 * ======== FrameQ_sendNotify ========
 * Send forced notification
 */
Int32 FrameQ_sendNotify(FrameQ_Handle handle, UInt16 msg)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_sendNotify", handle, msg);

    GT_assert(curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_sendNotify", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_sendNotify", status,
                "handle passed is NULL!");
    }
    else {
#endif
        if (handle->fxns->sendNotify != NULL) {
            status = handle->fxns->sendNotify(handle->obj->handle, msg);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_sendNotify",
                    status, "This function is not implemented in the"
                    " interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_sendNotify", status);

    return (status);
}

/*
 *  ======== FrameQ_getNumFrames ========
 * For Writer: get the available frames in filled queue0 of
 *             primary queue for writer.
 * For Reader: get the available frames in filled queue0 of
 *             the caller client.
 */
Int32 FrameQ_getNumFrames(FrameQ_Handle handle, UInt32 *numFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_getNumFrames", handle, numFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != numFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getNumFrames", status,
                "Module was not initialized!");
    }
    else {
#endif

        if  (handle->fxns->getNumFrames != NULL) {
            status = handle->fxns->getNumFrames (handle->obj->handle,
                    numFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getNumFrames",
                    status, "This function is not implemented in the"
                    " interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_getNumFrames", status);

    return (status);
}

/*
 * ======== FrameQ_getvNumFrames ========
 * Find the number of available frames in a FrameQ
 */
Int32 FrameQ_getvNumFrames(FrameQ_Handle handle, UInt32 numFrames[],
        UInt8 filledQId[], UInt8 numFilledQids)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_getvNumFrames", handle, numFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != numFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getvNumFrames",
                status, "Module was not initialized!");
    }
    else {
#endif

        if (handle->fxns->getvNumFrames != NULL) {
            status = handle->fxns->getvNumFrames(handle->obj->handle,
                    numFrames, filledQId, numFilledQids);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getvNumFrames",
                    status, "This function is not implemented in the"
                    " interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_getvNumFrames", status);

    return (status);
}

/*
 * ======== FrameQ_getNumFreeFrames ========
 * Find number of available free frames in a FrameQBufMgr
 *
 */
Int32 FrameQ_getNumFreeFrames(FrameQ_Handle handle, UInt32 *numFreeFrames)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_getNumFreeFrames", handle,
               numFreeFrames);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != numFreeFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getNumFreeFrames",
                status, "Module was not initialized!");
    }
    else {
#endif

        if (handle->fxns->getNumFreeFrames != NULL) {
            status = handle->fxns->getNumFreeFrames(handle->obj->handle,
                    numFreeFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getNumFreeFrames",
                    status, "This function is not implemented in the"
                    " interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_getNumFreeFrames", status);

    return (status);
}


/*
 *  ======== FrameQ_getvNumFreeFrames ========
 * Get the number of free frames available in a FrameQBufMgr
 */
Int32 FrameQ_getvNumFreeFrames(FrameQ_Handle handle, UInt32 numFreeFrames[],
        UInt8 freeQId[], UInt8 numFreeQids)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_3trace(curTrace, GT_ENTER, "FrameQ_getvFreeNumFrames", handle,
               numFreeFrames, freeQId);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != numFreeFrames));
    GT_assert(curTrace, (NULL != freeQId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getvNumFreeFrames",
                status, "Module was not initialized!");
    }
    else {
#endif

        if (handle->fxns->getvNumFreeFrames != NULL) {
            status = handle->fxns->getvNumFreeFrames(handle->obj->handle,
                    numFreeFrames, freeQId, numFreeQids);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQ_getvNumFreeFrames", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_getvNumFreeFrames", status);

    return (status);
}

/*
 *  ======== FrameQ_control ========
 *  Perform implementation dependent operation
 */
Int32 FrameQ_control(FrameQ_Handle handle, Int32 cmd, Ptr arg)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_3trace(curTrace, GT_ENTER, "FrameQ_control", handle, cmd, arg);

    GT_assert(curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_control", status,
                "Module was not initialized!");
    }
    else {
#endif

        if (handle->fxns->control != NULL) {
            status = handle->fxns->control(handle->obj->handle, cmd, arg);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_control", status,
                    "This function is not implemented in the interface");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_control", status);

    return (status);
}

/*
 *  ======== FrameQ_Params_init ========
 *  Initialize config-params structure with defaults
 */
Void FrameQ_Params_init(FrameQ_Handle handle, FrameQ_Params *params)
{
    FrameQ_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_Params_init", handle, params);

    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_Params_init",
                FrameQ_E_INVALIDSTATE, "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_Params_init",
                FrameQ_E_FAIL, "Argument params is NULL!");
    }
    else {
#endif
        if (handle == NULL) {
            Memory_copy(params, &(FrameQ_state.defInstParams),
                         sizeof(FrameQ_Params));
        }
        else {
            obj = (FrameQ_Obj *)handle->obj;
            Memory_copy(params, &(obj->params), sizeof (FrameQ_Params));
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_0trace(curTrace, GT_LEAVE, "FrameQ_Params_init");
}


/*=============================================================================
 * Internal API
 *=============================================================================
 */
GateMP_Handle FrameQ_getGate(FrameQ_Handle handle)
{
    FrameQ_Object *     object;
    FrameQ_Obj    *     obj;
    FrameQ_ShMem_Handle shMemHandle;

    object = (FrameQ_Object *)handle;
    obj = (FrameQ_Obj *)(object->obj);
    shMemHandle = obj->handle;
    return(FrameQ_ShMem_getGate(shMemHandle));
}

/*
 *  ======== _FrameQ_create ========
 *  Internal function called by FrameQ_create when openFlag is FALSE.
 */
static inline Int32 _FrameQ_create(FrameQ_Handle handle, FrameQ_Params* params)
{
    Int32 status = FrameQ_S_SUCCESS;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    UInt32                  len;
    FrameQ_NameServerEntry  entry;
#endif
    FrameQ_ShMem_Handle     shMemHandle;
    FrameQ_ShMem_Params    *pShMemParams;
    FrameQ_Obj             *obj;

    GT_2trace(curTrace, GT_ENTER, "_FrameQ_create", handle,params);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQ_state.refCount),
            FRAMEQ_MAKE_MAGICSTAMP(0), FRAMEQ_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create", status,
                "Module was not initialized!");
    }
    else if (NULL == handle) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create", status,
                "obj of FrameQ_Object * is NULL!");

    }
    else if ( NULL == params) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create", status,
                "Params pointer is NULL!");
    }
    else {
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if name already exists in the name server */
        if (FrameQ_state.nameServer != NULL) {
            len = sizeof (FrameQ_NameServerEntry);
            status = NameServer_get(FrameQ_state.nameServer,
                    (String)params->commonCreateParams.name, &entry, &len,
                    NULL);
            if (status >= 0) {
                /* Already instance with the name exists */
                status = FrameQ_E_INST_EXISTS;
                GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create",
                        status, "Instance is already created and entry "
                        "exists in Nameserver!");
            }
            else if ((status != NameServer_E_NOTFOUND) && (status < 0)) {
                GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create",
                        status, "NameServer_get failed!");
            }
            else {
                /* There is no instance with that name */
                status = FrameQ_S_SUCCESS;
            }
        }
#endif
        if (status >= 0) {
            /* Based on the type of interface mentioned call create-specific
             * implementation.
             */
            switch (params->commonCreateParams.ctrlInterfaceType) {
                case FrameQ_INTERFACE_SHAREDMEM:
                    pShMemParams = (FrameQ_ShMem_Params *)params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    /* Check the size  of params */
                    if(pShMemParams->commonCreateParams.size
                        < sizeof(FrameQ_ShMem_Params)) {
                        /*! @retval NULL  Params passed is NULL
                         */
                        status = FrameQ_E_INVALIDARG;
                        GT_setFailureReason (curTrace, GT_4CLASS,
                                "_FrameQ_create", status, "commonCreateParams "
                                ".size is not valid!");
                    }
#endif
                    /* Create the shared memory based FrameQ  instance.*/
                    shMemHandle = FrameQ_ShMem_create(pShMemParams);
                    if (shMemHandle == NULL) {
                        status = FrameQ_E_FAIL;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "_FrameQ_create", status,
                                "Failed to create internal instance "
                                "of type FrameQ_ShMem!");
                    }
                    else {
                        /* Upcast the handle so that we can call Implementation
                         * specific API using IFrameQ.
                         */
                        /* Plugin the ShMem function table*/
                        handle->fxns = &(FrameQ_ShMem_fxns);
                        obj =  handle->obj;
                        obj->handle        = shMemHandle;
                        obj->interfaceType = FrameQ_INTERFACE_SHAREDMEM;
                        obj->objType       = FrameQ_DYNAMIC_CREATE;
                        /*  Get the instId  to update the toplevel object */
                        obj->notifyId = (UInt32)-1;
                    }
                    break;

                default:
                    status = FrameQ_E_INVALID_INTERFACE;
                    GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create",
                            status, "invalid interface type!");
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "_FrameQ_create", status);

    return (status);
}

/*!
 * @brief   Internal function called by FrameQ_create function when
 *          openFlag is  TRUE.
 *
 * @param   obj FrameQbufMgr internal object.
 */
static inline
Int32 _FrameQ_open(FrameQ_Object* obj, FrameQ_Params* params)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_assert(curTrace, (NULL != obj));
    GT_assert(curTrace, (NULL != params));

    return (status);
}

/*!
 * @brief Function to get the handle of the client notifyMgr used by
 *        the FrameQ instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_getCliNotifyMgrHandle (FrameQ_Handle handle)
{
    FrameQ_Obj             *obj;
    ClientNotifyMgr_Handle cliHandle;

    GT_1trace (curTrace,
               GT_ENTER,
              "_FrameQ_getCliNotifyMgrHandle",
               handle);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->obj)));

    obj = (FrameQ_Obj*)handle->obj;

    GT_assert(curTrace, (obj != NULL));

    if  (handle->fxns->getCliNotifyMgrHandle != NULL) {
        cliHandle = handle->fxns->getCliNotifyMgrHandle(obj->handle);
    }
    else {
        cliHandle = NULL;
    }

    GT_1trace (curTrace,
               GT_LEAVE,
               "_FrameQ_getCliNotifyMgrHandle",
               cliHandle);

    /*! @retval  valid handle  if API is successful*/
    return (cliHandle);
}

/*!
 * @brief Function to get the gate used in  client notifyMgr of the
 *        FrameQ instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_getCliNotifyMgrGate (FrameQ_Handle handle)
{
    FrameQ_Obj            *obj  ;
    GateMP_Handle            gate;
    GT_1trace (curTrace,
               GT_ENTER,
              "_FrameQ_getCliNotifyMgrGate",
               handle);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->obj)));

    obj = (FrameQ_Obj*)handle->obj;

    GT_assert(curTrace, (obj != NULL));

    if  (handle->fxns->getCliNotifyMgrHandle != NULL) {
        gate = handle->fxns->getCliNotifyMgrGate(obj->handle);
    }
    else {
        gate = NULL;
    }

    GT_1trace (curTrace,
               GT_LEAVE,
               "_FrameQ_getCliNotifyMgrGate",
               gate);

    /*! @retval  valid handle  if API is successful*/
    return (gate);
}

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_getCliNotifyMgrShAddr (FrameQ_Handle handle)
{
    FrameQ_Obj             *obj;
    Ptr                    shAddr;

    GT_1trace (curTrace,
               GT_ENTER,
              "_FrameQ_getCliNotifyMgrShAddr",
               handle);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->obj)));

    obj = (FrameQ_Obj*)handle->obj;

    GT_assert(curTrace, (obj != NULL));

    if  (handle->fxns->getCliNotifyMgrShAddr != NULL) {
        shAddr = handle->fxns->getCliNotifyMgrShAddr(obj->handle);
    }
    else {
        shAddr = NULL;
    }

    GT_1trace (curTrace,
               GT_LEAVE,
               "_FrameQ_getCliNotifyMgrShAddr",
               shAddr );

    /*! @retval  valid handle  if API is successful*/
    return (shAddr);
}

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_getCliNotifyMgrGateShAddr (FrameQ_Handle handle)
{
    FrameQ_Obj             *obj;
    Ptr                    shAddr;

    GT_1trace (curTrace,
               GT_ENTER,
              "_FrameQ_getCliNotifyMgrGateShAddr",
               handle);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->obj)));

    obj = (FrameQ_Obj*)handle->obj;

    GT_assert(curTrace, (obj != NULL));

    if  (handle->fxns->getCliNotifyMgrGateShAddr != NULL) {
        shAddr = handle->fxns->getCliNotifyMgrGateShAddr(obj->handle);
    }
    else {
        shAddr = NULL;
    }

    GT_1trace (curTrace,
               GT_LEAVE,
               "_FrameQ_getCliNotifyMgrGateShAddr",
               shAddr );

    /*! @retval  valid handle  if API is successful*/
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
 _FrameQ_setNotifyId (FrameQ_Handle handle, UInt32 notifyId)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQ_Obj             *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "_FrameQ_setNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace,
               (NULL != handle));

    obj = (FrameQ_Obj*)handle->obj;

    if  (handle->fxns->setNotifyId != NULL) {
        status = handle->fxns->setNotifyId(obj->handle, notifyId);
    }
    else {
        status = FrameQ_E_NOTIMPLEMENTED;
    }

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_setNotifyId", status);

    /*! @retval  FrameQ_S_SUCCESS  Successfully registered callback fucntion*/
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
 _FrameQ_resetNotifyId (FrameQ_Handle handle, UInt32 notifyId)
{
    Int32                   status       = FrameQ_S_SUCCESS;
    FrameQ_Obj             *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "_FrameQ_resetNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace, (NULL != handle));
    obj = (FrameQ_Obj*)handle->obj;

    if  (handle->fxns->resetNotifyId != NULL) {
        status = handle->fxns->resetNotifyId(obj->handle, notifyId);
    }
    else {
        status = FrameQ_E_NOTIMPLEMENTED;
    }

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_resetNotifyId", status);

    /*! @retval  FrameQ_S_SUCCESS  Successfully registered callback fucntion*/
    return (status);
}
