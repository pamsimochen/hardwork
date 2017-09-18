/*
 *  @file   FrameQBufMgr.c
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

/* Utilities & OSAL headers */
#include <ti/syslink/utils/Gate.h>

#include <ti/syslink/utils/GateMutex.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/_MultiProc.h>

#include <ti/syslink/utils/SyslinkMemMgr.h>
/* Module level headers */
#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/ipc/_FrameQBufMgr_ShMem.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>
#include <ti/syslink/inc/usr/Linux/FrameQBufMgrDrv.h>
#include <ti/syslink/inc/FrameQBufMgrDrvDefs.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/_SharedRegion.h>


/** ============================================================================
 *  @const  IPC_BUFFER_ALIGN
 *
 *  @desc   Macro to align a number.
 *          x: The number to be aligned
 *          y: The value that the number should be aligned to.
 *  ============================================================================
 */
#define IPC_BUFFER_ALIGN(x, y) (UInt32)((UInt32)((x + y - 1) / y) * y)

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/* Structure defining object for the Gate Peterson */
struct FrameQBufMgr_Object {
    List_Elem    listElem;


    UInt32                  objType;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    UInt32                  ctrlInterfaceType;
    /*!<  Type of the interface */

    Ptr                     knlObject;
    /*!< Pointer to the kernel-side FrameQ object. */
    UInt32                  instId;
    /*!< Id of the instance*/
    ClientNotifyMgr_Handle  clientNotifyMgrHandle;
    /* ClientNotifyMgr instance used for this instance at user space */
    GateMP_Handle             clientNotifyMgrGate;
    /* Gate used for ClientNotifyMgr instance */
    UInt32                  notifyId;
    Bool                    isRegistered;

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

typedef struct FrameQBufMgr_Object  FrameQBufMgr_Object;
/*!
 *  @brief  FrameQ Module state object on user space
 */
typedef struct FrameQBufMgr_ModuleObject_Tag {
    UInt32              setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
    FrameQBufMgr_Config cfg;
    /* Current config */
    List_Object         objList;
    /* List holding created objects */
    IGateProvider_Handle listLock;
    /* Lock to protect the objList */
    NameServer_Handle   nameServer;
    /* Handle to the nameserver created for  FrameQBufMgr */
} FrameQBufMgr_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    FrameQBufMgr_module_obj
 *
 *  @brief  FrameQBufMgr state object variable  in process
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
FrameQBufMgr_ModuleObject FrameQBufMgr_module_obj =
{
    .setupRefCount = 0
};

/*!
 *  @var    FrameQBufMgr_module
 *
 *  @brief  Pointer to FrameQBufMgr_module_obj .
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
FrameQBufMgr_ModuleObject* FrameQBufMgr_module = &FrameQBufMgr_module_obj ;

/*=============================================================================
 * Forward declarations of internal Functions
 *=============================================================================
 */
static inline
Int32 _FrameQBufMgr_create(FrameQBufMgr_Handle handle,
                      FrameQBufMgr_Params* params);

/* =============================================================================
 * APIS
 * =============================================================================
 */

/*!
 *  @brief  Function to get the  Module config parameters.
 *
 *  @param  cfg Configuration values.
 */
Int32 FrameQBufMgr_getConfig(FrameQBufMgr_Config *cfg)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgrDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        status = FrameQBufMgr_E_INVALIDARG ;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getConfig",
                             status,
                             "Argument of type (FrameQBufMgr_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Temporarily open the handle to get the configuration. */
        status = FrameQBufMgrDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            /*! @retval FrameQBufMgr_E_INVALIDSTATE  Failed to open driver handle.
             */
            status = FrameQBufMgr_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.getConfig.config = cfg;
            status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_GETCONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_getConfig",
                                     status,
                                    "API (through IOCTL) failed on kernel-side!");

            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        FrameQBufMgrDrv_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_getConfig", status);

    /*! @retval FrameQBufMgr_S_SUCCESS operation was successful */
    return (status);
}

/*!
 *  @brief      Function to setup the FrameQBufMgr module.
 *
 *  @sa         FrameQBufMgr_destroy
 */
Int32
FrameQBufMgr_setup (FrameQBufMgr_Config * config)
{
    Int                      status  = FrameQBufMgr_S_SUCCESS;
    Error_Block              eb;
    IArg                     key;
    FrameQBufMgrDrv_CmdArgs  cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_setup", config);
    Error_init (&eb);

    key = Gate_enterSystem();
    FrameQBufMgr_module->setupRefCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (FrameQBufMgr_module->setupRefCount > 1) {
        Gate_leaveSystem(key);
        /*! @retval FrameQBufMgr_S_ALREADYSETUP Success: FrameQBufMgr module has been
                                           already setup in this process */
        status = FrameQBufMgr_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "FrameQBufMgr module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   FrameQBufMgr_module->setupRefCount);
    }
    else {
        Gate_leaveSystem(key);
        /* Open the driver handle. */
        status = FrameQBufMgrDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.setup.config = (FrameQBufMgr_Config *) config;
            status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_SETUP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_setup",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                FrameQBufMgr_module->nameServer =
                              NameServer_getHandle(FRAMEQBUFMGR_NAMESERVERNAME);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (FrameQBufMgr_module->nameServer == NULL) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgr_setup",
                                         status,
                                         "NameServer_getHandle() Failed!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    Memory_copy ((Ptr) &FrameQBufMgr_module->cfg,
                                 (Ptr) config,
                                 sizeof (FrameQBufMgr_Config));
                    /* Construct the list object */
                    List_construct (&FrameQBufMgr_module->objList, NULL);
                    FrameQBufMgr_module->listLock = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
                    GT_assert(curTrace,(FrameQBufMgr_module->listLock != NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_setup", status);

    /*! @retval FrameQBufMgr_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the FrameQBufMgr module.
 *
 *  @sa         FrameQBufMgr_setup
 */
Int32
FrameQBufMgr_destroy (Void)
{
    Int                         status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    IArg                        key;
    Int32                       tmpStatus;

    GT_0trace (curTrace, GT_ENTER, "FrameQBufMgr_destroy");

    key = Gate_enterSystem();
    if (FrameQBufMgr_module->setupRefCount <= 0) {
        Gate_leaveSystem(key);
        /*! @retval FrameQ_S_ALREADYDESTROYED Success: FrameQBufMgr module has
         * been already destroyed in this process
         */
        status = FrameQBufMgr_S_ALREADYDESTROYED;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "FrameQBufMgr module has been already destroyed in this process.\n"
                   "    RefCount: [%d]\n",
                   FrameQBufMgr_module->setupRefCount);
    }
    else  {
        FrameQBufMgr_module->setupRefCount--;
        /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
        if (FrameQBufMgr_module->setupRefCount > 1) {
            Gate_leaveSystem(key);
            /*! @retval FrameQBufMgr_S_ALREADYSETUP Success: FrameQBufMgr module has
             * been already setup in this process
             */
            status = FrameQBufMgr_S_ALREADYSETUP;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "FrameQBufMgr module has been already setup in this "
                       "process.\n    RefCount: [%d]\n",
                       FrameQBufMgr_module->setupRefCount);
        }
        else {
            Gate_leaveSystem(key);
            status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_DESTROY, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_destroy",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Destruct the list object */
            List_destruct (&FrameQBufMgr_module->objList);

            /* Delete the list lock */
            tmpStatus = GateMutex_delete ((GateMutex_Handle*)&FrameQBufMgr_module->listLock);
            if (tmpStatus < 0) {
                status  = FrameQBufMgr_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_destroy",
                                     status,
                                     "GateMutex_delete failed !");
            }
                /* Clear cfg area */
            Memory_set ((Ptr) &FrameQBufMgr_module->cfg,
                        0,
                        sizeof (FrameQBufMgr_Config));
            /* Close the driver handle. */
            FrameQBufMgrDrv_close ();
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_destroy", status);

    return (status);
}


/*!
 *  @brief      Creates a new instance of FrameQBufMgr module.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         FrameQBufMgr_delete, FrameQBufMgr_open, FrameQBufMgr_close
 */
FrameQBufMgr_Handle FrameQBufMgr_create(Ptr vparams)
{
    Int32 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Handle     handle         = NULL;
    FrameQBufMgr_Params     *params = (FrameQBufMgr_Params*)vparams;
//    IArg                     key;
    IArg                    key0;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_create", vparams);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_create",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_create",
                             FrameQBufMgr_E_INVALIDARG,
                             "params passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate memory for the handle */
        handle = (FrameQBufMgr_Handle) Memory_calloc (
                                                   NULL,
                                                   sizeof (FrameQBufMgr_Object),
                                                   0,
                                                   NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle == NULL) {
            /*! @retval NULL Memory allocation failed for handle */
            status = FrameQBufMgr_E_ALLOC_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_create",
                                 status,
                                 "Memory allocation failed for handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            status = _FrameQBufMgr_create (handle, params);
            if (status < 0) {
                /*! @retval NULL  Failed to create the instance.
                 */
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_create",
                                     status,
                                     "Failed to create instance !");
           }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif  /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (status >= 0) {
            /* Put in the local list */
            key0 = IGateProvider_enter (FrameQBufMgr_module->listLock);
            List_elemClear (&handle->listElem);
            List_put ((List_Handle) &(FrameQBufMgr_module->objList),
                       &(handle->listElem));
            IGateProvider_leave (FrameQBufMgr_module->listLock, key0);
        }
        else {
            if (handle != NULL) {
                Memory_free (NULL, handle, sizeof (FrameQBufMgr_Object));
                handle = NULL;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)

    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_create", handle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */
    return (handle);
}

/*!
 *  @brief      Deletes a instance of FrameQBufMgr module.
 *
 *  @param      handlePtr  Pointer to handle to previously created instance. The
 *                         user pointer is reset upon success.
 *
 *  @sa         FrameQBufMgr_create, FrameQBufMgr_open, FrameQBufMgr_close
 */
Int32
FrameQBufMgr_delete (FrameQBufMgr_Handle * handlePtr)
{
    Int32                       status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    FrameQBufMgr_Object     *obj ;
//    IArg                     key;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (*handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE Modules is invalidstate*/
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_delete",
                             status,
                             "Modules is invalidstate!");
    }
    else if (handlePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_delete",
                             status,
                             "handlePtr passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG *handlePtr passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_delete",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQBufMgr_Object *)(*handlePtr) ;
        switch (obj->objType) {

        case FrameQBufMgr_DYNAMIC_OPEN:
        {
           // key0 = IGateProvider_enter (FrameQBufMgr_module->listLock);
            /* Remove it from the local list */
            List_remove ((List_Handle) &FrameQBufMgr_module->objList,
                         &(*handlePtr)->listElem);
           // IGateProvider_leave(FrameQBufMgr_module->listLock, key0);

            status = ClientNotifyMgr_close (
                                       &((* handlePtr)->clientNotifyMgrHandle));
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
            cmdArgs.args.close.handle = (*handlePtr)->knlObject;
            status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_CLOSE,
                                            &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_delete",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Memory_free (NULL,
                        (*handlePtr),
                         sizeof (FrameQBufMgr_Object));
           *handlePtr = NULL;
        }
        break;

        case FrameQBufMgr_DYNAMIC_CREATE:
        {
            // key0 = IGateProvider_enter (FrameQBufMgr_module->listLock);
            /* Remove it from the local list */
            List_remove ((List_Handle) &FrameQBufMgr_module->objList,
                         &(*handlePtr)->listElem);
           // IGateProvider_leave(FrameQBufMgr_module->listLock, key0);
            status = ClientNotifyMgr_close (
                                        &((*handlePtr)->clientNotifyMgrHandle));
            if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgr_delete",
                                         status,
                                         "ClientNotifyMgr_delete failed!");
            }
            else {
                GateMP_close (&((*handlePtr)->clientNotifyMgrGate));
            }
            cmdArgs.args.deleteInstance.handle = (*handlePtr)->knlObject;
            status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_DELETE,
                                            &cmdArgs);
            Memory_free (NULL,
                        (*handlePtr),
                        sizeof (FrameQBufMgr_Object));
            *handlePtr = NULL;
        }
        break;

        default:
            status = FrameQBufMgr_E_ACCESSDENIED;
            /*! @retval FrameQBufMgr_E_ACCESSDENIED  Instance is not dynamically
             * created or opened .
             */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_delete",
                                 status,
                                 "Invalid Handle!");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_delete", status);

    /*! @retval FrameQBufMgr_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief  Function to open the created FrameQBufMgr instance.
 *          The instance must be  created before openinig it.
 *
 *  @param  handlePtr
 *          Location to receive the created FrameQBufMgr instance.
 *  @param  intOpenParams
 *          open parameters specific to implementation. FrameQ_ShMem_OpenParams
 *          in case the instance is of type ShMem.
 */
Int32
FrameQBufMgr_open(FrameQBufMgr_Handle     *handlePtr,
                  Ptr                      instOpenParams)
{
    Int32                       status      = FrameQBufMgr_S_SUCCESS;
    ClientNotifyMgr_Handle      cliMgrHandle = NULL;
    GateMP_Handle               gateHandle   = NULL;
    FrameQBufMgr_Handle         handle = NULL;
    FrameQBufMgr_OpenParams     *openParams = (FrameQBufMgr_OpenParams*)
        instOpenParams;
    Ptr                         sharedAddr = NULL;
    FrameQBufMgr_ShMem_OpenParams *shMemOpenParams;
    Int32                        tmpStatus;
    FrameQBufMgr_NameServerEntry entry;
    FrameQBufMgr_NameServerEntry *pentry;
    UInt32                       ctrlInterfaceType;
    UInt32                       bufInterfaceType;
    UInt8                        nameLen;
    IArg                         key;
    ClientNotifyMgr_Params      clientMgrParams;
    UInt16                      index;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    UInt32                      len;
    FrameQBufMgr_Object         *obj;
    Bool                        ctrlStructCacheFlag = FALSE;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_open",
               handlePtr,
               instOpenParams);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (instOpenParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE Modules is invalidstate*/
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_open",
                             status,
                             "Modules is invalidstate!");
    }
    else if (handlePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_open",
                             status,
                             "handlePtr passed is NULL!");
    }
    else if (   (String_len((String)openParams->commonOpenParams.name) == 0)
              &&(openParams->commonOpenParams.sharedAddr == NULL)) {
        /*! @retval FrameQBufMgr_E_INVALIDARG
         * Either name or sharedaddr must be non null.
         */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_open",
                             status,
                             "Either name or sharedaddr must be non null..");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        nameLen = String_len((String)openParams->commonOpenParams.name);
        if (   ( (nameLen > 0) && ( nameLen <= (FrameQBufMgr_MAXNAMELEN-1))
            && (NULL != FrameQBufMgr_module->nameServer))) {
            len =sizeof (FrameQBufMgr_NameServerEntry);
            status = NameServer_get (FrameQBufMgr_module->nameServer,
                                     (String)openParams->commonOpenParams.name,
                                     &entry,
                                     &len,
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
            GT_assert(curTrace, (index != SharedRegion_INVALIDREGIONID));

            GT_assert(curTrace, ((UInt32)pentry %
                          SharedRegion_getCacheLineSize(index) == 0));

            if (SharedRegion_isCacheEnabled(index)) {
                /* Invalidating the entry located in shared memory. The memory
                 * provided by this entry should be in cache aligend size
                 */
                key = Gate_enterSystem();
                Cache_inv (pentry,
                           sizeof (FrameQBufMgr_NameServerEntry),
                           Cache_Type_ALL,
                           TRUE);
                Gate_leaveSystem(key);
            }
            ctrlInterfaceType = pentry->ctrlInterfaceType;
            bufInterfaceType  = pentry->bufInterfaceType;
        }
        else {
            status = FrameQBufMgr_E_INVALIDARG;
            /*! @retval FrameQBufMgr_E_INVALIDARG
             * Either name or sharedaddr must be non null.
             */
            status = FrameQBufMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_open",
                                 status,
                                 "Either name or sharedaddr must be non null.");
        }
        if (status >= 0) {
             /* Check the interface type and if it is not supported
              * return failure.
              */
            if( !FrameQBufMgr_isSupportedInterface(ctrlInterfaceType)) {
                /*! @retval FrameQBufMgr_E_INVALID_INTERFACE
                 * Unsupported interface type.
                 */
                status = FrameQBufMgr_E_INVALID_INTERFACE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_open",
                                     status,
                                     "Unsupported interface type.");
            }
            if( !FrameQBufMgr_isSupportedBufInterface(ctrlInterfaceType,
                                                      bufInterfaceType)) {
                /*! @retval FrameQBufMgr_E_INVALID_BUFINTERFACETYPE
                 * Unsupported buf interface type.
                 */
                status = FrameQBufMgr_E_INVALID_BUFINTERFACETYPE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_open",
                                     status,
                                     "Unsupported buf interface type.");
            }
            else {
                /* Allocate memory for the handle */
                handle = (FrameQBufMgr_Handle) Memory_calloc (NULL,
                                                   sizeof (FrameQBufMgr_Object),
                                                   0,
                                                   NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    /*! @retval NULL Memory allocation failed for handle */
                    status = FrameQBufMgr_E_ALLOC_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgr_create",
                                         status,
                                         "Memory allocation failed for handle!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj                 = handle;
                    /* Open handle to the implementation specific handle
                     * and plug in to the top level FrameQ object.
                     */
                    switch (ctrlInterfaceType) {
                        case FrameQBufMgr_INTERFACE_SHAREDMEM:
                        {
                            shMemOpenParams = (FrameQBufMgr_ShMem_OpenParams *)
                                                                 instOpenParams;
                            cmdArgs.args.open.interfaceType =
                                               FrameQBufMgr_INTERFACE_SHAREDMEM;
                            cmdArgs.args.open.openParams = (Ptr)Memory_calloc(
                                        NULL,
                                        sizeof(FrameQBufMgr_ShMem_OpenParams),
                                        0,
                                        NULL);
                            Memory_copy(cmdArgs.args.open.openParams,
                                       shMemOpenParams,
                                       sizeof(FrameQBufMgr_ShMem_OpenParams));

                            if  (  shMemOpenParams->commonOpenParams.sharedAddr
                                 != NULL) {
                                /* Convert user space shared addr to shared region address */
                                index = SharedRegion_getId(
                                  shMemOpenParams->commonOpenParams.sharedAddr);

              ((FrameQBufMgr_ShMem_OpenParams *) cmdArgs.args.open.openParams)->
                  commonOpenParams.sharedAddr = (Ptr)SharedRegion_getSRPtr(
                                                shMemOpenParams->
                                                    commonOpenParams.sharedAddr,
                                                index);
                     GT_assert (curTrace,
                            (SharedRegion_SRPtr)
                            (((FrameQBufMgr_ShMem_OpenParams *) cmdArgs.args.open.openParams)
                            ->commonOpenParams.sharedAddr) != SharedRegion_INVALIDSRPTR);
                            }
                            if (shMemOpenParams->commonOpenParams.name != NULL) {
                                cmdArgs.args.open.nameLen =
                                String_len (shMemOpenParams->commonOpenParams.name) + 1;
                            }
                            else {
                                cmdArgs.args.open.nameLen = 0;
                            }


                            /* Convert user space virtual address to shared region address */
                            if (shMemOpenParams->commonOpenParams.sharedAddr !=NULL) {
                                index = SharedRegion_getId (
                                  shMemOpenParams->commonOpenParams.sharedAddr);
                                ((FrameQBufMgr_ShMem_OpenParams *)
                                cmdArgs.args.open.openParams)->commonOpenParams.sharedAddr = (Ptr)
                                 SharedRegion_getSRPtr(
                                    shMemOpenParams->commonOpenParams.sharedAddr,
                                    index);
                                GT_assert (curTrace,
                                    (SharedRegion_SRPtr)
                                    (((FrameQBufMgr_ShMem_OpenParams *)cmdArgs.args.open.openParams)
                                    ->commonOpenParams.sharedAddr) != SharedRegion_INVALIDSRPTR);
                            }
                            status = FrameQBufMgrDrv_ioctl (
                                                          CMD_FRAMEQBUFMGR_OPEN,
                                                          &cmdArgs);
                            if (status < 0) {
                                GT_setFailureReason (curTrace,
                                                     GT_4CLASS,
                                                     "FrameQBufMgr_open",
                                                     status,
                                                     "API (through IOCTL) failed on kernel-side!");
                            }

                            if (cmdArgs.args.open.openParams != NULL) {
                                Memory_free(
                                        NULL,
                                        cmdArgs.args.open.openParams,
                                        sizeof(FrameQBufMgr_ShMem_OpenParams));
                            }
                        }
                        break;

                        default:
                            /* NameServer entry does not contain the
                             * implementation type.
                             */
                            status = FrameQBufMgr_E_INVALID_INTERFACE;
                    }

                    if  (status >= 0) {
                        ClientNotifyMgr_Params_init(&clientMgrParams);
                         /* Open the ClientNotifyMgr on user side */
                        clientMgrParams.sharedAddr = SharedRegion_getPtr(
                                     cmdArgs.args.open.cliNotifyMgrSharedMem);
                        sharedAddr     = SharedRegion_getPtr
                                           (cmdArgs.args.open.cliGateSharedMem);
                        tmpStatus = GateMP_openByAddr(sharedAddr, &gateHandle);
                      if ((tmpStatus < 0) || (gateHandle == NULL)) {
                            /*! @retval FrameQBufMgr_E_OPEN_GATEPETERSON Failed to open
                             * the gate peterson
                             */
// TBD                        status = FrameQBufMgr_E_OPEN_GATE;
//                            GT_setFailureReason (
//                                          curTrace,
//                                          GT_4CLASS,
//                                          "_FrameQ_create",
//                                          FrameQBufMgr_E_OPEN_GATE,
//                                          "Failed to open the gate peterson !");
                        }
                        else {
                            /* Open  the already created clientNotifyMgr
                             * instance
                             */
                            clientMgrParams.name = (Ptr)
                                              openParams->commonOpenParams.name;

                            /*Open the client notify Mgr instace*/
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
                                /*! @retval
                                 * FrameQBufMgr_E_FAIL_CLIEN0TIFYMGR_OPEN
                                 * Failed to open the clientNotifyMgr instance.
                                 */
                                status =
                                       FrameQBufMgr_E_CLIENTN0TIFYMGR_OPEN;
                                GT_setFailureReason (
                                               curTrace,
                                               GT_4CLASS,
                                              "FrameQBufMgr_open",
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
                    obj->instId         = cmdArgs.args.open.instId;
                    obj->ctrlInterfaceType  = ctrlInterfaceType;
                    obj->objType        = FrameQBufMgr_DYNAMIC_OPEN;
                    obj->clientNotifyMgrGate = (Ptr)gateHandle;
                    obj->clientNotifyMgrHandle = (Ptr)cliMgrHandle;
                    obj->ctrlStructCacheFlag = ctrlStructCacheFlag;

                    *handlePtr         = handle;
                    /* Put in the local list */
                    // key0 = IGateProvider_enter (FrameQBufMgr_module->listLock);
                    List_elemClear (&handle->listElem);
                    List_put ((List_Handle) &(FrameQBufMgr_module->objList),
                               &(handle->listElem));
                    //IGateProvider_leave (FrameQBufMgr_module->listLock, key0);
                }
                else {
                    *handlePtr         = NULL;
                    if (handle != NULL) {
                        Memory_free (NULL,
                                     handle,
                                     sizeof (FrameQBufMgr_Object));
                        handle = NULL;
                    }

                }
            } /*if( !FrameQBufMgr_isSupportedInterface( */
        }   /* if (status >= 0)*/
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_open", status);

    /*! @retval FrameQBufMgr_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief  API to close the previousely opened FrameQBufMgr client.
 *
 * @param  Handle to previously opened instance.
 * @sa     FrameQBufMgr_create, FrameQBufMgr_delete, FrameQBufMgr_open
 */
Int32
FrameQBufMgr_close (FrameQBufMgr_Handle * handlePtr)
{
    Int32                      status = FrameQBufMgr_S_SUCCESS;
//    FrameQBufMgrDrv_CmdArgs    cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (*handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_close",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handlePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_close",
                             status,
                             "handlePtr passed is null!");
    }
    else if (*handlePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG *handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_close",
                             status,
                             "*handlePtr passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
           status = FrameQBufMgr_delete (handlePtr);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_close", status);

    /*! @retval FrameQBufMgr_S_SUCCESS Operation successful */
    return (status);
}

/*
 *  @brief  Function to allocate frame.
 *
 *  @param  handle  FrameQBugMgr instance handle.
 *  @param  buf     Location to receive the allocated frame.
 *
 */
Int32
FrameQBufMgr_alloc(FrameQBufMgr_Handle handle,
                   FrameQBufMgr_Frame  *framePtr)
{
    Int32                       status        = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_FrameBufInfo   *frameBufInfo;
    UInt32                      j;
    Ptr                         addr;
    Ptr                         virtPtr;
    FrameQBufMgr_Object         *obj ;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "FrameQBufMgr_alloc", handle, framePtr);

    GT_assert ( curTrace, (NULL != handle));
    GT_assert ( curTrace, (NULL != framePtr));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_close",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_alloc",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG buf passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_alloc",
                             status,
                             "framePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;

        cmdArgs.args.alloc.handle         = handle->knlObject;
        cmdArgs.args.alloc.frame          = NULL;

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_ALLOC,
                                        &cmdArgs);
        if (status < 0) {
            *framePtr = NULL;
        }
        else {
            /* Convert Frame and frame buffer address in frame to user virtual
             * space
             */
            *framePtr = Memory_translate((Ptr)cmdArgs.args.alloc.frame,
                                          Memory_XltFlags_Phys2Virt);

            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
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

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_alloc", status);

    /*! @retval FrameQBufMgr_S_SUCCESS if buffer is allocated successfully */
    return (status);
}

/*!
 *  @brief  Function to allocate multiple frames from the FrameQbufMgr instance.
 *
 *  @param  handle   Instance handle.
 *  @param  framePtr Array to receive pointers to allocated frames.
 *  @param  freeQId  Array of free frame pool nos from which API needs to
                     allocate frames actual size of that is allocated.
 *  @param  numFrames Number of frames that needs to be allocated. When API
 *                    returns it holds the actual number of frames allocated.
 *
 */
Int32
FrameQBufMgr_allocv (FrameQBufMgr_Handle  handle ,
                     FrameQBufMgr_Frame   framePtr[],
                     UInt32                freeQId[],
                     UInt8                *numFrames)
{
    Int32                       status        = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_FrameBufInfo   *frameBufInfo;
    UInt32                      j;
    UInt32                      i;
    Ptr                         addr;
    Ptr                         virtPtr;
    FrameQBufMgr_Object         *obj ;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    UInt32  _tmp_aFrames[FrameQ_MAX_FRAMESINVAPI];

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_allocv",
               handle,
               framePtr,
               freeQId,
               numFrames);

    GT_assert (curTrace,(handle != NULL));
    GT_assert (curTrace,(framePtr != NULL));
    GT_assert (curTrace,(freeQId != NULL));
    GT_assert (curTrace,(numFrames != NULL));
    GT_assert (curTrace, (*numFrames <= FrameQ_MAX_FRAMESINVAPI));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_allocv",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_allocv",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG buf passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_allocv",
                             status,
                             "framePtr passed is NULL!");
    }
    else if (*numFrames == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDARG *numFrames is zero */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_allocv",
                             status,
                             "*numFrames passed is zero!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
        GT_assert (curTrace, (obj != NULL));

        cmdArgs.args.allocv.handle      = handle->knlObject;
        cmdArgs.args.allocv.aframePtr   = _tmp_aFrames;
        cmdArgs.args.allocv.afreeQId    = freeQId;
        cmdArgs.args.allocv.numFrames   = *numFrames;

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_ALLOCV, &cmdArgs);
        if (status < 0) {
            for (i = 0; i < *numFrames; i++) {
                framePtr[i] = NULL;
            }
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_allocv",
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
                                          (Ptr)cmdArgs.args.allocv.aframePtr[i],
                                          Memory_XltFlags_Phys2Virt);

                frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
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
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_allocv", status);

    /*! @retval FrameQBufMgr_S_SUCCESS if buffer is allocated successfully */
    return (status);
}

/*!
 *  @brief  Function to free frame.
 *
 *          This API picks up the freeFrmQueueNo from the frame header and
 *          consider  that frame pool to free the given frame.
 *
 *  @param  handle  FrameQBugMgr instance handle.
 *  @param  frame   Frame to be freed.
 *
 */
Int32
FrameQBufMgr_free (FrameQBufMgr_Handle  handle,
                   FrameQBufMgr_Frame   frame)
{
    Int32                       status        = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_FrameBufInfo   *frameBufInfo;
    UInt32                      j;
    Ptr                         addr;
    Ptr                         phyPtr;
    FrameQBufMgr_Object         *obj;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "FrameQBufMgr_free", handle, frame);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != frame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_free",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is in invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_free",
                             status,
                             "handlePtr passed is null!");
    }
    else if (frame == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_free",
                             status,
                             "frame passed is NULL!");
    }
    else {
#endif
        obj = handle;
        GT_assert (curTrace, (obj != NULL));

        cmdArgs.args.free.handle  = handle->knlObject;

        /* Convert all frame buffer address to physical */
        frameBufInfo = (FrameQBufMgr_FrameBufInfo *)&(frame->frameBufInfo[0]);
        for(j = 0; j < frame->numFrameBuffers; j++) {
            addr = (Ptr)frameBufInfo[j].bufPtr;
            phyPtr = Memory_translate(addr, Memory_XltFlags_Virt2Phys);
            GT_assert(curTrace,(phyPtr != NULL));
            frameBufInfo[j].bufPtr = (UInt32)phyPtr;
        }

        cmdArgs.args.free.frame   = Memory_translate(
                                                    frame,
                                                    Memory_XltFlags_Virt2Phys);

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_FREE, &cmdArgs);

#if !defined (SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_ShMem_free",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }

    }
#endif

     GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_free", status);

     /*! < @retval FrameQBufMgr_S_SUCCESS if buffer is freed successfully.
      */
     return (status);
}

/*!
 *  @brief  Function to free multiple frames.
 *
 *          Function to free multiple frames  to the  FrameQBufMgr.
 *          Notification to other clients will be generated once all the frames
 *          are freed.
 *
 *  @param  handle      FrameQBugMgr instance handle.
 *  @param  framePtr    Array of frames to be freed.
 *  @param  numFrames   Number of frames to be freed.
 */
Int32
FrameQBufMgr_freev (FrameQBufMgr_Handle  handle,
                    FrameQBufMgr_Frame   framePtr[],
                    UInt32               numFrames)
{
    Int32                       status        = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_FrameBufInfo   *frameBufInfo;
    UInt32                      i;
    UInt32                      j;
    Ptr                         addr;
    Ptr                         phyPtr;
    FrameQBufMgr_Object         *obj;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;

    GT_3trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_freev",
               handle,
               framePtr,
               numFrames);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (framePtr != NULL));
    GT_assert (curTrace, (numFrames != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_freev",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is in invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_freev",
                             status,
                             "handle passed is NULL!");
    }
    else if (framePtr == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG frame passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_freev",
                             status,
                             "framePtr passed is NULL!");
    }
    else if (numFrames == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDARG numFrames passed is zero */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_freev",
                             status,
                             "numFrames passed is zero!");
    }
    else {
#endif
        obj = handle;

        cmdArgs.args.freev.handle    = handle->knlObject;
        cmdArgs.args.freev.aframePtr =   Memory_alloc(NULL,
                                                     numFrames * sizeof(UInt32),
                                                     0,
                                                     NULL);
        cmdArgs.args.freev.numFrames = numFrames;
        for (i = 0; i < numFrames;i++) {
            /* Convert all frame buffer address to physical */
            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                                &(framePtr[i]->frameBufInfo[0]);
            for(j = 0; j < framePtr[i]->numFrameBuffers; j++) {
                addr = (Ptr)frameBufInfo[j].bufPtr;
                phyPtr = Memory_translate(addr, Memory_XltFlags_Virt2Phys);
                GT_assert(curTrace,(phyPtr != NULL));
                frameBufInfo[j].bufPtr = (UInt32)phyPtr;
            }
            cmdArgs.args.freev.aframePtr[i]   = (UInt32)Memory_translate(
                                                     (Ptr)framePtr[i],
                                                     Memory_XltFlags_Virt2Phys);
        }
        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_FREE, &cmdArgs);

#if !defined (SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_freev",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
#endif
        if (cmdArgs.args.freev.aframePtr != NULL)  {
            Memory_free (NULL,
                         cmdArgs.args.freev.aframePtr,
                         numFrames * sizeof(UInt32));
        }

#if !defined (SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_freev", status);

    /*! < @retval FrameQBufMgr_S_SUCCESS if buffer is freed successfully.
     */
    return (status);
}

/*!
 * @brief   API to duplicate the given frame i.e it increments the corresponding
 *          frames 's reference count.It internally allocates  headers for the
 *          duped frames.It fails if it is not able to dup all the frames.
 *  @param  frame           Frame to duplicated.
 *  @param  dupedFramePtr   Location to receive duplicated frames.
 *  @param  numDupedFrames  Number of duplicated frames required.
 */
Int32
FrameQBufMgr_dup (FrameQBufMgr_Handle  handle,
                  FrameQBufMgr_Frame   frame,
                  FrameQBufMgr_Frame   dupedFramePtr[],
                  UInt32               numDupedFrames)
{
    Int32                       status        = FrameQBufMgr_S_SUCCESS;
    UInt32                      i;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    FrameQBufMgr_Frame          _tmp_dupedFramePtr[FrameQ_MAX_FRAMESINVAPI];

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_dup",
               handle,
               frame,
               dupedFramePtr,
               numDupedFrames);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frame  != NULL));
    GT_assert (curTrace, (dupedFramePtr != NULL));
    GT_assert (curTrace, (numDupedFrames > 0));
    GT_assert (curTrace, (numDupedFrames <= FrameQ_MAX_FRAMESINVAPI));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dup",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is in invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dup",
                             status,
                             "handlePtr passed is null!");
    }
    else if (handle->knlObject == NULL) {
         /*! @retval FrameQBufMgr_E_INVALIDARG The provided handle does not have
         * permission to do put. The handle is not a reader handle.
         */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dup",
                             status,
                             "handle->knlObject is null.!");
    }
    else {
#endif
        cmdArgs.args.dup.handle  = handle->knlObject;
        /* Convert all frame buffer address to physical */
        /*TODO: Check if frame buffer  address need to be converted to physical
         */
        cmdArgs.args.dup.frame   = Memory_translate((Ptr)frame,
                                                    Memory_XltFlags_Virt2Phys);

        cmdArgs.args.dup.adupedFramePtr = _tmp_dupedFramePtr;
        cmdArgs.args.dup.numDupedFrames = numDupedFrames;

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_DUP, &cmdArgs);

#if !defined (SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            /* Reset numDupedFramePtr*/
            for (i = 0; i < numDupedFrames; i++) {
                dupedFramePtr[i] = NULL;
            }
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_dup",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif
            for (i = 0; i < numDupedFrames;i++) {
                /* Convert allduped frames  address to virtual */
                dupedFramePtr[i]   = Memory_translate(
                                       (Ptr) cmdArgs.args.dup.adupedFramePtr[i],
                                       Memory_XltFlags_Phys2Virt);
            }
#if !defined (SYSLINK_BUILD_OPTIMIZE)
        }
#endif
#if !defined (SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_dup", status);

    /*! < @retval FrameQBufMgr_S_SUCCESS if buffer duplicated successfully.
     */
    return (status);
}

/*!
 * @brief   API to duplicate the given frames i.e it increments the
 *          corresponding frames 's reference count.It internally allocates
 *          headers for the duped frames.API fails if it is not able to dup all
 *          the frames.
 * @param  framePtr         Array of frames to duplicated.
 * @param  dupedFramePtr    Location to receive duplicated frames.
 * @param  numDupedFrames   Number of duplicated frames required for each  given
 *                          frame.
 * @param  numFrames        Number of original frames that needs to be
 *                          duplicated.
 */
Int32
FrameQBufMgr_dupv (FrameQBufMgr_Handle   handle,
                   FrameQBufMgr_Frame    framePtr[],
                   FrameQBufMgr_Frame  **dupedFramePtr,
                   UInt32                numDupedFrames,
                   UInt32                numFrames)
{

    Int32                       status        = FrameQBufMgr_S_SUCCESS;
    UInt32                      i;
    UInt32                      j;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    FrameQBufMgr_Object         *obj;

    GT_5trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_dupv",
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

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is in invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_dupv",
                             status,
                             "handlePtr passed is null!");
    }
    else {
#endif
        obj = handle;
        cmdArgs.args.dupv.handle    = handle->knlObject;
        /* Convert all frame buffer address to physical */
        /*TODO: Check if frame buffer  address need to be converted to physical
         */
        cmdArgs.args.dupv.framePtr  = Memory_alloc(NULL,
                                                   numFrames * sizeof(UInt32),
                                                   0,
                                                   NULL);
        for (i = 0; i < numFrames; i++) {
            cmdArgs.args.dupv.framePtr[i] = Memory_translate(
                                                   (Ptr)framePtr[i],
                                                    Memory_XltFlags_Virt2Phys);
        }
        cmdArgs.args.dupv.dupedFramePtr     = dupedFramePtr;
        cmdArgs.args.dupv.numDupedFrames    = numDupedFrames;
        cmdArgs.args.dupv.numFrames         = numFrames;

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_DUPV,
                                        &cmdArgs);

#if !defined (SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_dupv",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif
            for (i = 0; i < numFrames; i++) {
                for (j = 0; j < numDupedFrames;j++) {
                    /* Convert allduped frames  address to virtual */
                    dupedFramePtr[i][j]   = Memory_translate(
                                   (Ptr) cmdArgs.args.dupv.dupedFramePtr[i][j],
                                    Memory_XltFlags_Phys2Virt);
                }
            }
#if !defined (SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif

    if (cmdArgs.args.dupv.framePtr != NULL)  {
        Memory_free (NULL,
                     cmdArgs.args.dupv.framePtr,
                     numFrames * sizeof(UInt32));
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_dupv", status);

    /*! < @retval FrameQBufMgr_S_SUCCESS if buffer duplicated successfully.
     */
    return (status);
}

/*!
 *  @brief  Function to Register notification with the instance.
 *
 *          It internally registers notification with the individual free queues
 *          (free frame pools). If alloc on free pool 0 failed,Notification
 *          will be generated if free frames in that pool becomes more than
 *          watermark(all the free frames in that pool).
 *
 *  @param  handle          FrameQBugMgr instance handle.
 *  @param  notifyParams    notification registration parameters.
 */
Int32
FrameQBufMgr_registerNotifier (FrameQBufMgr_Handle        handle,
                               FrameQBufMgr_NotifyParams *notifyParams)
{
    Int32                       status  = FrameQBufMgr_S_SUCCESS;
    ClientNotifyMgr_registerNoitifyParams  regParams;
    UInt32                      notifyId;
    FrameQBufMgr_Object         *obj;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;

    GT_2trace (curTrace,
               GT_ENTER,
              "FrameQBufMgr_registerNotifier",
               handle,
               notifyParams);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != notifyParams));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_registerNotifier",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is in invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_registerNotifier",
                             status,
                             "handlePtr passed is null!");
    }
    else {
#endif
        obj = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->isRegistered != TRUE) {
#endif
            /* Register call back function with the clientNotifyMgr.Pass the
             * cbClientHandle which is passed by the caller of this API to
             * clientNotifyMgr. so that it will be passed as first arg to the
             * callback function during notification.
             */
            regParams.clientHandle   = notifyParams->cbClientHandle;
            regParams.procId         = MultiProc_self();
            regParams.notifyType     = notifyParams->notifyType;
            regParams.fxnPtr         = (ClientNotifyMgr_FnCbck)
                                                           (notifyParams->cbFxnPtr);
            regParams.cbContext      = notifyParams->cbContext;
            regParams.watermarkCond1 = notifyParams->watermark;
            regParams.watermarkCond2 = -1;

            status = ClientNotifyMgr_registerClient(obj->clientNotifyMgrHandle,
                                                    &regParams,
                                                    &notifyId);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_ShMem_registerNotifier",
                                     status,
                                     "ClientNotifyMgr_registerClient failed.!");
            }
            else {
                obj->notifyId     = notifyId;
                obj->isRegistered = TRUE;
                cmdArgs.args.setNotifyId.handle   = handle->knlObject;
                cmdArgs.args.setNotifyId.notifyId = notifyId;
                status = FrameQBufMgrDrv_ioctl (
                                                CMD_FRAMEQBUFMGR_SET_NOTIFYID,
                                                &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgr_registerNotifier",
                                         status,
                                         "API (through IOCTL) failed on kernel-side"
                                         "in setting notifyId in kernel obbject!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined (SYSLINK_BUILD_OPTIMIZE)
        }
        else {
            status = FrameQBufMgr_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_registerNotifier",
                                 status,
                                 "Error:Callback is already registered.!");
        }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_registerNotifier", status);

    /*! @retval  FRAMEQ_SUCCESS  Successfully registered callback fucntion*/
    return (status) ;
}


/*!
 * @brief Function to unregister the notification call back function.
 *
 * @param  handle Handle to the FrameQBufMgr instance
 */
Int32
FrameQBufMgr_unregisterNotifier (FrameQBufMgr_Handle      handle)
{
    Int32                       status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Object         *obj;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;

    GT_1trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_unregisterNotifier",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        /*! @retval FrameQBufMgr_E_INVALIDSTATE if Module was not initialized */
        status = FrameQBufMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_unregisterNotifier",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handle passed is NULL */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_unregisterNotifier",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->isRegistered == TRUE) {
#endif

            status = ClientNotifyMgr_unregisterClient (obj->clientNotifyMgrHandle,
                                                       obj->notifyId);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgr_unregisterNotifier",
                                     status,
                                     "ClientNotifyMgr_unregisterClient failed.!");
            }
            else {
                obj->notifyId     = -1;
                obj->isRegistered = FALSE;
                cmdArgs.args.resetNotifyId.handle   = handle->knlObject;
                cmdArgs.args.resetNotifyId.notifyId = obj->notifyId;
                status = FrameQBufMgrDrv_ioctl (
                                              CMD_FRAMEQBUFMGR_RESET_NOTIFYID,
                                              &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgr_unregisterNotifier",
                                         status,
                                         "API (through IOCTL) failed on kernel-side"
                                         "in resetting notifyId in kernel object!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
        else {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_unregisterNotifier",
                                 status,
                                 "Callback is not registered So can not unregister!");
        }
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "FrameQBufMgr_unregisterNotifier",
               status);

    /*! @retval FrameQBufMgr_S_SUCCESS if callback function unregistered
     *  successfully.
     */
    return (status);
}

/*
 *  @brief  Function to dynamically add frame to the instance.
 *
 *  @param  handle  FrameQBugMgr instance handle.
 *  @param  freeQId framPoolNo to which this frame needs to be added.
 *  @param  framPtr frame to add.
 */
Ptr
FrameQBufMgr_add(FrameQBufMgr_Handle  handle,
                 UInt8                freeQId)
{
    Ptr                           ptr = NULL;
//    FrameQBufMgrDrv_CmdArgs       cmdArgs;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_add",
               handle,
               freeQId);

    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_add",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_close",
                             FrameQBufMgr_E_INVALIDARG,
                             "handlePtr passed is null!");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Not implemented */
        ptr = NULL;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_add", ptr);

   /*! < @retval frame pointer if frame added successfully */
    return (ptr);
}

/*
 *  @brief  Function to dynamically remove frame to the instance.
 *
 *  @param  handle  FrameQBugMgr instance handle.
 *  @param  freeQId framPoolNo to which this frame needs to be removed.
 *  @param  framPtr frame to remove.
 */
Int32
FrameQBufMgr_remove(FrameQBufMgr_Handle  handle,
                    UInt8                freeQId,
                    FrameQBufMgr_Frame   framePtr)
{
    Int32                         status       = FrameQBufMgr_S_SUCCESS;

    GT_3trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_remove",
               handle,
               freeQId,
               framePtr);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (NULL != framePtr));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_remove",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        /*! @retval FrameQBufMgr_E_INVALIDARG handlePtr passed is null */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_remove",
                             status,
                             "handlePtr passed is null!");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = FrameQBufMgr_E_NOTIMPLEMENTED;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_remove", status);

   /*! < @retval FrameQBufMgr_S_SUCCESS if frame removed  successfully */
    return (status);
}

/*!
 *  @brief  Write back the contents  of  frame header.
 *
 *          API treates the passed buffer as frame Header.
 *
 *  @param  handle      Handle to the instance.
 *  @param  headerBuf   frame header to be written back.
 */
Int32
FrameQBufMgr_writeBackHeaderBuf(FrameQBufMgr_Handle  handle,
                                Ptr                  headerBuf)
{
    Int32                 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Object   *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_writeBackHeaderBuf",
               handle,
               headerBuf);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (headerBuf  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        if (obj->frmHdrBufCacheFlag == TRUE) {
            Cache_wbInv(headerBuf,
                        ((FrameQBufMgr_Frame)headerBuf)->headerSize,
                        Cache_Type_ALL,
                        TRUE);
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace,
              GT_LEAVE,
              "FrameQBufMgr_writeBackHeaderBuf",
              status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successfully write back doen for frame
    * header
    */
   return (status);
}

/*!
 *  @brief  Write back the contents  of  frame header.
 *
 *          API treates the passed buffer as frame Header.
 *
 *  @param  handle      Handle to the instance.
 *  @param  headerBuf   frame header to be written back.
 */
Int32
FrameQBufMgr_invalidateHeaderBuf(FrameQBufMgr_Handle  handle,
                                 Ptr                  headerBuf)
{
    Int32                 status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Object   *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_invalidateHeaderBuf",
               handle,
               headerBuf);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (headerBuf  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        if (obj->frmHdrBufCacheFlag == TRUE) {
            /* Do invalidation first one cache line */
            Cache_wbInv(headerBuf,
                        FrameQBufMgr_CACHE_LINESIZE,
                        Cache_Type_ALL,
                        TRUE);
            Cache_wbInv(headerBuf,
                        ((FrameQBufMgr_Frame)headerBuf)->headerSize,
                        Cache_Type_ALL,
                        TRUE);
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace,
              GT_LEAVE,
              "FrameQBufMgr_invalidateHeaderBuf",
              status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successful invalidation done for frame
    * header
    */
   return (status);
}

/*!
 *  @brief  Write back the contents  of frame buffer.
 *
 *          Passed buffer is treated as frame  buffer.
 *          bufNumInFrame denotes  buffer number in the frame.This is  to
 *          identify the cache flags and cpu access flags for the  buffer.
 *
 *  @param  handle      Handle to the instance.
 *  @param  frameBuf    frame buffer to be written back.
 *  @param  size        Size of buffer.
 *  @param  bufIndexInFrame index of the frame buffer in the frame that this
 *          frame buffer belongs to.
 */
Int32
FrameQBufMgr_writeBackFrameBuf(FrameQBufMgr_Handle  handle,
                               Ptr                  frameBuf,
                               UInt32               size,
                               UInt8                bufIndexInFrame)
{
    Int32                   status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Object     *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_writeBackFrameBuf",
               handle,
               frameBuf,
               size,
               bufIndexInFrame);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frameBuf  != NULL));
    GT_assert (curTrace, ( 8 > bufIndexInFrame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        /* Find out the buffer type from the flags */
        if (   (obj->frmBufCacheFlag[bufIndexInFrame] == TRUE)
            && (obj->bufCpuAccessFlag[bufIndexInFrame] == TRUE)) {
            Cache_wbInv(frameBuf,
                        size,
                        Cache_Type_ALL,
                        TRUE);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace,
              GT_LEAVE,
              "FrameQBufMgr_writeBackFrameBuf",
              status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successfully write back done for frame
    * buffer.
    */
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
Int32
FrameQBufMgr_invalidateFrameBuf(FrameQBufMgr_Handle  handle,
                                Ptr                  frameBuf,
                                UInt32               size,
                                UInt8                bufIndexInFrame)
{
    Int32                   status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Object     *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_invalidateFrameBuf",
               handle,
               frameBuf,
               size,
               bufIndexInFrame);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frameBuf  != NULL));
    GT_assert (curTrace, ( 8 > bufIndexInFrame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        /* Find out the buffer type from the flags */
        if (   (obj->frmBufCacheFlag[bufIndexInFrame] == TRUE)
            && (obj->bufCpuAccessFlag[bufIndexInFrame] == TRUE)) {
            Cache_inv(frameBuf,
                      size,
                      Cache_Type_ALL,
                      TRUE);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace,
              GT_LEAVE,
              "FrameQBufMgr_invalidateFrameBuf",
              status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successfully write back done for frame
    * buffer.
    */
   return (status);
}
/*!
 *  @brief  Write back the contents  of a frame including frame buffers..
 *
 *          NOTE: frame address and frame buffer address in the frame should be in
 *          virtual format.
 *  @param  handle  Handle to the instance.
 *  @param  frame   Frame to be writeback.
 */
Int32
FrameQBufMgr_writeBack(FrameQBufMgr_Handle  handle,
                       FrameQBufMgr_Frame   frame)
{
    Int32                       status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_FrameBufInfo   *frameBufInfo;
    Ptr                         addr;
    UInt32                      i;
    FrameQBufMgr_Object         *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_writeBack",
               handle,
               frame);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frame  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        GT_assert (curTrace, obj != NULL);
        frameBufInfo = (FrameQBufMgr_FrameBufInfo*) &(frame->frameBufInfo[0]);

        for (i = 0; i < frame->numFrameBuffers; i++) {
            /* Write  back the buffer if cache flag  and cpu access flag is
             * enabled.
             */
            if (   (obj->frmBufCacheFlag[i] == TRUE)
                && (obj->bufCpuAccessFlag[i] == TRUE)) {

                /* Write back only valid contents in frame buffer */
                addr = (Ptr)(   (UInt32)(frameBufInfo[i].bufPtr)
                              + (UInt32)(frameBufInfo[i].startOffset));
                Cache_wbInv(addr,
                            frameBufInfo[i].validSize,
                            Cache_Type_ALL,
                            TRUE);
//                frameBufInfo[i].bufPtr = (UInt32)MemMgr_translate (
//                                                   obj->memMgrAllocater_hdrBufs,
//                                                   (Ptr)frameBufInfo[i].bufPtr,
//                                                   MemMgr_AddrType_Virtual,
//                                                   MemMgr_AddrType_Portable);
            }
        }

        /* Write  back frame  if cache flag  is enabled.
         */
        if (obj->frmHdrBufCacheFlag == TRUE) {
            Cache_wbInv(frame,
                        frame->headerSize,
                        Cache_Type_ALL,
                        TRUE);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_writeBack", status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successfully written back */
   return (status);
}

/*!
 *  @brief  Invalidate the contents  of a frame including frame buffers.
 *
 *          frame  address in virtual format.
 *          It converts the internal portable frame buffer pointers to virtual
 *          format in the frame.
 *  @param  handle  Handle to the instance.
 *  @param  frame   Frame to be writeback.
 */
Int32
FrameQBufMgr_invalidate(FrameQBufMgr_Handle  handle,
                        FrameQBufMgr_Frame   frame)
{
    Int32                           status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_FrameBufInfo       *frameBufInfo;
    Ptr                             addr;
    UInt32                          i;
    FrameQBufMgr_Object             *obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_invalidate",
               handle,
               frame);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (frame  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        frameBufInfo = (FrameQBufMgr_FrameBufInfo*) &(frame->frameBufInfo[0]);

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
            /* Write  back the buffer if cache flag  and cpu access flag is
             * enabled.
             */
            if (   (obj->frmBufCacheFlag[i] == TRUE)
                && (obj->bufCpuAccessFlag[i] == TRUE)) {
//                frameBufInfo[i].bufPtr = (UInt32)MemMgr_translate (
//                                         obj->memMgrAllocater_hdrBufs,
//                                        (Ptr)frameBufInfo[i].bufPtr,
//                                         MemMgr_AddrType_Portable,
//                                         MemMgr_AddrType_Virtual);

                /* Write back only valid contents in frame buffer */
                addr = (Ptr)(   (UInt32)(frameBufInfo[i].bufPtr)
                              + (UInt32)(frameBufInfo[i].startOffset));
                Cache_inv(addr,
                          frameBufInfo[i].validSize,
                          Cache_Type_ALL,
                          TRUE);
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_invalidate", status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successfully invalidates frame */
   return (status);
}

/*!
 *  @brief  Function to translate address between different types.
 *
 *          see also FrameQBufMgr_AddrType.
 *
 *  @param  handle      Handle to the instance.
 *  @param  dstAddr     Destination address.
 *  @param  dstAddrType Destination address type.
 *  @param  srcAddr     Source address.
 *  @param  srcAddrType Source address type.
 *  @param  bufType index  Source address specified is frame header or frame
 *          buffer.
 */
Int32
FrameQBufMgr_translateAddr (FrameQBufMgr_Handle      handle,
                            Ptr *                    dstAddr,
                            FrameQBufMgr_AddrType    dstAddrType,
                            Ptr                      srcAddr,
                            FrameQBufMgr_AddrType    srcAddrType,
                            FrameQBufMgr_BufType     bufType)
{
    Int32                   status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Object     *obj;
    FrameQBufMgrDrv_CmdArgs cmdArgs;

    GT_5trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_translateAddr",
               dstAddr,
               dstAddrType,
               srcAddr,
               srcAddrType,
               bufType);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (dstAddr  != NULL));
    GT_assert (curTrace, (srcAddr  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
        obj = handle;
        GT_assert (curTrace, obj != NULL);
        cmdArgs.args.translate.handle  = handle->knlObject;
        /* Convert all frame buffer address to physical */
        /*TODO: Check if frame buffer  address need to be converted to physical
         */
        if (srcAddrType == FrameQBufMgr_AddrType_Virtual) {
            cmdArgs.args.translate.srcAddr      = Memory_translate((Ptr)srcAddr,
                                                        Memory_XltFlags_Virt2Phys);
        }
        else {
            /* srcAddr in portable format defined by FrameQbufMgr */
            cmdArgs.args.translate.srcAddr      = srcAddr;
        }

        cmdArgs.args.translate.srcAddrType  = srcAddrType;

        cmdArgs.args.translate.dstAddrType  = dstAddrType;

        cmdArgs.args.translate.bufType      = bufType;

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_TRANSLATE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_translateAddr",
                                 status,
                                 "API (through IOCTL) failed on kernel-side"
                                 "in resetting notifyId in kernel obbject!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (dstAddrType == FrameQBufMgr_AddrType_Virtual) {
                *dstAddr =  Memory_translate(
                                       (Ptr)cmdArgs.args.translate.dstAddr,
                                        Memory_XltFlags_Phys2Virt);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace,
              GT_LEAVE,
              "FrameQBufMgr_translateAddr",
              status);

   /*! @retval FrameQBufMgr_S_SUCCESS  if successfully translated the given source
    * address.
    */
   return (status);
}

/*!
 *  @brief  Function to get FrameQbufMgr Id.
 *
 *  @param  handle  Handle to the instance.
 *
 */
UInt32
FrameQBufMgr_getId (FrameQBufMgr_Handle handle)
{
   GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_getId", handle);

   GT_assert (curTrace, (handle != NULL));

   GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_getId", handle->instId);

   /*!< @retval returns  the Id of the instance */
   return (handle->instId);
}


/*!
 *  @brief  Function to get handle from the FrameQBufMgr Id
 *
 *  @param  Id  FrameQBufMgr id.
 *
 */
Ptr
FrameQBufMgr_getHandle (UInt32 id)
{
    Int32                       status = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgr_Handle         handle  = NULL ;
    FrameQBufMgrDrv_CmdArgs     cmdArgs;
    List_Elem                   *elem;
//    IArg                        key;

    GT_1trace (curTrace,
               GT_ENTER,
               "FrameQBufMgr_getHandle",
               id);


    cmdArgs.args.getHandle.instId = id;
    status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_GETHANDLE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getHandle",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        List_traverse (elem, (List_Handle) &FrameQBufMgr_module->objList) {
            if (   ((FrameQBufMgr_Object *)elem)->knlObject
                    == cmdArgs.args.getHandle.handle) {
               // key = IGateProvider_enter (FrameQBufMgr_module->listLock);
                /* Check if we have created the GP or not */
                handle  = (FrameQBufMgr_Object*)elem;
               // IGateProvider_leave (FrameQBufMgr_module->listLock, key);
                break;
            }

        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_getHandle", handle);

    /*! < @retval Valid handle if API is successful.
     *            NULL if API failed to get handle.
     */
    return (handle);
}

/*!
 *  @brief Function to get the kernel object pointer embedded in userspace heap.
 *         Some MemMgr implementations return the kernel object handle.
 *         MemMgrs which do not have kernel object pointer embedded return NULL.
 *
 *  @params handle handle to a FrameQBufMgr instance
 *
 *  @sa
 */
Void *
FrameQBufMgr_getKnlHandle (FrameQBufMgr_Handle handle)
{
    FrameQBufMgr_Object * Obj = (FrameQBufMgr_Object *) handle;
    Ptr           knlHandle = NULL;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_getKnlHandle", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval NULL Handle passed is invalid */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getKnlHandle",
                             FrameQBufMgr_E_INVALIDARG,
                             "Handle passed is invalid!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

           knlHandle = Obj->knlObject;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "FrameQBufMgr_getKnlHandle");

    /*! @retval Kernel-object-handle Operation successfully completed. */
    return (knlHandle);
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
FrameQBufMgr_getNumFreeFrames  (FrameQBufMgr_Handle handle,
                                UInt32* numFreeFrames)
{
    Int32                   status  = FrameQBufMgr_S_SUCCESS;
    FrameQBufMgrDrv_CmdArgs       cmdArgs;
    FrameQBufMgr_Object           *obj;

    GT_2trace (curTrace,
               GT_ENTER,
              "FrameQBufMgr_getNumFreeFrames",
               handle,
               numFreeFrames);

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (NULL != numFreeFrames));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getNumFreeFrames",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQBufMgr_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getNumFreeFrames.handle   = handle->knlObject;
        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_getNumFreeFrames",
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

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_getNumFreeFrames", status);

    /*! @retval  FrameQBufMgr_S_SUCCESS   Successfully queried the number of frames*/
    return (status);
}

/*
 * @brief Function to get the number of free frames available in the plugged in
 *        FrameQbufMgr.
 *
 */
Int32
FrameQBufMgr_getvNumFreeFrames(FrameQBufMgr_Handle     handle,
                               UInt32            numFreeFrames[],
                               UInt8             freeQId[],
                               UInt8             numFreeQids)
{
    Int32                   status  = FrameQBufMgr_S_SUCCESS;
    UInt32                  i;
    FrameQBufMgrDrv_CmdArgs       cmdArgs;
    FrameQBufMgr_Object           *obj;
    UInt32  _tmp_numFreeFrames[FrameQ_MAX_FRAMESINVAPI];


    GT_3trace (curTrace,
               GT_ENTER,
              "FrameQBufMgr_getvNumFreeFrames",
               handle,
               numFreeFrames,
               freeQId);

    GT_assert (curTrace, (numFreeQids <= FrameQ_MAX_FRAMESINVAPI));

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (NULL != numFreeFrames));

    GT_assert (curTrace, (NULL != freeQId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_getvNumFreeFrames",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (FrameQBufMgr_Object*)handle;
        GT_assert (curTrace, (NULL != obj));

        cmdArgs.args.getvNumFreeFrames.handle    = handle->knlObject;
        cmdArgs.args.getvNumFreeFrames.numFreeFrames = _tmp_numFreeFrames;

        cmdArgs.args.getvNumFreeFrames.freeQId  = (Ptr)freeQId;
        cmdArgs.args.getvNumFreeFrames.numFreeQids  = numFreeQids;

        status = FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_getvNumFreeFrames",
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

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgr_getvNumFreeFrames", status);

    /*! @retval  FrameQBufMgr_S_SUCCESS   Successfully queried the number of frames*/
    return (status);
}

/*
 *  ======== FrameQBufMgr_getBaseHeaderSize ========
 *  API to get the base frame heade if number of frame buffers known.
 */
UInt32 FrameQBufMgr_getBaseHeaderSize(UInt8 numFrameBufs)
{
    UInt32 baseHdrSize;

    GT_assert (curTrace, (numFrameBufs <= 8));

    baseHdrSize = (  sizeof(FrameQBufMgr_FrameHeader)
                   + (sizeof(FrameQBufMgr_FrameBufInfo)* (numFrameBufs-1)));

    return (baseHdrSize);
}

/*!
 * @brief API to to find out the specified interface type is supported or not.
 * @param type  interface type.
 */
inline
Int32 FrameQBufMgr_isSupportedInterface(UInt32 type)
{
    Int32  status = TRUE;

    switch (type)
    {
        case FrameQBufMgr_INTERFACE_SHAREDMEM:
            /* Supported interface */
        break;
        default:
            /* unsupported interface type specified */
            status = FALSE;
    }

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
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation for ShMem instance.
 *              NOTE: Only supports returning default params.ie. first arg
 *              handle is ignored.
 *
 *  @param      params  Instance config-params structure.
 */
Void
FrameQBufMgr_ShMem_Params_init (FrameQBufMgr_ShMem_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status  = FrameQBufMgr_S_SUCCESS;
#endif
    FrameQBufMgrDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_ShMem_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_ShMem_Params_init",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_ShMem_Params_init",
                             FrameQBufMgr_E_INVALIDARG,
                             "Argument of type (FrameQBufMgr_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
        /* Supprts only getting default params*/
        cmdArgs.args.ParamsInit.handle = NULL ;


#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_ShMem_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "FrameQBufMgr_ShMem_Params_init");
}

/*********************** Internal functions **********************************/
/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      params  Instance config-params structure.
 */
Void
FrameQBufMgr_Params_init (FrameQBufMgr_Handle   handle,
                          FrameQBufMgr_Params  *params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status  = FrameQBufMgr_S_SUCCESS;
#endif
    FrameQBufMgrDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_Params_init",
                             FrameQBufMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        status = FrameQBufMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_Params_init",
                             status,
                             "Argument of type (FrameQBufMgr_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
        if (handle != NULL) {
            cmdArgs.args.ParamsInit.handle = handle->knlObject;
        }
        else {
            cmdArgs.args.ParamsInit.handle = NULL ;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "FrameQBufMgr_Params_init");
}

/*!
 * @brief   Internal function called by FrameQBufMgr_create function when
 *          openFlag is  FALSE.
 *
 * @param   object FrameQbufMgr handle(uncooked)
 */
static inline
Int32
_FrameQBufMgr_create (FrameQBufMgr_Handle  handle,
                      FrameQBufMgr_Params* params)
{
    Int32                           status     = FrameQBufMgr_S_SUCCESS;
    Ptr                             sharedAddr = NULL;
    UInt32                          cpuAccessFlags = 0;
    FrameQBufMgr_ShMem_Params       *pShMemParams;
    ClientNotifyMgr_Handle          cliMgrHandle;
    GateMP_Handle                   gateHandle;
    ClientNotifyMgr_Params          clientMgrParams;
    UInt16                          index;
    FrameQBufMgrDrv_CmdArgs         cmdArgs;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    FrameQBufMgr_NameServerEntry    entry;
    UInt32                          len;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    FrameQBufMgr_Object             *obj;

    GT_2trace (curTrace, GT_ENTER, "_FrameQBufMgr_create", handle,params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (FrameQBufMgr_module->setupRefCount == 0) {
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
         /*! @retval NULL  Params passed is NULL
          */
         status = FrameQBufMgr_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_FrameQBufMgr_create",
                             status,
                             "Params pointer is NULL!");

    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if name is already exists in the name server */
        if (FrameQBufMgr_module->nameServer != NULL) {
            len = sizeof (FrameQBufMgr_NameServerEntry);
            status = NameServer_get (FrameQBufMgr_module->nameServer,
                                     (String)params->commonCreateParams.name,
                                     &entry,
                                     &len ,
                                     NULL);

            if (status >= 0) {
                /* Already instance with the name exists */
                /*! @retval NULL  Instance is already created and entry exists
                 *  in Nameserver.
                 */
                status = FrameQBufMgr_E_INST_EXISTS;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_FrameQBufMgr_create",
                                     status,
                                     "Instance is already created and entry"
                                     "exists in Nameserver.!");

            }
            else if ((status != NameServer_E_NOTFOUND) && (status < 0)) {
                /* Error happened in NameServer. Pass the error up. */
                /*! @retval NULL  IError happened in NameServer.
                 */
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
                    if (FrameQBufMgr_isSupportedBufInterface(params->commonCreateParams.ctrlInterfaceType,
                                                             pShMemParams->bufInterfaceType)) {
#endif
                        cpuAccessFlags   = pShMemParams->cpuAccessFlags;

                        cmdArgs.args.create.params = Memory_alloc(NULL,
                                                sizeof(FrameQBufMgr_ShMem_Params),
                                                0,
                                                NULL);
                        GT_assert(curTrace, (cmdArgs.args.create.params != NULL));
                        cmdArgs.args.create.interfaceType =
                                                   FrameQBufMgr_INTERFACE_SHAREDMEM;
                        Memory_copy(cmdArgs.args.create.params,
                                   pShMemParams,
                                   sizeof(FrameQBufMgr_ShMem_Params));

                        cmdArgs.args.create.name =
                                              pShMemParams->commonCreateParams.name;
                        if (pShMemParams->commonCreateParams.name != NULL) {
                            cmdArgs.args.create.nameLen =
                                   String_len (pShMemParams->commonCreateParams.name) + 1;
                        }
                        else {
                            cmdArgs.args.create.nameLen = 0;
                        }

#ifdef SYSLINK_BUILDOS_QNX
                        if (pShMemParams->gate != NULL) {
                            ((FrameQBufMgr_ShMem_Params*)
                            cmdArgs.args.create.params)->gate =
                            (GateMP_Handle)GateMP_getSharedAddr(pShMemParams->gate);
                        }
                        else {
                            ((FrameQBufMgr_ShMem_Params*)cmdArgs.args.create.params)->
                                gate = (GateMP_Handle)SharedRegion_INVALIDSRPTR;
                        }
#else
                        if (pShMemParams->gate != NULL) {
                            ((FrameQBufMgr_ShMem_Params*)cmdArgs.args.create.params)->
                                gate =  GateMP_getKnlHandle(pShMemParams->gate);
                        }
                        else {
                            ((FrameQBufMgr_ShMem_Params*)cmdArgs.args.create.params)->
                                gate = NULL;
                        }
#endif

                        /* Convert user space shared addr to shared region address */
                        index = SharedRegion_getId(pShMemParams->sharedAddr);

                        ((FrameQBufMgr_ShMem_Params*)cmdArgs.args.create.params)->
                            sharedAddr = (Ptr)SharedRegion_getSRPtr(
                                               pShMemParams->sharedAddr, index);

                        /* Convert user space sharedAddrHdrBuf to shared
                         * region address
                         */
                        index = SharedRegion_getId(
                                                pShMemParams->sharedAddrHdrBuf);

                        ((FrameQBufMgr_ShMem_Params*)cmdArgs.args.create.params)->
                                sharedAddrHdrBuf =(Ptr) SharedRegion_getSRPtr(
                                             pShMemParams->sharedAddrHdrBuf, index);

                        /* Convert user space sharedAddrFrmBuf to shared
                         * region address
                         */
                        index = SharedRegion_getId(
                                                pShMemParams->sharedAddrFrmBuf);

                        ((FrameQBufMgr_ShMem_Params*)cmdArgs.args.create.params)->
                                sharedAddrFrmBuf = (Ptr)SharedRegion_getSRPtr(
                                            pShMemParams->sharedAddrFrmBuf, index);

                        /* TODO: Get kernel handles for memory allocaters
                         * and pass them down.
                         */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            FrameQBufMgrDrv_ioctl (CMD_FRAMEQBUFMGR_CREATE,
                                                   &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (
                                    curTrace,
                                    GT_4CLASS,
                                    "FrameQBufMgr_create",
                                    status,
                                    "API (through IOCTL) failed on kernel-side!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            obj                = handle;
                            obj->knlObject = cmdArgs.args.create.handle;
                            obj->instId = cmdArgs.args.create.instId;
                            obj->ctrlInterfaceType = FrameQBufMgr_INTERFACE_SHAREDMEM;
                            obj->objType       = FrameQBufMgr_DYNAMIC_CREATE;
                            obj->notifyId = (UInt32)-1;
                            obj->ctrlStructCacheFlag =
    						    SharedRegion_isCacheEnabled(SharedRegion_getId(
    						    		SharedRegion_getPtr(
    	                                (cmdArgs.args.create.cliNotifyMgrSharedMem))));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
#endif
                        if (cmdArgs.args.create.params != NULL) {
                            Memory_free( NULL,
                                         cmdArgs.args.create.params,
                                         sizeof(FrameQBufMgr_ShMem_Params));
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                    else {
                        GT_setFailureReason (
                                curTrace,
                                GT_4CLASS,
                                "FrameQBufMgr_create",
                                status,
                                "Invalid interface type specified!");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
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
        if (status >= 0) {
            /* Open clientnotifyMgr instance for this instance in user space */
            ClientNotifyMgr_Params_init(&clientMgrParams);
            /* Open the ClientNotifyMgr on user side */
            clientMgrParams.sharedAddr = SharedRegion_getPtr(
                                (cmdArgs.args.create.cliNotifyMgrSharedMem));
            sharedAddr     = SharedRegion_getPtr
                                     (cmdArgs.args.create.cliGateSharedMem);
            status = GateMP_openByAddr(sharedAddr, &gateHandle);
            GT_assert (curtrace,(status >= 0));
           /*
            * Pass the shared memory address to create the client
            * manager instance. This address starts right after the
            * size of gate peterson from the
            * params->sharedAddr.
            */
            clientMgrParams.gate = (Ptr) gateHandle;
            handle->clientNotifyMgrGate = (Ptr) gateHandle;

            /* Pass the same name for ClientNotifyMgr
             */
            clientMgrParams.name = params->commonCreateParams.name;

            /* Event no to be used for this instance */
            clientMgrParams.eventNo =  FrameQBufMgr_NOTIFY_RESERVED_EVENTNO;
            /* This is actually open  call  to open the created instance in
             * Kernel space.
             */
            clientMgrParams.openFlag = TRUE;

            if (handle->ctrlStructCacheFlag == TRUE) {
                clientMgrParams.cacheFlags =
                                   ClientNotifyMgr_CONTROL_CACHEUSE;
            }
            cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);
            GT_assert (curtrace,(NULL != cliMgrHandle));
            if (NULL == cliMgrHandle) {
                /*! @retval FrameQBufMgr_E_FAIL_CLIEN0TIFYMGR_CREATE
                 * Failed to create ClientNotifyMgr instance.
                 */
                status = FrameQBufMgr_E_FAIL_CLIENTN0TIFYMGR_CREATE;
                GT_setFailureReason (
                                 curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgr_ShMem_create",
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

    GT_1trace (curTrace, GT_LEAVE, "_FrameQBufMgr_create", status);

    return (status);
}
