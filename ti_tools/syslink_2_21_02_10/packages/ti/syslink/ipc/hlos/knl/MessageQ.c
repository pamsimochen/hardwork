/*
 *  @file   MessageQ.c
 *
 *  @brief      Defines MessageQ module.
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

/* Osal And Utils  headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/OsalSemaphore.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/GateSpinlock.h>

/* Module level headers */
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/syslink/inc/knl/TransportSetupProxy.h>
#include <ti/syslink/inc/IMessageQTransport.h>


/* =============================================================================
 * Macros/Constants
 * =============================================================================
 */
/*!
 *  @brief  Name of the reserved NameServer used for MessageQ.
 */
#define MessageQ_NAMESERVER  "MessageQ"

/*! @brief Macro to make a correct module magic number with refCount */
#define MessageQ_MAKE_MAGICSTAMP(x) ((MessageQ_MODULEID << 12u) | (x))

/*! Mask to extract version setting */
const UInt MessageQ_VERSIONMASK = 0xE000;

/*! Mask to extract priority setting */
const UInt MessageQ_TRANSPORTPRIORITYMASK = 0x1;


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for MessageQ module state */
typedef struct MessageQ_ModuleObject {
    Atomic              refCount;
    /*!< Reference count */
    NameServer_Handle   nameServer;
    /*!< Handle to the local NameServer used for storing GP objects */
    IGateProvider_Handle gate;
    /*!< Handle of gate to be used for local thread safety */
    MessageQ_Config     cfg;
    /*!< Current config values */
    MessageQ_Config     defaultCfg;
    /*!< Default config values */
    MessageQ_Params     defaultInstParams;
    /*!< Default instance creation parameters */
    IMessageQTransport_Handle transports
                      [MultiProc_MAXPROCESSORS][MessageQ_NUM_PRIORITY_QUEUES];
    /*!< Transport to be set in MessageQ_registerTransport */
    MessageQ_Handle *   queues;
    /*!< MessageQ handle */
    IHeap_Handle *      heaps;
    /*!< Heap to be set in MessageQ_registerHeap */
    UInt16              numQueues;
    /*!< Number of queues */
    UInt16              numHeaps;
    /*!< Number of Heaps */
    Bool                canFreeQueues;
    /*!< Grow option */
    Bits16              seqNum;
    /*!< sequence number                    */
    IGateProvider_Handle resGate;
    /*!< Resource gate */
} MessageQ_ModuleObject;

/*!
 *  @brief  Structure for the Handle for the MessageQ.
 */
typedef struct MessageQ_Object_tag {
    MessageQ_Params         params;
    /*! Instance specific creation parameters */
    MessageQ_QueueId        queue;
    /* Unique id */
    List_Object             normalList;
    /* Embedded List objects */
    List_Object             highList;
    /* Embedded List objects */
    Ptr                     nsKey;
    /* NameServer key */
    OsalSemaphore_Handle    synchronizer;
    /* Semaphore used for synchronizing message events */
    Bool                    unblocked;
    /* Whether MessageQ is unblocked */
    Int                     state;
    /* internal state of object */
} MessageQ_Object;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
MessageQ_ModuleObject MessageQ_state =
{
    .nameServer             = NULL,
    .gate                   = NULL,
    .queues                 = NULL,
    .heaps                  = NULL,
    .numQueues              = 1u,
    .numHeaps               = 8u,
    .canFreeQueues          = FALSE,
    .defaultCfg.traceFlag   = FALSE,
    .defaultCfg.numHeaps    = 8u,
    .defaultCfg.maxRuntimeEntries = 32u,
    .defaultCfg.maxNameLen    = 32u,
    .defaultInstParams.synchronizer = NULL
};

/*!
 *  @var    MessageQ_module
 *
 *  @brief  Pointer to the MessageQ module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
MessageQ_ModuleObject * MessageQ_module = &MessageQ_state;


/* =============================================================================
 * Forward declarations of internal functions
 * =============================================================================
 */
/*
 *  @brief   Grow the MessageQ table
 *
 *  @sa      MessageQ_create
 */
UInt16 _MessageQ_grow (MessageQ_Object * obj);

/* This is a helper function to initialize a message. */
static Void MessageQ_msgInit (MessageQ_Msg msg);


/* =============================================================================
 * APIS
 * =============================================================================
 */
/* Function to get default configuration for the MessageQ module. */
Void
MessageQ_getConfig (MessageQ_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "MessageQ_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (cfg == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_getConfig",
                             MessageQ_E_INVALIDARG,
                             "Argument of type (MessageQ_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_TRUE(   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                  MessageQ_MAKE_MAGICSTAMP(0),
                                                  MessageQ_MAKE_MAGICSTAMP(1))
                        == TRUE)) {
            /* (If setup has not yet been called) */
            Memory_copy (cfg,
                         &MessageQ_module->defaultCfg,
                         sizeof (MessageQ_Config));
        }
        else {
            Memory_copy (cfg,
                         &MessageQ_module->cfg,
                         sizeof (MessageQ_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "MessageQ_getConfig");
}


/* Function to setup the MessageQ module. */
Int
MessageQ_setup (const MessageQ_Config * cfg)
{
    Int               status = MessageQ_S_SUCCESS;
    Error_Block       eb;
    NameServer_Params params;
    MessageQ_Config   tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_setup", cfg);
    Error_init (&eb);

    if (EXPECT_FALSE (cfg == NULL)) {
        MessageQ_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (EXPECT_FALSE (cfg == NULL)) {
        MessageQ_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&MessageQ_module->refCount,
                            MessageQ_MAKE_MAGICSTAMP(0),
                            MessageQ_MAKE_MAGICSTAMP(0));

    if (EXPECT_FALSE (   Atomic_inc_return (&MessageQ_module->refCount)
                      != MessageQ_MAKE_MAGICSTAMP(1u))) {
        status = MessageQ_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "MessageQ Module already initialized!");
    }
    else {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (cfg->maxNameLen == 0)) {
            /*! @retval MessageQ_E_INVALIDARG cfg->maxNameLen passed is 0 */
            status = MessageQ_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_setup",
                                 status,
                                 "cfg->maxNameLen passed is 0!");
        }
        else if (EXPECT_FALSE (cfg->maxRuntimeEntries == 0)) {
            /*! @retval MessageQ_E_INVALIDARG cfg->maxRuntimeEntries passed
                                              is 0 */
            status = MessageQ_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_setup",
                                 status,
                                 "cfg->maxRuntimeEntries passed is 0!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Create a default local gate. */
            MessageQ_module->gate = (IGateProvider_Handle)
                        GateSpinlock_create ((GateSpinlock_Params *) NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (MessageQ_module->gate == NULL)) {
                /*! @retval MessageQ_E_FAIL Failed to create GateSpinlock! */
                status = MessageQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MessageQ_setup",
                                     status,
                                     "Failed to create GateSpinlock!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Memory_copy (&MessageQ_module->cfg,
                             (void *) cfg,
                             sizeof (MessageQ_Config));
                /* Initialize the parameters */
                NameServer_Params_init (&params);
                params.maxValueLen = sizeof (UInt32);
                params.maxNameLen  = cfg->maxNameLen;

                MessageQ_module->seqNum = 0;

                /* Create the nameserver for modules */
                MessageQ_module->nameServer = NameServer_create (
                                                            MessageQ_NAMESERVER,
                                                            &params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (MessageQ_module->nameServer == NULL)) {
                    /*! @retval MessageQ_E_FAIL Failed to create the MessageQ
                     *                          name server
                     */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "MessageQ_setup",
                                         MessageQ_E_FAIL,
                                         "Failed to create the MessageQ "
                                         "nameserver!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    MessageQ_module->numHeaps = cfg->numHeaps;
                    MessageQ_module->heaps = (IHeap_Handle *) Memory_calloc (
                                                    NULL,
                                                    (sizeof (IHeap_Handle) *
                                                    MessageQ_module->numHeaps),
                                                    0,
                                                    NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (MessageQ_module->heaps == NULL)) {
                        /*! @retval MessageQ_E_MEMORY Failed to allocate memory
                                                    for the heaps array */
                        status = MessageQ_E_MEMORY;
                        GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "MessageQ_setup",
                            status,
                            "Failed to allocate memory for the heaps array!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        MessageQ_module->numQueues = cfg->maxRuntimeEntries;
                        MessageQ_module->queues = (MessageQ_Handle *)
                                Memory_calloc (NULL,
                                               (    sizeof (MessageQ_Handle)
                                                * MessageQ_module->numQueues),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (EXPECT_FALSE (MessageQ_module->queues == NULL)) {
                            /*! @retval MessageQ_E_MEMORY Failed to allocate
                                                 memory for the queues array */
                            status = MessageQ_E_MEMORY;
                            GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "MessageQ_setup",
                                    status,
                            "Failed to allocate memory for the queues array!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            Memory_set (&(MessageQ_module->transports),
                                        0,
                                        (   sizeof (IMessageQTransport_Handle)
                                         *  MultiProc_MAXPROCESSORS
                                         *  MessageQ_NUM_PRIORITY_QUEUES));

                            /* create the resource gate */
                            MessageQ_module->resGate = (IGateProvider_Handle)
                                    GateMutex_create(NULL, NULL);

                            if (MessageQ_module->resGate == NULL) {
                                status = MessageQ_E_FAIL;
                                GT_setFailureReason(curTrace, GT_4CLASS,
                                        "MessageQ_setup", status,
                                        "Failed to create resource gate");
                            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
                    }
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            if (status < 0) {
                MessageQ_destroy ();
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_setup", status);

    /*! @retval MessageQ_S_SUCCESS Operation Successsful */
    return status;
}


/* Function to destroy the MessageQ module. */
Int
MessageQ_destroy (Void)
{
    Int    status    = MessageQ_S_SUCCESS;
    Int    tmpStatus = MessageQ_S_SUCCESS;
    UInt32 i;

    GT_0trace (curTrace, GT_ENTER, "MessageQ_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        /*! @retval MessageQ_E_INVALIDSTATE Module was not initialized */
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_FALSE (   Atomic_dec_return (&MessageQ_module->refCount)
                          == MessageQ_MAKE_MAGICSTAMP(0))) {
             /* Temporarily increment the refCount */
            Atomic_set (&MessageQ_module->refCount,
                        MessageQ_MAKE_MAGICSTAMP(1));

            /* Delete any Message Queues that have not been deleted so far. */
            for (i = 0; i< MessageQ_module->numQueues; i++) {
                GT_assert (curTrace, (MessageQ_module->queues [i] == NULL));
                if (MessageQ_module->queues [i] != NULL) {
                    tmpStatus = MessageQ_delete (
                                                &(MessageQ_module->queues [i]));
                    GT_assert (curTrace, (tmpStatus >= 0));
                    if (EXPECT_FALSE ((tmpStatus < 0) && (status >= 0))) {
                        status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "MessageQ_destroy",
                                             status,
                                             "MessageQ_delete failed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                }
            }

            /* Decrease the refCount */
            Atomic_set (&MessageQ_module->refCount,
                        MessageQ_MAKE_MAGICSTAMP(0));

            if (EXPECT_TRUE (MessageQ_module->nameServer != NULL)) {
                /* Delete the nameserver for modules */
                tmpStatus = NameServer_delete (&MessageQ_module->nameServer);
                GT_assert (curTrace, (tmpStatus >= 0));
                if (EXPECT_FALSE ((tmpStatus < 0) && (status >= 0))) {
                    status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "MessageQ_destroy",
                                         status,
                                         "NameServer_delete failed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            /* Delete the local gate */
            if (EXPECT_TRUE (MessageQ_module->gate != NULL)) {
                tmpStatus = GateSpinlock_delete ((GateSpinlock_Handle *)
                                                      &MessageQ_module->gate);
                GT_assert (curTrace, (tmpStatus >= 0));
                if (EXPECT_FALSE ((tmpStatus < 0) && (status >= 0))) {
                    status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "MessageQ_destroy",
                                         status,
                                         "GateSpinlock_delete failed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

                Memory_set (&(MessageQ_module->transports),
                            0,
                            (   sizeof (IMessageQTransport_Handle)
                             *  MultiProc_MAXPROCESSORS
                             *  MessageQ_NUM_PRIORITY_QUEUES));

                if (EXPECT_TRUE (MessageQ_module->heaps != NULL)) {
                    Memory_free (NULL,
                             MessageQ_module->heaps,
                             sizeof (IHeap_Handle) * MessageQ_module->numHeaps);
                    MessageQ_module->heaps = NULL;
                }

                if (EXPECT_TRUE (MessageQ_module->queues != NULL)) {
                    Memory_free (NULL,
                                 MessageQ_module->queues,
                                 (  sizeof (MessageQ_Handle)
                                  * MessageQ_module->numQueues));
                    MessageQ_module->queues = NULL;
                }

                Memory_set (&MessageQ_module->cfg, 0, sizeof (MessageQ_Config));
                MessageQ_module->numQueues  = 0u;
                MessageQ_module->numHeaps   = 1u;
                MessageQ_module->canFreeQueues = TRUE;

                /* delete the resource gate */
                if (MessageQ_module->resGate != NULL) {
                    status = GateMutex_delete(
                            (GateMutex_Handle *)&MessageQ_module->resGate);

                    if (status < 0) {
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "MessageQ_destroy", status,
                                 "GateMutex_delete failed");
                        status = MessageQ_E_FAIL;
                    }
                }
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_destroy", status);

    /*! @retval MessageQ_S_SUCCESS Operation Successsful */
    return status;
}


/* Function to initialize the parameters for the MessageQ instance. */
Void
MessageQ_Params_init (MessageQ_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "MessageQ_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_Params_init",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (params == NULL)) {
        /*! @retval None */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_Params_init",
                             MessageQ_E_INVALIDARG,
                             "Argument of type (MessageQ_CreateParams *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(MessageQ_module->defaultInstParams),
                     sizeof (MessageQ_Params));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_Params_init");
}


/* Function to create a MessageQ object. */
MessageQ_Handle
MessageQ_create (      String            name,
                 const MessageQ_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                 status = MessageQ_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    MessageQ_Object   * obj    = NULL;
    Bool                found  = FALSE;
    UInt16              count  = 0;
    UInt16              queueIndex = 0u;
    Int                 i;
    UInt16              start;
    IArg                key;
    List_Params         listParams;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_create", name, params);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_create",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE ((params != NULL) && (params->synchronizer != NULL))){
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_create",
                             status,
                             "Cannot provide non-NULL synchronizer on HLOS!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Create the generic obj */
        obj = (MessageQ_Object *) Memory_calloc (NULL,
                                                 sizeof (MessageQ_Object),
                                                 0u,
                                                 NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (obj == NULL)) {
            status = MessageQ_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_create",
                                 status,
                                 "Memory allocation failed for obj!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            key = IGateProvider_enter (MessageQ_module->gate) ;
            MessageQ_setState((MessageQ_Handle)obj, MessageQ_State_NORMAL);
            start = 0; /* Statically allocated objects not supported */
            count = MessageQ_module->numQueues;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "    MessageQ_create: Max number of queues %d",
                       MessageQ_module->numQueues);

            /* Search the dynamic array for any holes */
            for (i = start; i < count ; i++) {
                if (MessageQ_module->queues [i] == NULL) {
                    MessageQ_module->queues [i] = (MessageQ_Handle) obj;
                    queueIndex = i;
                    found = TRUE;
                    break;
                }
            }

            if (EXPECT_FALSE (found == FALSE)) {
                /* Growth is always allowed. */
                queueIndex = _MessageQ_grow (obj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (queueIndex == MessageQ_INVALIDMESSAGEQ)) {
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "MessageQ_create",
                                         status,
                                         "Failed to grow the queue array");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_TRUE (status >= 0u)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (params != NULL) {
                    /* Populate the params member */
                    Memory_copy ((Ptr) &obj->params,
                                 (Ptr)params,
                                 sizeof (MessageQ_Params));
                    if (EXPECT_FALSE (params->synchronizer == NULL)) {
                        obj->synchronizer = OsalSemaphore_create
                                            (   OsalSemaphore_Type_Binary
                                             | OsalSemaphore_IntType_Interruptible);
                    }
                    else {
                        obj->synchronizer = params->synchronizer;
                    }
                }
                else {
                    obj->synchronizer = OsalSemaphore_create
                                        (   OsalSemaphore_Type_Binary
                                         | OsalSemaphore_IntType_Interruptible);
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (obj->synchronizer == NULL)) {
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "MessageQ_create",
                                    status,
                                    "Failed to create synchronizer semaphore");
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            IGateProvider_leave (MessageQ_module->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_TRUE (status >= 0u)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Construct the list objects */
                List_Params_init (&listParams);
                List_construct (&obj->normalList, &listParams);
                List_construct (&obj->highList, &listParams);
                obj->queue =   (MessageQ_QueueId)
                             (  ((UInt32) (MultiProc_self ()) << 16)
                              | queueIndex);
                obj->unblocked = FALSE;

                if (EXPECT_TRUE (name != NULL)) {
                    obj->nsKey = NameServer_addUInt32 (
                                                  MessageQ_module->nameServer,
                                                  name,
                                                  obj->queue);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (obj->nsKey == NULL)) {
                        status = MessageQ_E_FAIL;
                        GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "MessageQ_create",
                                    status,
                                    "Failed in NameServer_addUInt32");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (status < 0)) {
                MessageQ_delete ((MessageQ_Handle *) &obj);
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_create", obj);

    return (MessageQ_Handle) obj;
}


/* Function to delete a MessageQ object for a specific slave processor. */
Int
MessageQ_delete (MessageQ_Handle * handlePtr)
{
    Int               status    = MessageQ_S_SUCCESS;
    Int               tmpStatus = MessageQ_S_SUCCESS;
    MessageQ_Object * obj       = NULL;
    IArg              key;
    IArg              resKey;
    MessageQ_Msg      tempMsg;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handlePtr == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_delete",
                             status,
                             "handlePtr passed is NULL!");
    }
    else if (EXPECT_FALSE (*handlePtr == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_delete",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (MessageQ_Object *) (*handlePtr);

        if (EXPECT_TRUE (obj->nsKey != NULL)) {
            /* Remove from the name server */
            status = NameServer_removeEntry (MessageQ_module->nameServer,
                                             obj->nsKey);
            GT_assert (curTrace, (status >= 0));
            if (EXPECT_FALSE (status < 0)) {
                /* Override with a MessageQ status code. */
                status = MessageQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MessageQ_delete",
                                     status,
                                     "NameServer_removeEntry failed!");
            }
            else {
                status = MessageQ_S_SUCCESS;
            }
        }

        /* enter the resource gate to prevent late message delivery */
        resKey = IGateProvider_enter(MessageQ_module->resGate);

        /* Remove all the messages for the message queue's normalList queue */
        while (TRUE) {
            /* abandon all messages if in terminated state */
            if (obj->state == MessageQ_State_TERMINATED) {
                break;
            }
            tempMsg = (MessageQ_MsgHeader *) List_get (&obj->normalList);
            if (tempMsg == NULL) {
                break;
            }
            tmpStatus = MessageQ_free (tempMsg);
            GT_assert (curTrace, (tmpStatus >= 0));
            if (EXPECT_FALSE ((tmpStatus < 0) && (status >= 0))) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MessageQ_delete",
                                     status,
                                     "MessageQ_free failed for normalList!");
            }
        }

        List_destruct (&obj->normalList);

        /* Remove all the messages for the message queue's highList queue */
        while (TRUE) {
            /* abandon all messages if in terminated state */
            if (obj->state == MessageQ_State_TERMINATED) {
                break;
            }
            tempMsg = (MessageQ_MsgHeader *) List_get (&obj->highList);
            if (tempMsg == NULL) {
                break;
            }
            tmpStatus = MessageQ_free (tempMsg);
            GT_assert (curTrace, (tmpStatus >= 0));
            if (EXPECT_FALSE ((tmpStatus < 0) && (status >= 0))) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MessageQ_delete",
                                     status,
                                     "MessageQ_free failed for highList!");
            }
        }

        /* Free the list */
        List_destruct (&obj->highList);

        if (EXPECT_TRUE (obj->synchronizer != NULL)) {
            if (EXPECT_TRUE (obj->params.synchronizer == NULL)) {
                tmpStatus = OsalSemaphore_delete (&obj->synchronizer);
                GT_assert (curTrace, (tmpStatus >= 0));
                if (EXPECT_FALSE ((tmpStatus < 0) && (status >= 0))) {
                    /* Override with a MessageQ status code. */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "MessageQ_delete",
                                         status,
                                         "OsalSemaphore_delete failed!");
                }
            }
        }

        /* Take the local lock */
        key = IGateProvider_enter (MessageQ_module->gate);

        /* Clear the MessageQ obj from array. */
        MessageQ_module->queues [(MessageQ_QueueIndex) (obj->queue)] = NULL;

        /* Release the local lock */
        IGateProvider_leave (MessageQ_module->gate, key);

        /* Now free the obj */
        Memory_free (NULL, obj, sizeof (MessageQ_Object));
        *handlePtr = NULL;

        /* leave the resource gate */
        IGateProvider_leave(MessageQ_module->resGate, resKey);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_delete", status);

    return status;
}


/* Opens a created instance of MessageQ module. */
Int
MessageQ_open (String name, MessageQ_QueueId * queueId)
{
    Int status = MessageQ_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_open", name, queueId);

    GT_assert (curTrace, (name != NULL));
    GT_assert (curTrace, (queueId != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_open",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (name == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_open",
                             status,
                             "name passed is NULL!");
    }
    else if (EXPECT_FALSE (queueId == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_open",
                             status,
                             "queueId passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = NameServer_getUInt32 (MessageQ_module->nameServer,
                                       name,
                                       queueId,
                                       NULL);
        if (EXPECT_FALSE (status == NameServer_E_NOTFOUND)) {
            /* Set return queue ID to invalid. */
            *queueId = MessageQ_INVALIDMESSAGEQ;
            status = MessageQ_E_NOTFOUND;
            /* Do not set failure reason since this is a runtime error. */
        }
        else if (EXPECT_TRUE (status >= 0)) {
            /* Override with a MessageQ status code. */
            status = MessageQ_S_SUCCESS;
        }
        else {
            /* Set return queue ID to invalid. */
            *queueId = MessageQ_INVALIDMESSAGEQ;
            /* Override with a MessageQ status code. */
            status = MessageQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_open",
                                 status,
                                 "Failed in NameServer_getUInt32!");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_open", status);

    return status;
}


/* Closes previously opened/created instance of MessageQ module. */
Int
MessageQ_close (MessageQ_QueueId * queueId)
{
    Int32 status = MessageQ_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_close", queueId);

    GT_assert (curTrace, (queueId != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_close",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (queueId == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_close",
                             MessageQ_E_INVALIDARG,
                             "queueId passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Nothing more to be done for closing the MessageQ. */
        *queueId = MessageQ_INVALIDMESSAGEQ;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_close", status);

    return status;
}


/* Gets a message for a message queue and blocks if the queue is empty.
 * If a message is present, it returns it.  Otherwise it blocks
 * waiting for a message to arrive.
 * When a message is returned, it is owned by the caller.
 */
Int
MessageQ_get (MessageQ_Handle handle, MessageQ_Msg * msg, UInt timeout)
{
    Int               status = MessageQ_S_SUCCESS;
    MessageQ_Object * obj    = (MessageQ_Object *) handle;

    GT_3trace (curTrace, GT_ENTER, "MessageQ_get", handle, msg, timeout);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_get",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_get",
                             status,
                             "handle passed is null!");
    }
    else if (EXPECT_FALSE (msg == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_get",
                             status,
                             "msg pointer passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Keep looping while there is no element in the list */

        /* No need to protect, since this can be called only by one thread.
         * List_get is internally protected.
         */

        /* First try to get from highList. */
        *msg = (MessageQ_Msg) List_get ((List_Handle) &obj->highList);
        while (*msg == NULL) {
            *msg = (MessageQ_Msg) List_get ((List_Handle) &obj->normalList);
            if (*msg == NULL) {
                /*
                 *  Block until notified.  If pend times-out, no message
                 *  should be returned to the caller
                 */
                status = OsalSemaphore_pend (obj->synchronizer, timeout);
                if (EXPECT_FALSE (    (status == OSALSEMAPHORE_E_TIMEOUT)
                                  ||  (status == OSALSEMAPHORE_E_WAITNONE))) {
                    *msg = NULL;
                    status = MessageQ_E_TIMEOUT;
                    /* Do not set failure reason since this is a runtime error*/
                    break;
                }
                else if (EXPECT_TRUE (  (status >= 0)
                                     && (status != -ERESTARTSYS))) {
                    if (obj->unblocked) {
                        /* *(msg) may be NULL */
                        status = MessageQ_E_UNBLOCKED;
                        break;
                    }
                    else {
                        *msg = (MessageQ_Msg)
                                    List_get ((List_Handle) &obj->highList);
                        status = MessageQ_S_SUCCESS;
                    }
                }
                else if (status == -ERESTARTSYS) {
                    /* leave status as -ERESTARTSYS */
                    break;
                }
                else {
                    status = MessageQ_E_FAIL;
                    break;
                }
            }
        }

        if (   (status >= 0)
            && EXPECT_FALSE (    (MessageQ_module->cfg.traceFlag == TRUE)
                             ||  (((*msg)->flags & MessageQ_TRACEMASK) != 0))) {
            GT_4trace (curTrace,
                       GT_1CLASS,
                       "MessageQ_get",
                       (*msg),
                       ((*msg)->seqNum),
                       ((*msg)->srcProc),
                       (obj));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_get", status);

    return (status);
}


/* Return a count of the number of messages in the queue */
Int
MessageQ_count (MessageQ_Handle handle)
{
    MessageQ_Object * obj   = (MessageQ_Object *) handle;
    Int               count = 0;
    List_Elem *       elem;
    IArg              key;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_count", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_count",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_count",
                             MessageQ_E_INVALIDARG,
                             "Invalid NULL handle specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();

        List_traverse (elem, (List_Handle) &obj->highList) {
            count++;
        }

        List_traverse (elem, (List_Handle) &obj->normalList) {
            count++;
        }

        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_count", count);

    return (count);
}


/* Initializes a message not obtained from MessageQ_alloc. */
Void
MessageQ_staticMsgInit (MessageQ_Msg msg, UInt32 size)
{
    GT_2trace (curTrace, GT_ENTER, "MessageQ_staticMsgInit", msg, size);

    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_staticMsgInit",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (msg == NULL)) {
        /*! @retval None */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_staticMsgInit",
                             MessageQ_E_INVALIDMSG,
                             "msg is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Fill in the fields of the message */
        MessageQ_msgInit (msg);
        msg->heapId  = MessageQ_STATICMSG;
        msg->msgSize = size;

        if (EXPECT_FALSE (MessageQ_module->cfg.traceFlag == TRUE)) {
            GT_3trace (curTrace,
                       GT_1CLASS,
                       "MessageQ_staticMsgInit",
                       (msg),
                       ((msg)->seqNum),
                       ((msg)->srcProc));
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_staticMsgInit");
}


/* Allocate a message and initialize the needed fields (note some
 * of the fields in the header are set via other APIs or in the
 * MessageQ_put function.)
 */
MessageQ_Msg
MessageQ_alloc (UInt16 heapId, UInt32 size)
{
    MessageQ_Msg msg = NULL;
    UInt16 id;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_alloc", heapId, size);

    GT_assert (curTrace, (heapId < MessageQ_module->numHeaps));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                  MessageQ_MAKE_MAGICSTAMP(0),
                                  MessageQ_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_alloc",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (heapId >= MessageQ_module->numHeaps) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_alloc",
                             MessageQ_E_INVALIDHEAPID,
                             "Heap id is invalid!");
    }
    else if (MessageQ_module->heaps [heapId] == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_alloc",
                             MessageQ_E_INVALIDHEAPID,
                             "Heap id is not registered with MessageQ!");
    }
    else if (size < sizeof (MessageQ_MsgHeader)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_alloc",
                             MessageQ_E_INVALIDARG,
                             "Size is less than MessageQ_MsgHeader size!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate the message. No alignment requested */
        msg = Memory_alloc (MessageQ_module->heaps [heapId],
                            size,
                            0,
                            NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (msg == NULL)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_alloc",
                                 MessageQ_E_MEMORY,
                                 "Message allocation failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Fill in the fields of the message */
            MessageQ_msgInit (msg);
            msg->msgSize = size;
            msg->heapId  = heapId;

            if (EXPECT_FALSE (MessageQ_module->cfg.traceFlag == TRUE)) {
                GT_3trace (curTrace,
                           GT_4CLASS,
                           "MessageQ_alloc",
                           (msg),
                           ((msg)->seqNum),
                           ((msg)->srcProc));
            }

            id = SharedRegion_getId(msg);

            if ((id != SharedRegion_INVALIDREGIONID) &&
                SharedRegion_isCacheEnabled(id)) {
                Cache_wbInv(msg, size, Cache_Type_ALL, TRUE);
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_alloc", msg);

    return msg;
}


/* Frees the message back to the heap that was used to allocate it. */
Int
MessageQ_free (MessageQ_Msg msg)
{
    UInt32         status = MessageQ_S_SUCCESS;
    IHeap_Handle   heap   = NULL;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_free", msg);

    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_free",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (msg == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_free",
                             MessageQ_E_INVALIDMSG,
                             "msg passed is null!");
    }
    else if (EXPECT_FALSE (msg->heapId == MessageQ_STATICMSG)) {
        status = MessageQ_E_CANNOTFREESTATICMSG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_free",
                             MessageQ_E_CANNOTFREESTATICMSG,
                             "Static message has been passed!");
    }
    else if (EXPECT_FALSE (msg->heapId >= MessageQ_module->numHeaps)) {
        status = MessageQ_E_INVALIDHEAPID;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_free",
                             MessageQ_E_INVALIDHEAPID,
                             "Heap id is invalid!");
    }
    else if (EXPECT_FALSE (MessageQ_module->heaps [msg->heapId] == NULL)) {
        status = MessageQ_E_INVALIDHEAPID;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_free",
                             MessageQ_E_INVALIDHEAPID,
                             "Heap is not registered with MessageQ module!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_FALSE (    (MessageQ_module->cfg.traceFlag == TRUE)
                          ||  (msg->flags & MessageQ_TRACEMASK) != 0)) {
            GT_3trace (curTrace,
                       GT_4CLASS,
                       "MessageQ_free",
                       (msg),
                       ((msg)->seqNum),
                       ((msg)->srcProc));
        }

        heap = MessageQ_module->heaps [msg->heapId];

        Memory_free (heap, msg, msg->msgSize);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_free", status);

    return status;
}


/* Place a message onto a message queue. */
Int MessageQ_put (MessageQ_QueueId queueId,
                  MessageQ_Msg     msg)
{
    Int                       status    = MessageQ_S_SUCCESS;
    UInt16                    dstProcId = (MessageQ_QueueIndex)(queueId >> 16);
    MessageQ_Object        *  obj       = NULL;
    IMessageQTransport_Handle transport = NULL;
    UInt16                    seqNum    = 0u;
    UInt16                    srcProc   = MultiProc_INVALIDID;
    UInt                      priority;
    UInt16                    flags;
    IArg                      key;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_put", queueId, msg);

    GT_assert (curTrace, (queueId != MessageQ_INVALIDMESSAGEQ));
    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_put",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (queueId == MessageQ_INVALIDMESSAGEQ)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_put",
                             status,
                             "queueId is MessageQ_INVALIDMESSAGEQ!");
    }
    else if (EXPECT_FALSE (msg == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_put",
                             status,
                             "msg is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        msg->dstId   = (UInt16)(queueId);
        msg->dstProc = (UInt16)(queueId >> 16);

        if (EXPECT_TRUE (dstProcId != MultiProc_self ())) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (dstProcId >= MultiProc_MAXPROCESSORS)) {
                status = MessageQ_E_INVALIDPROCID;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MessageQ_put",
                                     status,
                                     "ProcId invalid!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Put the high and urgent messages to the high priority
                 * transport */
                priority = (UInt)((msg->flags)& MessageQ_TRANSPORTPRIORITYMASK);

                /* Call the transport associated with this message queue */
                GT_assert (curTrace, (dstProcId < MultiProc_MAXPROCESSORS));
                transport =  MessageQ_module->transports [dstProcId][priority];

                if (EXPECT_TRUE (transport == NULL)) {
                    /* Try the other transport */
                    priority = !priority;
                    transport= MessageQ_module->transports[dstProcId][priority];
                    GT_assert (curTrace, (transport != NULL));
                }

                /* Send to the transport */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (transport == NULL)) {
                    status = MessageQ_E_INVALIDSTATE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "MessageQ_put",
                                         status,
                                         "Transport was not initialized!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* use local vars so msg does not get cached after put */
                    flags = msg->flags;

                    if (EXPECT_FALSE (  (MessageQ_module->cfg.traceFlag == TRUE)
                                      ||(flags & MessageQ_TRACEMASK) != 0)) {
                        /* use local vars so msg does not get cached after put*/
                        seqNum  = msg->seqNum;
                        srcProc = msg->srcProc;
                    }

                    /* put msg to remote processor using transport */
                    status = IMessageQTransport_put (transport, msg);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (status < 0)) {
                        /* Transport returns MessageQ status code, so no
                         * translation needed.
                         */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "MessageQ_put",
                                             status,
                                             "Transport put failed!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        if (EXPECT_FALSE (   (   MessageQ_module->cfg.traceFlag
                                              == TRUE)
                                          || (  msg->flags & MessageQ_TRACEMASK)
                                              != 0)) {
                            GT_4trace (curTrace,
                                       GT_4CLASS,
                                       "MessageQ_put",
                                       msg,
                                       seqNum,
                                       srcProc,
                                       dstProcId);
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else {
            /* enter the resource gate */
            key = IGateProvider_enter(MessageQ_module->resGate);

            /* Assert queueId is valid */
            GT_assert(curTrace,(UInt16)queueId < MessageQ_module->numQueues);

            /* It is a local MessageQ */
            obj = (MessageQ_Object *)
                               (MessageQ_module->queues [(UInt16)(queueId)]);

            /* if obj is null, assume resource tracking removed the queue */
            if (obj == NULL) {
                goto leave;
            }

            /* Assert object is not NULL */
            GT_assert(curTrace,obj != NULL);

            if (EXPECT_FALSE (obj == NULL)) {
                status = MessageQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MessageQ_put",
                                     status,
                                     "MessageQ_Object is NULL failed!");
            }
            else {

                if (EXPECT_FALSE (   (msg->flags & MessageQ_PRIORITYMASK)
                              == MessageQ_URGENTPRI)) {
                    /* Send to the front of the list. */
                    List_putHead ((List_Handle) &obj->highList,
                                  (List_Elem *) msg);
                    /* Override the status with a MessageQ status code. */
                    status = MessageQ_S_SUCCESS;
                }
                else {
                    if (EXPECT_TRUE (    (msg->flags & MessageQ_PRIORITYMASK)
                                ==  MessageQ_NORMALPRI)) {
                        List_put ((List_Handle) &obj->normalList,
                                  (List_Elem *) msg);
                        /* Override the status with a MessageQ status code. */
                        status = MessageQ_S_SUCCESS;
                    }
                    else {
                        List_put ((List_Handle) &obj->highList,
                                  (List_Elem *) msg);
                        /* Override the status with a MessageQ status code. */
                        status = MessageQ_S_SUCCESS;
                    }
                }

                /* Notify the reader. */
                if (EXPECT_TRUE (obj->synchronizer != NULL)) {
                    status = OsalSemaphore_post (obj->synchronizer);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (status < 0)) {
                        /* Override the status with a MessageQ status code. */
                        status = MessageQ_E_FAIL;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "MessageQ_put",
                                             status,
                                             "OsalSemaphore_post put failed!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        status = MessageQ_S_SUCCESS;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

                if (EXPECT_FALSE (    (MessageQ_module->cfg.traceFlag == TRUE)
                                  ||  (msg->flags & MessageQ_TRACEMASK) != 0)) {
                    GT_4trace (curTrace,
                               GT_4CLASS,
                               "MessageQ_put",
                               (msg),
                               ((msg)->seqNum),
                               ((msg)->srcProc),
                               (obj));
                }
            }
leave:
            IGateProvider_leave(MessageQ_module->resGate, key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_put", status);

    return (status);
}


/* Register a heap with MessageQ. */
Int
MessageQ_registerHeap (Ptr heap, UInt16 heapId)
{
    Int  status = MessageQ_S_SUCCESS;
    IArg key;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_registerHeap", heap, heapId);

    GT_assert (curTrace, (heap != NULL));
    GT_assert (curTrace, (heapId < MessageQ_module->numHeaps));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_registerHeap",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (heap == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_registerHeap",
                             status,
                             "Invalid NULL heap provided!");
    }
    else if (EXPECT_FALSE (heapId >= MessageQ_module->numHeaps)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_registerHeap",
                             status,
                             "Invalid heapId!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (MessageQ_module->heaps [heapId] != NULL)) {
            status = MessageQ_E_ALREADYEXISTS;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_registerHeap",
                                 status,
                                 "Specified heap is already registered.!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            MessageQ_module->heaps [heapId] = (IHeap_Handle) heap;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_registerHeap", status);

    return status;
}


/* Unregister a heap with MessageQ. */
Int
MessageQ_unregisterHeap (UInt16 heapId)
{
    Int  status = MessageQ_S_SUCCESS;
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_unregisterHeap", heapId);

    /* Make sure the heapId is valid */
    GT_assert (curTrace, (heapId < MessageQ_module->numHeaps));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_unregisterHeap",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (heapId >= MessageQ_module->numHeaps)) {
        status =  MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_unregisterHeap",
                             status,
                             "Invalid heapId!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();

        /* Assert to make sure user is providing the correct heapId. */
        GT_assert (curTrace, (MessageQ_module->heaps [heapId] != NULL));

        MessageQ_module->heaps [heapId] = NULL;

        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_unregisterHeap", status);

    return status;
}


/* Unblocks a MessageQ */
Void
MessageQ_unblock (MessageQ_Handle handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int               status = MessageQ_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    MessageQ_Object * obj   = (MessageQ_Object *) handle;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_unblock", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_unblock",
                             MessageQ_E_INVALIDARG,
                             "handle passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Set instance to 'unblocked' state */
        obj->unblocked = TRUE;

        /* Signal the synchronizer */
        GT_assert (curTrace, (obj->synchronizer != NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        OsalSemaphore_post (obj->synchronizer);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (status < 0)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_unblock",
                                 status,
                                 "OsalSemaphore_post failed!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_unblock");
}


/* Embeds a source message queue into a message. */
Void
MessageQ_setReplyQueue (MessageQ_Handle   handle,
                        MessageQ_Msg      msg)
{
    MessageQ_Object * obj   = (MessageQ_Object *) handle;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_setReplyQueue", handle, msg);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_setReplyQueue",
                             MessageQ_E_INVALIDARG,
                             "handle passed is null!");
    }
    else if (EXPECT_FALSE (msg == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_setReplyQueue",
                             MessageQ_E_INVALIDMSG,
                             "msg passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        msg->replyId   = (UInt16)(obj->queue);
        msg->replyProc = (UInt16)(obj->queue >> 16);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_setReplyQueue");
}


/* Returns the QueueId associated with the handle. */
MessageQ_QueueId
MessageQ_getQueueId (MessageQ_Handle handle)
{
    MessageQ_Object * obj     = (MessageQ_Object *) handle;
    UInt32            queueId = MessageQ_INVALIDMESSAGEQ;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_getQueueId", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_getQueueId",
                             MessageQ_E_INVALIDARG,
                             "obj passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        queueId = (obj->queue);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_getQueueId", queueId);

    return queueId;
}

/* Sets the tracing of a message */
Void
MessageQ_setMsgTrace (MessageQ_Msg msg, Bool traceFlag)
{
    GT_2trace (curTrace, GT_ENTER, "MessageQ_setMsgTrace", msg, traceFlag);

    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (msg == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_setMsgTrace",
                             MessageQ_E_INVALIDARG,
                             "msg passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        msg->flags =    (msg->flags & ~MessageQ_TRACEMASK)
                     |  (traceFlag << MessageQ_TRACESHIFT);
        GT_4trace (curTrace,
                   GT_1CLASS,
                   "MessageQ_setMsgTrace",
                    msg,
                    msg->seqNum,
                    msg->srcProc,
                    traceFlag);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_setMsgTrace");
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*!
 *  @brief   Grow the MessageQ table
 *
 *  @param   obj     Pointer to the MessageQ object.
 *
 *  @sa      MessageQ_create
 */
UInt16
_MessageQ_grow (MessageQ_Object * obj)
{
    UInt16            queueIndex = MessageQ_module->numQueues;
    UInt16            oldSize;
    MessageQ_Handle * queues;
    MessageQ_Handle * oldQueues;

    GT_1trace (curTrace, GT_ENTER, "_MessageQ_grow", obj);

    GT_assert (curTrace, (obj != NULL));

    /* No parameter validation required since this is an internal function. */

    oldSize = (MessageQ_module->numQueues) * sizeof (MessageQ_Handle);

    /* Allocate larger table */
    queues = Memory_calloc (NULL,
                            (oldSize + sizeof (MessageQ_Handle)),
                            0,
                            NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (queues == NULL)) {
        queueIndex = MessageQ_INVALIDMESSAGEQ;
        /*! @retval Existing-numQueues Growing the MessageQ failed */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MessageQ_grow",
                             MessageQ_INVALIDMESSAGEQ,
                             "Growing the MessageQ failed!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Copy contents into new table */
        memcpy (queues, MessageQ_module->queues, oldSize);

        /* Fill in the new entry */
        queues[queueIndex] = (MessageQ_Handle)obj;

        /* Hook-up new table */
        oldQueues = MessageQ_module->queues;
        MessageQ_module->queues = queues;
        MessageQ_module->numQueues++;

        /* Delete old table if not statically defined */
        if (EXPECT_FALSE (MessageQ_module->canFreeQueues == TRUE)) {
            Memory_free (NULL, oldQueues, oldSize);
        }
        else {
            MessageQ_module->canFreeQueues = TRUE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

     GT_1trace (curTrace, GT_LEAVE, "_MessageQ_grow", queueIndex);

      /*! @retval Valid-queue-index Operation Successsful */
     return (queueIndex);
}


/* Returns the amount of shared memory used by one transport instance.
 *
 *  The MessageQ module itself does not use any shared memory but the
 *  underlying transport may use some shared memory.
 */
SizeT
MessageQ_sharedMemReq (Ptr sharedAddr)
{
    SizeT memReq = 0u;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_sharedMemReq", sharedAddr);

    if (EXPECT_TRUE (MultiProc_getNumProcessors ()  > 1)) {
        /* Determine device-specific shared memory requirements */
        memReq = MessageQ_SetupTransportProxy_sharedMemReq (sharedAddr);
    }

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_sharedMemReq", memReq);

    return (memReq);
}


/* Calls the SetupProxy to setup the MessageQ transports. */
Int
MessageQ_attach (UInt16 remoteProcId, Ptr sharedAddr)
{
    Int status = MessageQ_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_attach", remoteProcId, sharedAddr);

    if (EXPECT_TRUE (MultiProc_getNumProcessors () > 1)) {
        /* Use the MessageQ_SetupTransportProxy to attach transports */
        status = MessageQ_SetupTransportProxy_attach(remoteProcId, sharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_attach",
                                 status,
                                 "Failed in transport setup!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    else {
        status = MessageQ_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_attach",
                             status,
                             "Function called incorrectly!");
    }

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_attach", status);

    /*! @retval MessageQ_S_SUCCESS Operation successfully completed! */
    return status;
}


/* Calls the SetupProxy to detach the MessageQ transports. */
Int
MessageQ_detach (UInt16 remoteProcId)
{
    Int status = MessageQ_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_detach", remoteProcId);

    if (EXPECT_TRUE (MultiProc_getNumProcessors () > 1)) {
        /* Use the MessageQ_SetupTransportProxy to detach transports */
        status = MessageQ_SetupTransportProxy_detach (remoteProcId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (status < 0)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_detach",
                                 status,
                                 "Failed in transport detach!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    else {
        status = MessageQ_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_detach",
                             status,
                             "Function called incorrectly!");
    }

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_detach", status);

    /*! @retval MessageQ_S_SUCCESS Operation successfully completed! */
    return status;
}


/* Register a transport with the MessageQ module. This API is called
 * by the transport when it is created.
 */
Int
MessageQ_registerTransport (IMessageQTransport_Handle   handle,
                            UInt16                      procId,
                            UInt                        priority)

{
    Int  status = MessageQ_S_SUCCESS;
    IArg key;

    GT_3trace (curTrace, GT_ENTER, "MessageQ_registerTransport",
               handle, procId, priority);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MessageQ_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_registerTransport",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_registerTransport",
                             status,
                             "Invalid NULL handle provided!");
    }
    else if (EXPECT_FALSE (procId >= MultiProc_MAXPROCESSORS)) {
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_registerTransport",
                             status,
                             "Invalid procId!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();

        /* Make sure the id is not already in use */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (   MessageQ_module->transports [procId][priority]
                          != NULL)) {
            status = MessageQ_E_ALREADYEXISTS;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MessageQ_registerTransport",
                                 status,
                                 "Specified transport is already registered!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            MessageQ_module->transports [procId][priority] = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MessageQ_registerTransport", status);

    return status;
}


/* Unregister a transport with MessageQ. */
Void
MessageQ_unregisterTransport (UInt16 procId, UInt priority)
{
    IArg key;

    GT_2trace (curTrace, GT_ENTER, "MessageQ_unregisterTransport",
               procId, priority);

    /* Make sure the procId is valid */
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_unregisterTransport",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (procId >= MultiProc_MAXPROCESSORS)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_unregisterTransport",
                             MessageQ_E_INVALIDARG,
                             "Invalid procId!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();

        /* Assert to make sure user is providing the correct procId. */
        GT_assert (curTrace,
                   (MessageQ_module->transports [procId][priority] != NULL));

        MessageQ_module->transports [procId][priority] = NULL;

        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_unregisterTransport");
}


/* This is a helper function to initialize a message. */
static
Void
MessageQ_msgInit (MessageQ_Msg msg)
{
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "MessageQ_msgInit", msg);

    GT_assert (curTrace, (msg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MessageQ_module->refCount),
                                                MessageQ_MAKE_MAGICSTAMP(0),
                                                MessageQ_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_msgInit",
                             MessageQ_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (msg == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MessageQ_msgInit",
                             MessageQ_E_INVALIDARG,
                             "Invalid NULL msg specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        msg->replyId = (UInt16) MessageQ_INVALIDMESSAGEQ;
        msg->msgId   = MessageQ_INVALIDMSGID;
        msg->dstId   = (UInt16) MessageQ_INVALIDMESSAGEQ;
        msg->flags   =   MessageQ_HEADERVERSION
                       | MessageQ_NORMALPRI;
        msg->srcProc = MultiProc_self ();

        key = Gate_enterSystem();
        msg->seqNum  = MessageQ_module->seqNum++;
        Gate_leaveSystem(key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MessageQ_msgInit");
}

/*
 *  ======== MessageQ_setState ========
 */
Void MessageQ_setState(MessageQ_Handle handle, Int state)
{
    MessageQ_Object *obj = (MessageQ_Object *)handle;

    obj->state = state;
}
