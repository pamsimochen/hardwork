/*
 *  @file   HeapMemMP.c
 *
 *  @brief      Defines HeapMemMP based memory allocator.
 *
 *  HeapMemMP is a heap implementation that manages variable size buffers that
 *  can be used in a multiprocessor system with shared memory. HeapMemMP
 *  manages a single buffer in shared memory from which blocks of user-
 *  specified length are allocated and freed.
 *
 *  The HeapMemMP module uses a NameServerinstance to
 *  store instance information when an instance is created.  The name supplied
 *  must be unique for all HeapMemMP instances.
 *
 *  The create initializes the shared memory as needed. Once an
 *  instance is created, an open can be performed. The
 *  open is used to gain access to the same HeapMemMP instance.
 *  Generally an instance is created on one processor and opened on the
 *  other processor(s).
 *
 *  The open returns a HeapMemMP instance handle like the create,
 *  however the open does not modify the shared memory.
 *
 *  Because HeapMemMP is a variable-size heap implementation, it is also used
 *  by Shared Region to manage shared memory in each shared
 *  region.  When any shared memory IPC instance is created in a
 *  particular shared region, the HeapMemMP that manages shared memory in the
 *  shared region allocates shared memory for the instance.
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

/* Utilities headers */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/HeapMemMP.h>

#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/_Ipc.h>

/* Module level headers */
#include <ti/syslink/inc/_HeapMemMP.h>


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    HeapMemMP_nameServer
 *
 *  @brief  Name of the reserved NameServer used for HeapMemMP.
 */
#define HeapMemMP_NAMESERVER  "HeapMemMP"

/*! @brief Macro to make a correct module magic number with refCount */
#define HeapMemMP_MAKE_MAGICSTAMP(x) ((HeapMemMP_MODULEID << 12u) | (x))



/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Structure defining processor related information for the
 *          module.
 */
typedef struct HeapMemMP_ProcAttrs_tag {
    Bool   creator;   /*!< Creatoror opener */
    UInt16 procId;    /*!< Processor Identifier */
    UInt32 openCount; /*!< How many times it is opened on a processor */
} HeapMemMP_ProcAttrs;


/* Header */
typedef struct HeapMemMP_Header_tag {
    SharedRegion_SRPtr next;
    /* SRPtr to next header (Header *)    */
    Bits32             size;
    /* Size of this segment (Memory.size) */
} HeapMemMP_Header;

/*! Structure of attributes in shared memory */
typedef struct HeapMemMP_Attrs_tag {
    Bits32              status;
    /* Module status                 */
    SharedRegion_SRPtr  bufPtr;
    /* Memory managed by instance    */
    HeapMemMP_Header    head;
    /* HeapMemMP      head */
    SharedRegion_SRPtr  gateMPAddr;
    /* GateMP SRPtr (shm safe)       */
} HeapMemMP_Attrs;

/*!
 *  Structure for HeapMemMP module state
 */
typedef struct HeapMemMP_ModuleObject {
    Atomic                 refCount;
    /*!< Reference count */
    NameServer_Handle      nameServer;
    /*!< Handle to the local NameServer used for storing GP objects */
    List_Object            objList;
    /*!< List holding created objects */
    IGateProvider_Handle   localLock;
    /*!< Handle to lock for protecting objList */
    HeapMemMP_Config       cfg;
    /*!< Current config values */
    HeapMemMP_Config       defaultCfg;
    /*!< Default config values */
    HeapMemMP_Params       defaultInstParams;
    /*!< Default instance creation parameters */
} HeapMemMP_ModuleObject;


/*!
 *  @brief  Structure for the Handle for the HeapMemMP.
 */
typedef struct HeapMemMP_Obj_tag {
    List_Elem           listElem;
    /* Used for creating a linked list */
    volatile HeapMemMP_Attrs   * attrs;
    /* Local pointer to attrs        */
    GateMP_Handle       gate;
    /* Gate for critical regions     */
    Ptr                 nsKey;
    /* Used to remove NS entry       */
    Bool                cacheEnabled;
    /* Whether to do cache calls     */
    UInt16              regionId;
    /* SharedRegion index            */
    UInt32              allocSize;
    /* Shared memory allocated       */
    Char              * buf;
    /* Local pointer to buf          */
    UInt32              minAlign;
    /* Minimum alignment required    */
    UInt32              bufSize;
    /* Buffer Size */
    HeapMemMP_ProcAttrs owner;
    /* Processor related information for owner processor */
    Ptr                 top;
    /* Pointer to the top Object */
    HeapMemMP_Params    params;
    /* instance creation parameters */
} HeapMemMP_Obj;


/* =============================================================================
 * Globals
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
HeapMemMP_ModuleObject HeapMemMP_state =
{
    .defaultCfg.maxNameLen            = 32u,
    .defaultCfg.maxRunTimeEntries     = 32u,
    .defaultInstParams.gate           = NULL,
    .defaultInstParams.name           = NULL,
    .defaultInstParams.regionId       = 0u,
    .defaultInstParams.sharedAddr     = NULL,
    .defaultInstParams.sharedBufSize  = 0,
};

/*!
 *  @var    HeapMemMP_module
 *
 *  @brief  Pointer to the HeapMemMP module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
HeapMemMP_ModuleObject * HeapMemMP_module = &HeapMemMP_state;


/* =============================================================================
 * Forward declaration of internal function
 * =============================================================================
 */
/*!
 *  @brief      Creates a new instance of HeapMemMP module.
 *              This is an internal function as both HeapMemMP_create
 *              and HeapMemMP_open use the functionality.
 *
 *  @param      handlePtr  Return value: Handle
 *  @param      params  Instance config-params structure.
 *  @param      createFlag Indicates whether this is a create or open call.
 *
 *  @sa         HeapMemMP_delete,
 *              ListMP_Params_init
 *              ListMP_sharedMemReq
 *              Gate_enter
 *              Gate_leave
 *              GateMutex_delete
 *              NameServer_addUInt32
 */
Int _HeapMemMP_create (      HeapMemMP_Handle * handlePtr,
                       const HeapMemMP_Params * params,
                             Bool               createFlag);

/* slice and dice the buffer */
Int HeapMemMP_postInit (HeapMemMP_Object * handle);


/* =============================================================================
 * APIS
 * =============================================================================
 */
/* Get the default configuration for the HeapMemMP module. */
Void
HeapMemMP_getConfig (HeapMemMP_Config * cfgParams)
{
    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getConfig",
                             HeapMemMP_E_INVALIDARG,
                             "Argument of type (HeapMemMP_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                      HeapMemMP_MAKE_MAGICSTAMP(0),
                                      HeapMemMP_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfgParams,
                         &HeapMemMP_module->defaultCfg,
                         sizeof (HeapMemMP_Config));
        }
        else {
            Memory_copy (cfgParams,
                         &HeapMemMP_module->cfg,
                         sizeof (HeapMemMP_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "HeapMemMP_getConfig");
}


/* Setup the HeapMemMP module. */
Int
HeapMemMP_setup (const HeapMemMP_Config * cfg)
{
    Int               status = HeapMemMP_S_SUCCESS;
    Error_Block       eb;
    HeapMemMP_Config  tmpCfg;
    NameServer_Params params;

    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        HeapMemMP_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (cfg == NULL) {
        HeapMemMP_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&HeapMemMP_module->refCount,
                            HeapMemMP_MAKE_MAGICSTAMP(0),
                            HeapMemMP_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&HeapMemMP_module->refCount)
        != HeapMemMP_MAKE_MAGICSTAMP(1u)) {
        status = HeapMemMP_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "HeapMemMP Module already initialized!");
    }
    else {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (cfg->maxNameLen == 0) {
            /*! @retval HeapMemMP_E_INVALIDARG cfg->maxNameLen passed is 0 */
            status = HeapMemMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMemMP_setup",
                                 status,
                                 "cfg->maxNameLen passed is 0!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
             /* Initialize the parameters */
             NameServer_Params_init (&params);
             params.maxValueLen = sizeof (UInt32);
             params.maxNameLen  = cfg->maxNameLen;

             /* Create the nameserver for modules */
             HeapMemMP_module->nameServer = NameServer_create (
                                                        HeapMemMP_NAMESERVER,
                                                        &params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
             if (HeapMemMP_module->nameServer == NULL) {
                 /*! @retval HeapMemMP_E_FAIL Failed to create the HeapMemMP
                  *          name server
                  */
                 status = HeapMemMP_E_FAIL;
                 GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "HeapMemMP_setup",
                                      HeapMemMP_E_FAIL,
                                      "Failed to create the HeapMemMP "
                                      "nameserver!");
             }
            if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Construct the list object */
                List_construct (&HeapMemMP_module->objList, NULL);
                /* Copy the cfg */
                Memory_copy ((Ptr) &HeapMemMP_module->cfg,
                             (Ptr) cfg,
                             sizeof (HeapMemMP_Config));
                /* Create a lock for protecting list object */
                HeapMemMP_module->localLock = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (HeapMemMP_module->localLock == NULL) {
                    /*! @retval HeapMemMP_E_FAIL Failed to create the localLock*/
                    status = HeapMemMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapMemMP_setup",
                                         HeapMemMP_E_FAIL,
                                         "Failed to create the localLock!");
                    HeapMemMP_destroy ();
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_setup", status);

    /*! @retval HeapMemMP_S_SUCCESS Operation successful */
    return status;
}


/* Function to destroy the HeapMemMP module. */
Int
HeapMemMP_destroy (void)
{
    Int           status = HeapMemMP_S_SUCCESS;
    List_Elem  *  elem;

    GT_0trace (curTrace, GT_ENTER, "HeapMemMP_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        /*! @retval HeapMemMP_E_INVALIDSTATE Module was not initialized */
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&HeapMemMP_module->refCount)
            == HeapMemMP_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&HeapMemMP_module->refCount,
                        HeapMemMP_MAKE_MAGICSTAMP(1));
            /* Check if any HeapMemMP instances have not been deleted so far.
             * If not, delete them.
             */
            List_traverse (elem, (List_Handle) &HeapMemMP_module->objList) {
                if (   (   ((HeapMemMP_Obj *) elem)->owner.procId
                    == MultiProc_self ())) {
                    status = HeapMemMP_delete ((HeapMemMP_Handle*)
                                             &(((HeapMemMP_Obj *) elem)->top));
                }
                else {
                    status = HeapMemMP_close ((HeapMemMP_Handle *)
                                              &(((HeapMemMP_Obj *) elem)->top));
                }
            }

            /* Decrease the refCount */
            Atomic_set (&HeapMemMP_module->refCount,
                        HeapMemMP_MAKE_MAGICSTAMP(0));

            if (EXPECT_TRUE (HeapMemMP_module->nameServer != NULL)) {
                /* Delete the nameserver for modules */
                status = NameServer_delete (&HeapMemMP_module->nameServer);
                GT_assert (curTrace, (status >= 0));
                if (status < 0) {
                    /* Override the status to return a HeapMemMP status code. */
                    status = HeapMemMP_E_FAIL;
                }
            }

            /* Destruct the list object */
            List_destruct (&HeapMemMP_module->objList);

            /* Delete the list lock */
            if (HeapMemMP_module->localLock != NULL) {
                status = GateMutex_delete ((GateMutex_Handle *)
                                           &HeapMemMP_module->localLock);
                GT_assert (curTrace, (status >= 0));
                if (status < 0) {
                    /* Override the status to return a HeapMemMP status code. */
                    status = HeapMemMP_E_FAIL;
                }
            }

            Memory_set (&HeapMemMP_module->cfg, 0, sizeof (HeapMemMP_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_destroy", status);

    /*! @retval HeapMemMP_S_SUCCESS Operation successful */
    return status;
}


/* Initialize this config-params structure with supplier-specified
 * defaults before instance creation.
 */
Void
HeapMemMP_Params_init (HeapMemMP_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_Params_init",
                             HeapMemMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_Params_init",
                             HeapMemMP_E_INVALIDARG,
                             "Argument of type (HeapMemMP_Params *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(HeapMemMP_module->defaultInstParams),
                     sizeof (HeapMemMP_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMP_Params_init");
}


/*!
 *  @brief      Creates a new instance of HeapMemMP module.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         HeapMemMP_delete,
 *              ListMP_Params_init
 *              ListMP_sharedMemReq
 *              Gate_enter
 *              Gate_leave
 *              NameServer_addUInt32
 */
HeapMemMP_Handle
HeapMemMP_create (const HeapMemMP_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                status = HeapMemMP_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    HeapMemMP_Object * handle = NULL;
    HeapMemMP_Params   sparams;

    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        /*! @retval NULL if Module was not initialized */
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_create",
                             status,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        /*! @retval NULL if params pointer passed is NULL */
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_create",
                             status,
                             "params pointer passed is NULL!");
    }
    else if (params->sharedBufSize == 0) {
        /*! @retval HeapMemMP_E_INVALIDARG if Shared Buffer size cannot be 0
         */
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_create",
                             status,
                             "Shared buffer size is 0!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (&sparams,
                     (Ptr)params,
                     sizeof (HeapMemMP_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        _HeapMemMP_create ((HeapMemMP_Handle *) &handle, &sparams, TRUE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_create",
                             status,
                             "_HeapMemMP_create failed in HeapMemMP_create!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_create", handle);

    /*! @retval Valid-Handle Operation successful*/
    return (HeapMemMP_Handle) handle;
}


/* Deletes an instance of HeapMemMP module. */
Int
HeapMemMP_delete (HeapMemMP_Handle * handlePtr)
{
    Int                status = HeapMemMP_S_SUCCESS;
    HeapMemMP_Object * handle = NULL;
    HeapMemMP_Obj    * obj    = NULL;
    HeapMemMP_Params * params = NULL;
    HeapMemMP_Handle * regionHeap = NULL;
    IArg               key    = 0;
    IArg               key1   = 0;

    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_delete",
                             status,
                             "The parameter handlePtr i.e. pointer to handle "
                             "passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_delete",
                             status,
                             "Handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (HeapMemMP_Object *) (*handlePtr);

        obj    = (HeapMemMP_Obj *) handle->obj;

        if (obj != NULL) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            /* Check if we have created the HeapMemMP or not */
            if (obj->owner.procId != MultiProc_self ()) {
                status = HeapMemMP_E_INVALIDSTATE;
                GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMemMP_delete",
                                 status,
                                 "Instance was not created on this processor!");
             }
             else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Take the local lock */
                key = GateMP_enter (obj->gate);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (obj->owner.openCount > 1) {
                    status = HeapMemMP_E_INVALIDSTATE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapMemMP_delete",
                                         status,
                                         "Unmatched open/close calls!");
                    /* Release the global lock */
                    GateMP_leave (obj->gate, key);
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Remove from  the local list */
                    key1 = IGateProvider_enter (HeapMemMP_module->localLock);
                    List_remove ((List_Handle) &HeapMemMP_module->objList,
                                 &obj->listElem);
                    IGateProvider_leave (HeapMemMP_module->localLock, key1);

                    params = (HeapMemMP_Params *) &obj->params;

                    if (EXPECT_TRUE(params->name != NULL)) {
                        if (EXPECT_TRUE(obj->nsKey != NULL)) {
                            NameServer_removeEntry (
                                                HeapMemMP_module->nameServer,
                                                obj->nsKey);
                            obj->nsKey = NULL;
                        }
                        Memory_free (NULL,
                                     obj->params.name,
                                     String_len (obj->params.name) + 1u);
                    }

                    /* Set status to 'not created' */
                    if (obj->attrs != NULL) {
                        obj->attrs->status = 0;
                        if (obj->cacheEnabled) {
                            Cache_wbInv ((Ptr) obj->attrs,
                                         sizeof (HeapMemMP_Attrs),
                                         Cache_Type_ALL,
                                         TRUE);
                        }
                    }

                    /* Release the shared lock */
                    GateMP_leave (obj->gate, key);

                    /* If necessary, free shared memory  if memory is internally
                     * allocated
                     */
                    regionHeap = SharedRegion_getHeap (obj->regionId);

                    if (   (regionHeap != NULL)
                        && (obj->params.sharedAddr == NULL)
                        && (obj->attrs != NULL)){
                        Memory_free ((IHeap_Handle) regionHeap,
                                     (Ptr) obj->attrs,
                                     obj->allocSize);
                    }

                    /* Now free the handle */
                    Memory_free (NULL, obj, sizeof (HeapMemMP_Obj));
                    Memory_free (NULL, handle, sizeof (HeapMemMP_Object));
                    *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else {
            Memory_free (NULL, handle, sizeof (HeapMemMP_Object));
            *handlePtr = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_delete", status);

    return status;
}


/*  Opens a created instance of HeapMemMP module. */
Int
HeapMemMP_open (String             name,
                HeapMemMP_Handle * handlePtr)
{
    Int                  status = HeapMemMP_S_SUCCESS;
    SharedRegion_SRPtr   sharedShmBase = SharedRegion_INVALIDSRPTR;
    Ptr                  sharedAddr    = NULL;
    IArg                 key    = 0;
    Bool                 doneFlag = FALSE;
    List_Elem      *     elem     = NULL;

    GT_2trace (curTrace, GT_ENTER, "HeapMemMP_open", name, handlePtr);

    GT_assert (curTrace, (name != NULL));
    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_open",
                             status,
                             "Module was not initialized!");
    }
    else if (name == NULL) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_open",
                             status,
                             "name passed is NULL!");
    }
    else if (handlePtr == NULL) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_open",
                             status,
                             "Pointer to handle instance passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First check in the local list */
        List_traverse (elem, (List_Handle) &HeapMemMP_module->objList) {
            if (((HeapMemMP_Obj *) elem)->params.name != NULL) {
                if (String_cmp (   ((HeapMemMP_Obj *)elem)->params.name, name)
                                == 0) {
                    key = IGateProvider_enter (HeapMemMP_module->localLock);
                    /* Check if we have created the HeapMemMP or not */
                    if (   ((HeapMemMP_Obj *) elem)->owner.procId
                        == MultiProc_self ()) {
                        ((HeapMemMP_Obj *) elem)->owner.openCount++;
                    }

                    *handlePtr = (((HeapMemMP_Obj *) elem)->top);
                    IGateProvider_leave (HeapMemMP_module->localLock, key);
                    doneFlag = TRUE;
                    break;
                }
            }
        }

        if (EXPECT_TRUE (doneFlag == FALSE)) {
            /* Find in name server */
            status = NameServer_getUInt32 (HeapMemMP_module->nameServer,
                                           name,
                                           &sharedShmBase,
                                           NULL);
            /* This check must not be in SYSLINK_BUILD_OPTIMIZE, because it must
             * handle the case for the runtime error HeapMemMP_E_NOTFOUND.
             */
            if (EXPECT_FALSE (status == NameServer_E_NOTFOUND)) {
                status = HeapMemMP_E_NOTFOUND;
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            else if (EXPECT_FALSE (status < 0)) {
                /* Override the status to return a HeapMemMP status code. */
                status = HeapMemMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapMemMP_open",
                                     status,
                                     "Failure in NameServer_get!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            else {
                /* Convert from shared region pointer to local address*/
                sharedAddr = SharedRegion_getPtr (sharedShmBase);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (sharedAddr == NULL)) {
                    status = HeapMemMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapMemMP_open",
                                         status,
                                         "Invalid pointer received from"
                                         "NameServer!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    status = HeapMemMP_openByAddr (sharedAddr, handlePtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (status < 0)) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "HeapMemMP_open",
                                             status,
                                            "Failed to open HeapMemMP handle!");
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_open", status);

    return status;
}


/* Closes previously opened instance of HeapMemMP module. */
Int
HeapMemMP_close (HeapMemMP_Handle * handlePtr)
{
    Int                status = HeapMemMP_S_SUCCESS;
    HeapMemMP_Object * handle = NULL;
    HeapMemMP_Obj    * obj    = NULL;
    IArg               key    = 0;

    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_close",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_close",
                             status,
                             "The parameter handlePtr i.e. pointer to handle "
                             "passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_close",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (HeapMemMP_Object *) (*handlePtr);
        obj    = (HeapMemMP_Obj *) handle->obj;
        if (obj != NULL) {
            key = IGateProvider_enter (HeapMemMP_module->localLock);
            if (obj->owner.procId == MultiProc_self ()) {
                obj->owner.openCount--;
            }

            /* Check if HeapMemMP is opened on same processor and this is the
             * last closure.
             */
            if (    (obj->owner.creator   == FALSE)
                &&  (obj->owner.openCount == 0)) {
                List_remove ((List_Handle) &HeapMemMP_module->objList,
                             &obj->listElem);

                if (obj->gate != NULL) {
                    /* Close the  instance gate*/
                    GateMP_close (&obj->gate);
                }

                /* Now free the handle */
                Memory_free (NULL, obj, sizeof (HeapMemMP_Obj));
                obj = NULL;
                Memory_free (NULL, handle, sizeof (HeapMemMP_Object));
                *handlePtr = NULL;
            }

            IGateProvider_leave (HeapMemMP_module->localLock, key);
        }
        else {
            Memory_free (NULL, handle, sizeof (HeapMemMP_Object));
            *handlePtr = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_close", status);

    return status;
}


/*
 * NOTE:
 * Embedded within the code for HeapMemMP_alloc and HeapMemMP_free are comments
 * that can be used to match a shared memory reference with its required
 * cache call.  This is done because the code for alloc and free is complex.
 * These two-character comments indicate
 * 1) The type of cache operation that is being performed {A, B}
 *    A = Cache_inv
 *    B = Cache_wbInv
 * 2) A numerical id of the specific cache call that is performed.
 *    1, 2, 3
 *    For example, the comment 'A2' indicates that the corresponding cache call
 *    is a Cache_inv operation identified by the number '2'
 */

/*
 *  ======== HeapMemMP_alloc ========
 *  HeapMemMP is implemented such that all of the memory and blocks it works
 *  with have an alignment that is a multiple of the minimum alignment and have
 *  a size which is a multiple of the minAlign. Maintaining this requirement
 *  throughout the implementation ensures that there are never any odd
 *  alignments or odd block sizes to deal with.
 *
 *  Specifically:
 *  The buffer managed by HeapMemMP:
 *    1. Is aligned on a multiple of obj->minAlign
 *    2. Has an adjusted size that is a multiple of obj->minAlign
 *  All blocks on the freelist:
 *    1. Are aligned on a multiple of obj->minAlign
 *    2. Have a size that is a multiple of obj->minAlign
 *  All allocated blocks:
 *    1. Are aligned on a multiple of obj->minAlign
 *    2. Have a size that is a multiple of obj->minAlign
 *
 */


/* Allocs a block */
Void *
HeapMemMP_alloc (HeapMemMP_Handle handle,
                 UInt32           reqSize,
                 UInt32           reqAlign)
{
    HeapMemMP_Obj    * obj    = NULL;
    Char             * allocAddr = NULL;
    IArg             key    = 0;
    HeapMemMP_Header * prevHeader;
    HeapMemMP_Header * newHeader;
    HeapMemMP_Header * curHeader;
    UInt32           curSize;
    UInt32           adjSize;
    UInt32           remainSize; /* free memory after allocated memory */
    UInt32           adjAlign;
    UInt32           offset;

    GT_3trace (curTrace, GT_ENTER, "HeapMemMP_alloc",
               handle, reqSize, reqAlign);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (reqSize != 0u));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_alloc",
                             HeapMemMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_alloc",
                             HeapMemMP_E_INVALIDARG,
                             "Invalid NULL handle pointer specified!");
    }
    else if (EXPECT_FALSE ((reqAlign & (reqAlign - 1)) != 0)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_alloc",
                             HeapMemMP_E_MEMORY,
                             "Requested reqAlign is not a power of 2!");
    }
    else if (EXPECT_FALSE (reqSize == 0)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_alloc",
                             HeapMemMP_E_INVALIDARG,
                             "Requested block size is zero!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapMemMP_Obj *) ((HeapMemMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        adjSize = (UInt32) reqSize;

        /* Make size requested a multiple of obj->minAlign */
        if ((offset = (adjSize & (obj->minAlign - 1))) != 0) {
            adjSize = adjSize + (obj->minAlign - offset);
        }

        /*
         *  Make sure the alignment is at least as large as obj->minAlign
         *  Note: adjAlign must be a power of 2 (by function constraint) and
         *  obj->minAlign is also a power of 2,
         */
        adjAlign = reqAlign;
        if (adjAlign == 0) {
            adjAlign =  obj->minAlign;
        }

        if (adjAlign & (obj->minAlign - 1)) {
            /* adjAlign is less than obj->minAlign */
            adjAlign = obj->minAlign;
        }

        /* No need to Cache_inv Attrs- 'head' should be constant */
        prevHeader = (HeapMemMP_Header *) &obj->attrs->head;

        key = GateMP_enter (obj->gate);

        /*
         *  The block will be allocated from curHeader. Maintain a pointer to
         *  prevHeader so prevHeader->next can be updated after the alloc.
         */
        if (EXPECT_FALSE (obj->cacheEnabled)) {
            Cache_inv (prevHeader,
                       sizeof (HeapMemMP_Header),
                       Cache_Type_ALL,
                       TRUE); /* A1 */
        }
        curHeader = (HeapMemMP_Header *) SharedRegion_getPtr (prevHeader->next);
        /* A1 */

        /* Loop over the free list. */
        while (curHeader != NULL) {
            /* Invalidate curHeader */
            if (EXPECT_FALSE (obj->cacheEnabled)) {
                Cache_inv (curHeader,
                           sizeof (HeapMemMP_Header),
                           Cache_Type_ALL,
                           TRUE); /* A2 */
            }

            curSize = curHeader->size;

            /*
             *  Determine the offset from the beginning to make sure
             *  the alignment request is honored.
             */
            offset = (UInt32)curHeader & (adjAlign - 1);
            if (offset) {
                offset = adjAlign - offset;
            }

            /* Internal Assert that offset is a multiple of obj->minAlign */
            if (((offset & (obj->minAlign - 1)) != 0)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapMemMP_alloc",
                                     HeapMemMP_E_FAIL,
                                     "offset is not a multiple of"
                                     " obj->minAlign!");
            }
            else {
                /* big enough? */
                if (curSize >= (adjSize + offset)) {
                    /* Set the pointer that will be returned. Alloc from front */
                    allocAddr = (Char *) ((UInt32) curHeader + offset);

                    /*
                     *  Determine the remaining memory after the allocated block.
                     *  Note: this cannot be negative because of above comparison.
                     */
                    remainSize = curSize - adjSize - offset;

                    /* Internal Assert that remainSize is a multiple of
                     * obj->minAlign
                     */
                    if (((remainSize & (obj->minAlign - 1)) != 0)) {
                        allocAddr = (UInt32) NULL;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "HeapMemMP_alloc",
                                             HeapMemMP_E_FAIL,
                                             "remainSize is not a multiple of"
                                             " obj->minAlign!");
                        break;
                    }
                    else {
                        /*
                         *  If there is memory at the beginning (due to alignment
                         *  requirements), maintain it in the list.
                         *
                         *  offset and remainSize must be multiples of
                         *  sizeof(HeapMemMP_Header). Therefore the address of the newHeader
                         *  below must be a multiple of the sizeof(HeapMemMP_Header), thus
                         *  maintaining the requirement.
                         */
                        if (offset) {
                            /* Adjust the curHeader size accordingly */
                            curHeader->size = offset; /* B2 */
                            /* Cache wb at end of this if block */

                            /*
                             *  If there is remaining memory, add into the free list.
                             *  Note: no need to coalesce and we have HeapMemMP locked so
                             *        it is safe.
                             */
                            if (remainSize) {
                                newHeader = (HeapMemMP_Header *)
                                            ((UInt32) allocAddr + adjSize);

                                /* curHeader has been inv at top of 'while' loop */
                                newHeader->next = curHeader->next;  /* B1 */
                                newHeader->size = remainSize;       /* B1 */
                                if (EXPECT_FALSE (obj->cacheEnabled)) {
                                    /* Writing back curHeader will cache-wait */
                                    Cache_wbInv (newHeader,
                                                 sizeof (HeapMemMP_Header),
                                                 Cache_Type_ALL,
                                                 FALSE); /* B1 */
                                }

                                curHeader->next = SharedRegion_getSRPtr
                                                                (newHeader,
                                                                 obj->regionId);
                                GT_assert (curTrace,
                                           (    curHeader->next
                                            !=  SharedRegion_INVALIDSRPTR));
                            }

                            /* Write back (and invalidate) newHeader and curHeader */
                            if (EXPECT_FALSE (obj->cacheEnabled)) {
                                /* B2 */
                                Cache_wbInv (curHeader,
                                             sizeof (HeapMemMP_Header),
                                             Cache_Type_ALL,
                                             TRUE);
                            }
                        }
                        else {
                            /*
                             *  If there is any remaining, link it in,
                             *  else point to the next free block.
                             *  Note: no need to coalesce and we have HeapMemMP locked so
                             *        it is safe.
                             */
                            if (remainSize) {
                                newHeader = (HeapMemMP_Header *)
                                            ((UInt32) allocAddr + adjSize);

                                newHeader->next  = curHeader->next; /* A2, B3  */
                                newHeader->size  = remainSize;      /* B3      */

                                if (EXPECT_FALSE (obj->cacheEnabled)) {
                                    /* Writing back prevHeader will cache-wait */
                                    Cache_wbInv (newHeader,
                                                 sizeof (HeapMemMP_Header),
                                                 Cache_Type_ALL,
                                                 FALSE); /* B3 */
                                }

                                /* B4 */
                                prevHeader->next = SharedRegion_getSRPtr (newHeader,
                                                                          obj->regionId);
                            }
                            else {
                                /* curHeader has been inv at top of 'while' loop */
                                prevHeader->next = curHeader->next; /* A2, B4 */
                            }

                            if (EXPECT_FALSE (obj->cacheEnabled)) {
                                /* B4 */
                                Cache_wbInv (prevHeader,
                                             sizeof(HeapMemMP_Header),
                                             Cache_Type_ALL,
                                             TRUE);
                            }
                        }
                    }

                    /* Success, return the allocated memory */
                    break;

                }
                else {
                    prevHeader = curHeader;
                    curHeader = SharedRegion_getPtr (curHeader->next);
                }
            }
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_alloc", allocAddr);

    return allocAddr;
}


/* Frees a block */
Void
HeapMemMP_free (HeapMemMP_Handle   handle,
                Ptr                addr,
                UInt32             size)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                status = HeapMemMP_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    HeapMemMP_Obj    * obj    = NULL;
    IArg               key    = 0;
    HeapMemMP_Header * curHeader = NULL;
    HeapMemMP_Header * newHeader = NULL;
    HeapMemMP_Header * nextHeader= NULL;
    SizeT              offset;

    GT_3trace (curTrace, GT_ENTER, "HeapMemMP_free", handle, addr, size);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (addr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_free",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_free",
                             status,
                             "Invalid NULL handle pointer specified!");
    }
    else if (EXPECT_FALSE (addr == NULL)) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_free",
                             status,
                             "Invalid NULL addr pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapMemMP_Obj *) ((HeapMemMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));
        GT_assert (curTrace, ((UInt32)addr % obj->minAlign == 0));

        /*
         * obj->attrs never changes, doesn't need Gate protection
         * and Cache invalidate
         */
        curHeader = (HeapMemMP_Header *) &(obj->attrs->head);

        /* Restore size to actual allocated size */
        offset = size & (obj->minAlign - 1);
        if (offset != 0) {
            size += obj->minAlign - offset;
        }

        key = GateMP_enter (obj->gate);

        newHeader = (HeapMemMP_Header *) addr;

        if (EXPECT_FALSE (obj->cacheEnabled)) {
            /* A1 */
            Cache_inv (curHeader,
                       sizeof (HeapMemMP_Header),
                       Cache_Type_ALL,
                       TRUE);
        }
        nextHeader = SharedRegion_getPtr (curHeader->next);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Make sure the entire buffer is in the range of the heap. */
        if (EXPECT_FALSE (!(    ((SizeT) newHeader >= (SizeT) obj->buf)
                            && (   (SizeT) newHeader + size
                                <= (SizeT) obj->buf + obj->bufSize)))) {
            status = HeapMemMP_E_FAIL;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_free",
                             status,
                             "Entire buffer is not in the range of the heap!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Go down freelist and find right place for buf */
            while ((nextHeader != NULL) && (nextHeader < newHeader)) {
                if (EXPECT_FALSE (obj->cacheEnabled)) {
                    Cache_inv (nextHeader,
                               sizeof(HeapMemMP_Header),
                               Cache_Type_ALL,
                               TRUE); /* A2 */
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                /* Make sure the addr is not in this free block */
                if (EXPECT_FALSE (    (SizeT) newHeader
                                  < ((SizeT) nextHeader + nextHeader->size))) {
                    /* A2 */
                    status = HeapMemMP_E_INVALIDSTATE;
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapMemMP_free",
                                     status,
                                     "Address is in this free block");
                    break;
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    curHeader = nextHeader;
                    /* A2 */
                    nextHeader = SharedRegion_getPtr (nextHeader->next);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_TRUE (status >= 0)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* B2 */
                newHeader->next = SharedRegion_getSRPtr (nextHeader,
                                                         obj->regionId);
                newHeader->size = size;

                /* B1, A1 */
                curHeader->next = SharedRegion_getSRPtr (newHeader,
                                                         obj->regionId);

                /* Join contiguous free blocks */
                if (nextHeader != NULL) {
                    /*
                     *  Verify the free size is not overlapping. Not all cases
                     *  are detectable, but it is worth a shot. Note: only do
                     *  this assert if nextHeader is non-NULL.
                     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (  ((SizeT) newHeader + size)
                                      > (SizeT) nextHeader)) {
                        /* A2 */
                        status = HeapMemMP_E_INVALIDSTATE;
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapMemMP_free",
                                         status,
                                         "Free size is overlapping");
                    }
                    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Join with upper block */
                        if (((UInt32) newHeader + size) == (UInt32)nextHeader) {
                            if (EXPECT_FALSE (obj->cacheEnabled)) {
                                Cache_inv (nextHeader,
                                           sizeof(HeapMemMP_Header),
                                           Cache_Type_ALL,
                                           TRUE);
                            }
                            newHeader->next = nextHeader->next; /* A2, B2 */
                            newHeader->size += nextHeader->size; /* A2, B2 */

                            /* Correct size for following Cache_wbInv. Needed
                             * due to another cache line fill caused by
                             * reading nextHeader->next
                             */
                            size += obj->minAlign;

                            /* Don't Cache_wbInv, this will be done later */
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_TRUE (status >= 0)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /*
                     *  Join with lower block. Make sure to check to see if not the
                     *  first block. No need to invalidate attrs since head shouldn't change.
                     */
                    if (   (curHeader != &obj->attrs->head)
                        && (    ((UInt32) curHeader + curHeader->size)
                            ==  (UInt32) newHeader)) {
                        /*
                         * Don't Cache_inv newHeader since newHeader has
                         * data that hasn't been written back yet (B2)
                         */
                        curHeader->next = newHeader->next; /* B1, B2 */
                        curHeader->size += newHeader->size; /* B1, B2 */
                    }

                    if (EXPECT_FALSE (obj->cacheEnabled)) {
                        Cache_wbInv (curHeader,
                                     sizeof (HeapMemMP_Header),
                                     Cache_Type_ALL,
                                     FALSE); /* B1 */
                        /*
                         *  Invalidate entire buffer being freed to ensure that
                         *  stale cache data in block isn't evicted later.
                         */
                        Cache_wbInv (newHeader,
                                     size,
                                     Cache_Type_ALL,
                                     TRUE);  /* B2 */
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMP_free");
}


/* Get memory statistics */
Void
HeapMemMP_getStats (HeapMemMP_Handle  handle,
                    Ptr               stats)
{
    HeapMemMP_Obj    * obj    = NULL;
    HeapMemMP_Header * curHeader = NULL;
    Memory_Stats     * memStats  = NULL;
    IArg               key       = 0;

    GT_2trace (curTrace, GT_ENTER, "HeapMemMP_getStats", handle, stats);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (stats != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getStats",
                             HeapMemMP_E_INVALIDARG,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getStats",
                             HeapMemMP_E_INVALIDARG,
                             "handle passed is null!");
    }
    else if (EXPECT_FALSE (stats == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getStats",
                             HeapMemMP_E_INVALIDARG,
                             "Invalid NULL stats pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapMemMP_Obj *) ((HeapMemMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        memStats = (Memory_Stats *) stats;

        memStats->totalSize         = obj->bufSize;
        memStats->totalFreeSize     = 0;  /* determined later */
        memStats->largestFreeSize   = 0;  /* determined later */

        key = GateMP_enter(obj->gate);

        /* Invalidate curHeader */
        if (EXPECT_FALSE (obj->cacheEnabled)) {
            Cache_inv ((Ptr) &(obj->attrs->head),
                       sizeof (HeapMemMP_Header),
                       Cache_Type_ALL,
                       TRUE);
        }

        curHeader = SharedRegion_getPtr ((SharedRegion_SRPtr)
                                                    obj->attrs->head.next);

        while (curHeader != NULL) {
            /* Invalidate curHeader */
            if (EXPECT_FALSE (obj->cacheEnabled)) {
                Cache_inv (curHeader,
                           sizeof (HeapMemMP_Header),
                           Cache_Type_ALL,
                           TRUE);
            }

            memStats->totalFreeSize += curHeader->size;
            if (memStats->largestFreeSize < curHeader->size) {
                memStats->largestFreeSize = curHeader->size;
            }
            /* This condition is required to avoid
             * assertions during call to SharedRegion_getPtr  because at the end
             * of the calculation curHeader->next  will become
             * SharedRegion_INVALIDSRPTR.
             */
            if (curHeader->next != SharedRegion_INVALIDSRPTR) {
                curHeader = SharedRegion_getPtr ((SharedRegion_SRPtr)
                                                               curHeader->next);
            }
            else {
                curHeader = NULL;
            }
        }

        GateMP_leave(obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMP_getStats");
}


/* Indicate whether the heap may block during an alloc or free call */
Bool
HeapMemMP_isBlocking (HeapMemMP_Handle handle)
{
    Bool isBlocking = FALSE;

    GT_1trace (curTrace, GT_ENTER, "Heap_isBlocking", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_isBlocking",
                             HeapMemMP_E_INVALIDARG,
                             "handle passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* TBD: Figure out how to determine whether the gate is blocking */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_isBlocking", isBlocking);

    return isBlocking;
}


/* Get extended statistics */
Void
HeapMemMP_getExtendedStats (HeapMemMP_Handle           handle,
                            HeapMemMP_ExtendedStats  * stats)
{
    HeapMemMP_Obj * obj = NULL;

    GT_2trace (curTrace, GT_ENTER, "HeapMemMP_getExtendedStats", handle, stats);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (stats != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getExtendedStats",
                             HeapMemMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getExtendedStats",
                             HeapMemMP_E_INVALIDARG,
                             "Invalid NULL handle pointer specified!");
    }
    else if (EXPECT_FALSE (stats == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getExtendedStats",
                             HeapMemMP_E_INVALIDARG,
                             "Invalid NULL stats pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapMemMP_Obj *) ((HeapMemMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        stats->buf   = obj->buf;
        stats->size  = obj->bufSize;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMP_getExtendedStats");
}


/* Returns the shared memory size requirement for a single instance. */
SizeT
HeapMemMP_sharedMemReq (const HeapMemMP_Params * params)
{
    SizeT   memReq = 0;
    UInt32  minAlign;
    UInt16  regionId;

    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_sharedMemReq", params);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (params == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_sharedMemReq",
                             HeapMemMP_E_INVALIDARG,
                             "Params pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (params->sharedAddr == NULL) {
            regionId = params->regionId;
        }
        else {
            regionId = SharedRegion_getId(params->sharedAddr);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionId == SharedRegion_INVALIDREGIONID)  {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMemMP_sharedMemReq",
                                 HeapMemMP_E_FAIL,
                                 "params->sharedAddr is not in a"
                                 " valid SharedRegion!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            minAlign = sizeof (HeapMemMP_Header);
            if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
                minAlign = SharedRegion_getCacheLineSize (regionId);
            }

            /* Add size of HeapMemMP Attrs */
            memReq = _Ipc_roundup (sizeof(HeapMemMP_Attrs), minAlign);

            /* Add the buffer size */
            memReq += params->sharedBufSize;

            /* Make sure the size is a multiple of minAlign (round down) */
            memReq = (memReq / minAlign) * minAlign;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_sharedMemReq", memReq);

    return memReq;
}


/*!
 *  @brief      Returns the HeapMemMP kernel object pointer.
 *
 *  @param      handle  Handle to previousely created/opened instance.
 *
 */
Void *
HeapMemMP_getKnlHandle (HeapMemMP_Handle handle)
{
    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_getKnlHandle", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        /*! @retval NULL Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getKnlHandle",
                             HeapMemMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        /*! @retval NULL handle passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_getKnlHandle",
                             HeapMemMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Nothing to be done for kernel-side implementation. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_getKnlHandle", handle);

    /*! @retval Kernel-Object-handle Operation successfully completed. */
    return (handle);
}


/* HeapMemMP open by address */
Int
HeapMemMP_openByAddr (Ptr                sharedAddr,
                      HeapMemMP_Handle * handlePtr)
{
    Int               status   = HeapMemMP_S_SUCCESS;
    Bool              doneFlag = FALSE;
    HeapMemMP_Attrs * attrs    = NULL;
    List_Elem *       elem     = NULL;
    IArg              key;
    UInt16            id;
    HeapMemMP_Params  params;

    GT_2trace (curTrace, GT_ENTER, "HeapMemMP_openByAddr",
               sharedAddr, handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapMemMP_module->refCount),
                                                HeapMemMP_MAKE_MAGICSTAMP(0),
                                                HeapMemMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = HeapMemMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_openByAddr",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handlePtr == NULL)) {
        status = HeapMemMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMP_openByAddr",
                             status,
                             "handlePtr pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First check in the local list */
        List_traverse (elem, (List_Handle) &HeapMemMP_module->objList) {
            if (((HeapMemMP_Obj *) elem)->params.sharedAddr == sharedAddr) {
                key = IGateProvider_enter (HeapMemMP_module->localLock);
                if (   ((HeapMemMP_Obj *)elem)->owner.procId
                    == MultiProc_self ()) {
                    ((HeapMemMP_Obj *)elem)->owner.openCount++;
                }
                IGateProvider_leave (HeapMemMP_module->localLock, key);
                *handlePtr = (((HeapMemMP_Obj *)elem)->top);
                doneFlag = TRUE;
                break;
            }
        }

        /* If not already existing locally, create object locally for open. */
        if (EXPECT_FALSE (doneFlag == FALSE)) {
            HeapMemMP_Params_init(&params);
            params.sharedAddr = sharedAddr;
            attrs = (HeapMemMP_Attrs *) sharedAddr;
            id = SharedRegion_getId (sharedAddr);

            if (EXPECT_FALSE (SharedRegion_isCacheEnabled (id))) {
                Cache_inv (attrs,
                           sizeof (HeapMemMP_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }

            if (EXPECT_FALSE (attrs->status != HeapMemMP_CREATED)) {
                *handlePtr = NULL;
                status = HeapMemMP_E_NOTFOUND;
                /* Don't set failure reason since this is an expected
                 * run-time failure.
                 */
                GT_1trace (curTrace,
                           GT_3CLASS,
                           "HeapMemMP Instance is not created yet at the "
                           "provided shared addr.\n"
                           "    sharedAddr [0x%x]!",
                           sharedAddr);
            }
            else {
                status = _HeapMemMP_create (handlePtr, &params, FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (status < 0)) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapMemMP_openByAddr",
                                         status,
                                         "_HeapMemMP_create failed!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_openByAddr", status);

    return (status);
}



/* =============================================================================
 * Internal function
 * =============================================================================
 */
GateMP_Handle HeapMemMP_getGate(HeapMemMP_Handle handle)
{
    HeapMemMP_Object *  object;
    HeapMemMP_Obj    *  obj;

    object = (HeapMemMP_Object *)handle;
    obj = (HeapMemMP_Obj *)(object->obj);
    return(obj->gate);
}

/* Creates a new instance of HeapMemMP module.
 * This is an internal function as both HeapMemMP_create
 * and HeapMemMP_open use the functionality.
 */
Int
_HeapMemMP_create (      HeapMemMP_Handle *  handlePtr,
                   const HeapMemMP_Params *  params,
                         Bool                createFlag)
{
    Int                 status = HeapMemMP_S_SUCCESS;
    HeapMemMP_Obj    *  obj    = NULL;
    HeapMemMP_Object *  handle = NULL;
    GateMP_Handle       gateHandle = NULL;
    Ptr                 localAddr = NULL;
    IArg                key;
    SharedRegion_SRPtr  sharedShmBase;

    GT_3trace (curTrace, GT_ENTER, "_HeapMemMP_create",
               handlePtr, params, createFlag);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (params != NULL));

    /* No need for parameter checks, since this is an internal function. */

    /* Create the generic handle */
    handle = (HeapMemMP_Object *) Memory_calloc (NULL,
                                                 sizeof (HeapMemMP_Object),
                                                 0u,
                                                 NULL);
    *handlePtr = (HeapMemMP_Handle) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval HeapMemMP_E_MEMORY Memory allocation failed for pointer of
         *          type HeapMemMP_Object
         */
        status = HeapMemMP_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_HeapMemMP_create",
                             status,
                             "Memory allocation failed for pointer"
                             " of type HeapMemMP_Object!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Create the Heap Buf specific handle */
        obj = (HeapMemMP_Obj *) Memory_calloc (NULL,
                                               sizeof (HeapMemMP_Obj),
                                               0u,
                                               NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            /*! @retval HeapMemMP_E_MEMORY Memory allocation failed for pointer
             *          of type HeapMemMP_Obj
             */
            status = HeapMemMP_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_HeapMemMP_create",
                                 status,
                                 "Memory allocation failed for pointer"
                                 " of type HeapMemMP_Obj");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            handle->obj = (HeapMemMP_Obj *) obj ;
            handle->alloc = (IHeap_allocFxn) &HeapMemMP_alloc;
            handle->free  = (IHeap_freeFxn) &HeapMemMP_free;
            handle->getStats     = (IHeap_getStatsFxn) &HeapMemMP_getStats;
            handle->getKnlHandle = (IHeap_getKnlHandleFxn)
                                                    &HeapMemMP_getKnlHandle;
            handle->isBlocking   = (IHeap_isBlockingFxn) &HeapMemMP_isBlocking;

            obj->nsKey     = NULL;
            obj->allocSize = 0;

            /* Put in the local list */
            key = IGateProvider_enter (HeapMemMP_module->localLock);
            List_elemClear (&obj->listElem);
            List_put ((List_Handle) &HeapMemMP_module->objList, &obj->listElem);
            IGateProvider_leave (HeapMemMP_module->localLock, key);

            if (createFlag == FALSE) {
                obj->owner.creator = FALSE;
                obj->owner.openCount = 0u;
                obj->owner.procId = MultiProc_INVALIDID;
                obj->top = handle;

                obj->attrs = (HeapMemMP_Attrs *) params->sharedAddr;

                /* No need to Cache_inv- already done in openByAddr() */
                obj->buf = (Char *) SharedRegion_getPtr ((SharedRegion_SRPtr)
                                                            obj->attrs->bufPtr);
                obj->bufSize      = obj->attrs->head.size;
                obj->regionId     = SharedRegion_getId (obj->buf);
                GT_assert (curTrace,
                           (obj->regionId != SharedRegion_INVALIDSRPTR));
                obj->cacheEnabled = SharedRegion_isCacheEnabled (obj->regionId);

                /* Set minAlign */
                obj->minAlign = sizeof (HeapMemMP_Header); /* 64 bits = 8 bytes */
                if (    SharedRegion_getCacheLineSize (obj->regionId)
                    >   obj->minAlign) {
                    obj->minAlign = SharedRegion_getCacheLineSize (
                                                                obj->regionId);
                }

                localAddr = SharedRegion_getPtr (obj->attrs->gateMPAddr);

                status = GateMP_openByAddr (localAddr, &gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override the status with a HeapMemMP status code. */
                    status = HeapMemMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_HeapMemMP_create",
                                         status,
                                         "Failed to open GateMP!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj->gate = gateHandle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else {
                /* Creating the HeapMemMP ... */
                obj->owner.creator = TRUE;
                obj->owner.openCount = 1u;
                obj->owner.procId = MultiProc_self ();
                obj->top = handle;

                /* Creating the gate */
                if (params->gate != NULL) {
                    obj->gate = params->gate;
                }
                else {
                    /* If no gate specified, get the default system gate */
                    obj->gate = GateMP_getDefaultRemote ();
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (obj->gate == NULL) {
                    status = HeapMemMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_HeapMemMP_create",
                                         status,
                                         "GateMP is NULL!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj->bufSize = params->sharedBufSize;

                    if (params->sharedAddr == NULL) {
                        /* Creating using a shared region ID */
                        /* It is allowed to have NULL name for an anonymous, not
                         * to be opened by name, heap.
                         */
                        obj->attrs = NULL; /* Will be alloced in postInit */
                        obj->regionId = params->regionId;
                    }
                    else {
                        /* Creating using sharedAddr */
                        obj->regionId = SharedRegion_getId (params->sharedAddr);

                        /* Assert that the buffer is in a valid shared
                         * region
                         */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (obj->regionId == SharedRegion_INVALIDREGIONID) {
                            status = HeapMemMP_E_FAIL;
                            /*! @retval HeapMemMP_E_FAIL params->sharedAddr is
                                              not in a valid SharedRegion. */
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_HeapMemMP_create",
                                                 status,
                                                 "params->sharedAddr is not in"
                                                 " a valid SharedRegion!");
                        }
                        else if (  ((UInt32) params->sharedAddr
                                 % SharedRegion_getCacheLineSize (obj->regionId)
                                 != 0)) {
                            status = HeapMemMP_E_FAIL;
                            /*! @retval HeapMemMP_E_FAIL params->sharedAddr does
                                        not meet cache alignment constraints */
                            GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_ListMP_create",
                                         status,
                                         "params->sharedAddr does not"
                                         " meet cache alignment constraints!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            /* obj->buf will get alignment-adjusted in
                             * postInit
                             */
                            obj->buf = (Ptr) (  (UInt32) params->sharedAddr
                                              + sizeof (HeapMemMP_Attrs));
                            obj->attrs = (HeapMemMP_Attrs *) params->sharedAddr;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        obj->cacheEnabled = SharedRegion_isCacheEnabled (
                                                                obj->regionId);

                        /* Set minAlign */
                        obj->minAlign = sizeof (HeapMemMP_Header);
                        if (    SharedRegion_getCacheLineSize(obj->regionId)
                            >   obj->minAlign) {
                            obj->minAlign = SharedRegion_getCacheLineSize (
                                                                obj->regionId);
                        }

                        status = HeapMemMP_postInit ((HeapMemMP_Object *)
                                                         handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (EXPECT_FALSE (status < 0)) {
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_HeapMemMP_create",
                                                 status,
                                                 "HeapMemMP_postInit failed!");
                        }
                    }
                }

                if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Populate the params member */
                    Memory_copy ((Ptr) &obj->params,
                                 (Ptr) params,
                                 sizeof (HeapMemMP_Params));

                    /* Copy the name */
                    if (params->name != NULL) {
                        obj->params.name = (String) Memory_alloc (NULL,
                                            (String_len (params->name)+ 1u),
                                            0,
                                            NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (obj->params.name == NULL) {
                            /*! @retval HeapMemMP_E_MEMORY Memory allocation
                             *          failed for name
                             */
                            status = HeapMemMP_E_MEMORY;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_HeapMemMP_create",
                                                 status,
                                                 "Memory allocation "
                                                 "failed for name!");
                        }
                        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            String_ncpy (obj->params.name,
                                         params->name,
                                         String_len (params->name) + 1u);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* We will store a shared pointer in the NameServer */
                    sharedShmBase = SharedRegion_getSRPtr ((Ptr) obj->attrs,
                                                           obj->regionId);
                    /* Just assert. No need to check since it's checked earlier.
                     */
                    GT_assert (curTrace,
                               (sharedShmBase != SharedRegion_INVALIDSRPTR));
                    if (obj->params.name != NULL) {
                        obj->nsKey = NameServer_addUInt32 (
                                               HeapMemMP_module->nameServer,
                                               params->name,
                                               (UInt32) sharedShmBase);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (obj->nsKey == NULL) {
                            /*! @retval HeapMemMP_E_FAIL Failed to add to
                             *                         NameServer */
                            status = HeapMemMP_E_FAIL;
                            GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "HeapMemMP_create",
                                            status,
                                            "Failed to add to NameServer!");
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        if (createFlag == TRUE) {
            HeapMemMP_delete ((HeapMemMP_Handle *) handlePtr);
        }
        else {
            HeapMemMP_close ((HeapMemMP_Handle *) handlePtr);
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_HeapMemMP_create", status);

    /*! @retval HeapMemMP_S_SUCCESS Operation successful*/
    return status;
}


/*
 *  Slice and dice the buffer up into the correct size blocks and
 *  add to the freelist.
 */
Int
HeapMemMP_postInit (HeapMemMP_Object * handle)
{
    Int              status     = HeapMemMP_S_SUCCESS;
    HeapMemMP_Obj *  obj        = NULL;
    HeapMemMP_Handle regionHeap = NULL;
    HeapMemMP_Params params;

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMP_postInit");

    GT_assert (curTrace, (handle != NULL));

    /* No need for parameter checks, since this is an internal function. */

    obj = handle->obj;

    if (obj->attrs == NULL) {
        /* Need to allocate from the heap */
        HeapMemMP_Params_init (&params);
        params.regionId      = obj->regionId;
        params.sharedBufSize = obj->bufSize;
        obj->allocSize = HeapMemMP_sharedMemReq (&params);
        regionHeap = SharedRegion_getHeap (obj->regionId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionHeap == NULL) {
            status = HeapMemMP_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMemMP_postInit",
                                 status,
                                 "Shared Region heap is null!");

        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->attrs = Memory_alloc ((IHeap_Handle) regionHeap,
                                       obj->allocSize,
                                       obj->minAlign,
                                       NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj->attrs == NULL) {
                status = HeapMemMP_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapMemMP_postInit",
                                     status,
                                     "Failed to allocate shared memory!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                obj->buf = (Ptr)((UInt32)obj->attrs + sizeof(HeapMemMP_Attrs));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Round obj->buf up by obj->minAlign */
        obj->buf = (Ptr) _Ipc_roundup ((obj->buf), (obj->minAlign));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (  obj->bufSize
                          < SharedRegion_getCacheLineSize (obj->regionId))) {
            status = HeapMemMP_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMemMP_postInit",
                                 status,
                                 "Buffer is not large enough!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Make sure the size is a multiple of obj->minAlign */
            obj->bufSize = (obj->bufSize / obj->minAlign) * obj->minAlign;

            obj->attrs->gateMPAddr = GateMP_getSharedAddr (obj->gate);
            obj->attrs->bufPtr     = SharedRegion_getSRPtr (obj->buf,
                                                            obj->regionId);

            /* Store computed obj->bufSize in shared mem */
            obj->attrs->head.size = obj->bufSize;

            /* Place the initial header */
            HeapMemMP_restore ((HeapMemMP_Handle) handle);

            /* Last thing, set the status */
            obj->attrs->status = HeapMemMP_CREATED;

            if (EXPECT_FALSE (obj->cacheEnabled)) {
                Cache_wbInv ((Ptr) obj->attrs,
                             sizeof (HeapMemMP_Attrs),
                             Cache_Type_ALL,
                             TRUE);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMP_postInit", status);

    return status;
}


/* Restore an instance to it's original created state. */
Void
HeapMemMP_restore (HeapMemMP_Handle handle)
{
    HeapMemMP_Header * begHeader = NULL;
    HeapMemMP_Obj    * obj       = NULL;

    GT_1trace (curTrace, GT_ENTER, "HeapMemMP_restore", handle);

    obj = ((HeapMemMP_Object *) handle)->obj;
    GT_assert (curTrace, (obj != NULL));

    /*
     *  Fill in the top of the memory block
     *  next: pointer will be NULL (end of the list)
     *  size: size of this block
     *  NOTE: no need to Cache_inv because obj->attrs->bufPtr should be const
     */
    begHeader = (HeapMemMP_Header *) obj->buf;
    begHeader->next = (SharedRegion_SRPtr) SharedRegion_INVALIDSRPTR;
    begHeader->size = obj->bufSize;

    obj->attrs->head.next = obj->attrs->bufPtr;
    if (EXPECT_FALSE (obj->cacheEnabled)) {
        Cache_wbInv ((Ptr) &(obj->attrs->head),
                     sizeof (HeapMemMP_Header),
                     Cache_Type_ALL,
                     FALSE);
        Cache_wbInv (begHeader,
                     sizeof (HeapMemMP_Header),
                     Cache_Type_ALL,
                     TRUE);
    }

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMP_restore");
}
