/*
 *  @file   ProcMgr.c
 *
 *  @brief      The Processor Manager on a master processor provides control
 *              functionality for a slave device.
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

/* OSAL & Utils headers */
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/String.h>

/* Module level headers */
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/ProcMgrDrvDefs.h>
#include <ti/syslink/inc/usr/ProcMgrDrvUsr.h>
#include <ti/syslink/inc/_ProcMgrDefs.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Checks if a value lies in given range.
 */
#define IS_RANGE_VALID(x,min,max) (((x) < (max)) && ((x) >= (min)))


/*!
 *  @brief  ProcMgr Module state object
 */
typedef struct ProcMgr_ModuleObject_tag {
    UInt32         setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
    ProcMgr_Handle procHandles [MultiProc_MAXPROCESSORS];
    /*!< Array of Handles of ProcMgr instances */
} ProcMgr_ModuleObject;

/*!
 *  @brief  ProcMgr instance object
 */
typedef struct ProcMgr_Object_tag {
    Ptr                    knlObject;
    /*!< Pointer to the kernel-side ProcMgr object. */
    UInt32                 openRefCount;
    /*!< Reference count for number of times open/close were called in this
         process. */
    Bool                   created;
    /*!< Indicates whether the object was created in this process. */
    UInt16                 procId;
    /*!< Processor ID */
    UInt32                 maxMemoryRegions;
    /*!< Number of storage slots in memEntries */
    UInt32                 numMemEntries;
    /*!< Number of Mem entries */
    ProcMgr_MappedMemEntry memEntries [0];
    /*!< Memory entries */
} ProcMgr_Object;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    ProcMgr_state
 *
 *  @brief  ProcMgr state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
ProcMgr_ModuleObject ProcMgr_state =
{
    .setupRefCount = 0
};


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default configuration for the ProcMgr
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to ProcMgr_setup filled in by the
 *              ProcMgr module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      cfg        Pointer to the ProcMgr module configuration structure
 *                         in which the default config is to be returned.
 *
 *  @sa         ProcMgr_setup, ProcMgrDrvUsr_open, ProcMgrDrvUsr_ioctl,
 *              ProcMgrDrvUsr_close
 */
Void
ProcMgr_getConfig (ProcMgr_Config * cfg)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                         status = ProcMgr_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    ProcMgr_CmdArgsGetConfig    cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getConfig",
                             ProcMgr_E_INVALIDARG,
                             "Argument of type (ProcMgr_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Temporarily open the handle to get the configuration. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ProcMgrDrvUsr_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.cfg = cfg;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETCONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "ProcMgr_getConfig",
                                  status,
                                  "API (through IOCTL) failed on kernel-side!");
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        ProcMgrDrvUsr_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ProcMgr_getConfig");
}


/*!
 *  @brief      Function to setup the ProcMgr module.
 *
 *              This function sets up the ProcMgr module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then ProcMgr_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call ProcMgr_setup with NULL parameters.
 *              The default parameters would get automatically used.
 *
 *  @param      cfg   Optional ProcMgr module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         ProcMgr_destroy, ProcMgrDrvUsr_open, ProcMgrDrvUsr_ioctl
 */
Int
ProcMgr_setup (ProcMgr_Config * cfg)
{
    Int                     status = ProcMgr_S_SUCCESS;
    ProcMgr_CmdArgsSetup    cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_setup", cfg);

    /* TBD: Protect from multiple threads. */
    ProcMgr_state.setupRefCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (ProcMgr_state.setupRefCount > 1) {
        /*! @retval ProcMgr_S_ALREADYSETUP Success: ProcMgr module has been
                                           already setup in this process */
        status = ProcMgr_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "    ProcMgr_setup: ProcMgr module has been already setup "
                   "in this process.\n"
                   "        RefCount: [%d]\n",
                   (ProcMgr_state.setupRefCount - 1));
    }
    else {
        /* Open the driver handle. */
        status = ProcMgrDrvUsr_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.cfg = cfg;
            status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_SETUP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ProcMgr_setup",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                cmdArgs.cfg = cfg;
                status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_CONFIGSYSMEMMAP,
                                              &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_setup", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the ProcMgr module.
 *
 *              Once this function is called, other ProcMgr module APIs, except
 *              for the ProcMgr_getConfig API cannot be called anymore.
 *
 *  @sa         ProcMgr_setup, ProcMgrDrvUsr_ioctl, ProcMgrDrvUsr_close
 */
Int
ProcMgr_destroy (Void)
{
    Int                     status = ProcMgr_S_SUCCESS;
    ProcMgr_CmdArgsDestroy  cmdArgs;
    UInt16                  i;

    GT_0trace (curTrace, GT_ENTER, "ProcMgr_destroy");

    /* TBD: Protect from multiple threads. */
    ProcMgr_state.setupRefCount--;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (ProcMgr_state.setupRefCount > 1) {
        /*! @retval ProcMgr_S_SETUP Success: ProcMgr module has been setup
                                             by other clients in this process */
        status = ProcMgr_S_SETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "ProcMgr module has been setup by other clients in this"
                   " process.\n"
                   "    RefCount: [%d]\n",
                   (ProcMgr_state.setupRefCount + 1));
    }
    else {
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_DESTROY, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_destroy",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Check if any ProcMgr instances have not been deleted so far. If not,
         * delete them.
         */
        for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
            GT_assert (curTrace, (ProcMgr_state.procHandles [i] == NULL));
            if (ProcMgr_state.procHandles [i] != NULL) {
                ProcMgr_close (&(ProcMgr_state.procHandles [i]));
            }
        }
    }

    /* Close the driver handle. */
    ProcMgrDrvUsr_close ();

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_destroy", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for the ProcMgr instance.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to ProcMgr_create filled in by the
 *              ProcMgr module with the default parameters.
 *
 *  @param      handle   Handle to the ProcMgr object. If specified as NULL,
 *                       the default global configuration values are returned.
 *  @param      params   Pointer to the ProcMgr instance params structure in
 *                       which the default params is to be returned.
 *
 *  @sa         ProcMgr_create, ProcMgrDrvUsr_ioctl
 */
Void
ProcMgr_Params_init (ProcMgr_Handle handle, ProcMgr_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                         status          = ProcMgr_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    ProcMgr_Object *            procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsParamsInit   cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_Params_init", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_Params_init",
                             status,
                             "Argument of type (ProcMgr_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Check if the handle is NULL and pass it in directly to kernel-side in
         * that case. Otherwise send the kernel object pointer.
         */
        if (procMgrHandle == NULL) {
            cmdArgs.handle = handle;
        }
        else {
            cmdArgs.handle = procMgrHandle->knlObject;
        }
        cmdArgs.params = params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ProcMgrDrvUsr_ioctl (CMD_PROCMGR_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ProcMgr_Params_init");
}


/*!
 *  @brief      Function to create a ProcMgr object for a specific slave
 *              processor.
 *
 *              This function creates an instance of the ProcMgr module and
 *              returns an instance handle, which is used to access the
 *              specified slave processor. The processor ID specified here is
 *              the ID of the slave processor as configured with the MultiProc
 *              module.
 *              Instance-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then ProcMgr_Params_init can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. For this
 *              API, the params argument is not optional, since the user needs
 *              to provide some essential values such as loader, PwrMgr and
 *              Processor instances to be used with this ProcMgr instance.
 *
 *  @param      procId   Processor ID represented by this ProcMgr instance
 *  @param      params   ProcMgr instance configuration parameters.
 *
 *  @sa         ProcMgr_delete, Memory_calloc, ProcMgrDrvUsr_ioctl
 */
ProcMgr_Handle
ProcMgr_create (UInt16 procId, const ProcMgr_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                     status = ProcMgr_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    ProcMgr_Object *        handle = NULL;
    /* TBD: UInt32                  key;*/
    ProcMgr_CmdArgsCreate   cmdArgs;
    Int                     procMgrObjectSize;
    UInt32                  maxMemoryRegions;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_create", procId, params);

    GT_assert (curTrace, IS_VALID_PROCID (procId));
    GT_assert (curTrace, (params != NULL));
    GT_assert (curTrace, ((params != NULL)) && (params->procHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (!IS_VALID_PROCID (procId)) {
        /*! @retval NULL Invalid procId specified */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_create",
                             status,
                             "Invalid procId specified");
    }
    else if (params == NULL) {
        /*! @retval NULL Invalid NULL params pointer specified */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_create",
                             status,
                             "Invalid NULL params pointer specified");
    }
    else if (params->procHandle == NULL) {
        /*! @retval NULL Invalid NULL procHandle specified in params */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_create",
                             status,
                             "Invalid NULL procHandle specified in params");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* TBD: Enter critical section protection. */
        /* key = IGateProvider_enter (ProcMgr_state.gateHandle); */
        if (ProcMgr_state.procHandles [procId] != NULL) {
            /* If the object is already created/opened in this process, return
             * handle in the local array.
             */
            handle = (ProcMgr_Object *) ProcMgr_state.procHandles [procId];
            handle->openRefCount++;
            GT_1trace (curTrace,
                       GT_2CLASS,
                       "    ProcMgr_create: Instance already exists in this"
                       " process space"
                       "        RefCount [%d]\n",
                       (handle->openRefCount - 1));
        }
        else {
            cmdArgs.procId = procId;
            /* Get the kernel objects of Processor, Loader and PwrMgr modules,
             * and pass them to the kernel-side.
             */
            cmdArgs.params.procHandle = ((ProcMgr_CommonObject *)
                                             (params->procHandle))->knlObject;
            cmdArgs.params.loaderHandle = ((ProcMgr_CommonObject *)
                                             (params->loaderHandle))->knlObject;
            cmdArgs.params.pwrHandle = ((ProcMgr_CommonObject *)
                                             (params->pwrHandle))->knlObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            ProcMgrDrvUsr_ioctl (CMD_PROCMGR_CREATE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "ProcMgr_create",
                                  status,
                                  "API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the handle */
                maxMemoryRegions = cmdArgs.maxMemoryRegions;
                procMgrObjectSize = sizeof(ProcMgr_Object) +
                                    (maxMemoryRegions *
                                     sizeof(ProcMgr_MappedMemEntry));
                handle = (ProcMgr_Object *) Memory_calloc (NULL,
                                                           procMgrObjectSize,
                                                           0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    /*! @retval NULL Memory allocation failed for handle */
                    status = ProcMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "ProcMgr_create",
                                        status,
                                        "Memory allocation failed for handle!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Set pointer to kernel object into the user handle. */
                    handle->knlObject = cmdArgs.handle;
                    /* Indicate that the object was created in this process. */
                    handle->created = TRUE;
                    handle->procId = procId;
                    handle->maxMemoryRegions = maxMemoryRegions;
                    handle->numMemEntries = 0;
                    Memory_set ((Ptr) handle->memEntries,
                                0,
                                sizeof(ProcMgr_MappedMemEntry) *
                                maxMemoryRegions);
                    /* Store the ProcMgr handle in the local array. */
                    ProcMgr_state.procHandles [procId] = (ProcMgr_Handle)handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        /* TBD: Leave critical section protection. */
        /* IGateProvider_leave (ProcMgr_state.gateHandle, key); */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_create", handle);

    /*! @retval Valid-Handle Operation successful */
    return (ProcMgr_Handle) handle;
}


/*!
 *  @brief      Function to delete a ProcMgr object for a specific slave
 *              processor.
 *
 *              Once this function is called, other ProcMgr instance level APIs
 *              that require the instance handle cannot be called.
 *
 *  @param      handlePtr   Pointer to Handle to the ProcMgr object
 *
 *  @sa         ProcMgr_create, Memory_free, ProcMgrDrvUsr_ioctl
 */
Int
ProcMgr_delete (ProcMgr_Handle * handlePtr)
{
    Int                     status    = ProcMgr_S_SUCCESS;
    Int                     tmpStatus = ProcMgr_S_SUCCESS;
    ProcMgr_Object *        handle;
    /* TBD: UInt32          key;*/
    ProcMgr_CmdArgsDelete   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handlePtr == NULL) {
        /*! @retval ProcMgr_E_INVALIDARG Invalid NULL handlePtr specified*/
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_delete",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval ProcMgr_E_HANDLE Invalid NULL *handlePtr specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (ProcMgr_Object *) (*handlePtr);
        /* TBD: Enter critical section protection. */
        /* key = IGateProvider_enter (ProcMgr_state.gateHandle); */

        if (handle->openRefCount != 0) {
            /* There are still some open handles to this ProcMgr.
             * Give a warning, but still go ahead and delete the object.
             */
            status = ProcMgr_S_OPENHANDLE;
            GT_assert (curTrace, (handle->openRefCount != 0));
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "    ProcMgr_delete: Warning, some handles are"
                       " still open!\n"
                       "        RefCount: [%d]\n",
                       handle->openRefCount);
        }

        if (handle->created == FALSE) {
            /*! @retval ProcMgr_E_ACCESSDENIED The ProcMgr object was not
                   created in this process and access is denied to delete it. */
            status = ProcMgr_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_delete",
                                 status,
                                 "The ProcMgr object was not created in this"
                                 "process and access is denied to delete it.");
        }

        if (status >= 0) {
            /* Only delete the object if it was created in this process. */
            cmdArgs.handle = handle->knlObject;
            tmpStatus = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_DELETE, &cmdArgs);
            if (tmpStatus < 0) {
                /* Only override the status if kernel call failed. Otherwise
                 * we want the status from above to carry forward.
                 */
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "ProcMgr_delete",
                                  status,
                                  "API (through IOCTL) failed on kernel-side!");
            }
        }

        /* TBD: Leave critical section protection. */
        /* IGateProvider_leave (ProcMgr_state.gateHandle, key); */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_delete", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/* Function to open a handle to an existing ProcMgr object handling the
 * procId.
 */
Int
ProcMgr_open (ProcMgr_Handle * handlePtr, UInt16 procId)
{
    Int                      status = ProcMgr_S_SUCCESS;
    ProcMgr_Object *         handle = NULL;
    /* UInt32           key; */
    ProcMgr_CmdArgsOpen      cmdArgs;
    ProcMgr_MappedMemEntry * mme;
    UInt32 *                 addr;
    UInt32                   i;
    ProcMgr_CmdArgsClose     closeArgs;
    ProcMgr_ProcInfo       * procInfo;
    Int                      procInfoSize;
    UInt32                   maxMemoryRegions;
    Int                      procMgrObjectSize;


    GT_2trace (curTrace, GT_ENTER, "ProcMgr_open", handlePtr, procId);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, IS_VALID_PROCID (procId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handlePtr == NULL) {
        /*! @retval ProcMgr_E_INVALIDARG Invalid NULL handle pointer specified*/
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_open",
                             status,
                             "Invalid NULL handle pointer specified");
    }
    else if (!IS_VALID_PROCID (procId)) {
        *handlePtr = NULL;
        /*! @retval ProcMgr_E_INVALIDARG Invalid procId specified */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_open",
                             status,
                             "Invalid procId specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* TBD: Enter critical section protection. */
        /* key = IGateProvider_enter (ProcMgr_state.gateHandle); */
        if (ProcMgr_state.procHandles [procId] != NULL) {
            /* If the object is already created/opened in this process, return
             * handle in the local array.
             */
            handle = (ProcMgr_Object *) ProcMgr_state.procHandles [procId];
            handle->openRefCount++;
            status = ProcMgr_S_ALREADYEXISTS;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "    ProcMgr_open: Instance already exists in this"
                       " process space"
                       "        RefCount [%d]\n",
                       (handle->openRefCount - 1));
        }
        else {
            /* The object was not created/opened in this process. Need to drop
             * down to the kernel to get the object instance.
             */
            cmdArgs.procId = procId;
            cmdArgs.handle = NULL; /* Return parameter */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            ProcMgrDrvUsr_ioctl (CMD_PROCMGR_OPEN, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "ProcMgr_open",
                                  status,
                                  "OPEN API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the handle */
                maxMemoryRegions = cmdArgs.maxMemoryRegions;
                procMgrObjectSize = sizeof(ProcMgr_Object) +
                                    (maxMemoryRegions *
                                     sizeof(ProcMgr_MappedMemEntry));
                handle = (ProcMgr_Object *) Memory_calloc (NULL,
                                                           procMgrObjectSize,
                                                           0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    /*! @retval ProcMgr_E_MEMORY Memory allocation failed for
                                                 handle */
                    status = ProcMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "ProcMgr_open",
                                        status,
                                        "Memory allocation failed for handle!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Set pointer to kernel object into the user handle. */
                    handle->knlObject = cmdArgs.handle;
                    /* Store the ProcMgr handle in the local array. */
                    ProcMgr_state.procHandles[procId] = (ProcMgr_Handle)handle;
                    handle->openRefCount = 1;
                    handle->created = FALSE;
                    handle->procId = procId;
                    handle->maxMemoryRegions = maxMemoryRegions;

                    /* Store the memory information received, only if the Proc
                     * has been attached-to already, which will create the
                     * mappings on kernel-side.
                     */
                    if (handle->numMemEntries == 0) {
                        procInfoSize = sizeof(ProcMgr_ProcInfo) +
                                        (maxMemoryRegions *
                                         sizeof(ProcMgr_MappedMemEntry));
                        procInfo = Memory_alloc(NULL, procInfoSize, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (procInfo == NULL) {
                            /*! @retval ProcMgr_E_MEMORY Memory allocation failed for
                                                         procInfo */
                            status = ProcMgr_E_MEMORY;
                            GT_setFailureReason (curTrace,
                                                GT_4CLASS,
                                                "ProcMgr_open",
                                                status,
                                                "Memory allocation failed for procInfo!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        status = ProcMgr_getProcInfo((ProcMgr_Handle)handle,
                                                     procInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                              GT_4CLASS,
                                              "ProcMgr_open",
                                              status,
                                              "GETPROCINFO API (through IOCTL) failed on kernel-side!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                        handle->numMemEntries = procInfo->numMemEntries;
                        for (i = 0;
                             (   (i < handle->maxMemoryRegions)
                              && (status >= 0));
                             i++) {
                            mme = &procInfo->memEntries [i];
                            if (mme->inUse == TRUE) {
                                status = ProcMgr_map ((ProcMgr_Handle)handle,
                                        ProcMgr_MASTERUSRVIRT,
                                        &mme->info,
                                        ProcMgr_AddrType_MasterPhys);
                                if (status < 0) {
                                    GT_setFailureReason (curTrace,
                                                        GT_4CLASS,
                                                        "ProcMgr_open",
                                                        status,
                                                        "ProcMgr_map failed!");
                                }
                            }
                        }

                        /* TBD: Unmap all mapped entries till now */
                       if (status < 0) {
                            for (i = 0;
                                 (   (i < handle->maxMemoryRegions)
                                  && (status >= 0));
                                 i++) {
                                mme  = &procInfo->memEntries [i];
                                addr = mme->info.addr;
                                if (   (mme->inUse == TRUE)
                                    && (   addr [ProcMgr_AddrType_MasterUsrVirt]
                                        != -1)) {
                                    status = ProcMgr_unmap(
                                        (ProcMgr_Handle)handle,
                                        ProcMgr_MASTERUSRVIRT,
                                        &mme->info,
                                        ProcMgr_AddrType_MasterPhys);
                                    if (status < 0) {
                                        GT_setFailureReason (curTrace,
                                                            GT_4CLASS,
                                                            "ProcMgr_open",
                                                            status,
                                                            "ProcMgr_map failed!");
                                    }
                                }
                            }

                            handle->numMemEntries = 0;
                            closeArgs.handle = handle->knlObject;
                            ProcMgrDrvUsr_ioctl(CMD_PROCMGR_CLOSE, &closeArgs);
                            Memory_free(NULL, handle, procMgrObjectSize);
                            handle = NULL;
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                        Memory_free(NULL, procInfo, procInfoSize);
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                 }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        *handlePtr = (ProcMgr_Handle) handle;
        /* TBD: Leave critical section protection. */
        /* IGateProvider_leave (ProcMgr_state.gateHandle, key); */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_open", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/* Function to close this handle to the ProcMgr instance. */
Int
ProcMgr_close (ProcMgr_Handle * handlePtr)
{
    Int                      status = ProcMgr_S_SUCCESS;
    ProcMgr_Object *         procMgrHandle;
    ProcMgr_CmdArgsClose     cmdArgs;
    ProcMgr_MappedMemEntry * mme;
    UInt32 *                 addr;
    UInt32                   i;
    Int                      procMgrObjectSize;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handlePtr == NULL) {
        /*! @retval ProcMgr_E_INVALIDARG Invalid NULL handlePtr specified */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_close",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval ProcMgr_E_HANDLE Invalid NULL *handlePtr specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_close",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        procMgrHandle = (ProcMgr_Object *) (*handlePtr);
        /* TBD: Enter critical section protection. */
        /* key = IGateProvider_enter (ProcMgr_state.gateHandle); */
        if (procMgrHandle->openRefCount == 0) {
            /*! @retval ProcMgr_E_ACCESSDENIED All open handles to this ProcMgr
                                               object are already closed. */
            status = ProcMgr_E_ACCESSDENIED;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_close",
                                 status,
                                 "All open handles to this ProcMgr object are"
                                 " already closed.");

        }
        else if (procMgrHandle->openRefCount > 1) {
            /* Simply reduce the reference count. There are other threads in
             * this process that have also opened handles to this ProcMgr
             * instance.
             */
            procMgrHandle->openRefCount--;
            status = ProcMgr_S_OPENHANDLE;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "    ProcMgr_close: Other handles to this instance"
                       " are still open\n"
                       "        RefCount: [%d]\n",
                       (procMgrHandle->openRefCount + 1));
        }
        else {
            /* The object can be closed now since all open handles are closed.*/
            cmdArgs.handle = procMgrHandle->knlObject;
            status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_CLOSE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "ProcMgr_close",
                                  status,
                                  "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            if (procMgrHandle->created == FALSE) {
                /* Clear the ProcMgr handle in the local array. */
                GT_assert (curTrace,
                           (procMgrHandle->procId < MultiProc_MAXPROCESSORS));
                ProcMgr_state.procHandles [procMgrHandle->procId] = NULL;
                if (procMgrHandle->numMemEntries != 0) {
                    for (i = 0;  (i < procMgrHandle->maxMemoryRegions);  i++) {
                        mme  = &procMgrHandle->memEntries [i];
                        addr = mme->info.addr;
                        if (   (mme->inUse == TRUE)
                            && (   addr [ProcMgr_AddrType_MasterUsrVirt]
                                != -1)) {
                            status = ProcMgr_unmap((*handlePtr),
                                    ProcMgr_MASTERUSRVIRT,
                                    &mme->info, ProcMgr_AddrType_MasterPhys);
                            if (status < 0) {
                                GT_setFailureReason (curTrace,
                                                    GT_4CLASS,
                                                    "ProcMgr_open",
                                                    status,
                                                    "ProcMgr_map failed!");
                            }
                        }
                    }
                    procMgrHandle->numMemEntries = 0;
                }
                /* Free memory for the handle only if it was not created in
                 * this process.
                 */
                procMgrObjectSize = sizeof(ProcMgr_Object) +
                                    (procMgrHandle->maxMemoryRegions *
                                     sizeof(ProcMgr_MappedMemEntry));
                Memory_free (NULL, procMgrHandle, procMgrObjectSize);
            }
            *handlePtr = NULL;
        }
        /* TBD: Leave critical section protection. */
        /* IGateProvider_leave (ProcMgr_state.gateHandle, key); */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_close", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/* Function to initialize the parameters for the ProcMgr attach function. */
Void
ProcMgr_getAttachParams (ProcMgr_Handle handle, ProcMgr_AttachParams * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                             status          = ProcMgr_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    ProcMgr_Object *                procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetAttachParams  cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_getAttachParams", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getAttachParams",
                             ProcMgr_E_INVALIDARG,
                             "Argument of type (ProcMgr_AttachParams *) passed "
                             "is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* If NULL, send the same to kernel-side. Otherwise translate and send
         * the kernel handle.
         */
        if (procMgrHandle == NULL) {
            cmdArgs.handle = handle;
        }
        else {
            cmdArgs.handle = procMgrHandle->knlObject;
        }
        cmdArgs.params = params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETATTACHPARAMS, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getAttachParams",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ProcMgr_getAttachParams");
}


/* Function to attach the client to the specified slave and also initialize the
 * slave (if required).
 */
Int
ProcMgr_attach (ProcMgr_Handle handle, ProcMgr_AttachParams * params)
{
    Int                      status           = ProcMgr_S_SUCCESS;
    ProcMgr_Object *         procMgrHandle    = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsAttach    cmdArgs;
    ProcMgr_CmdArgsDetach    detachArgs;
    UInt32                   i;
    ProcMgr_MappedMemEntry * mme;
    UInt32 *                 addr;
    ProcMgr_ProcInfo       * procInfo;
    Int                      procInfoSize;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_attach", handle, params);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_attach",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.params = params;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_ATTACH, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_attach",
                                 status,
                                 "ATTACH API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Store the memory information received, only if the Proc
             * has been attached-to already, which will create the
             * mappings on kernel-side.
             */
            if (procMgrHandle->numMemEntries == 0) {
                procInfoSize = sizeof(ProcMgr_ProcInfo) +
                               (procMgrHandle->maxMemoryRegions *
                                sizeof(ProcMgr_MappedMemEntry));
                procInfo = Memory_alloc(NULL, procInfoSize, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (procInfo == NULL) {
                    /*! @retval ProcMgr_E_MEMORY Memory allocation failed for
                                                 procInfo */
                    status = ProcMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "ProcMgr_open",
                                        status,
                                        "Memory allocation failed for procInfo!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = ProcMgr_getProcInfo(handle, procInfo);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "ProcMgr_open",
                                      status,
                                      "GETPROCINFO API (through IOCTL) failed on kernel-side!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                procMgrHandle->numMemEntries = procInfo->numMemEntries;
                for (i = 0;
                     (   (i < procMgrHandle->maxMemoryRegions)
                      && (status >= 0));
                     i++) {
                    mme = &procInfo->memEntries [i];
                    if (mme->inUse == TRUE) {
                        status = ProcMgr_map(handle,
                                ProcMgr_MASTERUSRVIRT, &mme->info,
                                ProcMgr_AddrType_MasterPhys);
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                                GT_4CLASS,
                                                "ProcMgr_open",
                                                status,
                                                "ProcMgr_map failed!");
                        }
                    }
                }

                /* TBD: Unmap all mapped entries till now */
                if (status < 0) {
                    for (i = 0;
                         (   (i < procMgrHandle->maxMemoryRegions)
                          && (status >= 0));
                         i++) {

                        mme  = &procInfo->memEntries [i];
                        addr = mme->info.addr;
                        if (   (mme->inUse == TRUE)
                            && (   addr [ProcMgr_AddrType_MasterUsrVirt]
                                != -1)) {
                            status = ProcMgr_unmap(handle,
                                    ProcMgr_MASTERUSRVIRT, &mme->info,
                                    ProcMgr_AddrType_MasterPhys);
                            if (status < 0) {
                                GT_setFailureReason (curTrace,
                                                    GT_4CLASS,
                                                    "ProcMgr_open",
                                                    status,
                                                    "ProcMgr_map failed!");
                            }
                        }
                    }
                    procMgrHandle->numMemEntries = 0;
                    detachArgs.handle = procMgrHandle->knlObject;
                    ProcMgrDrvUsr_ioctl (CMD_PROCMGR_DETACH, &detachArgs);
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Memory_free(NULL, procInfo, procInfoSize);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_attach", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/* Function to detach the client from the specified slave and also finalze the
 * slave (if required).
 */
Int
ProcMgr_detach (ProcMgr_Handle handle)
{
    Int                      status           = ProcMgr_S_SUCCESS;
    ProcMgr_Object *         procMgrHandle    = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsDetach    cmdArgs;
    UInt32                   i;
    ProcMgr_MappedMemEntry * mme;
    UInt32 *                 addr;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_detach", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_detach",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (procMgrHandle->numMemEntries != 0) {
            for (i = 0;  (i < procMgrHandle->maxMemoryRegions);  i++) {
                mme  = &procMgrHandle->memEntries [i];
                addr = mme->info.addr;
                if (   (mme->inUse == TRUE)
                    && (addr [ProcMgr_AddrType_MasterUsrVirt] != -1)) {
                    status = ProcMgr_unmap(handle, ProcMgr_MASTERUSRVIRT,
                            &mme->info, ProcMgr_AddrType_MasterPhys);
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "ProcMgr_detach",
                                            status,
                                            "ProcMgr_unmap failed!");
                    }
                }
            }
            procMgrHandle->numMemEntries = 0;
        }

        cmdArgs.handle = procMgrHandle->knlObject;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_DETACH, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_detach",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_detach", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return (status);
}


/* Function to load the specified slave executable on the slave Processor. */
Int
ProcMgr_load (ProcMgr_Handle handle,
              String         imagePath,
              UInt32         argc,
              String *       argv,
              Ptr            params,
              UInt32 *       fileId)
{
    Int                 status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *    procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsLoad cmdArgs;

    GT_5trace (curTrace, GT_ENTER, "ProcMgr_load",
               handle, imagePath, argc, argv, params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (fileId != NULL));
    /* imagePath may be NULL if a non-file based loader is used. In that case,
     * loader-specific params will contain the required information.
     */

    GT_assert (curTrace,
               (   ((argc == 0) && (argv == NULL))
                || ((argc != 0) && (argv != NULL))));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_load",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (   ((argc == 0) && (argv != NULL))
             || ((argc != 0) && (argv == NULL))) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid argument */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_load",
                             status,
                             "Invalid argc/argv values specified");
    }
    else if (fileId == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid argument */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_load",
                             status,
                             "Invalid fileId pointer specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        *fileId = 0; /* Initialize return parameter. */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.imagePath = imagePath;
        cmdArgs.imagePathLen = String_len (imagePath) + 1;
        cmdArgs.argc = argc;
        cmdArgs.argv = argv;
        cmdArgs.params = params;
        cmdArgs.fileId = 0; /* Return parameter */
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_LOAD, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_load",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            *fileId = cmdArgs.fileId;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_load", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to unload the previously loaded file on the slave processor.
 * The fileId received from the load function must be passed to this
 * function.
 */
Int
ProcMgr_unload (ProcMgr_Handle handle, UInt32 fileId)
{
    Int                    status           = ProcMgr_S_SUCCESS;
    ProcMgr_Object *       procMgrHandle    = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsUnload  cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_unload", handle, fileId);

    GT_assert (curTrace, (handle != NULL));
    /* Cannot check for fileId because it is loader dependent. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_unload",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.fileId = fileId;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_UNLOAD, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_unload",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_unload", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to initialize the parameters for the ProcMgr start function. */
Void
ProcMgr_getStartParams (ProcMgr_Handle handle, ProcMgr_StartParams * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                             status          = ProcMgr_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    ProcMgr_Object *                procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetStartParams   cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_getStartParams", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getStartParams",
                             ProcMgr_E_INVALIDARG,
                             "Argument of type (ProcMgr_StartParams *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.params = params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETSTARTPARAMS, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getStartParams",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ProcMgr_getStartParams");
}


/* Function to starts execution of the loaded code on the slave from the
 * starting point specified in the slave executable loaded earlier by call to
 * ProcMgr_load ().
 */
Int
ProcMgr_start (ProcMgr_Handle handle, ProcMgr_StartParams * params)
{
    Int                     status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *        procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsStart    cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_start", handle, params);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_start",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.params = params;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_START, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_start",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_start", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to stop execution of the slave Processor. */
Int
ProcMgr_stop (ProcMgr_Handle handle)
{
    Int                 status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *    procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsStop cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_stop", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_stop",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_STOP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_stop",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_stop", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to get the current state of the slave Processor as maintained on
 * the master Processor state machine.
 */
ProcMgr_State
ProcMgr_getState (ProcMgr_Handle handle)
{
    Int                     status          = ProcMgr_S_SUCCESS;
    ProcMgr_State           state           = ProcMgr_State_Unknown;
    ProcMgr_Object *        procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetState cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_getState", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /* No status set here since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getState",
                             ProcMgr_E_HANDLE,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETSTATE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getState",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            state = cmdArgs.procMgrState;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_getState", state);

    /*! @retval Processor state */
    return state;
}


/* Function to read from the slave Processor's memory space. */
Int
ProcMgr_read (ProcMgr_Handle handle,
              UInt32         procAddr,
              UInt32 *       numBytes,
              Ptr            buffer)
{
    Int                  status         = ProcMgr_S_SUCCESS;
    ProcMgr_Object *     procMgrHandle  = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsRead  cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "ProcMgr_read",
               handle, procAddr, numBytes, buffer);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (numBytes != NULL));
    GT_assert (curTrace, (buffer   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_read",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (numBytes == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument numBytes */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "ProcMgr_read",
                           status,
                           "Invalid value NULL provided for argument numBytes");
    }
    else if (buffer == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument buffer */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_read",
                             status,
                             "Invalid value NULL provided for argument buffer");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.procAddr = procAddr;
        cmdArgs.numBytes = *numBytes;
        cmdArgs.buffer = buffer;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_READ, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_read",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Return number of bytes actually read. */
            *numBytes = cmdArgs.numBytes;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_read", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to read from the slave Processor's memory space. */
Int
ProcMgr_write (ProcMgr_Handle handle,
               UInt32         procAddr,
               UInt32 *       numBytes,
               Ptr            buffer)
{
    Int                     status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *        procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsWrite    cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "ProcMgr_write",
               handle, procAddr, numBytes, buffer);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (numBytes != NULL));
    GT_assert (curTrace, (buffer   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_write",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (numBytes == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument numBytes */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "ProcMgr_write",
                           status,
                           "Invalid value NULL provided for argument numBytes");
    }
    else if (buffer == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument buffer */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_write",
                             status,
                             "Invalid value NULL provided for argument buffer");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.procAddr = procAddr;
        cmdArgs.numBytes = *numBytes;
        cmdArgs.buffer = buffer;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_WRITE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_write",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Return number of bytes actually written. */
            *numBytes = cmdArgs.numBytes;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_write", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function that provides a hook for performing device dependent operations on
 * the slave Processor.
 */
Int
ProcMgr_control (ProcMgr_Handle handle, Int32 cmd, Ptr arg)
{
    Int                     status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *        procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsControl  cmdArgs;

    GT_3trace (curTrace, GT_ENTER, "ProcMgr_control", handle, cmd, arg);

    GT_assert (curTrace, (handle != NULL));
    /* cmd and arg can be 0/NULL, so cannot check for validity. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_control",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.cmd = cmd;
        cmdArgs.arg = arg;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_CONTROL, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_control",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_control", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to translate between two types of address spaces. */
Int
ProcMgr_translateAddr (ProcMgr_Handle   handle,
                       Ptr *            dstAddr,
                       ProcMgr_AddrType dstAddrType,
                       Ptr              srcAddr,
                       ProcMgr_AddrType srcAddrType)
{
    Int                      status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *         procMgrHandle   = (ProcMgr_Object *) handle;
    UInt32                   fmAddrBase      = (UInt32) NULL;
    UInt32                   toAddrBase      = (UInt32) NULL;
    Bool                     found           = FALSE;
    ProcMgr_MappedMemEntry * me;
    UInt32                   i;

    GT_5trace (curTrace, GT_ENTER, "ProcMgr_translateAddr",
               handle, dstAddr, dstAddrType, srcAddr, srcAddrType);

    GT_assert (curTrace, (handle        != NULL));
    GT_assert (curTrace, (dstAddr       != NULL));
    GT_assert (curTrace, (dstAddrType   < ProcMgr_AddrType_EndValue));
    GT_assert (curTrace, (srcAddr       != NULL));
    GT_assert (curTrace, (srcAddrType   < ProcMgr_AddrType_EndValue));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_translateAddr",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (dstAddr == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument dstAddr */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "ProcMgr_translateAddr",
                            status,
                            "Invalid value NULL provided for argument dstAddr");
    }
    else if (dstAddrType >= ProcMgr_AddrType_EndValue) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value provided for
                     argument dstAddrType */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_translateAddr",
                             status,
                             "Invalid value provided for argument dstAddrType");
    }
    else if (srcAddr == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument srcAddr */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "ProcMgr_translateAddr",
                            status,
                            "Invalid value NULL provided for argument srcAddr");
    }
    else if (srcAddrType >= ProcMgr_AddrType_EndValue) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value provided for
                     argument srcAddrType */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_translateAddr",
                             status,
                             "Invalid value provided for argument srcAddrType");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        *dstAddr = NULL;
        /* Translate the address. */
        for (i = 0; i < procMgrHandle->maxMemoryRegions; i++) {
            me = &procMgrHandle->memEntries [i];
            if (me->inUse == TRUE) {
                fmAddrBase = me->info.addr [srcAddrType];
                toAddrBase = me->info.addr [dstAddrType];
                if (IS_RANGE_VALID ((UInt32) srcAddr,
                                    fmAddrBase,
                                    (fmAddrBase + me->info.size))) {
                    found = TRUE;
                    *dstAddr = (Ptr) (  ((UInt32) srcAddr - fmAddrBase)
                                      + toAddrBase);
                    break;
                }
            }
        }

        /* This check must not be removed even with build optimize. */
        if (found == FALSE) {
            /*! @retval ProcMgr_E_TRANSLATE Failed to translate address. */
            status = ProcMgr_E_TRANSLATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_translateAddr",
                                 status,
                                 "Failed to translate address");
        }
        else {
            GT_2trace (curTrace,
                       GT_1CLASS,
                       "    ProcMgr_translateAddr: srcAddr [0x%x] "
                       "dstAddr [0x%x]\n",
                       srcAddr,
                      *dstAddr);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_translateAddr", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function that gets the slave address corresponding to a symbol within an
 * executable currently loaded on the slave Processor.
 */
Int
ProcMgr_getSymbolAddress (ProcMgr_Handle handle,
                          UInt32         fileId,
                          String         symbolName,
                          UInt32 *       symValue)
{
    Int                             status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *                procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetSymbolAddress cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "ProcMgr_getSymbolAddress",
               handle, fileId, symbolName, symValue);

    GT_assert (curTrace, (handle      != NULL));
    /* fileId may be 0, so no check for fileId. */
    GT_assert (curTrace, (symbolName  != NULL));
    GT_assert (curTrace, (symValue    != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getSymbolAddress",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (symbolName == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument symbolName */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                         GT_4CLASS,
                         "ProcMgr_getSymbolAddress",
                         status,
                         "Invalid value NULL provided for argument symbolName");
    }
    else if (symValue == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument symValue */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                         GT_4CLASS,
                         "ProcMgr_getSymbolAddress",
                         status,
                         "Invalid value NULL provided for argument symValue");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.fileId = fileId;
        cmdArgs.symbolName = symbolName;
        cmdArgs.symValue = 0u; /* Return parameter. */
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETSYMBOLADDRESS, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getSymbolAddress",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Return symbol address. */
            *symValue = cmdArgs.symValue;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_getSymbolAddress", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function that maps the specified slave address to master address space. */
Int
ProcMgr_map (ProcMgr_Handle     handle,
             ProcMgr_MapMask    mapType,
             ProcMgr_AddrInfo * addrInfo,
             ProcMgr_AddrType   srcAddrType)
{
    Int                 status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *    procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsMap  cmdArgs;
    Memory_MapInfo      mInfo;
    UInt32              i;

    GT_4trace (curTrace, GT_ENTER, "ProcMgr_map",
               handle, mapType, addrInfo, srcAddrType);

    GT_assert (curTrace, (handle  != NULL));
    GT_assert (curTrace, (addrInfo != NULL));
    GT_assert (curTrace, (srcAddrType < ProcMgr_AddrType_EndValue));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_map",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (addrInfo == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value provided for
                     argument type */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_map",
                             status,
                             "Invalid value provided for argument addrInfo");
    }
    else if (srcAddrType >= ProcMgr_AddrType_EndValue) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value provided for
                     argument srcAddrType */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_map",
                             status,
                             "Invalid value provided for argument srcAddrType");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Initialize the address of ProcMgr_AddrType_MasterUsrVirt */
        addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt] = -1u;

        /* Handle mapping of physical and kernel buffer here */
        if (mapType & ProcMgr_MASTERUSRVIRT) {
            switch (srcAddrType) {
                case ProcMgr_AddrType_MasterPhys:
                {
                    mInfo.src = addrInfo->addr[ProcMgr_AddrType_MasterPhys];
                    mInfo.size = addrInfo->size;
                    mInfo.isCached = addrInfo->isCached;
                    status = Memory_map (&mInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* Override with ProcMgr status code. */
                        status = ProcMgr_E_MAP;
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgr_map",
                                         status,
                                         "Memory_map failed");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt] =
                                                                  mInfo.dst;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
                break;
                case ProcMgr_AddrType_MasterKnlVirt:
                {
                    /* TBD: handle this */
                }
                break;
                default:
                {
                }
                break;
            }
        }

        if ((mapType & ~ProcMgr_MASTERUSRVIRT) > 0) {
            cmdArgs.handle = procMgrHandle->knlObject;
            cmdArgs.addrInfo = addrInfo;
            cmdArgs.srcAddrType = srcAddrType;
            cmdArgs.mapType = mapType;
            status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_MAP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ProcMgr_map",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   if (status >= 0) {
        /* Find a free slot */
        for (i = 0; i < procMgrHandle->maxMemoryRegions; i++) {
            if (procMgrHandle->memEntries [i].inUse == FALSE) {
                break;
            }
        }

        if (i != procMgrHandle->maxMemoryRegions) {
            GT_2trace (curTrace,
                       GT_1CLASS,
                    "ProcMgr_map: Mapping entry\n"
                    "addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt] [0x%x]\n"
                    "addrInfo->addr [ProcMgr_AddrType_MasterPhys] [0x%x]\n",
                    addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt],
                    addrInfo->addr [ProcMgr_AddrType_MasterPhys]);
            Memory_copy(&(procMgrHandle->memEntries[i].info),
                    addrInfo, sizeof (ProcMgr_AddrInfo));
            procMgrHandle->memEntries[i].inUse = TRUE;
            procMgrHandle->memEntries[i].srcAddrType = srcAddrType;
            procMgrHandle->memEntries[i].mapMask = mapType;
            procMgrHandle->numMemEntries++;
        }
        else {
            /* TBD: unmap the mapped */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_map",
                                 status,
                                 "All memEntries slots are in use!");
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_map", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function that unmaps the specified slave address from master address space. */
Int
ProcMgr_unmap (ProcMgr_Handle     handle,
               ProcMgr_MapMask    unmapType,
               ProcMgr_AddrInfo * addrInfo,
               ProcMgr_AddrType   srcAddrType)
{
    Int                      status         = ProcMgr_S_SUCCESS;
    ProcMgr_Object *         procMgrHandle  = (ProcMgr_Object *) handle;
    ProcMgr_MappedMemEntry * mme = NULL;
    ProcMgr_CmdArgsUnmap     cmdArgs;
    UInt32                   i;
    Memory_UnmapInfo         umInfo;

    GT_4trace (curTrace, GT_ENTER, "ProcMgr_unmap",
               handle, unmapType, addrInfo, srcAddrType);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (addrInfo != NULL));
    GT_assert (curTrace, (srcAddrType < ProcMgr_AddrType_EndValue));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid argument */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_unmap",
                             status,
                             "Invalid handle specified");
    }
    else if (addrInfo == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Addrinfo is NULL */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_unmap",
                             status,
                             "Addrinfo is NULL");
    }
    else if (srcAddrType >= ProcMgr_AddrType_EndValue) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value provided for
                     argument srcAddrType */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                         GT_4CLASS,
                         "ProcMgr_unmap",
                         status,
                         "Invalid value provided for argument srcAddrType");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First try to find exact entry */
        for (i = 0; i < procMgrHandle->maxMemoryRegions; i++) {
            if ((procMgrHandle->memEntries[i].info.addr[srcAddrType] ==
                    addrInfo->addr[srcAddrType]) &&
                    (unmapType == procMgrHandle->memEntries[i].mapMask) &&
                    (procMgrHandle->memEntries[i].inUse == TRUE))   {
                mme = &procMgrHandle->memEntries[i];
                GT_5trace (curTrace,
                     GT_1CLASS,
                 "_ProcMgr_unmap check:\n"
                 "    unmapType                                       [0x%x]\n"
                 "    srcAddrType                                     [0x%x]\n"
                 "    mme->info.addr [ProcMgr_AddrType_MasterPhys]    [0x%x]\n"
                 "    mme->info.addr [ProcMgr_AddrType_SlaveVirt]     [0x%x]\n"
                 "    mme->info.addr [ProcMgr_AddrType_MasterUsrVirt] [0x%x]\n",
                 unmapType,
                 srcAddrType,
                 mme->info.addr [ProcMgr_AddrType_MasterPhys],
                 mme->info.addr [ProcMgr_AddrType_SlaveVirt],
                 mme->info.addr [ProcMgr_AddrType_MasterUsrVirt]);

                /* Check if the address provided for unmap type matches. If it
                 * doesn't, then there are multiple mappings for the same source
                 * address, and this is not the correct one to be unmappd.
                 * In that case, continue to search.
                 */
                if ((unmapType & ProcMgr_MASTERUSRVIRT)
                    &&  (mme->info.addr [ProcMgr_AddrType_MasterUsrVirt]
                         != addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt])) {
                    continue;
                }

                /* Otherwise found the entry, so break. */
                break;
            }
        }

        /* This may be partial unmap */
        if (i == procMgrHandle->maxMemoryRegions) {
            for (i = 0; i < procMgrHandle->maxMemoryRegions; i++) {
                if ( (    procMgrHandle->memEntries [i].info.addr [srcAddrType]
                       == addrInfo->addr [srcAddrType])
                    && (procMgrHandle->memEntries [i].inUse == TRUE)) {
                    mme = &procMgrHandle->memEntries [i];

                    GT_5trace (curTrace,
                               GT_1CLASS,
                 "_ProcMgr_unmap check:\n"
                 "    unmapType                                       [0x%x]\n"
                 "    srcAddrType                                     [0x%x]\n"
                 "    mme->info.addr [ProcMgr_AddrType_MasterPhys]    [0x%x]\n"
                 "    mme->info.addr [ProcMgr_AddrType_SlaveVirt]     [0x%x]\n"
                 "    mme->info.addr [ProcMgr_AddrType_MasterUsrVirt] [0x%x]\n",
                 unmapType,
                 srcAddrType,
                 mme->info.addr [ProcMgr_AddrType_MasterPhys],
                 mme->info.addr [ProcMgr_AddrType_SlaveVirt],
                 mme->info.addr [ProcMgr_AddrType_MasterUsrVirt]);

                    /* Check if the address provided for unmap type matches. If
                     * it doesn't, then there are multiple mappings for the same
                     * source address, and this is not the correct one to be
                     * unmapped. In that case, continue to search.
                     */
                    if ((unmapType & ProcMgr_MASTERUSRVIRT)
                        && (mme->info.addr [ProcMgr_AddrType_MasterUsrVirt]
                           != addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt])){
                        continue;
                    }

                    /* Otherwise found the entry, so break. */
                    break;
                }
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (i == procMgrHandle->maxMemoryRegions) {
            status = ProcMgr_E_NOTFOUND;
            /*! @retval  ProcMgr_E_NOTFOUND Info provided does not match with
                     any mapped entry */
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_unmap",
                             status,
                             "Info provided does not match with any"
                             " mapped entry");
            GT_5trace (curTrace,
                 GT_4CLASS,
                 "_ProcMgr_unmap failure!\n"
                 "    unmapType                                       [0x%x]\n"
                 "    srcAddrType                                     [0x%x]\n"
                 "    addrInfo->addr [ProcMgr_AddrType_MasterPhys]    [0x%x]\n"
                 "    addrInfo->addr [ProcMgr_AddrType_SlaveVirt]     [0x%x]\n"
                 "    addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt] [0x%x]\n",
                 unmapType,
                 srcAddrType,
                 addrInfo->addr [ProcMgr_AddrType_MasterPhys],
                 addrInfo->addr [ProcMgr_AddrType_SlaveVirt],
                 addrInfo->addr [ProcMgr_AddrType_MasterUsrVirt]);
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (unmapType & ProcMgr_MASTERUSRVIRT) {
                umInfo.addr = mme->info.addr [ProcMgr_AddrType_MasterUsrVirt];
                umInfo.size = mme->info.size;
                umInfo.isCached = mme->info.isCached;
                status = Memory_unmap (&umInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override with ProcMgr status code. */
                    status = ProcMgr_E_MAP;
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ProcMgr_unmap",
                                     status,
                                     "Memory_unmap failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if ((unmapType & ~ProcMgr_MASTERUSRVIRT) > 0) {
                /* Unmap from host address space. */
                cmdArgs.handle = procMgrHandle->knlObject;
                cmdArgs.addrInfo = addrInfo;
                cmdArgs.srcAddrType = srcAddrType;
                cmdArgs.mapType = unmapType;
                status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_UNMAP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgr_unmap",
                                         status,
                                         "Processor_unmap failed!");
                }
#endif
            }

            if (unmapType == mme->mapMask) {
                /* Since entry is unmap from all address space, we can
                 * safely free the slot used */
                mme->inUse = FALSE;
                procMgrHandle->numMemEntries--;
            }
            else {
                mme->mapMask &= ~(unmapType);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_unmap", status);

    return (status);
}


/*
 * Get the maximum number of memory entries
 *
 * The kernel objects contain tables of size ProcMgr_MAX_MEMORY_REGIONS, so
 * this function retrieves that value for dynamically-sized arrays.
 */
UInt32 ProcMgr_getMaxMemoryRegions(ProcMgr_Handle handle)
{
    UInt32 maxMemRegions;
    ProcMgr_Object *objectPtr;

    GT_1trace(curTrace, GT_ENTER, "ProcMgr_getMaxMemoryRegions", handle);

    GT_assert(curTrace, (handle != NULL));

    maxMemRegions = 0;
    if (handle) {
        objectPtr = (ProcMgr_Object *)handle;
        maxMemRegions = objectPtr->maxMemoryRegions;
    }

    GT_1trace(curTrace, GT_LEAVE, "ProcMgr_getMaxMemoryRegions", maxMemRegions);

    return (maxMemRegions);
}


/* Function that returns information about the characteristics of the slave
 * processor.
 */
Int
ProcMgr_getProcInfo (ProcMgr_Handle     handle,
                     ProcMgr_ProcInfo * procInfo)
{
    Int                         status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *            procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetProcInfo  cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "ProcMgr_getProcInfo", handle, procInfo);

    GT_assert (curTrace, (handle    != NULL));
    GT_assert (curTrace, (procInfo  != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getProcInfo",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (procInfo == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument procInfo */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "ProcMgr_getProcInfo",
                           status,
                           "Invalid value NULL provided for argument procInfo");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.procInfo = procInfo;
        cmdArgs.maxMemEntries = procMgrHandle->maxMemoryRegions;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETPROCINFO, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getProcInfo",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_getProcInfo", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function that returns section information given the name of section and
 * number of bytes to read
 */
Int ProcMgr_getSectionInfo (ProcMgr_Handle        handle,
                            UInt32                fileId,
                            String                sectionName,
                            ProcMgr_SectionInfo * sectionInfo)
{
    Int                            status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *               procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetSectionInfo  cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "ProcMgr_getSectionInfo",
               handle, fileId, sectionName, sectionInfo);

    GT_assert (curTrace, (handle    != NULL));
    GT_assert (curTrace, (sectionInfo  != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getSectionInfo",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (sectionInfo == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument sectionInfo */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "ProcMgr_getSectionInfo",
                           status,
                           "Invalid value NULL provided for argument"
                           "sectionInfo");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.sectionInfo = sectionInfo;
        cmdArgs.fileId = fileId;
        cmdArgs.sectionName = sectionName;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETSECTIONINFO, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getSectionInfo",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_getSectionInfo", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function that returns section data in a buffer given section id and size
 * to be read
 */
Int ProcMgr_getSectionData (ProcMgr_Handle        handle,
                            UInt32                fileId,
                            ProcMgr_SectionInfo * sectionInfo,
                            Ptr                   buffer)
{
    Int                            status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *               procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetSectionData  cmdArgs;


    GT_4trace (curTrace, GT_ENTER, "ProcMgr_getSectionData",
               handle, fileId, sectionInfo, buffer);

    GT_assert (curTrace, (handle  != NULL));
    GT_assert (curTrace, (buffer  != 0));
    GT_assert (curTrace, (sectionInfo != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval  ProcMgr_E_HANDLE Invalid NULL handle specified */
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getSectionData",
                             status,
                             "Invalid NULL handle specified");
    }
    else if (buffer == NULL) {
        /*! @retval  ProcMgr_E_INVALIDARG Invalid value NULL provided for
                     argument buffer */
        status = ProcMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "ProcMgr_getSectionData",
                           status,
                           "Invalid value NULL provided for argument"
                           "buffer");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        cmdArgs.fileId = fileId;
        cmdArgs.sectionInfo = sectionInfo;
        cmdArgs.buffer = buffer;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETSECTIONDATA, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getSectionData",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_getSectionData", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successful */
    return status;
}


/* Function to access fileId field from ProcMgr object */
UInt32 ProcMgr_getLoadedFileId (ProcMgr_Handle handle)
{
    Int                            status          = ProcMgr_S_SUCCESS;
    ProcMgr_Object *               procMgrHandle   = (ProcMgr_Object *) handle;
    ProcMgr_CmdArgsGetLoadedFileId cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ProcMgr_getLoadedFileId", handle);

    GT_assert (curTrace, (handle  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = ProcMgr_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgr_getLoadedFileId",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.handle = procMgrHandle->knlObject;
        status = ProcMgrDrvUsr_ioctl (CMD_PROCMGR_GETLOADEDFILEID, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgr_getLoadedFileId",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgr_getLoadedFileId", cmdArgs.fileId);

    /*! @retval fileId Operation successful */
    return cmdArgs.fileId;
}
