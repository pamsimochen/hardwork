/*
 *  @file   FrameQ.c
 *
 *  @brief  Front end  implementation of FrameQ module on RTOS side(DSP/BIOS).
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

#ifdef xdc_target__isaCompatible_64P
#include <ti/sysbios/family/c64p/Cache.h>
#else
#include <ti/sysbios/hal/Cache.h>
#endif
/*-------------------------    Generic includes  ---------------------------- */
#include <string.h>

/*-------------------------    IPC module specific includes ------------------*/
#include <ti/sdo/ipc/Ipc.h>



#include "ti/syslink/FrameQ.h"

#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/NameServer.h>

#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/inc/_FrameQ_ShMem.h>
#include <ti/syslink/inc/_FrameQ.h>

#include <ti/syslink/utils/Trace.h>

/* TODO: Should be in a header! */
extern FrameQ_Fxns FrameQ_ShMem_fxns;
/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*
 * @brief Structure for FrameQ module state
 */
typedef struct FrameQ_ModuleObject_Tag {
    UInt32              refCount;
    /* TBD : can not be atomic */
    NameServer_Handle     nameServer;
    /*!< Handle to the local NameServer used for storing FrameQ instance
     *   information.
     */
    IGateProvider_Handle  gate;
    /*!< Handle to lock for protection in NameServer*/
    List_Handle           objList;
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
typedef struct FrameQ_Object {
    FrameQ_Fxns *fxns;
   /*Pointer to implementation specific function table interface */
    FrameQ_Obj *obj;
    /*!< Pointer to the FrameQ internal object*/
} FrameQ_Object;
//typedef struct FrameQ_Object    FrameQ_Object;

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
    .refCount = 0,
    .defaultCfg.eventNo = FrameQ_NOTIFY_RESERVED_EVENTNO,
    .defInstParams.commonCreateParams.size = sizeof(FrameQ_Params),
    .defInstParams.commonCreateParams.ctrlInterfaceType = FrameQ_INTERFACE_NONE,
    .defInstParams.commonCreateParams.name = NULL,
    .defInstParams.commonCreateParams.openFlag = FALSE,
};

/*!
 *  @var    FrameQ_moduleCfg
 *
 *  @brief  FrameQ module configuration. This will be referred to by all
 *          delegates in order to get access to FrameQ module level
 *          configuration information.
 */
FrameQ_Config       FrameQ_moduleCfg;

/*!
 *  @var    Syslink_FrameQ_moduleCfg
 *
 *  @brief  C based FrameQ configuration
 */
extern FrameQ_Config       Syslink_FrameQ_moduleCfg;

/*!
 *  @var    FrameQ_module
 *
 *  @brief  Pointer to the FrameQ module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
FrameQ_ModuleObject * FrameQ_module = &FrameQ_state;


/* Reserved name used for name server instance of the FrameQ.
 * Note: Applications should not used this name for the instance creation
 */
#define FRAMEQ_NAMESERVERNAME  "FrameQ"

/*  NameServer handle for the FrameQ module */
Ptr gFrameQ_nsHandle; /* TODO: remove this.Hack for NameServer_getHandle issue in nameserver */

/* Internal API declarations */
Void FrameQ_Params_init(FrameQ_Handle handle,
        FrameQ_Params *params);
Int32 _FrameQ_create(FrameQ_Handle handle,
        const FrameQ_Params *params);

Int32 _FrameQ_open(FrameQ_Handle handle, const FrameQ_Params* params);

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

    GT_assert(curTrace, cfg != NULL);

    if (FrameQ_module->refCount == 0) {
        /* setup has not yet been called, return default static config */
        cfg->eventNo = FrameQ_NOTIFY_RESERVED_EVENTNO;
        cfg->maxInstances  = FrameQ_MAX_INSTANCES;
    }
    else {
        /* Return the currently configure parameters */
        memcpy(cfg, &FrameQ_module->cfg, sizeof(FrameQ_Config));
    }

    GT_0trace (curTrace, GT_LEAVE, "FrameQ_getConfig");
}


/*
 *  ======== FrameQ_setup ========
 */
Int32 FrameQ_setup(FrameQ_Config *cfg)
{
    Int32 status = FrameQ_S_SUCCESS;
    List_Params listParams;
    IArg key;
    Error_Block eb;
    Int32 tmpStatus;
    FrameQ_Config tmpCfg;
    NameServer_Params nsParams;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_setup", cfg);

    key = Gate_enterSystem();
    FrameQ_module->refCount++;
    if (FrameQ_module->refCount > 1) {
        status = FrameQ_S_ALREADYSETUP;
        Gate_leaveSystem(key);
    }
    else {
        Gate_leaveSystem(key);

        if (cfg == NULL) {
            FrameQ_getConfig(&tmpCfg);
            cfg = &tmpCfg;
        }
        memcpy(&FrameQ_moduleCfg, cfg, sizeof(FrameQ_Config));

        GT_assert(curTrace, (FrameQ_MAX_NAMELEN > 0));
        GT_assert(curTrace, (cfg->maxInstances > 0));
        /* Initialize the parameters */
        NameServer_Params_init(&nsParams);
        nsParams.maxNameLen        = FrameQ_MAX_NAMELEN;
        nsParams.maxRuntimeEntries = cfg->maxInstances;
        nsParams.checkExisting     = TRUE;
        nsParams.maxValueLen       = sizeof(FrameQ_NameServerEntry);

        /* Create the Name Server instance */
        FrameQ_module->nameServer = NameServer_create(FRAMEQ_NAMESERVERNAME,
                &nsParams);

        /* TODO:H - is this still necessary? */
        gFrameQ_nsHandle = FrameQ_state.nameServer;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (FrameQ_module->nameServer == NULL) {
            status = FrameQ_E_CREATE_NAMESERVER;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_setup", status,
                    "Failed to create FrameQ NameServer");
        }
        else {
#endif

            Error_init(&eb);

            /* Create the list */
            List_Params_init(&listParams);
            FrameQ_module->objList = List_create(&listParams, &eb);
            GT_assert(curTrace, (NULL != FrameQ_module->objList));
#if 0  /* Needed? */
            /* Copy the cfg */
            Memory_copy((Ptr)&FrameQ_state.cfg, (Ptr)cfg,
                    sizeof(FrameQ_Config));
#endif

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
                FrameQ_module->refCount = 0;
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
Int32 FrameQ_destroy(Void)
{
    Int32 status = FrameQ_S_SUCCESS;
    IArg key;

    GT_0trace(curTrace, GT_ENTER, "FrameQ_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->refCount == 0) {
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_destroy", status,
                "Module was not initialized!");
    }
    else {
#endif
        key = Gate_enterSystem();
        FrameQ_module->refCount--;
        if (FrameQ_module->refCount == 0) {
            Gate_leaveSystem(key);

            FrameQ_ShMem_destroy();

            /* TODO: module supplied gates needs to be finalized(?) */

            /* Delete the list object */
            List_delete(&FrameQ_module->objList);

            if (FrameQ_state.listLock != NULL) {
                GateMutex_delete((GateMutex_Handle*)&FrameQ_state.listLock);
            }
            if (FrameQ_state.gate != NULL) {
                GateMutex_delete((GateMutex_Handle*)&FrameQ_state.gate);
            }

            /* Delete name server instance created for this module,
             * if it is dynamically created
             */
            if (FrameQ_module->nameServer != NULL) {
                status = NameServer_delete(&FrameQ_module->nameServer);
                GT_assert(curTrace, (status >= 0));
            }

            /* Clear cfg area */
            memset((Ptr)&FrameQ_state.cfg, 0, sizeof(FrameQ_Config));
        }
        else {
            Gate_leaveSystem(key);
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

    /* Allocate memory for handle */
    handle = (FrameQ_Handle)Memory_calloc(NULL, sizeof(FrameQ_Object), 0,
            NULL);
    if (handle == NULL) {
        status = FrameQ_E_ALLOC_MEMORY;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create", status,
                "Memory allocation failed for handle!");
    }
    else {
        obj = (FrameQ_Obj *)Memory_calloc(NULL, sizeof(FrameQ_Obj), 0u,
                NULL);
        if (obj == NULL) {
            status = FrameQ_E_ALLOC_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create",
                status, "Memory allocation failed for internal object!");

            Memory_free(NULL, handle, sizeof(FrameQ_Object));
            handle = NULL;
        }
        else {
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
        }
    }
    if (status >= 0) {
        /* Populate the params member */
        memcpy((Ptr)&obj->params, (Ptr)params, sizeof(FrameQ_Params));

        /* Put in the local list */
        key0 = IGateProvider_enter(FrameQ_state.gate);

        List_elemClear(&obj->listElem);
        List_put((List_Handle)FrameQ_state.objList, &(obj->listElem));

        IGateProvider_leave(FrameQ_state.gate, key0);
    }
    else {
         /* Failed to create/open the instance. Clean up */
        if (obj != NULL) {
            Memory_free(NULL, obj, sizeof(FrameQ_Obj));
        }
        if (handle != NULL) {
            Memory_free(NULL, handle, sizeof(FrameQ_Object));
            handle = NULL;
        }
    }

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
    Int32                   key;
    UInt32                  index;
    Error_Block             eb;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_open", handlePtr, instOpenParams);

    if ((handlePtr == NULL) || (openParams == NULL)) {
        status = FrameQ_E_INVALIDARG;
    }
    else {
        Error_init(&eb);
        /* Get implementation type by querying  the name server and call
         * Specific implementation's open call.
         */
        nameLen = strlen((String)openParams->commonOpenParams.name);
        if (openParams->commonOpenParams.name != NULL) {
            /* If name is provided use name based  open only */
            openParams->commonOpenParams.sharedAddr = NULL;
        }

        if ((nameLen == 0)
            && (openParams->commonOpenParams.sharedAddr == NULL)) {
            /*if  both name and shared addr is not provided return error */
            status = FrameQ_E_INVALIDARG;
        }
        else if (((nameLen > 0)
                     && ( nameLen <= (FrameQ_MAX_NAMELEN-1))
                     && (NULL != FrameQ_module->nameServer))) {
            len = sizeof (FrameQ_NameServerEntry);
            status = NameServer_get ((NameServer_Handle)FrameQ_module->nameServer,
                                     openParams->commonOpenParams.name,
                                     &entry, &len, NULL);
            if (status < 0) {
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

    }

    if (status >= 0) {
        object = handle;
        handle->fxns = fxns;
        obj = object->obj;
        obj->handle        = impHandle;
        obj->interfaceType = interfaceType;
        obj->objType       = FrameQ_DYNAMIC_OPEN;
        *handlePtr         = handle;
    }
    else {
       *handlePtr     = NULL;
    }

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

    if (pHandle == NULL) {
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
        handle = *pHandle;
        obj = (FrameQ_Obj *) handle->obj;
        GT_assert (curTrace, (NULL != obj));

        switch (obj->objType) {

            case FrameQ_DYNAMIC_OPEN:
                key0 = IGateProvider_enter(FrameQ_state.gate);
                /* Remove it from the local list */
                List_remove((List_Handle)FrameQ_state.objList,
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
                List_remove((List_Handle)FrameQ_state.objList,
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

    }

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

    if (handle == NULL) {
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
    }

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
    GT_assert(curTrace, (freeQId != NULL));
    GT_assert(curTrace, (numFrames != NULL));
    if (handle == NULL) {
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
    }

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

    if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQ_free", status,
                "handle passed is NULL!");
    }
    else {
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
    }

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
    GT_assert(curTrace, (numFrames != NULL));

    if (handle == NULL) {
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
    }

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

    if (handle == NULL) {
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
    }

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
    GT_assert(curTrace, (NULL != numFrames));

    if (handle == NULL) {
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
    }

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

    if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                "handle passed is NULL!");
    }
    else if (pframe == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_get", status,
                "framePtr passed is NULL!");
    }
    else {
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
    }

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
    GT_assert(curTrace, (NULL != filledQueueId));
    GT_assert(curTrace, (NULL != numFrames));

    if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                "handle passed is NULL!");
    }
    else if (pframe == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                "framePtr passed is NULL!");
    }
    else {

        if (handle->fxns->getv != NULL) {
            status = handle->fxns->getv(handle->obj->handle, pframe,
                    filledQueueId, numFrames);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_getv", status,
                    "This function is not implemented in the interface");
        }
    }

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

    if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_registerNotifier",
                status, "handle passed is NULL!");
    }
    else {
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
    }

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

    if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_unregisterNotifier",
                status, "handle passed is NULL!");
    }
    else {
        if (handle->fxns->unregisterNotifier != NULL) {
            status = handle->fxns->unregisterNotifier(handle->obj->handle);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "FrameQ_unregisterNotifier", status,
                    "This function is not implemented in the interface");
        }
    }

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
    GT_assert(curTrace, (dupedFrame != NULL));


    if (handle->fxns->dup != NULL) {
        status = handle->fxns->dup(handle->obj->handle, frame, dupedFrame);
    }
    else {
        status = FrameQ_E_NOTIMPLEMENTED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_dup", status,
                "This function is not implemented in the interface");
    }

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

    if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_sendNotify", status,
                "handle passed is NULL!");
    }
    else {
        if (handle->fxns->sendNotify != NULL) {
            status = handle->fxns->sendNotify(handle->obj->handle, msg);
        }
        else {
            status = FrameQ_E_NOTIMPLEMENTED;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_sendNotify",
                    status, "This function is not implemented in the"
                    " interface");
        }
    }

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
    GT_assert(curTrace, (NULL != filledQId));

    GT_assert(curTrace, (NULL != numFilledQids));

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
    GT_assert(curTrace, (0 != numFreeQids));

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

    if (handle->fxns->control != NULL) {
        status = handle->fxns->control(handle->obj->handle, cmd, arg);
    }
    else {
        status = FrameQ_E_NOTIMPLEMENTED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_control", status,
                "This function is not implemented in the interface");
    }
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

    if (handle == NULL) {
        memcpy(params, &(FrameQ_state.defInstParams),
                     sizeof(FrameQ_Params));
    }
    else {
        obj = (FrameQ_Obj *)handle->obj;
        memcpy(params, &(obj->params), sizeof (FrameQ_Params));
    }

    GT_0trace(curTrace, GT_LEAVE, "FrameQ_Params_init");
}


/*=============================================================================
 * Helper APIs
 *=============================================================================
 */

/*
 *  ======== FrameQ_getExtendedHeaderPtr ========
 * API  to get the pointer to the extended header.Apps has to use this API to
 * get  pointer  to their application defined exteneded header that starts after
 * the bsee frame header.
 */
Ptr FrameQ_getExtendedHeaderPtr(FrameQ_Frame frame)
{
    Ptr    extHeaderPtr = NULL;
    UInt32 offset;

    GT_assert(curTrace, (NULL != frame));

    offset = (  sizeof (FrameQ_FrameHeader)
              + (   (frame->numFrameBuffers - 1)
                  * sizeof (FrameQ_ShMem_FrameBufInfo)));

    extHeaderPtr = (Ptr) ( (UInt32)frame + offset);

    return (extHeaderPtr);
}

/*
 *  ======== FrameQ_ShMem_getNumFrameBuffers ========
 *  API to find out the number of frame buffers in a frame.
 */
UInt32 FrameQ_getNumFrameBuffers(FrameQ_Frame frame)
{
    GT_assert(curTrace, (NULL != frame));

    return (frame->numFrameBuffers);
}

/*
 *  ======== FrameQ_getFrameBuffer ========
 *  API to retrieve framebuffer  identified by frameBufNum from a given frame.
 * Here no validation is performed for frameBufNum. User is expected to pass the
 * right value for it.
 * ( APPs can use getNumFrameBuffers API to know the number of frame buffers
 *  associated with the frame)
 */
Ptr FrameQ_getFrameBuffer(FrameQ_Frame frame, UInt32 frameBufNum)
{
    Ptr                  frmBuffer = NULL;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);

    frmBuffer = (Ptr) frameBufInfo[frameBufNum].bufPtr;

    return (frmBuffer);
}

/*
 *  ======== FrameQ_getFrameBufSize ========
 *  API to get the size of the framebuffer.
 */
UInt32
FrameQ_getFrameBufSize(FrameQ_Frame frame, UInt32 frameBufNum )
{
    UInt32               size;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
    size = frameBufInfo[frameBufNum].bufSize;

    return (size);
}


/*
 *  ======== FrameQ_getFrameBufValidSize ========
 *  API to get the valid data size of a framebuffer  identified by frameBufNum
 *  in a given frame.
 */
UInt32
FrameQ_getFrameBufValidSize(FrameQ_Frame frame, UInt32 frameBufNum )
{
    UInt32               size;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
    size = frameBufInfo[frameBufNum].validSize;

    return (size);
}

/*
 *  ======== FrameQ_getFrameBufDataStartOffset ========
 *  API to get the valid data start offset in framebuffer  identified by
 *  frameBufNum in a given frame.
 */
UInt32
FrameQ_getFrameBufDataStartOffset(FrameQ_Frame frame,
                                        UInt32         frameBufNum )
{
    UInt32               offSet;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers >= frameBufNum));

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
    offSet = frameBufInfo[frameBufNum].startOffset;

    return (offSet);
}

/*
 *  ======== FrameQ_setFrameBufValidSize ========
 *  API to set the valid data size of a framebuffer  identified by
 *  frameBufNum in  given frame.
 */
Int32
FrameQ_setFrameBufValidSize(FrameQ_Frame frame,
                            UInt32       frameBufNum,
                            UInt32       validDataSize )
{
    Int32                      status       = FrameQ_S_SUCCESS;
    FrameQ_FrameBufInfo       *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers > frameBufNum));

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);

    if (frameBufInfo[frameBufNum].bufSize <
           (validDataSize + frameBufInfo[frameBufNum].startOffset)) {
        status = FrameQ_E_INVALIDARG;
    }
    else {
        frameBufInfo[frameBufNum].validSize = validDataSize;
    }
    return (status);
}

/*
 *  ======== FrameQ_setFrameBufDataStartOffset ========
 *  API to set the valid data start offset of a framebuffer  identified by
 *  frameBufNum in a given frame.
 */
Int32
FrameQ_setFrameBufDataStartOffset (FrameQ_Frame  frame,
                                   UInt32        frameBufNum,
                                   UInt32        dataStartOffset)
{
    Int32                      status       = FrameQ_S_SUCCESS;
    FrameQ_FrameBufInfo        *frameBufInfo;

    GT_assert(curTrace, (NULL != frame));
    GT_assert(curTrace, (frame->numFrameBuffers > frameBufNum));

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);

    if (frameBufInfo[frameBufNum].bufSize <
            (dataStartOffset + frameBufInfo[frameBufNum].validSize)) {
        status = FrameQ_E_INVALIDARG;
    }
    else {
        frameBufInfo[frameBufNum].startOffset = dataStartOffset;
    }
    return (status);
}

/*
 *  ======== FrameQ_isSupportedInterface ========
 *  API to to find out the interface type is supported .
 */
Int32 FrameQ_isSupportedInterface(UInt32 type)
{
    Int32  status = TRUE;

    switch (type)
    {
        case FrameQ_INTERFACE_SHAREDMEM:
            /* Supported interface */
        break;
        default:
            /* unsupported interface type specifiec */
            status = FALSE;
    }

    return (status);
}

/*
 *  ======== _FrameQ_create ========
 *  Internal function called by FrameQ_create when openFlag is FALSE.
 */
Int32 _FrameQ_create(FrameQ_Handle handle, const FrameQ_Params *params)
{
    Int32 status = FrameQ_S_SUCCESS;
    UInt32                  len;
    FrameQ_NameServerEntry  entry;
    FrameQ_ShMem_Handle     shMemHandle;
    FrameQ_ShMem_Params    *pShMemParams;
    FrameQ_Obj             *obj;

    GT_2trace(curTrace, GT_ENTER, "_FrameQ_create", handle,params);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (params != NULL));

    /* Check if name already exists in the name server */
    if (FrameQ_module->nameServer != NULL) {
        len = sizeof (FrameQ_NameServerEntry);
        status = NameServer_get((NameServer_Handle)FrameQ_module->nameServer,
                (String)params->commonCreateParams.name, &entry, &len,
                NULL);
        if (status >= 0) {
            /* Already instance with the name exists */
            status = FrameQ_E_INST_EXISTS;
            GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create",
                    status, "Instance is already created and entry "
                    "exists in Nameserver!");
        }
        else if (status == NameServer_E_FAIL) {
            /* Error happened in NameServer. Pass the error up. */
            status = FrameQ_E_FAIL;
        }
        else {
            /* There is no instance with that name */
            status = FrameQ_S_SUCCESS;
        }
    }

    if (status >= 0) {
        /* Based on the type of interface mentioned call create-specific
         * implementation.
         */
        switch (params->commonCreateParams.ctrlInterfaceType) {
            case FrameQ_INTERFACE_SHAREDMEM:
                pShMemParams = (FrameQ_ShMem_Params *)params;

                GT_assert(curTrace, (pShMemParams->commonCreateParams.size >=
                        sizeof(FrameQ_ShMem_Params)));

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
                }
                break;

            default:
                status = FrameQ_E_INVALID_INTERFACE;
                GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create",
                        status, "invalid interface type!");
        }
    }


    GT_1trace(curTrace, GT_LEAVE, "_FrameQ_create", status);

    return (status);
}

Int32 _FrameQ_open(FrameQ_Object* obj, const FrameQ_Params* params)
{
    Int32 status = FrameQ_S_SUCCESS;

    GT_assert(curTrace, (NULL != obj));
    GT_assert(curTrace, (NULL != params));

    return (status);
}
