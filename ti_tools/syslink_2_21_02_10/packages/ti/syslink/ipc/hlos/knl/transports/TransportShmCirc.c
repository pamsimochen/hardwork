/*
 *  @file   TransportShmCirc.c
 *
 *  @brief      MessageQ Circular Buffer based shared memory transport
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
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/inc/_Ipc.h>

/* Module level headers */
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/syslink/inc/TransportShmCirc.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*!
 *  @brief  Macro to make a correct module magic number with refCount
 */
#define TransportShmCirc_MAKE_MAGICSTAMP(x)                                \
                                  ((TransportShmCirc_MODULEID << 12u) | (x))

#define TransportShmCirc_RESERVED_EVENTNO  (2u)

#define TransportShmCirc_notifyEventId (UInt32) TransportShmCirc_RESERVED_EVENTNO + \
                                                (UInt32)(Notify_SYSTEMKEY << 16)

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief   Defines the TransportShmCirc state object, which contains all
 *           the module specific information.
 */
typedef struct TransportShmCirc_ModuleObject_tag {
    Atomic                       refCount;
    /* Flag to indicate initialization state of GP */
    TransportShmCirc_Config      cfg;
    /*!< TransportShmCirc configuration structure */
    TransportShmCirc_Config      defCfg;
    /*!< Default module configuration */
    TransportShmCirc_Params      defInstParams;
    /*!< Default instance parameters */
    TransportShmCirc_Handle      transports
                      [MultiProc_MAXPROCESSORS][MessageQ_NUM_PRIORITY_QUEUES];
    /*!< Transport to be set in MessageQ_registerTransport */
    TransportShmCirc_ErrFxn      errFxn;
    /*!< Error function */
    Bool                         enableStats;
    /*!<
     *  Enable statistics for sending an event
     *
     *  If this parameter is to 'TRUE' and 'waitClear' is also set to
     *  TRUE when calling, then the module keeps track of the number of times
     *  the processor spins waiting for an empty slot and the max amount of time
     *  it waits.
     */
    UInt                         numMsgs;
    /*!<
     *  The number of messages or slots in the circular buffer
     *
     *  This is use to determine the size of the put and get buffers.
     *  Each eventEntry is two 32bits wide, therefore the total size
     *  of each circular buffer is [numMsgs * sizeof(eventEntry)].
     *  The total size of each buffer must be a multiple of the
     *  the cache line size. For example, if the cacheLineSize = 128
     *  then numMsgs could be 16, 32, etc...
     */
    UInt                         maxIndex;
    /*!< The max index set to (numMsgs - 1) */
    UInt                         modIndex;
    /*!<
     *  The modulo index value. Set to (numMsgs / 4).
     *  Used in the isr for doing cache_wb of readIndex.
     */
} TransportShmCirc_ModuleObject;

/*!
 *  @brief  Structure defining config parameters for the MessageQ transport
 *  instances.
 */
typedef struct TransportShmCirc_Obj_tag {
    TransportShmCirc_Params          params;
    /*!< Instance parameters (configuration values) */
    Ptr                             *putBuffer;
    /*!< buffer used to put events */
    Bits32                          *putReadIndex;
    /*!< ptr to readIndex for put buffer */
    Bits32                          *putWriteIndex;
    /*!< ptr to writeIndex for put buffer */
    Ptr                             *getBuffer;
    /*!< buffer used to get events */
    Bits32                          *getReadIndex;
    /*!< ptr to readIndex for get buffer */
    Bits32                          *getWriteIndex;
    /*!< ptr to writeIndex for get buffer */
    SizeT                            opCacheSize;
    /*!< optimized cache size for wb/inv */
    UInt16                           regionId;
    /*!< Shared Region ID */
    UInt16                           remoteProcId;
    /*!< Processor ID of the remote processor which which this driver instance
         communicates. */
    UInt32                           cacheEnabled;
    /*!< Whether to perform cache calls */
    UInt16                           priority;
    /*!< Priority to register */
} TransportShmCirc_Obj;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    TransportShmCirc_state
 *
 *  @brief  TransportShmCirc state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmCirc_ModuleObject TransportShmCirc_state =
{
    .enableStats = FALSE,
    .numMsgs = 32u,
    .maxIndex = (32u - 1u), /* numMsgs - 1 */
    .modIndex = (32u / 4u), /* numMsgs / 4 */
    .defCfg.errFxn = NULL,
    .defCfg.notifyEventId = TransportShmCirc_RESERVED_EVENTNO,
    .defInstParams.sharedAddr = NULL,
    .defInstParams.priority = MessageQ_NORMALPRI
};

/*!
 *  @var    TransportShmCirc_module
 *
 *  @brief  Pointer to the TransportShmCirc module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmCirc_ModuleObject * TransportShmCirc_module =
                                                        &TransportShmCirc_state;


/* =============================================================================
 * Forward declarations of internal functions
 * =============================================================================
 */
/* Callback function registered with the Notify module. */
Void _TransportShmCirc_notifyFxn (UInt16  procId,
                                  UInt16  lineId,
                                  UInt32  eventId,
                                  Ptr     arg,
                                  UInt32  payload);


/* =============================================================================
 * APIs called directly by applications
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the TransportShmCirc
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to TransportShmCirc_setup filled in
 *              by the TransportShmCirc module with the default parameters.
 *              If the user does not wish to make any change in the default
 *              parameters, this API is not required to be called.
 *
 *  @param      cfg        Pointer to the TransportShmCirc module
 *                         configuration structure in which the default config
 *                         is to be returned.
 *
 *  @sa         TransportShmCirc_setup
 */
Void
TransportShmCirc_getConfig (TransportShmCirc_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "TransportShmCirc_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_getConfig",
                             MessageQ_E_INVALIDARG,
                             "Argument of type (TransportShmCirc_Config *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(TransportShmCirc_module->refCount),
                                      TransportShmCirc_MAKE_MAGICSTAMP(0),
                                      TransportShmCirc_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfg,
                         &(TransportShmCirc_module->defCfg),
                         sizeof (TransportShmCirc_Config));
        }
        else {
            Memory_copy (cfg,
                         &(TransportShmCirc_module->cfg),
                         sizeof (TransportShmCirc_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "TransportShmCirc_getConfig");
}


/*!
 *  @brief      Setup the TransportShmCirc module.
 *
 *              This function sets up the TransportShmCirc module. This
 *              function must be called before any other instance-level APIs can
 *              be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then TransportShmCirc_getConfig can be called to
 *              get the configuration filled with the default values. After
 *              this, only the required configuration values can be changed. If
 *              the user does not wish to make any change in the default
 *              parameters, the application can simply call
 *              TransportShmCirc_setup with NULL parameters. The default
 *              parameters would get automatically used.
 *
 *  @param      cfg   Optional TransportShmCirc module configuration.
 *                    If provided as NULL, default configuration is used.
 *
 *  @sa         TransportShmCirc_destroy
 *              GateMutex_create
 */
Int
TransportShmCirc_setup (const TransportShmCirc_Config * cfg)
{
    Int                 status = MessageQ_S_SUCCESS;
    TransportShmCirc_Config tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "TransportShmCirc_setup", cfg);

    if (cfg == NULL) {
        TransportShmCirc_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&TransportShmCirc_module->refCount,
                            TransportShmCirc_MAKE_MAGICSTAMP(0),
                            TransportShmCirc_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&TransportShmCirc_module->refCount)
        != TransportShmCirc_MAKE_MAGICSTAMP(1u)) {
        status = MessageQ_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "MessageQ Module already initialized!");
    }
    else {
        /* Copy the user provided values into the state object. */
        Memory_copy (&TransportShmCirc_module->cfg,
                     (TransportShmCirc_Config *) cfg,
                     sizeof (TransportShmCirc_Config));
        Memory_set (&(TransportShmCirc_module->transports),
                    0,
                    (   sizeof (TransportShmCirc_Handle)
                     *  MultiProc_MAXPROCESSORS
                     *  MessageQ_NUM_PRIORITY_QUEUES));
    }

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_setup", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the TransportShmCirc module.
 *
 *              Once this function is called, other TransportShmCirc module
 *              APIs, except for the TransportShmCirc_getConfig API cannot
 *              be called anymore.
 *
 *  @sa         TransportShmCirc_setup
 *              GateMutex_delete
 */
Int
TransportShmCirc_destroy (Void)
{
    Int    status = MessageQ_S_SUCCESS;
    UInt16 i;
    UInt16 j;

    GT_0trace (curTrace, GT_ENTER, "TransportShmCirc_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmCirc_module->refCount),
                                  TransportShmCirc_MAKE_MAGICSTAMP(0),
                                  TransportShmCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized */
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&TransportShmCirc_module->refCount)
            == TransportShmCirc_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&TransportShmCirc_module->refCount,
                        TransportShmCirc_MAKE_MAGICSTAMP(1));

            /* Delete/close any Transports that have not been deleted/closed so
             * far.
             */
            for (i = 0; i < MultiProc_MAXPROCESSORS; i++) {
                for (j = 0 ; j < MessageQ_NUM_PRIORITY_QUEUES; j++) {
                    GT_assert (curTrace,
                           (TransportShmCirc_module->transports [i][j] == NULL));
                    if (TransportShmCirc_module->transports [i][j] != NULL) {
                        TransportShmCirc_delete (
                            &(TransportShmCirc_module->transports [i][j]));
                    }
                }
            }

            /* Decrease the refCount */
            Atomic_set (&TransportShmCirc_module->refCount,
                        TransportShmCirc_MAKE_MAGICSTAMP(0));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_destroy", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Get Instance parameters
 *
 *  @param      handle          Handle to TransportShmCirc instance
 *  @param      params          Instance creation parameters
 *
 *  @sa         TransportShmCirc_create
 */
Void
TransportShmCirc_Params_init (TransportShmCirc_Params  * params)
{
     GT_1trace (curTrace, GT_ENTER, "TransportShmCirc_Params_init", params);

     GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmCirc_module->refCount),
                                  TransportShmCirc_MAKE_MAGICSTAMP(0),
                                  TransportShmCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_Params_init",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
         /*! @retval None */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "TransportShmCirc_Params_init",
                              MessageQ_E_INVALIDARG,
                              "Argument of type "
                              "(TransportShmCirc_Params *) is "
                              "NULL!");
     }
     else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(TransportShmCirc_module->defInstParams),
                     sizeof (TransportShmCirc_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
     }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

     GT_0trace (curTrace, GT_LEAVE, "TransportShmCirc_Params_init");
}


/*
 *  @brief    Create a transport instance.
 *
 *  @param    procId     Remote processor ID
 *  @param    params     Instance creation parameters
 *
 *  @sa       TransportShmCirc_delete
 *            Notify_registerEvent
 */
TransportShmCirc_Handle
TransportShmCirc_create (      UInt16                      procId,
                           const TransportShmCirc_Params * params)
{
    Int                       status = MessageQ_S_SUCCESS;
    TransportShmCirc_Object * handle = NULL;
    TransportShmCirc_Obj *    obj    = NULL;
    Int                       localIndex;
    Int                       remoteIndex;
    UInt32                    minAlign;
    SizeT                     ctrlSize;
    SizeT                     circBufSize;
    SizeT                     totalSelfSize;

    GT_2trace (curTrace, GT_ENTER, "TransportShmCirc_create", procId, params);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmCirc_module->refCount),
                                  TransportShmCirc_MAKE_MAGICSTAMP(0),
                                  TransportShmCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_create",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL params passed is null */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_create",
                             MessageQ_E_INVALIDARG,
                             "params passed is null!");
    }
    else if (   TransportShmCirc_module->transports [procId][params->priority]
             != NULL) {
        /*! @retval MessageQ_E_ALREADYEXISTS A transport for specified procId
                               with specified params->priority already exists */
        status = MessageQ_E_ALREADYEXISTS;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_create",
                             status,
                             "A transport for specified procId with specified"
                             " params->priority already exists!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Create the generic handle */
        handle = (TransportShmCirc_Object *) Memory_calloc (NULL,
                                             sizeof (TransportShmCirc_Object),
                                             0u,
                                             NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle == NULL) {
            /*! @retval MessageQ_E_MEMORY Memory allocation failed for pointer
             *          of type TransportShmCirc_Object
             */
            status = MessageQ_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmCirc_create",
                                 status,
                                 "Memory allocation failed for pointer"
                                 " of type TransportShmCirc_Object!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj = (TransportShmCirc_Obj *) Memory_calloc (NULL,
                                               sizeof (TransportShmCirc_Obj),
                                               0,
                                               NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj == NULL) {
                /*! @retval MessageQ_E_MEMORY Memory allocation failed for pointer
                 *          of type TransportShmCirc_Obj
                 */
                status = MessageQ_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmCirc_create",
                                     status,
                                     "Memory allocation failed for pointer"
                                     " of type TransportShmCirc_Obj");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                handle->obj = obj ;
                handle->put = (IMessageQTransport_putFxn)
                                            &TransportShmCirc_put;
                handle->getStatus = (IMessageQTransport_getStatusFxn)
                                                &TransportShmCirc_getStatus;
                handle->controlFn = (IMessageQTransport_controlFxn)
                                                    &TransportShmCirc_control;

                /* determine which slot to use */
                if (MultiProc_self () < procId) {
                    localIndex  = 0;
                    remoteIndex = 1;
                }
                else {
                    localIndex  = 1;
                    remoteIndex = 0;
                }

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
                                         "TransportShmCirc_create",
                                         status,
                                         "params->sharedAddr is not in a"
                                         " valid SharedRegion!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* determine the minimum alignment */
                    minAlign = Memory_getMaxDefaultTypeAlign ();
                    if (SharedRegion_getCacheLineSize (obj->regionId) > minAlign) {
                        minAlign = SharedRegion_getCacheLineSize (obj->regionId);
                    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (((UInt32)params->sharedAddr% minAlign) != 0) {
                        status = MessageQ_E_FAIL;
                        /*! @retval MessageQ_E_FAIL params->sharedAddr does not
                                        meet cache alignment constraints */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "TransportShmCirc_create",
                                             status,
                                             "params->sharedAddr does not meet"
                                             " cache alignment constraints!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        obj->cacheEnabled = SharedRegion_isCacheEnabled (
                                                                obj->regionId);
                        obj->priority = params->priority;
                        obj->remoteProcId  = procId;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }

                if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* calculate the circular buffer size one-way */
                    circBufSize = _Ipc_roundup (    sizeof (Bits32)
                                             * TransportShmCirc_module->numMsgs,
                                             minAlign);

                    /* calculate the control size one-way */
                    ctrlSize = _Ipc_roundup (sizeof (Bits32), minAlign);

                    /* calculate the total size one-way */
                    totalSelfSize =  circBufSize + (2 * ctrlSize);

                    /*
                     *  Init put/get buffer and index pointers.
                     *  These are all on different cache lines.
                     */
                    obj->putBuffer = (Ptr) (    (UInt32) params->sharedAddr
                                            +   (localIndex * totalSelfSize));

                    obj->putWriteIndex = (Bits32 *) (   (UInt32)obj->putBuffer
                                                     +  circBufSize);

                    obj->putReadIndex = (Bits32 *) (  (UInt32) obj->putWriteIndex
                                                    + ctrlSize);

                    obj->getBuffer = (Ptr) (    (UInt32) params->sharedAddr
                                            +   (remoteIndex * totalSelfSize));

                    obj->getWriteIndex = (Bits32 *) (   (UInt32) obj->getBuffer
                                                     +  circBufSize);

                    obj->getReadIndex = (Bits32 *) ( (UInt32) obj->getWriteIndex
                                                    + ctrlSize);

                    /*
                     *  Calculate the size for cache inv in isr.
                     *  This size is the circular buffer + putWriteIndex.
                     *  [sizeof(Ptr) * numMsgs] + [the sizeof(Ptr)]
                     *  aligned to a cache line.
                     */
                    obj->opCacheSize = (    (UInt32) obj->putReadIndex
                                        -   (UInt32) obj->putBuffer);


                    status = Notify_registerEventSingle (procId,
                            0, /* lineId */
                            TransportShmCirc_notifyEventId,
                            (Notify_FnNotifyCbck) _TransportShmCirc_notifyFxn,
                            (Ptr) obj);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* Override the status with MessageQ status */
                        status = MessageQ_E_FAIL;
                        /*! @retval MessageQ_E_FAIL Notify_registerEventSingle
                                                    failed */
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_TransportShmCirc_create",
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
                                             "_TransportShmCirc_create",
                                             status,
                                             "MessageQ_registerTransport failed!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            /* init the putWrite and putRead Index to 0 */
                            obj->putWriteIndex [0] = 0;
                            obj->putReadIndex [0] = 0;

                            /* cache wb the putWrite/Read Index but no need to
                             * inv them
                             */
                            if (obj->cacheEnabled) {
                                Cache_wb (obj->putWriteIndex,
                                          sizeof(Bits32),
                                          Cache_Type_ALL,
                                          TRUE);

                                Cache_wb (obj->putReadIndex,
                                          sizeof(Bits32),
                                          Cache_Type_ALL,
                                          TRUE);
                            }

                            /* Set handle in the local array. */
                            TransportShmCirc_module->transports
                                   [obj->remoteProcId][obj->priority] =
                                            (TransportShmCirc_Handle) handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
                    }
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            /* Cleanup in case of error. */
            if (status < 0) {
                TransportShmCirc_delete ((TransportShmCirc_Handle *) &handle);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_create", handle);

    /*! @retval Handle Operation successful */
    return (TransportShmCirc_Handle) handle;
}


/*
 *  @brief    Delete instance
 *
 *  @param    handlePtr  Pointer to instance handle to be deleted
 *
 *  @sa       TransportShmCirc_create
 *            ListMP_delete
 *            ListMP_close
 *            Notify_unregisterEvent
 */
Int
TransportShmCirc_delete (TransportShmCirc_Handle * handlePtr)
{
    Int                   status    = MessageQ_S_SUCCESS;
    Int                   tmpStatus = MessageQ_S_SUCCESS;
    TransportShmCirc_Object * handle;
    TransportShmCirc_Obj *    obj;

    GT_1trace (curTrace, GT_ENTER, "TransportShmCirc_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmCirc_module->refCount),
                                  TransportShmCirc_MAKE_MAGICSTAMP(0),
                                  TransportShmCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not
         * initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handlePtr
                                                      pointer specified*/
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL *handlePtr specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (TransportShmCirc_Object *) (*handlePtr);
        obj    = (TransportShmCirc_Obj *) handle->obj;

        if (obj != NULL) {
            /* Clear handle in the local array. */
            TransportShmCirc_module->transports [obj->remoteProcId]
                                                [obj->priority] = NULL;

            MessageQ_unregisterTransport (obj->remoteProcId,
                                          obj->priority);

            tmpStatus = Notify_unregisterEventSingle (obj->remoteProcId,
                                              0,
                                              TransportShmCirc_notifyEventId);
            if ((tmpStatus < 0) && (status >= 0)) {
                /*! @retval MessageQ_E_FAIL Failed to unregister Notify event */
                /* Override the status with a MessageQ status code. */
                status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmCirc_delete",
                                     status,
                                     "Failed to unregister Notify event");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            Memory_free (NULL, obj, sizeof (TransportShmCirc_Obj));
        }

        Memory_free (NULL, handle, sizeof (TransportShmCirc_Object));
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_delete", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief     Put msg to remote MessageQ
 *
 * @param     obj        Handle to TransportShmCirc instance
 * @param     msg        Message to be delivered to remote MessageQ
 *
 * @sa        Notify_sendEvent
 *            ListMP_putTail
 */
Int
TransportShmCirc_put (TransportShmCirc_Handle   handle,
                      Ptr                       msg)
{
    Int                    status = MessageQ_S_SUCCESS;
    TransportShmCirc_Obj * obj    = (TransportShmCirc_Obj *) handle;
    Bool                   loop   = FALSE;
    UInt16                 regionId = SharedRegion_INVALIDREGIONID;
    IArg                   sysKey;
    SharedRegion_SRPtr     msgSRPtr;
    UInt32 *               eventEntry;
    UInt32                 writeIndex;
    UInt32                 readIndex;

    GT_2trace (curTrace, GT_ENTER, "TransportShmCirc_put", handle, msg);

    GT_assert (curTrace, (msg != NULL));
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(TransportShmCirc_module->refCount),
                                  TransportShmCirc_MAKE_MAGICSTAMP(0),
                                  TransportShmCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized*/
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_put",
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
                             "TransportShmCirc_put",
                             status,
                             "Invalid NULL msg pointer specified");
    }
    else if (handle == NULL) {
        /*! @retval MessageQ_E_INVALIDARG Invalid NULL handle specified */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_put",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (TransportShmCirc_Obj *) ((TransportShmCirc_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        regionId = SharedRegion_getId(msg);
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionId >= SharedRegion_getNumRegions()) {
            /*! @retval MessageQ_E_INVALIDARG Invalid messgae passed */
            status = MessageQ_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmCirc_put",
                                 status,
                                 "msg contains invalid sharedregion id");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* writeback invalidate the message */
            if (EXPECT_FALSE (SharedRegion_isCacheEnabled (regionId))) {
                Cache_wbInv ((Ptr) msg,
                             ((MessageQ_Msg)(msg))->msgSize,
                             Cache_Type_ALL,
                             TRUE);
            }

            /*
             *  Get the msg's SRPtr.
             */
            msgSRPtr = SharedRegion_getSRPtr (msg, regionId);

            /*
             *  Retrieve the get Index. No need to cache inv the
             *  readIndex until the writeIndex wraps. Only need to invalidate
             *  once every N times [N = number of messages].
             */
            readIndex = obj->putReadIndex [0];

            do {
                /* Enter system gate */
                sysKey = Gate_enterSystem ();

                /* retrieve the put index */
                writeIndex = obj->putWriteIndex [0];

                /* if slot available 'break' out of loop */
                if (    ((writeIndex + 1) & TransportShmCirc_module->maxIndex)
                    !=  readIndex) {
                    break;
                }

                /* Leave system gate */
                Gate_leaveSystem (sysKey);

                /* check to make sure code has looped */
                if (loop) {
                    /*! @retval MessageQ_E_FAIL if no slot available */
                    status = MessageQ_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShmCirc_put",
                                         status,
                                         "No slot is available");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    break;
                }

                /* cache invalidate the putReadIndex */
                if (obj->cacheEnabled) {
                    Cache_inv (obj->putReadIndex,
                               sizeof(Bits32),
                               Cache_Type_ALL,
                               TRUE);
                }

                /* re-read the putReadIndex */
                readIndex = obj->putReadIndex [0];

                /* convey that the code has looped around */
                loop = TRUE;
            } while (1);

            if (status >= 0) {
                /* System gate is entered at this point */

                /* calculate the next available entry */
                eventEntry = (UInt32 *)
                        ((UInt32)obj->putBuffer + (writeIndex * sizeof(Ptr)));

                /* Set the eventId field and payload for the entry */
                eventEntry [0] = msgSRPtr;

                /*
                 *  Writeback the event entry. No need to invalidate since
                 *  only one processor ever writes here. No need to wait for
                 *  cache operation since another cache operation is done below.
                 */
                if (obj->cacheEnabled) {
                    Cache_wb (eventEntry,
                              sizeof(Ptr),
                              Cache_Type_ALL,
                              FALSE);
                }

                /* update the putWriteIndex */
                obj->putWriteIndex[0] =   (writeIndex + 1)
                                        & TransportShmCirc_module->maxIndex;

                /* Leave the system gate */
                Gate_leaveSystem (sysKey);
            }

            if (status >= 0) {
                status = Notify_sendEvent (obj->remoteProcId,
                                           0,
                                           TransportShmCirc_notifyEventId,
                                           0,
                                           FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override status with MessageQ status code. */
                    status = MessageQ_E_TIMEOUT;
                    GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "TransportShmCirc_put",
                                    status,
                                    "Notification to remote processor failed!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_put", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief     Control Function
 *
 * @param     obj        Handle to TransportShmCirc instance
 * @param     cmd        cmd ID
 * @param     cmdArg     Argument specific to cmd ID
 *
 * @sa        None
 */
Int
TransportShmCirc_control (TransportShmCirc_Handle   handle,
                          UInt                      cmd,
                          UArg                      cmdArg)
{
    GT_3trace (curTrace, GT_ENTER, "TransportShmCirc_control",
                                   handle, cmd, cmdArg);

    GT_assert (curTrace, (handle != NULL));

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_control",
               MessageQ_E_INVALIDARG);

    /*! @retval MessageQ_E_INVALIDARG Specified operation is not supported. */
    return (MessageQ_E_INVALIDARG);
}


/*
 * @brief     Get status
 *
 * @param     obj        Handle to TransportShmCirc instance
 *
 * @sa        None
 */
Int
TransportShmCirc_getStatus (TransportShmCirc_Handle handle)
{
    GT_1trace (curTrace, GT_ENTER, "TransportShmCirc_getStatus", handle);

    GT_assert (curTrace, (handle != NULL));

    GT_1trace (curTrace,
               GT_LEAVE,
               "TransportShmCirc_getStatus",
               0);

    /*! @retval Status-of-MessageQ-Transport Operation successful. */
    return (0);
}


/*
 * @brief     Get shared memory requirements
 *
 * @param     params     Instance creation parameters
 *
 * @sa        None
 */
SizeT
TransportShmCirc_sharedMemReq (const TransportShmCirc_Params * params)
{
    SizeT         memReq = 0;
    SizeT         minAlign;
    UInt16        regionId;

    GT_1trace (curTrace, GT_ENTER, "TransportShmCirc_sharedMemReq", params);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params == NULL) {
        /*! @retval 0 params pointer passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCirc_sharedMemReq",
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
                                 "TransportShmCirc_sharedMemReq",
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

            /*
             *  Amount of shared memory:
             *  1 putBuffer with numMsgs (rounded to CLS) +
             *  1 putWriteIndex ptr (rounded to CLS) +
             *  1 putReadIndex put (rounded to CLS) +
             *  1 getBuffer with numMsgs (rounded to CLS) +
             *  1 getWriteIndex ptr (rounded to CLS) +
             *  1 getReadIndex put (rounded to CLS) +
             *
             *  For CLS of 128b it is:
             *      128b + 128b + 128b + 128b+ 128b + 128b = 768b
             *
             *  Note: CLS means Cache Line Size
             */
            memReq = 2 * (
                (_Ipc_roundup (sizeof(Ptr) * TransportShmCirc_module->numMsgs,
                               minAlign))
                + (2 * _Ipc_roundup(sizeof(Bits32), minAlign)));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCirc_sharedMemReq", memReq);

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
TransportShmCirc_setErrFxn (TransportShmCirc_ErrFxn errFxn)
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
_TransportShmCirc_notifyFxn (UInt16  procId,
                         UInt16  lineId,
                         UInt32  eventId,
                         Ptr     arg,
                         UInt32  payload)
{
    TransportShmCirc_Obj * obj = NULL;
    MessageQ_Msg           msg = NULL;
    UInt32                 queueId;
    UInt32 *               eventEntry;
    UInt32                 writeIndex;
    UInt32                 readIndex;

    GT_5trace (curTrace, GT_ENTER, "_TransportShmCirc_notifyFxn",
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
                             "_TransportShmCirc_notifyFxn`",
                             MessageQ_E_INVALIDARG,
                             "arg passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = ((TransportShmCirc_Obj *) arg);
        GT_assert (curTrace, (obj != NULL));

        /*
         *  Invalidate both getBuffer and getWriteIndex from cache.
         */
        if (obj->cacheEnabled) {
            Cache_inv (obj->getBuffer,
                       obj->opCacheSize,
                       Cache_Type_ALL,
                       TRUE);
        }

        /* get the writeIndex and readIndex */
        writeIndex = obj->getWriteIndex [0];
        readIndex = obj->getReadIndex [0];

        /* get the next entry to be processed */
        eventEntry = (UInt32 *) &(obj->getBuffer[readIndex]);

        while (writeIndex != readIndex) {
            /* get the msg (convert SRPtr to Ptr) */
            msg = SharedRegion_getPtr ((SharedRegion_SRPtr) eventEntry [0]);

            /* get the queue id */
            queueId = MessageQ_getDstQueue (msg);

            /* put message on local queue */
            MessageQ_put (queueId, msg);

            /* update the local readIndex. */
            readIndex = ((readIndex + 1) & TransportShmCirc_module->maxIndex);

            /* set the getReadIndex */
            obj->getReadIndex [0] = readIndex;

            /*
             *  Write back the getReadIndex once every N / 4 messages.
             *  No need to invalidate since only one processor ever
             *  writes this . No need to wait for operation to complete
             *  since remote core updates its readIndex at least once
             *  every N messages and the remote core will not use a slot
             *  until it sees that the event has been processed with this
             *  cache wb. Chances are small that the remote core needs
             *  to spin due to this since we are doing a wb N / 4.
             */
            if ((obj->cacheEnabled) &&
                ((readIndex % TransportShmCirc_module->modIndex) == 0)) {
                Cache_wb (obj->getReadIndex,
                          sizeof(Bits32),
                          Cache_Type_ALL,
                          FALSE);
            }

            /* get the next entry */
            eventEntry = (UInt32 *) &(obj->getBuffer [readIndex]);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "_TransportShmCirc_notifyFxn");
}
