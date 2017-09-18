/*
 *  @file   TransportShm.c
 *
 *  @brief      MessageQ transport
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
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/inc/_Ipc.h>

/* Module level headers */
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/GatePeterson.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/TransportShm.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*!
 *  @brief  Indicates that the transport is up.
 */
#define TransportShm_UP          0xBADC0FFE

/*!
 *  @brief  Macro to make a correct module magic number with refCount
 */
#define TransportShm_MAKE_MAGICSTAMP(x)                                \
                                  ((TransportShm_MODULEID << 12u) | (x))

#define TransportShm_RESERVED_EVENTNO  (2u)

#define TransportShm_notifyEventId (UInt32)TransportShm_RESERVED_EVENTNO + \
                                                (UInt32)(Notify_SYSTEMKEY << 16)
/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief   Defines the TransportShm state object, which contains all
 *           the module specific information.
 */
typedef struct TransportShm_ModuleObject_tag {
    Atomic                   refCount;
    /* Flag to indicate initialization state of GP */
    TransportShm_Config      cfg;
    /*!< TransportShm configuration structure */
    TransportShm_Config      defCfg;
    /*!< Default module configuration */
    TransportShm_Params      defInstParams;
    /*!< Default instance parameters */
    IGateProvider_Handle     gateHandle;
    /*!< Handle to the gate for local thread safety */
    TransportShm_Handle      transports
                      [MultiProc_MAXPROCESSORS][MessageQ_NUM_PRIORITY_QUEUES];
    /*!< Transport to be set in MessageQ_registerTransport */
    TransportShm_ErrFxn      errFxn;
    /*!< Error function */
} TransportShm_ModuleObject;


/*!
 *  @brief  Structure of attributes in shared memory
 */
typedef struct TransportShm_Attrs_tag {
    volatile Bits32 flag;
    /*!<  Flag */
    volatile Bits32 creatorProcId;
    /*!< Creator Processor Id */
    volatile Bits32 notifyEventId;
    /*!< Notify Event Number */
    volatile Bits16 priority;
    /*!< Flag */
    volatile SharedRegion_SRPtr gateMPAddr;
    /*!< Gate MP address */
} TransportShm_Attrs;

/*!
 *  @brief  Structure defining config parameters for the MessageQ transport
 *  instances.
 */
typedef struct TransportShm_Obj_tag {
    volatile TransportShm_Attrs  * self;
    /* Attrs in shared memory        */
    volatile TransportShm_Attrs  * other;
    /* Remote ShmFlag            */
    ListMP_Handle                 localList;
    /* List for this processor        */
    ListMP_Handle                 remoteList;
    /* List for remote processor      */
    volatile Int                  status;
    /* Current status                 */
    /* 1 | 0                         */
    SizeT                         allocSize;
    /* Shared memory allocated       */
    Bool                          cacheEnabled;
    /* Whether to do cache calls */
    UInt16                        regionId;
    /* The shared region id          */
    UInt16                        remoteProcId;
    /* dst proc id               */
    UInt16                        priority;
    /*!<  Priority of messages supported by this transport */
    GateMP_Handle                 gate;
    /* Gate for critical regions     */
    TransportShm_Params           params;
    /* Instance specific parameters  */
} TransportShm_Obj;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    TransportShm_state
 *
 *  @brief  TransportShm state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShm_ModuleObject TransportShm_state =
{
    .gateHandle = NULL,
    .defCfg.errFxn = NULL,
    .defCfg.notifyEventId = TransportShm_RESERVED_EVENTNO,
    .defInstParams.gate = NULL,
    .defInstParams.sharedAddr = NULL,
    .defInstParams.priority = MessageQ_NORMALPRI
};

/*!
 *  @var    TransportShm_module
 *
 *  @brief  Pointer to the TransportShm module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShm_ModuleObject * TransportShm_module = &TransportShm_state;


/* =============================================================================
 * Forward declarations of internal functions
 * =============================================================================
 */
/* Callback function registered with the Notify module. */
Void _TransportShm_notifyFxn (UInt16  procId,
                              UInt16  lineId,
                              UInt32  eventId,
                              Ptr     arg,
                              UInt32  payload);
/* Function to create/open the handle. */
Int
_TransportShm_create (      TransportShm_Handle *     handlePtr,
                            UInt16                    procId,
                      const TransportShm_Params     * params,
                            Bool                      createFlag);


/* =============================================================================
 * APIs called directly by applications
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the TransportShm
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to TransportShm_setup filled in
 *              by the TransportShm module with the default parameters.
 *              If the user does not wish to make any change in the default
 *              parameters, this API is not required to be called.
 *
 *  @param      cfg        Pointer to the TransportShm module
 *                         configuration structure in which the default config
 *                         is to be returned.
 *
 *  @sa         TransportShm_setup
 */
Void
TransportShm_getConfig (TransportShm_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "TransportShm_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_getConfig",
                             MessageQ_E_INVALIDARG,
                             "Argument of type (TransportShm_Config *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                      TransportShm_MAKE_MAGICSTAMP(0),
                                      TransportShm_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfg,
                         &(TransportShm_module->defCfg),
                         sizeof (TransportShm_Config));
        }
        else {
            Memory_copy (cfg,
                         &(TransportShm_module->cfg),
                         sizeof (TransportShm_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "TransportShm_getConfig");
}


/*!
 *  @brief      Setup the TransportShm module.
 *
 *              This function sets up the TransportShm module. This
 *              function must be called before any other instance-level APIs can
 *              be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then TransportShm_getConfig can be called to
 *              get the configuration filled with the default values. After
 *              this, only the required configuration values can be changed. If
 *              the user does not wish to make any change in the default
 *              parameters, the application can simply call
 *              TransportShm_setup with NULL parameters. The default
 *              parameters would get automatically used.
 *
 *  @param      cfg   Optional TransportShm module configuration.
 *                    If provided as NULL, default configuration is used.
 *
 *  @sa         TransportShm_destroy
 *              GateMutex_create
 */
Int
TransportShm_setup (const TransportShm_Config * cfg)
{
    Int                 status = MessageQ_S_SUCCESS;
    Error_Block         eb;
    TransportShm_Config tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "TransportShm_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        TransportShm_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (cfg == NULL) {
        TransportShm_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&TransportShm_module->refCount,
                            TransportShm_MAKE_MAGICSTAMP(0),
                            TransportShm_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&TransportShm_module->refCount)
        != TransportShm_MAKE_MAGICSTAMP(1u)) {
        status = MessageQ_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "MessageQ Module already initialized!");
    }
    else {
        /* Create a default gate handle for module level local protection. */
        TransportShm_module->gateHandle = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (TransportShm_module->gateHandle == NULL) {
            /*! @retval MessageQ_E_FAIL Failed to create GateMutex!
             */
            status = MessageQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShm_setup",
                                 status,
                                 "Failed to create GateMutex!");
            Atomic_set (&TransportShm_module->refCount,
                        TransportShm_MAKE_MAGICSTAMP(0));
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&TransportShm_module->cfg,
                         (TransportShm_Config *) cfg,
                         sizeof (TransportShm_Config));
            Memory_set (&(TransportShm_module->transports),
                        0,
                        (   sizeof (TransportShm_Handle)
                         *  MultiProc_MAXPROCESSORS
                         *  MessageQ_NUM_PRIORITY_QUEUES));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_setup", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the TransportShm module.
 *
 *              Once this function is called, other TransportShm module
 *              APIs, except for the TransportShm_getConfig API cannot
 *              be called anymore.
 *
 *  @sa         TransportShm_setup
 *              GateMutex_delete
 */
Int
TransportShm_destroy (Void)
{
    Int    status = MessageQ_S_SUCCESS;
    TransportShm_Obj * obj;
    UInt16 i;
    UInt16 j;

    GT_0trace (curTrace, GT_ENTER, "TransportShm_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized */
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&TransportShm_module->refCount)
            == TransportShm_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&TransportShm_module->refCount,
                        TransportShm_MAKE_MAGICSTAMP(1));

            /* Delete/close any Transports that have not been deleted/closed so
             * far.
             */
            for (i = 0; i < MultiProc_MAXPROCESSORS; i++) {
                for (j = 0 ; j < MessageQ_NUM_PRIORITY_QUEUES; j++) {
                    GT_assert (curTrace,
                           (TransportShm_module->transports [i][j] == NULL));
                    if (TransportShm_module->transports [i][j] != NULL) {
                        obj = (TransportShm_Obj *)
                                        TransportShm_module->transports [i][j];
                        if (obj->self != NULL) {
                            if (obj->self->creatorProcId == MultiProc_self ()) {
                                TransportShm_delete (
                                    &(TransportShm_module->transports [i][j]));
                            }
                            else {
                                TransportShm_close (
                                    &(TransportShm_module->transports [i][j]));
                            }
                        }
                    }
                }
            }

            /* Decrease the refCount */
            Atomic_set (&TransportShm_module->refCount,
                        TransportShm_MAKE_MAGICSTAMP(0));

            if (TransportShm_module->gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                        &(TransportShm_module->gateHandle));
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_destroy", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Get Instance parameters
 *
 *  @param      handle          Handle to TransportShm instance
 *  @param      params          Instance creation parameters
 *
 *  @sa         TransportShm_create
 */
Void
TransportShm_Params_init (TransportShm_Params  * params)
{
     GT_1trace (curTrace, GT_ENTER, "TransportShm_Params_init", params);

     GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_Params_init",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
         /*! @retval None */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "TransportShm_Params_init",
                              MessageQ_E_INVALIDARG,
                              "Argument of type "
                              "(TransportShm_Params *) is "
                              "NULL!");
     }
     else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(TransportShm_module->defInstParams),
                     sizeof (TransportShm_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
     }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

     GT_0trace (curTrace, GT_LEAVE, "TransportShm_Params_init");
}


/*
 *  @brief    Create a transport instance. This function waits for the remote
 *            processor to complete its transport creation. Hence it must be
 *            called only after the remote processor is running.
 *
 *  @param    procId     Remote processor ID
 *  @param    params     Instance creation parameters
 *
 *  @sa       TransportShm_delete
 *            Notify_registerEvent
 */
TransportShm_Handle
TransportShm_create (      UInt16                procId,
                     const TransportShm_Params * params)
{
    Int                   status = MessageQ_S_SUCCESS;
    TransportShm_Handle   handle = NULL;

    GT_2trace (curTrace, GT_ENTER, "TransportShm_create", procId, params);

    GT_assert (curTrace, (params != NULL));
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_create",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL params passed is null */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_create",
                             MessageQ_E_INVALIDARG,
                             "params passed is null!");
    }
    else if (   TransportShm_module->transports [procId][params->priority]
             != NULL) {
        /*! @retval MessageQ_E_ALREADYEXISTS A transport for specified procId
                               with specified params->priority already exists */
        status = MessageQ_E_ALREADYEXISTS;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_create",
                             status,
                             "A transport for specified procId with specified"
                             " params->priority already exists!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = _TransportShm_create (&handle, procId, params, TRUE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0){
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShm_create",
                                 status,
                                 "_TransportShm_create failed!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_create", handle);

    /*! @retval Handle Operation successful */
    return (TransportShm_Handle) handle;
}


/*
 *  @brief    Delete instance
 *
 *  @param    handlePtr  Pointer to instance handle to be deleted
 *
 *  @sa       TransportShm_create
 *            ListMP_delete
 *            ListMP_close
 *            Notify_unregisterEvent
 */
Int
TransportShm_delete (TransportShm_Handle * handlePtr)
{
    Int                   status    = MessageQ_S_SUCCESS;
    Int                   tmpStatus = MessageQ_S_SUCCESS;
    TransportShm_Object * handle;
    TransportShm_Obj *    obj;

    GT_1trace (curTrace, GT_ENTER, "TransportShm_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not
         * initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handlePtr
                                                      pointer specified*/
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL *handlePtr specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (TransportShm_Object *) (*handlePtr);
        obj    = (TransportShm_Obj *) handle->obj;

        if (obj != NULL) {
            /* Clear handle in the local array. */
            TransportShm_module->transports [obj->remoteProcId]
                                              [obj->params.priority] = NULL;

            if (obj->self != NULL) {
                /* clear the self flag */
                obj->self->flag = 0;

                if (EXPECT_FALSE (obj->cacheEnabled)) {
                    Cache_wbInv ((Ptr) &(obj->self->flag),
                                 SharedRegion_getCacheLineSize (obj->regionId),
                                 Cache_Type_ALL,
                                 TRUE);
                }
            }

            if (obj->localList != NULL) {
                status = ListMP_delete ((ListMP_Handle *) &obj->localList);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /*! @retval MessageQ_E_FAIL Failed to delete local ListMP */
                    /* Override the status with a MessageQ status code. */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShm_delete",
                                         status,
                                         "Failed to delete local ListMP"
                                         " instance!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (obj->remoteList != NULL) {
                tmpStatus = ListMP_delete (
                                        (ListMP_Handle *) &(obj->remoteList));
                if ((tmpStatus < 0) && (status >= 0)) {
                    /*! @retval MessageQ_E_FAIL Failed to delete remote ListMP*/
                    /* Override the status with a MessageQ status code. */
                    status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShm_delete",
                                     status,
                                     "Failed to delete remote ListMP"
                                     " instance!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            MessageQ_unregisterTransport (obj->remoteProcId,
                                          obj->params.priority);

            tmpStatus = Notify_unregisterEventSingle (obj->remoteProcId,
                                                      0,
                                                      TransportShm_notifyEventId);
            if ((tmpStatus < 0) && (status >= 0)) {
                /*! @retval MessageQ_E_FAIL Failed to unregister Notify event */
                /* Override the status with a MessageQ status code. */
                status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShm_delete",
                                     status,
                                     "Failed to unregister Notify event");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            Memory_free (NULL, obj, sizeof (TransportShm_Obj));
        }

        Memory_free (NULL, handle, sizeof (TransportShm_Object));
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_delete", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 *  @brief    Open a transport instance.
 *
 *  @param    sharedAddr Shared memory address
 *  @param    handlePtr  OUT parameter: Opened instance handle
 *
 *  @sa       TransportShm_delete
 *            Notify_registerEvent
 */
Int
TransportShm_openByAddr (Ptr                   sharedAddr,
                         TransportShm_Handle * handlePtr)
{
    Int                     status = MessageQ_S_SUCCESS;
    TransportShm_Attrs   *  attrs  = NULL;
    TransportShm_Params     params;
    UInt16                  id;

    GT_2trace (curTrace, GT_ENTER, "TransportShm_openByAddr",
               sharedAddr, handlePtr);

    GT_assert (curTrace, (sharedAddr != NULL));
    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized */
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_openByAddr",
                             status,
                             "Module was not initialized!");
    }
    else if (sharedAddr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG sharedAddr passed is null */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_openByAddr",
                             status,
                             "sharedAddr passed is null!");
    }
    else if (handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG handlePtr passed is null */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_openByAddr",
                             status,
                             "handlePtr passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        attrs = (TransportShm_Attrs *) sharedAddr;
        id = SharedRegion_getId (sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (id == SharedRegion_INVALIDREGIONID)  {
            status = MessageQ_E_FAIL;
            /*! @retval MessageQ_E_FAIL sharedAddr is not in a
                                      valid SharedRegion. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShm_openByAddr",
                                 status,
                                 "sharedAddr is not in a valid SharedRegion!");
        }
        else if (   ((UInt32) sharedAddr % SharedRegion_getCacheLineSize (id)
                 != 0)) {
            status = MessageQ_E_FAIL;
            /*! @retval MessageQ_E_FAIL sharedAddr does not
                            meet cache alignment constraints */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShm_openByAddr",
                                 status,
                                 "sharedAddr does not meet"
                                 " cache alignment constraints!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* invalidate the attrs before using it */
            if (EXPECT_FALSE (SharedRegion_isCacheEnabled (id))) {
                Cache_inv ((Ptr) attrs,
                           sizeof (TransportShm_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }

            TransportShm_Params_init (&params);
            /* set params field */
            params.sharedAddr    = sharedAddr;

            params.priority      = attrs->priority;

            if (EXPECT_FALSE (attrs->flag != TransportShm_UP)) {
                /*! @retval MessageQ_E_NOTFOUND Transport is not up */
                status = MessageQ_E_NOTFOUND;
                /* Do not set failure reason, since this is a possible runtime
                 * failure.
                 */
                *handlePtr = NULL;
            }
            else {
                /* Create the object */
                status = _TransportShm_create (handlePtr,
                                               attrs->creatorProcId,
                                               &params,
                                               FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShm_openByAddr",
                                         status,
                                         "Failed to create local transport"
                                         " instance");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_openByAddr", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return status;
}


/*
 *  @brief    Close instance
 *
 *  @param    handlePtr  Pointer to instance handle to be closed
 *
 *  @sa       TransportShm_create
 *            ListMP_delete
 *            ListMP_close
 *            Notify_unregisterEvent
 */
Int
TransportShm_close (TransportShm_Handle * handlePtr)
{
    Int                   status    = MessageQ_S_SUCCESS;
    Int                   tmpStatus = MessageQ_S_SUCCESS;
    TransportShm_Object * handle;
    TransportShm_Obj *    obj;

    GT_1trace (curTrace, GT_ENTER, "TransportShm_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not
         * initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_close",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handlePtr
                                                      pointer specified*/
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_close",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL *handlePtr specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_close",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (TransportShm_Object *) (*handlePtr);
        obj    = (TransportShm_Obj *) handle->obj;

        if (obj != NULL) {
            /* Clear handle in the local array. */
            TransportShm_module->transports [obj->remoteProcId]
                                              [obj->params.priority] = NULL;

            if (obj->other != NULL) {
                /* other flag was set by remote proc */
                obj->other->flag = 0;

                if (EXPECT_FALSE (obj->cacheEnabled)) {
                    Cache_wbInv ((Ptr) &(obj->other->flag),
                                 SharedRegion_getCacheLineSize (obj->regionId),
                                 Cache_Type_ALL,
                                 TRUE);
                }
            }

            if (obj->gate != NULL) {
                status = GateMP_close ((GateMP_Handle *) &(obj->gate));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /*! @retval MessageQ_E_FAIL Failed to close GateMP instance
                     */
                    /* Override the status with a MessageQ status code. */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShm_close",
                                         status,
                                         "Failed to close GateMP instance!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (obj->localList != NULL) {
                tmpStatus = ListMP_close ((ListMP_Handle *) &(obj->localList));
                if ((tmpStatus < 0) && (status >= 0)) {
                    /*! @retval MessageQ_E_FAIL Failed to close local ListMP */
                    /* Override the status with a MessageQ status code. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShm_close",
                                         status,
                                         "Failed to close local ListMP"
                                         " instance!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            if (obj->remoteList != NULL) {
                tmpStatus = ListMP_close ((ListMP_Handle *) &(obj->remoteList));
                if ((tmpStatus < 0) && (status >= 0)) {
                    /*! @retval MessageQ_E_FAIL Failed to close remote ListMP */
                    /* Override the status with a MessageQ status code. */
                    status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShm_close",
                                         status,
                                         "Failed to close remote ListMP"
                                         " instance!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            MessageQ_unregisterTransport (obj->remoteProcId,
                                          obj->params.priority);

            tmpStatus = Notify_unregisterEventSingle (obj->remoteProcId,
                                                      0,
                                                      TransportShm_notifyEventId);
            if ((tmpStatus < 0) && (status >= 0)) {
                /*! @retval MessageQ_E_FAIL Failed to unregister Notify event */
                /* Override the status with a MessageQ status code. */
                status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShm_close",
                                     status,
                                     "Failed to unregister Notify event");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            Memory_free (NULL, obj, sizeof (TransportShm_Obj));
            *handlePtr = NULL;
        }

        Memory_free (NULL, handle, sizeof (TransportShm_Object));
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_close", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief     Put msg to remote list
 *
 * @param     obj        Handle to TransportShm instance
 * @param     msg        Message to be delivered to remote list
 *
 * @sa        Notify_sendEvent
 *            ListMP_putTail
 */
Int
TransportShm_put (TransportShm_Handle   handle,
                  Ptr                   msg)
{
    Int                status = MessageQ_S_SUCCESS;
    TransportShm_Obj * obj    = (TransportShm_Obj *) handle;
    IArg               key;
    UInt16             id;

    GT_2trace (curTrace, GT_ENTER, "TransportShm_put", handle, msg);

    GT_assert (curTrace, (msg != NULL));
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShm_module->refCount),
                                  TransportShm_MAKE_MAGICSTAMP(0),
                                  TransportShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_put",
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
                             "TransportShm_put",
                             status,
                             "Invalid NULL msg pointer specified");
    }
    else if (handle == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handle specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_put",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (TransportShm_Obj *) ((TransportShm_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        id = SharedRegion_getId(msg);
        GT_assert (curTrace, (id != SharedRegion_INVALIDREGIONID));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (id >= SharedRegion_getNumRegions()) {
            /*! @retval MessageQ_E_INVALIDARG Invalid messgae passed */
            status = MessageQ_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShm_put",
                                 status,
                                 "msg contains invalid sharedregion id");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* writeback invalidate the message */
            if (EXPECT_FALSE (SharedRegion_isCacheEnabled(id))
                ) {
                Cache_wbInv ((Ptr) msg,
                             ((MessageQ_Msg)(msg))->msgSize,
                             Cache_Type_ALL,
                             TRUE);
            }

            /* make sure ListMP_put and sendEvent are done before remote executes */
            key = GateMP_enter (obj->gate);

            status = ListMP_putTail ((ListMP_Handle) obj->remoteList,
                                     (ListMP_Elem *) msg);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                /* Override status with MessageQ status code. */
                /*! @retval MessageQ_E_FAIL ListMP_putTail failed */
                status = MessageQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShm_put",
                                     status,
                                     "ListMP_putTail failed");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = Notify_sendEvent (obj->remoteProcId,
                                           0,
                                           TransportShm_notifyEventId,
                                           0,
                                           FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override status with MessageQ status code. */
                    status = MessageQ_E_TIMEOUT;

                    /* If sending the event failed, then remove the element from the
                     * list.
                     */
                    /* Ignore the status of remove. */
                    ListMP_remove ((ListMP_Handle) obj->remoteList,
                                   (ListMP_Elem *) msg);
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "TransportShm_put",
                                        status,
                                        "Notification to remote processor failed!");
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            /* leave the gate */
            GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_put", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief     Control Function
 *
 * @param     obj        Handle to TransportShm instance
 * @param     cmd        cmd ID
 * @param     cmdArg     Argument specific to cmd ID
 *
 * @sa        None
 */
Int
TransportShm_control (TransportShm_Handle   handle,
                      UInt                  cmd,
                      UArg                  cmdArg)
{
    GT_3trace (curTrace, GT_ENTER, "TransportShm_control", handle, cmd, cmdArg);

    GT_assert (curTrace, (handle != NULL));

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_control",
               MessageQ_E_INVALIDARG);

    /*! @retval MessageQ_E_INVALIDARG Specified operation is not supported. */
    return (MessageQ_E_INVALIDARG);
}


/*
 * @brief     Get status
 *
 * @param     obj        Handle to TransportShm instance
 *
 * @sa        None
 */
Int
TransportShm_getStatus (TransportShm_Handle handle)
{
    TransportShm_Obj * obj;;

    GT_1trace (curTrace, GT_ENTER, "TransportShm_getStatus", handle);

    GT_assert (curTrace, (handle != NULL));

    obj = (TransportShm_Obj *) ((TransportShm_Object *) handle)->obj;
    GT_assert (curTrace, (obj != NULL));

    GT_1trace (curTrace,
               GT_LEAVE,
               "TransportShm_getStatus",
               obj->status);

    /*! @retval Status-of-MessageQ-Transport Operation successful. */
    return (obj->status);
}


/*
 * @brief     Get shared memory requirements
 *
 * @param     params     Instance creation parameters
 *
 * @sa        None
 */
SizeT
TransportShm_sharedMemReq (const TransportShm_Params * params)
{
    SizeT         memReq = 0;
    SizeT         minAlign;
    UInt16        regionId;
    ListMP_Params listMPParams;

    GT_1trace (curTrace, GT_ENTER, "TransportShm_sharedMemReq", params);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params == NULL) {
        /*! @retval 0 params pointer passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShm_sharedMemReq",
                             MessageQ_E_INVALIDARG,
                             "params pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        regionId = SharedRegion_getId (params->sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionId == SharedRegion_INVALIDREGIONID)  {
            /*! @retval 0 params->sharedAddr is not in a valid SharedRegion. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShm_sharedMemReq",
                                 MessageQ_E_FAIL,
                                 "params->sharedAddr is not in a"
                                 " valid SharedRegion!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            minAlign = Memory_getMaxDefaultTypeAlign ();
            if (SharedRegion_getCacheLineSize (regionId) > minAlign) {
                minAlign = SharedRegion_getCacheLineSize (regionId);
            }

            /* for the Attrs structure */
            memReq = _Ipc_roundup (sizeof (TransportShm_Attrs), minAlign);

            /* for the second Attrs structure */
            memReq += _Ipc_roundup (sizeof (TransportShm_Attrs), minAlign);

            ListMP_Params_init (&listMPParams);
            listMPParams.regionId = regionId;

            /* for localListMP */
            memReq += ListMP_sharedMemReq (&listMPParams);

            /* for remoteListMP */
            memReq += ListMP_sharedMemReq (&listMPParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShm_sharedMemReq", memReq);

    /*! @retval Shared-memory-requirements Operation successfully completed. */
    return (memReq);
}



/*
 * @brief     Set the asynchronous error function for the transport module
 *
 * @param     errFxn     Error function to be set
 *
 * @sa        None
 */
Void
TransportShm_setErrFxn (TransportShm_ErrFxn errFxn)
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
_TransportShm_notifyFxn (UInt16  procId,
                         UInt16  lineId,
                         UInt32  eventId,
                         Ptr     arg,
                         UInt32  payload)
{
    TransportShm_Obj * obj = NULL;
    MessageQ_Msg          msg = NULL;
    UInt32                queueId;

    GT_5trace (curTrace, GT_ENTER, "_TransportShm_notifyFxn",
                                    procId,
                                    lineId,
                                    eventId,
                                    arg,
                                    payload);

    GT_assert (curTrace, (arg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (arg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_TransportShm_notifyFxn`",
                             MessageQ_E_INVALIDARG,
                             "arg passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = ((TransportShm_Obj *) arg);
        GT_assert (curTrace, (obj != NULL));

        /*
         *  While there is are messages, get them out and send them to
         *  their final destination.
         */
        msg = (MessageQ_Msg) ListMP_getHead (obj->localList);
        while (msg != NULL) {
            /* Get the destination message queue Id */
            queueId = MessageQ_getDstQueue (msg);

            /* put the message to the destination queue */
            MessageQ_put (queueId, msg);

            /* check to see if there are more messages */
            msg = (MessageQ_Msg) ListMP_getHead (obj->localList);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "_TransportShm_notifyFxn");
}


/*
 *  @brief    Create a transport instance. This function waits for the remote
 *            processor to complete its transport creation. Hence it must be
 *            called only after the remote processor is running.
 *
 *  @param    procId     Remote processor ID
 *  @param    params     Instance creation parameters
 *
 *  @sa       TransportShm_delete
 *            Notify_registerEvent
 */
Int
_TransportShm_create (      TransportShm_Handle *     handlePtr,
                            UInt16                    procId,
                      const TransportShm_Params     * params,
                            Bool                      createFlag)
{
    Int                      status = MessageQ_S_SUCCESS;
    TransportShm_Obj *       obj    = NULL;
    TransportShm_Object *    handle = NULL;
    Ptr                      localAddr = NULL;
    Int                      localIndex;
    Int                      remoteIndex;
    UInt32                   minAlign;
    ListMP_Params            listMPParams [2];

    GT_4trace (curTrace, GT_ENTER, "_TransportShm_create",
               handlePtr, procId, params, createFlag);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (params != NULL));
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));

    /* No need for parameter checking. This is an internal function. */

    /*
     *  Determine who gets the '0' slot and who gets the '1' slot
     *  The '0' slot is given to the lower MultiProc id.
     */
    if (MultiProc_self () < procId) {
        localIndex  = 0;
        remoteIndex = 1;
    }
    else {
        localIndex  = 1;
        remoteIndex = 0;
    }

    /* Create the generic handle */
    handle = (TransportShm_Object *) Memory_calloc (NULL,
                                                 sizeof (TransportShm_Object),
                                                 0u,
                                                 NULL);
    *handlePtr = (TransportShm_Handle) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval MessageQ_E_MEMORY Memory allocation failed for pointer of
         *          type TransportShm_Object
         */
        status = MessageQ_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_TransportShm_create",
                             status,
                             "Memory allocation failed for pointer"
                             " of type TransportShm_Object!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (TransportShm_Obj *) Memory_calloc (NULL,
                                                 sizeof (TransportShm_Obj),
                                                 0,
                                                 NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            /*! @retval MessageQ_E_MEMORY Memory allocation failed for pointer
             *          of type TransportShm_Obj
             */
            status = MessageQ_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_TransportShm_create",
                                 status,
                                 "Memory allocation failed for pointer"
                                 " of type TransportShm_Obj");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            handle->obj = obj ;
            handle->put = (IMessageQTransport_putFxn) &TransportShm_put;
            handle->getStatus = (IMessageQTransport_getStatusFxn)
                                                    &TransportShm_getStatus;
            handle->controlFn = (IMessageQTransport_controlFxn)
                                                    &TransportShm_control;

            if (createFlag == FALSE) {
                /* Open by sharedAddr */
                obj->self = (TransportShm_Attrs *) params->sharedAddr;
                obj->regionId = SharedRegion_getId (params->sharedAddr);
                GT_assert (curTrace,
                           (obj->regionId != SharedRegion_INVALIDREGIONID));

                obj->cacheEnabled = SharedRegion_isCacheEnabled (obj->regionId);

                localAddr = SharedRegion_getPtr (obj->self->gateMPAddr);
                GT_assert (curTrace, (localAddr != NULL));

                status = GateMP_openByAddr (localAddr,
                                            (GateMP_Handle *) &obj->gate);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override the status to return a MessageQ status. */
                    /*! @retval MessageQ_E_FAIL GateMP_openByAddr failed */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_TransportShm_create",
                                         status,
                                         "GateMP_openByAddr failed!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else {
                /* Init the gate for ListMP create below */
                if (params->gate != NULL) {
                    obj->gate = params->gate;
                }
                else {
                    obj->gate = GateMP_getDefaultRemote ();
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (obj->gate == NULL) {
                    /*! @retval MessageQ_E_FAIL GateMP is NULL */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_TransportShm_create",
                                         status,
                                         "GateMP is NULL!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    Memory_copy (&(obj->params),
                                 (Ptr) params,
                                 sizeof (TransportShm_Params));


                    /* Creating using sharedAddr */
                    obj->regionId = SharedRegion_getId (params->sharedAddr);

                    /* Assert that the buffer is in a valid shared
                     * region
                     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (obj->regionId == SharedRegion_INVALIDREGIONID)  {
                        status = MessageQ_E_FAIL;
                        /*! @retval MessageQ_E_FAIL params->sharedAddr is not
                                                 in a valid SharedRegion. */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_TransportShm_create",
                                             status,
                                             "params->sharedAddr is not in a"
                                             " valid SharedRegion!");
                    }
                    else if (  ((UInt32)params->sharedAddr
                              % SharedRegion_getCacheLineSize (obj->regionId)
                              != 0)) {
                        status = MessageQ_E_FAIL;
                        /*! @retval MessageQ_E_FAIL params->sharedAddr does not
                                        meet cache alignment constraints */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_TransportShm_create",
                                             status,
                                             "params->sharedAddr does not meet"
                                             " cache alignment constraints!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* set handle's cacheEnabled, type, attrs */
                        obj->cacheEnabled = SharedRegion_isCacheEnabled (
                                                                obj->regionId);
                        obj->self = (TransportShm_Attrs *) params->sharedAddr;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Determine the minimum alignment to align to */
                minAlign = Memory_getMaxDefaultTypeAlign ();
                if (SharedRegion_getCacheLineSize (obj->regionId) > minAlign) {
                    minAlign = SharedRegion_getCacheLineSize (obj->regionId);
                }

                /*
                 *  Carve up the shared memory.
                 *  If cache is enabled, these need to be on separate cache
                 *  lines. This is done with minAlign and _Ipc_roundup function.
                 */

                obj->other = (TransportShm_Attrs *)((UInt32)(obj->self) +
                           (_Ipc_roundup(sizeof(TransportShm_Attrs), minAlign)));

                ListMP_Params_init (&(listMPParams [0]));
                listMPParams[0].gate = obj->gate;
                listMPParams[0].sharedAddr = (Ptr)
                        (  (UInt32) (obj->other)
                         + (_Ipc_roundup (sizeof (TransportShm_Attrs), minAlign)));

                ListMP_Params_init (&listMPParams [1]);
                listMPParams[1].gate = obj->gate;
                listMPParams[1].sharedAddr = (Ptr)
                                   (   (UInt32) (listMPParams [0].sharedAddr)
                                    +  ListMP_sharedMemReq (&listMPParams [0]));

                obj->priority      = params->priority;
                obj->remoteProcId  = procId;

                if (createFlag == TRUE) {
                    obj->localList = ListMP_create (
                                                &(listMPParams [localIndex]));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (obj->localList == NULL) {
                        /*! @retval MessageQ_E_FAIL ListMP_create failed for
                                                                    localList */
                        status = MessageQ_E_FAIL;
                        GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_TransportShm_create",
                                     status,
                                     "ListMP_create failed for local list!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        obj->remoteList = ListMP_create (
                                                &(listMPParams [remoteIndex]));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (obj->remoteList == NULL) {
                            /*! @retval MessageQ_E_FAIL ListMP_create failed for
                                                                remoteList */
                            status = MessageQ_E_FAIL;
                            GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_TransportShm_create",
                                     status,
                                     "ListMP_create failed for remoteList!");
                        }
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
                else {
                    /* Open the local ListMP instance */
                    status = ListMP_openByAddr (
                                        listMPParams [localIndex].sharedAddr,
                                        &(obj->localList));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* Override with MessageQ status. */
                        /*! @retval MessageQ_E_FAIL ListMP_openByAddr failed for
                                                                localList */
                        status = MessageQ_E_FAIL;
                        GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_TransportShm_create",
                                     status,
                                     "ListMP_openByAddr failed for localList!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        status = ListMP_openByAddr (
                                        listMPParams [remoteIndex].sharedAddr,
                                        &(obj->remoteList));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            /* Override with MessageQ status. */
                            /*! @retval MessageQ_E_FAIL ListMP_openByAddr failed
                                                               for remoteList */
                            status = MessageQ_E_FAIL;
                            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_TransportShm_create",
                                 status,
                                 "ListMP_openByAddr failed for remoteList!");
                        }
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    status = Notify_registerEventSingle (procId,
                                0, /* lineId */
                                TransportShm_notifyEventId,
                                (Notify_FnNotifyCbck) _TransportShm_notifyFxn,
                                (Ptr) obj);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* Override the status with MessageQ status */
                        status = MessageQ_E_FAIL;
                        /*! @retval MessageQ_E_FAIL Notify_registerEventSingle
                                                    failed */
                        GT_setFailureReason (curTrace,
                                GT_4CLASS,
                                "_TransportShm_create",
                                status,
                                "Notify_registerEventSingle failed!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        if (createFlag == TRUE) {
                            obj->self->creatorProcId = MultiProc_self ();
                            obj->self->notifyEventId =
                                         TransportShm_module->cfg.notifyEventId;
                            obj->self->priority = obj->priority;

                            /* Store the GateMP sharedAddr in the Attrs */
                            obj->self->gateMPAddr = GateMP_getSharedAddr (
                                                                    obj->gate);
                            obj->self->flag = TransportShm_UP;

                            if (EXPECT_FALSE (obj->cacheEnabled)) {
                                Cache_wbInv ((Ptr) obj->self,
                                             sizeof (TransportShm_Attrs),
                                             Cache_Type_ALL,
                                             TRUE);
                            }
                        }
                        else {
                            obj->other->flag = TransportShm_UP;
                            if (EXPECT_FALSE (obj->cacheEnabled)) {
                                Cache_wb ((Ptr) &(obj->other->flag),
                                           minAlign,
                                           Cache_Type_ALL,
                                           TRUE);
                            }
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }

                    if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Register the transport with MessageQ */
                        status = MessageQ_registerTransport ((IMessageQTransport_Handle) handle,
                                                             procId,
                                                             params->priority);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "_TransportShm_create",
                                        status,
                                        "MessageQ_registerTransport failed!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            obj->status = TransportShm_UP;
                            /* Set handle in the local array. */
                            TransportShm_module->transports [obj->remoteProcId]
                                              [obj->params.priority] =
                                                (TransportShm_Handle) handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
                    }
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            /* Cleanup in case of error. */
            if (status < 0) {
                if (createFlag == TRUE) {
                    TransportShm_delete ((TransportShm_Handle *) handlePtr);
                }
                else {
                    TransportShm_close ((TransportShm_Handle *) handlePtr);
                }
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_TransportShm_create", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return status;
}
