/*
 *  @file   Notify.c
 *
 *  @brief      Implementation of Notify module.
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

/* Osal & utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/List.h>
#include <ti/ipc/MultiProc.h>

/* Module headers */
#include <ti/ipc/Notify.h>
#include <ti/syslink/inc/knl/_NotifyDefs.h>
#include <ti/syslink/inc/INotifyDriver.h>

#include <ti/syslink/inc/_Notify.h>    /* for auto-setting of USE_SYSLINK_NOTIFY */

#if defined (USE_SYSLINK_NOTIFY)
#include <ti/syslink/inc/knl/NotifySetupProxy.h>
#else
#include <syslink/notify.h>
#include <syslink/_notify.h>
#endif


/* =============================================================================
 *  Macros and types
 *  For additional, see _NotifyDefs.h
 * =============================================================================
 */
/*!
 *  @brief   Defines the Event listener information instance
 */
typedef struct Notify_EventListener_tag {
    List_Elem            element;
    /*!< List element for linking with List module */
    Notify_EventCallback callback;
    /*!< Event callback information */
} Notify_EventListener;


/* =============================================================================
 *  Forward declarations of internal functions.
 * =============================================================================
 */
/* internal function that register a single callback for an event */
static inline Int
_Notify_registerEventSingle (UInt16              procId,
                             UInt16              lineId,
                             UInt32              eventId,
                             Notify_FnNotifyCbck fnNotifyCbck,
                             UArg                cbckArg,
                             bool                acqGate);

/* internal inline function  to Remove an event listener from an event */
static inline  Int32
_Notify_unregisterEventSingle (UInt16              procId,
                               UInt16              lineId,
                               UInt32              eventId,
                               bool                acqGate);

/* Function registered with Notify_exec when multiple registrations are present
 * for the events.
 */
Void
Notify_execMany (UInt16 procId,
                 UInt16 lineId,
                 UInt32 eventId,
                 UArg   arg,
                 UInt32 payload);


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    Notify_state
 *
 *  @brief  Notify state object variable
 */
Notify_ModuleObject Notify_state =
{
    .defCfg.numEvents = 32u,
    .defCfg.sendEventPollCount = -1u,
    .defCfg.numLines = 1u,
    .defCfg.reservedEvents = 5u,
    .gateHandle = NULL,
    .localNotifyHandle = NULL
};

/*!
 *  @var    Notify_module
 *
 *  @brief  Pointer to the Notify module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
Notify_ModuleObject * Notify_module = &Notify_state;

/* =============================================================================
 *  APIs called by applications.
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the Notify module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to Notify_setup filled in by the
 *              Notify module with the default parameters. If the user
 *              does not wish to make any change in the default parameters, this
 *              API is not required to be called.
 *
 *  @param      cfg        Pointer to the Notify module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         Notify_setup
 */
Void
Notify_getConfig (Notify_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "Notify_getConfig", cfg);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_getConfig",
                             Notify_E_INVALIDARG,
                             "Argument of type (Notify_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                      Notify_MAKE_MAGICSTAMP(0),
                                      Notify_MAKE_MAGICSTAMP(1))
            == TRUE) {
            /* (If setup has not yet been called) */
            Memory_copy (cfg,
                         &Notify_module->defCfg,
                         sizeof (Notify_Config));
        }
        else {
            Memory_copy (cfg,
                         &Notify_module->cfg,
                         sizeof (Notify_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    /* Need to ensure that Notify_Config and notify_config structures are same*/
    notify_get_config((Ptr)cfg);
#endif
    GT_0trace (curTrace, GT_LEAVE, "Notify_getConfig");
}


/*!
 *  @brief      Setup the Notify module.
 *
 *              This function sets up the Notify module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then Notify_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call Notify_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional Notify module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         Notify_destroy
 *              GateSpinlock_create
 */
Int
Notify_setup (Notify_Config * cfg)
{
    Int           status = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    UInt16        procId = MultiProc_self();
    Error_Block   eb;
    Notify_Config tmpCfg;
#endif

    GT_1trace (curTrace, GT_ENTER, "Notify_setup", cfg);

#if defined (USE_SYSLINK_NOTIFY)
    Error_init (&eb);

    if (cfg == NULL) {
        Notify_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (cfg == NULL) {
        Notify_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&Notify_module->refCount,
                            Notify_MAKE_MAGICSTAMP(0),
                            Notify_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&Notify_module->refCount)
        != Notify_MAKE_MAGICSTAMP(1u)) {
        status = Notify_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "Notify Module already initialized!");
    }
    else {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if(procId == MultiProc_INVALIDID) {
            status = Notify_E_FAIL;
            GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "Notify_setup",
                            status,
                            "Local Proc Id is not set to valid Id!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Create a default gate handle for local module protection. */
            Notify_module->gateHandle = (IGateProvider_Handle)
                       GateSpinlock_create ((GateSpinlock_Params *) NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (Notify_module->gateHandle == NULL) {
                /*! @retval Notify_E_FAIL Failed to create GateSpinlock! */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_setup",
                                     status,
                                     "Failed to create GateSpinlock!");
            }
            else if (cfg->numEvents > Notify_MAXEVENTS) {
                /*! @retval Notify_E_INVALIDARG Configured numEvents is greater than
                                   maximum supported by module: Notify_MAXEVENTS */
                status = Notify_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_setup",
                                     status,
                                     "Configured numEvents is greater than maximum"
                                     " supported by module: Notify_MAXEVENTS");
            }
            else if (cfg->numLines > Notify_MAX_INTLINES) {
                /*! @retval Notify_E_INVALIDARG Configured numLines is greater than
                                 maximum supported by module: Notify_MAX_INTLINES */
                status = Notify_E_INVALIDARG ;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_setup",
                                     status,
                                     "Configured numLines is greater than maximum"
                                     " supported by module: Notify_MAX_INTLINES");
            }
            else if (cfg->reservedEvents > cfg->numEvents) {
                /*! @retval Notify_E_INVALIDARG Configured reservedEvents is greater
                                            than configured numEvents */
                status = Notify_E_INVALIDARG ;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_setup",
                                     status,
                                     "Configured numLines is greater than maximum"
                                     " supported by module: Notify_MAX_INTLINES");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Copy the user provided values into the state object. */
                Memory_copy (&Notify_module->cfg,
                             cfg,
                             sizeof (Notify_Config));
                Notify_module->localEnableMask = -1u;
                Notify_module->startComplete = FALSE;

                /* Initialize the driver mapping array. */
                Memory_set (&Notify_module->notifyHandles,
                            0,
                            (   sizeof (Notify_Handle)
                             *  MultiProc_MAXPROCESSORS
                             *  Notify_MAX_INTLINES));

                /* tbd: Should return Notify_Handle */
                Notify_module->localNotifyHandle = Notify_create (NULL,
                                                                  procId,
                                                                  0,
                                                                  NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (Notify_module->localNotifyHandle == NULL) {
                    /*! @retval Notify_E_FAIL Failed to create local Notify driver*/
                    status = Notify_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Notify_setup",
                                         status,
                                         "Failed to create local Notify driver");

                    if (Notify_module->gateHandle != NULL) {
                        GateSpinlock_delete ((GateSpinlock_Handle *)
                                         &(Notify_module->gateHandle));
                    }
                    Atomic_set (&Notify_module->refCount,
                                Notify_MAKE_MAGICSTAMP(0));
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#else
    status = notify_setup((Ptr)cfg);
#endif

    GT_1trace (curTrace, GT_LEAVE, "Notify_setup", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the Notify module.
 *
 *              Once this function is called, other Notify module APIs,
 *              except for the Notify_getConfig API cannot be called
 *              anymore.
 *
 *  @sa         Notify_setup
 *              GateSpinlock_delete
 */
Int32
Notify_destroy (Void)
{
    Int    status = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    UInt16 i;
    UInt16 j;
#endif

    GT_0trace (curTrace, GT_ENTER, "Notify_destroy");

#if defined (USE_SYSLINK_NOTIFY)
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval Notify_E_INVALIDSTATE Module was not initialized */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&Notify_module->refCount)
            == Notify_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&Notify_module->refCount,
                        Notify_MAKE_MAGICSTAMP(1));
            if (Notify_module->localNotifyHandle != NULL) {
                status = Notify_delete (&(Notify_module->localNotifyHandle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_destroy",
                                     status,
                                     "Failed to delete local Notify driver");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* Decrease the refCount */
            Atomic_set (&Notify_module->refCount,
                        Notify_MAKE_MAGICSTAMP(0));

            /* Check if any Notify driver instances have not been deleted so far.
             * If not, assert.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                for (j = 0 ; j < Notify_MAX_INTLINES ; j++) {
                    GT_assert (curTrace,
                               (Notify_module->notifyHandles [i][j] == NULL));
                }
            }

            if (Notify_module->gateHandle != NULL) {
                GateSpinlock_delete ((GateSpinlock_Handle *)
                                     &(Notify_module->gateHandle));
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = notify_destroy();
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_destroy", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/* Function to create an instance of Notify driver */
Notify_Handle
Notify_create (      INotifyDriver_Handle driverHandle,
                     UInt16               remoteProcId,
                     UInt16               lineId,
               const Notify_Params *      params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int             status = Notify_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Notify_Object * obj    = NULL;
    UInt i;
    List_Params           listparams;

    GT_4trace (curTrace, GT_ENTER, "Notify_create",
               driverHandle, remoteProcId, lineId, params);

    GT_assert (curTrace, (remoteProcId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    /* driverHandle can be NULL for local create */
    /* params can be NULL */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_create",
                             status,
                             "Notify module not setup");
    }
    else if (remoteProcId >= MultiProc_getNumProcessors ()) {
        /*! @retval  Notify_E_INVALIDARG Invalid remoteProcId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_create",
                             status,
                             "Invalid remoteProcId argument provided.");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*! @retval  Notify_E_INVALIDARG Invalid lineId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_create",
                             status,
                             "Invalid lineId argument provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate memory for the Notify object. */
        obj = Memory_calloc (NULL, sizeof (Notify_Object), 0u, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            /*! @retval Notify_E_MEMORY Failed to allocate memory for
                                        Notify_Object! */
            status = Notify_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_create",
                                 status,
                                 "Failed to allocate memory for "
                                 "Notify_Object!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->remoteProcId = remoteProcId;
            obj->lineId = lineId;
            obj->nesting = 0;

            for (i = 0; i < Notify_module->cfg.numEvents; i++) {
                List_Params_init (&listparams);
                List_construct (&(obj->eventList [i]), &listparams);
            }

            /* Used solely for remote driver
             * (NULL if remoteProcId == self)
             */
            obj->driverHandle = driverHandle;

            if (driverHandle != NULL) {
                /* Send this handle to the INotifyDriver */
                INotifyDriver_setNotifyHandle (driverHandle, obj);
            }

            Notify_module->notifyHandles [remoteProcId][lineId] = obj;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }

        if (status < 0) {
            if (obj != NULL) {
                if (driverHandle != NULL) {
                    INotifyDriver_setNotifyHandle (driverHandle, NULL);
                }
                for (i = 0; i < Notify_module->cfg.numEvents; i++) {
                    List_destruct (&(obj->eventList [i]));
                }

                Memory_free (NULL, obj, sizeof (Notify_Object));
                obj = NULL;
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Notify_create", obj);

    return ((Notify_Handle) obj);
}


/* Function to delete an instance of Notify driver */
Int
Notify_delete (Notify_Handle * handlePtr)
{
    Int status = Notify_S_SUCCESS;
    Int tmpStatus = Notify_S_SUCCESS;
    List_Elem *     listener = NULL;
    Notify_Object * obj;
    UInt16          i;

    GT_1trace (curTrace, GT_ENTER, "Notify_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_delete",
                             status,
                             "Notify module not setup");
    }
    else if (handlePtr == NULL) {
        /*! @retval Notify_E_INVALIDARG The parameter handlePtr pointer
         *                             to handle passed is NULL
         */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_delete",
                             status,
                             "The parameter handlePtr i.e. pointer to handle "
                             "passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        /*! @retval Notify_E_INVALIDARG Handle passed is NULL */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_delete",
                             status,
                             "Handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (Notify_Object *) (*handlePtr);

        for (i = 0; i < Notify_module->cfg.numEvents; i++) {
            /* If event is not unregistered, and it is not reserved, unregister
             * remaining callbacks.
             */
            if (   !Notify_eventAvailable (obj->remoteProcId, obj->lineId, i)
                && ISRESERVED(i, Notify_module->cfg.reservedEvents)) {
                while (!List_empty (&(obj->eventList [i]))) {
                    listener = List_get (&(obj->eventList [i]));
                    /* Free the memory alloc'ed for the event listener */
                    Memory_free (NULL, listener, sizeof (Notify_EventListener));
                }

                tmpStatus = Notify_unregisterEventSingle (obj->remoteProcId,
                                                          obj->lineId,
                                                          i);
            }
            List_destruct (&(obj->eventList [i]));
        }

        if (obj->driverHandle != NULL) {
            INotifyDriver_setNotifyHandle (obj->driverHandle, NULL);
        }
        Notify_module->notifyHandles [obj->remoteProcId][obj->lineId] = NULL;

        Memory_free (NULL, obj, sizeof (Notify_Object));
        obj = NULL;
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Notify_delete", status);

    return status;
}

/*
 *  ======== Notify_numIntLines ========
 */
UInt16 Notify_numIntLines(UInt16 procId)
{
#if defined (USE_SYSLINK_NOTIFY)
    UInt16 numIntLines;

    if (MultiProc_self() == procId) {
        /* There is always a single interrupt line to the loopback instance */
        numIntLines = 1;
    }
    else {
        /* Query the NotifySetup module for the device */
        numIntLines = Notify_SetupProxy_numIntLines(procId);
    }
    return numIntLines;
#else
    return (notify_numintlines(procId));
#endif

}


/* Register a callback for a specific event with the Notify module. */
Int
Notify_registerEvent (UInt16              procId,
                      UInt16              lineId,
                      UInt32              eventId,
                      Notify_FnNotifyCbck fnNotifyCbck,
                      UArg                cbckArg)
{
    Int32                  status      = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    UInt32                 strippedEventId = (eventId & Notify_EVENT_MASK);
    INotifyDriver_Handle    driverHandle;
    List_Handle            eventList;
    Notify_EventListener * listener;
    Bool                   listWasEmpty;
    Notify_Object * obj;
    IArg    key;
#endif
    GT_5trace (curTrace, GT_ENTER, "Notify_registerEvent",
               procId, lineId, eventId, fnNotifyCbck, cbckArg);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (fnNotifyCbck != NULL));
    /* cbckArg is optional and may be NULL. */
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));

    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));



#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_registerEvent",
                             status,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*! @retval  Notify_E_INVALIDARG Invalid procId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_registerEvent",
                             status,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*! @retval  Notify_E_INVALIDARG Invalid lineId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_registerEvent",
                             status,
                             "Invalid lineId argument provided");
    }
    else if (fnNotifyCbck == NULL) {
        /*! @retval  Notify_E_INVALIDARG Invalid NULL fnNotifyCbck argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_registerEvent",
                             status,
                             "Invalid NULL fnNotifyCbck provided.");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*! @retval  Notify_E_EVTNOTREGISTERED Invalid eventId specified. */
        status = Notify_E_EVTNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_registerEvent",
                             status,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId , Notify_module->cfg.reservedEvents )) {
        /*! @retval  Notify_E_EVTRESERVED Invalid usage of reserved event
                                            number. */
        status = Notify_E_EVTRESERVED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_registerEvent",
                             status,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_registerEvent",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            /* Allocate a new EventListener */
            listener = Memory_alloc (NULL,
                                     sizeof (Notify_EventListener),
                                     0,
                                     NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (listener == NULL) {
                /*! @retval Notify_E_MEMORY Failed to allocate memory for
                                            Notify_EventListener! */
                status = Notify_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_registerEvent",
                                     status,
                                     "Failed to allocate memory for "
                                     "Notify_EventListener!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                listener->callback.fnNotifyCbck = fnNotifyCbck;
                listener->callback.cbckArg = cbckArg;

                eventList = &(obj->eventList [strippedEventId]);

                /* Store whether the list was empty so we know whether to
                 * register the callback
                 */
                listWasEmpty = List_empty (eventList);

                /*
                 *  Need to atomically add to the list using the system gate
                 *  because Notify_exec might preempt List_remove.  List_put is
                 *  atomic.
                 */
                List_put (eventList, (List_Elem *) listener);
                if (listWasEmpty) {
                    /* Registering this event for the first time. Need to
                     * register the callback function.This call need  not to take
                     * the  gate.So passing acqGate as false.
                     */
                    status = _Notify_registerEventSingle (procId,
                                                          lineId,
                                                          eventId,
                                                          Notify_execMany,
                                                          (UArg) obj,
                                                           FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Notify_registerEvent",
                                         status,
                                         "Notify_registerEventSingle failed!");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = notify_register_event(procId,
                                   lineId,
                                   eventId,
                                   fnNotifyCbck,
                                   cbckArg);
#endif

    GT_1trace (curTrace, GT_LEAVE, "Notify_registerEvent", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}

/* Register a single callback for an event */
Int
Notify_registerEventSingle (UInt16              procId,
                            UInt16              lineId,
                            UInt32              eventId,
                            Notify_FnNotifyCbck fnNotifyCbck,
                            UArg                cbckArg)
{
    GT_5trace (curTrace, GT_ENTER, "Notify_registerEventSingle",
               procId, lineId, eventId, fnNotifyCbck, cbckArg);

#if defined (USE_SYSLINK_NOTIFY)
    return (_Notify_registerEventSingle (procId,
                                         lineId,
                                         eventId,
                                         fnNotifyCbck,
                                         cbckArg,
                                         TRUE));

#else
    return (notify_register_event_single (procId,
                                          lineId,
                                          eventId,
                                          fnNotifyCbck,
                                          cbckArg));
#endif
}

/* Register a single callback for an event */
static inline Int
_Notify_registerEventSingle (UInt16              procId,
                             UInt16              lineId,
                             UInt32              eventId,
                             Notify_FnNotifyCbck fnNotifyCbck,
                             UArg                cbckArg,
                             bool                acqGate)
{
    Int32               status      = Notify_S_SUCCESS;
    UInt32              strippedEventId = (eventId & Notify_EVENT_MASK);
    INotifyDriver_Handle driverHandle;
    Notify_Object *     obj;
    IArg                key;

    GT_5trace (curTrace, GT_ENTER, "_Notify_registerEventSingle",
               procId, lineId, eventId, fnNotifyCbck, cbckArg);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (fnNotifyCbck != NULL));
    /* cbckArg is optional and may be NULL. */
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));
    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_registerEventSingle",
                             status,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*! @retval  Notify_E_INVALIDARG Invalid procId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_registerEventSingle",
                             status,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*! @retval  Notify_E_INVALIDARG Invalid lineId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_registerEventSingle",
                             status,
                             "Invalid lineId argument provided");
    }
    else if (fnNotifyCbck == NULL) {
        /*! @retval  Notify_E_INVALIDARG Invalid NULL fnNotifyCbck argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_registerEventSingle",
                             status,
                             "Invalid NULL fnNotifyCbck provided.");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*! @retval  Notify_E_EVTNOTREGISTERED Invalid eventId specified. */
        status = Notify_E_EVTNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_registerEventSingle",
                             status,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId , Notify_module->cfg.reservedEvents )) {
        /*! @retval  Notify_E_EVTRESERVED Invalid usage of reserved event
                                            number. */
        status = Notify_E_EVTRESERVED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_registerEventSingle",
                             status,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (acqGate == TRUE) {
            /* Enter critical section protection. */
            key = IGateProvider_enter (Notify_module->gateHandle);
        }

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Notify_registerEventSingle",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj->callbacks [strippedEventId].fnNotifyCbck != NULL) {
                /*! @retval Notify_E_ALREADYEXISTS Callback is already
                                                   registered for this event! */
                status = Notify_E_ALREADYEXISTS;
                /* Don't use GT_setFailureReason here. The application
                 * might be trying to dynamically discover an unused
                 * notify event.
                 */
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /*
                 * No callback is registered. Register it. There is no need to
                 * protect these modifications because the event isn't
                 * registered yet.
                 */
                obj->callbacks [strippedEventId].fnNotifyCbck = fnNotifyCbck;
                obj->callbacks [strippedEventId].cbckArg = cbckArg;

                if (procId != MultiProc_self ()) {
                    GT_assert (curTrace, (driverHandle != NULL));
                    /* Tell the remote notify driver that the event is now
                     * registered
                     */
                    status = INotifyDriver_registerEvent (driverHandle,
                                                          strippedEventId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                       GT_4CLASS,
                                       "_Notify_registerEventSingle",
                                       status,
                                       "Driver specific registerEvent function"
                                       " failed");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (acqGate == TRUE) {
            /* Leave critical section protection. */
            IGateProvider_leave (Notify_module->gateHandle, key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Notify_registerEventSingle", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/* Un-register the callback for the specific event with the Notify module.*/
Int32
Notify_unregisterEvent (UInt16              procId,
                        UInt16              lineId,
                        UInt32              eventId,
                        Notify_FnNotifyCbck fnNotifyCbck,
                        UArg                cbckArg)
{
    Int32                  status      = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    UInt32                 strippedEventId = (eventId & Notify_EVENT_MASK);
    List_Elem *            listener = NULL;
    Bool                   found    = FALSE;
    INotifyDriver_Handle    driverHandle;
    List_Handle            eventList;
    Notify_Object *        obj;
    IArg                   modKey;
    IArg                   sysKey;
#endif
    GT_5trace (curTrace, GT_ENTER, "Notify_unregisterEvent",
               procId, lineId, eventId, fnNotifyCbck, cbckArg);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (fnNotifyCbck != NULL));
    /* cbckArg is optional and may be NULL. */
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));
    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_unregisterEvent",
                             status,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*! @retval  Notify_E_INVALIDARG Invalid procId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_unregisterEvent",
                             status,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*! @retval  Notify_E_INVALIDARG Invalid lineId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_unregisterEvent",
                             status,
                             "Invalid lineId argument provided");
    }
    else if (fnNotifyCbck == NULL) {
        /*! @retval  Notify_E_INVALIDARG Invalid NULL fnNotifyCbck argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_unregisterEvent",
                             status,
                             "Invalid NULL fnNotifyCbck provided.");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*! @retval  Notify_E_EVTNOTREGISTERED Invalid eventId specified. */
        status = Notify_E_EVTNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_unregisterEvent",
                             status,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId , Notify_module->cfg.reservedEvents )) {
        /*! @retval  Notify_E_EVTRESERVED Invalid usage of reserved event
                                            number. */
        status = Notify_E_EVTRESERVED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_unregisterEvent",
                             status,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        modKey = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_enableEvent",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            eventList = &(obj->eventList [strippedEventId]);

            if (List_empty (eventList) == TRUE) {
                /*! @retval Notify_E_NOTFOUND Failed to find the registration
                                          since list of listeners is empty! */
                status = Notify_E_NOTFOUND;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Notify_unregisterEvent",
                                         status,
                                         "Failed to find the registration since"
                                         " list of listeners is empty!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Traverse the list to find the element to unregister. */
                List_traverse (listener, eventList) {
                    if (   (((Notify_EventListener *) listener)->
                                        callback.fnNotifyCbck == fnNotifyCbck)
                    && (((Notify_EventListener *) listener)->callback.cbckArg ==
                                            cbckArg)) {
                        /* Found a match! */
                        found = TRUE;
                        break;
                    }
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (found == FALSE) {
                    /*! @retval Notify_E_NOTFOUND Event listener not found! */
                    status = Notify_E_NOTFOUND;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Notify_unregisterEvent",
                                         status,
                                         "Event listener not found!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /*
                     *  Need to atomically remove from the list using the system
                     *  gate because Notify_exec might preempt List_remove (the
                     *  event is still registered) TBD
                     */
                    sysKey = Gate_enterSystem ();
                    List_remove (eventList, listener);
                    Gate_leaveSystem (sysKey);
                    if (List_empty (eventList) == TRUE) {
                        /*
                         * Last callback being removed.
                         * Unregister the callback function.pass acgGate as false
                         * as we have already taken the gate.
                         */
                        status = _Notify_unregisterEventSingle (procId,
                                                                lineId,
                                                                eventId,
                                                                FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "Notify_unregisterEvent",
                                        status,
                                        "Notify_unregisterEventSingle failed!");
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }

                    /* Free the memory alloc'ed for the event listener */
                    Memory_free (NULL, listener, sizeof (Notify_EventListener));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, modKey);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = notify_unregister_event(procId,
                                     lineId,
                                     eventId,
                                     fnNotifyCbck,
                                     cbckArg);
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_unregisterEvent",status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return status ;
}


/* Remove an event listener from an event */
Int32
Notify_unregisterEventSingle (UInt16              procId,
                              UInt16              lineId,
                              UInt32              eventId)
{
    GT_3trace (curTrace, GT_ENTER, "Notify_unregisterEventSingle",
               procId, lineId, eventId);

#if defined (USE_SYSLINK_NOTIFY)
return (_Notify_unregisterEventSingle (procId, lineId, eventId, TRUE));

#else
    return (notify_unregister_event_single (procId, lineId, eventId));
#endif
}

/* internal inline function  to Remove an event listener from an event */
static inline Int32
_Notify_unregisterEventSingle (UInt16              procId,
                               UInt16              lineId,
                               UInt32              eventId,
                               bool                acqGate)
{
    Int32                  status      = Notify_S_SUCCESS;
    UInt32                 strippedEventId = (eventId & Notify_EVENT_MASK);
    INotifyDriver_Handle   driverHandle;
    Notify_Object *        obj;
    IArg                   key;

    GT_4trace (curTrace, GT_ENTER, "_Notify_unregisterEventSingle",
               procId, lineId, eventId, acqGate);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));
    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_unregisterEventSingle",
                             status,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*! @retval  Notify_E_INVALIDARG Invalid procId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_unregisterEventSingle",
                             status,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*! @retval  Notify_E_INVALIDARG Invalid lineId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_unregisterEventSingle",
                             status,
                             "Invalid lineId argument provided");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*! @retval  Notify_E_EVTNOTREGISTERED Invalid eventId specified. */
        status = Notify_E_EVTNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_unregisterEventSingle",
                             status,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId, Notify_module->cfg.reservedEvents)) {
        /*! @retval  Notify_E_EVTRESERVED Invalid usage of reserved event
                                            number. */
        status = Notify_E_EVTRESERVED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_unregisterEventSingle",
                             status,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. TBD: nesting */
        if (acqGate == TRUE) {
            key = IGateProvider_enter (Notify_module->gateHandle);
        }

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Notify_unregisterEventSingle",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj->callbacks [strippedEventId].fnNotifyCbck == NULL) {
                /*! @retval  Notify_E_FAIL No callback is registered. */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_Notify_unregisterEventSingle",
                                     status,
                                     "No callback is registered");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (procId != MultiProc_self()) {
                    GT_assert (curTrace, (driverHandle != NULL));
                    /* Pass on the call to the driver. */
                    status = INotifyDriver_unregisterEvent (driverHandle,
                                                            strippedEventId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_Notify_unregisterEventSingle",
                                             status,
                                             "Driver specific unregisterEvent"
                                             " function failed");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

                /*
                 *  No need to protect these modifications with the system gate
                 *  because we shouldn't get preempted by Notify_exec after
                 *  driver-specific unregisterEvent.
                 */
                obj->callbacks [strippedEventId].fnNotifyCbck = NULL;
                obj->callbacks [strippedEventId].cbckArg = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (acqGate == TRUE) {
            /* Leave critical section protection. */
            IGateProvider_leave (Notify_module->gateHandle, key);
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Notify_unregisterEventSingle", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return status ;
}

/* Send a notification to the specified event. */
Int
Notify_sendEvent (UInt16              procId,
                  UInt16              lineId,
                  UInt32              eventId,
                  UInt32              payload,
                  Bool                waitClear)
{

    Int32                status      = Notify_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    UInt32               strippedEventId = (eventId & Notify_EVENT_MASK);
    INotifyDriver_Handle driverHandle;
    Notify_Object *      obj;
    IArg                 sysKey;
#endif
    GT_5trace (curTrace, GT_ENTER, "Notify_sendEvent",
               procId, lineId, eventId, payload, waitClear);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));
    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
       /*! @retval  Notify_E_INVALIDSTATE Notify module not setup */
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_sendEvent",
                             status,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*! @retval  Notify_E_INVALIDARG Invalid procId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_sendEvent",
                             status,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*! @retval  Notify_E_INVALIDARG Invalid lineId argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_sendEvent",
                             status,
                             "Invalid lineId argument provided");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*! @retval  Notify_E_EVTNOTREGISTERED Invalid eventId specified. */
        status = Notify_E_EVTNOTREGISTERED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_sendEvent",
                             status,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId , Notify_module->cfg.reservedEvents )) {
        /*! @retval  Notify_E_EVTRESERVED Invalid usage of reserved event
                                            number. */
        status = Notify_E_EVTRESERVED;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_sendEvent",
                             status,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            status = Notify_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_sendEvent",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            if (procId != MultiProc_self ()) {
                GT_assert (curTrace, (driverHandle != NULL));
                /* Pass on the call to the driver. */
                status = INotifyDriver_sendEvent (driverHandle,
                                                  strippedEventId,
                                                  payload,
                                                  waitClear);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                /* Some failures are run-time and should not generate set
                 * failure reason.
                 */
                if (   (status < 0)
                        && (status != Notify_E_NOTINITIALIZED)
                        && (status != Notify_E_EVTNOTREGISTERED)
                        && (status != Notify_E_EVTDISABLED)) {
                    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_sendEvent",
                                 status,
                                 "Driver specific sendEvent function failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else {
                /*
                 *  The check agaist non-NULL fnNotifyCbCk must be atomic with
                 *  Notify_exec so Notify_exec doesn't encounter a null callback.
                 */
                sysKey = Gate_enterSystem();

                /*
                 * If nesting == 0 (the driver is enabled) and the event is
                 * enabled, send the event
                 */
                if (obj->callbacks [strippedEventId].fnNotifyCbck == NULL) {
                    /* No callbacks are registered locally for the event. */
                    status = Notify_E_EVTNOTREGISTERED;
                    /* This may be used for polling till other-side is ready, so
                     * do not set failure reason.
                     */
                }
                else if (obj->nesting != 0) {
                    /* Driver is disabled */
                    status = Notify_E_FAIL;
                    /* This may be used for polling till other-side is ready, so
                     * do not set failure reason.
                     */
                }
                else if (!TEST_BIT (Notify_module->localEnableMask, strippedEventId)){
                    /* Event is disabled */
                    status = Notify_E_EVTDISABLED;
                    /* This may be used for polling till other-side is ready, so
                     * do not set failure reason.
                     */
                }
                else {
                    Gate_leaveSystem(sysKey);
                    /* Execute the callback function registered to the event */
                    Notify_exec ((Notify_Handle) obj, strippedEventId, payload);
                    status = Notify_S_SUCCESS;
                    sysKey = Gate_enterSystem();
                }

                Gate_leaveSystem(sysKey);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = notify_send_event(procId, lineId, eventId, payload, waitClear);
#endif

    GT_1trace (curTrace, GT_LEAVE, "Notify_sendEvent", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/* Disable all events for specified procId and lineId. This is equivalent to
 * interrupt disable for specified processor, however restricted within the
 * interrupt handled by the Notify module as indicated by the lineId. All
 * callbacks registered for all events are disabled with this API. It is not
 * possible to disable a specific callback.
 */
UInt
Notify_disable (UInt16 procId, UInt16 lineId)
{
    UInt16               key = 0;
#if defined (USE_SYSLINK_NOTIFY)
    INotifyDriver_Handle driverHandle;
    Notify_Object *      obj;
    IArg                 modKey;
#endif
    GT_2trace (curTrace, GT_ENTER, "Notify_disable", procId, lineId);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /* No retVal since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disable",
                             Notify_E_INVALIDSTATE,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /* No retVal since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disable",
                             Notify_E_INVALIDARG,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /* No retVal since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disable",
                             Notify_E_INVALIDARG,
                             "Invalid lineId argument provided");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        modKey = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_disable",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            obj->nesting++;
            if (obj->nesting == 1) {
                /* Disable receiving all events */
                if (procId != MultiProc_self ()) {
                    GT_assert (curTrace, (driverHandle != NULL));
                    INotifyDriver_disable (driverHandle);
                }
            }

            key = obj->nesting;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, modKey);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    key = notify_disable(procId, lineId);
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_disable", key);

    return (key);
}


/* Restore the Notify module for specified procId and lineID to its state before
 * the corresponding Notify_disable was called.
 * This is equivalent to global interrupt restore, however restricted within
 *              interrupts handled by the Notify module.
 * All callbacks registered for all events as specified in the flags are enabled
 * with this API. It is not possible to enable a specific callback.
 */
Void
Notify_restore (UInt16 procId, UInt16 lineId, UInt key)
{
#if defined (USE_SYSLINK_NOTIFY)
    INotifyDriver_Handle driverHandle;
    IArg                 modKey;
    Notify_Object *      obj;
#endif
    GT_3trace (curTrace, GT_ENTER, "Notify_restore", procId, lineId, key);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /* No retVal since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_restore",
                             Notify_E_INVALIDSTATE,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /* No retVal since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_restore",
                             Notify_E_INVALIDARG,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /* No retVal since this function does not return status. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_restore",
                             Notify_E_INVALIDARG,
                             "Invalid lineId argument provided");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        modKey = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_restore",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            GT_assert (curTrace, (key == obj->nesting));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (key != obj->nesting) {
                /* No retVal since this function does not return status. */
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Notify_restore",
                                     Notify_E_INVALIDSTATE,
                                     "Invalid nesting state for Notify module");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                obj->nesting--;
                if (obj->nesting == 0) {
                    /* Enable receiving events */
                    if (procId != MultiProc_self ()) {
                        /* Pass on the call to the driver. */
                        GT_assert (curTrace, (driverHandle != NULL));
                        INotifyDriver_enable (driverHandle);
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, modKey);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    notify_restore(procId, lineId, key);
#endif

    GT_0trace (curTrace, GT_LEAVE, "Notify_restore");
}


/* Disable a specific event.
 * All callbacks registered for the specific event are disabled with this API.
 * It is not possible to disable a specific callback.
 */
Void
Notify_disableEvent (UInt16              procId,
                     UInt16              lineId,
                     UInt32              eventId)
{
#if defined (USE_SYSLINK_NOTIFY)
    UInt32               strippedEventId = (eventId & Notify_EVENT_MASK);
    INotifyDriver_Handle driverHandle;
    Notify_Object *      obj;
    IArg                 key;
#endif
    GT_3trace (curTrace, GT_ENTER, "Notify_disableEvent",
               procId, lineId, eventId);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));
    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disableEvent",
                             Notify_E_INVALIDSTATE,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disableEvent",
                             Notify_E_INVALIDARG,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disableEvent",
                             Notify_E_INVALIDARG,
                             "Invalid lineId argument provided");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disableEvent",
                             Notify_E_EVTNOTREGISTERED,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId , Notify_module->cfg.reservedEvents )) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_disableEvent",
                             Notify_E_EVTRESERVED,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_disableEvent",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            if (procId != MultiProc_self ()) {
                /* disable the remote event */
                GT_assert (curTrace, (driverHandle != NULL));
                INotifyDriver_disableEvent (driverHandle, strippedEventId);
            }
            else {
                /* disable the local event */
                CLEAR_BIT (Notify_module->localEnableMask, strippedEventId);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    notify_disable_event(procId, lineId, eventId);
#endif

    GT_0trace (curTrace, GT_LEAVE, "Notify_disableEvent");
}


/* Enable a specific event.
 * All callbacks registered for this specific event are enabled
 * with this API. It is not possible to enable a specific callback.
 */
Void
Notify_enableEvent (UInt16              procId,
                    UInt16              lineId,
                    UInt32              eventId)
{
#if defined (USE_SYSLINK_NOTIFY)
    UInt32               strippedEventId = (eventId & Notify_EVENT_MASK);
    INotifyDriver_Handle driverHandle;
    Notify_Object *      obj;
    IArg                 key;
#endif
    GT_3trace (curTrace, GT_ENTER, "Notify_enableEvent",
               procId, lineId, eventId);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));
    GT_assert (curTrace, (ISRESERVED(eventId , Notify_module->cfg.reservedEvents )));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_enableEvent",
                             Notify_E_INVALIDSTATE,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_enableEvent",
                             Notify_E_INVALIDARG,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_enableEvent",
                             Notify_E_INVALIDARG,
                             "Invalid lineId argument provided");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_enableEvent",
                             Notify_E_EVTNOTREGISTERED,
                             "Invalid eventId specified.");
    }
    else if (!ISRESERVED(eventId, Notify_module->cfg.reservedEvents)) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_enableEvent",
                             Notify_E_EVTRESERVED,
                             "Invalid usage of reserved event number");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Notify_enableEvent",
                                 Notify_E_INVALIDSTATE,
                                 "Notify driver not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            driverHandle = obj->driverHandle;
            /* driverHandle may be NULL for local Notify */

            if (procId != MultiProc_self ()) {
                /* enable the remote event */
                GT_assert (curTrace, (driverHandle != NULL));
                INotifyDriver_enableEvent (driverHandle, strippedEventId);
            }
            else {
                /* enable the local event */
                SET_BIT (Notify_module->localEnableMask, strippedEventId);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    notify_enable_event(procId, lineId, eventId);
#endif

GT_0trace (curTrace, GT_LEAVE, "Notify_enableEvent");
}


/* Whether event is available and free to be used. */
Bool
Notify_eventAvailable (UInt16 procId, UInt16 lineId, UInt32 eventId)
{
    Bool available = FALSE;
#if defined (USE_SYSLINK_NOTIFY)
    UInt32              strippedEventId = (eventId & Notify_EVENT_MASK);
    Notify_Object *     obj;
    IArg                key;
#endif

    GT_3trace (curTrace, GT_ENTER, "Notify_eventAvailable",
               procId, lineId, eventId);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (strippedEventId < (Notify_module->cfg.numEvents)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_eventAvailable",
                             Notify_E_INVALIDSTATE,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_eventAvailable",
                             Notify_E_INVALIDARG,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_eventAvailable",
                             Notify_E_INVALIDARG,
                             "Invalid lineId argument provided");
    }
    else if (strippedEventId >= (Notify_module->cfg.numEvents)) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_eventAvailable",
                             Notify_E_EVTNOTREGISTERED,
                             "Invalid eventId specified.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (Notify_module->gateHandle);

        obj = Notify_module->notifyHandles [procId][lineId];

        /* Driver not registered or event is reserved and caller is not using
         * system key
         */
        if (    (obj == NULL)
            ||  (!ISRESERVED(eventId, Notify_module->cfg.reservedEvents))) {
            available = FALSE;
        }
        else {
            available = (obj->callbacks[strippedEventId].fnNotifyCbck == NULL);
        }

        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    available = notify_event_available (procId, lineId, eventId);
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_eventAvailable", available);

    return(available);
}


/* Whether notification via interrupt line has been registered. */
Bool
Notify_intLineRegistered(UInt16 procId, UInt16 lineId)
{
    Bool isRegistered = FALSE;

    GT_2trace (curTrace, GT_ENTER, "Notify_intLineRegistered",
               procId, lineId);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Notify_module->refCount),
                                  Notify_MAKE_MAGICSTAMP(0),
                                  Notify_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_intLineRegistered",
                             Notify_E_INVALIDSTATE,
                             "Notify module not setup");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_intLineRegistered",
                             Notify_E_INVALIDARG,
                             "Invalid procId argument provided");
    }
    else if (lineId >= Notify_MAX_INTLINES) {
        /*  No retVal since function is Void */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Notify_intLineRegistered",
                             Notify_E_INVALIDARG,
                             "Invalid lineId argument provided");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        isRegistered = (Notify_module->notifyHandles[procId][lineId] != NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    isRegistered = notify_intline_registered(procId, lineId);
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_intLineRegistered", isRegistered);

    return isRegistered;
}


/* Creates notify drivers and registers them with Notify */
Int
Notify_attach (UInt16 procId, Ptr sharedAddr)
{
    Int status = Notify_S_SUCCESS;
    GT_1trace (curTrace, GT_ENTER, "Notify_attach",
               sharedAddr);

#if defined (USE_SYSLINK_NOTIFY)
    /* Use the NotifySetup proxy to setup drivers */
    status = Notify_SetupProxy_attach (procId, sharedAddr);

    Notify_module->startComplete = TRUE;

#else
    status = notify_attach (procId, sharedAddr);
#endif
    GT_0trace (curTrace, GT_LEAVE, "Notify_attach");
    return(status);
}


/* Creates notify drivers and registers them with Notify */
Int
Notify_detach (UInt16 procId)
{
    Int status = 0;

    GT_0trace (curTrace, GT_ENTER, "Notify_detach");

#if defined (USE_SYSLINK_NOTIFY)
    /* Use the NotifySetup proxy to destroy drivers */
    status = Notify_SetupProxy_detach (procId);

    Notify_module->startComplete = FALSE;
#else
    status = notify_detach(procId);
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_detach", status);

    return status;
}


/* =============================================================================
 *  Internal functions
 * =============================================================================
 */
/* Returns the total amount of shared memory used by the Notify module
 * and all instances after Notify_start has been called.
 */
SizeT
Notify_sharedMemReq (UInt16 procId,
                     Ptr sharedAddr)
{
    SizeT memReq = 0x0;

    GT_1trace (curTrace, GT_ENTER, "Notify_sharedMemReq",
               sharedAddr);
#if defined (USE_SYSLINK_NOTIFY)

    if (MultiProc_getNumProcessors () > 1) {
        /* Determine device-specific shared memory requirements */
        memReq = Notify_SetupProxy_sharedMemReq (procId, sharedAddr);
    }
    else {
        /* Only 1 processor: no shared memory needed */
        memReq = 0;
    }
#else
    memReq = notify_shared_mem_req(procId, sharedAddr);
#endif
    GT_1trace (curTrace, GT_LEAVE, "Notify_sharedMemReq", memReq);

    return (memReq);
}


/* Indicates whether Notify_start is completed. */
Bool
Notify_startComplete(Void)
{
    return (Notify_module->startComplete);
}


/* Function registered as callback with the Notify driver */
Void
Notify_exec (Notify_Object * obj, UInt32 eventId, UInt32 payload)
{
    Notify_EventCallback * callback;

    GT_3trace (curTrace, GT_ENTER, "Notify_exec", obj, eventId, payload);

    GT_assert (curTrace, (obj != NULL));
    GT_assert (curTrace, (eventId < (Notify_module->cfg.numEvents)));

    callback = &(obj->callbacks [eventId]);
    GT_assert (curTrace, (callback->fnNotifyCbck != NULL));
    /* Execute the callback function with its argument and the payload */
    callback->fnNotifyCbck (obj->remoteProcId,
                            obj->lineId,
                            eventId,
                            callback->cbckArg,
                            payload);

    GT_0trace (curTrace, GT_LEAVE, "Notify_exec");
}


/* Function registered with Notify_exec when multiple registrations are present
 * for the events.
 */
Void
Notify_execMany (UInt16 procId,
                 UInt16 lineId,
                 UInt32 eventId,
                 UArg   arg,
                 UInt32 payload)
{
    Notify_Object * obj = (Notify_Object *) arg;
    List_Handle eventList;
    List_Elem * listener;
    IArg key;

    GT_3trace (curTrace, GT_ENTER, "Notify_execMany", obj, eventId, payload);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (obj != NULL));
    GT_assert (curTrace, (lineId < Notify_MAX_INTLINES));
    GT_assert (curTrace, (eventId < (Notify_module->cfg.numEvents)));

    /* Both loopback and the the event itself are enabled */
    eventList = &(obj->eventList [eventId]);

    /* Enter critical section protection. */
    key = IGateProvider_enter (Notify_module->gateHandle);
    /* Use "NULL" to get the first EventListener on the list */
    List_traverse (listener, eventList) {
        /* Leave critical section protection. */
        IGateProvider_leave (Notify_module->gateHandle, key);
        ((Notify_EventListener *) listener)->callback.fnNotifyCbck (procId,
                          lineId,
                          eventId,
                          ((Notify_EventListener *) listener)->callback.cbckArg,
                          payload);
        /* Enter critical section protection. */
        key = IGateProvider_enter (Notify_module->gateHandle);
    }

    /* Leave critical section protection. */
    IGateProvider_leave (Notify_module->gateHandle, key);

    GT_0trace (curTrace, GT_LEAVE, "Notify_execMany");
}
