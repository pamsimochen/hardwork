/*
 *  @file   NotifyDriverShm.c
 *
 *  @brief      Notify shared memory driver implementation
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
#include <ti/syslink/inc/NotifyDriverShm.h>
#include <ti/syslink/inc/knl/_NotifyDriverShm.h>
#include <ti/ipc/SharedRegion.h>

/* HAL Headers */
#include <ti/syslink/inc/knl/_ArchIpcInt.h>
#include <ti/syslink/inc/knl/ArchIpcInt.h>

#include <ti/syslink/inc/_Notify.h>    /* for auto-setting of USE_SYSLINK_NOTIFY */

#if !defined (USE_SYSLINK_NOTIFY)
#include <syslink/notify_shm_drv.h>
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

/*!
 *  @brief  Get address of event entry.
 */
#define EVENTENTRY(eventChart, align, eventId) \
            ((NotifyDriverShm_EventEntry *) \
             ((UInt32)eventChart + (align * eventId)));

/*!
 *  @brief  Stamp indicating that the Notify Shared Memory driver on the
 *          processor has been initialized.
 */
#define NotifyDriverShm_INIT_STAMP      0xA9C8B7D6

/*!
 *  @brief  Flag indicating event is set.
 */
#define NotifyDriverShm_UP    1

/*!
 *  @brief  Flag indicating event is not set.
 */
#define NotifyDriverShm_DOWN  0

/* Macro to make a correct module magic number with refCount */
#define NotifyDriverShm_MAKE_MAGICSTAMP(x) \
                            ((NOTIFYDRIVERSHM_MODULEID << 12u) | (x))


/*!
 *  @brief   Defines the NotifyDriverShm state object, which contains all the
 *           module specific information.
 */
typedef struct NotifyDriverShm_ModuleObject_tag {
    Atomic                           refCount;
    /* Reference count */
    NotifyDriverShm_Config           cfg;
    /*!< NotifyDriverShm configuration structure */
    NotifyDriverShm_Config           defCfg;
    /*!< Default module configuration */
    NotifyDriverShm_Params           defInstParams;
    /*!< Default instance parameters */
    IGateProvider_Handle             gateHandle;
    /*!< Handle to the gate for local thread safety */
    INotifyDriver_Handle             driverHandles [MultiProc_MAXPROCESSORS]
                                                        [Notify_MAX_INTLINES];
    /*!< Loader handle array. */
} NotifyDriverShm_ModuleObject;

/*!
 *  @brief  NotifyDriverShm instance object.
 */
struct NotifyDriverShm_Object_tag {
    NotifyDriverShm_Params           params;
    /*!< Instance parameters (configuration values) */
    volatile NotifyDriverShm_ProcCtrl *   selfProcCtrl;
    /*!< Pointer to control structure in shared memory for self processor. */
    volatile NotifyDriverShm_ProcCtrl *   otherProcCtrl;
    /*!< Pointer to control structure in shared memory for remote processor. */
    volatile NotifyDriverShm_EventEntry * selfEventChart;
    /*!< Pointer to event chart for local processor */
    volatile NotifyDriverShm_EventEntry * otherEventChart;
    /*!< Pointer to event chart for remote processor */
    UInt32                           regChart [Notify_MAXEVENTS];
    /*!< Local event registration chart for tracking registered events. */
    UInt16                           selfId;
    /*!< Self ID used for identification of local control region */
    UInt16                           otherId;
    /*!< Other ID used for identification of remote control region */
    UInt16                           remoteProcId;
    /*!< Processor ID of the remote processor which which this driver instance
         communicates. */
    UInt16                           lineId;
    /*!< Interrupt line number being worked on by the driver */
    Notify_Handle                    notifyHandle;
    /*!< Common NotifyDriver handle */
    UInt32                           nesting;
    /*!< For disable/restore nesting */
    UInt32                           cacheEnabled;
    /*!< Whether to perform cache calls */
    UInt32                           eventEntrySize;
    /* Spacing between event entries   */
    UInt32                           numEvents;
    /*!< Number of events configured */
};


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    NotifyDriverShm_state
 *
 *  @brief  Notify state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NotifyDriverShm_ModuleObject NotifyDriverShm_state =
{
    .gateHandle = NULL,
    .defInstParams.sharedAddr = 0x0,
    .defInstParams.cacheEnabled = FALSE,
    .defInstParams.cacheLineSize = 128u,
    .defInstParams.remoteProcId = MultiProc_INVALIDID,
    .defInstParams.lineId = 0x0,
    .defInstParams.localIntId = (UInt32) -1,
    .defInstParams.remoteIntId = (UInt32) -1,
};

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
static Bool _NotifyDriverShm_ISR (Void * refData);


/* =============================================================================
 * APIs called directly by applications
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the NotifyDriverShm module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to NotifyDriverShm_setup filled in by
 *              the NotifyDriverShm module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfg        Pointer to the NotifyDriverShm module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         NotifyDriverShm_setup
 */
Void
NotifyDriverShm_getConfig (NotifyDriverShm_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_getConfig",
                             Notify_E_INVALIDARG,
                             "Argument of type (NotifyDriverShm_Config *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if defined (USE_SYSLINK_NOTIFY)
        if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                      NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                      NotifyDriverShm_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfg,
                         &(NotifyDriverShm_state.defCfg),
                         sizeof (NotifyDriverShm_Config));
        }
        else {
            Memory_copy (cfg,
                         &(NotifyDriverShm_state.cfg),
                         sizeof (NotifyDriverShm_Config));
        }
#else
        notify_shm_drv_get_config((Ptr)cfg);
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverShm_getConfig");
}


/*!
 *  @brief      Setup the NotifyDriverShm module.
 *
 *              This function sets up the NotifyDriverShm module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then NotifyDriverShm_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call NotifyDriverShm_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional NotifyDriverShm module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         NotifyDriverShm_destroy
 *              GateSpinlock_create
 */
Int
NotifyDriverShm_setup (NotifyDriverShm_Config * cfg)
{
    Int                    status = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    Error_Block            eb;
    NotifyDriverShm_Config tmpCfg;
    UInt16                 i;
    UInt16                 j;
#endif

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_setup", cfg);
#if defined (USE_SYSLINK_NOTIFY)
    Error_init (&eb);


    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&NotifyDriverShm_state.refCount,
                            NotifyDriverShm_MAKE_MAGICSTAMP(0),
                            NotifyDriverShm_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&NotifyDriverShm_state.refCount)
        != NotifyDriverShm_MAKE_MAGICSTAMP(1u)) {
        status = Notify_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "NotifyDriverShm Module already initialized!");
    }
    else {
        if (cfg == NULL) {
            NotifyDriverShm_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }

        /* Create a default gate handle for local module protection. */
        NotifyDriverShm_state.gateHandle = (IGateProvider_Handle)
                         GateSpinlock_create ((GateSpinlock_Params *) NULL, &eb);

        /* Initialize the driver handles. */
        for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
            for (j = 0 ; j < Notify_MAX_INTLINES ; j++) {
                NotifyDriverShm_state.driverHandles [i][j] = NULL;
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NotifyDriverShm_state.gateHandle == NULL) {
            Atomic_set (&NotifyDriverShm_state.refCount,
                        NotifyDriverShm_MAKE_MAGICSTAMP(0));
            /*! @retval Notify_E_FAIL Failed to create GateSpinlock! */
            status = Notify_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyDriverShm_setup",
                                 status,
                                 "Failed to create GateSpinlock!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&NotifyDriverShm_state.cfg,
                         cfg,
                         sizeof (NotifyDriverShm_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#else
    status = notify_shm_drv_setup((Ptr)cfg);
#endif
    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_setup", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the NotifyDriverShm module.
 *
 *              Once this function is called, other NotifyDriverShm module APIs,
 *              except for the NotifyDriverShm_getConfig API cannot be called
 *              anymore.
 *
 *  @sa         NotifyDriverShm_setup
 *              GateSpinlock_delete
 */
Int
NotifyDriverShm_destroy (Void)
{
    Int    status = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    UInt16 i;
    UInt16 j;
#endif
    GT_0trace (curTrace, GT_ENTER, "NotifyDriverShm_destroy");

#if defined (USE_SYSLINK_NOTIFY)

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval Notify_E_INVALIDSTATE Module was not initialized */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&NotifyDriverShm_state.refCount)
            == NotifyDriverShm_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&NotifyDriverShm_state.refCount,
                        NotifyDriverShm_MAKE_MAGICSTAMP(1));

            /* Check if any NotifyDriverShm instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                for (j = 0 ; j < Notify_MAX_INTLINES ; j++) {
                    if (NotifyDriverShm_state.driverHandles [i][j] != NULL) {
                        NotifyDriverShm_delete (
                               &(NotifyDriverShm_state.driverHandles [i][j]));
                    }
                }
            }

            /* Reset the refCount */
            Atomic_set (&NotifyDriverShm_state.refCount,
                        NotifyDriverShm_MAKE_MAGICSTAMP(0));

            /* Check if the gateHandle was created internally. */
            if (NotifyDriverShm_state.gateHandle != NULL) {
                GateSpinlock_delete ((GateSpinlock_Handle *)
                                     &(NotifyDriverShm_state.gateHandle));
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = notify_shm_drv_destroy();
#endif
    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_destroy", status);

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
 *  @sa         NotifyDriverShm_create
 */
Void
NotifyDriverShm_Params_init (NotifyDriverShm_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_Params_init",
               params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_Params_init",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_Params_init",
                             Notify_E_INVALIDARG,
                             "Argument of type (NotifyDriverShm_Params *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(NotifyDriverShm_state.defInstParams),
                     sizeof (NotifyDriverShm_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverShm_Params_init");
}


/*!
 *  @brief      Function to create an instance of this NotifyDriver.
 *
 *  @param      driverName  Name of the NotifyDriver instance.
 *  @param      params      Configuration parameters.
 *
 *  @sa         NotifyDriverShm_delete, Notify_registerDriver, List_create,
 *              OsalIsr_install, OsalIsr_create
 */
INotifyDriver_Handle
NotifyDriverShm_create (const NotifyDriverShm_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                          status    = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NotifyDriverShm_Object *     obj       = NULL;
    INotifyDriver_Object *       notifyDrvObj = NULL;
    IArg                         key;
    UInt32                       i;
    UInt16                       regionId;
    SizeT                        regionCacheSize;
    SizeT                        minAlign;
    UInt32                       procCtrlSize;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_create", params);

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
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_create",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL Invalid NULL params specified! */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_create",
                              Notify_E_INVALIDARG,
                             "Invalid NULL params specified!");
    }
    else if (   (params->remoteProcId == MultiProc_INVALIDID)
             || (params->remoteProcId == MultiProc_self ())) {
         /*! @retval NULL Invalid remoteProcId specified. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_create",
                             status,
                             "Invalid remoteProcId specified.");
    }
    else if (params->lineId >= Notify_MAX_INTLINES) {
         /*! @retval NULL Invalid lineId specified. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_create",
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
                             "NotifyDriverShm_create",
                             status,
                             "Shared memory base address is not aligned "
                             "to cache line size.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (NotifyDriverShm_state.gateHandle);

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
                                 "NotifyDriverShm_create",
                                 Notify_E_MEMORY,
                                 "Failed to allocate memory for "
                                 "INotifyDriver_Object!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Function table information */
            notifyDrvObj->fxnTable.registerEvent =
                                        (INotifyDriver_RegisterEventFxn)
                                                &NotifyDriverShm_registerEvent;
            notifyDrvObj->fxnTable.unregisterEvent =
                                     (INotifyDriver_UnregisterEventFxn)
                                            &NotifyDriverShm_unregisterEvent;
            notifyDrvObj->fxnTable.sendEvent = (INotifyDriver_SendEventFxn)
                                                    &NotifyDriverShm_sendEvent;
            notifyDrvObj->fxnTable.disable = (INotifyDriver_DisableFxn)
                                                    &NotifyDriverShm_disable;
            notifyDrvObj->fxnTable.enable  = (INotifyDriver_EnableFxn)
                                                        &NotifyDriverShm_enable;
            notifyDrvObj->fxnTable.disableEvent= (INotifyDriver_DisableEventFxn)
                                                &NotifyDriverShm_disableEvent;
            notifyDrvObj->fxnTable.enableEvent = (INotifyDriver_EnableEventFxn)
                                                &NotifyDriverShm_enableEvent;
            notifyDrvObj->fxnTable.setNotifyHandle =
                                        (INotifyDriver_SetNotifyHandleFxn)
                                            &NotifyDriverShm_setNotifyHandle;

            /* Allocate memory for the NotifyDriverShm_Object object. */
            obj = Memory_calloc (NULL,
                                 sizeof (NotifyDriverShm_Object),
                                 0u,
                                 NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj == NULL) {
                /*! @retval NULL Failed to allocate memory for
                                 NotifyDriverShm_Object! */
                status = Notify_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDriverShm_create",
                                     Notify_E_MEMORY,
                                     "Failed to allocate memory for "
                                     "NotifyDriverShm_Object!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                notifyDrvObj->obj = obj;

                /* Copy params into instance object. */
                Memory_copy (&(obj->params),
                             (Ptr) params,
                             sizeof (NotifyDriverShm_Params));
                obj->numEvents = Notify_state.cfg.numEvents;

                /* Set the handle in the driverHandles array. */
                NotifyDriverShm_state.driverHandles
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
            regionId = SharedRegion_getId ((Ptr) params->sharedAddr);
            if (regionId != SharedRegion_INVALIDREGIONID) {
                /*
                 *  Override the user cacheEnabled setting if the region
                 *  cacheEnabled is FALSE.
                 */
                if (!SharedRegion_isCacheEnabled (regionId)) {
                    obj->cacheEnabled = FALSE;
                }

                regionCacheSize = SharedRegion_getCacheLineSize (regionId);

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
            GT_assert (curTrace,
                       ((UInt32) params->sharedAddr % minAlign == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((UInt32) params->sharedAddr % minAlign != 0) {
                /*! @retval NULL params->sharedAddr does not meet cache
                                 alignment constraints! */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDriverShm_create",
                                      status,
                                     "params->sharedAddr does not meet"
                                     " cache alignment constraints!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                obj->remoteProcId = params->remoteProcId;
                obj->lineId = params->lineId;
                obj->nesting      = 0;

                if (params->remoteProcId > MultiProc_self ()) {
                    obj->selfId  = 0;
                    obj->otherId = 1;
                }
                else {
                    obj->selfId  = 1;
                    obj->otherId = 0;
                }

                /* Initialize pointers to shared memory regions */
                procCtrlSize = _Ipc_roundup(sizeof (NotifyDriverShm_ProcCtrl),
                                         minAlign);


                /*
                 *  Save the eventEntrySize in obj since we will need it at
                 *  runtime to index the event charts
                 */

                obj->eventEntrySize = _Ipc_roundup(
                                            sizeof(NotifyDriverShm_EventEntry),
                                                minAlign);

                obj->selfProcCtrl = (NotifyDriverShm_ProcCtrl *)
                                    (   (UInt32) params->sharedAddr
                                     +  (obj->selfId * procCtrlSize));
                obj->otherProcCtrl = (NotifyDriverShm_ProcCtrl *)
                                    (  (UInt32) params->sharedAddr
                                     + (obj->otherId * procCtrlSize));
                obj->selfEventChart  = (NotifyDriverShm_EventEntry *)
                            (  (UInt32) params->sharedAddr
                             + (2 * procCtrlSize)
                         + (obj->eventEntrySize * obj->numEvents * obj->selfId));
                obj->otherEventChart  = (NotifyDriverShm_EventEntry *)
                            (  (UInt32) params->sharedAddr
                             + (2 * procCtrlSize)
                         + (obj->eventEntrySize * obj->numEvents * obj->otherId));

                for (i = 0; i < obj->numEvents; i++) {
                    obj->regChart [i] = ~0;
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }

        if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Setup shared memory */

            /* All events are initially unflagged. Since the shared memory is
             * initialized to zero, it is not required to clear it here.
             * Also, all events are initially not registered.
             */
            /* Enable all events initially.*/
            obj->selfProcCtrl->eventEnableMask = 0xFFFFFFFF;

            /* Write back our own ProcCtrl */
            if (obj->cacheEnabled) {
                Cache_wbInv ((Ptr) obj->selfProcCtrl,
                             sizeof (NotifyDriverShm_ProcCtrl),
                             Cache_Type_ALL,
                             TRUE);
            }

            /* Register the incoming interrupt */
            IGateProvider_leave (NotifyDriverShm_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                     ArchIpcInt_interruptRegister (obj->remoteProcId,
                                                   obj->params.localIntId,
                                                   _NotifyDriverShm_ISR,
                                                   (Ptr) obj);
            IGateProvider_enter (NotifyDriverShm_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                /*! @retval NULL Failed to register interrupt! */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDriverShm_create",
                                     status,
                                     "ArchIpcInt_interruptRegister failed");
            }
            /* Indicate that the driver is initialized for this processor
             * only when the corresponding Notify driver is also created,
             * i.e. in NotifyDriverShm_setNotifyHandle.
             */
        }

        /* Cleanup on error. */
        if (status < 0) {
            /* Check if obj was allocated. */
            if (obj != NULL) {
                ArchIpcInt_interruptUnregister(obj->remoteProcId,
                        obj->params.localIntId, (Ptr)obj);

                if (obj->selfProcCtrl != NULL) {
                    /* Clear initialization status in shared memory. */
                    obj->selfProcCtrl->recvInitStatus = 0x0;
                    obj->selfProcCtrl->sendInitStatus = 0x0;
                    obj->selfProcCtrl = NULL;
                    /* Write back our own ProcCtrl */
                    if (obj->cacheEnabled) {
                        Cache_wbInv ((Ptr) obj->selfProcCtrl,
                                     sizeof (NotifyDriverShm_ProcCtrl),
                                     Cache_Type_ALL,
                                     TRUE);
                    }
                }

                Memory_free (NULL,
                             obj,
                             sizeof (NotifyDriverShm_Object));
                obj = NULL;

                Memory_free (NULL,
                             notifyDrvObj,
                             sizeof (INotifyDriver_Object));
                notifyDrvObj = NULL;
                /* Clear the NotifyDriverShm handle in the local array. */
                GT_assert (curTrace,
                           (params->remoteProcId < MultiProc_MAXPROCESSORS));
                GT_assert (curTrace,
                           (params->lineId < Notify_MAX_INTLINES));
                NotifyDriverShm_state.driverHandles
                                [params->remoteProcId] [params->lineId] = NULL;
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (NotifyDriverShm_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_create", notifyDrvObj);

    /*! @retval Driver-handle Operation successfully completed. */
    return ((INotifyDriver_Handle) notifyDrvObj);
}


/*!
 *  @brief      Function to delete the instance of shared memory driver
 *
 *  @param      handlePtr   Pointer to NotiyDriverShm handle. This is reset
 *                          upon completion of the API.
 *
 *  @sa         NotifyDriverShm_create, Notify_unregisterDriver, List_delete,
 *              OsalIsr_uninstall, OsalIsr_delete
 */
Int
NotifyDriverShm_delete (INotifyDriver_Handle * handlePtr)
{
    Int                           status    = Notify_S_SUCCESS;
    Int                           tmpStatus = Notify_S_SUCCESS;
    NotifyDriverShm_Object *      obj = NULL;
    INotifyDriver_Object *        notifyDrvObj = NULL;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (handlePtr != NULL) && (*handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_delete",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        /*! @retval Notify_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval Notify_E_INVALIDARG Invalid NULL handle specified */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_delete",
                             status,
                             "Invalid NULL handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        notifyDrvObj = (INotifyDriver_Object *) (*handlePtr);
        obj = (NotifyDriverShm_Object *) notifyDrvObj->obj;

        if (obj != NULL) {
            tmpStatus = ArchIpcInt_interruptUnregister(obj->remoteProcId,
                    obj->params.localIntId, (Ptr)obj);
            if ((status >= 0) && (tmpStatus < 0)) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyDriverShm_delete",
                                 status,
                                 "ArchIpcInt_interruptUnregister failed!");
            }

            if (obj->selfProcCtrl != NULL) {
                /* Clear initialization status in shared memory. */
                obj->selfProcCtrl->recvInitStatus = 0x0;
                obj->selfProcCtrl->sendInitStatus = 0x0;

                /* Write back our own ProcCtrl */
                if (obj->cacheEnabled) {
                    Cache_wbInv ((Ptr) obj->selfProcCtrl,
                                 sizeof (NotifyDriverShm_ProcCtrl),
                                 Cache_Type_ALL,
                                 TRUE);
                }
                obj->selfProcCtrl = NULL;
            }
            /* Clear the NotifyDriverShm handle in the local array. */
            GT_assert (curTrace,
                       (obj->params.remoteProcId < MultiProc_MAXPROCESSORS));
            GT_assert (curTrace,
                       (obj->params.lineId < Notify_MAX_INTLINES));
            NotifyDriverShm_state.driverHandles
                    [obj->params.remoteProcId] [obj->params.lineId] = NULL;
            Memory_free (NULL,
                         obj,
                         sizeof (NotifyDriverShm_Object));
            obj = NULL;
        }

        Memory_free (NULL,
                     notifyDrvObj,
                     sizeof (INotifyDriver_Object));
        notifyDrvObj = NULL;

        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if defined  (SYSLINK_INT_LOGGING)
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
#endif /* if defined  (SYSLINK_INT_LOGGING) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_delete", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed. */
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
 *  @sa         NotifyDriverShm_unregisterEvent, List_put
 *
 */
Int
NotifyDriverShm_registerEvent (NotifyDriverShm_Handle handle,
                               UInt32                 eventId)
{
    Int                             status    = Notify_S_SUCCESS;
    NotifyDriverShm_Object *        obj;
volatile NotifyDriverShm_EventEntry * eventEntry;
    Int32                           i; /* Used as -ve so needs to be Int32 */
    Int32                           j; /* Used as -ve so needs to be Int32 */

    GT_2trace (curTrace,
               GT_ENTER,
               "NotifyDriverShm_registerEvent",
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
                             "NotifyDriverShm_registerEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;

        /*
         *  Disable interrupt line to ensure that NotifyDriverShm_isr doesn't
         *  preempt registerEvent and encounter corrupt state
         */
        NotifyDriverShm_disable (obj);

        /* This function is only called for the first register, i.e. when the
         * first callback is being registered.
         */
        /*
         *  Add an entry for the registered event into the Event Registration
         *  Chart, in ascending order of event numbers (and decreasing
         *  priorities).
         */
        GT_assert (curTrace, (obj->regChart != NULL));
        for (i = 0 ; i < obj->numEvents ; i++) {
            /* Find the correct slot in the registration array. */
            if (obj->regChart [i] == (UInt32) -1) {
                for (j = (i - 1); j >= 0; j--) {
                    if (eventId < obj->regChart [j]) {
                        obj->regChart [j + 1] = obj->regChart [j];
                        i = j;
                    }
                    else {
                        /* End the loop, slot found. */
                        j = -1;
                    }
                }

                obj->regChart [i] = eventId;
                GT_1trace(curTrace, GT_2CLASS, "NotifyDriverShm_registerEvent\n"
                        "    Added entry in regChart: [%d]", i);
                break;
            }
        }

        /* Clear any pending unserviced event as there are no listeners
         * for the pending event
         */
        eventEntry = EVENTENTRY(obj->selfEventChart, obj->eventEntrySize, eventId)
        eventEntry->flag = NotifyDriverShm_DOWN;

        /* Set the registered bit in shared memory and write back */
        SET_BIT (obj->selfProcCtrl->eventRegMask, eventId);

        /* Write back both the flag and the reg mask */
        if (obj->cacheEnabled) {
            /* Writeback event entry */
            Cache_wbInv ((Ptr) eventEntry,
                         sizeof(NotifyDriverShm_EventEntry),
                         Cache_Type_ALL,
                         TRUE);

            /* Writeback eventRegMask */
            Cache_wbInv ((Ptr) obj->selfProcCtrl,
                         sizeof(NotifyDriverShm_ProcCtrl),
                         Cache_Type_ALL,
                         TRUE);
        }

        /* Restore the interrupt line */
        NotifyDriverShm_enable (obj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_registerEvent", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed. */
    return (status);
}


/*!
 *  @brief      Unregister a callback for an event with the Notify driver.
 *
 *  @param      handle       handle to notify driver
 *  @param      eventId      event to be unregistered
 *
 *  @sa         NotifyDriverShm_registerEvent, List_remove
 */
Int
NotifyDriverShm_unregisterEvent (NotifyDriverShm_Handle handle,
                                 UInt32              eventId)
{
    Int                        status    = Notify_S_SUCCESS;
    NotifyDriverShm_EventEntry * eventEntry;
    NotifyDriverShm_Object *   obj;
    UInt32                     i;
    UInt32                     j;

    GT_2trace (curTrace,
               GT_ENTER,
               "NotifyDriverShm_unregisterEvent",
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
                             "NotifyDriverShm_unregisterEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;

        /*
         *  Disable interrupt line to ensure that NotifyDriverShm_isr doesn't
         *  preempt registerEvent and encounter corrupt state
         */
        NotifyDriverShm_disable (obj);

        /* This function is only called for the last unregister, i.e. when the
         * final remaining callback is being unregistered.
         */
        /*
         *  First unset the registered bit in shared memory so no notifications
         *  arrive after this point
         */
        CLEAR_BIT (obj->selfProcCtrl->eventRegMask, eventId);

        /*
         *  Clear any pending unserviced event as there are no listeners
         *  for the pending event.  This should be done only after the event
         *  is unregistered from shared memory so the other processor doesn't
         *  successfully send an event our way immediately after unflagging this
         *  event.
         */
        eventEntry = EVENTENTRY(obj->selfEventChart, obj->eventEntrySize, eventId);
        eventEntry->flag = NotifyDriverShm_DOWN;

        /* Write back both the flag and the reg mask */
        if (obj->cacheEnabled) {
            /* Writeback eventRegMask */
            Cache_wbInv ((Ptr) obj->selfProcCtrl,
                         sizeof (NotifyDriverShm_ProcCtrl),
                         Cache_Type_ALL,
                         TRUE);
            /* Writeback event entry */
            Cache_wbInv (eventEntry,
                         sizeof(NotifyDriverShm_EventEntry),
                         Cache_Type_ALL,
                         TRUE);

        }

        /*
         *  Re-arrange eventIds in the Event Registration Chart so there is
         *  no gap caused by the removal of this eventId
         *
         *  There is no need to make this atomic since Notify_exec cannot
         *  preempt: the event has already been disabled in shared memory
         *  (see above)
         */
        for (i = 0; i < obj->numEvents; i++) {
            /* Find the correct slot in the registration array. */
            if (eventId == obj->regChart [i]) {
                obj->regChart[i] = (UInt32)-1;
                for (j = (i + 1);
                     (j != obj->numEvents)
                     && (obj->regChart [j] != (UInt32) -1) ;
                     j++) {
                    obj->regChart [j - 1] = obj->regChart [j];
                    obj->regChart [j] = (UInt32) -1;
                }
                break;
            }
        }

        /* Restore the interrupt line */
        NotifyDriverShm_enable (obj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_unregisterEvent", status);

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
NotifyDriverShm_sendEvent (NotifyDriverShm_Handle handle,
                           UInt32              eventId,
                           UInt32              payload,
                           Bool                waitClear)
{
     Int                           status  = Notify_S_SUCCESS;
     UInt32                        i       = 0;
     NotifyDriverShm_Object *      obj;
volatile NotifyDriverShm_EventEntry *  eventEntry;
     UInt32                        maxPollCount;
     IArg                          sysKey;

    GT_4trace (curTrace,
               GT_ENTER,
               "NotifyDriverShm_sendEvent",
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
                             "NotifyDriverShm_sendEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;
        GT_assert (curTrace, (eventId < obj->numEvents));

        eventEntry = EVENTENTRY (obj->otherEventChart, obj->eventEntrySize, eventId);

        /* Invalidate cache for the other processor's procCtrl. */
        if (obj->cacheEnabled) {
            Cache_inv ((Ptr) obj->otherProcCtrl,
                       sizeof (NotifyDriverShm_ProcCtrl),
                       Cache_Type_ALL,
                       TRUE);
        }

        maxPollCount = Notify_state.cfg.sendEventPollCount;

        /* Check whether driver on other processor is initialized */
        if (obj->otherProcCtrl->recvInitStatus != NotifyDriverShm_INIT_STAMP) {
            /* This may be used for polling till other-side driver is ready, so
             * do not set failure reason.
             */
            status = Notify_E_NOTINITIALIZED;
        }
        /* Check if other side has registered to receive this event. */
        else if (!TEST_BIT (obj->otherProcCtrl->eventRegMask, eventId)) {
            status = Notify_E_EVTNOTREGISTERED;
            /* This may be used for polling till other-side is ready, so
             * do not set failure reason.
             */
        }
        else if (!TEST_BIT(obj->otherProcCtrl->eventEnableMask, eventId)) {
            status = Notify_E_EVTDISABLED;
            /* This may be used for polling till other-side is ready, so
             * do not set failure reason.
             */
        }
        else {
            if (waitClear == TRUE) {
                if (obj->cacheEnabled) {
                    Cache_inv ((Ptr) eventEntry,
                               sizeof(NotifyDriverShm_EventEntry),
                               Cache_Type_ALL,
                               TRUE);
                }
                /*
                 *  The system gate is needed to ensure that the check of
                 *  eventEntry->flag  is atomic with the eventEntry modifications
                 *  (flag/payload).
                 */
                sysKey = Gate_enterSystem ();

                /* Wait for completion of previous event from other side. */
                while (eventEntry->flag != NotifyDriverShm_DOWN) {
                    /* Leave critical section protection. Create a window
                     * of opportunity for other interrupts to be handled.
                     */
                    Gate_leaveSystem (sysKey);
                    i++;
                    if (    (maxPollCount != (UInt32) -1)
                        &&  (i == maxPollCount)) {
                        /*! @retval Notify_E_TIMEOUT Timed out
                                waiting for completion of previous event. */
                        status = Notify_E_TIMEOUT;
                        /* This is a run-time error and should not be
                         * compiled out with SYSLINK_BUILD_OPTIMIZE.
                         */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        GT_setFailureReason (curTrace,
                                           GT_4CLASS,
                                           "NotifyDriverShm_sendEvent",
                                           status,
                                           "Timed out waiting for "
                                           "completion of previous event.");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        break;
                    }

                    if (obj->cacheEnabled) {
                        Cache_inv ((Ptr) eventEntry,
                                   sizeof(NotifyDriverShm_EventEntry),
                                   Cache_Type_ALL,
                                   TRUE);
                    }

                    /* Re-enter the system gate */
                    sysKey = Gate_enterSystem();
                }
            }
            else {
                /*
                 *  The system gate is needed to ensure that checking eventEntry->flag
                 *  is atomic with the eventEntry modifications (flag/payload).
                 */
                sysKey = Gate_enterSystem();
            }
            if (status >= 0) {
                /* Set the event bit field and payload. */
                eventEntry->payload = payload;
                eventEntry->flag    = NotifyDriverShm_UP;

                if (obj->cacheEnabled) {
                    Cache_wbInv ((Ptr) eventEntry,
                                 sizeof (NotifyDriverShm_EventEntry),
                                 Cache_Type_ALL,
                                 TRUE);
                }

                /* Send an interrupt with the event information to the
                   remote processor */
                ArchIpcInt_sendInterrupt (obj->remoteProcId,
                                          obj->params.remoteIntId,
                                          eventId);
                /* OK to leave system gate before sending the interrupt */
                Gate_leaveSystem(sysKey);


            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_sendEvent", status);

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
 *  @sa         NotifyDriverShm_restore
 */
Int
NotifyDriverShm_disable (NotifyDriverShm_Handle handle)
{
    NotifyDriverShm_Object * obj;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_disable", handle);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_disable",
                             Notify_E_DRIVERNOTREGISTERED,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;

        /* Disable the receive interrupt. */
        ArchIpcInt_interruptDisable (obj->remoteProcId,
                                     obj->params.localIntId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_disable", 0x0);

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
 *  @sa         NotifyDriverShm_disable
 */
Void
NotifyDriverShm_enable (NotifyDriverShm_Handle handle)
{
    NotifyDriverShm_Object * obj;

    GT_1trace (curTrace, GT_ENTER, "NotifyDriverShm_enable", handle);

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checking, since it is done by Notify module. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* handle is specific to NotifyShmDriver, so check its validity. */
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_enable",
                             Notify_E_DRIVERNOTREGISTERED,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;

        /* Enable the receive interrupt. */
        ArchIpcInt_interruptEnable (obj->remoteProcId,
                                    obj->params.localIntId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverShm_enable");
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
 *  @sa         NotifyDriverShm_enableEvent
 */
Void
NotifyDriverShm_disableEvent (NotifyDriverShm_Handle handle,
                              UInt32              eventId)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                      status = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    NotifyDriverShm_EventEntry      *eventEntry;
    NotifyDriverShm_Object * obj;
    IArg                     key;

    GT_2trace (curTrace,
               GT_ENTER,
               "NotifyDriverShm_disableEvent",
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
                             "NotifyDriverShm_disableEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;
        GT_assert (curTrace, (eventId < obj->numEvents));

        /* Enter critical section protection. */
        key = IGateProvider_enter (NotifyDriverShm_state.gateHandle);
        /* Unset the corresponding bit in the processor's eventEnableMask */
        CLEAR_BIT (obj->selfProcCtrl->eventEnableMask, eventId);
        /* Leave critical section protection. */
        IGateProvider_leave (NotifyDriverShm_state.gateHandle, key);
        if (obj->cacheEnabled) {
            Cache_wbInv ((Ptr) obj->selfProcCtrl,
                         sizeof (NotifyDriverShm_ProcCtrl),
                         Cache_Type_ALL,
                         TRUE);
        }


        eventEntry = EVENTENTRY(obj->selfEventChart, obj->eventEntrySize,
                eventId);
        if (obj->cacheEnabled) {
            Cache_inv(eventEntry,
                      sizeof(NotifyDriverShm_EventEntry),
                      Cache_Type_ALL, TRUE);
        }

        /*
         *  Disable incoming Notify interrupts.  This is done to ensure that the
         *  eventEntry->flag is read atomically with any write back to shared
         *  memory
         */
        NotifyDriverShm_disable(handle);

        /*
         *  Is the local NotifyDriverShm_disableEvent happening between the
         *  following two NotifyDriverShm_sendEvent operations on the remote
         *  processor?
         *  1. Writing NotifyDriverShm_UP to shared memory
         *  2. Sending the interrupt across
         *  If so, we should handle this event so the other core isn't left
         *  spinning until the event is re-enabled and the next
         *  NotifyDriverShm_isr executes This race condition is very rare but we
         *  need to account for it:
         */
        if (eventEntry->flag == NotifyDriverShm_UP) {
            /*
             *  Acknowledge the event. No need to store the payload. The other
             *  side will not send this event again even though flag is down,
             *  because the event is now disabled. So the payload within the
             *  eventChart will not get overwritten.
             */
            eventEntry->flag = NotifyDriverShm_DOWN;

            /* Write back acknowledgement */
            if (obj->cacheEnabled) {
                Cache_wbInv(eventEntry, sizeof(NotifyDriverShm_EventEntry),
                    Cache_Type_ALL, TRUE);
            }

            /*
             *  Execute the callback function. This will execute in a Task
             *  or Swi context (not Hwi!)
             */
            Notify_exec (obj->notifyHandle,
                         eventId,
                         eventEntry->payload);
        }

        /* Re-enable incoming Notify interrupts */
        NotifyDriverShm_enable(handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverShm_disableEvent");
}


/*!
 *  @brief      Enable a specific event.
 *              All callbacks registered for this specific event are enabled
 *              with this API. It is not possible to enable a specific callback.
 *
 *  @param      handle    Handle to the Notify Driver
 *  @param      eventId   Event number to be enabled
 *
 *  @sa         NotifyDriverShm_disableEvent
 */
Void
NotifyDriverShm_enableEvent (NotifyDriverShm_Handle handle,
                             UInt32              eventId)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                      status = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NotifyDriverShm_Object * obj;
    IArg                     key;

    GT_2trace (curTrace,
               GT_ENTER,
               "NotifyDriverShm_enableEvent",
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
                             "NotifyDriverShm_enableEvent",
                             status,
                             "Notify driver object is NULL.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NotifyDriverShm_Object *) handle;
        GT_assert (curTrace, (eventId < obj->numEvents));

        /* Enter critical section protection. */
        key = IGateProvider_enter (NotifyDriverShm_state.gateHandle);
        /* Set the corresponding bit in the processor's enableMask */
        SET_BIT(obj->selfProcCtrl->eventEnableMask, eventId);
        /* Leave critical section protection. */
        IGateProvider_leave (NotifyDriverShm_state.gateHandle, key);
        if (obj->cacheEnabled) {
            Cache_wbInv ((Ptr) obj->selfProcCtrl,
                         sizeof (NotifyDriverShm_ProcCtrl),
                         Cache_Type_ALL,
                         TRUE);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverShm_enableEvent");
}


/*!
 *  @brief      Get the shared memory requirements for the NotifyDriverShm.
 *
 *  @param      params  Configuration parameters.
 *
 *  @sa         NotifyDriverShm_create
 *
 */
SizeT
NotifyDriverShm_sharedMemReq (const NotifyDriverShm_Params * params)
{
    SizeT  memReq = 0;
    UInt16 regionId;
    SizeT  regionCacheSize;
    SizeT  minAlign;

    /* Ensure that params is non-NULL */
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_sharedMemReq",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL Invalid NULL params specified! */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_sharedMemReq",
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

    /* Determine obj->eventEntrySize which will be used to _Ipc_roundup addresses */
    memReq = ((_Ipc_roundup(sizeof(NotifyDriverShm_ProcCtrl), minAlign)) * 2)
           + ((_Ipc_roundup(sizeof(NotifyDriverShm_EventEntry), minAlign) * 2
              * Notify_state.cfg.numEvents));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDriverShm_sharedMemReq", memReq);

    /*! @retval Shared-memory-requirements Operation successfully completed. */
    return (memReq);
}


/*!
 *  @brief      Set Notify object handle within the driver.
 *
 *  @param      handle  NotifyDriverShm handle.
 *  @param      driverHandle  Notify handle.
 *
 *  @sa
 *
 */
Void
NotifyDriverShm_setNotifyHandle (NotifyDriverShm_Handle handle,
                                 Ptr                    driverHandle)
{
    NotifyDriverShm_Object * obj = (NotifyDriverShm_Object *) handle;

    GT_2trace (curTrace, GT_ENTER, "NotifyDriverShm_setNotifyHandle",
               handle, driverHandle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NotifyDriverShm_state.refCount),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(0),
                                  NotifyDriverShm_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_setNotifyHandle",
                             Notify_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDriverShm_setNotifyHandle",
                              Notify_E_INVALIDARG,
                             "Invalid NULL handle specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->notifyHandle = driverHandle;

            /* If the driver handle is being set to a valid value, this
             * indicates that creation of the Notify driver is fully done. Hence
             * complete the final indication that the driver is initialized.
             */
            if (driverHandle != NULL) {
                /* Indicate that the driver is initialized for this processor */
                obj->selfProcCtrl->recvInitStatus = NotifyDriverShm_INIT_STAMP;
                obj->selfProcCtrl->sendInitStatus = NotifyDriverShm_INIT_STAMP;

                /* Write back our own ProcCtrl */
                if (obj->cacheEnabled) {
                    Cache_wbInv ((Ptr) obj->selfProcCtrl,
                                 sizeof (NotifyDriverShm_ProcCtrl),
                                 Cache_Type_ALL,
                                 TRUE);
                }
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NotifyDriverShm_setNotifyHandle");
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
 *  @sa         _NotifyDriverShm_checkAndClearFunc
 */
static
Bool
_NotifyDriverShm_ISR (Void * arg)
{
    UInt32                          payload = 0;
    UInt32                          i       = 0;
volatile NotifyDriverShm_EventEntry * eventEntry;
    NotifyDriverShm_Object *        obj;
    UInt32                          eventId;

    GT_1trace (curTrace, GT_ENTER, "_NotifyDriverShm_ISR", arg);

    obj = (NotifyDriverShm_Object *) arg;
    GT_assert (curTrace, (obj != NULL));

    /* Interrupt clear is done by ArchIpcInt. */

    /* Execute the loop till no asserted event is found for one complete loop
     * through all registered events.
     */
    do {
        /* Check if the entry is a valid registered event. */
        eventId = obj->regChart [i];
        if (eventId != (UInt32) -1) {
            /* Determine the current high priority event. */
            eventEntry = EVENTENTRY (obj->selfEventChart, obj->eventEntrySize, (UInt32)eventId);
            if (obj->cacheEnabled) {
                Cache_inv ((Ptr) eventEntry,
                           sizeof (NotifyDriverShm_EventEntry),
                           Cache_Type_ALL,
                           TRUE);
            }

            /* Check if the event is set and enabled. */
            if (   (eventEntry->flag == NotifyDriverShm_UP)
                && TEST_BIT (obj->selfProcCtrl->eventEnableMask,
                             (UInt32) eventId)) {
                payload = eventEntry->payload;

                /* Acknowledge the event. */
                eventEntry->flag = NotifyDriverShm_DOWN;

                /* Write back acknowledgement */
                if (obj->cacheEnabled) {
                    Cache_wbInv ((Ptr) eventEntry,
                                 sizeof (NotifyDriverShm_EventEntry),
                                 Cache_Type_ALL,
                                 TRUE);
                }

#if defined  (SYSLINK_INT_LOGGING)
                SysLogging_NotifyCallbackCount++;
#endif /* if defined  (SYSLINK_INT_LOGGING) */
                /* Execute the callback function */
                Notify_exec (obj->notifyHandle,
                             eventId,
                             payload);

                /* Reinitialize the event check counter. */
                i = 0;
            }
            else {
                /* Check for next event. */
                i++;
            }
        }
    }
    while ((eventId != (UInt32) -1) && (i < obj->numEvents));

    GT_1trace (curTrace, GT_LEAVE, "_NotifyDriverShm_ISR", TRUE);

    /*! @retval TRUE ISR has been handled. */
    return (TRUE); /* ISR is always handled. */
}
