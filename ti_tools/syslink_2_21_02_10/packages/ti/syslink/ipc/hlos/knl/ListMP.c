/*
 *  @file   ListMP.c
 *
 *  @brief      Defines for shared memory doubly linked list.
 *
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

/* Osal And Utils  headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/IGateProvider.h>

/* Module level headers */
#include <ti/ipc/NameServer.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/syslink/inc/_Ipc.h>


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @brief  Name of the reserved NameServer used for ListMP.
 */
#define ListMP_NAMESERVER  "ListMP"


/*! @brief Macro to make a correct module magic number with refCount */
#define ListMP_MAKE_MAGICSTAMP(x)   ((LISTMP_MODULEID << 12u) | (x))


/* =============================================================================
 * Structures and Enums
 * =============================================================================
 */
/*! @brief structure for ListMP module state */
typedef struct ListMP_ModuleObject_tag {
    Atomic              refCount;
    /*!< Reference count */
    NameServer_Handle   nameServer;
    /*!< Handle to the local NameServer used for storing ListMP
     *   objects
     */
    List_Object         objList;
    /*!< List holding created ListMP objects */
    IGateProvider_Handle localLock;
    /*!< Handle to lock for protecting objList */
    ListMP_Config       cfg;
    /*!< Current config values */
    ListMP_Config       defaultCfg;
    /*!< Default config values */
    ListMP_Params       defaultInstParams;
    /*!< Default instance creation parameters */
} ListMP_ModuleObject;

/*!
 *  @brief  Structure for the internal Handle for the ListMP.
 */
typedef struct ListMP_Object_tag {
    List_Elem                    listElem;
    /*!< Used for creating a linked list */
    volatile ListMP_Attrs *      attrs;
    /*!< Shared memory attributes */
    Ptr                          nsKey;
    /*!< NameServer key required for remove */
    GateMP_Handle                gate;
    /*!< Gate for critical regions  */
    UInt32                       allocSize;
    /*!< Shared memory allocated */
    UInt16                       regionId;
    /* SharedRegion ID               */
    Bool                         cacheEnabled;
    /* Whether to do cache calls     */
    ListMP_ProcAttrs             owner;
    /*!< Creator's attributes associated with an instance */
    ListMP_Params                params;
    /*!< the parameter structure */
    Ptr                          top;
    /*!< Pointer to the top Object */
} ListMP_Object;


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    ListMP_nameServer
 *
 *  @brief  Name of the reserved NameServer used for ListMP.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
ListMP_ModuleObject ListMP_state =
{
    .defaultCfg.maxRuntimeEntries     = 32u,
    .defaultCfg.maxNameLen            = 32u,
    .defaultInstParams.sharedAddr     = 0u,
    .defaultInstParams.name           = NULL,
    .defaultInstParams.gate           = NULL,
    .defaultInstParams.regionId       = 0u,
};

/*!
 *  @var    ListMP_module
 *
 *  @brief  Pointer to the ListMP module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
ListMP_ModuleObject * ListMP_module = &ListMP_state;


/* =============================================================================
 * Function definitions
 * =============================================================================
 */
/*!
 *  @brief      Creates a new instance of ListMP module.
 *              This is an internal function because both
 *              ListMP_create and ListMP_open
 *              call use the same functionality
 *
 *  @param      handlePtr OUT parameter: Pointer to handle.
 *  @param      params    Instance config-params structure.
 *  @param      createFlag Indicates if this is create or open.
 *
 *  @sa         ListMP_delete
 */
Int
_ListMP_create (ListMP_Handle * handlePtr,
                ListMP_Params * params,
                UInt32          createFlag);

/*
 *  @brief      Clear ListMP element
 *
 *  @param      elem  ListMP element
 *
 *  @sa         None
 */
Void ListMP_elemClear (ListMP_Elem *elem);


/* =============================================================================
 * Function API's
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the ListMP module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to ListMP_setup filled in by
 *              the ListMP module with the default parameters. If
 *              the user does not wish to make any change in the default
 *              parameters, this API is not required to be called.
 *
 *  @param      cfgParams  Pointer to the HeapBuf module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         ListMP_setup
 */
Void
ListMP_getConfig (ListMP_Config * cfgParams)
{
    GT_1trace (curTrace, GT_ENTER, "ListMP_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (cfgParams == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getConfig",
                             ListMP_E_INVALIDARG,
                        "Argument of type (ListMP_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_TRUE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                   ListMP_MAKE_MAGICSTAMP(0),
                                                   ListMP_MAKE_MAGICSTAMP(1))
                         == TRUE)) {
            /* If setup has not yet been called) */
            Memory_copy (cfgParams,
                         &ListMP_module->defaultCfg,
                         sizeof (ListMP_Config));
        }
        else {
            Memory_copy (cfgParams,
                         &ListMP_module->cfg,
                         sizeof (ListMP_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ListMP_getConfig");
}


/*!
 *  @brief      Setup the HeapBuf module.
 *
 *              This function sets up the ListMP module. This
 *              function must be called before any other instance-level APIs can
 *              be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then ListMP_getConfig can be called to
 *              get the configuration filled with the default values. After
 *              this, only the required configuration values can be changed. If
 *              the user does not wish to make any change in the default
 *              parameters, the application can simply call
 *              ListMP_setup with NULL parameters. The default
 *              parameters would get automatically used.
 *
 *  @param      cfg   Optional ListMP module configuration. If
 *                    provided as NULL, default configuration is used.
 *
 *  @sa         ListMP_destroy
 *              NameServer_Params_init
 *              NameServer_create
 */
Int
ListMP_setup (ListMP_Config * cfg)
{
    Int               status = ListMP_S_SUCCESS;
    Error_Block       eb;
    NameServer_Params params;
    ListMP_Config     tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "ListMP_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        ListMP_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (cfg == NULL) {
        ListMP_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&ListMP_module->refCount,
                            ListMP_MAKE_MAGICSTAMP(0),
                            ListMP_MAKE_MAGICSTAMP(0));

    if (EXPECT_FALSE (   Atomic_inc_return (&ListMP_module->refCount)
                      != ListMP_MAKE_MAGICSTAMP(1u))) {
        status = ListMP_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "ListMP Module already initialized!");
    }
    else {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
       if (EXPECT_FALSE (cfg->maxNameLen == 0)) {
            /*! @retval ListMP_E_INVALIDARG cfg->maxNameLen
             * passed is 0
             */
            status = ListMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ListMP_setup",
                                 status,
                                 "cfg->maxNameLen passed is 0!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Initialize the parameters */
            NameServer_Params_init (&params);
            params.maxValueLen = 4u;
            params.maxNameLen  = cfg->maxNameLen;

            /* Create the nameserver for modules */
            ListMP_module->nameServer = NameServer_create (ListMP_NAMESERVER,
                                                       &params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (ListMP_module->nameServer == NULL)) {
                /*! @retval ListMP_E_FAIL Failed to create the
                 * ListMP nameserver
                 */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                      "ListMP_setup",
                                     ListMP_E_FAIL,
                                     "Failed to create the ListMP "
                                     "name server!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Construct the list object */
                List_construct (&ListMP_module->objList, NULL);
                /* Copy the cfg */
                Memory_copy ((Ptr) &ListMP_module->cfg,
                             (Ptr) cfg,
                             sizeof (ListMP_Config));
                /* Create a lock for protecting list object */
                ListMP_module->localLock = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (ListMP_module->localLock == NULL)) {
                    status = NameServer_delete (&(ListMP_module->nameServer));
                    GT_assert (curTrace, (status >= 0));
                    /*! @retval ListMP_E_FAIL Failed to create the
                     * gate
                     */
                    status = ListMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                          "ListMP_setup",
                                         ListMP_E_FAIL,
                                         "Failed to create the gate!");
                    Atomic_set (&ListMP_module->refCount,
                                ListMP_MAKE_MAGICSTAMP(0));
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "ListMP_setup", status);

    /*! @retval ListMP_S_SUCCESS Operation Successful*/
    return status;
}


/*!
 *  @brief      Function to destroy the ListMP module.
 *
 *  @param      None
 *
 *  @sa         ListMP_setup
 *              NameServer_delete
 *              GateMutex_delete
 */
Int
ListMP_destroy (void)
{
    Int         status  = ListMP_S_SUCCESS;
    List_Elem * elem    = NULL;

    GT_0trace (curTrace, GT_ENTER, "ListMP_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        /*! @retval ListMP_E_INVALIDSTATE Module was
         *  not initialized
         */
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_TRUE (   Atomic_dec_return (&ListMP_module->refCount)
                         == ListMP_MAKE_MAGICSTAMP(0))) {
            /* Temporarily increment refCount here. */
            Atomic_set (&ListMP_module->refCount,
                        ListMP_MAKE_MAGICSTAMP(1));
            /* Check if any ListMP instances have not been deleted so
             * far. If not, delete them.
             */
            List_traverse (elem,
                           (List_Handle) &ListMP_module->objList) {
                if (   (   ((ListMP_Object *) elem)->owner.procId
                    == MultiProc_self ())) {
                    status =
                        ListMP_delete ((ListMP_Handle*)
                                     &(((ListMP_Object *) elem)->top));
                }
                else {
                    status =
                        ListMP_close ((ListMP_Handle *)
                                     &(((ListMP_Object *) elem)->top));
                }
            }
            /* Again reset refCount. */
            Atomic_set (&ListMP_module->refCount, ListMP_MAKE_MAGICSTAMP(0));

            if (EXPECT_TRUE (ListMP_module->nameServer != NULL)) {
                /* Delete the nameserver for modules */
                status = NameServer_delete (&(ListMP_module->nameServer));
                GT_assert (curTrace, (status >= 0));
                if (EXPECT_FALSE (status < 0)) {
                    /* Override the status to return a ListMP status code. */
                    status = ListMP_E_FAIL;
                }
            }

            /* Destruct the list object */
            List_destruct (&ListMP_module->objList);

            /* Delete the local lock */
            if (EXPECT_TRUE (ListMP_module->localLock != NULL)) {
                status = GateMutex_delete ((GateMutex_Handle *)
                                           &(ListMP_module->localLock));
                GT_assert (curTrace, (status >= 0));
                if (EXPECT_FALSE (status < 0)) {
                    /* Override the status to return a ListMP status code. */
                    status = ListMP_E_FAIL;
                }
            }

            Memory_set (&ListMP_module->cfg,
                        0,
                        sizeof (ListMP_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_destroy", status);

    /*! @retval ListMP_S_SUCCESS Operation Successful*/
    return status;
}

/* Initialize this config-params structure with supplier-specified
 * defaults before instance creation.
 */
Void
ListMP_Params_init (ListMP_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "ListMP_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_Params_init",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (params == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_Params_init",
                             ListMP_E_INVALIDARG,
                             "Argument of type "
                             "(ListMP_Params *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(ListMP_module->defaultInstParams),
                     sizeof (ListMP_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ListMP_Params_init");
}


/* Creates a new instance of ListMP module. */
ListMP_Handle
ListMP_create (const ListMP_Params * params)
{
    Int             status = ListMP_S_SUCCESS;
    ListMP_Object * obj  = NULL;
    IArg            key    = 0;
    ListMP_Params   sparams;

    GT_1trace (curTrace, GT_ENTER, "LISTMP_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_create",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (params == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_create",
                             ListMP_E_INVALIDARG,
                             "params passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy ((Ptr)&sparams,
                     (Ptr)params,
                     sizeof (ListMP_Params));

        key = IGateProvider_enter (ListMP_module->localLock);

        status = _ListMP_create ((ListMP_Handle *) &obj, &sparams, TRUE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (status < 0)){
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ListMP_create",
                                 status,
                                 "_ListMP_create failed!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        IGateProvider_leave (ListMP_module->localLock, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_create", obj);

    return ((ListMP_Handle) obj);
}


/* Deletes a instance of ListMP module. */
Int
ListMP_delete (ListMP_Handle * handlePtr)
{
    Int             status = ListMP_S_SUCCESS;
    ListMP_Object * obj    = NULL;
    ListMP_Params * params = NULL;
    IArg            key;

    GT_1trace (curTrace, GT_ENTER, "ListMP_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handlePtr == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_delete",
                             status,
                             "Pointer to handle passed is NULL!");
    }
    else if (EXPECT_FALSE (*handlePtr == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_delete",
                             status,
                             "Handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) (*handlePtr);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (obj->owner.procId != MultiProc_self ())) {
            status = ListMP_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ListMP_delete",
                                 status,
                                 "Instance was not created on this processor!");
        }
        else if (EXPECT_FALSE (obj->owner.openCount > 1)) {
            status = ListMP_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ListMP_delete",
                                 status,
                                 "Unmatched open/close calls!");
        }
        else if (EXPECT_FALSE (obj->owner.openCount != 1)) {
            status = ListMP_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ListMP_delete",
                                 status,
                                 "Opener of the instance has not closed the"
                                 "instance!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Remove from  the local list */
            key = IGateProvider_enter (ListMP_module->localLock);
            List_remove ((List_Handle) &ListMP_module->objList,
                         &obj->listElem);
            IGateProvider_leave (ListMP_module->localLock, key);

            params = (ListMP_Params *) &obj->params;
            if (EXPECT_TRUE (params->name != NULL)) {
                /* Free memory for the name */
                Memory_free (NULL,
                             params->name,
                             String_len (params->name) + 1u);
                /* Remove from the name server */
                if (obj->nsKey != NULL) {
                    NameServer_removeEntry (ListMP_module->nameServer,
                                            obj->nsKey);
                    obj->nsKey = NULL;
                }
            }

            /* Now free the obj */
            Memory_free (NULL, obj, sizeof (ListMP_Object));
            obj = NULL;
            *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_delete", status);

    return status;
}


/* Function to open an ListMP instance */
Int ListMP_open (String          name,
                 ListMP_Handle * handlePtr)
{
    Int         status = ListMP_S_SUCCESS;
    Ptr         sharedAddr = NULL;
    Bool        doneFlag = FALSE;
    IArg        key = 0;
    List_Elem * elem;
    SharedRegion_SRPtr  sharedShmBase;

    GT_2trace (curTrace, GT_ENTER, "ListMP_open", name, handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (name != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_open",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handlePtr == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_open",
                             status,
                             "handlePtr pointer passed is NULL!");
    }
    else if (EXPECT_FALSE (name == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_open",
                             status,
                             "name passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First check in the local list */
        List_traverse (elem, (List_Handle) &ListMP_module->objList) {
            if ((((ListMP_Object *)elem)->params.name != NULL)) {
                if (String_cmp (   ((ListMP_Object *)elem)->params.name, name)
                                == 0) {
                    key = IGateProvider_enter (ListMP_module->localLock);
                    if (   ((ListMP_Object *)elem)->owner.procId
                        == MultiProc_self ()) {
                        ((ListMP_Object *)elem)->owner.openCount++;
                    }
                    *handlePtr = (((ListMP_Object *)elem)->top);
                    IGateProvider_leave (ListMP_module->localLock, key);
                    doneFlag = TRUE;
                    break;
                }
            }
        }

        /* If not found locally, check in NameServer. */
        if (EXPECT_TRUE (doneFlag == FALSE)) {
            /* Find in name server */
            status = NameServer_getUInt32 (ListMP_module->nameServer,
                                           name,
                                           &sharedShmBase,
                                           NULL);
            /* This check must not be in SYSLINK_BUILD_OPTIMIZE, because it must
             * handle the case for the runtime error ListMP_E_NOTFOUND.
             */
            if (EXPECT_FALSE (status == NameServer_E_NOTFOUND)) {
                status = ListMP_E_NOTFOUND;
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            else if (EXPECT_FALSE (status < 0)) {
                /* Override the status to return a ListMP status code. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (
                                   curTrace,
                                   GT_4CLASS,
                                   "ListMP_open",
                                   status,
                                   "Failure in NameServer_get!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            else {
                /* Convert from shared region pointer to local address*/
                sharedAddr = SharedRegion_getPtr (sharedShmBase);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (sharedAddr == NULL)) {
                    status = ListMP_E_FAIL;
                    GT_setFailureReason (
                                       curTrace,
                                       GT_4CLASS,
                                       "ListMP_open",
                                       status,
                                       "Invalid pointer received from"
                                       "NameServer!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    status = ListMP_openByAddr (sharedAddr, handlePtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (status < 0)) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "ListMP_open",
                                             status,
                                             "Failure in ListMP_openByAddr!");
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_open", status);

    return status;
}


/* Function to open an ListMP instance */
Int ListMP_openByAddr (Ptr             sharedAddr,
                       ListMP_Handle * handlePtr)
{
    Int                status = ListMP_S_SUCCESS;
    Bool               doneFlag = FALSE;
    IArg               key;
    ListMP_Params      params;
    List_Elem *        elem;
    ListMP_Attrs *     attrs;
    UInt16             id;

    GT_2trace (curTrace,
               GT_ENTER,
               "ListMP_openByAddr",
               sharedAddr,
               handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_openByAddr",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handlePtr == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_openByAddr",
                             status,
                             "handlePtr pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First check in the local list */
        List_traverse (elem, (List_Handle) &ListMP_module->objList) {
            if (((ListMP_Object *)elem)->params.sharedAddr == sharedAddr) {
                key = IGateProvider_enter (ListMP_module->localLock);
                if (   ((ListMP_Object *)elem)->owner.procId
                    == MultiProc_self ()) {
                    ((ListMP_Object *)elem)->owner.openCount++;
                }
                IGateProvider_leave (ListMP_module->localLock, key);
                *handlePtr = (((ListMP_Object *)elem)->top);
                doneFlag = TRUE;
                break;
            }
        }

        /* If not already existing locally, create object locally for open. */
        if (EXPECT_FALSE (doneFlag == FALSE)) {
            /* Convert from shared region pointer to local address*/
            ListMP_Params_init (&params);
            params.sharedAddr = sharedAddr;

            attrs = (ListMP_Attrs *) sharedAddr;
            id = SharedRegion_getId (sharedAddr);

            if (EXPECT_FALSE (SharedRegion_isCacheEnabled (id))) {
                Cache_inv (attrs, sizeof (ListMP_Attrs), Cache_Type_ALL, TRUE);
            }

            if (EXPECT_FALSE (attrs->status != ListMP_CREATED)) {
                *handlePtr = NULL;
                status = ListMP_E_NOTFOUND;
                /* Don't set failure reason since this is an expected
                 * run-time failure.
                 */
                GT_1trace (curTrace,
                           GT_3CLASS,
                           "ListMP Instance is not created yet at the "
                           "provided shared addr.\n"
                           "    sharedAddr [0x%x]!",
                           sharedAddr);
            }
            else {
                key = IGateProvider_enter (ListMP_module->localLock);
                status = _ListMP_create (handlePtr, &params, FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (status < 0)) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ListMP_openByAddr",
                                         status,
                                         "_ListMP_create failed!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                IGateProvider_leave (ListMP_module->localLock, key);
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_openByAddr", status);

    return status;
}


/* Function to close a previously opened instance */
Int ListMP_close (ListMP_Handle * handlePtr)
{
    Int             status = ListMP_S_SUCCESS;
    ListMP_Object * obj    = NULL;
    ListMP_Params * params = NULL;
    IArg            key;

    GT_1trace (curTrace, GT_ENTER, "ListMP_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_close",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handlePtr == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_close",
                             status,
                             "The parameter handlePtr i.e. pointer to "
                             "handle passed is NULL!");
    }
    else if (EXPECT_FALSE (*handlePtr == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_close",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) (*handlePtr);
        if (EXPECT_TRUE (obj != NULL)) {
            params = (ListMP_Params *) &obj->params;

            key = IGateProvider_enter (ListMP_module->localLock);
            if (EXPECT_FALSE (obj->owner.procId == MultiProc_self ())) {
                (obj)->owner.openCount--;
            }
            /* Check if ListMP is opened on same processor*/
            if (EXPECT_TRUE (obj->owner.creator == FALSE)) {
                List_remove ((List_Handle) &ListMP_module->objList,
                             &obj->listElem);

                /* remove from the name server */
                if (EXPECT_TRUE (params->name != NULL)) {
                    /* Free memory for the name */
                    Memory_free (NULL,
                                 params->name,
                                 String_len (params->name) + 1u);
                }

                GateMP_close (&obj->gate);

                IGateProvider_leave (ListMP_module->localLock, key);

                /* Now free the obj */
                Memory_free (NULL, obj, sizeof (ListMP_Object));
                obj = NULL;
                *handlePtr = NULL;
            }
            else {
                IGateProvider_leave (ListMP_module->localLock, key);
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_close", status);

    return status;
}


/* Function to check if list is empty */
Bool
ListMP_empty (ListMP_Handle handle)
{
    Bool               flag = FALSE;
    ListMP_Object *    obj;
    IArg               key;
    SharedRegion_SRPtr sharedHead;

    GT_1trace (curTrace, GT_ENTER, "ListMP_empty", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_empty",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_empty",
                             ListMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) handle;

        key = GateMP_enter (obj->gate);

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* Invalidate the head to make sure we are not getting stale data */
            Cache_inv ((Ptr) &(obj->attrs->head),
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        sharedHead = SharedRegion_getSRPtr ((Ptr) &(obj->attrs->head),
                                            obj->regionId);

        if (obj->attrs->head.next == sharedHead) {
            flag = TRUE;
        }
        /* invalidate the head */
        if (obj->cacheEnabled) {
            Cache_inv((Ptr) &(obj->attrs->head),
                      sizeof(ListMP_Elem),
                      Cache_Type_ALL,
                      TRUE);
        }
        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_empty", flag);

    return (flag);
}


/* Retrieves the GateMP handle associated with the ListMP instance. */
GateMP_Handle
ListMP_getGate (ListMP_Handle handle)
{
    GateMP_Handle   gate = NULL;
    ListMP_Object * obj;

    GT_1trace (curTrace, GT_ENTER, "ListMP_getGate", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getGate",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getGate",
                             ListMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) handle;
        gate = obj->gate;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_getGate", gate);

    return (gate);
}


/* Function to get head element from list */
Ptr
ListMP_getHead (ListMP_Handle handle)
{
    ListMP_Elem   *    elem          = NULL;
    IArg               key;
    ListMP_Elem *      localHeadNext;
    ListMP_Elem *      localNext;
    ListMP_Object *    obj;
    Bool               localNextCache;

    GT_1trace (curTrace, GT_ENTER, "ListMP_getHead", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getHead",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getHead",
                             ListMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *)handle;

        key = GateMP_enter (obj->gate);

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* Invalidate the head to make sure we are not getting stale data */
            Cache_inv ((Ptr) &(obj->attrs->head),
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        localHeadNext = SharedRegion_getPtr ((SharedRegion_SRPtr)
                                                 (obj->attrs->head.next));
        GT_assert (curTrace, (localHeadNext != NULL));

        /* See if the ListMP_Object was empty */
        if (localHeadNext != (ListMP_Elem *) (&obj->attrs->head)) {
            /* Elem to return */
            elem = localHeadNext;
            GT_assert (curTrace, (elem != NULL));

            if (SharedRegion_isCacheEnabled(SharedRegion_getId(elem))) {
                /* Invalidate elem */
                Cache_inv (elem,
                           sizeof (ListMP_Elem),
                           Cache_Type_ALL,
                           TRUE);
            }

            localNext = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->next);
            GT_assert (curTrace, (localNext != NULL));

            localNextCache =
                SharedRegion_isCacheEnabled(SharedRegion_getId(localNext));
            if (localNextCache) {
                Cache_inv(localNext,
                          sizeof(ListMP_Elem),
                          Cache_Type_ALL,
                          TRUE);
            }

            /* Fix the head of the list next pointer */
            obj->attrs->head.next = elem->next;
            /* Fix the prev pointer of the new first elem on the list */
            localNext->prev = localHeadNext->prev;

            if (EXPECT_FALSE (obj->cacheEnabled)) {
                Cache_wbInv ((Ptr) &(obj->attrs->head),
                             sizeof (ListMP_Elem),
                             Cache_Type_ALL,
                             FALSE);
            }

            if (localNextCache) {
                Cache_wbInv ((Ptr) localNext,
                             sizeof (ListMP_Elem),
                             Cache_Type_ALL,
                             TRUE);
            }
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_getHead", elem);

    return elem;
}


/* Function to get tail element from list */
Ptr
ListMP_getTail (ListMP_Handle handle)
{
    ListMP_Elem   * elem = NULL;
    IArg            key;
    ListMP_Elem *   localHeadPrev;
    ListMP_Object * obj;
    ListMP_Elem *   localPrev;
    Bool            localPrevCache;

    GT_1trace (curTrace, GT_ENTER, "ListMP_getTail", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getTail",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_getTail",
                             ListMP_E_INVALIDARG,
                             "handle pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *)handle;

        key = GateMP_enter (obj->gate);

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* Invalidate the head to make sure we are not getting stale data */
            Cache_inv ((Ptr) &(obj->attrs->head),
                       sizeof(ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        localHeadPrev = SharedRegion_getPtr ((SharedRegion_SRPtr)
                                                (obj->attrs->head.prev));
        GT_assert (curTrace, (localHeadPrev != NULL));

        /* See if the ListMP_Object was not empty */
        if (localHeadPrev != (ListMP_Elem *) (&obj->attrs->head)) {
            /* Elem to return */
            elem = localHeadPrev;
            GT_assert (curTrace, (elem != NULL));

            if (SharedRegion_isCacheEnabled(SharedRegion_getId(elem))) {
                /* Invalidate elem */
                Cache_inv ((Ptr) elem,
                           sizeof (ListMP_Elem),
                           Cache_Type_ALL,
                           TRUE);
            }

            localPrev = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->prev);
            GT_assert (curTrace, (localPrev != NULL));

            localPrevCache =
                SharedRegion_isCacheEnabled(SharedRegion_getId(localPrev));
            if (localPrevCache) {
                Cache_inv(localPrev,
                          sizeof(ListMP_Elem),
                          Cache_Type_ALL,
                          TRUE);
            }

            /* Fix the head of the list prev pointer */
            obj->attrs->head.prev = elem->prev;
            /* Fix the next pointer of the new last elem on the list */
            localPrev->next = localHeadPrev->next;

            if (EXPECT_FALSE (obj->cacheEnabled)) {
                Cache_wbInv ((Ptr) &(obj->attrs->head),
                             sizeof(ListMP_Elem),
                             Cache_Type_ALL,
                             FALSE);
            }

            if (localPrevCache) {
                Cache_wbInv ((Ptr) localPrev,
                             sizeof (ListMP_Elem),
                             Cache_Type_ALL,
                             TRUE);
            }
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_getTail", elem);

    return elem;
}


/* Function to put head element into list */
Int
ListMP_putHead (ListMP_Handle     handle,
                ListMP_Elem     * elem)
{
    Int                status        = ListMP_S_SUCCESS;
    ListMP_Elem *      localNextElem = NULL;
    ListMP_Object *    obj;
    IArg               key;
    SharedRegion_SRPtr sharedElem;
    UInt16             index;
    Bool               localNextElemCache;

    GT_1trace (curTrace, GT_ENTER, "ListMP_putHead", handle);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (elem != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_putHead",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_putHead",
                             status,
                             "handle passed is NULL!");
    }
    else if (EXPECT_FALSE (elem == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_putHead",
                             status,
                             "elem passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) handle;

        /* Safe to do outside the gate */
        index = SharedRegion_getId (elem);
        sharedElem = SharedRegion_getSRPtr (elem, index);
        GT_assert (curTrace, (sharedElem != SharedRegion_INVALIDSRPTR));

        key = GateMP_enter (obj->gate);

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* Invalidate the head to make sure we are not getting stale data */
            Cache_inv ((Ptr) &(obj->attrs->head),
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        elem->next = obj->attrs->head.next;

        localNextElem = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->next);
        GT_assert (curTrace, (localNextElem != NULL));

        localNextElemCache =
            SharedRegion_isCacheEnabled(SharedRegion_getId(localNextElem));
        if (localNextElemCache) {
            Cache_inv (localNextElem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        elem->prev = localNextElem->prev;

        localNextElem->prev = sharedElem;
        obj->attrs->head.next = sharedElem;

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* Need to do cache operations */
            Cache_wbInv ((Ptr) &(obj->attrs->head),
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }

        if (localNextElemCache) {
            Cache_wbInv (localNextElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }

        if (SharedRegion_isCacheEnabled(SharedRegion_getId(elem))) {
            /* writeback invalidate only the elem structure */
            Cache_wbInv (elem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_putHead", status);

    return status;
}


/* Function to put tail element into list */
Int
ListMP_putTail (ListMP_Handle    handle,
                ListMP_Elem    * elem)
{
    Int                 status        = ListMP_S_SUCCESS;
    ListMP_Elem      *  localPrevElem = NULL;
    ListMP_Object *     obj;
    IArg                key;
    SharedRegion_SRPtr  sharedElem;
    UInt16              index;
    Bool                localPrevElemCache;

    GT_2trace (curTrace, GT_ENTER, "ListMP_putTail", handle, elem);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (elem   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_putTail",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_putTail",
                             status,
                             "handle passed is NULL!");
    }
    else if (EXPECT_FALSE (elem == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_putTail",
                             status,
                             "elem passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) handle;

        /* Safe to do outside the gate */
        index = SharedRegion_getId (elem);
        sharedElem = SharedRegion_getSRPtr (elem, index);
        GT_assert (curTrace, (sharedElem != SharedRegion_INVALIDSRPTR));

        key = GateMP_enter (obj->gate);

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* invalidate the head to make sure we are not getting stale data */
            Cache_inv ((Ptr) &(obj->attrs->head),
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        elem->prev = obj->attrs->head.prev;

        localPrevElem = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->prev);
        GT_assert (curTrace, (localPrevElem != NULL));

        localPrevElemCache =
            SharedRegion_isCacheEnabled(SharedRegion_getId(localPrevElem));
        if (localPrevElemCache) {
            Cache_inv (localPrevElem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        elem->next = localPrevElem->next;

        localPrevElem->next = sharedElem;
        obj->attrs->head.prev = sharedElem;

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* need to do cache operations */
            Cache_wbInv ((Ptr) &(obj->attrs->head),
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }

        if (localPrevElemCache) {
            Cache_wbInv (localPrevElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }

        if (SharedRegion_isCacheEnabled(SharedRegion_getId(elem))) {
            /* writeback invalidate only the elem structure */
            Cache_wbInv (elem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_putTail", status);

    return status;
}


/* Function to insert element into list */
Int
ListMP_insert (ListMP_Handle    handle,
               ListMP_Elem    * newElem,
               ListMP_Elem    * curElem)
{
    Int                   status        = ListMP_S_SUCCESS;
    ListMP_Elem *         localPrevElem = NULL;
    IArg                  key;
    SharedRegion_SRPtr    sharedNewElem;
    SharedRegion_SRPtr    sharedCurElem;
    UInt16                index;
    ListMP_Object       * obj;
    Bool                  curElemIsCached, localPrevElemIsCached;

    GT_3trace (curTrace, GT_ENTER, "ListMP_insert", handle, newElem, curElem);

    GT_assert (curTrace, (handle  != NULL));
    GT_assert (curTrace, (newElem != NULL));
    GT_assert (curTrace, (curElem != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_insert",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_insert",
                             status,
                             "handle passed is NULL!");
    }
    else if (EXPECT_FALSE (newElem == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_insert",
                             status,
                             "newElem passed is NULL!");
    }
    else if (EXPECT_FALSE (curElem == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_insert",
                             status,
                             "curElem passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *)handle;

        /* Get SRPtr for newElem */
        index = SharedRegion_getId (newElem);
        sharedNewElem = SharedRegion_getSRPtr (newElem, index);
        GT_assert (curTrace, (sharedNewElem != SharedRegion_INVALIDSRPTR));

        /* Get SRPtr for curElem */
        index = SharedRegion_getId (curElem);
        sharedCurElem = SharedRegion_getSRPtr (curElem, index);
        GT_assert (curTrace, (sharedCurElem != SharedRegion_INVALIDSRPTR));

        key = GateMP_enter (obj->gate);

        curElemIsCached =
            SharedRegion_isCacheEnabled(SharedRegion_getId(curElem));
        if (curElemIsCached) {
            Cache_inv (curElem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        /* Get pointer for curElem->prev */
        localPrevElem = SharedRegion_getPtr ((SharedRegion_SRPtr)
                                                        curElem->prev);
        GT_assert (curTrace, (localPrevElem != NULL));

        localPrevElemIsCached =
            SharedRegion_isCacheEnabled(SharedRegion_getId(localPrevElem));
        if (localPrevElemIsCached) {
            Cache_inv (localPrevElem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        newElem->next       = sharedCurElem;
        newElem->prev       = curElem->prev;
        localPrevElem->next = sharedNewElem;
        curElem->prev       = sharedNewElem;

        if (localPrevElemIsCached) {
            Cache_wbInv (localPrevElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }
        if (curElemIsCached) {
            Cache_wbInv (curElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }
        if (SharedRegion_isCacheEnabled(SharedRegion_getId(newElem))) {
            /* writeback invalidate only the elem structure  */
            Cache_wbInv (newElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_insert", status);

    return status;
}


/* Function to remove element from list. Element is not deleted. */
Int
ListMP_remove (ListMP_Handle   handle,
               ListMP_Elem   * elem)
{
    Int             status        = ListMP_S_SUCCESS;
    ListMP_Elem   * localPrevElem = NULL;
    ListMP_Elem   * localNextElem = NULL;
    ListMP_Object * obj;
    IArg            key;
    Bool            localPrevElemIsCached, localNextElemIsCached;

    GT_2trace (curTrace, GT_ENTER, "ListMP_remove", handle, elem);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (elem   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = ListMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_remove",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_remove",
                             status,
                             "handle passed is NULL!");
    }
    else if (EXPECT_FALSE (elem == NULL)) {
        status = ListMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_remove",
                             status,
                             "elem pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *)handle;

        key = GateMP_enter (obj->gate);

        localPrevElem = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->prev);
        localNextElem = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->next);

        localPrevElemIsCached =
            SharedRegion_isCacheEnabled(SharedRegion_getId(localPrevElem));
        localNextElemIsCached =
            SharedRegion_isCacheEnabled(SharedRegion_getId(localNextElem));
        if (localPrevElemIsCached) {
            Cache_inv (localPrevElem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }
        if (localNextElemIsCached) {
            Cache_inv (localNextElem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        localPrevElem->next = elem->next;
        localNextElem->prev = elem->prev;

        if (localPrevElemIsCached) {
            Cache_wbInv (localPrevElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         FALSE);
        }
        if (localNextElemIsCached) {
            Cache_wbInv (localNextElem,
                         sizeof (ListMP_Elem),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */


    GT_1trace (curTrace, GT_LEAVE, "ListMP_remove", status);

    return status ;
}


/* Function to traverse to next element in list
 * This function is not protected internally.
 */
Ptr
ListMP_next (ListMP_Handle   handle,
             ListMP_Elem   * elem)
{
    ListMP_Elem *   retElem = NULL;
    ListMP_Object * obj;
    Bool            elemIsCached;

    GT_2trace (curTrace, GT_ENTER, "ListMP_next", handle, elem);

    GT_assert (curTrace, (handle != NULL));
    /* elem can be NULL to return the first element in the list. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_next",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_next",
                             ListMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) handle;

        /* If element is NULL start at head */
        if (elem == NULL) {
            elem = (ListMP_Elem *) &(obj->attrs->head);
            elemIsCached = obj->cacheEnabled;
        }
        else {
            elemIsCached =
                SharedRegion_isCacheEnabled(SharedRegion_getId(elem));
        }

        if (elemIsCached) {
            Cache_inv (elem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        retElem = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->next);
        GT_assert (curTrace, (retElem != NULL));

        /* Return NULL if the last element in the list. */
        if (retElem == (ListMP_Elem *) (&obj->attrs->head)) {
            retElem = NULL;
        }

        /* invalidate the head */
        if (elemIsCached) {
            Cache_inv (elem,
                      sizeof (ListMP_Elem),
                      Cache_Type_ALL,
                      TRUE);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_next", retElem);

    return retElem;
}


/* Function to traverse to prev element in list
 * This function is not protected internally.
 */
Ptr
ListMP_prev (ListMP_Handle  handle,
             ListMP_Elem  * elem)
{
    ListMP_Elem *   retElem  = NULL;
    ListMP_Object * obj;
    Bool            elemIsCached;

    GT_2trace (curTrace, GT_ENTER, "ListMP_prev", handle, elem);

    GT_assert (curTrace, (handle != NULL));
    /* elem can be NULL to return the first element in the list. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(ListMP_module->refCount),
                                                ListMP_MAKE_MAGICSTAMP(0),
                                                ListMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_prev",
                             ListMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_prev",
                             ListMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (ListMP_Object *) handle;

        /* If element is NULL start at head */
        if (elem == NULL) {
            elem = (ListMP_Elem *) &(obj->attrs->head);
            elemIsCached = obj->cacheEnabled;
        }
        else {
            elemIsCached =
                SharedRegion_isCacheEnabled(SharedRegion_getId(elem));
        }

        if (elemIsCached) {
            Cache_inv (elem,
                       sizeof (ListMP_Elem),
                       Cache_Type_ALL,
                       TRUE);
        }

        retElem = SharedRegion_getPtr ((SharedRegion_SRPtr) elem->prev);
        GT_assert (curTrace, (retElem != NULL));

        /* Return NULL if the last element in the list. */
        if (retElem == (ListMP_Elem *) (&obj->attrs->head)) {
            retElem = NULL;
        }

        /* invalidate the head */
        if (elemIsCached) {
            Cache_inv (elem,
                      sizeof (ListMP_Elem),
                      Cache_Type_ALL,
                      TRUE);
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_prev", retElem);

    return (retElem);
}


/* Amount of shared memory required for creation of each instance. */
SizeT
ListMP_sharedMemReq (const ListMP_Params * params)
{
    SizeT  memReq  = 0;
    SizeT  minAlign;
    UInt16 regionId;

    GT_1trace (curTrace, GT_ENTER, "ListMP_sharedMemReq",params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (params == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMP_sharedMemReq",
                             ListMP_E_INVALIDARG,
                             "params pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (params->sharedAddr == NULL) {
            regionId = params->regionId;
        }
        else {
            regionId = SharedRegion_getId (params->sharedAddr);
        }
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        minAlign = Memory_getMaxDefaultTypeAlign ();
        if (SharedRegion_getCacheLineSize (regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize (regionId);
        }

        memReq = _Ipc_roundup (sizeof (ListMP_Attrs), minAlign);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMP_sharedMemReq", memReq);

    return memReq;
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/* Creates a new instance of ListMP module.
 * This is an internal function because both
 * ListMP_create and ListMP_open
 * call use the same functionality.
 */
Int
_ListMP_create (ListMP_Handle * handlePtr,
                ListMP_Params * params,
                UInt32          createFlag)
{
    Int                         status = ListMP_S_SUCCESS;
    ListMP_Object *             obj = NULL;
    Ptr                         localAddr  = NULL;
    SharedRegion_SRPtr          sharedShmBase;
    ListMP_Params               sparams;
    UInt16                      nameLen;
    IHeap_Handle                regionHeap;

    GT_3trace (curTrace, GT_ENTER, "_ListMP_create",
               handlePtr, params, createFlag);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (params != NULL));

    /* Create the obj */
    obj = (ListMP_Object *) Memory_calloc (NULL,
                                           sizeof (ListMP_Object),
                                           0u,
                                           NULL);
    *handlePtr = (ListMP_Handle) obj;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (obj == NULL)) {
        /*! @retval ListMP_E_MEMORY if Memory allocation failed for obj
         *          of type ListMP_Object
         */
        status = ListMP_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_ListMP_create",
                             status,
                             "Memory allocation failed for handle"
                             "of type ListMP_Object");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Populate the params member */
        Memory_copy ((Ptr)&obj->params,
                     (Ptr)params,
                     sizeof (ListMP_Params));

        if (createFlag == FALSE) {
            /* Update attrs */
            obj->attrs = (ListMP_Attrs *) params->sharedAddr;
            obj->regionId = SharedRegion_getId((Ptr)&(obj->attrs->head));
            obj->cacheEnabled = SharedRegion_isCacheEnabled (obj->regionId);


            /* get the local address of the SRPtr */
            localAddr = SharedRegion_getPtr(obj->attrs->gateMPAddr);

            status = GateMP_openByAddr(localAddr, &(obj->gate));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (status < 0)) {
                /* Override the status to return a ListMP status. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_ListMP_create",
                                     status,
                                     "GateMP_openByAddr failed!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else {
            List_elemClear (&obj->listElem);

            /* init the gate */
            if (params->gate != NULL) {
                obj->gate = params->gate;
            }
            else {
                obj->gate = GateMP_getDefaultRemote ();
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (obj->gate == NULL)) {
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_ListMP_create",
                                     status,
                                     "GateMP is NULL!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (params->sharedAddr == NULL) {
                    /* Creating using a shared region ID and name */
                    obj->regionId = params->regionId;
                    obj->cacheEnabled = SharedRegion_isCacheEnabled (
                                                                obj->regionId);

                    /* Need to allocate from the heap */
                    ListMP_Params_init(&sparams);
                    sparams.regionId = params->regionId;
                    obj->allocSize = ListMP_sharedMemReq (&sparams);

                    regionHeap = SharedRegion_getHeap (obj->regionId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (regionHeap == NULL)) {
                        status = ListMP_E_FAIL;
                        /*! @retval ListMP_E_FAIL SharedRegion_getHeap failed */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_ListMP_create",
                                             status,
                                             "SharedRegion_getHeap failed!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* HeapMemMP will do the alignment * */
                        obj->attrs = Memory_alloc (regionHeap,
                                                   obj->allocSize,
                                                   0,
                                                   NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (EXPECT_FALSE (obj->attrs == NULL)) {
                            status = ListMP_E_MEMORY;
                            /*! @retval ListMP_E_MEMORY Failed to allocate
                                            shared memory for ListMP instance */
                            GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_ListMP_create",
                                             status,
                                             "Failed to allocate shared memory"
                                             " for ListMP instance!");
                        }
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
                else {
                    /* Creating using sharedAddr */
                    obj->regionId = SharedRegion_getId (params->sharedAddr);

                    /* Assert that the buffer is in a valid shared
                     * region
                     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (   obj->regionId
                                      == SharedRegion_INVALIDREGIONID))  {
                        status = ListMP_E_FAIL;
                        /*! @retval ListMP_E_FAIL params->sharedAddr is not in a
                                                  valid SharedRegion. */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_ListMP_create",
                                             status,
                                             "params->sharedAddr is not in a"
                                             " valid SharedRegion!");
                    }
                    else if (EXPECT_FALSE (  ((UInt32)params->sharedAddr
                              % SharedRegion_getCacheLineSize (obj->regionId)
                              != 0))) {
                        status = ListMP_E_FAIL;
                        /*! @retval ListMP_E_FAIL params->sharedAddr does not
                                        meet cache alignment constraints */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_ListMP_create",
                                             status,
                                             "params->sharedAddr does not meet"
                                             " cache alignment constraints!");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* set object's cacheEnabled */
                        obj->cacheEnabled = SharedRegion_isCacheEnabled (
                                                              obj->regionId);
                        obj->attrs = (ListMP_Attrs *) params->sharedAddr;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_TRUE (status >= 0)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* init the head (to be empty) */
                ListMP_elemClear((ListMP_Elem *)&(obj->attrs->head));

                /* store the GateMP sharedAddr in the Attrs */
                obj->attrs->gateMPAddr = GateMP_getSharedAddr (obj->gate);

                /* last thing, set the status */
                obj->attrs->status = ListMP_CREATED;

                if (EXPECT_FALSE (obj->cacheEnabled)) {
                    Cache_wbInv ((Ptr) obj->attrs,
                                 sizeof (ListMP_Attrs),
                                 Cache_Type_ALL,
                                 TRUE);
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            if ((status >= 0) && (obj->params.name != NULL)) {
                nameLen = String_len (params->name) + 1u;
                /* Copy the name */
                obj->params.name = (String)
                                        Memory_alloc (NULL, nameLen, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (obj->params.name == NULL)) {
                    /*! @retval ListMP_E_MEMORY Memory allocation failed for
                                                 name */
                    status = ListMP_E_MEMORY;
                    GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "_ListMP_create",
                            status,
                            "Memory allocation failed for name!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    String_ncpy (obj->params.name, params->name, nameLen);
                    /* Add to NameServer */
                    sharedShmBase = SharedRegion_getSRPtr ((Ptr) obj->attrs,
                                                           obj->regionId);
                    GT_assert (curTrace,
                               (sharedShmBase != SharedRegion_INVALIDSRPTR));
                    obj->nsKey = NameServer_addUInt32 (ListMP_module->nameServer,
                                                       params->name,
                                                       (UInt32) sharedShmBase);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (obj->nsKey == NULL)) {
                        /*! @retval NULL Failed to add to NameServer */
                        status = ListMP_E_ALREADYEXISTS;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_ListMP_create",
                                             status,
                                             "Failed to add to NameServer!");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_TRUE (status >= 0)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Update owner and opener details */
            if (createFlag == TRUE) {
                obj->owner.creator     = TRUE;
                obj->owner.openCount   = 1u;
                obj->owner.procId      = MultiProc_self ();
            }
            else {
                obj->owner.creator     = FALSE;
                obj->owner.openCount   = 0u;
                obj->owner.procId      = MultiProc_INVALIDID;
            }

            obj->top = obj;

            /* Put in the local list. No locking here, since it's done
             * outside in create & open.
             */
            List_put ((List_Handle) &ListMP_module->objList, &obj->listElem);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }

    if (EXPECT_FALSE (status < 0)) {
        if (obj != NULL) {
            if (createFlag == TRUE) {
                if (obj->attrs != NULL) {
                    obj->attrs->status = 0x0;
                    if (EXPECT_FALSE (obj->cacheEnabled)) {
                        Cache_wbInv ((Ptr) obj->attrs,
                                     sizeof (ListMP_Attrs),
                                     Cache_Type_ALL,
                                     TRUE);
                    }
                }

                /* Remove from  the local list. No locking here, since it's done
                 * outside in create & open.
                 */
                List_remove ((List_Handle) &ListMP_module->objList,
                             &obj->listElem);

                if (obj->params.name != NULL) {
                    if (obj->nsKey != NULL) {
                        NameServer_removeEntry (ListMP_module->nameServer,
                                                obj->nsKey);
                        obj->nsKey = NULL;
                    }
                    Memory_free (NULL,
                                 obj->params.name,
                                 String_len (obj->params.name) + 1u);
                }

                Memory_free (NULL, obj, sizeof (ListMP_Object));
                obj = NULL;
                *handlePtr = NULL;
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_ListMP_create", status);

    /*! @retval Valid-Handle Operation Successful*/
    return (status);
}


/* Clears a ListMP element's pointers */
Void
ListMP_elemClear (ListMP_Elem * elem)
{
    SharedRegion_SRPtr sharedElem;
    Int                id;

    GT_assert (curTrace, (elem != NULL));

    id         = SharedRegion_getId (elem);
    sharedElem = SharedRegion_getSRPtr (elem, id);

    elem->next = elem->prev = sharedElem;

    if (EXPECT_FALSE (SharedRegion_isCacheEnabled (id))) {
        Cache_wbInv (elem,
                     sizeof(ListMP_Elem),
                     Cache_Type_ALL,
                     TRUE);
    }
}
