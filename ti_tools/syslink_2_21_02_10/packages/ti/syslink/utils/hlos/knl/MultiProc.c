/*
 *  @file   MultiProc.c
 *
 *  @brief      Handles processor id management in multi processor systems.Used
 *              by all modules which need processor ids for their oprations.
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

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/inc/Bitops.h>

/* Module level headers */
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>

#include <ti/syslink/inc/_Notify.h>    /* for auto-setting of USE_SYSLINK_NOTIFY */

#if !defined (USE_SYSLINK_NOTIFY)
#include <syslink/multiproc.h>
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define MultiProc_MAKE_MAGICSTAMP(x) ((MultiProc_MODULEID << 12u) | (x))

/*!
 *  @brief  MultiProc Module state object
 */
typedef struct MultiProc_ModuleObject_tag {
    MultiProc_Config cfg;
    /*!< Notify configuration structure */
    MultiProc_Config defCfg;
    /*!< Default module configuration */
    Atomic           refCount;
    /* Reference count */
    UInt16           id;
    /* Local processor ID */
} MultiProc_ModuleObject;


/* =============================================================================
 *  Extern declarations
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
MultiProc_ModuleObject MultiProc_state = {
    .defCfg.numProcessors     = 1u,
    .defCfg.id                = 0u,
    .id                       = MultiProc_INVALIDID
};

/*!
 *  @var    MultiProc_module
 *
 *  @brief  Pointer to the MultiProc module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
MultiProc_ModuleObject * MultiProc_module = &MultiProc_state;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Get the default configuration for the MultiProc module. */
Void
MultiProc_getConfig (MultiProc_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "MultiProc_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if defined (USE_SYSLINK_NOTIFY)

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (cfg == NULL)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_getConfig",
                             MultiProc_E_INVALIDARG,
                             "Argument of type (MultiProc_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_TRUE (  Atomic_cmpmask_and_lt (&(MultiProc_module->refCount),
                                                  MultiProc_MAKE_MAGICSTAMP(0),
                                                  MultiProc_MAKE_MAGICSTAMP(1))
                        == TRUE)) {
            /* (If setup has not yet been called) */
            Memory_copy (cfg,
                         &MultiProc_module->defCfg,
                         sizeof (MultiProc_Config));
        }
        else {
            Memory_copy (cfg,
                         &MultiProc_module->cfg,
                         sizeof (MultiProc_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    multiproc_get_config((Ptr)cfg);
#endif

    GT_0trace (curTrace, GT_LEAVE, "MultiProc_getConfig");
}


/* Setup the MultiProc module. */
Int
MultiProc_setup (MultiProc_Config * cfg)
{
    Int              status = MultiProc_S_SUCCESS;
#if defined (USE_SYSLINK_NOTIFY)
    Bool            duplicateNameFound = FALSE;
    MultiProc_Config tmpCfg;
    UInt16         i,j;
#endif

    GT_1trace (curTrace, GT_ENTER, "MultiProc_setup", cfg);

#if defined (USE_SYSLINK_NOTIFY)

    if (cfg == NULL) {
        MultiProc_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&MultiProc_module->refCount,
                            MultiProc_MAKE_MAGICSTAMP(0),
                            MultiProc_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&MultiProc_module->refCount)
        != MultiProc_MAKE_MAGICSTAMP(1u)) {
        status = MultiProc_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "MultiProc Module already initialized!");
    }
    else {
        for (i = 0; i < cfg->numProcessors; i++) {
            for(j = i + 1; j < cfg->numProcessors; j++) {
                 if(String_cmp (cfg->nameList[i], cfg->nameList[j]) == 0) {
                      duplicateNameFound = TRUE;
                      break;
                 }
            }
            if(duplicateNameFound)
            {
                status = MultiProc_E_FAIL;
                GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_setup",
                             status,
                             "Duplicate proc name found in the name list!");
                 break;
            }
        }
        if(status >= 0) {
            Memory_copy (&MultiProc_module->cfg, cfg, sizeof (MultiProc_Config));
            MultiProc_module->id = cfg->id;
        }
    }

#else
    status = multiproc_setup((Ptr)cfg);
#endif

    GT_1trace (curTrace, GT_LEAVE, "MultiProc_setup", status);

    /*! @retval MultiProc_S_SUCCESS Operation successful */
    return (status);
}


/* Destroy the MultiProc module. */
Int
MultiProc_destroy (Void)
{
    Int status = MultiProc_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "MultiProc_destroy");

#if defined (USE_SYSLINK_NOTIFY)

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MultiProc_module->refCount),
                                  MultiProc_MAKE_MAGICSTAMP(0),
                                  MultiProc_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MultiProc_E_INVALIDSTATE Module was not initialized */
        status = MultiProc_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Atomic_dec_return (&MultiProc_module->refCount);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = multiproc_destroy();
#endif
    GT_1trace (curTrace, GT_LEAVE, "MultiProc_destroy", status);

    /*! @retval MultiProc_S_SUCCESS Operation successful */
    return (status);
}


/* Function to set Local processor Id */
Int
MultiProc_setLocalId (UInt16 id)
{
    Int status = MultiProc_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "MultiProc_setLocalId", id);

#if defined (USE_SYSLINK_NOTIFY)

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MultiProc_module->refCount),
                                                MultiProc_MAKE_MAGICSTAMP(0),
                                                MultiProc_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        status = MultiProc_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_setLocalId",
                             status,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (id >= MultiProc_module->cfg.numProcessors)) {
        status = MultiProc_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_setLocalId",
                             MultiProc_E_INVALIDARG,
                             "ID cannot be greater than or equal to"
                             " numProcessors");
    }
    else if (EXPECT_FALSE (MultiProc_module->id != MultiProc_INVALIDID)) {
        status = MultiProc_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_setLocalId",
                             MultiProc_E_INVALIDARG,
                             "Cannot change the ID after module"
                             " startup");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        MultiProc_module->id = id;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    status = multiproc_set_local_id(id);
#endif

    GT_1trace (curTrace, GT_LEAVE, "MultiProc_setLocalId", status);

    return (status);
}


/* Function to get proccesor Id from proccessor name. */
UInt16
MultiProc_getId (String name)
{

#if defined (USE_SYSLINK_NOTIFY)
    UInt32 id    = MultiProc_INVALIDID;
    Bool   found = FALSE;
    Int    i;
#else
    UInt32 id    = -1;
#endif

    GT_1trace (curTrace, GT_ENTER, "MultiProc_getId", name);

#if defined (USE_SYSLINK_NOTIFY)

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MultiProc_module->refCount),
                                                 MultiProc_MAKE_MAGICSTAMP(0),
                                                 MultiProc_MAKE_MAGICSTAMP(1))
                       == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_getId",
                             MultiProc_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* If the name is NULL, simply return the local id */
        if (EXPECT_FALSE (name == NULL)) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
             GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "MultiProc_getId",
                              MultiProc_E_FAIL,
                              "Invalid Proc Name passed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else {
            for (i = 0; i < MultiProc_module->cfg.numProcessors ; i++) {
                if (   String_cmp (name, &MultiProc_module->cfg.nameList [i][0])
                    == 0) {
                    id = i;
                    found = TRUE;
                    break;
                }
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (!found)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MultiProc_getId",
                                     MultiProc_E_NOTFOUND,
                                     "Processor name not found");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_assert (curTrace, (found == TRUE));

#else
    id = multiproc_get_id((const char*)name);
#endif

    GT_1trace (curTrace, GT_LEAVE, "MultiProc_getId", id);

    return (id);
}


/* Function to get name from processor id. */
String
MultiProc_getName (UInt16 id)
{
    String name = NULL;

    GT_1trace (curTrace, GT_ENTER, "MultiProc_getName", id);

#if defined (USE_SYSLINK_NOTIFY)
    GT_assert (curTrace, (id < MultiProc_module->cfg.numProcessors));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MultiProc_module->refCount),
                                                MultiProc_MAKE_MAGICSTAMP(0),
                                                MultiProc_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_getName",
                             MultiProc_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (id >= MultiProc_module->cfg.numProcessors)) {
        GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "MultiProc_getName",
                    MultiProc_E_INVALIDARG,
                    "Processor id is not less than numProcessors");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        name = MultiProc_module->cfg.nameList [id];
        GT_1trace (curTrace, GT_1CLASS, "MultiProc_getName [%s]", name);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    name = (String)multiproc_get_name(id);
#endif

    GT_0trace (curTrace, GT_LEAVE, "MultiProc_getName");

    return (name);
}


/* Function to get maximum proc Id in the system. */
UInt16
MultiProc_getNumProcessors (Void)
{
    GT_0trace (curTrace, GT_ENTER, "MultiProc_getNumProcessors");

#if defined (USE_SYSLINK_NOTIFY)

    GT_1trace (curTrace,
               GT_LEAVE,
               "MultiProc_getNumProcessors",
               MultiProc_module->cfg.numProcessors);

    /* Don't put any checks here, since this needs to be very fast. */

    return (MultiProc_module->cfg.numProcessors);

#else
    return (multiproc_get_num_processors());
#endif

}


/* Return Id of current processor */
UInt16
MultiProc_self (Void)
{
    GT_0trace (curTrace, GT_ENTER, "MultiProc_self");

#if defined (USE_SYSLINK_NOTIFY)

    GT_1trace (curTrace, GT_LEAVE, "MultiProc_self", MultiProc_module->id);

    /* Don't put any checks here, since this needs to be very fast. */

    return (MultiProc_module->id);

#else
    return (multiproc_self());
#endif

}


/* Determines the offset for any two processors. */
UInt
MultiProc_getSlot (UInt16 remoteProcId)
{
    UInt slot = 0u;
#if defined (USE_SYSLINK_NOTIFY)
    UInt i;
    UInt j;
    UInt smallId;
    UInt largeId;
#endif

    GT_1trace (curTrace, GT_ENTER, "MultiProc_getSlot", remoteProcId);

#if defined (USE_SYSLINK_NOTIFY)

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (   Atomic_cmpmask_and_lt (&(MultiProc_module->refCount),
                                                MultiProc_MAKE_MAGICSTAMP(0),
                                                MultiProc_MAKE_MAGICSTAMP(1))
                      == TRUE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProc_getSlot",
                             MultiProc_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (EXPECT_FALSE (   remoteProcId
                           >= MultiProc_module->cfg.numProcessors)) {
        GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "MultiProc_getSlot",
                    MultiProc_E_INVALIDARG,
                    "Processor id is not less than numProcessors");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (remoteProcId > MultiProc_self ()) {
            smallId = MultiProc_self ();
            largeId = remoteProcId;
        }
        else {
            largeId = MultiProc_self ();
            smallId = remoteProcId;
        }

        /* determine what offset to create for the remote Proc Id */
        for (i = 0; i < MultiProc_module->cfg.numProcessors; i++) {
            for (j = i + 1; j < MultiProc_module->cfg.numProcessors; j++) {
                if ((smallId == i) && (largeId == j)) {
                    break;
                }
                slot++;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#else
    slot = multiproc_get_slot(remoteProcId);
#endif

    GT_1trace (curTrace, GT_LEAVE, "MultiProc_getSlot", slot);

    return (slot);
}
