/*
 *  @file   NotifyDriverCirc.c
 *
 *  @brief      Notify Circular Buffer driver implementation
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

/* OSAL Headers  */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_Ipc.h>

/* Notify Headers */
#include <ti/ipc/Notify.h>
#include <ti/syslink/inc/knl/_NotifyDefs.h>
#include <ti/syslink/inc/INotifyDriver.h>
#include <ti/syslink/inc/NotifyDriverCirc.h>
#include <ti/syslink/inc/knl/_NotifyDriverCirc.h>
#include <ti/ipc/SharedRegion.h>

/* HAL Headers */
#include <ti/syslink/inc/knl/_ArchIpcInt.h>
#include <ti/syslink/inc/knl/ArchIpcInt.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

/*!
 *  @brief  Get address of event entry.
 */
#define EVENTENTRY(eventChart, align, eventId) \
            ((NotifyDriverCirc_EventEntry *) \
             ((UInt32)eventChart + (align * eventId)));

/*!
 *  @brief  Stamp indicating that the Notify Shared Memory driver on the
 *          processor has been initialized.
 */
#define NotifyDriverCirc_INIT_STAMP      0xA9C8B7D6

/*!
 *  @brief  Flag indicating event is set.
 */
#define NotifyDriverCirc_UP    1

/*!
 *  @brief  Flag indicating event is not set.
 */
#define NotifyDriverCirc_DOWN  0

/* Macro to make a correct module magic number with refCount */
#define NotifyDriverCirc_MAKE_MAGICSTAMP(x) \
                            ((NotifyDriverCirc_MODULEID << 12u) | (x))


/*!
 *  @brief   Defines the NotifyDriverCirc state object, which contains all the
 *           module specific information.
 */
typedef struct NotifyDriverCirc_ModuleObject_tag {
    Atomic                           refCount;
    /* Reference count */
    NotifyDriverCirc_Config           cfg;
    /*!< NotifyDriverCirc configuration structure */
    NotifyDriverCirc_Config           defCfg;
    /*!< Default module configuration */
    NotifyDriverCirc_Params           defInstParams;
    /*!< Default instance parameters */
    IGateProvider_Handle             gateHandle;
    /*!< Handle to the gate for local thread safety */
    INotifyDriver_Handle             driverHandles [MultiProc_MAXPROCESSORS]
                                                        [Notify_MAX_INTLINES];
    /*!< Loader handle array. */
    Bool                             enableStats;
    /*!<
     *  Enable statistics for sending an event
     *
     *  If this parameter is to 'TRUE' and 'waitClear' is also set to
     *  TRUE when calling, then the module keeps track of the number of times
     *  the processor spins waiting for an empty slot and the max amount of time
     *  it waits.
     */
    UInt                             numMsgs;
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
    UInt                             maxIndex;
    /*!< The max index set to (numMsgs - 1) */
    UInt                             modIndex;
    /*!<
     *  The modulo index value. Set to (numMsgs / 4).
     *  Used in the isr for doing cache_wb of readIndex.
     */
} NotifyDriverCirc_ModuleObject;

/*!
 *  @brief  NotifyDriverCirc instance object.
 */
struct NotifyDriverCirc_Object_tag {
    NotifyDriverCirc_Params          params;
    /*!< Instance parameters (configuration values) */
    NotifyDriverCirc_EventEntry     *putBuffer;
    /*!< buffer used to put events */
    Bits32                          *putReadIndex;
    /*!< ptr to readIndex for put buffer */
    Bits32                          *putWriteIndex;
    /*!< ptr to writeIndex for put buffer */
    NotifyDriverCirc_EventEntry     *getBuffer;
    /*!< buffer used to get events */
    Bits32                          *getReadIndex;
    /*!< ptr to readIndex for get buffer */
    Bits32                          *getWriteIndex;
    /*!< ptr to writeIndex for get buffer */
    Bits32                           evtRegMask;
    /*!< local event register mask */
    SizeT                            opCacheSize;
    /*!< optimized cache size for wb/inv */
    UInt32                           spinCount;
    /*!< number of times sender waits */
    Notify_Handle                    notifyHandle;
    /*!< Common NotifyDriver handle */
    UInt16                           remoteProcId;
    /*!< Processor ID of the remote processor which which this driver instance
         communicates. */
    UInt32                           cacheEnabled;
    /*!< Whether to perform cache calls */
    UInt16                           lineId;
    /*!< Interrupt line number being worked on by the driver */
};


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    NotifyDriverCirc_state
 *
 *  @brief  Notify state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NotifyDriverCirc_ModuleObject NotifyDriverCirc_state =
{
    .enableStats = FALSE,
    .numMsgs = 32u,
    .maxIndex = (32u - 1u), /* numMsgs - 1 */
    .modIndex = (32u / 4u), /* numMsgs / 4 */
    .gateHandle = NULL,
    .defInstParams.sharedAddr = 0x0,
    .defInstParams.cacheEnabled = FALSE,
    .defInstParams.cacheLineSize = 128u,
    .defInstParams.remoteProcId = MultiProc_INVALIDID,
    .defInstParams.lineId = 0x0,
    .defInstParams.localIntId = (UInt32) -1,
    .defInstParams.remoteIntId = (UInt32) -1,
};

#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NotifyDriverCirc_ModuleObject * NotifyDriverCirc_module =
                                            &NotifyDriverCirc_state;

/* Extern declaration to Notify state object variable */
extern Notify_ModuleObject Notify_state;

#if defined  (SYSLINK_INT_LOGGING)
extern UInt32 SysLogging_intCount;
extern UInt32 SysLogging_checkAndClearCount;
extern UInt32 SysLogging_isrCount;
extern UInt32 SysLogging_threadCount;
extern UInt32 SysLogging_NotifyCallbackCount;
#endif /* if defined  (SYSLINK_INT_LOGGING) */


/* =============================================================================
 * Forward declarations of internal functions.
 * =============================================================================
 */
/* This function implements the interrupt service routine for the interrupt
 * received from the remote processor.
 */
static Bool _NotifyDriverCirc_ISR (Void * refData);


/* =============================================================================
 * APIs called directly by applications
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the NotifyDriverCirc module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to NotifyDriverCirc_setup filled in by
 *              the NotifyDriverCirc module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfg        Pointer to the NotifyDriverCirc module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         NotifyDriverCirc_setup
 */
Void
NotifyDriverCirc_getConfig (NotifyDriverCirc_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "NotifyDriverCirc_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_getConfig",
                             Notify_E_INVALIDARG,
                             "Argument of type (NotifyDriverCirc_Config *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(NotifyDriverCirc_state.refCount),
                                      NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                                      NotifyDriverCirc_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfg,
                         &(NotifyDriverCirc_state.defCfg),
                         sizeof (NotifyDriverCirc_Config));
        }
        else {
            Memory_copy (cfg,
                         &(NotifyDriverCirc_state.cfg),
                         sizeof (NotifyDriverCirc_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverCirc_getConfig");
}


/*!
 *  @brief      Setup the NotifyDriverCirc module.
 *
 *              This function sets up the NotifyDriverCirc module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then NotifyDriverCirc_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call NotifyDriverCirc_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional NotifyDriverCirc module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         NotifyDriverCirc_destroy
 *              GateSpinlock_create
 */
Int
NotifyDriverCirc_setup (NotifyDriverCirc_Config * cfg)
{
    Int                    status = Notify_S_SUCCESS;
    Error_Block            eb;
    NotifyDriverCirc_Config tmpCfg;
    UInt16                 i;
    UInt16                 j;
    GT_1trace (curTrace, GT_ENTER, "NotifyDriverCirc_setup", cfg);

    Error_init (&eb);

    if (cfg == NULL) {
        NotifyDriverCirc_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&NotifyDriverCirc_state.refCount,
                            NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                            NotifyDriverCirc_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&NotifyDriverCirc_state.refCount)
        != NotifyDriverCirc_MAKE_MAGICSTAMP(1u)) {
        status = Notify_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "NotifyDriverCirc Module already initialized!");
    }
    else {
        /* Create a default gate handle for local module protection. */
        NotifyDriverCirc_state.gateHandle = (IGateProvider_Handle)
                   GateSpinlock_create ((GateSpinlock_Params *) NULL, (Ptr)&eb);

        /* Initialize the driver handles. */
        for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
            for (j = 0 ; j < Notify_MAX_INTLINES ; j++) {
                NotifyDriverCirc_state.driverHandles [i][j] = NULL;
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NotifyDriverCirc_state.gateHandle == NULL) {
            Atomic_set (&NotifyDriverCirc_state.refCount,
                        NotifyDriverCirc_MAKE_MAGICSTAMP(0));
            /*! @retval Notify_E_FAIL Failed to create GateSpinlock! */
            status = Notify_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyDriverCirc_setup",
                                 status,
                                 "Failed to create GateSpinlock!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&NotifyDriverCirc_state.cfg,
                         cfg,
                         sizeof (NotifyDriverCirc_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_setup", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the NotifyDriverCirc module.
 *
 *              Once this function is called, other NotifyDriverCirc module APIs,
 *              except for the NotifyDriverCirc_getConfig API cannot be called
 *              anymore.
 *
 *  @sa         NotifyDriverCirc_setup
 *              GateSpinlock_delete
 */
Int
NotifyDriverCirc_destroy (Void)
{
    Int    status = Notify_S_SUCCESS;
    UInt16 i;
    UInt16 j;

    GT_0trace (curTrace, GT_ENTER, "NotifyDriverCirc_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverCirc_state.refCount),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval Notify_E_INVALIDSTATE Module was not initialized */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&NotifyDriverCirc_state.refCount)
            == NotifyDriverCirc_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&NotifyDriverCirc_state.refCount,
                        NotifyDriverCirc_MAKE_MAGICSTAMP(1));

            /* Check if any NotifyDriverCirc instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                for (j = 0 ; j < Notify_MAX_INTLINES ; j++) {
                    if (NotifyDriverCirc_state.driverHandles [i][j] != NULL) {
                        NotifyDriverCirc_delete (
                               &(NotifyDriverCirc_state.driverHandles [i][j]));
                    }
                }
            }

            /* Reset the refCount */
            Atomic_set (&NotifyDriverCirc_state.refCount,
                        NotifyDriverCirc_MAKE_MAGICSTAMP(0));

            /* Check if the gateHandle was created internally. */
            if (NotifyDriverCirc_state.gateHandle != NULL) {
                GateSpinlock_delete ((GateSpinlock_Handle *)
                                     &(NotifyDriverCirc_state.gateHandle));
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_destroy", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for this NotifyDriver
 *              instance.
 *
 *  @param      handle  If specified as NULL, default parameters are returned.
 *                      If not NULL, the parameters as configured for this
 *                      instance are returned.
 *  @param      params  Configuration parameters to be returned
 *
 *  @sa         NotifyDriverCirc_create
 */
Void
NotifyDriverCirc_Params_init (NotifyDriverCirc_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "NotifyDriverCirc_Params_init",
               params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverCirc_state.refCount),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_Params_init",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_Params_init",
                             Notify_E_INVALIDARG,
                             "Argument of type (NotifyDriverCirc_Params *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(NotifyDriverCirc_state.defInstParams),
                     sizeof (NotifyDriverCirc_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverCirc_Params_init");
}


/*!
 *  @brief      Function to create an instance of this NotifyDriver.
 *
 *  @param      driverName  Name of the NotifyDriver instance.
 *  @param      params      Configuration parameters.
 *
 *  @sa         NotifyDriverCirc_delete, Notify_registerDriver, List_create,
 *              OsalIsr_install, OsalIsr_create
 */
INotifyDriver_Handle
NotifyDriverCirc_create (const NotifyDriverCirc_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                          status    = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NotifyDriverCirc_Object *    obj       = NULL;
    INotifyDriver_Object *       notifyDrvObj = NULL;
    IArg                         key;
    UInt16                       regionId;
    UInt16                       localIndex;
    UInt16                       remoteIndex;
    SizeT                        regionCacheSize;
    SizeT                        minAlign;
    SizeT                        ctrlSize;
    SizeT                        circBufSize;
    SizeT                        totalSelfSize;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverCirc_create", params);

    GT_assert (curTrace, (params != NULL));

#if defined  (SYSLINK_INT_LOGGING)
    /* Initialize logging values */
    SysLogging_isrCount = 0;
    SysLogging_checkAndClearCount = 0;
    SysLogging_intCount = 0;
    SysLogging_threadCount = 0;
    SysLogging_NotifyCallbackCount = 0;
#endif /* if defined  (SYSLINK_INT_LOGGING) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverCirc_state.refCount),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_create",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL Invalid NULL params specified! */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_create",
                              Notify_E_INVALIDARG,
                             "Invalid NULL params specified!");
    }
    else if (   (params->remoteProcId == MultiProc_INVALIDID)
             || (params->remoteProcId == MultiProc_self ())) {
         /*! @retval NULL Invalid remoteProcId specified. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_create",
                             status,
                             "Invalid remoteProcId specified.");
    }
    else if (params->lineId >= Notify_MAX_INTLINES) {
         /*! @retval NULL Invalid lineId specified. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_create",
                             status,
                             "Invalid lineId specified.");
    }
    else if (   ((UInt32) params->sharedAddr % (UInt32) params->cacheLineSize)
             != 0) {
         /*! @retval NULL Shared memory base address is not aligned to cache
                          line size. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_create",
                             status,
                             "Shared memory base address is not aligned "
                             "to cache line size.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (NotifyDriverCirc_state.gateHandle);

        /* Allocate memory for the INotifyDriver_Object object. */
        notifyDrvObj = Memory_calloc (NULL,
                             sizeof (INotifyDriver_Object),
                             0u,
                             NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (notifyDrvObj == NULL) {
            /*! @retval NULL Failed to allocate memory for
                             INotifyDriver_Object! */
            status = Notify_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyDriverCirc_create",
                                 Notify_E_MEMORY,
                                 "Failed to allocate memory for "
                                 "INotifyDriver_Object!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Function table information */
            notifyDrvObj->fxnTable.registerEvent =
                                        (INotifyDriver_RegisterEventFxn)
                                                &NotifyDriverCirc_registerEvent;
            notifyDrvObj->fxnTable.unregisterEvent =
                                     (INotifyDriver_UnregisterEventFxn)
                                            &NotifyDriverCirc_unregisterEvent;
            notifyDrvObj->fxnTable.sendEvent = (INotifyDriver_SendEventFxn)
                                                    &NotifyDriverCirc_sendEvent;
            notifyDrvObj->fxnTable.disable = (INotifyDriver_DisableFxn)
                                                    &NotifyDriverCirc_disable;
            notifyDrvObj->fxnTable.enable  = (INotifyDriver_EnableFxn)
                                                        &NotifyDriverCirc_enable;
            notifyDrvObj->fxnTable.disableEvent= (INotifyDriver_DisableEventFxn)
                                                &NotifyDriverCirc_disableEvent;
            notifyDrvObj->fxnTable.enableEvent = (INotifyDriver_EnableEventFxn)
                                                &NotifyDriverCirc_enableEvent;
            notifyDrvObj->fxnTable.setNotifyHandle =
                                        (INotifyDriver_SetNotifyHandleFxn)
                                            &NotifyDriverCirc_setNotifyHandle;

            /* Allocate memory for the NotifyDriverCirc_Object object. */
            obj = Memory_calloc (NULL,
                                 sizeof (NotifyDriverCirc_Object),
                                 0u,
                                 NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj == NULL) {
                /*! @retval NULL Failed to allocate memory for
                                 NotifyDriverCirc_Object! */
                status = Notify_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDriverCirc_create",
                                     Notify_E_MEMORY,
                                     "Failed to allocate memory for "
                                     "NotifyDriverCirc_Object!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                notifyDrvObj->obj = obj;

                /* Copy params into instance object. */
                Memory_copy (&(obj->params),
                             (Ptr) params,
                             sizeof (NotifyDriverCirc_Params));

                /* Set the handle in the driverHandles array. */
                NotifyDriverCirc_state.driverHandles
                        [params->remoteProcId] [params->lineId] = notifyDrvObj;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status >= 0 ) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /*
             * Determine obj->cacheEnabled using params->cacheEnabled and
             * SharedRegion cache flag setting, if applicable.
             */
            obj->cacheEnabled = params->cacheEnabled;
            minAlign = params->cacheLineSize;
            if (minAlign == 0) {
                /* Fix alignment of zero */
                minAlign = sizeof(Ptr);
            }
            regionId = SharedRegion_getId (params->sharedAddr);
            if (regionId != SharedRegion_INVALIDREGIONID) {
                /*
                 *  Override the user cacheEnabled setting if the region
                 *  cacheEnabled is FALSE.
                 */
                if (!SharedRegion_isCacheEnabled(regionId)) {
                    obj->cacheEnabled = FALSE;
                }

                regionCacheSize = SharedRegion_getCacheLineSize(regionId);

                /*
                 *  Override the user cache line size setting if the region
                 *  cache line size is smaller.
                 */
                if (regionCacheSize < minAlign) {
                    minAlign = regionCacheSize;
                }
            }

            /* Check if shared memory base addr is aligned to cache line
             * boundary.
             */
            GT_assert(curTrace, ((UInt32) params->sharedAddr % minAlign == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((UInt32) params->sharedAddr % minAlign != 0) {
                status = Notify_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NotifyDriverCirc_create", status,
                        "params->sharedAddr has invalid cache alignment");
            }
            else {
#endif
                obj->remoteProcId = params->remoteProcId;
                obj->lineId = params->lineId;

                if (params->remoteProcId > MultiProc_self()) {
                    localIndex  = 0;
                    remoteIndex = 1;
                }
                else {
                    localIndex  = 1;
                    remoteIndex = 0;
                }

                /* counters for capturing spin wait statistics */
                obj->spinCount = 0;

                /* calculate the circular buffer size one-way */
                circBufSize = _Ipc_roundup((sizeof(NotifyDriverCirc_EventEntry) *
                        NotifyDriverCirc_module->numMsgs), minAlign);

                /* calculate the control size one-way */
                ctrlSize = _Ipc_roundup(sizeof(Bits32), minAlign);

                /* calculate the total size one-way */
                totalSelfSize =  circBufSize + (2 * ctrlSize);

                /*
                 *  Init put/get buffer and index pointers.
                 *  These are all on different cache lines.
                 */
                obj->putBuffer = (NotifyDriverCirc_EventEntry *)
                        ((UInt32)params->sharedAddr +
                        (localIndex * totalSelfSize));

                obj->putWriteIndex = (Bits32 *)
                        ((UInt32)obj->putBuffer + circBufSize);

                obj->putReadIndex = (Bits32 *)
                        ((UInt32)obj->putWriteIndex + ctrlSize);

                obj->getBuffer = (NotifyDriverCirc_EventEntry *)
                        ((UInt32)params->sharedAddr
                        +  (remoteIndex * totalSelfSize));

                obj->getWriteIndex = (Bits32 *) ((UInt32)obj->getBuffer
                        + circBufSize);

                obj->getReadIndex = (Bits32 *)((UInt32)obj->getWriteIndex
                        + ctrlSize);

                /*
                 *  Calculate the size for cache wb/inv in sendEvent and isr.
                 *  This size is the circular buffer + putWriteIndex.
                 *  [sizeof(EventEntry) * numMsgs] + [the sizeof(Ptr)]
                 *  aligned to a cache line.
                 */
                obj->opCacheSize = ((UInt32)obj->putReadIndex
                        - (UInt32)obj->putBuffer);

                /* init the putWrite and putRead Index to 0 */
                obj->putWriteIndex[0] = 0;
                obj->putReadIndex[0] = 0;

                /* cache wb the putWrite/Read Index but no need to inv them */
                if (obj->cacheEnabled) {
                    Cache_wb(obj->putWriteIndex, sizeof(Bits32),
                            Cache_Type_ALL, TRUE);

                    Cache_wb(obj->putReadIndex, sizeof(Bits32),
                            Cache_Type_ALL, TRUE);

                   /* invalidate any stale data of the get buffer and indexes */
                   Cache_inv(obj->getBuffer, totalSelfSize,
                            Cache_Type_ALL, TRUE);
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }

        if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Register the incoming interrupt */
            IGateProvider_leave (NotifyDriverCirc_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                     ArchIpcInt_interruptRegister (obj->remoteProcId,
                                                   obj->params.localIntId,
                                                   _NotifyDriverCirc_ISR,
                                                   (Ptr) obj);
            IGateProvider_enter (NotifyDriverCirc_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                status = Notify_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NotifyDriverCirc_create", status,
                        "ArchIpcInt_interruptRegister failed");
            }
            /* Indicate that the driver is initialized for this processor
             * only when the corresponding Notify driver is also created,
             * i.e. in NotifyDriverCirc_setNotifyHandle.
             */
        }

        /* Cleanup on error. */
        if (status < 0) {
            /* Check if obj was allocated. */
            if (obj != NULL) {
                ArchIpcInt_interruptUnregister(obj->remoteProcId,
                        obj->params.localIntId, (Ptr)obj);

                Memory_free(NULL, obj, sizeof(NotifyDriverCirc_Object));
                obj = NULL;

                Memory_free(NULL, notifyDrvObj, sizeof(INotifyDriver_Object));
                notifyDrvObj = NULL;
                /* Clear the NotifyDriverCirc handle in the local array. */
                GT_assert (curTrace,
                           (params->remoteProcId < MultiProc_MAXPROCESSORS));
                GT_assert (curTrace,
                           (params->lineId < Notify_MAX_INTLINES));
                NotifyDriverCirc_state.driverHandles
                                [params->remoteProcId] [params->lineId] = NULL;
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (NotifyDriverCirc_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_create", notifyDrvObj);

    /*! @retval Driver-handle Operation successfully completed. */
    return ((INotifyDriver_Handle) notifyDrvObj);
}


/*!
 *  @brief      Function to delete the instance of shared memory driver
 *
 *  @param      handlePtr   Pointer to NotiyDriverShm handle. This is reset
 *                          upon completion of the API.
 *
 *  @sa         NotifyDriverCirc_create, Notify_unregisterDriver, List_delete,
 *              OsalIsr_uninstall, OsalIsr_delete
 */
Int NotifyDriverCirc_delete(INotifyDriver_Handle *handlePtr)
{
    Int                       status       = Notify_S_SUCCESS;
    Int                       tmpStatus    = Notify_S_SUCCESS;
    NotifyDriverCirc_Object * obj          = NULL;
    INotifyDriver_Object *    notifyDrvObj = NULL;
    SizeT                     sizeToInv;

    GT_1trace(curTrace, GT_ENTER, "NotifyDriverCirc_delete", handlePtr);

    GT_assert(curTrace, (handlePtr != NULL));
    GT_assert(curTrace, (handlePtr != NULL) && (*handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(NotifyDriverCirc_state.refCount),
            NotifyDriverCirc_MAKE_MAGICSTAMP(0),
            NotifyDriverCirc_MAKE_MAGICSTAMP(1)) == TRUE) {
        GT_setFailureReason(curTrace, GT_4CLASS, "NotifyDriverCirc_delete",
                Notify_E_INVALIDSTATE, "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = Notify_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "NotifyDriverCirc_delete",
                status, "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        status = Notify_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "NotifyDriverCirc_delete",
                status, "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        notifyDrvObj = (INotifyDriver_Object *) (*handlePtr);
        obj = (NotifyDriverCirc_Object *) notifyDrvObj->obj;

        if (obj != NULL) {
            tmpStatus = ArchIpcInt_interruptUnregister(obj->remoteProcId,
                    obj->params.localIntId, (Ptr)obj);
            if ((status >= 0) && (tmpStatus < 0)) {
                status = tmpStatus;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NotifyDriverCirc_delete", status,
                        "ArchIpcInt_interruptUnregister failed!");
            }

            if (obj->cacheEnabled) {
                if (obj->remoteProcId > MultiProc_self()) {
                    /* calculate the size of the buffer and indexes for one side */
                    sizeToInv = ((UInt32)obj->getBuffer - (UInt32)obj->putBuffer);

                        /* invalidate the shared memory for this instance */
                        Cache_inv(obj->putBuffer, (sizeToInv * 2),
                                Cache_Type_ALL, TRUE);
                }
                else {
                    /* calculate the size of the buffer and indexes for one side */
                    sizeToInv = ((UInt32)obj->putBuffer - (UInt32)obj->getBuffer);

                    /* invalidate the shared memory for this instance */
                    Cache_inv(obj->getBuffer, (sizeToInv * 2),
                               Cache_Type_ALL, TRUE);
                }
            }

            /* Clear the NotifyDriverCirc handle in the local array. */
            GT_assert (curTrace,
                       (obj->params.remoteProcId < MultiProc_MAXPROCESSORS));
            GT_assert (curTrace,
                       (obj->params.lineId < Notify_MAX_INTLINES));
            NotifyDriverCirc_state.driverHandles
                    [obj->params.remoteProcId] [obj->params.lineId] = NULL;
            Memory_free(NULL, obj, sizeof(NotifyDriverCirc_Object));
            obj = NULL;
        }

        Memory_free (NULL, notifyDrvObj, sizeof (INotifyDriver_Object));
        notifyDrvObj = NULL;

        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

#if defined (SYSLINK_INT_LOGGING)
    Osal_printf ("****** Printing logging SysLogging_isrCount           : %d"
                 " ******\n",
                 SysLogging_isrCount);
    Osal_printf ("****** Printing logging SysLogging_checkAndClearCount : %d"
                 " ******\n",
                 SysLogging_checkAndClearCount);
    Osal_printf ("****** Printing logging SysLogging_intCount           : %d"
                 " ******\n",
                 SysLogging_intCount);
    Osal_printf ("****** Printing logging SysLogging_threadCount        : %d"
                 " ******\n",
                 SysLogging_threadCount);
    Osal_printf ("****** Printing logging SysLogging_NotifyCallbackCount: %d"
                 " ******\n",
                 SysLogging_NotifyCallbackCount);
#endif

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_delete", status);

    return (status);
}


/* =============================================================================
 * APIs called by Notify module (part of function table interface)
 * =============================================================================
 */
/*!
 *  @brief      Register a callback for an event with the Notify driver.
 *
 *  @param      handle       handle to notify driver
 *  @param      eventId      event to be registered
 *
 *  @sa         NotifyDriverCirc_unregisterEvent, List_put
 *
 */
Int NotifyDriverCirc_registerEvent(NotifyDriverCirc_Handle handle,
        UInt32 eventId)
{
    Int                             status    = Notify_S_SUCCESS;
    NotifyDriverCirc_Object *       obj;
    IArg                            key;

    GT_2trace(curTrace, GT_ENTER, "NotifyDriverCirc_registerEvent", handle,
            eventId);

    GT_assert(curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyCircDriver, so check its validity. */
    if (handle == NULL) {
        status = Notify_E_DRIVERNOTREGISTERED;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "NotifyDriverCirc_registerEvent",
                status, "Notify driver object is NULL.");
    }
    else {
#endif
        obj = (NotifyDriverCirc_Object *) handle;

        key = IGateProvider_enter (NotifyDriverCirc_state.gateHandle);

        /* Set the 'registered' bit */
        SET_BIT(obj->evtRegMask, eventId);

        IGateProvider_leave(NotifyDriverCirc_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "NotifyDriverCirc_registerEvent", status);

    return (status);
}


/*!
 *  @brief      Unregister a callback for an event with the Notify driver.
 *
 *  @param      handle       handle to notify driver
 *  @param      eventId      event to be unregistered
 *
 *  @sa         NotifyDriverCirc_registerEvent, List_remove
 */
Int
NotifyDriverCirc_unregisterEvent (NotifyDriverCirc_Handle handle,
                                 UInt32              eventId)
{
    Int                        status    = Notify_S_SUCCESS;
    NotifyDriverCirc_Object *  obj;
    IArg                       key;

    GT_2trace (curTrace,
               GT_ENTER,
               "NotifyDriverCirc_unregisterEvent",
               handle,
               eventId);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* driverHandle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
       /*! @retval  Notify_E_DRIVERNOTREGISTERED Notify driver object is
                                                 NULL. */
        status = Notify_E_DRIVERNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_unregisterEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverCirc_Object *) handle;

        key = IGateProvider_enter (NotifyDriverCirc_state.gateHandle);

        /* Clear the 'registered' bit */
        CLEAR_BIT (obj->evtRegMask, eventId);

        IGateProvider_leave (NotifyDriverCirc_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_unregisterEvent", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed. */
    return (status);
}


/*!
 *  @brief      Send an event to the remote processor
 *
 *  @param      handle      Handle to the Notify Driver
 *  @param      eventId     Event number to be sent.
 *  @param      payload     Payload to be sent alongwith the event.
 *  @param      waitClear   Indicates whether Notify driver will wait for
 *                          previous event to be cleared. If payload needs to
 *                          be sent across, this must be TRUE.
 *
 *  @sa
 */
Int
NotifyDriverCirc_sendEvent (NotifyDriverCirc_Handle handle,
                           UInt32              eventId,
                           UInt32              payload,
                           Bool                waitClear)
{
    Int                           status   = Notify_S_SUCCESS;
    Bool                          loop     = FALSE;
    Bool                          spinWait = FALSE;
    NotifyDriverCirc_Object *     obj;
    IArg                          sysKey;
    UInt32                        writeIndex;
    UInt32                        readIndex;
    NotifyDriverCirc_EventEntry * eventEntry;

    GT_4trace (curTrace,
               GT_ENTER,
               "NotifyDriverCirc_sendEvent",
               handle,
               eventId,
               payload,
               waitClear);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
       /*! @retval  Notify_E_DRIVERNOTREGISTERED Notify driver object is
                                                 NULL. */
        status = Notify_E_DRIVERNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_sendEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverCirc_Object *) handle;

        /*
         *  Retrieve the get Index. No need to cache inv the
         *  readIndex until out writeIndex wraps. Only need to invalidate
         *  once every N times [N = number of slots in buffer].
         */
        readIndex = obj->putReadIndex [0];

        do {
            /* Enter system gate */
            sysKey = Gate_enterSystem ();

            /* retrieve the put index */
            writeIndex = obj->putWriteIndex[0];

            /* if slot available 'break' out of loop */
            if (((writeIndex + 1) & NotifyDriverCirc_module->maxIndex) != readIndex) {
                break;
            }

            /* Leave system gate */
            Gate_leaveSystem (sysKey);

            /* check to make sure code has looped */
            if (loop && !waitClear) {
                /*! @retval Notify_E_FAIL if no slot available and waitClear
                                             is 'FALSE' */
                status = Notify_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                               GT_4CLASS,
                               "NotifyDriverCirc_sendEvent",
                               status,
                               "No slot is available and waitCLear is FALSE");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                break;
            }

            /* start timestamp for spin wait statistics */
            if (NotifyDriverCirc_module->enableStats) {
                if (loop && !spinWait) {
                    spinWait = TRUE;
                }
            }

            /* cache invalidate the putReadIndex */
            if (obj->cacheEnabled) {
                Cache_inv(obj->putReadIndex,
                          sizeof(Bits32),
                          Cache_Type_ALL,
                          TRUE);
            }

            /* re-read the get count */
            readIndex = obj->putReadIndex[0];

            /* convey that the code has looped around */
            loop = TRUE;
        } while (1);

        if (status >= 0) {
            /* System gate is entered at this point */

            /* increment spinCount and determine the spin wait time */
            if (NotifyDriverCirc_module->enableStats) {
                if (spinWait) {
                    obj->spinCount++;
                }
            }

            /* calculate the next available entry */
            eventEntry = (NotifyDriverCirc_EventEntry *)
                            (   (UInt32) obj->putBuffer
                             +  (  writeIndex
                                 * sizeof(NotifyDriverCirc_EventEntry)));

            /* Set the eventId field and payload for the entry */
            eventEntry->eventid = eventId;
            eventEntry->payload = payload;

            /*
             *  Writeback the event entry. No need to invalidate since
             *  only one processor ever writes here. No need to wait for
             *  cache operation since another cache operation is done below.
             */
            if (obj->cacheEnabled) {
                Cache_wb(eventEntry,
                         sizeof(NotifyDriverCirc_EventEntry),
                         Cache_Type_ALL,
                         FALSE);
            }

            /* update the putWriteIndex */
            obj->putWriteIndex [0] =   (writeIndex + 1)
                                     & NotifyDriverCirc_module->maxIndex;

            /* Leave the system gate */
            Gate_leaveSystem(sysKey);

            /*
             *  Writeback the putWriteIndex.
             *  No need to invalidate since only one processor
             *  ever writes here.
             */
            if (obj->cacheEnabled) {
                Cache_wb(obj->putWriteIndex,
                         sizeof(Bits32),
                         Cache_Type_ALL,
                         TRUE);
            }


            /* Send an interrupt with the event information to the
               remote processor */
            ArchIpcInt_sendInterrupt (obj->remoteProcId,
                                      obj->params.remoteIntId,
                                      eventId);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_sendEvent", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed. */
    return (status);
}


/*!
 *  @brief      Disable all events for specified driver.
 *              This is equivalent to global interrupt disable for specified
 *              processor, however restricted within interrupts handled
 *              by this driver. All callbacks registered for all events
 *              are disabled with this API. It is not possible to disable a
 *              specific callback.
 *
 *  @param      handle       Handle to the NotifyDriver object.
 *
 *  @sa         NotifyDriverCirc_restore
 */
Int
NotifyDriverCirc_disable (NotifyDriverCirc_Handle handle)
{
    NotifyDriverCirc_Object * obj;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverCirc_disable", handle);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_disable",
                             Notify_E_DRIVERNOTREGISTERED,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverCirc_Object *) handle;

        /* Disable the receive interrupt. */
        ArchIpcInt_interruptDisable (obj->remoteProcId,
                                     obj->params.localIntId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_disable", 0x0);

    /*! @retval flags Operation successfully completed. */
    return (0x0); /* No flags to be returned. */
}

/*!
 *  @brief      Enable all events for specified driver. This is
 *              equivalent to global interrupt enable for this driver, however
 *              restricted within interrupts handled by this driver.
 *              All callbacks registered for all events as specified in the
 *              flags are enabled with this API. It is not possible to enable a
 *              specific callback.
 *
 *  @param      handle       Handle to the NotifyDriver object.
 *
 *  @sa         NotifyDriverCirc_disable
 */
Void
NotifyDriverCirc_enable (NotifyDriverCirc_Handle handle)
{
    NotifyDriverCirc_Object * obj;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverCirc_enable", handle);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_enable",
                             Notify_E_DRIVERNOTREGISTERED,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverCirc_Object *) handle;

        /* Enable the receive interrupt. */
        ArchIpcInt_interruptEnable (obj->remoteProcId,
                                    obj->params.localIntId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverCirc_enable");
}


/*!
 *  @brief      Disable a specific event.
 *              All callbacks registered for the specific event are disabled
 *              with this API. It is not possible to disable a specific
 *              callback.
 *
 *  @param      handle    Handle to the Notify Driver
 *  @param      eventId   Event number to be disabled
 *
 *  @sa         NotifyDriverCirc_enableEvent
 */
Void
NotifyDriverCirc_disableEvent (NotifyDriverCirc_Handle handle,
                              UInt32              eventId)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                      status = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NotifyDriverCirc_Object * obj;

    GT_2trace (curTrace,
               GT_ENTER,
               "NotifyDriverCirc_disableEvent",
               handle,
               eventId);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
       /*! @retval  Notify_E_DRIVERNOTREGISTERED Notify driver object is
                                                 NULL. */
        status = Notify_E_DRIVERNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_disableEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverCirc_Object *) handle;
        /* Disable the receive interrupt. */
        ArchIpcInt_interruptDisable (obj->remoteProcId,
                                     obj->params.localIntId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverCirc_disableEvent");
}


/*!
 *  @brief      Enable a specific event.
 *              All callbacks registered for this specific event are enabled
 *              with this API. It is not possible to enable a specific callback.
 *
 *  @param      handle    Handle to the Notify Driver
 *  @param      eventId   Event number to be enabled
 *
 *  @sa         NotifyDriverCirc_disableEvent
 */
Void NotifyDriverCirc_enableEvent(NotifyDriverCirc_Handle handle,
        UInt32 eventId)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                      status = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NotifyDriverCirc_Object * obj;

    GT_2trace(curTrace, GT_ENTER, "NotifyDriverCirc_enableEvent", handle,
            eventId);

    GT_assert(curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyCircDriver, so check its validity. */
    if (handle == NULL) {
        status = Notify_E_DRIVERNOTREGISTERED;
        GT_setFailureReason (curTrace, GT_4CLASS,
                "NotifyDriverCirc_enableEvent", status,
                "Notify driver object is NULL.");
    }
    else {
#endif
        obj = (NotifyDriverCirc_Object *) handle;

        /* Enable the receive interrupt. */
        ArchIpcInt_interruptEnable(obj->remoteProcId, obj->params.localIntId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverCirc_enableEvent");
}


/*!
 *  @brief      Get the shared memory requirements for the NotifyDriverCirc.
 *
 *  @param      params  Configuration parameters.
 *
 *  @sa         NotifyDriverCirc_create
 *
 */
SizeT NotifyDriverCirc_sharedMemReq(const NotifyDriverCirc_Params *params)
{
    SizeT memReq = 0;
    UInt16 regionId;
    SizeT regionCacheSize;
    SizeT minAlign;

    /* Ensure that params is non-NULL */
    GT_assert(curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverCirc_state.refCount),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_sharedMemReq",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL Invalid NULL params specified! */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_sharedMemReq",
                              Notify_E_INVALIDARG,
                             "Invalid NULL params specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /*
         *  Determine obj->cacheEnabled using params->cacheEnabled and SharedRegion
         *  cache flag setting, if applicable.
         */
        minAlign = params->cacheLineSize;
        if (minAlign == 0) {
            /* Fix alignment of zero */
            minAlign = sizeof(Ptr);
        }
        regionId = SharedRegion_getId ((Ptr) params->sharedAddr);
        if (regionId != SharedRegion_INVALIDREGIONID) {
            regionCacheSize = SharedRegion_getCacheLineSize (regionId);

            /*
             *  Override the user cache line size setting if the region
             *  cache line size is smaller.
             */
            if (regionCacheSize < minAlign) {
                minAlign = regionCacheSize;
            }
        }

        /*
         *  Amount of shared memory:
         *  1 putBuffer with numMsgs (rounded to CLS) +
         *  1 putWriteIndex ptr (rounded to CLS) +
         *  1 putReadIndex ptr (rounded to CLS) +
         *  1 getBuffer with numMsgs (rounded to CLS) +
         *  1 getWriteIndex ptr (rounded to CLS) +
         *  1 getReadIndex ptr (rounded to CLS) +
         *
         *  For CLS of 128b it is:
         *      256b + 128b + 128b + 256b+ 128b + 128b = 1KB
         *
         *  Note: CLS means Cache Line Size
         */
        memReq = 2 *
            ((_Ipc_roundup(sizeof(NotifyDriverCirc_EventEntry) *
                           NotifyDriverCirc_module->numMsgs, minAlign))
            + ( 2 * _Ipc_roundup(sizeof(Bits32), minAlign)));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverCirc_sharedMemReq", memReq);

    /*! @retval Shared-memory-requirements Operation successfully completed. */
    return (memReq);
}


/*!
 *  @brief      Set Notify object handle within the driver.
 *
 *  @param      handle  NotifyDriverCirc handle.
 *  @param      driverHandle  Notify handle.
 *
 *  @sa
 *
 */
Void
NotifyDriverCirc_setNotifyHandle (NotifyDriverCirc_Handle handle,
                                 Ptr                    driverHandle)
{
    NotifyDriverCirc_Object * obj = (NotifyDriverCirc_Object *) handle;

    GT_2trace (curTrace, GT_ENTER, "NotifyDriverCirc_setNotifyHandle",
               handle, driverHandle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverCirc_state.refCount),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(0),
                                  NotifyDriverCirc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_setNotifyHandle",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverCirc_setNotifyHandle",
                              Notify_E_INVALIDARG,
                             "Invalid NULL handle specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->notifyHandle = driverHandle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverCirc_setNotifyHandle");
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*!
 *  @brief      This function implements the interrupt service routine for the
 *              interrupt received from the DSP.
 *
 *  @param      refData       object to be handled in ISR
 *
 *  @sa         _NotifyDriverCirc_checkAndClearFunc
 */
static
Bool
_NotifyDriverCirc_ISR (Void * arg)
{
    NotifyDriverCirc_EventEntry * eventEntry;
    NotifyDriverCirc_Object *     obj;
    UInt32                        writeIndex;
    UInt32                        readIndex;

    GT_1trace (curTrace, GT_ENTER, "_NotifyDriverCirc_ISR", arg);

    obj = (NotifyDriverCirc_Object *) arg;
    GT_assert (curTrace, (obj != NULL));

    /* Interrupt clear is done by ArchIpcInt. */

    /*
     *  Invalidate both getBuffer getWriteIndex from cache.
     *  Do the Cache_wait() below.
     */
    if (obj->cacheEnabled) {
        Cache_inv(obj->getBuffer,
                  obj->opCacheSize,
                  Cache_Type_ALL,
                  FALSE);
    }

    /* wait here to make sure inv is completely done */
    if (obj->cacheEnabled) {
        Cache_wait();
    }

    /* get the writeIndex and readIndex */
    writeIndex = obj->getWriteIndex[0];
    readIndex = obj->getReadIndex[0];

    /* get the event */
    eventEntry = &(obj->getBuffer[readIndex]);

    /* if writeIndex != readIndex then there is an event to process */
    while (writeIndex != readIndex) {
        /*
         *  Check to make sure event is registered. If the event
         *  is not registered, the event is not processed and is lost.
         */
        if (TEST_BIT(obj->evtRegMask, eventEntry->eventid)) {
#if defined  (SYSLINK_INT_LOGGING)
            SysLogging_NotifyCallbackCount++;
#endif /* if defined  (SYSLINK_INT_LOGGING) */
            /* Execute the callback function */
            Notify_exec (obj->notifyHandle,
                         eventEntry->eventid,
                         eventEntry->payload);

        }

        /* update the readIndex. */
        readIndex = ((readIndex + 1) & NotifyDriverCirc_module->maxIndex);

        /* set the getReadIndex */
        obj->getReadIndex[0] = readIndex;

        /*
         *  Write back the getReadIndex once every N / 4 messages.
         *  No need to invalidate since only one processor ever
         *  writes this. No need to wait for operation to complete
         *  since remote core updates its readIndex at least once
         *  every N messages and the remote core will not use a slot
         *  until it sees that the event has been processed with this
         *  cache wb.
         */
        if ((obj->cacheEnabled) &&
            ((readIndex % NotifyDriverCirc_module->modIndex) == 0)) {
            Cache_wb(obj->getReadIndex,
                     sizeof(Bits32),
                     Cache_Type_ALL,
                     FALSE);
        }

        /* get the next event */
        eventEntry = &(obj->getBuffer[readIndex]);
    }

    GT_1trace (curTrace, GT_LEAVE, "_NotifyDriverCirc_ISR", TRUE);

    /*! @retval TRUE ISR has been handled. */
    return (TRUE); /* ISR is always handled. */
}
