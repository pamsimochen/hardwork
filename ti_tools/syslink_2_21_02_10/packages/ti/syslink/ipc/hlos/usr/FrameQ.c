/*
 *  @file   FrameQ.c
 *
 *  @brief      Implements FrameQ  functions on user space.
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
#include <ti/syslink/Std.h>

/* Osal And Utils  headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>

#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/ListMP.h>
/* Module level headers */

#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/FrameQ_ShMem.h>
#include <ti/syslink/inc/_FrameQ_ShMem.h>
#include <ti/syslink/inc/usr/Linux/FrameQDrv.h>
#include <ti/syslink/inc/FrameQDrvDefs.h>

#include <ti/syslink/inc/ClientNotifyMgr.h>

#include <string.h>

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/* Structure defining object for the Gate Peterson */
struct FrameQ_Object {
    List_Elem               listElem;

    UInt32                  objType;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32                  interfaceType;
    /*!<  Type of the interface */

    Ptr                     knlObject;
    /*!< Pointer to the kernel-side FrameQ object. */

    ClientNotifyMgr_Handle  clientNotifyMgrHandle;
    /*!< Handle to the client notifyMgr handle */
    GateMP_Handle           clientNotifyMgrGate;
    /* Gate used for ClientNotifyMgr instance */

    FrameQBufMgr_Handle     frameQBufMgrHandle;
    /*!< Handle to the client FrameQBufMgr handle */

    UInt32                  notifyId;

    FrameQ_OpenMode         objMode;
    /*!< mode of the instacne */
    Bool                    isRegistered;
    /* !< Indicates whether this instance is registred a call back function with
     *  the clientNotifyMgr.
     */
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
};

typedef struct FrameQ_Object    FrameQ_Object;

/*!
 *  @brief  FrameQ Module state object on user space
 */
typedef struct FrameQ_ModuleObject_Tag {
    UInt32              setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
    FrameQ_Config       cfg;
    /* Current config */

    List_Object         objList;
    /* List holding created objects */
    IGateProvider_Handle listLock;
    /* Lock to protect the objList */
    NameServer_Handle   nameServer;
    /* Handle to the nameserver created for  FrameQBufMgr */

} FrameQ_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    FrameQ_module_obj
 *
 *  @brief  FrameQ state object variable  in process
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
FrameQ_ModuleObject FrameQ_module_obj =
{
    .setupRefCount = 0
};

/*!
 *  @var    FrameQ_module
 *
 *  @brief  Pointer to FrameQ_module_obj .
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
FrameQ_ModuleObject* FrameQ_module = &FrameQ_module_obj ;
/*=============================================================================
 * Forward declarations of internal Functions
 *=============================================================================
 */
static inline
Int32
_FrameQ_create (FrameQ_Handle  handle,
                FrameQ_Params* params);

/* =============================================================================
 * APIS
 * =============================================================================
 */

/*!
 *  @brief  Function to get the  Module config parameters.
 *
 *  @param  cfgParams Configuration values.
 */
Void
FrameQ_getConfig (FrameQ_Config *cfg)
{
    Int32               status = FrameQ_S_SUCCESS;
    FrameQDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_getConfig",cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        status = FrameQ_E_INVALIDARG ;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getConfig",
                             status,
                             "Argument of type (FrameQ_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Temporarily open the handle to get the configuration. */
        status = FrameQDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            /*! @retval FrameQ_E_INVALIDSTATE  Failed to open driver handle.
             */
            status = FrameQ_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.getConfig.config = cfg;
            status = FrameQDrv_ioctl (CMD_FRAMEQ_GETCONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQ_getConfig",
                                     status,
                                     "API (through IOCTL) failed on"
                                     "kernel-side!");

            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        FrameQDrv_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_ENTER, "FrameQ_getConfig", status);
}


/*!
 *  @brief      Function to setup the FrameQ module.
 *
 *  @sa         FrameQ_destroy
 */
Int32
FrameQ_setup (FrameQ_Config * config)
{
    Int                  status  = FrameQ_S_SUCCESS;
    Error_Block          eb;
    IArg                 key;
    FrameQDrv_CmdArgs    cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_setup", config);
    Error_init (&eb);

    /* TBD: Protect from multiple threads. */
    key = Gate_enterSystem();
    FrameQ_module->setupRefCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (FrameQ_module->setupRefCount > 1) {
        Gate_leaveSystem(key);
        /*! @retval FrameQ_S_ALREADYSETUP Success: FrameQ module has been
                                           already setup in this process */
        status = FrameQ_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "FrameQ module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   FrameQ_module->setupRefCount);
    }
    else {
        Gate_leaveSystem(key);
        /* Open the driver handle. */
        status = FrameQDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.setup.config = (FrameQ_Config *) config;
            status = FrameQDrv_ioctl (CMD_FRAMEQ_SETUP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQ_setup",
                                     status,
                                     "API (through IOCTL) failed on"
                                     "kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                FrameQ_module->nameServer =
                              NameServer_getHandle(FRAMEQ_NAMESERVERNAME);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (FrameQ_module->nameServer == NULL) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQ_setup",
                                         status,
                                         "NameServer_getHandle() Failed!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    Memory_copy ((Ptr) &FrameQ_module->cfg,
                                 (Ptr) config,
                                 sizeof (FrameQ_Config));
                    List_construct (&FrameQ_module->objList, NULL);
                    FrameQ_module->listLock = (IGateProvider_Handle)
                                 GateMutex_create ((GateMutex_Params*)NULL, &eb);
                    GT_assert(curTrace,(FrameQ_module->listLock != NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_setup", status);

    /*! @retval FrameQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the FrameQ module.
 *
 *  @sa         FrameQ_setup
 */
Int32
FrameQ_destroy (Void)
{
    Int                     status = FrameQ_S_SUCCESS;
    IArg                    key;
    FrameQDrv_CmdArgs       cmdArgs;
    Int32                   tmpStatus;

    GT_0trace (curTrace, GT_ENTER, "FrameQ_destroy");

    key = Gate_enterSystem();
    if (FrameQ_module->setupRefCount == 0) {
        Gate_leaveSystem(key);
        /*! @retval FrameQ_S_ALREADYDESTROYED Success: FrameQ module has been
                                             already destroyed in this process */
        status = FrameQ_S_ALREADYDESTROYED;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "FrameQ module has been already destroyed in this process.\n"
                   "    RefCount: [%d]\n",
                   FrameQ_module->setupRefCount);
    }
    else  {
        FrameQ_module->setupRefCount--;
        /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
        if (FrameQ_module->setupRefCount > 1) {
            Gate_leaveSystem(key);
            /*! @retval FrameQ_S_ALREADYSETUP Success: FrameQ module's ref count
             * has been decreased in this process.
             */
            status = FrameQ_S_ALREADYSETUP;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "FrameQ module has been already destroyed in this process.\n"
                       "    RefCount: [%d]\n",
                       FrameQ_module->setupRefCount);
        }
        else {
            Gate_leaveSystem(key);
            status = FrameQDrv_ioctl (CMD_FRAMEQ_DESTROY, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQ_destroy",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Destruct the list object */
            List_destruct (&FrameQ_module->objList);
            /* Delete the list lock */
            tmpStatus = GateMutex_delete ((GateMutex_Handle*)&FrameQ_module->listLock);
            if (tmpStatus < 0) {
                status  = FrameQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQ_destroy",
                                     status,
                                     "GateMutex_delete failed !");
            }
            /* Close the driver handle. */
            FrameQDrv_close ();
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_destroy", status);

    return (status);
}

/*!
 *  @brief      Creates a new instance of FrameQ module.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         FrameQ_delete, FrameQ_open, FrameQ_close
 */
FrameQ_Handle FrameQ_create(Ptr vparams)
{
    Int32 status = FrameQ_S_SUCCESS;
    FrameQ_Handle handle = NULL;
    FrameQ_Params *params = (FrameQ_Params*)vparams;
    IArg                    key0;

    GT_1trace(curTrace, GT_ENTER, "FrameQ_create", vparams);

    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
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
            status = _FrameQ_create (handle, params);
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_create",
                        status, "Failed to create instance !");
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif

        if (status >= 0) {
            /* Put in the local list */
            key0 = IGateProvider_enter (FrameQ_module->listLock);
            List_elemClear (&(handle->listElem));
            List_put ((List_Handle) &(FrameQ_module->objList),
                       &(handle->listElem));
            IGateProvider_leave (FrameQ_module->listLock, key0);
        }
        else {
            if (handle != NULL) {
                Memory_free (NULL, handle, sizeof (FrameQ_Object));
                handle = NULL;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)

    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_create", handle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */
    return (handle);
}


/*!
 *  @brief      Deletes a instance of FrameQ module.
 *
 *  @param      handlePtr  Pointer to handle to previously created instance. The
 *                         user pointer is reset upon success.
 *
 *  @sa         FrameQ_create, FrameQ_open, FrameQ_close
 */
Int32
FrameQ_delete (FrameQ_Handle * handlePtr)
{
    Int32                   status = FrameQ_S_SUCCESS;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj ;
    IArg                    key0;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (*handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        /*! @retval FrameQ_E_INVALIDSTATE Modules is invalidstate*/
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_delete",
                             status,
                             "Modules is invalidstate!");
    }
    else if (handlePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handlePtr passed is NULL */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_delete",
                             status,
                             "handlePtr passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG *handlePtr passed is NULL */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_delete",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else if (((FrameQ_Object *)(*handlePtr))->objType  !=
             FrameQ_DYNAMIC_CREATE
            && (((FrameQ_Object *)(*handlePtr))->objType  !=
             FrameQ_DYNAMIC_CREATE_USEDREGION)) {
        /*! @retval FrameQ_E_INVALIDARG *handlePtr passed is not the created
         * handle
         */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_delete",
                             status,
                             "*handlePtr passed is not the created handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object *)(*handlePtr) ;

        key0 = IGateProvider_enter (FrameQ_module->listLock);
        /* Remove it from the local list */
        List_remove ((List_Handle) &FrameQ_module->objList,
                     &(*handlePtr)->listElem);
        IGateProvider_leave(FrameQ_module->listLock, key0);

        status = ClientNotifyMgr_close (
                                       &((*handlePtr)->clientNotifyMgrHandle));
        if (status < 0) {
            status = FrameQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_delete",
                                 status,
                                 "ClientNotifyMgr_delete failed!");
        }
        else {
            GateMP_close (&((*handlePtr)->clientNotifyMgrGate));
        }
        status = FrameQBufMgr_close (&((*handlePtr)->frameQBufMgrHandle));
        if (status < 0) {
            status = FrameQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_delete",
                                 status,
                                 "FrameQBufMgr_close failed!");
        }

        cmdArgs.args.deleteInstance.handle = (*handlePtr)->knlObject;
        status = FrameQDrv_ioctl (CMD_FRAMEQ_DELETE, &cmdArgs);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_delete",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
            Memory_free (NULL,
                        (*handlePtr),
                        sizeof (FrameQ_Object));
            *handlePtr = NULL;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_delete", status);

    /*! @retval FrameQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief  Function to open the created FrameQ instance.
 *          The instance must be  created before openinig it.
 *
 *  @param  handlePtr
 *          Location to receive the created FrameQ instance.
 *  @param  intOpenParams
 *          open parameters specific to implementation. FrameQ_ShMem_OpenParams
 *          in case the instance is of type ShMem.
 */
Int32
FrameQ_open(Ptr             instOpenParams,
            FrameQ_Handle   *handlePtr)
{
    String                      fQBufMgrName = NULL;
    Ptr                         fQBufMgrShAddr = NULL;
    Int32                       status          = FrameQ_S_SUCCESS;
    ClientNotifyMgr_Handle      cliMgrHandle    = NULL;
    GateMP_Handle               gateHandle      = NULL;
    FrameQ_Handle               handle          = NULL;
    FrameQBufMgr_Handle         lframeQBufMgrHandle = NULL;
    FrameQ_OpenParams           *openParams = (FrameQ_OpenParams*)
                                                                 instOpenParams;
    IArg                        key         = 0;
    Ptr                         sharedAddr = NULL;
    FrameQ_ShMem_OpenParams     *shMemOpenParams;
    Int32                       tmpStatus;
    FrameQ_NameServerEntry      entry;
    FrameQ_NameServerEntry      *pentry;
    UInt32                      interfaceType;
    UInt8                       nameLen;
    ClientNotifyMgr_Params      clientMgrParams;
    UInt16                      index;
    UInt32                      len;
    FrameQDrv_CmdArgs           cmdArgs;
    FrameQBufMgr_ShMem_OpenParams frmQBufMgrOpenParams;
    FrameQ_Object               *obj;
    IArg                        key0;
    Bool                        ctrlStructCacheFlag = FALSE;


    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQ_open",
               handlePtr,
               instOpenParams);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (instOpenParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        /*! @retval FrameQ_E_INVALIDSTATE Modules is invalidstate*/
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_open",
                             status,
                             "Modules is invalidstate!");
    }
    else if (handlePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handlePtr passed is NULL */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_open",
                             status,
                             "handlePtr passed is NULL!");
    }
    else if (   (String_len((Ptr)openParams->commonOpenParams.name) == 0)
              &&(openParams->commonOpenParams.sharedAddr == NULL)) {
        /*! @retval FrameQ_E_INVALIDARG
         * Either name or sharedaddr must be non null.
         */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_open",
                             status,
                             "Either name or sharedaddr must be non null..");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
       /*String_len gives name length excluding the terminating character */
        nameLen = String_len((Ptr)openParams->commonOpenParams.name);
		if (openParams->commonOpenParams.name != NULL) {
			/* If name is provided use name based  open only */
			openParams->commonOpenParams.sharedAddr = NULL;
		}
        if (   ( (nameLen > 0) && ( nameLen <= (FrameQ_MAX_NAMELEN-1))
            && (NULL != FrameQ_module->nameServer))) {
            len = sizeof (FrameQ_NameServerEntry);
            status = NameServer_get (FrameQ_module->nameServer,
                                     (String)openParams->commonOpenParams.name,
                                     &entry,
                                     &len ,
                                     NULL);
            if (status < 0) {
                status = FrameQ_E_NOTFOUND;
                /* Do not set failure reason here, since it is an expected run-
                 * time failure in application.
                 */
            }
            else {
                interfaceType = entry.interfaceType;
                pentry = &entry;
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
                Cache_inv (pentry,
                           sizeof (FrameQ_NameServerEntry),
                           Cache_Type_ALL,
                           TRUE);
                Gate_leaveSystem(key);
            }
            interfaceType = pentry->interfaceType;
        }
        else {
            status = FrameQ_E_INVALIDARG;
            /*! @retval FrameQ_E_INVALIDARG
             * Either name or sharedaddr must be non null.
             */
            status = FrameQ_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_open",
                                 status,
                                 "Either name or sharedaddr must be non null.");
        }
        if (status >= 0) {
             /* Check the interface type and if it is not supported
              * return failure.
              */
            if( !FrameQ_isSupportedInterface(interfaceType)) {
                /*! @retval FrameQ_E_INVALID_INTERFACE
                 * Unsupported interface type.
                 */
                status = FrameQ_E_INVALID_INTERFACE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQ_open",
                                     status,
                                     "Unsupported interface type.");
            }
            else {
                /* Allocate memory for the handle */
                handle = (FrameQ_Handle) Memory_calloc (NULL,
                                                        sizeof (FrameQ_Object),
                                                        0,
                                                        NULL);
        #if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    /*! @retval NULL Memory allocation failed for handle */
                    status = FrameQ_E_ALLOC_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQ_open",
                                         status,
                                         "Memory allocation failed for handle!");
                }
                else {
        #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj                 = handle;
                    /* Open handle to the implementation specific handle
                     * and plug in to the top level FrameQ object.
                     */
                    switch (interfaceType) {
                        case FrameQ_INTERFACE_SHAREDMEM:
                        {
                            shMemOpenParams = (FrameQ_ShMem_OpenParams *)
                                                                 instOpenParams;

                            cmdArgs.args.open.interfaceType =
                                                    FrameQ_INTERFACE_SHAREDMEM;
                            cmdArgs.args.open.openParams = (Ptr)
                                Memory_calloc(
                                                NULL,
                                                sizeof(FrameQ_ShMem_OpenParams),
                                                0,
                                                NULL);
                            Memory_copy(cmdArgs.args.open.openParams,
                                        shMemOpenParams,
                                        sizeof(FrameQ_ShMem_OpenParams));
                            if (shMemOpenParams->commonOpenParams.name != NULL) {
                                cmdArgs.args.open.nameLen =
                                String_len (shMemOpenParams->commonOpenParams.name) + 1;

                                /* Give priority to name over shared addr. */
                                ((FrameQ_ShMem_OpenParams *)
                                        cmdArgs.args.open.openParams)->
                                            commonOpenParams.sharedAddr = (Ptr)
                                               SharedRegion_INVALIDSRPTR;
                            }
                            else {
                                cmdArgs.args.open.nameLen = 0;
                                if  (  shMemOpenParams->commonOpenParams.sharedAddr
                                     != NULL) {
                                    /* Convert user space shared addr to shared region address */
                                    index = SharedRegion_getId(
                                      shMemOpenParams->commonOpenParams.sharedAddr);

                                    ((FrameQ_ShMem_OpenParams *) cmdArgs.args.open.openParams)->commonOpenParams.sharedAddr =
                                             (Ptr)SharedRegion_getSRPtr(
                                            shMemOpenParams->commonOpenParams.sharedAddr,
                                            index);
                                     GT_assert (curTrace,
                                                (SharedRegion_SRPtr)
                                                (((FrameQ_ShMem_OpenParams *)
                                                cmdArgs.args.open.openParams)->
                                                commonOpenParams.sharedAddr) !=
                                                SharedRegion_INVALIDSRPTR);
                                }
                            }

                            status = FrameQDrv_ioctl (CMD_FRAMEQ_OPEN,
                                                      &cmdArgs);
                            if (status < 0) {
                                GT_setFailureReason (curTrace,
                                                     GT_4CLASS,
                                                     "FrameQ_open",
                                                     status,
                                                     "API (through IOCTL) failed on kernel-side!");
                            }
                            else {
                                fQBufMgrName = (String)
                                    pentry->instParams.shMemParams.frameQBufMgrName;
                                fQBufMgrShAddr = (Ptr)SharedRegion_getPtr(
                                    (SharedRegion_SRPtr)
                                    pentry->instParams.shMemParams.frameQBufMgrSharedAddr);

                            }
                        }
                        break;

                        default:
                            /* NameServer entry does not contain the
                             * implementation type.
                             */
                            status = FrameQ_E_INVALID_INTERFACE;
                    }


                    if (status >= 0) {
                        frmQBufMgrOpenParams.commonOpenParams.name =
                                                                   fQBufMgrName;
                        frmQBufMgrOpenParams.commonOpenParams.sharedAddr =
                                                                  fQBufMgrShAddr;

                        frmQBufMgrOpenParams.commonOpenParams.cpuAccessFlags  =
                                                shMemOpenParams->commonOpenParams.cpuAccessFlags;
                        tmpStatus = FrameQBufMgr_open (
                                                        &lframeQBufMgrHandle,
                                                        &frmQBufMgrOpenParams);
                        if  (tmpStatus < 0) {
                            status = FrameQ_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "FrameQ_open",
                                                 status,
                                                 "FrameQBufMgr_open failed!");
                        }

                    }
                    if (status >= 0) {
                        /* Open client notifymgr for this instance in user space
                         */
                        ClientNotifyMgr_Params_init(&clientMgrParams);
                        clientMgrParams.sharedAddr = SharedRegion_getPtr(
                                     cmdArgs.args.open.cliNotifyMgrSharedMem);
                        sharedAddr  = SharedRegion_getPtr (
                                           cmdArgs.args.open.cliGateSharedMem);
                        tmpStatus = GateMP_openByAddr(sharedAddr,
                                                &gateHandle);

                        if ((tmpStatus < 0) || (gateHandle == NULL)) {
                            /*! @retval FrameQ_E_OPEN_GATEPETERSON Failed to open
                             * the gate peterson
                             */
//                            status = FrameQ_E_OPEN_GATEPETERSON;
//                            GT_setFailureReason (
//                                          curTrace,
//                                          GT_4CLASS,
//                                          "FrameQ_ShMem_open",
//                                          FrameQ_E_OPEN_GATEPETERSON,
//                                          "Failed to open the gate peterson !");
                        }
                        else {
                            /* Open  the already created clientNotifyMgr
                             * instance in user space.
                             */
                            clientMgrParams.name = (Ptr)
                                              openParams->commonOpenParams.name;
                            clientMgrParams.openFlag = TRUE;
                            clientMgrParams.gate = (Ptr)gateHandle;
                            ctrlStructCacheFlag = 
						        SharedRegion_isCacheEnabled(SharedRegion_getId(
								clientMgrParams.sharedAddr));
                            if (ctrlStructCacheFlag == TRUE) {
                                clientMgrParams.cacheFlags =
                                               ClientNotifyMgr_CONTROL_CACHEUSE;
                            }
                            cliMgrHandle =
                                ClientNotifyMgr_create (&clientMgrParams);
                            if (cliMgrHandle == NULL) {
                                /*! @retval FrameQ_E_FAIL_CLIEN0TIFYMGR_OPEN
                                 * Failed to open the clientNotifyMgr instance.
                                 */
                                status = FrameQ_E_FAIL_CLIENTN0TIFYMGR_OPEN;
                                GT_setFailureReason (
                                               curTrace,
                                               GT_4CLASS,
                                              "FrameQ_open",
                                               status,
                                              "Failed to open the clientNotifyMgr"
                                              "instance. !");
                            }
                        }

                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif
                if (status >= 0) {
                    obj->knlObject      = cmdArgs.args.open.handle;
                    obj->interfaceType  = interfaceType;
                    obj->objType        = FrameQ_DYNAMIC_OPEN;
                    obj->objMode        = openParams->commonOpenParams.openMode;
                    obj->clientNotifyMgrGate = (Ptr)gateHandle;
                    obj->clientNotifyMgrHandle = (Ptr)cliMgrHandle;
                    obj->frameQBufMgrHandle =lframeQBufMgrHandle;
                    obj->ctrlStructCacheFlag = ctrlStructCacheFlag;
                    *handlePtr         = handle;
                    /* Put in the local list */
                    key0 = IGateProvider_enter (FrameQ_module->listLock);
                    List_elemClear (&handle->listElem);
                    List_put ((List_Handle) &(FrameQ_module->objList),
                               &(handle->listElem));
                    IGateProvider_leave (FrameQ_module->listLock, key0);
                }
                else {
                    *handlePtr         = NULL;
                    if (handle != NULL) {
                        Memory_free (NULL,
                                     handle,
                                     sizeof (FrameQ_Object));
                        handle = NULL;
                    }
                }
                if (cmdArgs.args.open.openParams != NULL) {
                    Memory_free(
                                NULL,
                                cmdArgs.args.open.openParams,
                                sizeof(FrameQ_ShMem_OpenParams));
                }

            } /*if( !FrameQ_isSupportedInterface( */
        }   /* if (status >= 0)*/
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_open", status);

    /*! @retval FrameQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief  API to close the previousely opened FrameQ client.
 *
 * @param  Handle to previously opened instance.
 * @sa     FrameQ_create, FrameQ_delete, FrameQ_open
 */
Int32
FrameQ_close (FrameQ_Handle * handlePtr)
{
    Int32                status = FrameQ_S_SUCCESS;
    FrameQDrv_CmdArgs    cmdArgs;
    IArg                 key0;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (*handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_close",
                             FrameQ_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handlePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handlePtr passed is null */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_close",
                             status,
                             "handlePtr passed is null!");
    }
    else if (*handlePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG *handlePtr passed is null */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_close",
                             status,
                             "*handlePtr passed is null!");
    }
    else if (((FrameQ_Object *)(*handlePtr))->objType  !=
             FrameQ_DYNAMIC_OPEN) {
        /*! @retval FrameQ_E_INVALIDARG *handlePtr passed is not the
         * reader/writer handle.
         */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_delete",
                             status,
                             "*handlePtr passed is not the reader/writer handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key0 = IGateProvider_enter (FrameQ_module->listLock);
        /* Remove it from the local list */
        List_remove ((List_Handle) &FrameQ_module->objList,
                     &(*handlePtr)->listElem);
        IGateProvider_leave(FrameQ_module->listLock, key0);
        status = ClientNotifyMgr_close (&((* handlePtr)->clientNotifyMgrHandle));
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_close",
                                 status,
                                 "ClientNotifyMgr_close failed!");
        }
        else {
            GateMP_close (&((*handlePtr)->clientNotifyMgrGate));
        }

        status = FrameQBufMgr_close (&((*handlePtr)->frameQBufMgrHandle));
        if (status < 0) {
            status = FrameQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_close",
                                 status,
                                 "FrameQBufMgr_close failed!");
        }

        cmdArgs.args.close.handle = (*handlePtr)->knlObject;
        status = FrameQDrv_ioctl (CMD_FRAMEQ_CLOSE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_close",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Memory_free (NULL,
                        (*handlePtr),
                        sizeof (FrameQ_Object));
            *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
       }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_close", status);

    /*! @retval FrameQ_S_SUCCESS Operation successful */
    return (status);
}

/*
 * @brief Function to allocate a frame.
 *
 * @param  handle   Instance handle.
 * @param  framePtr Location to receive the allocated frame
 *
 */
Int32
FrameQ_alloc (FrameQ_Handle     handle,
              FrameQ_Frame      *framePtr)
{
    Int32               status        = FrameQ_S_SUCCESS;
    FrameQ_Object          *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_alloc", handle, framePtr);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (framePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_alloc",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handle passed is NULL*/
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_alloc",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handle passed is NULL*/
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_alloc",
                             status,
                             "framePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
        GT_assert (curTrace, (obj != NULL));

        if (obj->objMode != FrameQ_MODE_WRITER) {
            /* If Caller is other than writer, return error. */
            *framePtr = NULL;
            /*! @retval FrameQ_E_INVALIDARG The handle is not writer
             *  handle
             */
            status = FrameQ_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "FrameQ_alloc",
                            status,
                            "The provided handle does not have permission to"
                            " do alloc. The handle is not writer"
                            "handle!");

        }
        else {
            /* Get the frame from free queue 0 of FrameQ BufMgr. */
            GT_assert (curTrace, (obj->frameQBufMgrHandle != NULL));
            status = FrameQBufMgr_alloc (obj->frameQBufMgrHandle,
                                         framePtr);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_alloc", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully allocated  frame */
    return (status) ;
}

/*
 * @brief   Function to allocate frames from multiple free pools of a plugged in
 *          FrameQBufMgr.
 *
 *          After API returns,numframes will denotes  the number of successfully
 *          allocated frames.
 *
 * @param  handle    Instance handle.
 * @param  framePtr  Location to receive the allocated frames
 * @param  freeQId   Free frame pool nos of plugged in FrameQBufMgr from which
 *                   frames needs to be allocated.
 * @param  numFrames Number of frames that needs to be allocated.It should be
 *                   equal to the  size of the framePtr and freeQId array.
 */
Int32
FrameQ_allocv (FrameQ_Handle  handle,
               FrameQ_Frame   framePtr[],
               UInt32          freeQId[],
               UInt8          *numFrames)
{
    Int32             status        = FrameQ_S_SUCCESS;
    UInt32            i;
    FrameQ_Object        *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQ_allocv",
               handle,
               framePtr,
               freeQId,
               *numFrames);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (framePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_allocv",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handle passed is NULL*/
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_allocv",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handle passed is NULL*/
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_allocv",
                             status,
                             "framePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
        GT_assert (curTrace, (obj != NULL));

        if (obj->objMode != FrameQ_MODE_WRITER) {
            /* If Caller is other than writer, return error. */
            for (i = 0; i < *numFrames; i++) {
                framePtr[i] = NULL;
            }
            *numFrames = 0;
            /*! @retval FrameQ_E_INVALIDARG The handle is not writer
             *  handle
             */
            status = FrameQ_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "FrameQ_allocv",
                            status,
                            "The provided handle does not have permission to"
                            " do alloc. The handle is not writer"
                            "handle!");

        }
        else {
            GT_assert (curTrace, (obj->frameQBufMgrHandle != NULL));
            /* Allocate  frames from the plugged in FrameQBufMgr instance.
             * Base frame headers need not be populated  here as FrameQBufMgr will
             * return the frames  having updated headers.
             *
             *  If Alloc fails FrameQbufMgr takes care of enablling notifications
             *  for the writer clients.
             */
            status = FrameQBufMgr_allocv (obj->frameQBufMgrHandle,
                                          framePtr,
                                          freeQId,
                                          numFrames);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_allocv", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully allocated  frame */
    return (status) ;
}

/*
 *  @brief Function to free a frame .
 *         Allow only writers/readers to call  this API.
 * @param framePtr    Frame to be duplicated.
 *
 */
Int32
FrameQ_free (FrameQ_Handle handle, FrameQ_Frame  frame)
{
    Int32             status           = FrameQ_S_SUCCESS ;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32             tmpStatus        = FrameQ_S_SUCCESS ;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    FrameQ_Object *   obj;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_free", frame);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frame != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_free",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handle passed is NULL*/
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_free",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
        GT_assert (curTrace, (obj != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (   (obj->objMode != FrameQ_MODE_WRITER)
            && (obj->objMode != FrameQ_MODE_READER)) {
            /*  If Caller is other than writer/writer return error. */
            /*! @retval FrameQ_E_INVALIDARG The handle is not writer
             *  handle
             */
            status = FrameQ_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "FrameQ_allocv",
                            status,
                            "The provided handle does not have permission to"
                            " do alloc. The handle is not writer"
                            "handle!");

        }
        else {
            /* Free frame to free queue 0 of FrameQ BufMgr. */
            GT_assert (curTrace, (obj->frameQBufMgrHandle != NULL));
            tmpStatus =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)*/
                FrameQBufMgr_free(obj->frameQBufMgrHandle,
                                  frame);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if  (tmpStatus < 0) {
                status = tmpStatus;
            }
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_free", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully freed frame */
    return (status) ;
}

/*
 *  @brief Function to free multiple frames .
 *         Allow only writers/readers to call  this API.
 * @param framePtr    Frame to be duplicated.
 *
 */
Int32
FrameQ_freev (FrameQ_Handle  handle,
              FrameQ_Frame   framePtr[],
              UInt32         numFrames)
{
    Int32                   status           = FrameQ_S_SUCCESS ;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   tmpStatus        = FrameQ_S_SUCCESS ;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    FrameQBufMgr_Handle     bufMgrHandle;
    FrameQ_Object *         obj;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_freev", framePtr);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (framePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_freev",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQ_E_INVALIDARG handle passed is NULL*/
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_freev",
                             status,
                             "handle passed is NULL!");
    }
    else if ((handle->objMode != FrameQ_MODE_WRITER)
             && (handle->objMode != FrameQ_MODE_READER)) {
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_freev", status,
                "The provided handle does not have permission to"
                " do alloc. The handle is not writer handle!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_freev", status,
                "framePtr passed is NULL!");
    }
    else {
#endif
        obj = handle;
        GT_assert (curTrace, (obj != NULL));

        bufMgrHandle = FrameQBufMgr_getHandle (framePtr[0]->frmAllocaterId);
        GT_assert(curTrace, (NULL != bufMgrHandle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (bufMgrHandle == NULL) {
            status = FrameQ_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "FrameQ_freev", status,
                    "FrameQBufMgr handle is null!");

        }
        else {
            tmpStatus =
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE)*/
            FrameQBufMgr_freev (bufMgrHandle,
                                framePtr,
                                numFrames);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if  (tmpStatus < 0) {
                status = tmpStatus;
            }
        }
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
    Int32                   status       = FrameQ_S_SUCCESS;
    UInt32                  i            = 0;
    FrameQ_FrameBufInfo     *frameBufInfo;
    Ptr                     addr;
    Ptr                     phyPtr;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_2trace(curTrace, GT_ENTER, "FrameQ_put", handle, frame);

    GT_assert(curTrace, (NULL != handle));
    GT_assert(curTrace, (NULL != frame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
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
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_READER)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_WRITER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not reader or writer handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "FrameQ_put",
                        status,
                        "The provided handle does not have permission to"
                        " do put. The handle is not reader or writer handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     = (FrameQ_Object*) handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.put.handle = handle->knlObject;
        /* Convert frame buffer pointer in frame and frame pointer to physical
         * addr format.
         */
        /* Convert all frame buffer address to physical */
        frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
        for(i = 0; i < frame->numFrameBuffers; i++) {
            addr = (Ptr)frameBufInfo[i].bufPtr;
            phyPtr = Memory_translate(addr, Memory_XltFlags_Virt2Phys);
            GT_assert(curTrace,(phyPtr != NULL));
            frameBufInfo[i].bufPtr = (UInt32)phyPtr;
        }
        cmdArgs.args.put.frame   = Memory_translate(
                                                    frame,
                                                    Memory_XltFlags_Virt2Phys);
        status = FrameQDrv_ioctl (CMD_FRAMEQ_PUT, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_put",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "FrameQ_put", status);

    return (status);
}


/*
 *  @brief Function to insert frame in to FrameQ .
 *         Allow only writers/readers to call  this API.
 *  @param framePtr    Frame to be duplicated.
 *
 */
Int32
FrameQ_putv  (FrameQ_Handle handle,
              FrameQ_Frame  framePtr[],
              UInt32         filledQueueId[],
              UInt8         numFrames)
{
    Int32                   status       = FrameQ_S_SUCCESS;
    FrameQ_FrameBufInfo    *frameBufInfo;
    UInt32                  i;
    UInt32                  j;
    Ptr                     addr;
    Ptr                     phyPtr;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQ_putv",
               handle,
               framePtr,
               filledQueueId,
               numFrames);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != framePtr));
    GT_assert (curTrace, (NULL != filledQueueId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_putv",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_putv",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_putv",
                             status,
                             "framePtr passed is NULL!");
    }
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_READER)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_WRITER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not reader or writer handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "FrameQ_putv",
                        status,
                        "The provided handle does not have permission to"
                        " do put. The handle is not reader or writer handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     = (FrameQ_Object*) handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.putv.handle    = handle->knlObject;
        cmdArgs.args.putv.aframePtr =   Memory_alloc(NULL,
                                                     numFrames * sizeof(UInt32),
                                                     0,
                                                     NULL);
        cmdArgs.args.putv.filledQueueId = (UInt32*)filledQueueId;
        cmdArgs.args.putv.numFrames     = numFrames;

        for (i = 0; i < numFrames;i++) {
            /* Convert all frame buffer address to physical */
            frameBufInfo = (FrameQ_FrameBufInfo *)
                                                &(framePtr[i]->frameBufInfo[0]);
            for(j = 0; j < framePtr[i]->numFrameBuffers; j++) {
                addr = (Ptr)frameBufInfo[j].bufPtr;
                phyPtr = Memory_translate(addr, Memory_XltFlags_Virt2Phys);
                GT_assert(curTrace,(phyPtr != NULL));
                frameBufInfo[j].bufPtr = (UInt32)phyPtr;
            }
            cmdArgs.args.putv.aframePtr[i]   = Memory_translate(
                                                     framePtr[i],
                                                     Memory_XltFlags_Virt2Phys);
        }
        status = FrameQDrv_ioctl (CMD_FRAMEQ_PUTV, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_putv",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_putv", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully inserted frame in to FrameQ */
    return (status);
}


/*
 *  @brief Function to get frame from FrameQ .
 *         Allowed only readers to call  this API.
 *  @param handle      Frame to be duplicated.
 *  @param framePtr    Location to receive the frame.
 *
 */
Int32
FrameQ_get  (FrameQ_Handle handle, FrameQ_Frame * framePtr)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    UInt32                  j;
    FrameQ_FrameBufInfo     *frameBufInfo;
    Ptr                     addr;
    Ptr                     virtPtr;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_get", handle, framePtr);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->knlObject)
                && (NULL != framePtr)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBuf_Params_init",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_get",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_get",
                             status,
                             "framePtr passed is NULL!");
    }
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_READER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not a reader handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_get",
                             status,
                             "The provided handle does not have permission to"
                             " do put. The handle is not a reader handle.!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object *) handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.get.handle         = handle->knlObject;

        status = FrameQDrv_ioctl (CMD_FRAMEQ_GET, &cmdArgs);
        if (status < 0) {
            *framePtr = NULL;
        }
        else {
            /* Convert Frame and frame buffer address in frame to user
             * virtual space
             */
            *framePtr = Memory_translate((Ptr)cmdArgs.args.get.frame,
                                          Memory_XltFlags_Phys2Virt);

            frameBufInfo = (FrameQ_FrameBufInfo *)
                                         &((*framePtr)->frameBufInfo[0]);
            for(j = 0; j < (*framePtr)->numFrameBuffers; j++) {
                addr = (Ptr)frameBufInfo[j].bufPtr;
                virtPtr = Memory_translate(addr, Memory_XltFlags_Phys2Virt);
                GT_assert(curTrace,(virtPtr != NULL));
                frameBufInfo[j].bufPtr = (UInt32)virtPtr;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_get", status);

    /*! @retval  FrameQ_S_SUCCESS   Able to get frame from the FrameQ */
    return (status);
}

/*
 *  @brief Function to get frame from FrameQ .
 *         Allowed only readers to call  this API.
 *  @param handle      Frame to be duplicated.
 *  @param framePtr    Location to receive the frame.
 *
 */
Int32
FrameQ_getv  (FrameQ_Handle     handle,
              FrameQ_Frame      framePtr[],
              UInt32            filledQueueId[],
              UInt8             *numFrames)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    UInt32                  i       = 0;
    UInt32                  j;
    FrameQ_FrameBufInfo     *frameBufInfo;
    Ptr                     addr;
    Ptr                     virtPtr;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQ_getv",
               handle,
               framePtr,
               filledQueueId,
               *numFrames);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->knlObject)
                && (NULL != framePtr)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBuf_Params_init",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getv",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getv",
                             status,
                             "framePtr passed is NULL!");
    }
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_READER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not a reader handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getv",
                             status,
                             "The provided handle does not have permission to"
                             " do put. The handle is not a reader handle.!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object *) handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getv.handle      = handle->knlObject;
        cmdArgs.args.getv.aframePtr   = Memory_alloc(NULL,
                                                    *numFrames * sizeof(UInt32),
                                                    0,
                                                    NULL);
        cmdArgs.args.getv.filledQueueId     = filledQueueId;
        cmdArgs.args.getv.numFrames         = *numFrames;

        status = FrameQDrv_ioctl (CMD_FRAMEQ_GETV, &cmdArgs);
        if (status < 0) {
            for (i = 0; i < *numFrames; i++) {
                framePtr[i] = NULL;
            }
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_getv",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
            i = 0;
            while( i < *numFrames) {
                /* Convert Frame and Frame buffer address in frame to user virtual
                 * space.
                 */
                framePtr[i] = Memory_translate(
                                            (Ptr)cmdArgs.args.getv.aframePtr[i],
                                             Memory_XltFlags_Phys2Virt);
                frameBufInfo = (FrameQ_FrameBufInfo *)
                                             &(framePtr[i]->frameBufInfo[0]);
                for(j = 0; j < framePtr[i]->numFrameBuffers; j++) {
                    addr = (Ptr)frameBufInfo[j].bufPtr;
                    virtPtr = Memory_translate(addr, Memory_XltFlags_Phys2Virt);
                    GT_assert(curTrace,(virtPtr != NULL));
                    frameBufInfo[j].bufPtr = (UInt32)virtPtr;
                }
                i++;
            }
        }

        if (cmdArgs.args.getv.aframePtr != NULL)  {
            Memory_free (NULL,
                         cmdArgs.args.getv.aframePtr,
                         *numFrames * sizeof(UInt32));
        }


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getv", status);

    /*! @retval  FrameQ_S_SUCCESS   Able to get frame from the FrameQ */
    return (status);
}


/*
 * @brief Function to resister a call back function.
 *
 * @param handle      Reader/Wrieter client Handle.
 * @param params      Nottify params.
 */
Int32
FrameQ_registerNotifier (FrameQ_Handle         handle,
                         FrameQ_NotifyParams  *notifyParams)
{
    Int32                       status       = FrameQ_S_SUCCESS;
    Int32                       tmpStatus;
    ClientNotifyMgr_registerNoitifyParams  regParams;
    FrameQBufMgr_NotifyParams   lnotifyParams;
    FrameQDrv_CmdArgs           cmdArgs;
    UInt32                      notifyId;
    FrameQ_Object               *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "FrameQ_registerNotifier",
               handle,
               notifyParams);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != notifyParams));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not a reader handle.
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_registerNotifier",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    if (notifyParams == NULL) {
        /*! @retval FrameQ_E_INVALIDARG notifyParams passed is null
         */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_registerNotifier",
                             status,
                             "notifyParams passed is null!");
    }
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_READER)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_WRITER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not a reader handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_registerNotifier",
                             status,
                             "The provided handle does not have permission to"
                             " do put. The handle is not a reader/writer handle.!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->isRegistered != TRUE) {
#endif
            notifyParams->cbClientHandle = handle;

            if (obj->objMode == FrameQ_MODE_READER) {
                regParams.clientHandle   = notifyParams->cbClientHandle;
                regParams.procId         = MultiProc_self();
                regParams.notifyType     = notifyParams->notifyType;
                regParams.fxnPtr         =
                                 (ClientNotifyMgr_FnCbck)notifyParams->cbFxnPtr;
                regParams.cbContext      = notifyParams->cbContext;
                regParams.watermarkCond1 = notifyParams->watermark;
                regParams.watermarkCond2 = (UInt32)-1;

                tmpStatus = ClientNotifyMgr_registerClient(
                                                     obj->clientNotifyMgrHandle,
                                                     &regParams,
                                                     &notifyId);
                if (tmpStatus < 0) {
                    status = FrameQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQ_registerNotifier",
                                         status,
                                         "ClientNotifyMgr_registerClient failed.!");
                }
                else {
                    /* Set notify id in the instance 's kernel object also.*/
                    obj->notifyId       = notifyId;
                    obj->isRegistered   = TRUE;
                    cmdArgs.args.setNotifyId.handle   = handle->knlObject;
                    cmdArgs.args.setNotifyId.notifyId = notifyId;
                    status = FrameQDrv_ioctl (CMD_FRAMEQ_SET_NOTIFYID,
                                              &cmdArgs);
        #if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "FrameQ_registerNotifier",
                                             status,
                                             "API (through IOCTL) failed on kernel-side!");
                    }
        #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

            }
            else {
                lnotifyParams.cbClientHandle = notifyParams->cbClientHandle;
                lnotifyParams.notifyType     =
                                   (SysLink_NotifyType)notifyParams->notifyType;
                lnotifyParams.cbFxnPtr       = (FrameQBufMgr_NotifyFunc)
                                                         notifyParams->cbFxnPtr;
                lnotifyParams.cbContext      = notifyParams->cbContext;
                lnotifyParams.watermark      = notifyParams->watermark;

                tmpStatus = FrameQBufMgr_registerNotifier(
                                                        obj->frameQBufMgrHandle,
                                                        &lnotifyParams);
                if (tmpStatus < 0) {
                    status = FrameQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQ_registerNotifier",
                                         status,
                                         "FrameQBufMgr_registerNotifier failed.!");
                }
                else {
                    obj->isRegistered   = TRUE;
                }
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
        else {
            status = FrameQ_E_ALREADYREGISTERED;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_registerNotifier",
                                 status,
                                 "Error:Call back function is already registerd!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_registerNotifier", status);

    /*! @retval  FrameQ_S_SUCCESS  Successfully registered callback fucntion*/
    return (status);
}

/*
 * @brief Function to un resister call back function.
 *
 *        Allowed  readers to call  this API.
 *        For reader :
 *        It Un registers notification function.
 *
 * @param handle      Reader client Handle.
 */
Int32
FrameQ_unregisterNotifier (FrameQ_Handle handle)
{
    Int32                   status       = FrameQ_S_SUCCESS;
    Int32                   tmpStatus;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_1trace (curTrace,
               GT_ENTER,
              "FrameQ_unregisterNotifier",
               handle);

    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_unregisterNotifier",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_READER)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_WRITER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle is not a reader handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_unregisterNotifier",
                             status,
                             "The provided handle does not have permission to"
                             " do put. The handle is not a reader/writer handle.!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->isRegistered == TRUE) {
#endif
            if (obj->objMode == FrameQ_MODE_READER) {
                status = ClientNotifyMgr_unregisterClient (
                                                        obj->clientNotifyMgrHandle,
                                                        obj->notifyId);
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQ_unregisterNotifier",
                                         status,
                                         "ClientNotifyMgr_unregisterClient failed.!");
                }
                else {
                    obj->notifyId     = (UInt32)-1;
                    obj->isRegistered = FALSE;
                    cmdArgs.args.resetNotifyId.handle   = handle->knlObject;
                    cmdArgs.args.resetNotifyId.notifyId = -1;
                    status = FrameQDrv_ioctl (CMD_FRAMEQ_RESET_NOTIFYID,
                                              &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "FrameQ_unregisterNotifier",
                                             status,
                                             "API (through IOCTL) failed on kernel-side!");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
               }
            }
            else {
                /* For FrameQ writer */
                tmpStatus = FrameQBufMgr_unregisterNotifier(
                                                       obj->frameQBufMgrHandle);
                if (tmpStatus < 0) {
                    status = FrameQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQ_unregisterNotifier",
                                         status,
                                         "FrameQBufMgr_unregisterNotifier failed.!");
                }
                else {
                    obj->isRegistered = FALSE;
                }
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
        else {
            status = FrameQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_unregisterNotifier",
                                 status,
                                 "FrameQ_unregisterNotifier failed.!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_unregisterNotifier", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully unregistered call back function*/
    return (status);
}

/*
 * @brief Function to duplicate a frame .
 *
 * @param framePtr    Orginal frame to be duplicated.
 * @param dupFramePtr Location to receive the duplicated frame.
 *
 */
Int32
FrameQ_dup (FrameQ_Handle       handle,
            FrameQ_Frame        frame,
            FrameQ_Frame*       dupedFrame)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQBufMgr_Frame      dupedFramePtr[1];
    FrameQ_Object           *obj;

    GT_3trace (curTrace, GT_ENTER, "FrameQ_dup", handle, frame,dupedFrame);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != handle->knlObject)
                && (NULL != frame)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_dup",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_dup",
                             status,
                             "handle passed is NULL!");
    }
    else if (frame == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_dup",
                             status,
                             "frame passed is NULL!");
    }
    else if (dupedFrame == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_dup",
                             status,
                             "dupedFrame passed is NULL!");
    }
    else if (handle->knlObject == NULL) {
       /*! @retval FrameQ_E_FAIL The provided handle's knlObject is
        * null.
        */
        status = FrameQ_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_dup",
                             status,
                             "The provided handle's knlObject is null.!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object *) handle;
        GT_assert (curTrace, (NULL != obj));

        status = FrameQBufMgr_dup (obj->frameQBufMgrHandle,
                                   frame,
                                   dupedFramePtr,
                                   1);
        /* No Need to check status to assign dupedFramePtr.
         * dupedFramePtr[0] will be NULL if dup fails.
         */
        *dupedFrame = (FrameQ_Frame) dupedFramePtr[0];

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_dup", status);

    /*! @retval  FrameQ_S_SUCCESS   Able to get frame from the FrameQ */
    return (status);
}

/*
 * @brief Function to send forced notification to the reader clients.
 *
 * @param handle    Reader client Handle.
 * @param msg       Payload.
 */
Int32
FrameQ_sendNotify (FrameQ_Handle handle, UInt16 msg)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQ_Object              *obj;

    GT_2trace (curTrace, GT_ENTER, "FrameQ_sendNotify", handle, msg);

    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_sendNotify",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_sendNotify",
                             status,
                             "handle passed is NULL!");
    }
    else if (   (handle->knlObject != NULL)
             && (((FrameQ_Object*) handle)->objMode != FrameQ_MODE_WRITER)) {
        /*! @retval FrameQ_E_ACCESSDENIED The provided handle does not have
         * permission to do put. The handle should be a writer handle.
         */
        status = FrameQ_E_ACCESSDENIED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_sendNotify",
                             status,
                             "The provided handle does not have permission to"
                             " do put. The handle is not a reader handle.!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        /* Send force notification to all the registered Reader clients.
         * status  will be  success if atleast one reader is notified.
         */
        status = ClientNotifyMgr_sendForceNotificationMulti(
                                            obj->clientNotifyMgrHandle,
                                            (UInt32)-1,
                                             msg);
        if (status < 0) {
            status = FrameQ_E_FAIL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_sendNotify", status);

    /*! @retval  FrameQ_S_SUCCESS   Able to get frame from the FrameQ */
    return (status);
}

/*
 * @brief Function to find out the number of available frames in a FrameQ .
 *
 *        For Writer:
 *        It returns the  number of available frames in the primary queue.
 *        For Reader:
 *        It returns the number of available frames in the queue that the reader
 *        has access to.
 *
 * @param handle      Reader client Handle.
 * @param handle      Reader client Handle.
 */
Int32
FrameQ_getNumFrames(FrameQ_Handle handle,
                    UInt32         *numFrames)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "FrameQ_getNumFrames",
               handle,
               numFrames);

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (NULL != numFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getNumFrames",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getNumFrames",
                             FrameQ_E_INVALIDSTATE,
                             "handle passed is null!");
    }
    else if (numFrames == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getNumFrames",
                             FrameQ_E_INVALIDSTATE,
                             "numFrames pointer is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getNumFrames.handle   = handle->knlObject;
        status = FrameQDrv_ioctl (CMD_FRAMEQ_GET_NUMFRAMES, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_getNumFrames",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            *numFrames = cmdArgs.args.getNumFrames.numFrames;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getNumFrames", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully queried the number of frames*/
    return (status);
}

/*
 * @brief Function to find out the number of available frames in a FrameQ .
 *
 *        For Writer:
 *        It returns the  number of available frames in the primary queue.
 *        For Reader:
 *        It returns the number of available frames in the queue that the reader
 *        has access to.
 *
 * @param handle      Reader client Handle.
 * @param handle      Reader client Handle.
 */
Int32
FrameQ_getvNumFrames(FrameQ_Handle     handle,
                     UInt32            numFrames[],
                     UInt8             filledQId[],
                     UInt8             numFilledQids)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    UInt32                  i;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;
    UInt32  tmpNumFrames[FrameQ_MAX_FRAMESINVAPI];

    GT_2trace (curTrace,
               GT_ENTER,
              "FrameQ_getvNumFrames",
               handle,
               numFrames);

    GT_assert (curTrace, (numFilledQids <= FrameQ_MAX_FRAMESINVAPI));

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (NULL != numFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getvNumFrames",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getvNumFrames.handle    = handle->knlObject;
        cmdArgs.args.getvNumFrames.numFrames = tmpNumFrames;
        cmdArgs.args.getvNumFrames.filledQId  = (Ptr)filledQId;
        cmdArgs.args.getvNumFrames.numFilledQids  = numFilledQids;

        status = FrameQDrv_ioctl (CMD_FRAMEQ_GET_VNUMFRAMES, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_ShMem_getNumFrames",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            for (i = 0; i < numFilledQids; i++) {
                 /* Get valid frames  */
                numFrames[i] = cmdArgs.args.getvNumFrames.numFrames[i];
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getvNumFrames", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully queried the number of frames*/
    return (status);
}

/*
 *   @brief Function to get the number of free frames available in the plugged in
 *          FrameQbufMgr's FreeFramePool 0.
 *          The returned number free frame may not exist in the frameQbufMgr
 *          after this call.This is because if any allocs are done by the other
 *          FrameQ writers on the same FrameQBufMgr.If frees are not other
 *          frameQ writers the free Frames would be even more than the value
 *          that we have got.
 *
 * @param handle            FrameQ Handle.
 * @param numFreeFrames     Location to receive the number of frames.
 */
Int32
FrameQ_getNumFreeFrames  (FrameQ_Handle handle,
                          UInt32* numFreeFrames)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "FrameQ_getNumFreeFrames",
               handle,
               numFreeFrames);

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (NULL != numFreeFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getNumFreeFrames",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getNumFreeFrames.handle   = handle->knlObject;
        status = FrameQDrv_ioctl (CMD_FRAMEQ_GET_NUMFREEFRAMES, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_getNumFreeFrames",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            *numFreeFrames = cmdArgs.args.getNumFreeFrames.numFreeFrames;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getNumFreeFrames", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully queried the number of frames*/
    return (status);
}

/*
 * @brief Function to get the number of free frames available in the plugged in
 *        FrameQbufMgr.
 *
 */
Int32
FrameQ_getvNumFreeFrames(FrameQ_Handle     handle,
                         UInt32            numFreeFrames[],
                         UInt8             freeQId[],
                         UInt8             numFreeQids)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    UInt32                  i;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;
    UInt32  tmpNumFreeFrames[FrameQ_MAX_FRAMESINVAPI];

    GT_3trace (curTrace,
               GT_ENTER,
              "FrameQ_getvNumFreeFrames",
               handle,
               numFreeFrames,
               freeQId);

    GT_assert (curTrace, (numFreeQids <= FrameQ_MAX_FRAMESINVAPI));

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (NULL != numFreeFrames));

    GT_assert (curTrace, (NULL != freeQId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getvNumFreeFrames",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getvNumFreeFrames.handle    = handle->knlObject;
        cmdArgs.args.getvNumFreeFrames.numFreeFrames = tmpNumFreeFrames;
        cmdArgs.args.getvNumFreeFrames.freeQId  = (Ptr)freeQId;
        cmdArgs.args.getvNumFreeFrames.numFreeQids  = numFreeQids;

        status = FrameQDrv_ioctl (CMD_FRAMEQ_GET_VNUMFREEFRAMES, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_getvNumFreeFrames",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            for (i = 0; i < numFreeQids; i++) {
                 /* Get valid frames  */
                numFreeFrames[i] =
                        cmdArgs.args.getvNumFreeFrames.numFreeFrames[i];
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getvNumFreeFrames", status);

    /*! @retval  FrameQ_S_SUCCESS   Successfully queried the number of frames*/
    return (status);
}

/*
 * @brief Provides a hook to perform implementation dependent operation
 *
 * @param handle    Instance handle.
 * @param cmd       Command to perform.
 * @param arg       void * argument.
 */
 Int32
FrameQ_control(FrameQ_Handle    handle,
               Int32            cmd,
               Ptr              arg)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQDrv_CmdArgs       cmdArgs;
    FrameQ_Object           *obj;

    GT_3trace (curTrace,
               GT_ENTER,
              "FrameQ_control",
               handle,
               cmd,
               arg);

    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_control",
                             FrameQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQ_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.control.handle   = handle->knlObject;
        cmdArgs.args.control.cmd      = cmd;
        cmdArgs.args.control.arg      = arg;
        status = FrameQDrv_ioctl (CMD_FRAMEQ_CONTROL, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_control", status);

    /*! @retval  FrameQ_S_SUCCESS   Operation successful*/
    return (status);
}

/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation for  shared memory.
 *              NOTE: Only supports returning default params.ie. first arg handle
 *              is ignored.
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GateMP_create
 */
Void
FrameQ_ShMem_Params_init (FrameQ_ShMem_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status  = FrameQ_S_SUCCESS;
#endif
    FrameQDrv_CmdArgs       cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_ShMem_Params_init", params);
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_ShMem_Params_init",
                             FrameQ_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_ShMem_Params_init",
                             FrameQ_E_INVALIDARG,
                             "Argument of type (FrameQ_ShMem_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
        cmdArgs.args.ParamsInit.handle = NULL ;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        FrameQDrv_ioctl (CMD_FRAMEQ_SHMEM_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_ShMem_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "FrameQ_ShMem_Params_init");
}

/********************* Internal functions *************************************/
/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GateMP_create
 */
Void
FrameQ_Params_init (FrameQ_Handle handle, FrameQ_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status  = FrameQ_S_SUCCESS;
#endif
    FrameQDrv_CmdArgs       cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQ_Params_init", params);
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_Params_init",
                             FrameQ_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_Params_init",
                             FrameQ_E_INVALIDARG,
                             "Argument of type (FrameQ_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
        cmdArgs.args.ParamsInit.handle = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        FrameQDrv_ioctl (CMD_FRAMEQ_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "FrameQ_Params_init");
}

/*!
 * @brief   Internal function called by FrameQ_create function when
 *          openFlag is  FALSE.
 *
 * @param   object FrameQ handle(uncooked)
 */
static inline
Int32
_FrameQ_create (FrameQ_Handle  handle,
                FrameQ_Params* params)
{
    String                          fQBufMgrName   = NULL;
    Ptr                             fQBufMgrShAddr = NULL;
    Int32                           status    = FrameQ_S_SUCCESS;
    UInt32                          cpuAccessFlags = 0;
    ClientNotifyMgr_Handle          cliMgrHandle = NULL;
    GateMP_Handle                   gateHandle = NULL;
    Ptr                             sharedAddr = NULL;
    FrameQ_ShMem_Params             *pShMemParams;
    ClientNotifyMgr_Params          clientMgrParams;
    UInt16                          index;
    FrameQDrv_CmdArgs               cmdArgs;
    FrameQBufMgr_ShMem_OpenParams         frmQBufMgrOpenParams;
    FrameQBufMgr_Handle             lframeQBufMgrHandle;
    Int32                           tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    FrameQ_NameServerEntry          entry;
    UInt32                          len;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    FrameQ_Object                   *obj;

    GT_2trace (curTrace, GT_ENTER, "_FrameQ_create", handle,params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
         /*! @retval NULL  Module was not initialized!
          */
        status = FrameQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQ_create",
                             status,
                             "Module was not initialized!");
    }
    else if ( NULL == handle) {
         /*! @retval NULL  Params passed is NULL
          */
         status = FrameQ_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQ_create",
                             status,
                             "obj of FrameQ_Object * is NULL!");

    }
    else if ( NULL == params) {
         /*! @retval NULL  Params passed is NULL
          */
         status = FrameQ_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQ_create",
                             status,
                             "Params pointer is NULL!");

    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if name is already exists in the name server */
        if (FrameQ_module->nameServer != NULL) {
            len = sizeof (FrameQ_NameServerEntry);
            status = NameServer_get (FrameQ_module->nameServer,
                                     (String)params->commonCreateParams.name,
                                     &entry,
                                     &len ,
                                     NULL);
            if (status >= 0) {
                /* Already instance with the name exists */
                /*! @retval NULL  Instance is already created and entry exists
                 *  in Nameserver.
                 */
                status = FrameQ_E_INST_EXISTS;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_FrameQ_create",
                                     status,
                                     "Instance is already created and entry"
                                     "exists in Nameserver.!");

            }
            else if ((status != NameServer_E_NOTFOUND) && (status < 0)) {
                /* Error happened in NameServer. Pass the error up. */
                /*! @retval NULL  IError happened in NameServer.
                 */
                status = FrameQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_FrameQ_create",
                                     status,
                                     "Error happened in NameServer.!");
            }
            else {
                /* Go ahead and create as there is no instance with that name
                 * exists.
                 */
                 status = FrameQ_S_SUCCESS;
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
                case FrameQ_INTERFACE_SHAREDMEM:
                {
                    pShMemParams     = (FrameQ_ShMem_Params *)params;
                    cpuAccessFlags   = pShMemParams->cpuAccessFlags;

                    cmdArgs.args.create.interfaceType = FrameQ_INTERFACE_SHAREDMEM;
                    cmdArgs.args.create.params = Memory_alloc(NULL,
                                            sizeof(FrameQ_ShMem_Params),
                                            0,
                                            NULL);
                    GT_assert(curTrace, (cmdArgs.args.create.params != NULL));
                    Memory_copy(cmdArgs.args.create.params,
                               pShMemParams,
                               sizeof(FrameQ_ShMem_Params));

                    cmdArgs.args.create.name =
                                          pShMemParams->commonCreateParams.name;
                    if (pShMemParams->commonCreateParams.name != NULL) {
                        cmdArgs.args.create.nameLen =
                         String_len (pShMemParams->commonCreateParams.name) + 1;
                    }
                    else {
                        cmdArgs.args.create.nameLen = 0;
                    }

                    fQBufMgrName =
                    cmdArgs.args.create.fQBMName =
                                                 pShMemParams->frameQBufMgrName;
                    if (pShMemParams->frameQBufMgrName != NULL) {
                        cmdArgs.args.create.fQBMnameLen =
                         String_len (pShMemParams->frameQBufMgrName) + 1;
                    }
                    else {
                        cmdArgs.args.create.fQBMnameLen = 0;
                    }

#ifdef SYSLINK_BUILDOS_QNX
                    if (pShMemParams->gate != NULL) {
                        ((FrameQ_ShMem_Params*)
                        cmdArgs.args.create.params)->gate =
                        (GateMP_Handle)GateMP_getSharedAddr(pShMemParams->gate);
                    }
                    else {
                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->
                            gate = (GateMP_Handle)SharedRegion_INVALIDSRPTR;
                    }
#else
                    if (pShMemParams->gate != NULL) {
                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->
                            gate = (Ptr) GateMP_getKnlHandle(pShMemParams->gate);

                    }
                    else {
                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->gate = NULL;
                    }
#endif

                    /* Convert user space shared addr to shared region address */
                    if (pShMemParams->sharedAddr != NULL ) {
                        index = SharedRegion_getId(pShMemParams->sharedAddr);

                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->sharedAddr = (Ptr)
                            SharedRegion_getSRPtr(
                                              pShMemParams->sharedAddr, index);
                        GT_assert (curTrace,
                                   (SharedRegion_SRPtr)
                                   (((FrameQ_ShMem_Params*) cmdArgs.args.create.params)
                                    ->sharedAddr) != SharedRegion_INVALIDSRPTR);
                    }
                    else {
                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->sharedAddr
                                                    = (Ptr)SharedRegion_INVALIDSRPTR;
                    }

                    fQBufMgrShAddr = pShMemParams->frameQBufMgrSharedAddr;
                    /* Convert user space shared addr to shared region address */
                    if (pShMemParams->frameQBufMgrSharedAddr != NULL ) {
                        index = SharedRegion_getId(pShMemParams->frameQBufMgrSharedAddr);

                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->frameQBufMgrSharedAddr = (Ptr)
                            SharedRegion_getSRPtr(
                                              pShMemParams->frameQBufMgrSharedAddr, index);
                        GT_assert (curTrace,
                                   (SharedRegion_SRPtr)
                                   (((FrameQ_ShMem_Params*)cmdArgs.args.create.params)
                                      ->frameQBufMgrSharedAddr)
                                      != SharedRegion_INVALIDSRPTR);
                    }
                    else {
                        ((FrameQ_ShMem_Params*)cmdArgs.args.create.params)->frameQBufMgrSharedAddr
                                                    = (Ptr)SharedRegion_INVALIDSRPTR;
                    }

                    status =
                        FrameQDrv_ioctl (CMD_FRAMEQ_CREATE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (
                                          curTrace,
                                          GT_4CLASS,
                                          "FrameQ_ShMem_create",
                                          status,
                                          "API (through IOCTL) failed on kernel-side!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                        obj                 = handle;
                        /* Set pointer to kernel object into the user handle. */
                        obj->knlObject      = cmdArgs.args.create.handle;
                        obj->objMode        = FrameQ_MODE_NONE;
                        obj->interfaceType  = FrameQ_INTERFACE_SHAREDMEM;
                        obj->objType        = FrameQ_DYNAMIC_CREATE;
                        obj->notifyId       = (UInt32)-1;
			            obj->isRegistered   = FALSE;
                        obj->ctrlStructCacheFlag = 
						    SharedRegion_isCacheEnabled(pShMemParams->regionId);
            
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                   }
#endif
                    if (cmdArgs.args.create.params != NULL) {
                        Memory_free( NULL,
                                     cmdArgs.args.create.params,
                                     sizeof(FrameQ_ShMem_Params));
                    }
                }
                break;

                default:
                    status = FrameQ_E_INVALID_INTERFACE;
                    GT_setFailureReason(curTrace, GT_4CLASS, "_FrameQ_create",
                            status, "invalid interface type!");
            }
        }

        if (status >= 0)  {
            frmQBufMgrOpenParams.commonOpenParams.name = fQBufMgrName;
            if (pShMemParams->frameQBufMgrName == NULL) {
                frmQBufMgrOpenParams.commonOpenParams.sharedAddr =
                                                                 fQBufMgrShAddr;
            }
            else {
                frmQBufMgrOpenParams.commonOpenParams.sharedAddr = NULL;
            }

            frmQBufMgrOpenParams.commonOpenParams.cpuAccessFlags  =
                                                       pShMemParams->cpuAccessFlags;
            tmpStatus = FrameQBufMgr_open (&lframeQBufMgrHandle,
                                           &frmQBufMgrOpenParams);
            if (tmpStatus < 0) {
                handle->frameQBufMgrHandle = NULL;
                status= FrameQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_FrameQ_create",
                                     status,
                                     "FrameQBufMgr_open() failed!");
            }
            else {
                handle->frameQBufMgrHandle = lframeQBufMgrHandle;
            }
        }
        if (status >= 0) {
            ClientNotifyMgr_Params_init(&clientMgrParams);
            clientMgrParams.regionId = pShMemParams->regionId;
            clientMgrParams.numNotifyEntries = pShMemParams->numReaders;
            clientMgrParams.numSubNotifyEntries = pShMemParams->numQueues;
            /* Open the ClientNotifyMgr on user side */
            clientMgrParams.sharedAddr = SharedRegion_getPtr(
                                (cmdArgs.args.create.cliNotifyMgrSharedMem));
            sharedAddr     = SharedRegion_getPtr
                                     (cmdArgs.args.create.cliGateSharedMem);
            status = GateMP_openByAddr (sharedAddr, &gateHandle);
            GT_assert (curtrace,(status >= 0));
           /*
            * Pass the shared memory address to create the client
            * manager instance. This address starts right after the
            * size of gate peterson from the
            * params->sharedAddr.
            */
            clientMgrParams.gate = (Ptr) gateHandle;
            handle->clientNotifyMgrGate = (Ptr) gateHandle;

            /* Pass the same name for ClientNotifyMgr.
             */
            clientMgrParams.name = params->commonCreateParams.name;
            /* Event no to be used for this instance */
            clientMgrParams.eventNo = FrameQ_NOTIFY_RESERVED_EVENTNO;
            /* This is actually create call */
            clientMgrParams.openFlag = TRUE;

            if (obj->ctrlStructCacheFlag == TRUE) {
                clientMgrParams.cacheFlags =
                                   ClientNotifyMgr_CONTROL_CACHEUSE;
            }
            cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);
            GT_assert (curtrace,(NULL != cliMgrHandle));
            if (NULL == cliMgrHandle) {
                /*! @retval FrameQ_E_FAIL_CLIEN0TIFYMGR_CREATE
                 * Failed to create ClientNotifyMgr instance.
                 */
                status = FrameQ_E_FAIL_CLIENTN0TIFYMGR_CREATE;
                GT_setFailureReason (
                                 curTrace,
                                 GT_4CLASS,
                                 "_FrameQ_create",
                                 status,
                                 "Failed to create ClientNotifyMgr "
                                 "instance." );
                handle->clientNotifyMgrHandle = NULL;
            }
            else {
                handle->clientNotifyMgrHandle = cliMgrHandle;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_FrameQ_create", status);

    return (status);
}



/*!
 *  @brief      API to get the shared memory requirements of the shared mem
 *              FrameQ.
 *  @param
 *
 *  @sa
 */
UInt32
FrameQ_ShMem_sharedMemReq (const FrameQ_ShMem_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status  = FrameQ_S_SUCCESS;
#endif
    UInt32                  size = 0;
    FrameQDrv_CmdArgs       cmdArgs;


    GT_1trace (curTrace, GT_ENTER, "FrameQ_ShMem_sharedMemReq", params);
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQ_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_ShMem_sharedMemReq",
                             FrameQ_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_ShMem_sharedMemReq",
                             FrameQ_E_INVALIDARG,
                             "Argument of type (FrameQ_ShMem_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.sharedMemReq.params = (Ptr)params;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        FrameQDrv_ioctl (CMD_FRAMEQ_SHMEM_MEMREQ, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQ_ShMem_sharedMemReq",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
            size = cmdArgs.args.sharedMemReq.bytes;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "FrameQ_ShMem_Params_init");

    return (size);
}
