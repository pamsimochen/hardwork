/*
 *  @file   NameServerRemoteNotify.c
 *
 *  @brief      NameServer Remote Transport
 *
 *              This module provides functionality to get name value pair from a
 *              remote nameserver. it uses notify to send packet containing data
 *              to the remote processor and then waits on a semaphore. Other
 *              processor on getting a notify event, finds the name value pair
 *              in the local nameservers. if it find the pair then it sends out
 *              a notify event with necessary packet to the other processor.<br>
 *              Modules specific name value pair are distinguished by looking
 *              into the name field, name field would be <module_name>:name type
 *              . So first, remote processor would get the module specific
 *              nameserver handle from the nameserver_modules nameserver by
 *              providing <module_name>, once it gets the module specific
 *              nameserver handle, it uses the name to find out the pair and
 *              sends it back.
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



/* Standard & data types headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalSemaphore.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/_NameServer.h>
#include <ti/syslink/inc/NameServerRemote.h>
#include <ti/syslink/inc/NameServerRemoteNotify.h>
#include <ti/syslink/inc/Bitops.h>

/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/Notify.h>


/* =============================================================================
 * Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(x)       \
                    ((NAMESERVERREMOTENOTIFY_MODULEID << 12u) | (x))
/*!
 *  @brief  Cache line size
 */
#define NAMESERVERREMOTENOTIFY_CACHESIZE   128u

/*!
 *  @brief  Maximum length of value buffer that can be stored in the NameServer
 *          managed by this NameServerRemoteNotify instance. Value in 4-byte
 *          words.
 */
#define NAMESERVERREMOTENOTIFY_MAXVALUEBUFLEN   75

#define NameServerRemoteNotify_RESERVED_EVENTNO (4u)

#define NameServerRemoteNotify_notifyEventId \
                                      NameServerRemoteNotify_RESERVED_EVENTNO \
                                             +  (UInt32)(Notify_SYSTEMKEY << 16)

/* Defines the NameServerRemoteNotify state object, which contains all the
 * module specific information.
 */
typedef struct NameServerRemoteNotify_ModuleObject_tag {
    NameServerRemoteNotify_Config    cfg;
    /* NameServerRemoteNotify configuration structure */
    NameServerRemoteNotify_Config    defCfg;
    /* Default module configuration */
    NameServerRemoteNotify_Params    defInstParams;
    /* Default instance parameters */
    Bool                             isSetup;
    /* Indicates whether the NameServerRemoteNotify module is setup. */
    IGateProvider_Handle             gateHandle;
    /* Handle to the gate for local thread safety */
    Atomic                           refCount;
    NameServerRemoteNotify_Handle    nsrHandles [MultiProc_MAXPROCESSORS];
    /* Reference count */
} NameServerRemoteNotify_ModuleObject;

/* NameServer remote transport packet definition */
typedef struct NameServerRemoteNotify_Message_tag {
    Bits32 request;
    /* If this is a request set to 1 */
    Bits32 response;
    /* If this is a response set to 1 */
    Bits32 requestStatus;
    /* If request sucessful set to 1 */
    Bits32 value;
    /* Holds value if len <= 4 */
    Bits32 valueLen;
    /* Len of value */
    Bits32 instanceName [8];
    /* Name of NameServer instance */
    Bits32 name [8]; /* Size is 8 to align to 128 cache line boundary */
    /* Name of NameServer entry */
    Bits32 valueBuf [NAMESERVERREMOTENOTIFY_MAXVALUEBUFLEN];
    /* Supports up to 300-byte value */
} NameServerRemoteNotify_Message;

/* NameServer remote transport state object definition */
typedef struct NameServerRemoteNotify_Obj {
    NameServerRemoteNotify_Message * msg [2];
    /* Pointer to the packets */
    NameServerRemoteNotify_Params    params;
    /* Configuration values used for initiailzation */
    UInt16                           regionId;
    /* SharedRegion ID */
    UInt16                           remoteProcId;
    /* Processor Identifier for remote processor */
    Bool                             cacheEnable;
    /* cacheability                   */
    IGateProvider_Handle             localGate;
    /* gate used for protecting from processes/threads */
    GateMP_Handle                    gate;
    /* gate used for protecting from processes/threads */
    OsalSemaphore_Handle             semHandle;
    /* Handle to the semaphore */
} NameServerRemoteNotify_Obj;

/* NameServer remote transport state object definition */
typedef struct NameServerRemoteNotify_Object {
    NameServerRemote_GetFxn get;
    /* Function to get data from remote nameserver */
    Ptr                     obj;
    /* Implementation specific object */
} NameServerRemoteNotify_Object;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    NameServerRemoteNotify_state
 *
 *  @brief  NameServerRemoteNotify state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NameServerRemoteNotify_ModuleObject NameServerRemoteNotify_state =
{
    .isSetup = FALSE,
    .gateHandle = NULL,
    .defCfg.notifyEventId = NameServerRemoteNotify_RESERVED_EVENTNO,
    .defInstParams.gate = NULL,
    .defInstParams.sharedAddr = 0x0,
};


/* =============================================================================
 * Forward declarations of internal functions
 * =============================================================================
 */
/* Forward declaration of function called when a Notify event is received.*/
Void
_NameServerRemoteNotify_callback (UInt16 procId,
                                  UInt16 lineId,
                                  UInt32 eventId,
                                  UArg   arg,
                                 UInt32 payload);


/* =============================================================================
 * APIs called directly by applications
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the NameServerRemoteNotify
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to NameServerRemoteNotify_setup filled
 *              in by the NameServerRemoteNotify module with the default
 *              parameters. If the user does not wish to make any change in the
 *              default parameters, this API is not required to be called.
 *
 *  @param      cfg        Pointer to the NameServerRemoteNotify module
 *                         configuration structure in which the default config
 *                         is to be returned.
 *
 *  @sa         NameServerRemoteNotify_setup
 */
Void
NameServerRemoteNotify_getConfig (NameServerRemoteNotify_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "NameServerRemoteNotify_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_getConfig",
                             NAMESERVERREMOTENOTIFY_E_INVALIDARG,
                             "Argument of type (NameServerRemoteNotify_Config *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (NameServerRemoteNotify_state.isSetup == FALSE) {
            Memory_copy (cfg,
                         &(NameServerRemoteNotify_state.defCfg),
                         sizeof (NameServerRemoteNotify_Config));
        }
        else {
            Memory_copy (cfg,
                         &(NameServerRemoteNotify_state.cfg),
                         sizeof (NameServerRemoteNotify_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NameServerRemoteNotify_getConfig");
}


/*!
 *  @brief      Setup the NameServerRemoteNotify module.
 *
 *              This function sets up the NameServerRemoteNotify module. This
 *              function must be called before any other instance-level APIs can
 *              be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then NameServerRemoteNotify_getConfig can be called
 *              to get the configuration filled with the default values. After
 *              this, only the required configuration values can be changed. If
 *              the user does not wish to make any change in the default
 *              parameters, the application can simply call
 *              NameServerRemoteNotify_setup with NULL parameters. The default
 *              parameters would get automatically used.
 *
 *  @param      cfg   Optional NameServerRemoteNotify module configuration.
 *                    If provided as NULL, default configuration is used.
 *
 *  @sa         NameServerRemoteNotify_destroy
 *              GateMutex_create
 */
Int
NameServerRemoteNotify_setup (NameServerRemoteNotify_Config * cfg)
{
    Int                           status = NAMESERVERREMOTENOTIFY_SUCCESS;
    Error_Block                   eb = 0;
    NameServerRemoteNotify_Config tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "NameServerRemoteNotify_setup", cfg);

    if (cfg == NULL) {
        NameServerRemoteNotify_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&NameServerRemoteNotify_state.refCount,
                            NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                            NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&NameServerRemoteNotify_state.refCount)
        != NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1u)) {
        status = NameServer_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "NameServerRemoteNotify Module already initialized!");
    }
    else {
        /* Create a default gate handle for local module protection. */
        NameServerRemoteNotify_state.gateHandle = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NameServerRemoteNotify_state.gateHandle == NULL) {
            /*! @retval NAMESERVERREMOTENOTIFY_E_FAIL Failed to create GateMutex! */
            status = NAMESERVERREMOTENOTIFY_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerRemoteNotify_setup",
                                 status,
                                 "Failed to create GateMutex!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&NameServerRemoteNotify_state.cfg,
                         cfg,
                         sizeof (NameServerRemoteNotify_Config));
            Memory_set (&NameServerRemoteNotify_state.nsrHandles,
                        0,
                        (   sizeof (NameServerRemoteNotify_Handle)
                         *  MultiProc_MAXPROCESSORS));
            NameServerRemoteNotify_state.isSetup = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "NameServerRemoteNotify_setup", status);

    /*! @retval NAMESERVERREMOTENOTIFY_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Destroy the NameServerRemoteNotify module.
 *
 *              Once this function is called, other NameServerRemoteNotify
 *              module APIs, except for the NameServerRemoteNotify_getConfig API
 *              cannot be called anymore.
 *
 *  @sa         NameServerRemoteNotify_setup
 *              GateMutex_delete
 */
Int
NameServerRemoteNotify_destroy (Void)
{
    Int status = NAMESERVERREMOTENOTIFY_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "NameServerRemoteNotify_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NAMESERVERREMOTENOTIFY_E_INVALIDSTATE Module was not setup*/
        status = NAMESERVERREMOTENOTIFY_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_destroy",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&NameServerRemoteNotify_state.refCount)
            == NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0)) {
            if (NameServerRemoteNotify_state.gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                  &(NameServerRemoteNotify_state.gateHandle));
            }

            NameServerRemoteNotify_state.isSetup = FALSE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServerRemoteNotify_destroy", status);

    /*! @retval NAMESERVERREMOTENOTIFY_SUCCESS Operation successful */
    return (status);
}


/*! @brief  Function to get the current configuration values.
 *
 *  @params params Configuration values
 */
Void
NameServerRemoteNotify_Params_init (NameServerRemoteNotify_Params * params)
{
    GT_1trace (curTrace,
               GT_ENTER,
               "NameServerRemoteNotify_Params_init",
               params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_Params_init",
                             NAMESERVERREMOTENOTIFY_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_Params_init",
                             NAMESERVERREMOTENOTIFY_E_INVALIDARG,
                             "Argument of type (NameServerRemoteNotify_Params*)"
                             " passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Memory_copy (params,
                         &(NameServerRemoteNotify_state.defInstParams),
                         sizeof (NameServerRemoteNotify_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "NameServerRemoteNotify_Params_init");
}


/*!
 *  @brief      Function to setup the nameserver remote transport module.
 *
 *  @params     config configuration values.
 *
 *  @sa         NameServerRemoteNotify_destroy
 */
NameServerRemoteNotify_Handle
NameServerRemoteNotify_create (      UInt16                        remoteProcId,
                               const NameServerRemoteNotify_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                             status = NAMESERVERREMOTENOTIFY_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NameServerRemoteNotify_Object * handle = NULL;
    NameServerRemoteNotify_Obj    * obj    = NULL;
    Int                             offset = 0;
    Error_Block                     eb;

    GT_2trace (curTrace,
               GT_ENTER,
               "NameServerRemoteNotify_create",
               remoteProcId,
               params);

    GT_assert (curTrace, (params != NULL));
    GT_assert (curTrace, (remoteProcId != MultiProc_self ()));
    GT_assert (curTrace, (remoteProcId <  MultiProc_getNumProcessors ()));
    Error_init (&eb);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NAMESERVERREMOTENOTIFY_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_create",
                             status,
                             "Module was not setup!");
    }
    else if (   (remoteProcId == MultiProc_self ())
             || (remoteProcId >= MultiProc_getNumProcessors ())) {
        status = NAMESERVERREMOTENOTIFY_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_create",
                             status,
                             "Argument of type (NameServerRemoteNotify_Params*)"
                             "is NULL!");
    }
    else if (params == NULL) {
        status = NAMESERVERREMOTENOTIFY_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_create",
                             status,
                             "Argument of type (NameServerRemoteNotify_Params*)"
                             "is NULL!");
    }
    else if (params->sharedAddr == NULL) {
        status = NAMESERVERREMOTENOTIFY_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_create",
                             status,
                             "sharedAddr is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate memory for the obj */
        obj = (NameServerRemoteNotify_Obj *) Memory_calloc (NULL,
                                         sizeof (NameServerRemoteNotify_Obj),
                                         0,
                                         NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            status = NAMESERVERREMOTENOTIFY_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerRemoteNotify_create",
                                 status,
                                 "Failed to allocate memory for handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->localGate = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj->localGate == NULL) {
                status = NAMESERVERREMOTENOTIFY_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServerRemoteNotify_create",
                                     status,
                                     "Failed to create a local gate!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                obj->remoteProcId = remoteProcId;
                if (MultiProc_self () > remoteProcId) {
                    offset = 1;
                }

                obj->regionId = SharedRegion_getId (params->sharedAddr);

                GT_assert (curTrace,
                              ((  (UInt32) params->sharedAddr
                                % SharedRegion_getCacheLineSize (obj->regionId))
                           == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (   (  (UInt32) params->sharedAddr
                        %  SharedRegion_getCacheLineSize (obj->regionId))
                    != 0) {
                    status = NAMESERVERREMOTENOTIFY_E_ADDRNOTCACHEALIGNED;
                    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerRemoteNotify_create",
                                 status,
                                 "Failed to create a local gate!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj->msg [0] = (NameServerRemoteNotify_Message *)
                                                        (params->sharedAddr);
                    obj->msg [1] = (NameServerRemoteNotify_Message *)
                                 (   (UInt32) obj->msg[0]
                                  +  sizeof (NameServerRemoteNotify_Message));
                    obj->gate = params->gate;
                    obj->remoteProcId  = remoteProcId;

                    /* Clear out self shared structures */
                    Memory_set (obj->msg [offset],
                                0,
                                sizeof (NameServerRemoteNotify_Message));

                    /* copy the passed params structures */
                    Memory_copy ((Ptr) &obj->params,
                                 (Ptr) params,
                                 sizeof (NameServerRemoteNotify_Params));

                    /* determine cacheability of the object from the regionId */
                    obj->cacheEnable = SharedRegion_isCacheEnabled (
                                                                obj->regionId);
                    if (obj->cacheEnable) {
                        /* write back shared memory that was modified */
                        Cache_wbInv (obj->msg [offset],
                                     sizeof(NameServerRemoteNotify_Message),
                                     Cache_Type_ALL,
                                     TRUE);
                    }


                    /* Register the callback */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                             Notify_registerEventSingle (remoteProcId,
                                               0, /* TBD: Interrupt line Id */
                                               NameServerRemoteNotify_notifyEventId,
                                               _NameServerRemoteNotify_callback,
                                               (Ptr) obj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "NameServerRemoteNotify_create",
                                             status,
                                             "Failed to register events!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Allocate memory for the handle */
                        handle = (NameServerRemoteNotify_Object *)
                                        Memory_alloc (NULL,
                                         sizeof (NameServerRemoteNotify_Object),
                                         0,
                                         NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (handle == NULL) {
                                status = NAMESERVERREMOTENOTIFY_E_MEMORY;
                                GT_setFailureReason (curTrace,
                                       GT_4CLASS,
                                       "NameServerRemoteNotify_create",
                                       status,
                                       "Failed to allocate memory for handle!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            handle->get = (Ptr) &NameServerRemoteNotify_get;
                            handle->obj = obj;
                            NameServer_registerRemoteDriver (
                                               (NameServerRemote_Handle) handle,
                                               remoteProcId);
                            obj->semHandle =
                               OsalSemaphore_create (OsalSemaphore_Type_Binary);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                            if (obj->semHandle == NULL) {
                                status = NAMESERVERREMOTENOTIFY_E_FAIL;
                                GT_setFailureReason (curTrace,
                                                GT_4CLASS,
                                                "NameServerRemoteNotify_create",
                                                status,
                                                "Failed to create semaphore!");
                            }
                        }
                    }
                }
            }
        }

        if (status < 0) {
            /* Unregister the event from Notify */
            Notify_unregisterEventSingle (obj->remoteProcId,
                                          0,
                                          NameServerRemoteNotify_notifyEventId);

            if (handle != NULL) {
                NameServer_unregisterRemoteDriver (remoteProcId);
            }

            if (obj != NULL) {
                if (obj->localGate != NULL) {
                    GateMutex_delete ((GateMutex_Handle *)
                                            &obj->localGate);
                    obj->localGate = NULL;
                }

                Memory_free (NULL,
                             obj,
                             sizeof (NameServerRemoteNotify_Obj));
                obj = NULL;
            }

            if (handle != NULL) {
                Memory_free (NULL,
                            handle,
                            sizeof (NameServerRemoteNotify_Object));
                handle = NULL;
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServerRemoteNotify_create", handle);

    /*! @retval valid-handle operation was successful */
    /*! @retval NULL operation was not successful */
    return handle;
}


/*!
 *  @brief      Function to delete the nameserver remote transport instance.
 *
 *  @params     handle Handle to the NameServerRemotetransport instance.
 *
 *  @sa         NameServerRemoteNotify_create
 */
Int
NameServerRemoteNotify_delete (NameServerRemoteNotify_Handle * handle)
{
    Int                          status    = NAMESERVERREMOTENOTIFY_SUCCESS;
    Int                          tmpStatus = NAMESERVERREMOTENOTIFY_SUCCESS;
    NameServerRemoteNotify_Obj * obj    = NULL;
    IGateProvider_Handle         gateHandle;
    IArg                         key;

    GT_1trace (curTrace, GT_ENTER, "NameServerRemoteNotify_delete", handle);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, ((handle != NULL) && (*handle != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_delete",
                             NAMESERVERREMOTENOTIFY_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval NAMESERVERREMOTENOTIFY_E_HANDLE Invalid NULL handle
                                                  specified */
        status = NAMESERVERREMOTENOTIFY_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_delete",
                             status,
                             "Invalid NULL handle specified!");
    }
    else if (*handle == NULL) {
        /*! @retval NAMESERVERREMOTENOTIFY_E_HANDLE Invalid NULL *handle
                                         specified */
        status = NAMESERVERREMOTENOTIFY_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_delete",
                             status,
                             "Invalid NULL *handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NameServerRemoteNotify_Obj *)(*handle)->obj;
        if (obj != NULL) {
            key = IGateProvider_enter (obj->localGate);

            if (obj->semHandle != NULL) {
                /* Delete the semaphore */
                status = OsalSemaphore_delete (&obj->semHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NameServerRemoteNotify_delete",
                                         status,
                                         "Failed to delete OsalSemaphore object");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* Unregister the remote driver */
            tmpStatus = NameServer_unregisterRemoteDriver (obj->remoteProcId);
            if ((tmpStatus < 0) && (status >= 0)) {
                status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServerRemoteNotify_delete",
                                     status,
                                     "Failed to unregister remote driver!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* Unregister the event from Notify */
            tmpStatus = Notify_unregisterEventSingle (obj->remoteProcId,
                                                      0,
                                                      NameServerRemoteNotify_notifyEventId);
            if ((tmpStatus < 0) && (status >= 0)) {
                status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServerRemoteNotify_delete",
                                     status,
                                     "Failed to unregister Notify event!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            gateHandle = obj->localGate;

            /* Free the memory */
            Memory_free (NULL, obj, sizeof (NameServerRemoteNotify_Obj));

            /* Leave the gate */
            IGateProvider_leave (gateHandle, key);

            if (gateHandle != NULL) {
                status = GateMutex_delete ((GateMutex_Handle *) &gateHandle);
                if ((tmpStatus < 0) && (status >= 0)) {
                    status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServerRemoteNotify_delete",
                                     status,
                                     "Failed to delete GateMutex instance!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }
        }

        /* Free the memory */
        Memory_free (NULL, (*handle), sizeof (NameServerRemoteNotify_Object));
        (*handle) = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServerRemoteNotify_delete", status);

     /*! @retval status Operation successfully completed */
    return (status);
}


/*!
 *  @brief      Function to get remote name value pair.
 *
 *  @params     handle          Handle to the NameServerRemotetransport instance
 *  @param      instanceName    Name of the NameServer instance.
 *  @param      name            Name of the value.
 *  @param      value           Return parameter: Value
 *  @param      valueLen        Size of the value buffer in bytes
 *  @param      reserved        Reserved parameter, can be passed as NULL
 */
Int
NameServerRemoteNotify_get (NameServerRemoteNotify_Handle   handle,
                            String                          instanceName,
                            String                          name,
                            Ptr                             value,
                            UInt32 *                        valueLen,
                            Ptr                             reserved)
{
    Int                          status = NameServer_S_SUCCESS;
    Int                          len    = 0;
    NameServerRemoteNotify_Obj * obj    = NULL;
    Int                          offset = 0;
    IArg                         key;
    IArg                         localKey;

    GT_5trace (curTrace,
               GT_ENTER,
               "NameServerRemoteNotify_get",
               handle,
               instanceName,
               name,
               value,
               valueLen);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (instanceName != NULL));
    GT_assert (curTrace, (name != NULL));
    GT_assert (curTrace, (value != NULL));
    GT_assert (curTrace, (valueLen != NULL));
    GT_assert (curTrace,
               (    (valueLen != NULL)
                &&  (*valueLen > 0u)
                &&  (*valueLen <= NAMESERVERREMOTENOTIFY_MAXVALUEBUFLEN)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             NAMESERVERREMOTENOTIFY_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval NameServer_E_INVALIDARG Handle passed is NULL */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             status,
                             "Handle passed is NULL!");
    }
    else if (instanceName == NULL) {
        /*! @retval NameServer_E_INVALIDARG instanceName passed is NULL */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             status,
                             "instanceName passed is NULL!");
    }
    else if (name == NULL) {
        /*! @retval NameServer_E_INVALIDARG name passed is NULL */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             status,
                             "name passed is NULL!");
    }
    else if (value == NULL) {
        /*! @retval NameServer_E_INVALIDARG value passed is NULL */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             status,
                             "value passed is NULL!");
    }
    else if (valueLen == NULL) {
        /*! @retval NameServer_E_INVALIDARG valueLen passed is NULL */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             status,
                             "valueLen passed is NULL!");
    }
    else if (   (*valueLen == 0)
             || (*valueLen > NAMESERVERREMOTENOTIFY_MAXVALUEBUFLEN)) {
        /*! @retval NameServer_E_INVALIDARG Invalid valueLen passed */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_get",
                             status,
                             "Invalid *valueLen passed!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (NameServerRemoteNotify_Obj *) handle->obj;
        localKey = IGateProvider_enter (obj->localGate);
        if (MultiProc_self () > obj->remoteProcId) {
            offset = 1;
        }

        if (obj->cacheEnable) {
            /* Make sure there's no outstanding message */
            Cache_inv(obj->msg[offset], sizeof(NameServerRemoteNotify_Message),
                      Cache_Type_ALL, TRUE);
        }

        key = GateMP_enter (obj->gate);
        /* this is a request message */
        obj->msg[offset]->request = 1;
        obj->msg[offset]->response = 0;
        obj->msg[offset]->requestStatus = 0;
        obj->msg[offset]->valueLen = *valueLen;

        /* copy the name of instance into shared memory */
        len = String_len (instanceName);
        String_ncpy ((String) obj->msg[offset]->instanceName,
                     instanceName,
                     (len + 1));

        /* copy the name of nameserver entry into shared memory */
        len = String_len (name);
        String_ncpy ((String) obj->msg[offset]->name,
                     name,
                     (len + 1));

        if (obj->cacheEnable) {
            Cache_wbInv(obj->msg[offset], sizeof(NameServerRemoteNotify_Message),
                        Cache_Type_ALL, TRUE);
        }

        /* send the notification to remote processor */
        status = Notify_sendEvent(obj->remoteProcId,
                                  0,
                                  NameServerRemoteNotify_notifyEventId,
                                  0, /* Payload */
                                  FALSE); /* Not sending a payload */

        GateMP_leave (obj->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            /* Undo previous operations */
            obj->msg[offset]->request = 0;
            obj->msg[offset]->valueLen = 0;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerRemoteNotify_get",
                                 status,
                                 "Notify_sendEvent failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Pend here until we get a notification back from remote
             * processor
             */
            OsalSemaphore_pend (obj->semHandle, OSALSEMAPHORE_WAIT_FOREVER);

            key = GateMP_enter (obj->gate);

            /* Initialize status to not found. */
            status = NameServer_E_NOTFOUND;

            if (obj->cacheEnable) {
                Cache_inv (obj->msg [offset],
                           sizeof(NameServerRemoteNotify_Message),
                           Cache_Type_ALL,
                           TRUE);
            }

            if (obj->msg [offset]->requestStatus == TRUE) {
                obj->msg [offset]->requestStatus = FALSE;

                if (obj->msg[offset]->valueLen == sizeof (UInt32)) {
                    Memory_copy ((Ptr) value,
                                 (Ptr) &(obj->msg [offset]->value),
                                 sizeof (UInt32));
                }
                else {
                    Memory_copy ((Ptr) value,
                                 (Ptr) &(obj->msg [offset]->valueBuf),
                                 obj->msg[offset]->valueLen);
                }
                /* Set length to amount of data that was copied */
                *valueLen = obj->msg [offset]->valueLen;

                status = NameServer_S_SUCCESS;
            }

            obj->msg [offset]->request  = 0;
            obj->msg [offset]->response = 0;

            if (obj->cacheEnable) {
                Cache_wbInv(obj->msg[offset],
                            sizeof(NameServerRemoteNotify_Message),
                            Cache_Type_ALL, TRUE);
            }

            GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        IGateProvider_leave (obj->localGate, localKey);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServerRemoteNotify_get", status);

    /*! @retval NameServer_S_SUCCESS Found the specified name */
    return status;
}


/*
 * @brief     Get shared memory requirements
 *
 * @param     params     Instance creation parameters
 *
 * @sa        None
 */
SizeT
NameServerRemoteNotify_sharedMemReq (
                                const NameServerRemoteNotify_Params * params)
{
    SizeT totalSize;

    /* params is not used. */
    (Void) params;

    GT_1trace (curTrace, GT_ENTER, "NameServerRemoteNotify_sharedMemReq",
               params);
    /*
     *  Two Message structs are required.
     *  One for sending request and one for sending response.
     */
    if (MultiProc_getNumProcessors () > 1) {
        totalSize = (2 * sizeof (NameServerRemoteNotify_Message));
    }

    GT_1trace (curTrace,
               GT_LEAVE,
               "NameServerRemoteNotify_sharedMemReq",
               totalSize);

    /*! @retval Shared-memory-requirements Operation successfully completed. */
    return (totalSize);
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*!
 *  @brief      Function called when a Notify event is received.
 *
 *  @param      procId   Slave processor identifier.
 *  @param      lineId   Interrupt line ID.
 *  @param      eventId  Notify event number.
 *  @param      arg      arg specified in the registerEvent.
 *  @param      payload  Payload specified in the sendEvent
 */
Void
_NameServerRemoteNotify_callback (UInt16 procId,
                                  UInt16 lineId,
                                  UInt32 eventId,
                                  UArg   arg,
                                  UInt32 payload)
{
    Int                             status = NAMESERVERREMOTENOTIFY_SUCCESS;
    NameServerRemoteNotify_Obj * handle = (NameServerRemoteNotify_Obj *) arg;
    UInt16                          offset = 0;
    NameServer_Handle               nsHandle = NULL;
    IArg                            key;
    UInt32                          valueLen;

    GT_5trace (curTrace,
               GT_ENTER,
               "_NameServerRemoteNotify_callback",
               procId,
               lineId,
               eventId,
               arg,
               payload);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));
    GT_assert (curTrace, (arg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_NameServerRemoteNotify_callback",
                             NAMESERVERREMOTENOTIFY_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (procId >= MultiProc_getNumProcessors ()) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_NameServerRemoteNotify_callback",
                             status,
                             "ProcId is invalid!");
    }
    else if (arg == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_NameServerRemoteNotify_callback",
                             status,
                             "arg is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (procId == handle->remoteProcId));
        if ((MultiProc_self () > procId)) {
            offset = 1;
        }

        if (handle->cacheEnable) {
            /* Invalidate both request and response messages */
            Cache_inv (handle->msg [0],
                       sizeof(NameServerRemoteNotify_Message) << 1,
                       Cache_Type_ALL,
                       TRUE);
        }

        if (handle->msg [1 - offset]->request == TRUE) {
            /* This is a request */
            nsHandle = NameServer_getHandle ((String)
                                        handle->msg[1 - offset]->instanceName);
            valueLen = handle->msg[1 - offset]->valueLen;

            if (nsHandle != NULL) {
                if (valueLen == sizeof(UInt32)) {
                    status = NameServer_getLocalUInt32 (nsHandle,
                                         (String) handle->msg[1 - offset]->name,
                                         &handle->msg[1 - offset]->value);
                }
                else {
                    status = NameServer_getLocal(nsHandle,
                                         (String) handle->msg[1 - offset]->name,
                                         &handle->msg[1 - offset]->valueBuf,
                                         &valueLen);
                }
            }

            GT_assert (curTrace,
                       (    (status == NameServer_S_SUCCESS)
                        ||  (status == NameServer_E_NOTFOUND)));

            key = GateMP_enter (handle->gate);

            /* If status == NameServer_S_SUCCESS then an entry was found */
            if (status == NameServer_S_SUCCESS) {
                handle->msg [1 - offset]->requestStatus = TRUE;
                handle->msg [1 - offset]->valueLen = valueLen;
            }

            /* Send a response back */
            handle->msg [1 - offset]->response = TRUE;
            handle->msg [1 - offset]->request = FALSE;

            if (handle->cacheEnable) {
                Cache_wbInv (handle->msg[1 - offset],
                            sizeof (NameServerRemoteNotify_Message),
                            Cache_Type_ALL,
                            TRUE);
            }

            /* now we can leave the gate */
            GateMP_leave(handle->gate, key);

            /*
             *  The NotifyDriver handle must exist at this point,
             *  otherwise the Notify_sendEvent should have failed
             */
            GT_assert (curTrace, (eventId == NameServerRemoteNotify_state.cfg.notifyEventId));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                     Notify_sendEvent(handle->remoteProcId,
                                      0,
                                      NameServerRemoteNotify_notifyEventId,
                                      0,
                                      FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_NameServerRemoteNotify_callback",
                                     status,
                                     "Notify_sendEvent failed!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        if (handle->msg [offset]->response == TRUE) {
            OsalSemaphore_post (handle->semHandle);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "_NameServerRemoteNotify_callback");
}


/*
 *  ======== NameServerRemoteNotify_attach ========
 */
Int NameServerRemoteNotify_attach(UInt16 remoteProcId, Ptr sharedAddr)
{
    NameServerRemoteNotify_Params nsrParams;
    Int status = NameServer_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "NameServerRemoteNotify_attach", sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_attach",
                             status,
                             "Module was not setup!");
    }
    else if (sharedAddr == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_attach",
                             status,
                             "Argument sharedAddr is NULL!");
    }
    else if (GateMP_getDefaultRemote() == NULL) {
        status = NameServer_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_attach",
                             status,
                             "GateMP_getDefaultRemote failed!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Use default GateMP */
        NameServerRemoteNotify_Params_init(&nsrParams);
        nsrParams.gate = GateMP_getDefaultRemote();
        nsrParams.sharedAddr = sharedAddr;

        /* create only if notify driver has been created to remote proc */
        if (Notify_intLineRegistered (remoteProcId, 0)) {
            NameServerRemoteNotify_state.nsrHandles [remoteProcId] =
                            NameServerRemoteNotify_create (remoteProcId,
                                                          &nsrParams);
            if (NameServerRemoteNotify_state.nsrHandles [remoteProcId] == NULL){
                status = NameServer_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServerRemoteNotify_attach",
                                     status,
                                     "NameServerRemoteNotify_create failed!");
            }
        }
        else {
            status = NameServer_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerRemoteNotify_attach",
                                 status,
                                 "Notify driver is not registered!");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    return (status);
}


NameServerRemoteNotify_Handle
NameServerRemoteNotify_getHandle (UInt16 remoteProcId)
{
    NameServerRemoteNotify_Handle handle = NULL;
    GT_assert (curTrace, (remoteProcId < (MultiProc_getNumProcessors ())));

    handle = NameServerRemoteNotify_state.nsrHandles [remoteProcId];

    return(handle);
};


/*
 *  ======== NameServerRemoteNotify_detach ========
 */
Int NameServerRemoteNotify_detach(UInt16 remoteProcId)
{
    NameServerRemoteNotify_Handle handle;
    Int status = NameServer_S_SUCCESS;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServerRemoteNotify_state.refCount),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(0),
                                  NAMESERVERREMOTENOTIFY_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemoteNotify_detach",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = NameServerRemoteNotify_getHandle(remoteProcId);

        if (handle == NULL) {
            status = NameServer_E_FAIL;
        }
        else {
            NameServerRemoteNotify_delete(&handle);
            NameServerRemoteNotify_state.nsrHandles [remoteProcId] = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    return (status);
}
