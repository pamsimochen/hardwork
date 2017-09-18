/*
 *  @file   HeapBufMP.c
 *
 *  @brief      Defines HeapBufMP based memory allocator.
 *
 *  ======== HeapBufMP ========
 *  Multi-processor fixed-size buffer heap implementation
 *
 *  Heap implementation that manages fixed size buffers that can be used
 *  in a multiprocessor system with shared memory.
 *
 *  The HeapBufMP manager provides functions to allocate and free storage from a
 *  heap of type HeapBufMP. HeapBufMP manages a single fixed-size buffer,
 *  split into equally sized allocatable blocks.
 *
 *  The HeapBufMP manager is intended as a very fast memory
 *  manager which can only allocate blocks of a single size. It is ideal for
 *  managing a heap that is only used for allocating a single type of object,
 *  or for objects that have very similar sizes.
 *
 *  The HeapBufMP module uses a NameServer instance to
 *  store instance information when an instance is created.  The name supplied
 *  must be unique for all HeapBufMP instances.
 *
 *  The create initializes the shared memory as needed. Once an
 *  instance is created, an open can be performed. The
 *  open is used to gain access to the same HeapBufMP instance.
 *  Generally an instance is created on one processor and opened on the
 *  other processor(s).
 *
 *  The open returns a HeapBufMP instance handle like the create,
 *  however the open does not modify the shared memory.
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
#include <ti/syslink/utils/List.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/ipc/HeapMemMP.h>

/* Module level headers */
#include <ti/ipc/HeapBufMP.h>
#include <ti/syslink/inc/_HeapBufMP.h>
#include <ti/ipc/ListMP.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*!
 *  @var    HeapBufMP_nameServer
 *
 *  @brief  Name of the reserved NameServer used for HeapBufMP.
 */
#define HeapBufMP_NAMESERVER  "HeapBufMP"

/*! @brief Macro to make a correct module magic number with refCount */
#define HeapBufMP_MAKE_MAGICSTAMP(x) ((HeapBufMP_MODULEID << 12u) | (x))



/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Structure defining processor related information for the
 *          module.
 */
typedef struct HeapBufMP_ProcAttrs_tag {
    Bool   creator;   /*!< Creator or opener */
    UInt16 procId;    /*!< Processor Identifier */
    UInt32 openCount; /*!< How many times it is opened on a processor */
} HeapBufMP_ProcAttrs;


/*! Structure of attributes in shared memory */
typedef struct HeapBufMP_Attrs_tag {
    Bits32              status;
    /*!< Module status                 */
    SharedRegion_SRPtr  gateMPAddr;
    /*!< GateMP SRPtr (shm safe)       */
    SharedRegion_SRPtr  bufPtr;
    /*!< Memory managed by instance    */
    Bits32              numFreeBlocks;
    /*!< Number of free blocks         */
    Bits32              minFreeBlocks;
    /*!< Min number of free blocks     */
    Bits32              blockSize;
    /*!< True size of each block       */
    Bits32              align;
    /*!< Alignment of each block       */
    Bits32              numBlocks;
    /*!< Number of individual blocks.  */
    Bits16              exact;
    /*!< For 'exact' allocation        */
} HeapBufMP_Attrs;

/*!
 *  Structure for HeapBufMP module state
 */
typedef struct HeapBufMP_ModuleObject {
    Atomic                 refCount;
    /*!< Reference count */
    NameServer_Handle      nameServer;
    /*!< Handle to the local NameServer used for storing GP objects */
    List_Object            objList;
    /*!< List holding created objects */
    IGateProvider_Handle   localLock;
    /*!< Handle to lock for protecting objList */
    HeapBufMP_Config       cfg;
    /*!< Current config values */
    HeapBufMP_Config       defaultCfg;
    /*!< Default config values */
    HeapBufMP_Params       defaultInstParams;
    /*!< Default instance creation parameters */
} HeapBufMP_ModuleObject;


/*!
 *  @brief  Structure for the Handle for the HeapBufMP.
 */
typedef struct HeapBufMP_Obj_tag {
    List_Elem           listElem;
    /* Used for creating a linked list */
    volatile HeapBufMP_Attrs   * attrs;
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
    ListMP_Handle       freeList;
    /* List of free buffers          */
    UInt32              blockSize;
    /* Adjusted blockSize            */
    UInt32              align;
    /* Adjusted alignment            */
    UInt                numBlocks;
    /* Number of blocks in buffer    */
    Bool                exact;
    /* Exact match flag              */
    HeapBufMP_ProcAttrs owner;
    /* Processor related information for owner processor */
    Ptr                 top;
    /* Pointer to the top Object */
    HeapBufMP_Params    params;
    /* instance creation parameters */
} HeapBufMP_Obj;


/* =============================================================================
 * Globals
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
HeapBufMP_ModuleObject HeapBufMP_state =
{
    .defaultCfg.maxNameLen            = 32u,
    .defaultCfg.maxRunTimeEntries     = 32u,
    .defaultCfg.trackAllocs           = FALSE,
    .defaultInstParams.gate           = NULL,
    .defaultInstParams.exact          = FALSE,
    .defaultInstParams.name           = NULL,
    .defaultInstParams.align          = 1u,
    .defaultInstParams.numBlocks      = 0u,
    .defaultInstParams.blockSize      = 0u,
    .defaultInstParams.regionId       = 0u,
    .defaultInstParams.sharedAddr     = NULL,
};

/*!
 *  @var    HeapBufMP_module
 *
 *  @brief  Pointer to the HeapBufMP module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
HeapBufMP_ModuleObject * HeapBufMP_module = &HeapBufMP_state;


/* =============================================================================
 * Forward declaration of internal function
 * =============================================================================
 */
/*!
 *  @brief      Creates a new instance of HeapBufMP module.
 *              This is an internal function as both HeapBufMP_create
 *              and HeapBufMP_open use the functionality.
 *
 *  @param      handlePtr  Return value: Handle
 *  @param      params  Instance config-params structure.
 *  @param      createFlag Indicates whether this is a create or open call.
 *
 *  @sa         HeapBufMP_delete,
 *              ListMP_Params_init
 *              ListMP_sharedMemReq
 *              Gate_enter
 *              Gate_leave
 *              GateMutex_delete
 *              NameServer_addUInt32
 */
Int _HeapBufMP_create (      HeapBufMP_Handle * handlePtr,
                       const HeapBufMP_Params * params,
                             Bool               createFlag);

/*
 *  Slice and dice the buffer up into the correct size blocks and
 *  add to the freelist.
 */
Int HeapBufMP_postInit (HeapBufMP_Object * handle);


/* =============================================================================
 * APIS
 * =============================================================================
 */
/* Function to get default configuration for the HeapBufMP module. */
Void
HeapBufMP_getConfig (HeapBufMP_Config * cfgParams)
{
    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getConfig",
                             HeapBufMP_E_INVALIDARG,
                             "Argument of type (HeapBufMP_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                      HeapBufMP_MAKE_MAGICSTAMP(0),
                                      HeapBufMP_MAKE_MAGICSTAMP(1))
            == TRUE) {
            Memory_copy (cfgParams,
                         &HeapBufMP_module->defaultCfg,
                         sizeof (HeapBufMP_Config));
        }
        else {
            Memory_copy (cfgParams,
                         &HeapBufMP_module->cfg,
                         sizeof (HeapBufMP_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "HeapBufMP_getConfig");
}


/* Function to setup the HeapBufMP module. */
Int
HeapBufMP_setup (const HeapBufMP_Config * cfg)
{
    Int                 status = HeapBufMP_S_SUCCESS;
    Error_Block         eb;
    HeapBufMP_Config    tmpCfg;
    NameServer_Params   params;

    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        HeapBufMP_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    if (cfg == NULL) {
        HeapBufMP_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&HeapBufMP_module->refCount,
                            HeapBufMP_MAKE_MAGICSTAMP(0),
                            HeapBufMP_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&HeapBufMP_module->refCount)
        != HeapBufMP_MAKE_MAGICSTAMP(1u)) {
        status = HeapBufMP_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "HeapBufMP Module already initialized!");
    }
    else {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (cfg->maxNameLen == 0) {
            /*! @retval HeapBufMP_E_INVALIDARG cfg->maxNameLen passed is 0 */
            status = HeapBufMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_setup",
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
             HeapBufMP_module->nameServer = NameServer_create (
                                                           HeapBufMP_NAMESERVER,
                                                           &params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
             if (HeapBufMP_module->nameServer == NULL) {
                 /*! @retval HeapBufMP_E_FAIL Failed to create the HeapBufMP
                  *          name server
                  */
                 status = HeapBufMP_E_FAIL;
                 GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "HeapBufMP_setup",
                                      HeapBufMP_E_FAIL,
                                      "Failed to create the HeapBufMP "
                                      "nameserver!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Construct the list object */
                List_construct (&HeapBufMP_module->objList, NULL);
                /* Copy the cfg */
                Memory_copy ((Ptr) &HeapBufMP_module->cfg,
                             (Ptr) cfg,
                             sizeof (HeapBufMP_Config));
                /* Create a lock for protecting list object */
                HeapBufMP_module->localLock = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (HeapBufMP_module->localLock == NULL) {
                    /*! @retval HeapBufMP_E_FAIL Failed to create the
                                                localLock */
                    status = HeapBufMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapBufMP_setup",
                                         HeapBufMP_E_FAIL,
                                         "Failed to create the localLock!");
                    HeapBufMP_destroy ();
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_setup", status);

    /*! @retval HeapBufMP_S_SUCCESS Operation successful */
    return status;
}


/* Function to destroy the HeapBufMP module. */
Int
HeapBufMP_destroy (void)
{
    Int         status = HeapBufMP_S_SUCCESS;
    List_Elem * elem   = NULL;

    GT_0trace (curTrace, GT_ENTER, "HeapBufMP_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval HeapBufMP_E_INVALIDSTATE Module was not initialized */
        status = HeapBufMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&HeapBufMP_module->refCount)
            == HeapBufMP_MAKE_MAGICSTAMP(0)) {
            /* Temporarily increment refCount here. */
            Atomic_set (&HeapBufMP_module->refCount,
                        HeapBufMP_MAKE_MAGICSTAMP (1));
            /* Check if any HeapBufMP instances have not been deleted so far.
             * If not, delete them.
             */
            List_traverse (elem, (List_Handle) &HeapBufMP_module->objList) {
                if (   (   ((HeapBufMP_Obj *) elem)->owner.procId
                    == MultiProc_self ())) {
                    status = HeapBufMP_delete ((HeapBufMP_Handle*)
                                             &(((HeapBufMP_Obj *) elem)->top));
                }
                else {
                    status = HeapBufMP_close ((HeapBufMP_Handle *)
                                             &(((HeapBufMP_Obj *) elem)->top));
                }
            }

            /* Again reset refCount. */
            Atomic_set (&HeapBufMP_module->refCount,
                        HeapBufMP_MAKE_MAGICSTAMP(0));

            if (EXPECT_TRUE (HeapBufMP_module->nameServer != NULL)) {
                /* Delete the nameserver for modules */
                status = NameServer_delete (&HeapBufMP_module->nameServer);
                GT_assert (curTrace, (status >= 0));
                if (status < 0) {
                    /* Override the status to return a HeapBufMP status code. */
                    status = HeapBufMP_E_FAIL;
                }
            }

            /* Destruct the list object */
            List_destruct (&HeapBufMP_module->objList);

            /* Delete the local lock */
            if (HeapBufMP_module->localLock != NULL) {
                status = GateMutex_delete ((GateMutex_Handle *)
                                           &HeapBufMP_module->localLock);
                GT_assert (curTrace, (status >= 0));
                if (status < 0) {
                    /* Override the status to return a HeapBufMP status code. */
                    status = HeapBufMP_E_FAIL;
                }
            }

            Memory_set (&HeapBufMP_module->cfg, 0, sizeof (HeapBufMP_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_destroy", status);

    /*! @retval HeapBufMP_S_SUCCESS Operation successful */
    return status;
}


/* Initialize this config-params structure with supplier-specified
 * defaults before instance creation.
 */
Void
HeapBufMP_Params_init (HeapBufMP_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_Params_init",
                             HeapBufMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_Params_init",
                             HeapBufMP_E_INVALIDARG,
                             "Argument of type (HeapBufMP_Params *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(HeapBufMP_module->defaultInstParams),
                     sizeof (HeapBufMP_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapBufMP_Params_init");
}


/* Creates a new instance of HeapBufMP module. */
HeapBufMP_Handle
HeapBufMP_create (const HeapBufMP_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                  status = HeapBufMP_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    HeapBufMP_Object *   handle = NULL;
    HeapBufMP_Params     sparams;

    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = HeapBufMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_create",
                             status,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_create",
                             status,
                             "Params pointer passed is NULL!");
    }
    else if (params->blockSize == 0) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_create",
                             status,
                             "Block size is 0!");
    }
    else if (params->numBlocks == 0) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_create",
                             status,
                             "Number of blocks is 0!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (&sparams,
                     (Ptr) params,
                     sizeof (HeapBufMP_Params));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        _HeapBufMP_create ((HeapBufMP_Handle *) &handle, &sparams, TRUE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_create",
                             status,
                             "_HeapBufMP_create failed in HeapBufMP_create!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_create", handle);

    return (HeapBufMP_Handle) handle;
}


/* Deletes an instance of HeapBufMP module. */
Int
HeapBufMP_delete (HeapBufMP_Handle * handlePtr)
{
    Int                status = HeapBufMP_S_SUCCESS;
    HeapBufMP_Object * handle = NULL;
    HeapBufMP_Obj    * obj    = NULL;
    HeapMemMP_Handle   regionHeap = NULL;
    IArg               key    = 0;
    IArg               key1   = 0;

    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = HeapBufMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_delete",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_delete",
                             status,
                             "The parameter handlePtr i.e. pointer to handle "
                             "passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_delete",
                             status,
                             "Handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (HeapBufMP_Object *) (*handlePtr);
        obj    = (HeapBufMP_Obj *) handle->obj;

        if (obj != NULL) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            /* Check if we have created the HeapBufMP or not */
            if (obj->owner.procId != MultiProc_self ()) {
                status = HeapBufMP_E_INVALIDSTATE;
                GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_delete",
                                 status,
                                 "Instance was not created on this processor!");
             }
             else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Take the local lock */
                key = GateMP_enter (obj->gate);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (obj->owner.openCount > 1) {
                    status = HeapBufMP_E_INVALIDSTATE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapBufMP_delete",
                                         status,
                                         "Unmatched open/close calls!");
                    /* Release the global lock */
                    GateMP_leave (obj->gate, key);
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Remove from  the local list */
                    key1 = IGateProvider_enter (HeapBufMP_module->localLock);
                    List_remove ((List_Handle) &HeapBufMP_module->objList,
                                 &obj->listElem);
                    IGateProvider_leave (HeapBufMP_module->localLock, key1);

                    if (EXPECT_TRUE(obj->params.name != NULL)) {
                        if (EXPECT_TRUE(obj->nsKey != NULL)) {
                            NameServer_removeEntry (HeapBufMP_module->nameServer,
                                                    obj->nsKey);
                            obj->nsKey = NULL;
                        }
                        Memory_free (NULL,
                                     obj->params.name,
                                     String_len (obj->params.name) + 1u);
                    }

                    if (obj->attrs != NULL) {
                        obj->attrs->status = 0;
                        if (obj->cacheEnabled) {
                            Cache_wbInv ((Ptr) obj->attrs,
                                         sizeof (HeapBufMP_Attrs),
                                         Cache_Type_ALL,
                                         TRUE);
                        }
                    }

                    /* Release the shared lock */
                    GateMP_leave (obj->gate, key);

                    if (obj->freeList != NULL) {
                        /* Free the list */
                        ListMP_delete ((ListMP_Handle *) &obj->freeList);
                    }

                    regionHeap = SharedRegion_getHeap (obj->regionId);

                    if (   (regionHeap != NULL)
                        && (obj->params.sharedAddr == NULL)
                        && (obj->attrs != NULL)){
                        Memory_free ((IHeap_Handle) regionHeap,
                                     (Ptr) obj->attrs,
                                     obj->allocSize);
                    }

                    /* Now free the handle */
                    Memory_free (NULL, obj, sizeof (HeapBufMP_Obj));
                    Memory_free (NULL, handle, sizeof (HeapBufMP_Object));
                    *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else {
            Memory_free (NULL, handle, sizeof (HeapBufMP_Object));
            *handlePtr = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_delete", status);

    return status;
}


/* Opens a created instance of HeapBufMP module. */
Int
HeapBufMP_open (String             name,
                HeapBufMP_Handle * handlePtr)
{
    Int                  status        = HeapBufMP_S_SUCCESS;
    SharedRegion_SRPtr   sharedShmBase = SharedRegion_INVALIDSRPTR;
    Ptr                  sharedAddr    = NULL;
    IArg                 key           = 0;
    Bool                 doneFlag      = FALSE;
    List_Elem      *     elem          = NULL;

    GT_2trace (curTrace, GT_ENTER, "HeapBufMP_open", name, handlePtr);

    GT_assert (curTrace, (name != NULL));
    GT_assert (curTrace, (handlePtr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = HeapBufMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_open",
                             status,
                             "Module was not initialized!");
    }
    else if (name == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_open",
                             status,
                             "name passed is NULL!");
    }
    else if (handlePtr == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_open",
                             status,
                             "Pointer to handle instance passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First check in the local list */
        List_traverse (elem, (List_Handle) &HeapBufMP_module->objList) {
            if ((((HeapBufMP_Obj *) elem)->params.name != NULL)) {
                if (String_cmp (   ((HeapBufMP_Obj *) elem)->params.name, name)
                                == 0) {
                    key = IGateProvider_enter (HeapBufMP_module->localLock);
                    /* Check if we have created the HeapBufMP or not */
                    if (   ((HeapBufMP_Obj *) elem)->owner.procId
                        == MultiProc_self ()) {
                        ((HeapBufMP_Obj *) elem)->owner.openCount++;
                    }
                    *handlePtr = (((HeapBufMP_Obj *) elem)->top);
                    IGateProvider_leave (HeapBufMP_module->localLock, key);
                    doneFlag = TRUE;
                    break;
                }
            }
        }

        if (EXPECT_TRUE (doneFlag == FALSE)) {
            /* Find in name server */
            status = NameServer_getUInt32 (HeapBufMP_module->nameServer,
                                           name,
                                           &sharedShmBase,
                                           NULL);
            /* This check must not be in SYSLINK_BUILD_OPTIMIZE, because it must
             * handle the case for the runtime error HeapBufMP_E_NOTFOUND.
             */
            if (EXPECT_FALSE (status == NameServer_E_NOTFOUND)) {
                status = HeapBufMP_E_NOTFOUND;
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            else if (EXPECT_FALSE (status < 0)) {
                /* Override the status to return a HeapBufMP status code. */
                status = HeapBufMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapBufMP_open",
                                     status,
                                     "Failure in NameServer_get!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            else {
                /* Convert from shared region pointer to local address*/
                sharedAddr = SharedRegion_getPtr (sharedShmBase);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (sharedAddr == NULL)) {
                    status = HeapBufMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapBufMP_open",
                                         status,
                                         "Invalid pointer received from"
                                         "NameServer!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    status = HeapBufMP_openByAddr (sharedAddr, handlePtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (EXPECT_FALSE (status < 0)) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "HeapBufMP_open",
                                             status,
                                            "Failed to open HeapBufMP handle!");
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_open", status);

    return status;
}


/* Closes previously opened instance of HeapBufMP module. */
Int
HeapBufMP_close (HeapBufMP_Handle * handlePtr)
{
    Int                status = HeapBufMP_S_SUCCESS;
    HeapBufMP_Object * handle = NULL;
    HeapBufMP_Obj    * obj    = NULL;
    IArg               key    = 0;

    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = HeapBufMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_close",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_close",
                             status,
                             "The parameter handlePtr i.e. pointer to handle "
                             "passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_close",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (HeapBufMP_Object *) (*handlePtr);
        obj    = (HeapBufMP_Obj *) handle->obj;
        if (obj != NULL) {
            key = IGateProvider_enter (HeapBufMP_module->localLock);
            if (obj->owner.procId == MultiProc_self ()) {
                obj->owner.openCount--;
            }

            /* Check if HeapBufMP is opened on same processor and this is the
             * last closure.
             */
            if (    (obj->owner.creator   == FALSE)
                &&  (obj->owner.openCount == 0)) {
                List_remove ((List_Handle) &HeapBufMP_module->objList,
                             &obj->listElem);

                if (obj->freeList != NULL) {
                    /* Close the list */
                    ListMP_close ((ListMP_Handle *) &(obj->freeList));
                }

                if (obj->gate != NULL) {
                    GateMP_close ((GateMP_Handle *) &(obj->gate));
                }

                /* Now free the handle */
                Memory_free (NULL, obj, sizeof (HeapBufMP_Obj));
                obj = NULL;
                Memory_free (NULL, handle, sizeof (HeapBufMP_Object));
                *handlePtr = NULL;
            }
            IGateProvider_leave (HeapBufMP_module->localLock, key);
        }
        else {
            Memory_free (NULL, handle, sizeof (HeapBufMP_Object));
            *handlePtr = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_close", status);

    return status;
}


/* Allocs a block */
Void *
HeapBufMP_alloc (HeapBufMP_Handle handle,
                 SizeT            size,
                 SizeT            align)
{
    HeapBufMP_Obj    * obj    = NULL;
    Char           *   block  = NULL;
    IArg               key    = 0;

    GT_3trace (curTrace, GT_ENTER, "HeapBufMP_alloc", handle, size, align);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                                HeapBufMP_MAKE_MAGICSTAMP(0),
                                                HeapBufMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_alloc",
                             HeapBufMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_alloc",
                             HeapBufMP_E_INVALIDARG,
                             "Invalid NULL handle pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapBufMP_Obj *) ((HeapBufMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (size > obj->blockSize)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_alloc",
                                 HeapBufMP_E_INVALIDARG,
                                 "Invalid size is provided for allocation!");
        }
        else if (EXPECT_FALSE (   (obj->params.exact == TRUE)
                               && (size != obj->blockSize))) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_alloc",
                                 HeapBufMP_E_INVALIDARG,
                                 "Exact match is required, but the size is not"
                                 " an exact match!");
        }
        else if (EXPECT_FALSE (align > obj->align)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_alloc",
                                 HeapBufMP_E_INVALIDARG,
                                 "Alignment constraints specified cannot be"
                                 " met!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            key = GateMP_enter (obj->gate);

            block = ListMP_getHead ((ListMP_Handle) obj->freeList);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (block == NULL)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapBufMP_alloc",
                                     HeapBufMP_E_MEMORY,
                                     "Ran out of memory blocks!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /*
                 * Keep track of the min number of free for this HeapBufMP, if
                 * user has set the config variable trackMaxAllocs to true.
                 * Also, keep track of the number of free blocks.
                 *
                 * The min number of free blocks, 'minFreeBlocks', will be used
                 * to compute the "all time" maximum number of allocated blocks
                 * in getExtendedStats().
                 */
                if (EXPECT_FALSE (HeapBufMP_module->cfg.trackAllocs)) {
                    /* Make sure the attrs are not in cache */
                    if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
                        Cache_inv ((Ptr) obj->attrs,
                                   sizeof(HeapBufMP_Attrs),
                                   Cache_Type_ALL,
                                   TRUE);
                    }

                    obj->attrs->numFreeBlocks--;

                    if (    obj->attrs->numFreeBlocks
                        <   (Int32)obj->attrs->minFreeBlocks) {
                        /* save the new minimum */
                        obj->attrs->minFreeBlocks = obj->attrs->numFreeBlocks;
                    }

                    /* Make sure the attrs are written out to memory */
                    if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
                        Cache_wbInv ((Ptr) obj->attrs,
                                     sizeof (HeapBufMP_Attrs),
                                     Cache_Type_ALL,
                                     TRUE);
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_alloc", block);

    return block;
}


/* Frees a block */
Void
HeapBufMP_free (HeapBufMP_Handle   handle,
                Ptr                block,
                SizeT              size)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int             status = HeapBufMP_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    HeapBufMP_Obj * obj    = NULL;
    IArg            key    = 0;
    UInt32          minAlign;

    GT_3trace (curTrace, GT_ENTER, "HeapBufMP_free", handle, block, size);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (block != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                                HeapBufMP_MAKE_MAGICSTAMP(0),
                                                HeapBufMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_free",
                             HeapBufMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_free",
                             HeapBufMP_E_INVALIDARG,
                             "Invalid NULL handle pointer specified!");
    }
    else if (EXPECT_FALSE (block == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_free",
                             HeapBufMP_E_INVALIDARG,
                             "Invalid NULL block pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapBufMP_Obj *) ((HeapBufMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        minAlign = Memory_getMaxDefaultTypeAlign ();
        if (SharedRegion_getCacheLineSize (obj->regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize (obj->regionId);
        }
        GT_assert (curTrace, ((UInt32)block % minAlign == 0));

        key = GateMP_enter (obj->gate);

        /* Put the block in free list */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ListMP_putTail (obj->freeList, block);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (status < 0)) {
            status = HeapBufMP_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_free",
                                 status,
                                 "ListMP_putTail failed!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (EXPECT_FALSE (HeapBufMP_module->cfg.trackAllocs)) {
            /* Make sure the attrs are not in cache */
            if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
                Cache_inv ((Ptr) obj->attrs,
                           sizeof(HeapBufMP_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }

            obj->attrs->numFreeBlocks++;

            /* Make sure the attrs are written out to memory */
            if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
                Cache_wbInv ((Ptr) obj->attrs,
                             sizeof (HeapBufMP_Attrs),
                             Cache_Type_ALL,
                             TRUE);
            }
        }
        /*
         *  Invalidate entire block to make sure stale cache data isn't
         *  evicted later
         */
        if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
            Cache_inv((Ptr)block, obj->attrs->blockSize, Cache_Type_ALL, TRUE);
        }


        GateMP_leave (obj->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapBufMP_free");
}


/* Get memory statistics */
Void
HeapBufMP_getStats (HeapBufMP_Handle  handle,
                    Ptr               stats)
{
    HeapBufMP_Obj    * obj    = NULL;
    Memory_Stats     * memStats = NULL;
    IArg               key    = 0;
    SizeT              blockSize;

    GT_2trace (curTrace, GT_ENTER, "HeapBufMP_getStats", handle, stats);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (stats != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                                HeapBufMP_MAKE_MAGICSTAMP(0),
                                                HeapBufMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getStats",
                             HeapBufMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getStats",
                             HeapBufMP_E_INVALIDARG,
                             "handle passed is null!");
    }
    else if (EXPECT_FALSE (stats == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getStats",
                             HeapBufMP_E_INVALIDARG,
                             "Invalid NULL stats pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (HeapBufMP_Obj *) ((HeapBufMP_Object *) handle)->obj;
        GT_assert (curTrace, (obj != NULL));

        memStats = (Memory_Stats *) stats;

        /* This is read-only, so do not need cache management */
        blockSize = obj->attrs->blockSize;

        if (EXPECT_FALSE (HeapBufMP_module->cfg.trackAllocs)) {
            /*
             * Protect this section so that numFreeBlocks doesn't change
             * between totalFreeSize and largestFreeSize
             */
            key = GateMP_enter(obj->gate);

            /* Make sure the attrs are not in cache */
            if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
                Cache_inv ((Ptr) obj->attrs,
                           sizeof (HeapBufMP_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }

            memStats->totalFreeSize = blockSize * obj->attrs->numFreeBlocks;
            memStats->largestFreeSize = (obj->attrs->numFreeBlocks > 0) ?
                                                            blockSize : 0;

            GateMP_leave(obj->gate, key);
        }
        else {
            /* Tracking disabled */
            memStats->totalFreeSize    = 0;
            memStats->largestFreeSize  = 0;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapBufMP_getStats");
}


/* Indicate whether the heap may block during an alloc or free call */
Bool
HeapBufMP_isBlocking (HeapBufMP_Handle handle)
{
    Bool isBlocking = FALSE;

    GT_1trace (curTrace, GT_ENTER, "Heap_isBlocking", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_isBlocking",
                             HeapBufMP_E_INVALIDARG,
                             "handle passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* TBD: Figure out how to determine whether the gate is blocking */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_isBlocking", isBlocking);

    return isBlocking;
}


/* Get extended statistics */
Void
HeapBufMP_getExtendedStats (HeapBufMP_Handle          handle,
                            HeapBufMP_ExtendedStats * stats)
{
    HeapBufMP_Object * object = NULL;
    HeapBufMP_Obj    * obj    = NULL;
    IArg               key    = 0;

    GT_2trace (curTrace, GT_ENTER, "HeapBufMP_getExtendedStats", handle, stats);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (stats != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                                HeapBufMP_MAKE_MAGICSTAMP(0),
                                                HeapBufMP_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getExtendedStats",
                             HeapBufMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (handle == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getExtendedStats",
                             HeapBufMP_E_INVALIDARG,
                             "Invalid NULL handle pointer specified!");
    }
    else if (EXPECT_FALSE (stats == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getExtendedStats",
                             HeapBufMP_E_INVALIDARG,
                             "Invalid NULL stats pointer specified!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (HeapBufMP_Object *) (handle);
        obj    = (HeapBufMP_Obj *) object->obj;

        /* Make sure the attrs are not in cache */
        if (EXPECT_FALSE (obj->cacheEnabled == TRUE)) {
            Cache_inv ((Ptr) obj->attrs,
                       sizeof (HeapBufMP_Attrs),
                       Cache_Type_ALL,
                       TRUE);
        }

        /*
         *  The maximum number of allocations for this HeapBufMP (for any given
         *  instance of time during its liftime) is computed as follows:
         *
         *  maxAllocatedBlocks = obj->numBlocks - obj->minFreeBlocks
         *
         *  Note that maxAllocatedBlocks is *not* the maximum allocation count, but
         *  rather the maximum allocations seen at any snapshot of time in the
         *  HeapBufMP instance.
         */
        key = GateMP_enter (obj->gate);
        /* if nothing has been alloc'ed yet, return 0 */
        if ((Int) (obj->attrs->minFreeBlocks) == -1) {
            stats->maxAllocatedBlocks = 0;
        }
        else {
            stats->maxAllocatedBlocks =   obj->attrs->numBlocks
                                        - obj->attrs->minFreeBlocks;
        }

        /* current # of alloc'ed blocks is computed using curr # of free blocks */
        stats->numAllocatedBlocks =   obj->attrs->numBlocks
                                    - obj->attrs->numFreeBlocks;

        GateMP_leave(obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "HeapBufMP_getExtendedStats");
}


/* Returns the shared memory size requirement for a single instance. */
SizeT
HeapBufMP_sharedMemReq (const HeapBufMP_Params * params)
{
    SizeT         memReq = 0;
    ListMP_Params listMPParams;
    UInt32        minAlign;
    UInt32        bufAlign;
    UInt32        blockSize;
    UInt16        regionId;

    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_sharedMemReq", params);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (params == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_sharedMemReq",
                             HeapBufMP_E_INVALIDARG,
                             "Params pointer passed is NULL!");
    }
    else if (EXPECT_FALSE (params->blockSize == 0)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_sharedMemReq",
                             HeapBufMP_E_INVALIDARG,
                             "Block size passed is NULL!");
    }
    else if (EXPECT_FALSE (params->numBlocks == 0)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_sharedMemReq",
                             HeapBufMP_E_INVALIDARG,
                             "Number of blocks passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Size for attrs */
        if (params->sharedAddr == NULL) {
            regionId = params->regionId;
        }
        else {
            regionId = SharedRegion_getId (params->sharedAddr);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionId == SharedRegion_INVALIDREGIONID)  {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_sharedMemReq",
                                 HeapBufMP_E_FAIL,
                                 "params->sharedAddr is not in a"
                                 " valid SharedRegion!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Determine the actual buffer alignment */
            bufAlign = params->align;

            minAlign = Memory_getMaxDefaultTypeAlign ();
            if (SharedRegion_getCacheLineSize (regionId) > minAlign) {
                minAlign = SharedRegion_getCacheLineSize (regionId);
            }

            if (bufAlign < minAlign) {
                bufAlign = minAlign;
            }

            /* Determine the actual block size */
            blockSize = _Ipc_roundup (params->blockSize, bufAlign);

            /* Add size of HeapBufMP Attrs */
            memReq = _Ipc_roundup (sizeof (HeapBufMP_Attrs), minAlign);

            /*
             *  Add size of ListMP Attrs.  No need to init params since it's
             *  not used to create.
             */
            ListMP_Params_init (&listMPParams);
            listMPParams.regionId = regionId;
            memReq += ListMP_sharedMemReq (&listMPParams);

            /* Round by the buffer alignment */
            memReq = _Ipc_roundup (memReq, bufAlign);

            /*
             *  Add the buffer size. No need to subsequently round because the
             *  product should be a multiple of cacheLineSize if cache alignment
             *  is enabled
             */
            memReq += (blockSize * params->numBlocks);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_sharedMemReq", memReq);

    return memReq;
}


/* Returns the HeapBufMP kernel object pointer. */
Ptr
HeapBufMP_getKnlHandle (HeapBufMP_Handle handle)
{
    GT_1trace (curTrace, GT_ENTER, "HeapBufMP_getKnlHandle", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NULL Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getKnlHandle",
                             HeapBufMP_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval NULL handle passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_getKnlHandle",
                             HeapBufMP_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Nothing to be done for kernel-side implementation. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_getKnlHandle", handle);

    /*! @retval Kernel-Object-handle Operation successfully completed. */
    return (handle);
}


/* HeapBufMP open by address */
Int
HeapBufMP_openByAddr (Ptr                sharedAddr,
                      HeapBufMP_Handle * handlePtr)
{
    Int                status = HeapBufMP_S_SUCCESS;
    Bool               doneFlag = FALSE;
    HeapBufMP_Attrs *  attrs = NULL;
    List_Elem *        elem  = NULL;
    IArg               key;
    UInt16             id;
    HeapBufMP_Params   params;

    GT_2trace (curTrace, GT_ENTER, "HeapBufMP_openByAddr",
               sharedAddr, handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(HeapBufMP_module->refCount),
                                  HeapBufMP_MAKE_MAGICSTAMP(0),
                                  HeapBufMP_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = HeapBufMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_openByAddr",
                             status,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = HeapBufMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMP_openByAddr",
                             status,
                             "handlePtr pointer passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* First check in the local list */
        List_traverse (elem, (List_Handle) &HeapBufMP_module->objList) {
            if (((HeapBufMP_Obj *)elem)->params.sharedAddr == sharedAddr) {
                key = IGateProvider_enter (HeapBufMP_module->localLock);
                if (   ((HeapBufMP_Obj *)elem)->owner.procId
                    == MultiProc_self ()) {
                    ((HeapBufMP_Obj *)elem)->owner.openCount++;
                }
                IGateProvider_leave (HeapBufMP_module->localLock, key);
                *handlePtr = (((HeapBufMP_Obj *)elem)->top);
                doneFlag = TRUE;
                break;
            }
        }

        /* If not already existing locally, create object locally for open. */
        if (EXPECT_FALSE (doneFlag == FALSE)) {
            HeapBufMP_Params_init(&params);
            params.sharedAddr = sharedAddr;
            attrs = (HeapBufMP_Attrs *) sharedAddr;
            id = SharedRegion_getId (sharedAddr);

            if (EXPECT_FALSE (SharedRegion_isCacheEnabled (id))) {
                Cache_inv (attrs,
                           sizeof (HeapBufMP_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }

            if (EXPECT_FALSE (attrs->status != HeapBufMP_CREATED)) {
                *handlePtr = NULL;
                status = HeapBufMP_E_NOTFOUND;
                /* Don't set failure reason since this is an expected
                 * run-time failure.
                 */
                GT_1trace (curTrace,
                           GT_3CLASS,
                           "HeapBufMP Instance is not created yet at the "
                           "provided shared addr.\n"
                           "    sharedAddr [0x%x]!",
                           sharedAddr);
            }
            else {
                status = _HeapBufMP_create (handlePtr, &params, FALSE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapBufMP_openByAddr",
                                         status,
                                         "_HeapBufMP_create failed!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_openByAddr", status);

    return (status);
}



/* =============================================================================
 * Internal function
 * =============================================================================
 */
GateMP_Handle HeapBufMP_getGate(HeapBufMP_Handle handle)
{
    HeapBufMP_Object *  object;
    HeapBufMP_Obj    *  obj;

    object = (HeapBufMP_Object *)handle;
    obj = (HeapBufMP_Obj *)(object->obj);
    return(obj->gate);
}

/* Creates a new instance of HeapBufMP module.
 * This is an internal function as both HeapBufMP_create
 * and HeapBufMP_open use the functionality.
 */
Int _HeapBufMP_create (      HeapBufMP_Handle *  handlePtr,
                       const HeapBufMP_Params *  params,
                             Bool                createFlag)
{
    Int                 status = HeapBufMP_S_SUCCESS;
    HeapBufMP_Obj     * obj    = NULL;
    HeapBufMP_Object  * handle = NULL;
    Ptr                 localAddr  = NULL;
    GateMP_Handle       gateHandle = NULL;
    IArg                key;
    SharedRegion_SRPtr  sharedShmBase;
    UInt32              minAlign;

    GT_3trace (curTrace, GT_ENTER, "_HeapBufMP_create",
               handlePtr, params, createFlag);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (params != NULL));

    /* No need for parameter checks, since this is an internal function. */

    /* Create the generic handle */
    handle = (HeapBufMP_Object *) Memory_calloc (NULL,
                                                 sizeof (HeapBufMP_Object),
                                                 0u,
                                                 NULL);
    *handlePtr = (HeapBufMP_Handle) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval HeapBufMP_E_MEMORY Memory allocation failed for pointer of
         *          type HeapBufMP_Object
         */
        status = HeapBufMP_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_HeapBufMP_create",
                             status,
                             "Memory allocation failed for pointer"
                             " of type HeapBufMP_Object!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Create the Heap Buf specific handle */
        obj = (HeapBufMP_Obj *) Memory_calloc (NULL,
                                               sizeof (HeapBufMP_Obj),
                                               0u,
                                               NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj == NULL) {
            /*! @retval HeapBufMP_E_MEMORY Memory allocation failed for pointer
             *          of type HeapBufMP_Obj
             */
            status = HeapBufMP_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_HeapBufMP_create",
                                 status,
                                 "Memory allocation failed for pointer"
                                 " of type HeapBufMP_Obj");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            handle->obj = (HeapBufMP_Obj *) obj ;
            handle->alloc = (IHeap_allocFxn) &HeapBufMP_alloc;
            handle->free  = (IHeap_freeFxn) &HeapBufMP_free;
            handle->getStats     = (IHeap_getStatsFxn) &HeapBufMP_getStats;
            handle->getKnlHandle = (IHeap_getKnlHandleFxn)
                                                    &HeapBufMP_getKnlHandle;
            handle->isBlocking   = (IHeap_isBlockingFxn) &HeapBufMP_isBlocking;

            obj->nsKey     = NULL;
            obj->allocSize = 0;

            /* Put in the local list */
            key = IGateProvider_enter (HeapBufMP_module->localLock);
            List_elemClear (&obj->listElem);
            List_put ((List_Handle) &HeapBufMP_module->objList, &obj->listElem);
            IGateProvider_leave (HeapBufMP_module->localLock, key);

            if (createFlag == FALSE) {
                obj->owner.creator = FALSE;
                obj->owner.openCount = 0u;
                obj->owner.procId = MultiProc_INVALIDID;
                obj->top = handle;

                obj->attrs = (HeapBufMP_Attrs *) params->sharedAddr;

                /* No need to Cache_inv attrs- already done in openByAddr() */
                obj->align          = obj->attrs->align;
                obj->numBlocks      = obj->attrs->numBlocks;
                obj->blockSize      = obj->attrs->blockSize;
                obj->exact          = obj->attrs->exact;
                obj->buf            = SharedRegion_getPtr (obj->attrs->bufPtr);
                obj->regionId       = SharedRegion_getId (obj->buf);

                minAlign = Memory_getMaxDefaultTypeAlign ();
                if (SharedRegion_getCacheLineSize (obj->regionId) > minAlign) {
                    minAlign = SharedRegion_getCacheLineSize (obj->regionId);
                }

                obj->cacheEnabled = SharedRegion_isCacheEnabled (obj->regionId);

                localAddr = SharedRegion_getPtr (obj->attrs->gateMPAddr);
                status = GateMP_openByAddr (localAddr, &gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* Override the status with a HeapBufMP status code. */
                    status = HeapBufMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_HeapBufMP_create",
                                         status,
                                         "Failed to open GateMP!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj->gate = gateHandle;

                    /* Open the ListMP */
                    localAddr = (Ptr) _Ipc_roundup (( (UInt32) obj->attrs
                                                 + sizeof (HeapBufMP_Attrs)),
                                                 minAlign);
                    status = ListMP_openByAddr(localAddr, &(obj->freeList));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_HeapBufMP_create",
                                             status,
                                             "Failed to open ListMP!");
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else {
                /* Creating the HeapBufMP ... */
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
                    status = HeapBufMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_HeapBufMP_create",
                                         status,
                                         "GateMP is NULL!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    obj->exact     = params->exact;
                    obj->align     = params->align;
                    obj->numBlocks = params->numBlocks;

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
                        if (obj->regionId == SharedRegion_INVALIDREGIONID)  {
                            status = HeapBufMP_E_FAIL;
                            /*! @retval HeapBufMP_E_FAIL params->sharedAddr is
                                              not in a valid SharedRegion. */
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_HeapBufMP_create",
                                                 status,
                                                 "params->sharedAddr is not in"
                                                 " a valid SharedRegion!");
                        }
                        else if (  ((UInt32) params->sharedAddr
                                 % SharedRegion_getCacheLineSize (obj->regionId)
                                 != 0)) {
                            status = HeapBufMP_E_FAIL;
                            /*! @retval HeapBufMP_E_FAIL params->sharedAddr does
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
                            obj->attrs = (HeapBufMP_Attrs *) params->sharedAddr;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        obj->cacheEnabled = SharedRegion_isCacheEnabled (
                                                                obj->regionId);

                        /* Fix the alignment (alignment may be needed even if
                         * cache is disabled)
                         */
                        obj->align = Memory_getMaxDefaultTypeAlign ();
                        if (   SharedRegion_getCacheLineSize (obj->regionId)
                            >  obj->align) {
                            obj->align = SharedRegion_getCacheLineSize (
                                                                obj->regionId);
                        }

                        /* Round the blockSize up by the adjusted alignment */
                        obj->blockSize = _Ipc_roundup (params->blockSize,
                                                  obj->align);

                        status = HeapBufMP_postInit (handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (EXPECT_FALSE (status < 0)) {
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_HeapBufMP_create",
                                                 status,
                                                 "HeapBufMP_postInit failed!");
                        }
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Populate the params member */
                    Memory_copy ((Ptr) &obj->params,
                                 (Ptr) params,
                                 sizeof (HeapBufMP_Params));

                    /* Copy the name */
                    if (params->name != NULL) {
                        obj->params.name = (String) Memory_alloc (NULL,
                                            (String_len (params->name)+ 1u),
                                            0,
                                            NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (obj->params.name == NULL) {
                            /*! @retval HeapBufMP_E_MEMORY Memory allocation
                             *          failed for name
                             */
                            status = HeapBufMP_E_MEMORY;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "_HeapBufMP_create",
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
                                               HeapBufMP_module->nameServer,
                                               params->name,
                                               (UInt32) sharedShmBase);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (obj->nsKey == NULL) {
                            /*! @retval HeapBufMP_E_FAIL Failed to add to
                             *                         NameServer */
                            status = HeapBufMP_E_FAIL;
                            GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "HeapBufMP_create",
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
            HeapBufMP_delete ((HeapBufMP_Handle *) handlePtr);
        }
        else {
            HeapBufMP_close ((HeapBufMP_Handle *) handlePtr);
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_HeapBufMP_create", status);

    /*! @retval HeapBufMP_S_SUCCESS Operation successful*/
    return status;
}


/*
 *                          Shared memory Layout:
 *
 *          sharedAddr   -> ---------------------------
 *                         |  HeapBufMP_Attrs        |
 *                         |  (minAlign PADDING)     |
 *                         |-------------------------|
 *                         |  ListMP shared instance |
 *                         |  (bufAlign PADDING)     |
 *                         |-------------------------|
 *                         |  HeapBufMP BUFFER       |
 *                         |-------------------------|
 */


/*
 *  Slice and dice the buffer up into the correct size blocks and
 *  add to the freelist.
 */
Int HeapBufMP_postInit (HeapBufMP_Object * handle)
{
    Int              status     = HeapBufMP_S_SUCCESS;
    Char *           buf        = NULL;
    HeapBufMP_Handle regionHeap = NULL;
    HeapBufMP_Obj *  obj        = NULL;
    UInt             i;
    UInt32           minAlign;
    HeapBufMP_Params heapParams;
    ListMP_Params    listMPParams;

    GT_0trace (curTrace, GT_ENTER, "HeapBufMP_postInit");

    GT_assert (curTrace, (handle != NULL));

    obj = (HeapBufMP_Obj *) handle->obj;

    minAlign = Memory_getMaxDefaultTypeAlign ();
    if (SharedRegion_getCacheLineSize (obj->regionId) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize (obj->regionId);
    }

    if (obj->attrs == NULL) {
        HeapBufMP_Params_init (&heapParams);
        heapParams.regionId     = obj->regionId;
        heapParams.numBlocks    = obj->numBlocks;
        heapParams.align        = obj->align;
        heapParams.blockSize    = obj->blockSize;
        obj->allocSize = HeapBufMP_sharedMemReq (&heapParams);

        regionHeap = SharedRegion_getHeap (obj->regionId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (regionHeap == NULL) {
            status = HeapBufMP_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_postInit",
                                 status,
                                 "Shared Region heap is null!");

        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->attrs = Memory_alloc ((IHeap_Handle) regionHeap,
                                       obj->allocSize,
                                       minAlign,
                                       NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (obj->attrs == NULL) {
                status = HeapBufMP_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapBufMP_postInit",
                                     status,
                                     "Failed to allocate shared memory!");
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Store the GateMP sharedAddr in the HeapBuf Attrs */
        obj->attrs->gateMPAddr = GateMP_getSharedAddr (obj->gate);

        /* Create the freeList */
        ListMP_Params_init (&listMPParams);
        listMPParams.sharedAddr = (Ptr) _Ipc_roundup ( ((UInt32) obj->attrs
                                                  + sizeof (HeapBufMP_Attrs)),
                                                  minAlign);
        listMPParams.gate = (GateMP_Handle) obj->gate;
        obj->freeList = ListMP_create (&listMPParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->freeList == NULL) {
            status = HeapBufMP_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMP_postInit",
                                 status,
                                 "ListMP_create failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* obj->buf will get alignment-adjusted in postInit */
            obj->buf = (Ptr) (  (SizeT) listMPParams.sharedAddr
                              + ListMP_sharedMemReq (&listMPParams));

            /* Round obj->buf up by obj->align */
            buf = obj->buf = (Ptr) _Ipc_roundup (obj->buf, obj->align);

            obj->attrs->numFreeBlocks = obj->numBlocks;
            obj->attrs->minFreeBlocks = (UInt) -1;
            obj->attrs->blockSize     = obj->blockSize;
            obj->attrs->align         = obj->align;
            obj->attrs->numBlocks     = obj->numBlocks;
            obj->attrs->exact         = obj->exact ? 1 : 0;

            /* Put a SRPtr in attrs */
            obj->attrs->bufPtr = SharedRegion_getSRPtr (obj->buf,
                                                        obj->regionId);
            GT_assert (curTrace,
                       (obj->attrs->bufPtr != SharedRegion_INVALIDSRPTR));

            /*
             * Split the buffer into blocks that are length "blockSize" and
             * add into the freeList Queue.
             */
            for (i = 0; i < obj->numBlocks; i++) {
                /* Add the block to the freeList */
                status = ListMP_putTail (obj->freeList, (ListMP_Elem *) buf);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    status = HeapBufMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "HeapBufMP_postInit",
                                         status,
                                         "ListMP_putTail failed!");
                    break;
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                buf = (Char *) ((UInt32) buf + obj->blockSize);
            }

            /* Last thing, set the status */
            obj->attrs->status = HeapBufMP_CREATED;

            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr) obj->attrs,
                             sizeof (HeapBufMP_Attrs),
                             Cache_Type_ALL,
                             TRUE);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMP_postInit", status);

    return (status);
}
