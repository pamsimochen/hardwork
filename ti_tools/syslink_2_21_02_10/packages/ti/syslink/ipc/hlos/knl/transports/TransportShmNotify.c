/*
 *  @file   TransportShmNotify.c
 *
 *  @brief      MessageQ transport that uses Notify directly
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
#include <ti/syslink/utils/String.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/inc/_Ipc.h>

/* Module level headers */
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/syslink/inc/TransportShmNotify.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*!
 *  @brief  Macro to make a correct module magic number with refCount
 */
#define TransportShmNotify_MAKE_MAGICSTAMP(x)                                \
                                  ((TransportShmNotify_MODULEID << 12u) | (x))

#define TransportShmNotify_RESERVED_EVENTNO  (2u)

#define TransportShmNotify_notifyEventId (UInt32)TransportShmNotify_RESERVED_EVENTNO + \
                                                (UInt32)(Notify_SYSTEMKEY << 16)

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief   Defines the TransportShmNotify state object, which contains all
 *           the module specific information.
 */
typedef struct TransportShmNotify_ModuleObject_tag {
    Atomic                         refCount;
    /* Flag to indicate initialization state of GP */
    TransportShmNotify_Config      cfg;
    /*!< TransportShmNotify configuration structure */
    TransportShmNotify_Config      defCfg;
    /*!< Default module configuration */
    TransportShmNotify_Params      defInstParams;
    /*!< Default instance parameters */
    TransportShmNotify_Handle      transports
                      [MultiProc_MAXPROCESSORS][MessageQ_NUM_PRIORITY_QUEUES];
    /*!< Transport to be set in MessageQ_registerTransport */
    TransportShmNotify_ErrFxn      errFxn;
    /*!< Error function */
} TransportShmNotify_ModuleObject;

/*!
 *  @brief  Structure defining config parameters for the MessageQ transport
 *  instances.
 */
typedef struct TransportShmNotify_Obj_tag {
    UInt16                        remoteProcId;
    /* dst proc id               */
    UInt16                        priority;
    /*!<  Priority of messages supported by this transport */
} TransportShmNotify_Obj;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    TransportShmNotify_state
 *
 *  @brief  TransportShmNotify state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmNotify_ModuleObject TransportShmNotify_state =
{
    .defCfg.errFxn = NULL,
    .defCfg.notifyEventId = TransportShmNotify_RESERVED_EVENTNO,
    .defInstParams.priority = MessageQ_NORMALPRI
};

/*!
 *  @var    TransportShmNotify_module
 *
 *  @brief  Pointer to the TransportShmNotify module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmNotify_ModuleObject * TransportShmNotify_module =
                                                &TransportShmNotify_state;


/* =============================================================================
 * Forward declarations of internal functions
 * =============================================================================
 */
/* Callback function registered with the Notify module. */
Void _TransportShmNotify_notifyFxn (UInt16  procId,
                              UInt16  lineId,
                              UInt32  eventId,
                              Ptr     arg,
                              UInt32  payload);


/* =============================================================================
 * APIs called by internal IPC setup module
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the TransportShmNotify
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to TransportShmNotify_setup filled in
 *              by the TransportShmNotify module with the default parameters.
 *              If the user does not wish to make any change in the default
 *              parameters, this API is not required to be called.
 *
 *  @param      cfg        Pointer to the TransportShmNotify module
 *                         configuration structure in which the default config
 *                         is to be returned.
 *
 *  @sa         TransportShmNotify_setup
 */
Void
TransportShmNotify_getConfig (TransportShmNotify_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "TransportShmNotify_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_getConfig",
                             MessageQ_E_INVALIDARG,
                             "Argument of type (TransportShmNotify_Config *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(TransportShmNotify_module->refCount),
                                      TransportShmNotify_MAKE_MAGICSTAMP(0),
                                      TransportShmNotify_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfg,
                         &(TransportShmNotify_module->defCfg),
                         sizeof (TransportShmNotify_Config));
        }
        else {
            Memory_copy (cfg,
                         &(TransportShmNotify_module->cfg),
                         sizeof (TransportShmNotify_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "TransportShmNotify_getConfig");
}


/*!
 *  @brief      Setup the TransportShmNotify module.
 *
 *              This function sets up the TransportShmNotify module. This
 *              function must be called before any other instance-level APIs can
 *              be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then TransportShmNotify_getConfig can be called to
 *              get the configuration filled with the default values. After
 *              this, only the required configuration values can be changed. If
 *              the user does not wish to make any change in the default
 *              parameters, the application can simply call
 *              TransportShmNotify_setup with NULL parameters. The default
 *              parameters would get automatically used.
 *
 *  @param      cfg   Optional TransportShmNotify module configuration.
 *                    If provided as NULL, default configuration is used.
 *
 *  @sa         TransportShmNotify_destroy
 *              GateMutex_create
 */
Int
TransportShmNotify_setup (const TransportShmNotify_Config * cfg)
{
    Int                       status = MessageQ_S_SUCCESS;
    TransportShmNotify_Config tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "TransportShmNotify_setup", cfg);

    if (cfg == NULL) {
        TransportShmNotify_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&TransportShmNotify_module->refCount,
                            TransportShmNotify_MAKE_MAGICSTAMP(0),
                            TransportShmNotify_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&TransportShmNotify_module->refCount)
        != TransportShmNotify_MAKE_MAGICSTAMP(1u)) {
        status = MessageQ_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "MessageQ Module already initialized!");
    }
    else {
        /* Copy the user provided values into the state object. */
        Memory_copy (&TransportShmNotify_module->cfg,
                     (TransportShmNotify_Config *) cfg,
                     sizeof (TransportShmNotify_Config));
        Memory_set (&(TransportShmNotify_module->transports),
                    0,
                    (   sizeof (TransportShmNotify_Handle)
                     *  MultiProc_MAXPROCESSORS
                     *  MessageQ_NUM_PRIORITY_QUEUES));
    }

    GT_1trace (curTrace, GT_LEAVE, "TransportShmNotify_setup", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the TransportShmNotify module.
 *
 *              Once this function is called, other TransportShmNotify module
 *              APIs, except for the TransportShmNotify_getConfig API cannot
 *              be called anymore.
 *
 *  @sa         TransportShmNotify_setup
 *              GateMutex_delete
 */
Int
TransportShmNotify_destroy (Void)
{
    Int    status = MessageQ_S_SUCCESS;
    UInt16 numProcs = MultiProc_getNumProcessors ();
    UInt16 i;
    UInt16 j;

    GT_0trace (curTrace, GT_ENTER, "TransportShmNotify_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmNotify_module->refCount),
                                  TransportShmNotify_MAKE_MAGICSTAMP(0),
                                  TransportShmNotify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized */
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&TransportShmNotify_module->refCount)
            == TransportShmNotify_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&TransportShmNotify_module->refCount,
                        TransportShmNotify_MAKE_MAGICSTAMP(1));

            /* Delete/close any Transports that have not been deleted/closed so
             * far.
             */
            for (i = 0; i < numProcs; i++) {
                for (j = 0 ; j < MessageQ_NUM_PRIORITY_QUEUES; j++) {
                    GT_assert (curTrace,
                       (TransportShmNotify_module->transports [i][j] == NULL));
                    if (TransportShmNotify_module->transports [i][j] != NULL) {
                        TransportShmNotify_delete (
                               &(TransportShmNotify_module->transports [i][j]));
                    }
                }
            }

            /* Decrease the refCount */
            Atomic_set (&TransportShmNotify_module->refCount,
                        TransportShmNotify_MAKE_MAGICSTAMP(0));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmNotify_destroy", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Get Instance parameters
 *
 *  @param      handle          Handle to TransportShmNotify instance
 *  @param      params          Instance creation parameters
 *
 *  @sa         TransportShmNotify_create
 */
Void
TransportShmNotify_Params_init (TransportShmNotify_Params  * params)
{
     GT_1trace (curTrace, GT_ENTER, "TransportShmNotify_Params_init", params);

     GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmNotify_module->refCount),
                                  TransportShmNotify_MAKE_MAGICSTAMP(0),
                                  TransportShmNotify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_Params_init",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
         /*! @retval None */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "TransportShmNotify_Params_init",
                              MessageQ_E_INVALIDARG,
                              "Argument of type "
                              "(TransportShmNotify_Params *) is "
                              "NULL!");
     }
     else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(TransportShmNotify_module->defInstParams),
                     sizeof (TransportShmNotify_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
     }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

     GT_0trace (curTrace, GT_LEAVE, "TransportShmNotify_Params_init");
}


/*
 *  @brief    Create a transport instance.
 *
 *  @param    procId     Remote processor ID
 *  @param    params     Instance creation parameters
 *
 *  @sa       TransportShmNotify_delete
 *            Notify_registerEvent
 */
TransportShmNotify_Handle
TransportShmNotify_create (      UInt16                      procId,
                           const TransportShmNotify_Params * params)
{
    Int                         status = MessageQ_S_SUCCESS;
    TransportShmNotify_Object * handle = NULL;
    TransportShmNotify_Obj *    obj    = NULL;
    TransportShmNotify_Params   tmpParams;

    GT_2trace (curTrace, GT_ENTER, "TransportShmNotify_create", procId, params);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));

    if (params == NULL) {
        TransportShmNotify_Params_init (&tmpParams);
        params = &tmpParams;
    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmNotify_module->refCount),
                                  TransportShmNotify_MAKE_MAGICSTAMP(0),
                                  TransportShmNotify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_create",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (   TransportShmNotify_module->transports [procId][params->priority]
             != NULL) {
        /*! @retval MessageQ_E_ALREADYEXISTS A transport for specified procId
                               with specified params->priority already exists */
        status = MessageQ_E_ALREADYEXISTS;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_create",
                             status,
                             "A transport for specified procId with specified"
                             " params->priority already exists!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Create the generic handle */
        handle = (TransportShmNotify_Object *) Memory_calloc (NULL,
                                             sizeof (TransportShmNotify_Object),
                                             0u,
                                             NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle == NULL) {
            /*! @retval MessageQ_E_MEMORY Memory allocation failed for pointer
             *          of type TransportShmNotify_Object
             */
            status = MessageQ_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmNotify_create",
                                 status,
                                 "Memory allocation failed for pointer"
                                 " of type TransportShmNotify_Object!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj = (TransportShmNotify_Obj *) Memory_calloc (NULL,
                                               sizeof (TransportShmNotify_Obj),
                                               0,
                                               NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj == NULL) {
                /*! @retval MessageQ_E_MEMORY Memory allocation failed for pointer
                 *          of type TransportShmNotify_Obj
                 */
                status = MessageQ_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmNotify_create",
                                     status,
                                     "Memory allocation failed for pointer"
                                     " of type TransportShmNotify_Obj");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                handle->obj = obj ;
                obj->priority = params->priority;
                obj->remoteProcId  = procId;
                handle->put = (IMessageQTransport_putFxn)
                                            &TransportShmNotify_put;
                handle->getStatus = (IMessageQTransport_getStatusFxn)
                                                &TransportShmNotify_getStatus;
                handle->controlFn = (IMessageQTransport_controlFxn)
                                                    &TransportShmNotify_control;

                status = Notify_registerEventSingle (procId,
                            0, /* lineId */
                            TransportShmNotify_notifyEventId,
                            (Notify_FnNotifyCbck) _TransportShmNotify_notifyFxn,
                            NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override the status with MessageQ status */
                    status = MessageQ_E_FAIL;
                    /*! @retval MessageQ_E_FAIL Notify_registerEventSingle
                                                failed */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShmNotify_create",
                                         status,
                                         "Notify_registerEventSingle failed!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Register the transport with MessageQ */
                    status = MessageQ_registerTransport (
                                            (IMessageQTransport_Handle) handle,
                                            procId,
                                            obj->priority);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShmNotify_create",
                                         status,
                                         "MessageQ_registerTransport failed!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Set handle in the local array. */
                        TransportShmNotify_module->transports
                                   [obj->remoteProcId][obj->priority] =
                                            (TransportShmNotify_Handle) handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            /* Cleanup in case of error. */
            if (status < 0) {
                TransportShmNotify_delete ((TransportShmNotify_Handle *)
                                                                &handle);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmNotify_create", handle);

    /*! @retval Handle Operation successful */
    return (TransportShmNotify_Handle) handle;
}


/*
 *  @brief    Delete instance
 *
 *  @param    handlePtr  Pointer to instance handle to be deleted
 *
 *  @sa       TransportShmNotify_create
 *            ListMP_delete
 *            ListMP_close
 *            Notify_unregisterEvent
 */
Int
TransportShmNotify_delete (TransportShmNotify_Handle * handlePtr)
{
    Int                         status = MessageQ_S_SUCCESS;
    TransportShmNotify_Object * handle;
    TransportShmNotify_Obj *    obj;

    GT_1trace (curTrace, GT_ENTER, "TransportShmNotify_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmNotify_module->refCount),
                                  TransportShmNotify_MAKE_MAGICSTAMP(0),
                                  TransportShmNotify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not
         * initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handlePtr
                                                      pointer specified*/
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL *handlePtr specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (TransportShmNotify_Object *) (*handlePtr);
        obj    = (TransportShmNotify_Obj *) handle->obj;

        if (obj != NULL) {
            /* Clear handle in the local array. */
            TransportShmNotify_module->transports [obj->remoteProcId]
                                                  [obj->priority] = NULL;

            MessageQ_unregisterTransport (obj->remoteProcId, obj->priority);

            status = Notify_unregisterEventSingle (obj->remoteProcId,
                                              0,
                                              TransportShmNotify_notifyEventId);
            if (status < 0) {
                /*! @retval MessageQ_E_FAIL Failed to unregister Notify event */
                /* Override the status with a MessageQ status code. */
                status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmNotify_delete",
                                     status,
                                     "Failed to unregister Notify event");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            Memory_free (NULL, obj, sizeof (TransportShmNotify_Obj));
        }

        Memory_free (NULL, handle, sizeof (TransportShmNotify_Object));
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmNotify_delete", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief     Put msg to remote MessageQ
 *
 * @param     obj        Handle to TransportShmNotify instance
 * @param     msg        Message to be delivered to remote MessageQ
 *
 * @sa        Notify_sendEvent
 *            ListMP_putTail
 */
Int
TransportShmNotify_put (TransportShmNotify_Handle   handle,
                        Ptr                         msg)
{
    Int                      status = MessageQ_S_SUCCESS;
    TransportShmNotify_Obj * obj = (TransportShmNotify_Obj *) handle;
    SharedRegion_SRPtr       msgSRPtr;
    UInt16                   regionId;

    GT_2trace (curTrace, GT_ENTER, "TransportShmNotify_put", handle, msg);

    GT_assert (curTrace, (msg != NULL));
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmNotify_module->refCount),
                                  TransportShmNotify_MAKE_MAGICSTAMP(0),
                                  TransportShmNotify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_put",
                             status,
                             "Module was not initialized!");
    }
    else if (msg == NULL) {
        /*! @retval MessageQ_E_INVALIDARG
         *          Invalid NULL msg pointer specified
         */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_put",
                             status,
                             "Invalid NULL msg pointer specified");
    }
    else if (handle == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handle specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmNotify_put",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (TransportShmNotify_Obj *)
                                ((TransportShmNotify_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        regionId = SharedRegion_getId (msg);
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionId >= SharedRegion_getNumRegions()) {
            /*! @retval MessageQ_E_INVALIDARG Invalid messgae passed */
            status = MessageQ_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmNotify_put",
                                 status,
                                 "msg contains invalid sharedregion id");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* writeback invalidate the message */
            if (EXPECT_FALSE (SharedRegion_isCacheEnabled(regionId))) {
                Cache_wbInv ((Ptr) msg,
                             ((MessageQ_Msg)(msg))->msgSize,
                             Cache_Type_ALL,
                             TRUE);
            }

            /*
             *  Get the msg's SRPtr.
             */
            msgSRPtr = SharedRegion_getSRPtr (msg, regionId);

            status = Notify_sendEvent (obj->remoteProcId,
                                       0,
                                       TransportShmNotify_notifyEventId,
                                       (UInt32)msgSRPtr,
                                       TRUE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                /* Override status with MessageQ status code. */
                status = MessageQ_E_TIMEOUT;
                GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "TransportShmNotify_put",
                                    status,
                                    "Notification to remote processor failed!");
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmNotify_put", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief     Control Function
 *
 * @param     obj        Handle to TransportShmNotify instance
 * @param     cmd        cmd ID
 * @param     cmdArg     Argument specific to cmd ID
 *
 * @sa        None
 */
Int
TransportShmNotify_control (TransportShmNotify_Handle handle,
                            UInt                      cmd,
                            UArg                      cmdArg)
{
    GT_3trace (curTrace, GT_ENTER, "TransportShmNotify_control",
                                   handle, cmd, cmdArg);

    GT_assert (curTrace, (handle != NULL));

    GT_1trace (curTrace, GT_LEAVE, "TransportShmNotify_control",
               MessageQ_E_INVALIDARG);

    /*! @retval MessageQ_E_INVALIDARG Specified operation is not supported. */
    return (MessageQ_E_INVALIDARG);
}


/*
 * @brief     Get status
 *
 * @param     obj        Handle to TransportShmNotify instance
 *
 * @sa        None
 */
Int
TransportShmNotify_getStatus (TransportShmNotify_Handle handle)
{
    GT_1trace (curTrace, GT_ENTER, "TransportShmNotify_getStatus", handle);

    GT_assert (curTrace, (handle != NULL));

    GT_1trace (curTrace,
               GT_LEAVE,
               "TransportShmNotify_getStatus",
               0);

    /*! @retval Status-of-MessageQ-Transport Operation successful. */
    return (0);
}


/*
 * @brief     Set the asynchronous error function for the transport module
 *
 * @param     errFxn     Error function to be set
 *
 * @sa        None
 */
Void
TransportShmNotify_setErrFxn (TransportShmNotify_ErrFxn errFxn)
{
    /* Ignore errFxn */
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*!
 *  @brief      Callback function registered with the Notify module.
 *
 *  @param      procId     Destination processor id for notification
 *  @param      lineId     Interrupt line number
 *  @param      eventId    Event number for notification
 *  @param      arg        Argument
 *  @param      payload    32 bit payload
 *
 *  @sa         ListMP_getHead
 *              MessageQ_put
 *              MessageQ_getDstQueue
 */
Void
_TransportShmNotify_notifyFxn (UInt16  procId,
                               UInt16  lineId,
                               UInt32  eventId,
                               Ptr     arg,
                               UInt32  payload)
{
    MessageQ_Msg msg;
    UInt32       queueId;

    GT_5trace (curTrace, GT_ENTER, "_TransportShmNotify_notifyFxn",
                                    procId,
                                    lineId,
                                    eventId,
                                    arg,
                                    payload);

    /* The Notify payload is the SRPtr for the Message */
    msg = SharedRegion_getPtr ((SharedRegion_SRPtr) payload);

    /* Get the destination message queue Id */
    queueId = MessageQ_getDstQueue (msg);

    /* Put the message to the destination queue */
    MessageQ_put (queueId, msg);

    GT_0trace (curTrace, GT_LEAVE, "_TransportShmNotify_notifyFxn");
}
