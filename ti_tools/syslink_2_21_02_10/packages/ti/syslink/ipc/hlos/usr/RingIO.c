/*
 *  @file   RingIO.c
 *
 *  @brief      RingIO module implementation
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




#include <ti/syslink/Std.h>

/* Osal And Utils  headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Cache.h>

#include <ti/syslink/utils/IGateProvider.h>

/* Utilities & OSAL headers */
#include <ti/syslink/utils/Gate.h>
#include <ti/ipc/MultiProc.h>

/* Module level headers */
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/RingIO.h>
//TBD : RingIO should not include RingIOShm.h it is a temporary solution
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/RingIODrvDefs.h>
#include <ti/syslink/inc/usr/Linux/RingIODrv.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>


#define   RINGIO_NOTIFYENRIES   2 /* For reader and writer */
#define   RINGIO_SUBNOTIFYENRIES   1

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/* Structure defining object for the Gate Peterson */
typedef struct RingIO_Object_tag {
    Ptr                     knlObject;
    /*!< Pointer to the kernel-side RingIO object. */
    Ptr                     createdKnlObject;
    /*!< Pointer to the kernel-side RingIO object returned in create call.  */
    ClientNotifyMgr_Handle  clientNotifyMgrHandle;
    /*!< Handle to the client notifyMgr handle */
    UInt32                  notifyId;
    /*!< Handle to the client notifyMgr handle */
    GateMP_Handle           clientNotifyMgrGate;
    /*!< Gate opened for client NotifyMgr */
    RingIO_OpenMode         objMode;
    /*!< mode of the instacne */
    Bool                    created;
    /*!< whether this object was created or opened */
    Bool                    isRegistered;
    /* !< Indicates whether this instance is registred a call back function with
     *  the clientNotifyMgr.
     */
} RingIO_Object;

/*!
 *  @brief  RingIO Module state object
 */
typedef struct RingIO_ModuleObject_tag {
    UInt32          setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
    RingIO_Config   cfg;
    /* Current config */
} RingIO_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    RingIO_state
 *
 *  @brief  RingIO state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
RingIO_ModuleObject RingIO_state =
{
    .setupRefCount = 0
};


/* =============================================================================
 * APIS
 * =============================================================================
 */
/* Function to initialize the Config parameter structure with default values.*/
Void
RingIO_getConfig (RingIO_Config * cfg)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32 status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getConfig",
                             RingIO_E_INVALIDARG,
                             "Argument of type (RingIO_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Temporarily open the handle to get the configuration. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIODrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.getConfig.config = cfg;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            RingIODrv_ioctl (CMD_RINGIO_GETCONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "RingIO_getConfig",
                                  status,
                                  "API (through IOCTL) failed on kernel-side!");

            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        RingIODrv_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "RingIO_getConfig");
}


/* Function to setup the RingIO module. */
Int32
RingIO_setup (const RingIO_Config * config)
{
    Int32              status = RingIO_S_SUCCESS;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_setup", config);

    /* TBD: Protect from multiple threads. */
    RingIO_state.setupRefCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (RingIO_state.setupRefCount > 1) {
        status = RingIO_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "RingIO module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   RingIO_state.setupRefCount);
    }
    else {
        /* Open the driver handle. */
        status = RingIODrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.setup.config = (RingIO_Config *) config;
            status = RingIODrv_ioctl (CMD_RINGIO_SETUP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "RingIO_setup",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Memory_copy ((Ptr) &RingIO_state.cfg,
                             (Ptr) config,
                             sizeof (RingIO_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setup", status);

    return status;
}


/* Function to destroy the RingIO module. */
Int32
RingIO_destroy (Void)
{
    Int32                 status = RingIO_S_SUCCESS;
    RingIODrv_CmdArgs   cmdArgs;

    GT_0trace (curTrace, GT_ENTER, "RingIO_destroy");

    /* TBD: Protect from multiple threads. */
    RingIO_state.setupRefCount--;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (RingIO_state.setupRefCount >= 1) {
        status = RingIO_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "RingIO module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   RingIO_state.setupRefCount);
    }
    else {
        status = RingIODrv_ioctl (CMD_RINGIO_DESTROY, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_destroy",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Close the driver handle. */
        RingIODrv_close ();
    }

    GT_1trace (curTrace, GT_LEAVE, "RingIO_destroy", status);

    return status;
}


/* Initialize this config-params structure with supplier-specified
 * defaults before instance creation.
 */
Void
RingIO_Params_init (Void * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32               status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    RingIODrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_Params_init",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_Params_init",
                             RingIO_E_INVALIDARG,
                             "Argument of type (RingIO_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIODrv_ioctl (CMD_RINGIO_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "RingIO_Params_init");

    return;
}


/* Create a RingIO */
RingIO_Handle
RingIO_create (const Void * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status = 0;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    RingIO_Object *         handle = NULL;
    ClientNotifyMgr_Handle  cliMgrHandle = NULL;
    GateMP_Handle           gateHandle   = NULL;
    Ptr                     sharedAddr   = NULL;
    RingIOShm_Params *      rshmParams   = (RingIOShm_Params *)params;
    RingIODrv_CmdArgs       cmdArgs;
    UInt16                  index;
    ClientNotifyMgr_Params  clientMgrParams;

    GT_1trace (curTrace, GT_ENTER, "RingIO_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL params pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.create.params = (Ptr)rshmParams;
        if (rshmParams->commonParams.name != NULL) {
            cmdArgs.args.create.nameLen = (String_len ((rshmParams->commonParams).name) + 1);
        }
        else {
            cmdArgs.args.create.nameLen = 0;
        }
        ((RingIOShm_Params *)(cmdArgs.args.create.params))->commonParams.name = rshmParams->commonParams.name;
        /* Translate Gate handle to kernel-side gate handle. */
        cmdArgs.args.create.knlLockHandle = (GateMP_Handle)GateMP_getKnlHandle (rshmParams->gateHandle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (rshmParams->ctrlSharedAddr != NULL) {
#endif
            sharedAddr = rshmParams->ctrlSharedAddr;
            index = SharedRegion_getId (sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (index != SharedRegion_INVALIDREGIONID) {
#endif
                cmdArgs.args.create.ctrlSharedAddrSrPtr =
                    SharedRegion_getSRPtr (sharedAddr, index);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (cmdArgs.args.create.ctrlSharedAddrSrPtr == SharedRegion_INVALIDSRPTR) {
                    status = RingIO_E_INVALIDARG;
                }
            }
            else {
                status = RingIO_E_INVALIDARG;
            }
        }
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status >= 0) {
            if (rshmParams->dataSharedAddr != NULL) {
#endif
                sharedAddr = rshmParams->dataSharedAddr;
                index = SharedRegion_getId (sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (index != SharedRegion_INVALIDREGIONID) {
#endif
                    cmdArgs.args.create.dataSharedAddrSrPtr =
                        SharedRegion_getSRPtr (sharedAddr, index);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (cmdArgs.args.create.dataSharedAddrSrPtr == SharedRegion_INVALIDSRPTR) {
                        status = RingIO_E_INVALIDARG;
                    }
                }
                else {
                    status = RingIO_E_INVALIDARG;
                }
            }
        }

        if (status >= 0) {
            if (rshmParams->attrSharedAddr != NULL) {
#endif
                sharedAddr = rshmParams->attrSharedAddr;
                index = SharedRegion_getId (sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (index != SharedRegion_INVALIDREGIONID) {
#endif
                    cmdArgs.args.create.attrSharedAddrSrPtr =
                        SharedRegion_getSRPtr (sharedAddr, index);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (cmdArgs.args.create.attrSharedAddrSrPtr == SharedRegion_INVALIDSRPTR) {
                        status = RingIO_E_INVALIDARG;
                    }
                }
                else {
                    status = RingIO_E_INVALIDARG;
                }
            }
        }

        if (status >= 0) {
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            RingIODrv_ioctl (CMD_RINGIO_CREATE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (    (status < 0)
                 || (cmdArgs.args.create.handle == NULL))    {
                /* @retval  NULL API (through IOCTL) failed on kernel-side */
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "RingIO_create",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the handle */
                handle = (RingIO_Object *) Memory_calloc (NULL,
                                                          sizeof (RingIO_Object),
                                                          0,
                                                          NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    /*! @retval NULL Memory allocation failed for handle */
                    status = RingIO_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "RingIO_create",
                                         status,
                                         "Memory allocation failed for handle!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Set pointer to kernel object into the user handle. */
                    handle->knlObject = (RingIO_Object *)cmdArgs.args.create.handle;
                    handle->createdKnlObject = (RingIO_Object *)cmdArgs.args.create.handle;
                    handle->objMode = (UInt32) -1;
                    handle->created = TRUE;
                    ClientNotifyMgr_Params_init(&clientMgrParams);

                    sharedAddr
                              = SharedRegion_getPtr (cmdArgs.args.create.cliGateSharedMem);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    status =
#endif
                        GateMP_openByAddr (sharedAddr, &gateHandle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_assert (curtrace,(status >= 0));
#endif

                    handle->clientNotifyMgrGate = (Ptr) gateHandle;

                 /*
                  * Pass the shared memory address to create the client
                  * manager instance. This address starts right after the
                  * size of gate peterson from the
                  * params->sharedAddr.
                  */
                  /* Open the ClientNotifyMgr on user side */
                    clientMgrParams.sharedAddr =
                             SharedRegion_getPtr
                        ((cmdArgs.args.create.cliNotifyMgrSharedMem));
                    clientMgrParams.numNotifyEntries =RINGIO_NOTIFYENRIES;
                    clientMgrParams.numSubNotifyEntries =
                                                        RINGIO_SUBNOTIFYENRIES;
                    clientMgrParams.sharedAddrSize =
                                        ClientNotifyMgr_sharedMemReq (
                                        (ClientNotifyMgr_Params*)&clientMgrParams );

                    clientMgrParams.gate = (Ptr) gateHandle;

                    /* Pass the same name for ClientNotifyMgr
                     */
                    if (rshmParams->commonParams.name != NULL) {
                        clientMgrParams.name = rshmParams->commonParams.name;
                    }

                    /* Event no to be used for this instance */
                    // TBD: notifyEventNo not required while opening
                    //clientMgrParams.eventNo = RingIO_state.cfg.notifyEventNo;

                    /* This is actually create call */
                    clientMgrParams.openFlag = TRUE;
					if (SharedRegion_isCacheEnabled(SharedRegion_getId(clientMgrParams.sharedAddr))) {
                        clientMgrParams.cacheFlags =
                                           ClientNotifyMgr_CONTROL_CACHEUSE;
					}
                    cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);
                    GT_assert (curtrace,(NULL != cliMgrHandle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (NULL == cliMgrHandle) {
                        /*! @retval RingIO_E_FAIL_CLIEN0TIFYMGR_CREATE
                         * Failed to create ClientNotifyMgr instance.
                         */
                        status = RingIO_E_FAIL;
                        GT_setFailureReason (
                                         curTrace,
                                         GT_4CLASS,
                                         "_RingIO_create",
                                         status,
                                         "Failed to create ClientNotifyMgr "
                                         "instance." );
                        handle->clientNotifyMgrHandle = NULL;
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        handle->clientNotifyMgrHandle = cliMgrHandle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                 }
             }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        /* To remove compilation warning  */
        RingIO_delete ((RingIO_Handle *)&handle);
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_create", handle);

    return (RingIO_Handle) handle;
}


/* Deletes a instance of RingIO module. */
Int32
RingIO_delete (RingIO_Handle * handlePtr)
{
    Int32                  status = RingIO_S_SUCCESS;
    RingIO_Object        * object = NULL;
    RingIODrv_CmdArgs      cmdArgs;
    ClientNotifyMgr_Handle clientNotifyMgrHandle;


    GT_1trace (curTrace, GT_ENTER, "RingIO_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "Modules is in an invalid state!");
    }
    else if (handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "handlePtr pointer passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else if (((RingIO_Object *)(*handlePtr))->created != TRUE) {
        status = RingIO_E_INVALIDCONTEXT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "*handlePtr passed is not a valid handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (RingIO_Object *)(*handlePtr);
        clientNotifyMgrHandle = object->clientNotifyMgrHandle;
        if (clientNotifyMgrHandle != NULL) {
            ClientNotifyMgr_delete ((ClientNotifyMgr_Handle *)(
                                   &(clientNotifyMgrHandle)));
        }

        if (object->clientNotifyMgrGate != NULL) {
            GateMP_close (&(object->clientNotifyMgrGate));
        }

        cmdArgs.args.deleteRingIO.handle =
                            ((RingIO_Object *)(*handlePtr))->createdKnlObject;
        RingIODrv_ioctl (CMD_RINGIO_DELETE, &cmdArgs);

        Memory_free (NULL, *handlePtr, sizeof (RingIO_Object));
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_delete", status);

    return status;
}


/* Function to open the created RingIO instance.The instance must be
 * created before opening it.
 */
Int32
RingIO_open (      String              name,
             const RingIO_openParams * openParams,
                   UInt16            * procIds,
                   RingIO_Handle     * handlePtr)
{
    Int32             status    = RingIO_S_SUCCESS;
    Int32             tmpStatus = RingIO_S_SUCCESS;
    RingIO_Object   * obj                   = NULL;
    RingIODrv_CmdArgs cmdArgs;
    Ptr               sharedAddr;
    ClientNotifyMgr_Handle cliMgrHandle;
    GateMP_Handle            gateHandle;
    ClientNotifyMgr_Params   clientMgrParams;

    GT_4trace (curTrace,
               GT_ENTER,
               "RingIO_open",
               name,
               openParams,
               procIds,
               handlePtr);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_open",
                             status,
                             "Modules is in an invalid state!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.open.params   = (RingIO_openParams *)openParams;
        cmdArgs.args.open.procIds  = procIds;
        if (name != NULL) {
            cmdArgs.args.open.nameLen = (String_len (name) + 1);
        }
        else {
            cmdArgs.args.open.nameLen = 0;
        }
        cmdArgs.args.open.name  = name;

        status = RingIODrv_ioctl (CMD_RINGIO_OPEN, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* RingIO_E_NOTFOUND is a valid runtime failure. */
        if (   (status < 0)
            && (status != RingIO_E_NOTFOUND)
            && (cmdArgs.args.open.handle == NULL))    {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_open",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif
            if (status >= 0) {
                /* Allocate memory for the handle */
                obj = (RingIO_Object *) Memory_calloc (NULL,
                                                       sizeof (RingIO_Object),
                                                       0,
                                                       NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (obj == NULL) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "RingIO_open",
                                         status,
                                         "Memory allocation for handle failed!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                    (obj)->knlObject = cmdArgs.args.open.handle;
                    (obj)->objMode = openParams->openMode;

                    obj->created = FALSE;

                    //(obj)->remoteProcId =cmdArgs.args.open.remoteProcId;

                    sharedAddr = SharedRegion_getPtr (cmdArgs.args.open.cliGateSharedMem);
                    tmpStatus = GateMP_openByAddr (sharedAddr, &gateHandle);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if ((tmpStatus < 0) || (gateHandle == NULL)) {
                        status = RingIO_E_FAIL;
                        GT_setFailureReason (
                                curTrace,
                                GT_4CLASS,
                                "RingIO_open",
                                RingIO_E_FAIL,
                                "Failed to open the Gate!");
                    }
                    else {
#endif
                        /* Open  the already created clientNotifyMgr
                         * instance
                         */
                        (obj)->clientNotifyMgrGate =  (Ptr)gateHandle;
                        ClientNotifyMgr_Params_init(&clientMgrParams);
                        clientMgrParams.sharedAddr = SharedRegion_getPtr
                            ((cmdArgs.args.open.cliNotifyMgrSharedMem));
                        clientMgrParams.numNotifyEntries = 2 ;
                        clientMgrParams.numSubNotifyEntries = 1;

                        if (name != NULL) {
                            clientMgrParams.name = name;
                        }

                        /*Open the client notify Mgr instace*/
                        clientMgrParams.openFlag = TRUE;
                        clientMgrParams.gate = (Ptr)gateHandle;

                        if (SharedRegion_isCacheEnabled(SharedRegion_getId(clientMgrParams.sharedAddr)) ) {
                            clientMgrParams.cacheFlags =
                                ClientNotifyMgr_CONTROL_CACHEUSE;
                        }
                        cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (cliMgrHandle == NULL) {
                            status = RingIO_E_FAIL;
                            GT_setFailureReason (
                                    curTrace,
                                    GT_4CLASS,
                                    "RingIO_open",
                                    status,
                                    "Failed to open the clientNotifyMgr"
                                    "instance. !");
                            (obj)->clientNotifyMgrHandle = NULL ;
                        }
                        else {
#endif
                            (obj)->clientNotifyMgrHandle = cliMgrHandle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
                    }
#endif

                    if (status >= 0) {
                        *handlePtr = (RingIO_Handle) obj;
                    }
                    else {
                        RingIO_close ((RingIO_Handle *)&obj);
                    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_open", status);

    return status;
}

/* Function to open the created RingIO instance by shared address.The
 * instance must be created before opening it.
 */
Int32
RingIO_openByAddr (
                         Ptr                 ctrlSharedAddr,
                   const RingIO_openParams * params,
                         RingIO_Handle     * handlePtr)
{
    Int32             status    = RingIO_S_SUCCESS;
    Int32             tmpStatus = RingIO_S_SUCCESS;
    RingIO_Object   * obj       = NULL;
    RingIODrv_CmdArgs cmdArgs;
    UInt16            index;
    Ptr               sharedAddr;
    ClientNotifyMgr_Handle   cliMgrHandle;
    GateMP_Handle            gateHandle;
    ClientNotifyMgr_Params   clientMgrParams;

    GT_3trace (curTrace,
               GT_ENTER,
               "RingIO_openByAddr",
               ctrlSharedAddr,
               params,
               handlePtr);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_openByAddr",
                             status,
                             "Modules is in an invalid state!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.openByAddr.params       = (RingIO_openParams *)params;
        if (ctrlSharedAddr != NULL) {
            sharedAddr = ctrlSharedAddr;
            index = SharedRegion_getId (sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (index != SharedRegion_INVALIDREGIONID) {
#endif
                cmdArgs.args.openByAddr.ctrlSharedAddrSrPtr =
                                         (SharedRegion_getSRPtr (sharedAddr,
                                                                 index));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (cmdArgs.args.openByAddr.ctrlSharedAddrSrPtr == SharedRegion_INVALIDSRPTR) {
                    status = RingIO_E_INVALIDARG;
                }
            }
            else {
                status = RingIO_E_INVALIDARG;
            }
#endif
        }

        status = RingIODrv_ioctl (CMD_RINGIO_OPENBYADDR, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (   (status < 0)
            && (cmdArgs.args.openByAddr.handle == NULL))    {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_openByAddr",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Allocate memory for the handle */
        obj = (RingIO_Object *) Memory_calloc (NULL,
                                               sizeof (RingIO_Object),
                                               0,
                                               NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_openByAddr",
                                 status,
                                 "Memory allocation for handle failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            (obj)->knlObject = cmdArgs.args.openByAddr.handle;

            (obj)->objMode = params->openMode;

            sharedAddr = SharedRegion_getPtr (cmdArgs.args.openByAddr.cliGateSharedMem);
            tmpStatus = GateMP_openByAddr (sharedAddr, &gateHandle);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((tmpStatus < 0) || (gateHandle == NULL)) {
                status = RingIO_E_FAIL;
                GT_setFailureReason (
                        curTrace,
                        GT_4CLASS,
                        "RingIO_openByAddr",
                        RingIO_E_FAIL,
                        "Failed to open the gate peterson !");
            }
            else {
#endif
                /* Open  the already created clientNotifyMgr
                 * instance
                 */
                (obj)->clientNotifyMgrGate =  (Ptr)gateHandle;
                ClientNotifyMgr_Params_init(&clientMgrParams);
                clientMgrParams.numNotifyEntries =RINGIO_NOTIFYENRIES;
                clientMgrParams.numSubNotifyEntries = RINGIO_SUBNOTIFYENRIES;
                clientMgrParams.sharedAddr = SharedRegion_getPtr
                    ((cmdArgs.args.openByAddr.cliNotifyMgrSharedMem));

                clientMgrParams.sharedAddrSize = ClientNotifyMgr_sharedMemReq (&clientMgrParams );


                /*Open the client notify Mgr instace*/
                clientMgrParams.openFlag = TRUE;
                clientMgrParams.gate = (Ptr)gateHandle;

				if (SharedRegion_isCacheEnabled(SharedRegion_getId(clientMgrParams.sharedAddr)) ) {
                    clientMgrParams.cacheFlags =
                        ClientNotifyMgr_CONTROL_CACHEUSE;
                }
                cliMgrHandle = ClientNotifyMgr_create (&clientMgrParams);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (cliMgrHandle == NULL) {
                    status = RingIO_E_FAIL;
                    GT_setFailureReason (
                            curTrace,
                            GT_4CLASS,
                            "RingIO_openByAddr",
                            status,
                            "Failed to open the clientNotifyMgr"
                            "instance. !");
                    (obj)->clientNotifyMgrHandle = NULL ;
                }
                else {
#endif
                    (obj)->clientNotifyMgrHandle = cliMgrHandle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
#endif

            if (status >= 0) {
                *handlePtr = (RingIO_Handle) obj;
            }
            else {
                RingIO_close ((RingIO_Handle *) &obj);
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_openByAddr", status);

    return status;
}

/* Function to close the dynamically opened instance */
Int32   RingIO_close (RingIO_Handle * handlePtr)
{
    Int32              status = RingIO_S_SUCCESS;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_close", handlePtr);

    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else if (handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDARG,
                             "Pointer to RingIO Handle passed is null!");
    }
    else if (*handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDARG,
                             "RingIO Handle passed is null!");
    }
    else if (((RingIO_Object *)*handlePtr)->created != FALSE) {
        status = RingIO_E_INVALIDCONTEXT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDCONTEXT,
                             "Handle passed is not a valid handle!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (((RingIO_Object *)(*handlePtr))->clientNotifyMgrGate != NULL) {
            GateMP_close  ((&((RingIO_Object *)(*handlePtr))->clientNotifyMgrGate));
        }
        if (((RingIO_Object *)(*handlePtr))->clientNotifyMgrHandle != NULL) {
            status = ClientNotifyMgr_delete ((ClientNotifyMgr_Handle *)(
                    &(((RingIO_Object *)(*handlePtr))->clientNotifyMgrHandle)));
        }
        cmdArgs.args.close.handle =
                            ((RingIO_Object *)(*handlePtr))->knlObject;
        status = RingIODrv_ioctl (CMD_RINGIO_CLOSE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_close",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
#endif

        Memory_free (NULL, *handlePtr, sizeof (RingIO_Object));
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_0trace (curTrace, GT_LEAVE, "RingIO_close");

    return status;
}


/*
 * Function to register a call back function with the RingIO.
 */
Int32   RingIO_registerNotifier (RingIO_Handle      handle,
                               RingIO_NotifyType  notifyType,
                               UInt32             watermark,
                               RingIO_NotifyFxn   notifyFunc,
                               Ptr                cbContext)
{
    Int32              status = RingIO_S_SUCCESS;
    ClientNotifyMgr_registerNoitifyParams  regParams;
    RingIO_Object      *obj;
    RingIODrv_CmdArgs  cmdArgs;
    UInt32             notifyId;

    GT_5trace (curTrace, GT_ENTER, "RingIO_registerNotifier",
               handle,
               notifyType,
               watermark,
               notifyFunc,
               cbContext);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_registerNotifier",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        obj = (RingIO_Object *) handle;

        regParams.clientHandle   = handle;
        regParams.procId         = MultiProc_self();
        regParams.notifyType     = notifyType;
        regParams.fxnPtr         = (ClientNotifyMgr_FnCbck)notifyFunc;
        regParams.cbContext      = cbContext;
        regParams.watermarkCond1 = watermark;
        regParams.watermarkCond2 = -1;

        if (obj->isRegistered == FALSE) {
            status = ClientNotifyMgr_registerClient(obj->clientNotifyMgrHandle,
                                                &regParams,
                                                &notifyId);
        }
        else {
		    status = RingIO_E_FAIL;
		    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_registerNotifier",
                                 status,
                                 "A notifier function had already been registered.");
		}

        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_registerNotifier",
                                 status,
                                 "ClientNotifyMgr_registerClient failed.!");
        }
        else {
            /* Set notify id in the instance 's kernel object also.*/
            obj->notifyId     = notifyId;
            obj->isRegistered = TRUE;
            cmdArgs.args.setNotifyId.handle   = obj->knlObject;
            cmdArgs.args.setNotifyId.notifyId = notifyId;
            cmdArgs.args.setNotifyId.notifyType = notifyType;
            cmdArgs.args.setNotifyId.watermark  = watermark;
            status = RingIODrv_ioctl (CMD_RINGIO_SET_NOTIFYID, &cmdArgs); // it was CMD_RINGIO_SET_NOTIFYID
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "RingIO_registerNotifier",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "RingIO_registerNotifier");

    return status;
}


/* Function to unregister the call back function.*/
Int32   RingIO_unregisterNotifier (RingIO_Handle      handle)
{
    Int32              status = RingIO_S_SUCCESS;
    RingIO_Object    * obj = NULL;
    RingIODrv_CmdArgs  cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_unregisterNotifier",
               handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_unregisterNotifier",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        obj = (RingIO_Object *) handle;

        status = ClientNotifyMgr_unregisterClient (obj->clientNotifyMgrHandle,
                                                   obj->notifyId);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "RingIO_unregisterNotifier",
                    status,
                    "ClientNotifyMgr_unregisterClient failed.!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->notifyId     = (UInt32)-1;
            obj->isRegistered = FALSE;
            cmdArgs.args.resetNotifyId.handle   = ((RingIO_Object *) handle)->knlObject;
            cmdArgs.args.resetNotifyId.notifyId = -1;
            status = RingIODrv_ioctl (CMD_RINGIO_RESET_NOTIFYID, &cmdArgs); //it was CMD_RINGIO_RESET_NOTIFYID

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "RingIO_unregisterNotifier",
                        status,
                        "API (through IOCTL) failed on kernel-side!");
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "RingIO_unregisterNotifier");

    return status;
}


/*!
 *  @brief      Function to set notification type
 *
 *  @param      handle
 *  @param      notifyType
 *
 */
Int32
RingIO_setNotifyType (RingIO_Handle          handle,
                      RingIO_NotifyType      notifyType)
{
    Int32              status = RingIO_S_SUCCESS;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_setNotifyType", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setNotifyType",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.setNotifyType.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.setNotifyType.notifyType = notifyType;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
            RingIODrv_ioctl (CMD_RINGIO_SETNOTIFYTYPE, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_setNotifyType",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setNotifyType", status);

    return status;
}

/* Returns the current valid data size */
UInt32
RingIO_getValidSize (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32              status   = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32             size     = (UInt32)-1;
    RingIODrv_CmdArgs  cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getValidSize", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getValidSize.handle = ((RingIO_Object *) handle)->knlObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIODrv_ioctl (CMD_RINGIO_GETVALIDSIZE, &cmdArgs);
        size   = cmdArgs.args.getValidSize.size;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            size = (UInt32)-1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getValidSize", size);

    return size;
}


/* Returns the current empty buffer size */
UInt32
RingIO_getEmptySize (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32             status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32            size    = (UInt32)-1;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getEmptySize", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getEmptySize.handle = ((RingIO_Object *) handle)->knlObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIODrv_ioctl (CMD_RINGIO_GETEMPTYSIZE, &cmdArgs);
        size   = cmdArgs.args.getEmptySize.size;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            size = (UInt32)-1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getEmptySize", size);

    return size;
}


/* Returns the current valid attribute size */
UInt32
RingIO_getValidAttrSize (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32              status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32            attrSize = (UInt32)-1;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getValidAttrSize", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getValidAttrSize.handle = ((RingIO_Object *) handle)->knlObject;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
        RingIODrv_ioctl (CMD_RINGIO_GETVALIDATTRSIZE, &cmdArgs);
        attrSize   = cmdArgs.args.getValidAttrSize.attrSize;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            attrSize = (UInt32) -1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getValidAttrSize", attrSize);

    return attrSize;
}


/* Returns the current empty attribute buffer size */
UInt32
RingIO_getEmptyAttrSize (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32              status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32            attrSize = (UInt32)-1;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getEmptyAttrSize", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getEmptyAttrSize.handle = ((RingIO_Object *) handle)->knlObject;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
        RingIODrv_ioctl (CMD_RINGIO_GETEMPTYATTRSIZE, &cmdArgs);
        attrSize = cmdArgs.args.getEmptyAttrSize.attrSize;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            attrSize = (UInt32) -1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getEmptyAttrSize", attrSize);

    return attrSize;
}


/* Returns the current acquire offset for the client */
UInt32
RingIO_getAcquiredOffset (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32              status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32            offset = (UInt32)-1;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getAcquiredOffset", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getAcquiredOffset.handle = ((RingIO_Object *) handle)->knlObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
        RingIODrv_ioctl (CMD_RINGIO_GETACQUIREDOFFSET, &cmdArgs);
        offset = cmdArgs.args.getAcquiredOffset.offset;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            offset = (UInt32)-1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getAcquiredOffset", offset);

    return offset;
}


/* Returns the size of buffer currently acquired */
UInt32
RingIO_getAcquiredSize (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32              status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32            size = (UInt32)-1;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getAcquiredSize", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getAcquiredSize.handle = ((RingIO_Object *) handle)->knlObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
        RingIODrv_ioctl (CMD_RINGIO_GETACQUIREDSIZE, &cmdArgs);
        size = cmdArgs.args.getAcquiredSize.size;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            size = (UInt32) -1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getAcquiredSize", size);

    return size;
}


/* The current watermark level specified by the client */
UInt32
RingIO_getWaterMark (RingIO_Handle   handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32              status = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32            waterMark = (UInt32)-1;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_getWaterMark", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDARG;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.getWaterMark.handle = ((RingIO_Object *) handle)->knlObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
        RingIODrv_ioctl (CMD_RINGIO_GETWATERMARK, &cmdArgs);
        waterMark = cmdArgs.args.getWaterMark.waterMark;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            waterMark = (UInt32) -1;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getWaterMark", waterMark);

    return waterMark;
}


/*
 * Function to set watermark
 */
Int32
RingIO_setWaterMark (RingIO_Handle   handle,
                     UInt32          watermark)
{
    Int32             status = RingIO_S_SUCCESS;
    RingIODrv_CmdArgs cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "RingIO_setWaterMark", handle, watermark);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.setWaterMark.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.setWaterMark.waterMark = watermark;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
        RingIODrv_ioctl (CMD_RINGIO_SETWATERMARK, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setWaterMark", status);

    return status;
}

/* Acquire a buffer from the RingIO instance
 * This function acquires a data buffer from RingIO for reading or
 * writing, depending on the mode in which the client (represented
 * by the handle) has been opened.
 */
Int32
RingIO_acquire (RingIO_Handle   handle,
                RingIO_BufPtr * pData,
                UInt32 *        pSize)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_3trace (curTrace, GT_ENTER, "RingIO_acquire", handle, pData, pSize);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_acquire",
                             RingIO_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.acquire.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.acquire.pData  = pData;
        cmdArgs.args.acquire.pSize  = pSize;
        status = RingIODrv_ioctl (CMD_RINGIO_ACQUIRE, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((status < 0) && (status != RingIO_E_BUFWRAP) &&
           (status != RingIO_E_BUFFULL)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_acquire",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        *pData = Memory_translate((Ptr)cmdArgs.args.acquire.pData,
                                  Memory_XltFlags_Phys2Virt);
        pSize = cmdArgs.args.acquire.pSize;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_acquire", status);

    return status;
}


/* Release a buffer to the RingIO instance
 * This function releases a data buffer to RingIO.
 * This function releases an acquired buffer or part of it.
 */
Int32
RingIO_release (RingIO_Handle   handle,
                UInt32          size)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "RingIO_release", handle, size);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.release.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.release.size   = size;
        status = RingIODrv_ioctl (CMD_RINGIO_RELEASE, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_release", status);

    return status;
}



/* Cancel the previous acquired buffer to the RingIO instance
 * This function cancels any data buffers acquired by reader or writer.
 * In the case of writer, all attributes that are set since the
 * first acquire are removed. In the case of reader, all attributes that
 * were obtained since the first acquired are re-instated in the
 * RingIO instance.
 */
Int32
RingIO_cancel (RingIO_Handle   handle)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIO_cancel", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.cancel.handle = ((RingIO_Object *) handle)->knlObject;
        status = RingIODrv_ioctl (CMD_RINGIO_CANCEL, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_cancel", status);

    return status;
}


/*!
 *  Get attribute from the RingIO instance
 */
Int32
RingIO_getAttribute (RingIO_Handle handle,
                      UInt16 *      type,
                      UInt32 *      param)
{
    Int32             status = 0;

    GT_3trace (curTrace, GT_ENTER, "RingIO_getAttribute",
               handle,
               type,
               param
               );

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    status = (RingIO_getvAttribute (handle, type, param, NULL, NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getAttribute", status);

    return status;
}



/* Get a variable sized attribute
 * This function gets an attribute with a variable-sized payload from
 * the attribute buffer.
 * If an attribute is present, the attribute type, the optional
 * parameter, a pointer to the optional payload and the payload
 * size are returned.
 */
Int32
RingIO_getvAttribute (RingIO_Handle handle,
                      UInt16 *      type,
                      UInt32 *      param,
                      RingIO_BufPtr vptr,
                      UInt32 *      pSize)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_5trace (curTrace, GT_ENTER, "RingIO_getvAttribute",
               handle,
               type,
               param,
               vptr,
               pSize);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.acquire.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.getvAttribute.type   = type;
        cmdArgs.args.getvAttribute.param  = param;
        cmdArgs.args.getvAttribute.vptr   = vptr;
        cmdArgs.args.getvAttribute.pSize   = pSize;
        status = RingIODrv_ioctl (CMD_RINGIO_GETVATTRIBUTE, &cmdArgs);
        type   = cmdArgs.args.getvAttribute.type;
        param  = cmdArgs.args.getvAttribute.param;
        pSize  = cmdArgs.args.getvAttribute.pSize;
        vptr   = cmdArgs.args.getvAttribute.vptr;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getvAttribute", status);

    return status;
}


/*!
 *  Set attribute in the RingIO instance
 *
 */
Int32
RingIO_setAttribute (RingIO_Handle handle,
                      UInt16        type,
                      UInt32        param,
                      Bool          sendNotification)
{
    Int32             status = 0;

    GT_3trace (curTrace, GT_ENTER, "RingIO_setAttribute",
               handle,
               type,
               param
               );

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        status =
            RingIO_setvAttribute (handle,
                    type,
                    param,
                    NULL,
                    0,
                    sendNotification);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setAttribute", status);

    return status;
}


/* This function sets an attribute with a variable sized payload at the
 * offset 0 provided in the acquired data buffer.
 * This function sets an attribute with a variable sized payload at
 * the offset provided in the acquired data buffer.
 * If the offset is not in the range of the acquired data buffer,
 * the attribute is not set, and an error is returned. One exception
 * to this rule is when no data buffer has been acquired. In this
 * case an attribute is set at the next data buffer offset that can be
 * acquired
 */
Int32
RingIO_setvAttribute (RingIO_Handle handle,
                      UInt16        type,
                      UInt32        param,
                      RingIO_BufPtr pData,
                      UInt32        size,
                      Bool          sendNotification)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_5trace (curTrace, GT_ENTER, "RingIO_setvAttribute",
               handle,
               type,
               param,
               pData,
               size);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.acquire.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.setvAttribute.type   = type;
        cmdArgs.args.setvAttribute.param  = param;
        cmdArgs.args.setvAttribute.pData  = pData;
        cmdArgs.args.setvAttribute.size   = size;
        cmdArgs.args.setvAttribute.sendNotification = sendNotification;
        status = RingIODrv_ioctl (CMD_RINGIO_SETVATTRIBUTE, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setvAttribute", status);

    return status;
}


/* Flushes the data buffer
 * This function flushes the data buffer.
 * This function is used to flush the data from the RingIO.
 * Behavior of this function depends on the value of hardFlush
 * argument
 * When hardFlush is false:
 * If function is called for the writer, all the valid data in
 * buffer after the first attribute location will be discarded. In
 * case there are no attributes, no data will be cleared from the buffer.
 * Note that this does not include the data that has been already
 * acquired by the reader. Note that the attribute will also be
 * cleared from the attribute buffer.
 * For the reader, all the data till the next attribute location will
 * be discarded. And if there is no attribute in the buffer, all valid
 * data will get discarded. Note that the attribute will remain the
 * attribute buffer. This is different from the behavior mentioned for
 * the writer.
 * When hardFlush is true:
 * If function is called from the writer, all committed data and
 * attributes that is not acquired by reader are removed from
 * the RingIO instance. The writer pointer is moved to point to
 * reader's head pointer If function is called from the reader, all
 * data and attributes that can be subsequently acquired from the
 * reader are removed.
 */
Int32
RingIO_flush (RingIO_Handle handle,
              Bool          hardFlush,
              UInt16 *      type,
              UInt32 *      param,
              UInt32 *      bytesFlushed)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_5trace (curTrace, GT_ENTER, "RingIO_flush",
               handle,
               hardFlush,
               type,
               param,
               bytesFlushed);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.flush.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.flush.hardFlush = hardFlush;
        cmdArgs.args.flush.type  = type;
        cmdArgs.args.flush.param = param;
        cmdArgs.args.flush.bytesFlushed = bytesFlushed;
        status = RingIODrv_ioctl (CMD_RINGIO_FLUSH, &cmdArgs);
        type   = cmdArgs.args.flush.type;
        param  = cmdArgs.args.flush.param;
        bytesFlushed  = cmdArgs.args.flush.bytesFlushed;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_flush", status);

    return status;
}


/*
 *  Send a notification to the other client manually
 */
Int32
RingIO_sendNotify (RingIO_Handle    handle,
               RingIO_NotifyMsg msg)
{
    Int32             status = 0;
    RingIODrv_CmdArgs cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "RingIO_sendNotify",
               handle,
               msg);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIO_state.setupRefCount == 0) {
        status = RingIO_E_INVALIDSTATE;
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        cmdArgs.args.notify.handle = ((RingIO_Object *) handle)->knlObject;
        cmdArgs.args.notify.msg     = msg;

        status = ClientNotifyMgr_sendForceNotificationMulti
                                                  (((RingIO_Object *)handle)->clientNotifyMgrHandle,
                                                  ((RingIO_Object *)handle)->notifyId,
                                                   msg);



#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_sendNotify", status);

    return status;
}


/* Get shared memory requirements */
UInt32
RingIO_sharedMemReq (const Void *                        params,
                           RingIO_sharedMemReqDetails * sharedMemReqDetails)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                status    = RingIO_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    UInt32             totalSize = 0;
    RingIODrv_CmdArgs  cmdArgs;

    GT_2trace (curTrace,
               GT_ENTER,
               "RingIO_sharedMemReq",
               params,
               sharedMemReqDetails);

        cmdArgs.args.sharedMemReq.params = (Ptr)params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIODrv_ioctl (CMD_RINGIO_SHAREDMEMREQ, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "RingIO_sharedMemReq",
                    status,
                    "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            totalSize = cmdArgs.args.sharedMemReq.bytes;
            if (sharedMemReqDetails != NULL) {
                sharedMemReqDetails->ctrlSharedMemReq =
                    cmdArgs.args.sharedMemReq.ctrlSharedMemReq;
                sharedMemReqDetails->dataSharedMemReq =
                    cmdArgs.args.sharedMemReq.dataSharedMemReq;
                sharedMemReqDetails->attrSharedMemReq =
                    cmdArgs.args.sharedMemReq.attrSharedMemReq;
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,
               GT_LEAVE,
               "RingIO_sharedMemReq",
               totalSize);

    return (totalSize);
}
