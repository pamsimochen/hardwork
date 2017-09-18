/*
 *  @file   FrameQBufMgr.c
 *
 *  @brief      Implements FrameQBufMgr functions.
 *
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
#if defined(SYSLINK_BUILDOS_LINUX)
#include <linux/string.h>
#else
#include <string.h>
#endif

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

/* Utilities & OSAL headers */

/* TODO, change MultiProc_MAXPROCESSORS to MultiProc_getNumProcessors()? */
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/ListMP.h>

#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/FrameQDefs.h>

#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>
#include <ti/syslink/ipc/_FrameQBufMgr_ShMem.h>
#include <ti/syslink/ipc/_FrameQBufMgr.h>

#include <ti/syslink/ipc/IFrameQBufMgr.h>

/** ============================================================================
 *  @const  IPC_BUFFER_ALIGN
 *
 *  @desc   Macro to align a number.
 *          x: The number to be aligned
 *          y: The value that the number should be aligned to.
 *  ============================================================================
 */
#define IPC_BUFFER_ALIGN(x, y) (UInt32)((UInt32)((x + y - 1) / y) * y)

/*=============================================================================
 * Macros
 *=============================================================================
 */

/*! @brief Macro to make a correct module magic number with refCount */
#define FRAMEQBUFMGR_MAKE_MAGICSTAMP(x) ((FrameQBufMgr_MODULEID << 12u) | (x))

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*
 * @brief Structure for FrameQBufMgr module state
 */
typedef struct FrameQBufMgr_ModuleObject_Tag {
    Atomic                      refCount;
    /*!< Reference count */
    NameServer_Handle           nameServer;
    /*!< Handle to the local NameServer used for storing FrameQBufMgrBufMgr instance
     *   information.
     */
    UInt32                      nameServerType;
    /*!< Name server create type. Dynamic or static*/
    IGateProvider_Handle        gate;
    /*!< Handle for module wide protection*/
    Bool                        gateType;
    /*!< Flag denotes if gate is dynamically created/opened one */
    List_Object                 objList;
    /* List holding created objects */
    IGateProvider_Handle        listLock;
    FrameQBufMgr_Config         cfg;
    /*!<  Current module wide config values*/
    FrameQBufMgr_Config         defaultCfg;
    /*!< Default config values*/
    FrameQBufMgr_Params     defaultInstParams;
    /*!< Default instance creation parameters.
     *   Same for all the implementations.
     */
     Bool isInstNoValid[ClientNotifyMgr_maxInstances];
    /*  Create call will check this flags and sets first invalid index to valid
     *  and  assigns this index to the first byte of obj->Id
     */
} FrameQBufMgr_ModuleObject;

/*
 * @brief Structure defining internal object for the FrameQBufMgrBufMgr
 */
typedef struct FrameQBufMgr_Obj_Tag {
    List_Elem               listElem;
    /*!< Used for creating a linked list */
    Ptr                     handle;
    /* Handle to implementation specific instance */
    UInt32                  instId;
    /*!< Id of the instance*/
    UInt32                  notifyId;
    /*!<  Location to  store the key returned by the
     *    ClientNotiftmgr_registerClient call
     */
    UInt32                  objType;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32                  ctrlInterfaceType;
    /*!<  Type of the interface */
    UInt32                  refCount;

    Char                    name[32];
    /*!< Name of the instance */
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
    Bool                    isRegistered ;
    /*!< Indicates if client has registered a call back function */
    Ptr                         top;
    /* Pointer to the top Object */
    FrameQBufMgr_Params          params;
    /*!< Instance  config params */
} FrameQBufMgr_Obj;

/*
 * @brief Structure defining object for the FrameQBufMgr.
 */
typedef struct FrameQBufMgr_Object{
    FrameQBufMgr_Obj *obj;
    /*!< Pointer to the FrameQBufMgr internal object*/
   /*TODO: Add function table interface */
} FrameQBufMgr_Object;

/*=============================================================================
 * Globals
 *=============================================================================
 */
/*!
 *  @var    FrameQBufMgr_module_obj
 *
 *  @brief  FrameQBufMgr_module state variable.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
FrameQBufMgr_ModuleObject FrameQBufMgr_module_obj = {
    .nameServer                   = NULL,
    .defaultCfg.eventNo           = FrameQBufMgr_NOTIFY_RESERVED_EVENTNO
};


/*!
 *  @var    FrameQBufMgr_module
 *
 *  @brief  Pointer to FrameQBufMgr_module_obj
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
FrameQBufMgr_ModuleObject* FrameQBufMgr_module = &FrameQBufMgr_module_obj;

/*  NameServer handle for the FrameQBufMgr module */
Ptr gFrameQBufMgr_nsHandle;
/*=============================================================================
 * Forward declarations of internal Functions
 *=============================================================================
 */
inline Int32 FrameQBufMgr_isSupportedInterface(UInt32 type);

Void FrameQBufMgr_Params_init(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Params *params);
static inline
Int32 _FrameQBufMgr_create(FrameQBufMgr_Object* obj,
        FrameQBufMgr_Params* params);

/*=============================================================================
 * APIs
 *=============================================================================
 */
/*!
 *  @brief  Function to initialize the config parameter structure  with default
 *          values.
 *
 *  @param  cfg
 *          Parameter structure to return the config parameters.
 *
 *  @sa    None.
 */
Int32 FrameQBufMgr_getConfig(FrameQBufMgr_Config *cfg)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "FrameQBufMgr_getConfig");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_getConfig",
                status, "cfg argument is NULL");
    }
    else {
#endif
        if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
                FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
            /* Return the default static config values */
            Memory_copy((Ptr)cfg, (Ptr)&(FrameQBufMgr_module->defaultCfg),
                    sizeof(FrameQBufMgr_Config));
        }
        else {
            Memory_copy ((Ptr) cfg,
                         (Ptr) &(FrameQBufMgr_module->cfg),
                         sizeof (FrameQBufMgr_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_getConfig", status);

    return (status);
}

/*!
 *  @brief  Function to setup the FrameQBufMgr module.
 *
 *          This function sets up the FrameQBufMgr module. This function must
 *          be called before any other instance-level APIs can be invoked.
 *          Module-level configuration needs to be provided to this
 *          function. If the user wishes to change some specific config
 *          parameters, then FrameQBufMgr_getConfig can be called to get the
 *          configuration filled with the default values. After this, only
 *          the required configuration values can be changed. If the user
 *          does not wish to make any change in the default parameters, the
 *          application can simply call setup with NULL parameters.
 *          The default parameters would get automatically used.
 *  @param  cfg
 *          Optional FrameQBufMgr module configuration. If provided as
 *          NULL, default configuration is used.
 *
 *  @sa    None.
 */
Int32 FrameQBufMgr_setup(FrameQBufMgr_Config *cfg)
{
    Int32                  status = FrameQBufMgr_S_SUCCESS;
    Error_Block            eb;
    NameServer_Params      nameServerParams;
    Int32                  tmpStatus;
    FrameQBufMgr_Config    tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        FrameQBufMgr_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (cfg == NULL) {
        FrameQBufMgr_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&FrameQBufMgr_module->refCount,
                            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&FrameQBufMgr_module->refCount)
        != FRAMEQBUFMGR_MAKE_MAGICSTAMP(1u)) {
        status = FrameQBufMgr_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "FrameQBufMgr Module already initialized!");
    }
    else {
        /* Create a locate gate */
        FrameQBufMgr_module->gate = (IGateProvider_Handle)
                GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (FrameQBufMgr_module->gate == NULL) {
            GT_assert(curTrace, (FrameQBufMgr_module->gate != NULL));
            status = FrameQBufMgr_E_CREATE_GATEMP ;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_setup",
                    status, "GateMutex_create()Failed to create module gate!");
        }
        else {
#endif
           /* Check the params required to create the name server instance */
            if (FrameQBufMgr_module->nameServer == NULL) {
                /* Create NameServer.
                 * Let NameServer create a gate for it
                 */
                /* Get the default params for  the Name server */
                NameServer_Params_init(&nameServerParams);
                /* Update the nameServerParams  as per FrameQ requirements */
                nameServerParams.maxRuntimeEntries = cfg->maxInstances; /* max instances on this processor */
                nameServerParams.checkExisting     = TRUE; /* Check if exists */
                /* Length of  the FrameQ instance */
                nameServerParams.maxNameLen        = FrameQBufMgr_MAXNAMELEN;
                nameServerParams.maxValueLen       =
                                         sizeof (FrameQBufMgr_NameServerEntry);

                /* Create the Name Server instance */
                FrameQBufMgr_module->nameServer = NameServer_create(
                                                    FRAMEQBUFMGR_NAMESERVERNAME,
                                                    &nameServerParams);
#if !defined (SYSLINK_BUILD_OPTIMIZE)
                if(NULL == FrameQBufMgr_module->nameServer) {
                    status = FrameQBufMgr_E_CREATE_NAMESERVER;
                    /*! @retval FrameQBufMgr_E_CREATE_NAMESERVER
                     * Failed to create NameServer */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgr_setup",
                                         status,
                                         "Failed to create NameServer!");
                    /* Decrement the reference count */
                    Atomic_dec_return (&FrameQBufMgr_module->refCount);
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    gFrameQBufMgr_nsHandle = FrameQBufMgr_module->nameServer;
                    FrameQBufMgr_module->nameServerType =
                                                FrameQBufMgr_DYNAMIC_CREATE;
#if !defined (SYSLINK_BUILD_OPTIMIZE)
                }
#endif
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif

        if (status >= 0) {
            /* Construct the list object */
            List_construct (&FrameQBufMgr_module->objList, NULL);
            /* Copy the cfg */
            Memory_copy ((Ptr) &FrameQBufMgr_module->cfg,
                         (Ptr) cfg,
                         sizeof (FrameQBufMgr_Config));
            /* Call all the implementataion specific setups */
            tmpStatus = FrameQBufMgr_ShMem_setup(cfg);
            if (tmpStatus < 0) {
                status = tmpStatus;
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_setup", status);

    /*!< FrameQBufMgr_S_SUCCESS if module setup is done successfully */
    return (status);
}


/*!
 *  @brief  Function to destroy(finalize) the FrameQBufMgr module.
 *
 */
Int32 FrameQBufMgr_destroy(Void)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    Int32 tmpStatus;

    GT_0trace (curTrace, GT_ENTER, "FrameQBufMgr_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE Module was not initialized */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&FrameQBufMgr_module->refCount)
            == FRAMEQBUFMGR_MAKE_MAGICSTAMP(0)) {

            /* TODO: Delete instances which are not finalized */


            /* Call all the implementation specific destroys */
            tmpStatus = FrameQBufMgr_ShMem_destroy();
            if (tmpStatus < 0) {
                status = tmpStatus;
            }
            /* Delete  name server instance created  for this module,
             * if it is dynamically created.
             */
            if (   (FrameQBufMgr_module->nameServer != NULL)
                && (    FrameQBufMgr_module->nameServerType
                    ==  FrameQBufMgr_DYNAMIC_CREATE)) {
                NameServer_delete (&FrameQBufMgr_module->nameServer);
                FrameQBufMgr_module->nameServer = NULL;
            }

            /* Destruct the list object */
            List_destruct (&FrameQBufMgr_module->objList);

            if (FrameQBufMgr_module->gate != NULL) {
                /* Delete the module gate. */
                status = GateMutex_delete ((GateMutex_Handle*)&FrameQBufMgr_module->gate);
                GT_assert (curTrace, (status >= 0));
            }
            /* Clear cfg area */
            Memory_set ((Ptr) &FrameQBufMgr_module->cfg,
                        0,
                        sizeof (FrameQBufMgr_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_destroy", status);

    return (status);
}

/*!
 * @brief Function to create a FrameQBufMgr instance
 *
 * @param  params Instance param structure.
 */
FrameQBufMgr_Handle FrameQBufMgr_create(Ptr vparams)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    Int32                         tmpStatus;
    IArg                          key0;
    FrameQBufMgr_Handle           handle         = NULL;
    FrameQBufMgr_Obj             *obj            = NULL;
    FrameQBufMgr_Params          *params = (FrameQBufMgr_Params*)vparams;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_create", params);

    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_create", status,
                "Module was not initialized!");
    }
    else if (NULL == params) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_create",
                status, "Params pointer is NULL!");
    }
    else {
#endif
        /* Create the handle */
        /* Allocate memory for handle */
        handle = (FrameQBufMgr_Handle)Memory_calloc(NULL,
                sizeof(FrameQBufMgr_Object), 0u, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle == NULL) {
            status = FrameQBufMgr_E_ALLOC_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_create",
                    status, "Memory allocation failed for handle!");
        }
        else {
#endif
            obj = (FrameQBufMgr_Obj *)Memory_calloc(NULL,
                    sizeof (FrameQBufMgr_Obj), 0u, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj == NULL) {
                status = FrameQBufMgr_E_ALLOC_MEMORY;
                Memory_free (NULL, handle, sizeof(FrameQBufMgr_Object));
                handle = NULL;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_create",
                        status, "Memory alloc failed for internal object!");
            }
            else {
#endif
                handle->obj = obj;
                obj->top    = handle;
                if (params->commonCreateParams.openFlag == TRUE) {
                    /* nothing to do */
                }
                else if (params->commonCreateParams.openFlag == FALSE) {
                    tmpStatus = _FrameQBufMgr_create (handle, params);
                    if (tmpStatus < 0) {
                        status = tmpStatus;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "FrameQBufMgr_create", status,
                                "Failed to create instance !");
                   }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif

        if (status >= 0) {
            /* Populate the params member */
            Memory_copy((Ptr) &obj->params, (Ptr)params,
                    sizeof(FrameQBufMgr_Params));
            /* Put in the local list */
            key0 = IGateProvider_enter (FrameQBufMgr_module->gate);
            List_elemClear (&obj->listElem);
            List_put((List_Handle)&FrameQBufMgr_module->objList,
                    &(obj->listElem));
            IGateProvider_leave (FrameQBufMgr_module->gate, key0);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Failed to crate /open the instance. Do clean up*/
        else {
            if (obj != NULL ) {
                Memory_free (NULL, obj, sizeof (FrameQBufMgr_Obj));
            }
            if (handle != NULL) {
                Memory_free (NULL, handle, sizeof (FrameQBufMgr_Object));
                handle = NULL;
            }
        }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_create", handle);

    return (handle);
}

/*
 *  ======== FrameQBufMgr_delete ========
 *  Delete the created FrameQBufMgr instance
 */
Int32 FrameQBufMgr_delete(FrameQBufMgr_Handle * pHandle)
{
    Int32                         status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Handle           handle;
    IArg                          key0;
    FrameQBufMgr_Obj             *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_delete", pHandle);

    GT_assert(curTrace, (pHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE Module was not initialized */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_delete", status,
                "Module was not initialized!");
    }
    else if (pHandle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL*/
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_delete", status,
                "pHandle passed is NULL!");
    }
    else if (*pHandle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL*/
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_delete", status,
                "*pHandle passed is NULL!");
    }
    else {
#endif
        handle = *pHandle;
        obj = (FrameQBufMgr_Obj *) handle->obj;
        GT_assert (curTrace, (NULL != obj));

        switch (obj->objType) {

        case FrameQBufMgr_DYNAMIC_OPEN:
        {
            key0 = IGateProvider_enter(FrameQBufMgr_module->gate);

            /* Remove it from the local list */
            List_remove((List_Handle)&FrameQBufMgr_module->objList,
                    &obj->listElem);

            IGateProvider_leave(FrameQBufMgr_module->gate, key0);

            /* Close the implementation specific handle and delete the top level
             * Object.
             */
            if (obj->handle != NULL) {
                switch (obj->ctrlInterfaceType) {
                    case FrameQBufMgr_INTERFACE_SHAREDMEM:
                    {
                        FrameQBufMgr_ShMem_close(
                                      (FrameQBufMgr_ShMem_Handle*)&obj->handle);
                    }
                    break;

                    default:
                    break;
                }
            }
            /* Now free the handle */
            Memory_free (NULL, obj, sizeof (FrameQBufMgr_Obj));
            obj = NULL;
            Memory_free (NULL, handle, sizeof (FrameQBufMgr_Object));
            handle = NULL;
            break;
        }

        case FrameQBufMgr_DYNAMIC_CREATE:
        {
            key0 = IGateProvider_enter (FrameQBufMgr_module->gate);
            /* Remove it from the local list */
            List_remove ((List_Handle) &FrameQBufMgr_module->objList,
                         &obj->listElem);
            IGateProvider_leave (FrameQBufMgr_module->gate, key0);
            /* Delete the implementation specific handle and delete the top
             * level Object.
             */
            if (obj->handle != NULL) {
                IFrameQBufMgr_delete((IFrameQBufMgr_Handle*)&obj->handle);
            }

            /* Now free the handle */
            Memory_free (NULL, obj, sizeof (FrameQBufMgr_Obj));
            obj = NULL;
            Memory_free (NULL, handle, sizeof (FrameQBufMgr_Object));
            handle = NULL;


            break;
        }

       default:
            status = FrameQBufMgr_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_delete",
                                 status,
                                 "Invalid Handle!");
            break;
        }
#if !defined (SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_delete", status);

    return (status);
}


/*
 *  ======== FrameQBufMgr_open ========
 *  Open a created FrameQBufMgr instance
 */
Int32 FrameQBufMgr_open(FrameQBufMgr_Handle *handlePtr, Ptr instOpenParams)
{
    Int32                        status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Handle          frmQBufMgrHandle = NULL;
    Ptr                          impHandle = NULL;
    UInt32                       instId = (UInt32)-1;
    FrameQBufMgr_Obj           * obj       = NULL;
    FrameQBufMgr_Params          params;
    FrameQBufMgr_NameServerEntry entry;
    FrameQBufMgr_NameServerEntry *pentry;
    UInt32                       ctrlInterfaceType;
    UInt32                       bufInterfaceType;
    UInt8                        nameLen;
    IArg                         key;
    UInt32                       len;
    UInt32                       index;
    FrameQBufMgr_OpenParams      *openParams = (FrameQBufMgr_OpenParams*)
                                                                 instOpenParams;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_open", handlePtr,
            instOpenParams);

    GT_assert(curTrace, (handlePtr != NULL));
    GT_assert(curTrace, (instOpenParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open", status,
                "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open", status,
                "handle is NULL!");
    }
    else if (instOpenParams == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open", status,
                "instOpenParams passed is null.");
    }
    else if ((String_len(openParams->commonOpenParams.name) == 0) &&
              (openParams->commonOpenParams.sharedAddr == NULL)) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open", status,
                "Either name or sharedaddr must be non null.");
    }
    else {
#endif
        nameLen = strlen((String)openParams->commonOpenParams.name);

        if (((nameLen > 0) && (nameLen <= (FrameQBufMgr_MAXNAMELEN-1))
                && (NULL != FrameQBufMgr_module->nameServer))) {
            len = sizeof(FrameQBufMgr_NameServerEntry);
            status = NameServer_get(FrameQBufMgr_module->nameServer,
                    (String)openParams->commonOpenParams.name, &entry, &len,
                    NULL);
            if (status < 0) {
                status = FrameQBufMgr_E_NOTFOUND;
            }
            else {
                ctrlInterfaceType = entry.ctrlInterfaceType;
                bufInterfaceType  = entry.bufInterfaceType;
            }
        }
        else if (openParams->commonOpenParams.sharedAddr != NULL) {
            pentry = (FrameQBufMgr_NameServerEntry*)
                    openParams->commonOpenParams.sharedAddr;
            /* Assert that sharedAddr is cache aligned */
            index = SharedRegion_getId(pentry);

            /* Assert that the buffer is in a valid shared region */
            GT_assert(curTrace, index != SharedRegion_INVALIDREGIONID);

            GT_assert(curTrace, ((UInt32)pentry %
                    SharedRegion_getCacheLineSize(index)) == 0);

            if (SharedRegion_isCacheEnabled(index))  {
                /* Invalidating the entry located in shared memory. The memory
                 * provided by this entry should be in cache aligend size
                 */
                key = Gate_enterSystem();
                Cache_inv (pentry, sizeof(FrameQBufMgr_NameServerEntry),
                        Cache_Type_ALL, TRUE);
                Gate_leaveSystem(key);
            }
            ctrlInterfaceType = pentry->ctrlInterfaceType;
            bufInterfaceType  = pentry->bufInterfaceType;
        }
        else {
            status = FrameQBufMgr_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open",
                    status, "Either name or sharedaddr must be non null.");
        }

        if (status >= 0) {
             /* Check the interface type and if it is not supported
              * return failure.
              */
            if (!FrameQBufMgr_isSupportedInterface(ctrlInterfaceType)) {
                status = FrameQBufMgr_E_INVALID_INTERFACE;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open",
                        status, "Unsupported interface type.");
            }
            else if (!FrameQBufMgr_isSupportedBufInterface(ctrlInterfaceType,
                    bufInterfaceType)) {
                status = FrameQBufMgr_E_INVALID_BUFINTERFACETYPE;
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_open",
                        status, "Unsupported buf interface type.");
            }
            else {
                /* Get the default params */
                FrameQBufMgr_Params_init(NULL, &params);
                /* Name is string pointer in CreateParams*/
                params.commonCreateParams.name = (String)
                        openParams->commonOpenParams.name;
                /* Opening the instance  */
                params.commonCreateParams.openFlag = TRUE;

                frmQBufMgrHandle = FrameQBufMgr_create(&params);
                if (frmQBufMgrHandle == NULL) {
                    status = FrameQBufMgr_E_FAIL;
                }
                else {
                    /* Open handle to the implementation specific handle
                     * and plug in to the top level FrameQ object.
                     */
                    switch (ctrlInterfaceType) {
                        case FrameQBufMgr_INTERFACE_SHAREDMEM:
                            status = FrameQBufMgr_ShMem_open(
                                    (FrameQBufMgr_ShMem_Handle *)&impHandle,
                                    (FrameQBufMgr_ShMem_OpenParams *)openParams);
                            if ((status < 0) || (impHandle == NULL)) {
                                FrameQBufMgr_close(&frmQBufMgrHandle);
                            }
                            else {
                                /* Get the instance id */
                                instId = FrameQBufMgr_ShMem_getId(impHandle);
                            }
                            break;

                        default:
                            /* NameServer entry does not contain the
                             * implementation type.
                             */
                            status = FrameQBufMgr_E_INVALID_INTERFACE;
                    }
                }
            }
        }

        if (status >= 0) {
            /* obj is inside FrameQBufMgr handle */
            obj                = frmQBufMgrHandle->obj;
            obj->handle        = impHandle;
            obj->ctrlInterfaceType = ctrlInterfaceType;
            obj->objType       = FrameQBufMgr_DYNAMIC_OPEN;
            obj->instId        = instId;
            *handlePtr         = frmQBufMgrHandle;
        }
        else {
           *handlePtr         = NULL;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    return (status);
}


/*!
 * @brief Function to close the dynamically opened instance
 *
 * @param Instance handle.
 */
Int32 FrameQBufMgr_close(FrameQBufMgr_Handle *pHandle)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_close", pHandle);

    GT_assert(curTrace, (pHandle != NULL));
    GT_assert(curTrace, ((pHandle != NULL) && (*pHandle != NULL)));

    status = FrameQBufMgr_delete (pHandle);

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_close", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_alloc ========
 *  Allocate frame
 */
Int32 FrameQBufMgr_alloc(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Frame *framePtr)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_alloc", handle, framePtr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_alloc", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_alloc", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_alloc", status,
                "framePtr passed is NULL!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert(curTrace, (obj != NULL));

        status = IFrameQBufMgr_alloc(obj->handle, framePtr);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_alloc", status);

    return (status);
}


/*
 *  ======== FrameQBufMgr_allocv ========
 *  Function to allocate multiple frames from the FrameQBufMgr instance
 */
Int32 FrameQBufMgr_allocv(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Frame framePtr[], UInt32 freeQId[], UInt8 *numFrames)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_4trace(curTrace, GT_ENTER, "FrameQBufMgr_allocv", handle, framePtr,
            freeQId, numFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));
    GT_assert(curTrace, (freeQId != NULL));
    GT_assert(curTrace, (numFrames != NULL));
    GT_assert (curTrace,(FrameQBufMgr_MAX_POOLS > *numFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_allocv", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_allocv", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_allocv", status,
                "framePtr passed is NULL!");
    }
    else if (freeQId == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_allocv", status,
                "freeQId passed is NULL!");
    }
    else if (numFrames == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_allocv", status,
                "numFrames passed is NULL!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert(curTrace, (obj != NULL));

        status = IFrameQBufMgr_allocv(obj->handle, framePtr, freeQId,
                numFrames);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_allocv", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_free ========
 *  Function to free frame.
 */
Int32 FrameQBufMgr_free(FrameQBufMgr_Handle handle, FrameQBufMgr_Frame frame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_free", handle, frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_free", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_free", status,
                "handle passed is NULL!");
    }
    else if (frame == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_free", status,
                "frame passed is NULL!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert(curTrace, (obj != NULL));

        status = IFrameQBufMgr_free (obj->handle, frame);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_free", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_freev ========
 *  Free multiple frames
 */
Int32 FrameQBufMgr_freev(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Frame framePtr[], UInt32 numFrames)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_3trace(curTrace, GT_ENTER, "FrameQBufMgr_freev", handle, framePtr,
            numFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));
    GT_assert(curTrace, (numFrames != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_freev", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_freev", status,
                "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_freev", status,
                "framePtr passed is NULL!");
    }
    else if (numFrames == 0) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_freev", status,
                "numFrames passed is zero!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert (curTrace, (obj != NULL));

        status = IFrameQBufMgr_freev (obj->handle, framePtr, numFrames);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_freev", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_add ========
 *  Add free frames allocated outside of FrameQBufMgr
 */
Ptr FrameQBufMgr_add(FrameQBufMgr_Handle handle, UInt8 freeQId)
{
    Ptr                 ptr = NULL;
    FrameQBufMgr_Obj   *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_add", handle, freeQId);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (freeQId < FrameQBufMgr_MAX_POOLS));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_add",
                FrameQBufMgr_E_INVALIDSTATE,
                "Module was not initialized!");
    }
    else if (freeQId >= FrameQBufMgr_MAX_POOLS) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_add",
                FrameQBufMgr_E_INVALIDARG,
                "Invalid freeQId provided for buf!");
    }
    else {
#endif
        obj = handle->obj;

        ptr = IFrameQBufMgr_add (obj->handle, freeQId);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_add", ptr);

    return (ptr);
}

/*
 *  ======== FrameQBufMgr_remove ========
 *  Remove dynamically added frame from FrameQBufMgr
 */
Int32 FrameQBufMgr_remove(FrameQBufMgr_Handle handle, UInt8 freeQId,
        FrameQBufMgr_Frame framePtr)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj   *obj;

    GT_3trace(curTrace, GT_ENTER, "FrameQBufMgr_remove", handle, freeQId,
            framePtr);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != framePtr));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_remove", status,
                "Module was not initialized!");
    }
    else if (framePtr == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_remove", status,
                "Invalid value NULL provided for framePtr!");
    }
    else {
#endif
        obj = handle->obj;

        status = IFrameQBufMgr_remove(obj->handle, freeQId, framePtr);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_remove", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_registerNotifier ========
 *  Register for notification
 */
Int32 FrameQBufMgr_registerNotifier(FrameQBufMgr_Handle handle,
        FrameQBufMgr_NotifyParams *notifyParams)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_registerNotifier", handle,
            notifyParams);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (notifyParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "FrameQBufMgr_registerNotifier", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "FrameQBufMgr_registerNotifier", status,
                "handle passed is NULL!");
    }
    else if (notifyParams == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_registerNotifier",
                             status,
                             "notifyParams passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, (obj != NULL));
        status = IFrameQBufMgr_registerNotifier (obj->handle, notifyParams);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_registerNotifier", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_unregisterNotifier ========
 *  Unregister the notification call back function
 */
Int32 FrameQBufMgr_unregisterNotifier (FrameQBufMgr_Handle handle)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj  *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_unregisterNotifier", handle);

    GT_assert(curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "FrameQBufMgr_unregisterNotifier", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "FrameQBufMgr_unregisterNotifier", status,
                "handle passed is NULL!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert(curTrace, obj != NULL);

        status = IFrameQBufMgr_unregisterNotifier(obj->handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_unregisterNotifier", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_dup ========
 * Duplicate the given frame
 */
Int32 FrameQBufMgr_dup(FrameQBufMgr_Handle handle, FrameQBufMgr_Frame frame,
        FrameQBufMgr_Frame dupedFramePtr[], UInt32 numDupedFrames)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj    *obj;

    GT_4trace(curTrace, GT_ENTER, "FrameQBufMgr_dup", handle, frame,
            dupedFramePtr, numDupedFrames);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frame != NULL));
    GT_assert (curTrace, (dupedFramePtr != NULL));
    GT_assert (curTrace, (numDupedFrames != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_dup", status,
                "Module was not initialized!");
    }
    else if (frame == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_dup", status,
                "frame to be duplicated is NULL!");
    }
    else if (dupedFramePtr == NULL) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_dup", status,
                "Location to receive duplicated frames is NULL !");
    }
    else if (numDupedFrames == 0) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_dup", status,
                "numDupedFrames passed is 0!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert(curTrace, (obj != NULL));
        status = IFrameQBufMgr_dup(obj->handle, frame, dupedFramePtr,
                numDupedFrames);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_dup", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_dupv ========
 *  Duplicate given frames
 */
Int32 FrameQBufMgr_dupv(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Frame framePtr[], FrameQBufMgr_Frame **dupedFramePtr,
        UInt32 numDupedFrames, UInt32 numFrames)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj    *obj;

    GT_5trace(curTrace, GT_ENTER, "FrameQBufMgr_dupv", handle, framePtr,
            dupedFramePtr, numDupedFrames, numFrames);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (framePtr != NULL));
    GT_assert(curTrace, (dupedFramePtr != NULL));
    GT_assert(curTrace, (numDupedFrames != 0));
    GT_assert(curTrace, (numFrames != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NULL if Module was not initialized */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             status,
                             "Module was not initialized!");
    }
    else if (framePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG Array of frames to be duplicated
         * is NULL
         */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             status,
                             "Array of frames to be duplicated is NULL!");
    }
    else if (dupedFramePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG Location to receive
         *  duplicated frames is NULL
         */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             status,
                             "Location to receive duplicated frames is NULL !");
    }
    else if (numDupedFrames == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDARG numDupedFrames passed is zero */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             status,
                             "numDupedFrames passed is zero!");
    }
    else if (numFrames == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDARG numFrames passed is zero */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             status,
                             "numFrames passed is zero!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, (obj != NULL));
        status = IFrameQBufMgr_dupv (obj->handle,
                                     framePtr,
                                     dupedFramePtr,
                                     numDupedFrames,
                                     numFrames);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_dupv", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_writeBack ========
 *  Write back the contents of a frame
 */
Int32 FrameQBufMgr_writeBack(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Frame frame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj  *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_writeBack", handle, frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBack",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBack",
                             status,
                             "handle passed is NULL!");
    }
    else if (frame == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBack",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_writeBack (obj->handle, frame);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_writeBack", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_invalidate ========
 *  Invalidate the contents of a frame
 */
Int32 FrameQBufMgr_invalidate(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Frame frame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj  *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_invalidate", handle, frame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frame  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidate",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidate",
                             status,
                             "handle passed is NULL!");
    }
    else if (frame == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidate",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_invalidate (obj->handle, frame);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_invalidate", status);

    /*! @retval FrameQBufMgr_S_SUCCESS  if successfully invalidates frame */
    return (status);
}

/*
 *  ======== FrameQBufMgr_writeBackHeaderBuf ========
 *  Write back the contents of frame header
 */
Int32 FrameQBufMgr_writeBackHeaderBuf(FrameQBufMgr_Handle handle,
        Ptr headerBuf)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_writeBackHeaderBuf", handle,
            headerBuf);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (headerBuf  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackHeaderBuf",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackHeaderBuf",
                             status,
                             "handle passed is NULL!");
    }
    else if (headerBuf == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame header  passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackHeaderBuf",
                             status,
                             "frame header passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_writeBackHeaderBuf (obj->handle, headerBuf);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_writeBackHeaderBuf", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_invalidateHeaderBuf ========
 *  Invalidate the contents of frame header
 */
Int32 FrameQBufMgr_invalidateHeaderBuf(FrameQBufMgr_Handle handle,
        Ptr headerBuf)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj   *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_invalidateHeaderBuf", handle,
            headerBuf);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (headerBuf  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateHeaderBuf",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateHeaderBuf",
                             status,
                             "handle passed is NULL!");
    }
    else if (headerBuf == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame header  passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateHeaderBuf",
                             status,
                             "frame header passed is NULL!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_invalidateHeaderBuf (obj->handle, headerBuf);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_invalidateHeaderBuf", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_writeBackFrameBuf ========
 *  Write back the contents of frame buffer
 */
Int32 FrameQBufMgr_writeBackFrameBuf(FrameQBufMgr_Handle handle, Ptr frameBuf,
        UInt32 size, UInt8 bufIndexInFrame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj *obj;

    GT_4trace (curTrace, GT_ENTER, "FrameQBufMgr_writeBackFrameBuf", handle,
            frameBuf, size, bufIndexInFrame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frameBuf != NULL));
    GT_assert(curTrace, (8 > bufIndexInFrame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackFrameBuf",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackFrameBuf",
                             status,
                             "handle passed is NULL!");
    }
    else if (frameBuf == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame buffer  passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackFrameBuf",
                             status,
                             "frame buffer passed is NULL!");
    }
    else if (bufIndexInFrame >= 8) {
        /*! @retval FrameQBufMgr_E_INVALIDARG bufIndexInFrame is greater than 8.
         * frame supports max 8 frame buffers 0 - 7
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_writeBackFrameBuf",
                             status,
                             "frame buffer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_writeBackFrameBuf(obj->handle, frameBuf, size,
                bufIndexInFrame);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_writeBackFrameBuf", status);

    return (status);
}

/*!
 *  @brief  Invalidate the contents  of frame buffer.
 *
 *          Passed buffer is treated as frame  buffer.
 *          bufNumInFrame denotes  buffer number in the frame.This is  to
 *          identify the cache flags and cpu access flags for the given buffer.
 *
 *  @param  handle      Handle to the instance.
 *  @param  frameBuf    frame buffer to be invalidated.
 *  @param  size        Size of buffer.
 *  @param  bufIndexInFrame index of the frame buffer in the frame that this
 *          frame buffer belongs to.
 */
Int32 FrameQBufMgr_invalidateFrameBuf(FrameQBufMgr_Handle handle, Ptr frameBuf,
        UInt32 size, UInt8 bufIndexInFrame)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj   *obj;

    GT_4trace (curTrace, GT_ENTER, "FrameQBufMgr_invalidateFrameBuf", handle,
            frameBuf, size, bufIndexInFrame);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (frameBuf != NULL));
    GT_assert(curTrace, (8 > bufIndexInFrame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateFrameBuf",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateFrameBuf",
                             status,
                             "handle passed is NULL!");
    }
    else if (frameBuf == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame buffer  passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateFrameBuf",
                             status,
                             "frame buffer passed is NULL!");
    }
    else if (bufIndexInFrame >= 8) {
        /*! @retval FrameQBufMgr_E_INVALIDARG bufIndexInFrame is greater than 8.
         * frame supports max 8 frame buffers 0 - 7
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_invalidateFrameBuf",
                             status,
                             "frame buffer passed is NULL!");
    }
    else {
#endif
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_invalidateFrameBuf(obj->handle, frameBuf,
                size, bufIndexInFrame);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_invalidateFrameBuf", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_translateAddr ========
 *  Translate address between different types
 */
Int32
FrameQBufMgr_translateAddr (FrameQBufMgr_Handle      handle,
                            Ptr *                    dstAddr,
                            FrameQBufMgr_AddrType    dstAddrType,
                            Ptr                      srcAddr,
                            FrameQBufMgr_AddrType    srcAddrType,
                            FrameQBufMgr_BufType     bufType)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj    *obj;

    GT_5trace(curTrace, GT_ENTER, "FrameQBufMgr_translateAddr", dstAddr,
            dstAddrType, srcAddr, srcAddrType, bufType);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (dstAddr != NULL));
    GT_assert(curTrace, (srcAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_translateAddr",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_translateAddr",
                             status,
                             "handle passed is NULL!");
    }
    else if (srcAddr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG srcAddr  passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_translateAddr",
                             status,
                             "srcAddr passed is NULL!");
    }
    else if (dstAddr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG dstAddr  passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_translateAddr",
                             status,
                             "dstAddr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        status = IFrameQBufMgr_translateAddr(obj->handle, dstAddr,
                dstAddrType, srcAddr, srcAddrType, bufType);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_translateAddr", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_getId ========
 *  Function to get FrameQBufMgr Id
 */
UInt32 FrameQBufMgr_getId(FrameQBufMgr_Handle handle)
{
    UInt32 id = 0;
    FrameQBufMgr_Obj  *obj;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_getId", handle);

    GT_assert(curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getId",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval 0 handle passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getId",
                             FrameQBufMgr_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        id = obj->instId;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_getId", id);

    /*! @retval id Instance ID */
    return (id);
}

/*
 *  ======== FrameQBufMgr_getHandle ========
 *  Internal API to return the FrameQBufMgr handle
 */
Ptr FrameQBufMgr_getHandle(UInt32 Id)
{
    FrameQBufMgr_Handle             handle  = NULL;
    IArg                             key;
    FrameQBufMgr_Obj                *obj;
    List_Elem                       *elem;
    UInt16                          instId;
    UInt16                          instNo;
    UInt16                          creProcId;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_getHandle", Id);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0), FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
            == TRUE) {
        /*! @retval NULL Module was not initialized */
        GT_setFailureReason (curTrace, GT_4CLASS, "FrameQBufMgr_getHandle",
                FrameQBufMgr_E_INVALIDSTATE, "Module was not initialized!");
    }
    else {
#endif
        instId = Id & 0xFFFF;
        /* Find out the creator procid */
        creProcId = ((instId >> FRAMEQBUFMGR_CREATORPROCID_BITOFFSET) &
                FRAMEQBUFMGR_CREATORPROCID_MASK);
        GT_assert(curTrace, (creProcId < MultiProc_MAXPROCESSORS));
        /* Get the instance no of the instance */
        instNo = ((instId >> FRAMEQBUFMGR_INSTNO_BITOFFSET) &
                FRAMEQBUFMGR_INSTNO_MASK);
        GT_assert(curTrace, (instNo < FrameQBufMgr_MAXINSTANCES));

        if (instNo < FrameQBufMgr_MAXINSTANCES) {
            key = IGateProvider_enter(FrameQBufMgr_module->gate);
            List_traverse(elem, (List_Handle) &FrameQBufMgr_module->objList) {
                if (((FrameQBufMgr_Obj *)elem)->instId == instId) {
                    obj = ((FrameQBufMgr_Obj *)elem);
                    /* Check if we have created the GP or not */
                    handle  = obj->top;
                    break;
                }
            }
            IGateProvider_leave(FrameQBufMgr_module->gate, key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_getHandle", handle);

    return (handle);
}

/*
 *  ======== FrameQ_control ========
 *  Hook to perform implementation dependent operations
 *
 */
Int32 FrameQBufMgr_control(FrameQBufMgr_Handle  handle,
                      Int32                cmd,
                      Ptr                  arg)
{
    Int32   status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj                *obj ;

    GT_3trace(curTrace, GT_ENTER, "FrameQBufMgr_control", handle, cmd, arg);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_control",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_control",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);

        /* Call implementation specific API.
         * Here obj->handle is the upcasted handle.
         */
        status = IFrameQBufMgr_control(obj->handle,
                                       cmd,
                                       arg);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_control", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_getBaseHeaderSize ========
 *  API to get the base frame heade if number of frame buffers known.
 */
UInt32 FrameQBufMgr_getBaseHeaderSize(UInt8 numFrameBufs)
{
    UInt32 baseHdrSize;

    GT_assert(curTrace, (numFrameBufs <= 8));

    baseHdrSize = (  sizeof(FrameQBufMgr_FrameHeader)
                   + (sizeof(FrameQBufMgr_FrameBufInfo)* (numFrameBufs-1)));

    return (baseHdrSize);
}
/*
 *  ======== FrameQBufMgr_getInstNo ========
 *  API to get the unique instane no for the instances of FrameQBufMgr
 *  instances.
 */
Int32 FrameQBufMgr_getInstNo(UInt8 *instNo)
{
    Int32  status = FrameQBufMgr_S_SUCCESS;
    UInt32 i;
    IArg   key;

    GT_1trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_getInstNo",
               instNo);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE Module was not initialized */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getInstNo",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Get unique id for the instance */
        key = IGateProvider_enter (FrameQBufMgr_module->gate );
        for (i = 0; i < FrameQBufMgr_MAXINSTANCES; i++) {
             if (FrameQBufMgr_module->isInstNoValid[i] == FALSE) {
                FrameQBufMgr_module->isInstNoValid[i] = TRUE;
                /* found the empty slot. This is the unique
                 * instance no for this instance on this processor.
                 * By appending creator proc id we get the unique
                 * instance id for this instance in the entire system
                 */
                *instNo = i;
                break;
             }
        }
        IGateProvider_leave (FrameQBufMgr_module->gate, key);

        if (i == FrameQBufMgr_MAXINSTANCES) {
            status = FrameQBufMgr_E_MAXINSTANCES;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_getInstNo", status);
    return (status);
}

/*
 *  ======== FrameQBufMgr_freeInstNo ========
 *  API to free the instance no to the FrameQBufMgr module.
 */
Int32 FrameQBufMgr_freeInstNo(UInt8 instNo)
{
    Int32  status = FrameQBufMgr_S_SUCCESS;
    IArg   key;

    GT_1trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_freeInstNo",
               instNo);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE Module was not initialized */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_freeInstNo",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (instNo >= FrameQBufMgr_MAXINSTANCES) {
            status = FrameQBufMgr_E_FAIL;
        }
        else {
            /* Get unique id for the instance */
            key = IGateProvider_enter (FrameQBufMgr_module->gate );

            FrameQBufMgr_module->isInstNoValid[instNo] = FALSE;

            IGateProvider_leave (FrameQBufMgr_module->gate, key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_freeInstNo", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_getNumFreeFrames ========
 *  Get the number of free frames
 *
 */
Int32 FrameQBufMgr_getNumFreeFrames(FrameQBufMgr_Handle handle,
        UInt32 *numFreeFrames)
{
    Int32   status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj                *obj ;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_getNumFreeFrames", handle,
            numFreeFrames);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getNumFreeFrames",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getNumFreeFrames",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);

        status = IFrameQBufMgr_getNumFreeFrames (obj->handle, numFreeFrames);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_getNumFreeFrames", status);

    return (status);
}

/*
 *  ======== FrameQBufMgr_getvNumFreeFrames ========
 * Function to get the number of free frames available in the plugged in
 * FrameQbufMgr.
 */
Int32
FrameQBufMgr_getvNumFreeFrames(FrameQBufMgr_Handle handle,
        UInt32 numFreeFrames[], UInt8 freeFramePoolNo[],
        UInt8 numFreeFramePools)
{
    Int32   status = FrameQBufMgr_S_SUCCESS;

    GT_3trace(curTrace, GT_ENTER, "FrameQBufMgr_getvNumFreeFrames", handle,
            numFreeFrames, freeFramePoolNo);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS,
                "FrameQBufMgr_getvNumFreeFrames", status,
                "Module was not initialized!");
    }
    else if (handle == NULL) {
        status =  FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace, GT_4CLASS,
                "FrameQBufMgr_getvNumFreeFrames", status,
                "handle passed is NULL!");
    }
    else {
#endif

        status = IFrameQBufMgr_getvNumFreeFrames(handle->obj->handle,
                numFreeFrames, freeFramePoolNo, numFreeFramePools);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQBufMgr_getvNumFreeFrames", status);

    return (status);
}

/*=============================================================================
 * Internal Functions
 *=============================================================================
 */
GateMP_Handle FrameQBufMgr_getGate(FrameQBufMgr_Handle handle)
{
    FrameQBufMgr_Object *       object;
    FrameQBufMgr_Obj    *       obj;
    FrameQBufMgr_ShMem_Handle   shMemHandle;

    object = (FrameQBufMgr_Object *)handle;
    obj = (FrameQBufMgr_Obj *)(object->obj);
    shMemHandle = obj->handle;
    return(FrameQBufMgr_ShMem_getGate(shMemHandle));
}

Void FrameQBufMgr_Params_init(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Params * params)
{
    FrameQBufMgr_Obj *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQBufMgr_Params_init", handle, params);

    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(FrameQBufMgr_module->refCount),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
            FRAMEQBUFMGR_MAKE_MAGICSTAMP(1)) == TRUE) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_Params_init",
                FrameQBufMgr_E_INVALIDSTATE, "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgr_Params_init",
                FrameQBufMgr_E_FAIL, "params is NULL!");
    }
    else {
#endif
        if (handle == NULL) {
            Memory_copy(params, &(FrameQBufMgr_module->defaultInstParams),
                    sizeof(FrameQBufMgr_Params));
        }
        else {
            obj = handle->obj;
            Memory_copy((Ptr)params, (Ptr)&obj->params,
                    sizeof(FrameQBufMgr_Params));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_0trace(curTrace, GT_LEAVE, "FrameQBufMgr_Params_init");
}


/*
 *  ======== _FrameQBufMgr_create ========
 *  Internal function called when openFlag is FALSE
 */
static inline
Int32 _FrameQBufMgr_create(FrameQBufMgr_Handle handle,
        FrameQBufMgr_Params *params)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_ShMem_Handle     shMemHandle;
    FrameQBufMgr_ShMem_Params    *pShMemParams;
    FrameQBufMgr_Obj             *obj;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    FrameQBufMgr_NameServerEntry  entry;
    UInt32                        len;
#endif

    GT_2trace(curTrace, GT_ENTER, "_FrameQBufMgr_create", handle, params);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(FrameQBufMgr_module->refCount),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(0),
                                  FRAMEQBUFMGR_MAKE_MAGICSTAMP(1))
        == TRUE) {
         /*! @retval NULL  Module was not initialized!
          */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQBufMgr_create",
                             status,
                             "Module was not initialized!");
    }
    else if ( NULL == handle) {
         /*! @retval NULL  Params passed is NULL
          */
         status = FrameQBufMgr_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQBufMgr_create",
                             status,
                             "obj of FrameQBufMgr_Object * is NULL!");

    }
    else if ( NULL == params) {
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQBufMgr_create",
                status, "Params pointer is NULL!");
    }
    else {
        /* Check if name already exists in the name server */
        if (FrameQBufMgr_module->nameServer != NULL) {
            len = sizeof (FrameQBufMgr_NameServerEntry);
            status = NameServer_get(FrameQBufMgr_module->nameServer,
                    (String)params->commonCreateParams.name,
                    &entry, &len, NULL);
            if (status >= 0) {
                /* Already instance with the name exists */
                status = FrameQBufMgr_E_INST_EXISTS;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "_FrameQBufMgr_create", status,
                        "Instance by this name exists.");
            }
            else if ((status != NameServer_E_NOTFOUND) && (status < 0)) {
                /* Error happened in NameServer. Pass the error up. */
                status = FrameQBufMgr_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_FrameQBufMgr_create",
                                     status,
                                     "Error happened in NameServer.!");
            }
            else {
                /* Go ahead and create as there is no instance with that name
                 * exists.
                 */
                status = FrameQBufMgr_S_SUCCESS;
            }
        }
#endif
        if (status >= 0) {
            /* Based on the type of interface  mentioned  call create of
             * specific implementation.Currently it has support for FrameQ on
             * shared memory. Need to extend this switch case  to add support
             * for other implementations.
             */
            switch (params->commonCreateParams.ctrlInterfaceType) {
                case FrameQBufMgr_INTERFACE_SHAREDMEM:
                {
                    pShMemParams = (FrameQBufMgr_ShMem_Params *)params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    /* Check the size  of params */
                    if(   pShMemParams->commonCreateParams.size
                        < sizeof(FrameQBufMgr_ShMem_Params)) {
                        /*! @retval NULL  Params passed is NULL
                         */
                        status = FrameQBufMgr_E_INVALIDARG;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_FrameQBufMgr_create",
                                             status,
                                             "ommonCreateParams"
                                             ".size is not equal size of"
                                             "FrameQBufMgr_ShMem_Params!");
                    }
                    else if( !FrameQBufMgr_isSupportedBufInterface(
                            params->commonCreateParams.ctrlInterfaceType,
                            pShMemParams->bufInterfaceType)) {
                        status = FrameQBufMgr_E_INVALID_BUFINTERFACETYPE;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "_FrameQBufMgr_create", status,
                                "Unsupported buf interface type.");
                    }
                    else {
#endif
                        /* Create the shared memory based FrameQ instance.*/
                        shMemHandle = FrameQBufMgr_ShMem_create(pShMemParams);
                        if (shMemHandle == NULL) {
                            status = FrameQBufMgr_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_FrameQBufMgr_create",
                                                 status,
                                                 "Failed to create internal instance"
                                                 "of type FrameQBufMgr_ShMem!");
                        }
                        else {
                            /* Upcast the handle so that we can call Implementation
                             * specific API using IFrameQBufMgr.
                             */
                            obj = handle->obj;
                            obj->handle = shMemHandle;
                            obj->ctrlInterfaceType =
                                    params->commonCreateParams.ctrlInterfaceType;
                            obj->objType       = FrameQBufMgr_DYNAMIC_CREATE;
                            /*  Get the instId  to update the toplevel object */
                            obj->instId = FrameQBufMgr_ShMem_getId(shMemHandle);
                            obj->refCount      = 1;
                            obj->notifyId = (UInt32)-1;

                       }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif
                }
                break;

                default:
                    /*! @retval NULL  invalid interface type
                     */
                    status = FrameQBufMgr_E_INVALID_INTERFACE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_FrameQBufMgr_create",
                                         status,
                                         "invalid interface type!");
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "_FrameQBufMgr_create", status);

    return (status);
}


/*!
 * @brief API to to find out the specified interface type is supported as buffer
 *  interface or not.
 * @param type  interface type.
 */
inline
Int32 FrameQBufMgr_isSupportedBufInterface(UInt32 ctrlInterfaceType,UInt32 bufType)
{
    Int32  status = TRUE;

    switch (ctrlInterfaceType)
    {
        case FrameQBufMgr_INTERFACE_SHAREDMEM:

            switch (bufType)
            {
                /* HLOS side only FrameQBufMgr_BUFINTERFACE_SHAREDMEM is
                 * supported.
                 */
            case FrameQBufMgr_BUFINTERFACE_SHAREDMEM:
                /* Supported interface */
            break;
            default:
                /* unsupported buf interface type specified */
                status = FALSE;
            }

        break;
        default:
            /* unsupported interface type specifiec */
            status = FALSE;
    }

    return (status);
}

/*!
 * @brief API to to find out the specified interface type is supported or not.
 * @param type  interface type.
 */
inline Int32 FrameQBufMgr_isSupportedInterface(UInt32 type)
{
    Int32  status = TRUE;

    switch (type)
    {
        case FrameQBufMgr_INTERFACE_SHAREDMEM:
            /* Supported interface */
        break;
        default:
            /* Unsupported interface type specified */
            status = FALSE;
    }

    return (status);
}

/*
 *  ======== FrameQBufMgr_isCacheEnabledForHeaderBuf ========
 * Function to find out if cache is enabled for header buffers.
 */
Bool FrameQBufMgr_isCacheEnabledForHeaderBuf(FrameQBufMgr_Handle handle)
{
    Bool cacheEnabled = FALSE;

    GT_1trace(curTrace, GT_ENTER, "FrameQBufMgr_isCacheEnabledForHeaderBuf",
            handle);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != handle->obj));

    cacheEnabled =
            IFrameQBufMgr_isCacheEnabledForHeaderBuf(handle->obj->handle);

    return (cacheEnabled);
}

/*
 *  ======== FrameQBufMgr_isCacheEnabledForFrameBuf ========
 * Function to find out if cache is enabled for frame buffers.
 */
Bool FrameQBufMgr_isCacheEnabledForFrameBuf(FrameQBufMgr_Handle handle,
        UInt8 frameBufIndex)
{
    Bool cacheEnabled = FALSE;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_isCacheEnabledForFrameBuf",
            handle);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != handle->obj));
    GT_assert(curTrace, (FrameQBufMgr_MAX_FRAMEBUFS > frameBufIndex));

    cacheEnabled =
            IFrameQBufMgr_isCacheEnabledForFrameBuf(handle->obj->handle,
            frameBufIndex);

    return (cacheEnabled);
}

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQBufMgr_getCliNotifyMgrShAddr (FrameQBufMgr_Handle handle)
{
    FrameQBufMgr_Obj      *obj;
    Ptr                    shAddr;

    GT_1trace (curTrace,
               GT_ENTER,
              "_FrameQBufMgr_getCliNotifyMgrShAddr",
               handle);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != handle->obj));

    obj = (FrameQBufMgr_Obj*)handle->obj;

    /* Call implementation specific API.
     * Here obj->handle is the upcasted handle.
     */
    shAddr = (Ptr)IFrameQBufMgr_getCliNotifyMgrShAddr(obj->handle);

    GT_1trace (curTrace,
               GT_LEAVE,
               "_FrameQBufMgr_getCliNotifyMgrShAddr",
               shAddr );

    /*! @retval  valid handle  if API is successful*/
    return (shAddr);
}

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQBufMgr_getCliNotifyMgrGateShAddr (FrameQBufMgr_Handle handle)
{
    FrameQBufMgr_Obj      *obj;
    Ptr                    shAddr;

    GT_1trace (curTrace,
               GT_ENTER,
              "_FrameQBufMgr_getCliNotifyMgrGateShAddr",
               handle);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->obj)));

    obj = (FrameQBufMgr_Obj*)handle->obj;

        /* Call implementation specific API.
     * Here obj->handle is the upcasted handle.
     */
    shAddr = IFrameQBufMgr_getCliNotifyMgrGateShAddr(obj->handle);

    GT_1trace (curTrace,
               GT_LEAVE,
               "_FrameQBufMgr_getCliNotifyMgrGateShAddr",
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
 _FrameQBufMgr_setNotifyId (FrameQBufMgr_Handle handle, UInt32 notifyId)
{
    Int32                   status       = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj        *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "_FrameQBufMgr_setNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace,
               (NULL != handle));

    obj = (FrameQBufMgr_Obj*)handle->obj;

    status = IFrameQBufMgr_setNotifyId(obj->handle, notifyId);

    GT_1trace (curTrace, GT_LEAVE, "_FrameQBufMgr_setNotifyId", status);

    /*! @retval  FrameQBufMgr_S_SUCCESS  */
    return (status);
}

/*!
 *  @brief     Function to reset the notifyid received in userspace during call to
 *             ClientNotifyMgr_register client.
 *  @param     handle  Instance handle.
 *  @param     notifyId  Id to to be set in the object.
 *
 */
Int32
 _FrameQBufMgr_resetNotifyId (FrameQBufMgr_Handle handle, UInt32 notifyId)
{
    Int32                   status  = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Obj        *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "_FrameQBufMgr_resetNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace,
               (NULL != handle));

    obj = (FrameQBufMgr_Obj*)handle->obj;

    status = IFrameQBufMgr_resetNotifyId(obj->handle, notifyId);

    GT_1trace (curTrace, GT_LEAVE, "_FrameQBufMgr_resetNotifyId", status);

    /*! @retval  FrameQBufMgr_S_SUCCESS  */
    return (status);
}
