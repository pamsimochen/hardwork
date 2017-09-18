/*
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
 */

/**
 *  @file   ti/syslink/family/hlos/knl/omap3530/omap3530dsp/omap3530proc.c
 *
 *  @brief   Processor implementation for OMAP3530.
 *
 *           This module is responsible for taking care of device-specific
 *           operations for the processor.
 *           The implementation is specific to OMAP3530.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Cfg.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>

/* Module headers */
#include <ti/syslink/inc/knl/ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>
#include <ti/syslink/inc/knl/omap3530proc.h>
#include <ti/syslink/inc/knl/_omap3530proc.h>
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu_defs.h>
#include <ti/syslink/inc/knl/omap3530_hal.h>
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu.h>
#include <ti/syslink/inc/knl/omap3530_hal_reset.h>
#include <ti/syslink/inc/knl/omap3530_hal_boot.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

/* Macro to make a correct module magic number with refCount */
#define OMAP3530PROC_MAKE_MAGICSTAMP(x)  ((OMAP3530PROC_MODULEID << 12u) | (x))

/*!
 *  @brief  DDR2 start address.
 */
#define DDR2_START                                      0x80000000

/*!
 *  @brief  DDR2 end address.
 */
#define DDR2_END                                        0x88000000

/*!
 *  @brief  Number of static entries in address translation table.
 */
#define AddrTable_STATIC_COUNT 5

/*!
 *  @brief  Max entries in address translation table.
 */
#define AddrTable_SIZE 32

/* config param for ds mmu */
#define PARAMS_mmuEnable "ProcMgr.proc[DSP].mmuEnable="


/*!
 *  @brief  OMAP3530PROC Module state object
 */
typedef struct OMAP3530PROC_ModuleObject_tag {
    UInt32                   configSize;
    /*!< Size of configuration structure */
    OMAP3530PROC_Config      cfg;
    /*!< OMAP3530PROC configuration structure */
    OMAP3530PROC_Config      defCfg;
    /*!< Default module configuration */
    OMAP3530PROC_Params      defInstParams;
    /*!< Default parameters for the OMAP3530PROC instances */
    Bool                     isSetup;
    /*!< Indicates whether the OMAP3530PROC module is setup. */
    OMAP3530PROC_Handle      procHandles [MultiProc_MAXPROCESSORS];
    /*!< Processor handle array. */
    IGateProvider_Handle     gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    Atomic                   refCount;
    /* Reference count */
} OMAP3530PROC_ModuleObject;

/*!
 *  @brief  OMAP3530PROC instance object.
 */
struct OMAP3530PROC_Object_tag {
    OMAP3530PROC_Params params;
    /*!< Instance parameters (configuration values) */
    Ptr                 halObject;
    /*!< Pointer to the Hardware Abstraction Layer object. */
    List_Object         mappedMemEntries;
    /*!< List of mapped entries */
};

/* Defines the OMAP3530PROC object type. */
typedef struct OMAP3530PROC_Object_tag OMAP3530PROC_Object;


/* number of active entries in address translation table */
static UInt32 AddrTable_count = AddrTable_STATIC_COUNT;

/* static memory regions
 * CAUTION: AddrTable_STATIC_COUNT must match number of entries below.
 */
static ProcMgr_AddrInfo AddrTable[AddrTable_SIZE] =
    {
        { /* DSP IRAM */
            .addr     = { -1u, -1u, 0x5C7F8000, 0x107F8000, 0x107F8000 },
            .size     = 0x18000,
            .isCached = FALSE,
            .isMapped = FALSE,
            .mapMask  = ProcMgr_MASTERKNLVIRT
        },
        { /* DSP L1PRAM */
            .addr     = { -1u, -1u, 0x5CE00000, 0x10E00000, 0x10E00000 },
            .size     = 0x8000,
            .isCached = FALSE,
            .isMapped = FALSE,
            .mapMask  = ProcMgr_MASTERKNLVIRT
        },
        { /* DSP L1DRAM */
            .addr     = { -1u, -1u, 0x5CF04000, 0x10F04000, 0x10F04000 },
            .size     = 0x14000,
            .isCached = FALSE,
            .isMapped = FALSE,
            .mapMask  = ProcMgr_MASTERKNLVIRT
        },
        { /* L4 CORE */
            .addr     = { -1u, -1u, 0x48000000, 0x48000000, 0x48000000 },
            .size     = 0x1000000,
            .isCached = FALSE,
            .isMapped = FALSE,
            .mapMask  = (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT)
        },
        { /* L4 PERIPHERAL */
            .addr     = { -1u, -1u, 0x49000000, 0x49000000, 0x49000000 },
            .size     = 0x100000,
            .isCached = FALSE,
            .isMapped = FALSE,
            .mapMask  = (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT)
        }
    };

/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    OMAP3530PROC_state
 *
 *  @brief  OMAP3530PROC state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
OMAP3530PROC_ModuleObject OMAP3530PROC_state =
{
    .isSetup = FALSE,
    .configSize = sizeof (OMAP3530PROC_Config),
    .gateHandle = NULL,
    .defInstParams.mmuEnable = TRUE,
    .defInstParams.numMemEntries = 5u
};

/* config override specified in SysLinkCfg.c, defined in ProcMgr.c */
extern String ProcMgr_sysLinkCfgParams;


/* =============================================================================
 * APIs directly called by applications
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default configuration for the OMAP3530PROC
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to OMAP3530PROC_setup filled in by the
 *              OMAP3530PROC module with the default parameters. If the user
 *              does not wish to make any change in the default parameters, this
 *              API is not required to be called.
 *
 *  @param      cfg        Pointer to the OMAP3530PROC module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         OMAP3530PROC_setup
 */
Void
OMAP3530PROC_getConfig (OMAP3530PROC_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "OMAP3530PROC_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_getConfig",
                             PROCESSOR_E_INVALIDARG,
                             "Argument of type (OMAP3530PROC_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (cfg,
                     &(OMAP3530PROC_state.defCfg),
                     sizeof (OMAP3530PROC_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAP3530PROC_getConfig");
}


/*!
 *  @brief      Function to setup the OMAP3530PROC module.
 *
 *              This function sets up the OMAP3530PROC module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then OMAP3530PROC_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call OMAP3530PROC_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional OMAP3530PROC module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         OMAP3530PROC_destroy
 *              GateMutex_create
 */
Int
OMAP3530PROC_setup (OMAP3530PROC_Config * cfg)
{
    Int                 status = PROCESSOR_SUCCESS;
    Error_Block         eb;
    OMAP3530PROC_Config tmpCfg;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PROC_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        OMAP3530PROC_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&OMAP3530PROC_state.refCount,
                            OMAP3530PROC_MAKE_MAGICSTAMP(0),
                            OMAP3530PROC_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&OMAP3530PROC_state.refCount)
        != OMAP3530PROC_MAKE_MAGICSTAMP(1u)) {
        status = OMAP3530PROC_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "OMAP3530PROC Module already initialized!");
    }
    else {
        /* Create a default gate handle for local module protection. */
        OMAP3530PROC_state.gateHandle = (IGateProvider_Handle)
                          GateMutex_create ((GateMutex_Params *) NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (OMAP3530PROC_state.gateHandle == NULL) {
            /*! @retval PROCESSOR_E_FAIL Failed to create GateMutex! */
            status = PROCESSOR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_setup",
                                 status,
                                 "Failed to create GateMutex!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&OMAP3530PROC_state.cfg,
                         cfg,
                         sizeof (OMAP3530PROC_Config));

            /* Initialize the name to handles mapping array. */
            Memory_set (&OMAP3530PROC_state.procHandles,
                        0,
                        (sizeof (OMAP3530PROC_Handle) * MultiProc_MAXPROCESSORS));
            OMAP3530PROC_state.isSetup = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_setup", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the OMAP3530PROC module.
 *
 *              Once this function is called, other OMAP3530PROC module APIs,
 *              except for the OMAP3530PROC_getConfig API cannot be called
 *              anymore.
 *
 *  @sa         OMAP3530PROC_setup
 *              GateMutex_delete
 */
Int
OMAP3530PROC_destroy (Void)
{
    Int    status = PROCESSOR_SUCCESS;
    UInt16 i;

    GT_0trace (curTrace, GT_ENTER, "OMAP3530PROC_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval OMAP3530PROC_E_INVALIDSTATE Module was not setup*/
        status = OMAP3530PROC_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_destroy",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&OMAP3530PROC_state.refCount)
            == OMAP3530PROC_MAKE_MAGICSTAMP(0)) {
            /* Check if any OMAP3530PROC instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                GT_assert (curTrace,
                          (OMAP3530PROC_state.procHandles [i] == NULL));
                if (OMAP3530PROC_state.procHandles [i] != NULL) {
                    OMAP3530PROC_delete (&(OMAP3530PROC_state.procHandles [i]));
                }
            }

            if (OMAP3530PROC_state.gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                        &(OMAP3530PROC_state.gateHandle));
            }

            OMAP3530PROC_state.isSetup = FALSE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_destroy", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for this Processor
 *              instance.
 *
 *  @param      params  Configuration parameters to be returned
 *
 *  @sa         OMAP3530PROC_create
 */
Void
OMAP3530PROC_Params_init (OMAP3530PROC_Handle   handle,
                          OMAP3530PROC_Params * params)
{
    Int i                            = 0;
    OMAP3530PROC_Object * procObject = (OMAP3530PROC_Object *) handle;
    ProcMgr_AddrInfo *    ai         = NULL;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PROC_Params_init", handle, params);
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_Params_init",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_Params_init",
                             PROCESSOR_E_INVALIDARG,
                             "Argument of type (OMAP3530PROC_Params *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (handle == NULL) {

            /* check for instance params override */
            Cfg_propBool(PARAMS_mmuEnable, ProcMgr_sysLinkCfgParams,
                &(OMAP3530PROC_state.defInstParams.mmuEnable));

            Memory_copy(params, &(OMAP3530PROC_state.defInstParams),
                sizeof (OMAP3530PROC_Params));

            /* initialize the translation table */
            for (i = AddrTable_count; i < AddrTable_SIZE; i++) {
                ai = &AddrTable[i];
                ai->addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                ai->addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                ai->addr[ProcMgr_AddrType_MasterPhys] = -1u;
                ai->addr[ProcMgr_AddrType_SlaveVirt] = -1u;
                ai->addr[ProcMgr_AddrType_SlavePhys] = -1u;
                ai->size = 0u;
                ai->isCached = FALSE;
                ai->mapMask = 0u;
                ai->isMapped = FALSE;
            }

            /* initialize refCount for all entries - both static and dynamic */
            for (i = 0; i < AddrTable_SIZE; i++) {
                AddrTable[i].refCount = 0u;
            }

            memcpy((Ptr)params->memEntries, AddrTable, sizeof(AddrTable));
        }
        else {
            /* Return updated OMAP3530PROC instance specific parameters. */
            Memory_copy(params, &(procObject->params),
                sizeof(OMAP3530PROC_Params));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAP3530PROC_Params_init");
}

/*!
 *  @brief      Function to create an instance of this Processor.
 *
 *  @param      name    Name of the Processor instance.
 *  @param      params  Configuration parameters.
 *
 *  @sa         OMAP3530PROC_delete
 */
OMAP3530PROC_Handle
OMAP3530PROC_create (      UInt16                procId,
                     const OMAP3530PROC_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                   status    = PROCESSOR_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Processor_Object *    handle    = NULL;
    OMAP3530PROC_Object * object    = NULL;
    IArg                  key;
    List_Params           listparams;

    GT_2trace(curTrace, GT_ENTER,
        "OMAP3530PROC_create: prodId=%d, params=0x%x", procId, params);

    GT_assert (curTrace, IS_VALID_PROCID (procId));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_create",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /* Not setting status here since this function does not return status.*/
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_create",
                             PROCESSOR_E_INVALIDARG,
                             "Invalid procId specified");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_create",
                             PROCESSOR_E_INVALIDARG,
                             "params passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAP3530PROC_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if the Processor already exists for specified procId. */
        if (OMAP3530PROC_state.procHandles [procId] != NULL) {
            status = PROCESSOR_E_ALREADYEXIST;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OMAP3530PROC_create",
                              status,
                              "Processor already exists for specified procId!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Allocate memory for the handle */
            handle = (Processor_Object *) Memory_calloc (NULL,
                                                      sizeof (Processor_Object),
                                                      0,
                                                      NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (handle == NULL) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PROC_create",
                                     PROCESSOR_E_MEMORY,
                                     "Memory allocation failed for handle!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Populate the handle fields */
                handle->procFxnTable.attach        = &OMAP3530PROC_attach;
                handle->procFxnTable.detach        = &OMAP3530PROC_detach;
                handle->procFxnTable.start         = &OMAP3530PROC_start;
                handle->procFxnTable.stop          = &OMAP3530PROC_stop;
                handle->procFxnTable.read          = &OMAP3530PROC_read;
                handle->procFxnTable.write         = &OMAP3530PROC_write;
                handle->procFxnTable.control       = &OMAP3530PROC_control;
                handle->procFxnTable.translateAddr = &OMAP3530PROC_translate;
                handle->procFxnTable.map           = &OMAP3530PROC_map;
                handle->procFxnTable.unmap         = &OMAP3530PROC_unmap;
                handle->state = ProcMgr_State_Unknown;

                /* Allocate memory for the OMAP3530PROC handle */
                handle->object = Memory_calloc (NULL,
                                                sizeof (OMAP3530PROC_Object),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle->object == NULL) {
                    status = PROCESSOR_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                GT_4CLASS,
                                "OMAP3530PROC_create",
                                status,
                                "Memory allocation failed for handle->object!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    handle->procId = procId;
                    object = (OMAP3530PROC_Object *) handle->object;
                    object->halObject = NULL;
                    /* Copy params into instance object. */
                    Memory_copy (&(object->params),
                                 (Ptr) params,
                                 sizeof (OMAP3530PROC_Params));

                    List_Params_init (&listparams);
                    List_construct (&object->mappedMemEntries, &listparams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif

        /* Leave critical section protection. */
        IGateProvider_leave (OMAP3530PROC_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        if (handle !=  NULL) {
            if (handle->object != NULL) {
                if (object != NULL) {
                    List_destruct (&object->mappedMemEntries);
                    object = NULL;
                }
                Memory_free (NULL,
                             handle->object,
                             sizeof (OMAP3530PROC_Object));
            }
            Memory_free (NULL, handle, sizeof (Processor_Object));
        }
        /*! @retval NULL Function failed */
        handle = NULL;
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace(curTrace, GT_LEAVE, "OMAP3530PROC_create: handle=0x%x", handle);

    /*! @retval Valid-Handle Operation successful */
    return (OMAP3530PROC_Handle) handle;
}


/*!
 *  @brief      Function to delete an instance of this Processor.
 *
 *              The user provided pointer to the handle is reset after
 *              successful completion of this function.
 *
 *  @param      handlePtr  Pointer to Handle to the Processor instance
 *
 *  @sa         OMAP3530PROC_create
 */
Int
OMAP3530PROC_delete (OMAP3530PROC_Handle * handlePtr)
{
    Int                   status = PROCESSOR_SUCCESS;
    OMAP3530PROC_Object * object = NULL;
    Processor_Object *    handle;
    IArg                  key;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PROC_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_delete",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (Processor_Object *) (*handlePtr);
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAP3530PROC_state.gateHandle);

        /* Reset handle in PwrMgr handle array. */
        GT_assert (curTrace, IS_VALID_PROCID (handle->procId));
        OMAP3530PROC_state.procHandles [handle->procId] = NULL;

        /* Free memory used for the OMAP3530PROC object. */
        if (handle->object != NULL) {
            object = (OMAP3530PROC_Object *) handle->object;
            if (object != NULL) {
                List_destruct (&object->mappedMemEntries);
                object = NULL;
            }
            Memory_free (NULL,
                         handle->object,
                         sizeof (OMAP3530PROC_Object));
            handle->object = NULL;
        }

        /* Free memory used for the Processor object. */
        Memory_free (NULL, handle, sizeof (Processor_Object));
        *handlePtr = NULL;

        /* Leave critical section protection. */
        IGateProvider_leave (OMAP3530PROC_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_delete", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to open a handle to an instance of this Processor. This
 *              function is called when access to the Processor is required from
 *              a different process.
 *
 *  @param      handlePtr   Handle to the Processor instance
 *  @param      procId      Processor ID addressed by this Processor instance.
 *
 *  @sa         OMAP3530PROC_close
 */
Int
OMAP3530PROC_open (OMAP3530PROC_Handle * handlePtr, UInt16 procId)
{
    Int status = PROCESSOR_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PROC_open", handlePtr, procId);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, IS_VALID_PROCID (procId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_open",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid NULL handlePtr specified */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_open",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /*! @retval PROCESSOR_E_INVALIDARG Invalid procId specified */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_open",
                             status,
                             "Invalid procId specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Initialize return parameter handle. */
        *handlePtr = NULL;

        /* Check if the PwrMgr exists and return the handle if found. */
        if (OMAP3530PROC_state.procHandles [procId] == NULL) {
            /*! @retval PROCESSOR_E_NOTFOUND Specified instance not found */
            status = PROCESSOR_E_NOTFOUND;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_open",
                             status,
                             "Specified OMAP3530PROC instance does not exist!");
        }
        else {
            *handlePtr = OMAP3530PROC_state.procHandles [procId];
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_open", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to close a handle to an instance of this Processor.
 *
 *  @param      handlePtr  Pointer to Handle to the Processor instance
 *
 *  @sa         OMAP3530PROC_open
 */
Int
OMAP3530PROC_close (OMAP3530PROC_Handle * handlePtr)
{
    Int status = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PROC_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_close",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_close",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_close",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Nothing to be done for close. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_close", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/* =============================================================================
 * APIs called by Processor module (part of function table interface)
 * =============================================================================
 */
/*!
 *  @brief      Function to initialize the slave processor
 *
 *  @param      handle  Handle to the Processor instance
 *  @param      params  Attach parameters
 *
 *  @sa         OMAP3530PROC_detach
 */
Int
OMAP3530PROC_attach(Processor_Handle handle, Processor_AttachParams *params)
{
    Int                             status = PROCESSOR_SUCCESS ;
    Processor_Object *              procHandle = (Processor_Object *) handle;
    OMAP3530PROC_Object *           object = NULL;
    UInt32                          i = 0;
    UInt32                          j = 0;
    UInt32                          index = 0;
    ProcMgr_AddrInfo *              me;
    SysLink_MemEntry *              entry;
    OMAP3530_HalMmuCtrlArgs_Enable  mmuEnableArgs;
    SysLink_MemEntry_Block *        memBlock;

    GT_2trace (curTrace, GT_ENTER,
        "OMAP3530PROC_attach: handle=0x%x, params=0x%x", handle, params);
    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_attach",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_attach",
                             status,
                             "Invalid handle specified");
    }
    else if (params == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_attach",
                                 status,
                                 "Invalid params specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* OMAP3530 DSP is of architecture C64x+ */
        params->procArch = Processor_ProcArch_C64x;

        /* check for instance params override */
        Cfg_propBool(PARAMS_mmuEnable, ProcMgr_sysLinkCfgParams,
            &(object->params.mmuEnable));

        GT_0trace(curTrace, GT_2CLASS,
                "OMAP3530PROC_attach: Mapping memory regions");

        /* search for dsp memory map */
        memBlock = NULL;
        for (j = 0; j < params->sysMemMap->numBlocks; j++) {
            if (!strcmp(params->sysMemMap->memBlocks[j].procName, "DSP")) {
                memBlock = &params->sysMemMap->memBlocks[j];
                break;
            }
        }

        /* update translation tables with memory map */
        for (i = 0; (memBlock != NULL) && (i < memBlock->numEntries)
            && (memBlock->memEntries[i].isValid) && (status >= 0); i++) {

            entry = &memBlock->memEntries[i];

            if (entry->map == FALSE) {
                /* update table with entries which don't require mapping */
                if (AddrTable_count != AddrTable_SIZE) {
                    me = &AddrTable[AddrTable_count];

                    me->addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                    me->addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                    me->addr[ProcMgr_AddrType_MasterPhys] =
                        entry->masterPhysAddr;
                    me->addr[ProcMgr_AddrType_SlaveVirt] = entry->slaveVirtAddr;
                    me->addr[ProcMgr_AddrType_SlavePhys] = -1u;
                    me->size = entry->size;
                    me->isCached = entry->isCached;
                    me->mapMask = entry->mapMask;

                    AddrTable_count++;
                }
                else {
                    status = PROCESSOR_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "OMAP3530PROC_attach", status,
                        "AddrTable_SIZE reached!");
                }
            }
            else if (entry->map == TRUE) {
                /* send these entries back to ProcMgr for mapping */
                index = object->params.numMemEntries;

                if (index != ProcMgr_MAX_MEMORY_REGIONS) {
                    me = &object->params.memEntries[index];

                    me->addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                    me->addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                    me->addr[ProcMgr_AddrType_MasterPhys] =
                        entry->masterPhysAddr;
                    me->addr[ProcMgr_AddrType_SlaveVirt] = entry->slaveVirtAddr;
                    me->addr[ProcMgr_AddrType_SlavePhys] = -1u;
                    me->size = entry->size;
                    me->isCached = entry->isCached;
                    me->mapMask = entry->mapMask;

                    object->params.numMemEntries++;
                }
                else {
                    status = PROCESSOR_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "OMAP3530PROC_attach", status,
                        "ProcMgr_MAX_MEMORY_REGIONS reached!");
                }
            }
            else {
                status = PROCESSOR_E_INVALIDARG;
                GT_setFailureReason(curTrace, GT_4CLASS,
                "OMAP3530PROC_attach", status,
                "System Memory map has entry with invalid 'map' value!");
            }
        } /* for (...) */

        if (status >= 0) {
            /* populate the return params */
            params->numMemEntries = object->params.numMemEntries;
            Memory_copy((Ptr)params->memEntries, (Ptr)object->params.memEntries,
                sizeof(ProcMgr_AddrInfo) * params->numMemEntries);

            status = OMAP3530_halInit(&(object->halObject), NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS,
                    "OMAP3530PROC_attach", status, "OMAP3530_halInit failed");
            }
            else {
#endif
                if ((procHandle->bootMode == ProcMgr_BootMode_Boot)
                    || (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)) {

                    /* place the slave processor in reset */
                    status = OMAP3530_halResetCtrl(object->halObject,
                        Processor_ResetCtrlCmd_Reset, NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason(curTrace, GT_4CLASS,
                            "OMAP3530PROC_attach", status,
                            "Failed to reset the slave processor");
                    }
                    else {
#endif
                        GT_0trace(curTrace, GT_2CLASS,
                                "OMAP3530PROC_attach: slave is now in reset");

                        if (object->params.mmuEnable) {
                            mmuEnableArgs.numMemEntries = 0;
                            status = OMAP3530_halMmuCtrl(object->halObject,
                                Processor_MmuCtrlCmd_Enable, &mmuEnableArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                            if (status < 0) {
                                GT_setFailureReason(curTrace, GT_4CLASS,
                                    "OMAP3530PROC_attach", status,
                                    "Failed to enable the slave MMU");
                            }
#endif
                            GT_0trace(curTrace, GT_2CLASS,
                                "    OMAP3530PROC_attach: Slave MMU "
                                "is configured!\n");
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "OMAP3530PROC_attach", status);

    return (status);
}


/*!
 *  @brief      Function to detach from the Processor.
 *
 *  @param      handle  Handle to the Processor instance
 *
 *  @sa         OMAP3530PROC_attach
 */
Int
OMAP3530PROC_detach (Processor_Handle handle)
{
    Int                   status       = PROCESSOR_SUCCESS;
    Int                   tmpStatus    = PROCESSOR_SUCCESS;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAP3530PROC_Object * object       = NULL;
    UInt32                i;
    ProcMgr_AddrInfo *    ai;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PROC_detach", handle);
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_detach",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_detach",
                             PROCESSOR_E_HANDLE,
                             "Invalid handle specified");
    }
    else {
#endif
        object = (OMAP3530PROC_Object *)procHandle->object;
        GT_assert (curTrace, (object != NULL));

        if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)) {

            if (object->params.mmuEnable) {
                GT_0trace(curTrace, GT_2CLASS,
                    "OMAP3530PROC_detach: Disabling Slave MMU...");

                status = OMAP3530_halMmuCtrl(object->halObject,
                    Processor_MmuCtrlCmd_Disable, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "OMAP3530PROC_detach", status,
                        "Failed to disable the slave MMU");
                }
#endif
            }

            /* delete all dynamically added entries */
            for (i = AddrTable_STATIC_COUNT; i < AddrTable_count; i++) {
                ai = &AddrTable[i];
                ai->addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                ai->addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                ai->addr[ProcMgr_AddrType_MasterPhys] = -1u;
                ai->addr[ProcMgr_AddrType_SlaveVirt] = -1u;
                ai->addr[ProcMgr_AddrType_SlavePhys] = -1u;
                ai->size = 0u;
                ai->isCached = FALSE;
                ai->mapMask = 0u;
                ai->isMapped = FALSE;
                ai->refCount = 0u;
            }

            object->params.numMemEntries = AddrTable_STATIC_COUNT;
            AddrTable_count = AddrTable_STATIC_COUNT;

            /* place the slave processor into reset */
            tmpStatus = OMAP3530_halResetCtrl(object->halObject,
                Processor_ResetCtrlCmd_Reset, NULL);

            GT_0trace (curTrace, GT_2CLASS,
                "OMAP3530PROC_detach: Slave processor is now in reset");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((tmpStatus < 0) && (status >= 0)) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PROC_detach",
                                     status,
                                     "Failed to reset the slave processor");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        tmpStatus = OMAP3530_halExit (object->halObject);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_detach",
                                 status,
                                 "Failed to finalize HAL object");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_detach", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to start the slave processor
 *
 *              Start the slave processor running from its entry point.
 *              Depending on the boot mode, this involves configuring the boot
 *              address and releasing the slave from reset.
 *
 *  @param      handle    Handle to the Processor instance
 *
 *  @sa         OMAP3530PROC_stop, OMAP3530_halBootCtrl, OMAP3530_halResetCtrl
 */
Int
OMAP3530PROC_start (Processor_Handle        handle,
                    UInt32                  entryPt,
                    Processor_StartParams * params)
{
    Int                   status        = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle    = (Processor_Object *) handle;
    OMAP3530PROC_Object * object        = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAP3530PROC_start",
               handle, entryPt, params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_start",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_start",
                             status,
                             "Invalid handle specified");
    }
    else if (params == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_start",
                                 status,
                                 "Invalid params specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_NoPwr)) {
            GT_1trace (curTrace,
                       GT_2CLASS,
                       "    OMAP3530PROC_start: Configuring boot register\n"
                       "        Reset vector [0x%x]!\n",
                       entryPt);
            /* Specify the DSP boot address in the boot config register */
            status = OMAP3530_halBootCtrl (object->halObject,
                                            Processor_BootCtrlCmd_SetEntryPoint,
                                            (Ptr) entryPt);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PROC_start",
                                     status,
                                     "Failed to set slave boot entry point");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Release the slave from reset */
                status = OMAP3530_halResetCtrl (object->halObject,
                                                Processor_ResetCtrlCmd_Release,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OMAP3530PROC_start",
                                         status,
                                         "Failed to release slave from reset");
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* For NoBoot mode, send an interrupt to the slave.
         */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (status >= 0) {
        GT_0trace(curTrace, GT_2CLASS,
                "OMAP3530PROC_start: Slave successfully started");
    }
    else {
        GT_0trace(curTrace, GT_1CLASS,
                "OMAP3530PROC_start: Slave could not be started");
    }

    GT_1trace(curTrace, GT_LEAVE, "OMAP3530PROC_start", status);

    return (status);
}


/*!
 *  @brief      Function to stop the slave processor
 *
 *              Stop the execution of the slave processor. Depending on the boot
 *              mode, this may result in placing the slave processor in reset.
 *
 *  @param      handle    Handle to the Processor instance
 *
 *  @sa         OMAP3530PROC_start, OMAP3530_halResetCtrl
 */
Int
OMAP3530PROC_stop (Processor_Handle handle)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAP3530PROC_Object * object       = NULL;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PROC_stop", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_stop",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_stop",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_NoPwr)) {
            /* Slave is to be stopped only for Boot mode and NoLoad mode. */
            /* Place the slave processor in reset. */
            status = OMAP3530_halResetCtrl (object->halObject,
                                            Processor_ResetCtrlCmd_Reset,
                                            NULL);
            GT_0trace(curTrace, GT_2CLASS,
                    "OMAP3530PROC_stop: Slave is now in reset");
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PROC_stop",
                                     status,
                                     "Failed to place slave in reset");
            }
#endif
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_stop", status);

    return (status);
}


/*!
 *  @brief      Function to read from the slave processor's memory.
 *
 *              Read from the slave processor's memory and copy into the
 *              provided buffer.
 *
 *  @param      handle     Handle to the Processor instance
 *  @param      procAddr   Address in host processor's address space of the
 *                         memory region to read from.
 *  @param      numBytes   IN/OUT parameter. As an IN-parameter, it takes in the
 *                         number of bytes to be read. When the function
 *                         returns, this parameter contains the number of bytes
 *                         actually read.
 *  @param      buffer     User-provided buffer in which the slave processor's
 *                         memory contents are to be copied.
 *
 *  @sa         OMAP3530PROC_write
 */
Int
OMAP3530PROC_read (Processor_Handle   handle,
                   UInt32             procAddr,
                   UInt32 *           numBytes,
                   Ptr                buffer)
{
    Int       status   = PROCESSOR_SUCCESS ;
    UInt8  *  procPtr8 = NULL;

    GT_4trace (curTrace, GT_ENTER, "OMAP3530PROC_read",
               handle, procAddr, numBytes, buffer);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (procAddr != 0));
    GT_assert (curTrace, (numBytes != NULL));
    GT_assert (curTrace, (buffer   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_read",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_read",
                             status,
                             "Invalid handle specified");
    }
    else if (procAddr == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_read",
                                 status,
                                 "Invalid procAddr specified");
    }
    else if (numBytes == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_read",
                                 status,
                                 "Invalid numBytes specified");
    }
    else if (buffer == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_read",
                                 status,
                                 "Invalid buffer specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        procPtr8 = (UInt8 *) procAddr ;
        buffer = Memory_copy (buffer, procPtr8, *numBytes);
        GT_assert (curTrace, (buffer != (UInt32) NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (buffer == (UInt32) NULL) {
            /*! @retval PROCESSOR_E_FAIL Failed in Memory_copy */
            status = PROCESSOR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_read",
                                 status,
                                 "Failed in Memory_copy");
            *numBytes = 0;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_read",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to write into the slave processor's memory.
 *
 *              Read from the provided buffer and copy into the slave
 *              processor's memory.
 *
 *  @param      handle     Handle to the Processor object
 *  @param      procAddr   Address in host processor's address space of the
 *                         memory region to write into.
 *  @param      numBytes   IN/OUT parameter. As an IN-parameter, it takes in the
 *                         number of bytes to be written. When the function
 *                         returns, this parameter contains the number of bytes
 *                         actually written.
 *  @param      buffer     User-provided buffer from which the data is to be
 *                         written into the slave processor's memory.
 *
 *  @sa         OMAP3530PROC_read, OMAP3530PROC_translateAddr
 */
Int
OMAP3530PROC_write (Processor_Handle handle,
                    UInt32           procAddr,
                    UInt32 *         numBytes,
                    Ptr              buffer)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAP3530PROC_Object * object       = NULL;
    UInt8  *              procPtr8     = NULL;
    UInt8                 temp8_1;
    UInt8                 temp8_2;
    UInt8                 temp8_3;
    UInt8                 temp8_4;
    UInt32                temp;

    GT_4trace (curTrace, GT_ENTER, "OMAP3530PROC_write",
               handle, procAddr, numBytes, buffer);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (procAddr != 0));
    GT_assert (curTrace, (numBytes != NULL));
    GT_assert (curTrace, (buffer   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_write",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_write",
                             status,
                             "Invalid handle specified");
    }
    else if (procAddr == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_write",
                                 status,
                                 "Invalid procAddr specified");
    }
    else if (numBytes == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_write",
                                 status,
                                 "Invalid numBytes specified");
    }
    else if (buffer == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PROC_write",
                                 status,
                                 "Invalid buffer specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        if (*numBytes != sizeof (UInt32)) {
            procPtr8 = (UInt8 *) procAddr ;
            procAddr = (UInt32) Memory_copy (procPtr8,
                                               buffer,
                                               *numBytes);
            GT_assert (curTrace, (procAddr != (UInt32) NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (procAddr == (UInt32) NULL) {
                /*! @retval PROCESSOR_E_FAIL Failed in Memory_copy */
                status = PROCESSOR_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PROC_write",
                                     status,
                                     "Failed in Memory_copy");
                *numBytes = 0;
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else  {
             /* For 4 bytes, directly write as a UInt32 */
            temp8_1 = ((UInt8 *) buffer) [0];
            temp8_2 = ((UInt8 *) buffer) [1];
            temp8_3 = ((UInt8 *) buffer) [2];
            temp8_4 = ((UInt8 *) buffer) [3];
            temp = (UInt32) (    ((UInt32) temp8_4 << 24)
                             |   ((UInt32) temp8_3 << 16)
                             |   ((UInt32) temp8_2 << 8)
                             |   ((UInt32) temp8_1));
            *((UInt32*) procAddr) = temp;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_write",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to perform device-dependent operations.
 *
 *              Performs device-dependent control operations as exposed by this
 *              implementation of the Processor module.
 *
 *  @param      handle     Handle to the Processor object
 *  @param      cmd        Device specific processor command
 *  @param      arg        Arguments specific to the type of command.
 *
 *  @sa
 */
Int
OMAP3530PROC_control (Processor_Handle handle, Int32 cmd, Ptr arg)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAP3530PROC_Object * object       = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAP3530PROC_control", handle, cmd, arg);

    GT_assert (curTrace, (handle   != NULL));
    /* cmd and arg can be 0/NULL, so cannot check for validity. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PROC_state.refCount),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(0),
                                  OMAP3530PROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_control",
                             OMAP3530PROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_control",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        /* No control operations currently implemented. */
        /*! @retval PROCESSOR_E_NOTSUPPORTED No control operations are supported
                                             for this device. */
        status = PROCESSOR_E_NOTSUPPORTED;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_control",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      function to translate slave virtual address to master physical.
 *
 *  @param      handle     Handle to the Processor object
 *  @param      dstAddr    Returned: master physical address.
 *  @param      srcAddr     Slave virtual address..
 *
 *  @sa
 */
Int
OMAP3530PROC_translate(
        Processor_Handle    handle,
        UInt32 *            dstAddr,
        UInt32              srcAddr)
{
    Int                     status = PROCESSOR_SUCCESS ;
    Processor_Object *      procHandle= (Processor_Object *)handle;
    OMAP3530PROC_Object *   object = NULL;
    UInt32                  i;
    UInt32                  startAddr;
    UInt32                  endAddr;
    UInt32                  offset;
    ProcMgr_AddrInfo *      ai;

    GT_3trace(curTrace, GT_ENTER,
        "OMAP3530PROC_translate: handle=0x%x, dstAddr=0x%x, srcAddr=0x%x",
        handle, dstAddr, srcAddr);

    GT_assert(curTrace, (handle != NULL));
    GT_assert(curTrace, (dstAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason(curTrace, GT_4CLASS,
            "OMAP3530PROC_translate", status, "Invalid handle specified");
    }
    else if (dstAddr == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG sglist provided as NULL */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS,
            "OMAP3530PROC_translate", status, "dstAddr provided as NULL");
    }
    else {
#endif
        object = (OMAP3530PROC_Object *)procHandle->object;
        GT_assert(curTrace, (object != NULL));
        *dstAddr = -1u;

        for (i = 0; i < AddrTable_count; i++) {
            ai = &AddrTable[i];
            startAddr = ai->addr[ProcMgr_AddrType_SlaveVirt];
            endAddr = startAddr + ai->size;

            if ((startAddr <= srcAddr) && (srcAddr < endAddr)) {
                offset = srcAddr - startAddr;
                *dstAddr = ai->addr[ProcMgr_AddrType_MasterPhys] + offset;
                break;
            }
        }

        if (*dstAddr == -1u) {
            if (!object->params.mmuEnable) {
                /* default to direct mapping (i.e. v=p) */
                *dstAddr = srcAddr;
            }
            else {
                /* srcAddr not found in slave address space */
                status = PROCESSOR_E_INVALIDARG;
                GT_setFailureReason(curTrace, GT_4CLASS,
                    "OMAP3530PROC_translate", status,
                    "srcAddr not found in slave address space");
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace(curTrace, GT_LEAVE,
        "OMAP3530PROC_translate: status=0x%x", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Map the given address translation into the dsp mmu
 *
 *  @param      handle      Handle to the Processor object
 *  @param      dstAddr     Base virtual address
 *  @param      nSegs       Number of given segments
 *  @param      sglist      Segment list
 */
Int
OMAP3530PROC_map(
        Processor_Handle    handle,
        UInt32 *            dstAddr,
        UInt32              nSegs,
        Memory_SGList *     sglist)
{
    Int                     status = PROCESSOR_SUCCESS;
    Processor_Object *      procHandle = (Processor_Object *)handle;
    OMAP3530PROC_Object *   object = NULL;
    Bool                    found = FALSE;
    UInt32                  startAddr;
    UInt32                  endAddr;
    UInt32                  i;
    UInt32                  j;
    ProcMgr_AddrInfo *      ai;
    OMAP3530_HalMmuCtrlArgs_AddEntry addEntryArgs;


    GT_4trace(curTrace, GT_ENTER,
        "OMAP3530PROC_map: handle=0x%x, *dstAddr=0x%x, nSegs=%d, sglist=0x%x",
        handle, *dstAddr, nSegs, sglist);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (sglist != NULL));
    GT_assert (curTrace, (nSegs > 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_map",
                             status,
                             "Invalid handle specified");
    }
    else if (sglist == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG sglist provided as NULL */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_map",
                             status,
                             "sglist provided as NULL");
    }
    else if (nSegs == 0) {
        /*! @retval PROCESSOR_E_INVALIDARG Number of segments provided is 0 */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_map",
                             status,
                             "Number of segments provided is 0");
    }
    else {
#endif
        object = (OMAP3530PROC_Object *)procHandle->object;
        GT_assert (curTrace, (object != NULL));

        for (i = 0; (i < nSegs) && (status >= 0); i++) {
            /* Update the translation table with entries for which mapping
             * is required. Add the entry only if the range does not exist
             * in the translation table.
             */

            /* check in static entries first */
            for (j = 0; j < AddrTable_STATIC_COUNT; j++) {
                ai = &AddrTable[j];
                startAddr = ai->addr[ProcMgr_AddrType_SlaveVirt];
                endAddr = startAddr + ai->size;

                if ((startAddr <= *dstAddr) && (*dstAddr < endAddr)) {
                    found = TRUE;
                    ai->refCount++;
                    break;
                }
            }

            /* if not found in static entries, check in dynamic entries */
            if (!found) {
                for (j = AddrTable_STATIC_COUNT; j < AddrTable_count; j++) {
                    ai = &AddrTable[j];

                    if (ai->isMapped == TRUE) {
                        startAddr = ai->addr[ProcMgr_AddrType_SlaveVirt];
                        endAddr = startAddr + ai->size;

                        if ((startAddr <= *dstAddr) && (*dstAddr < endAddr)
                            && ((*dstAddr + sglist[i].size) <= endAddr)) {
                            found = TRUE;
                            ai->refCount++;
                            break;
                        }
                    }
                }
            }

            /* if not found and mmu is enabled, add new entry to table */
            if (!found) {
                if (object->params.mmuEnable) {
                    if (AddrTable_count != AddrTable_SIZE) {
                        ai = &AddrTable[AddrTable_count];

                        ai->addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                        ai->addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                        ai->addr[ProcMgr_AddrType_MasterPhys] = sglist[i].paddr;
                        ai->addr[ProcMgr_AddrType_SlaveVirt] = *dstAddr;
                        ai->addr[ProcMgr_AddrType_SlavePhys] = -1u;
                        ai->size = sglist[i].size;
                        ai->isCached = sglist[i].isCached;
                        ai->refCount++;

                        AddrTable_count++;
                    }
                    else {
                        status = PROCESSOR_E_FAIL;
                        GT_setFailureReason(curTrace, GT_4CLASS,
                            "OMAP3530PROC_map", status,
                            "AddrTable_SIZE reached!");
                    }
                }
                else {
                    /* if mmu disabled, AddrTable not updated */
                    ai = NULL;
                }
            }

            /* if new entry, map into dsp mmu */
            if ((ai != NULL) && (ai->refCount == 1) && (status >= 0)) {
                ai->isMapped = TRUE;

                if (object->params.mmuEnable) {
                    /* add entry to dsp mmu */
                    addEntryArgs.masterPhyAddr = sglist [i].paddr;
                    addEntryArgs.size          = sglist [i].size;
                    addEntryArgs.slaveVirtAddr = (UInt32)*dstAddr;
                    /* TBD: elementSize, endianism, mixedSized are
                     * hard coded now, must be configurable later
                     */
                    addEntryArgs.elementSize   = ELEM_SIZE_16BIT;
                    addEntryArgs.endianism     = LITTLE_ENDIAN;
                    addEntryArgs.mixedSize     = MMU_TLBES;
                    status = OMAP3530_halMmuCtrl(object->halObject,
                        Processor_MmuCtrlCmd_AddEntry, &addEntryArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason(curTrace, GT_4CLASS,
                            "OMAP3530PROC_map", status,
                            "Processor_MmuCtrlCmd_AddEntry failed");
                    }
#endif
                }
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS,
                    "OMAP3530PROC_map", status, "DSP MMU configuration failed");
            }
#endif
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace(curTrace, GT_LEAVE, "OMAP3530PROC_map: status=0x%x", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to unmap slave address from host address space
 *
 *  @param      handle      Handle to the Processor object
 *  @param      dstAddr     Return parameter: Pointer to receive the mapped
 *                          address.
 *  @param      size        Size of the region to be mapped.
 *
 *  @sa
 */
Int
OMAP3530PROC_unmap(
        Processor_Handle    handle,
        UInt32              addr,
        UInt32              size)
{
    Int                     status = PROCESSOR_SUCCESS;
    Processor_Object *      procHandle = (Processor_Object *)handle;
    OMAP3530PROC_Object *   object = NULL;
    ProcMgr_AddrInfo *      ai;
    Int                     i;
    UInt32                  startAddr;
    UInt32                  endAddr;
    OMAP3530_HalMmuCtrlArgs_DeleteEntry deleteEntryArgs;


    GT_3trace (curTrace, GT_ENTER, "OMAP3530PROC_unmap",
               handle, addr, size);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (size   != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_unmap",
                             status,
                             "Invalid handle specified");
    }
    else if (size == 0) {
        /*! @retval  PROCESSOR_E_INVALIDARG Size provided is zero */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PROC_unmap",
                             status,
                             "Size provided is zero");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* Delete dynamically added non-default entries from translation
         * table only in last unmap called on that entry
         */
        for (i = AddrTable_STATIC_COUNT; i < AddrTable_count; i++) {
            ai = &AddrTable[i];

            if (!ai->isMapped) {
                continue;
            }

            startAddr = ai->addr[ProcMgr_AddrType_SlaveVirt];
            endAddr = startAddr + ai->size;

            if ((startAddr <= addr) && (addr < endAddr)) {
                ai->refCount--;

                if (ai->refCount == 0) {
                    ai->addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                    ai->addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                    ai->addr[ProcMgr_AddrType_MasterPhys] = -1u;
                    ai->addr[ProcMgr_AddrType_SlaveVirt] = -1u;
                    ai->addr[ProcMgr_AddrType_SlavePhys] = -1u;
                    ai->size = 0u;
                    ai->isCached = FALSE;
                    ai->mapMask = 0u;
                    ai->isMapped = FALSE;

                    if (object->params.mmuEnable) {
                        /* remove the entry from the dsp mmu */
                        deleteEntryArgs.size          = size;
                        deleteEntryArgs.slaveVirtAddr = addr;
                        /* TBD: elementSize, endianism, mixedSized are
                         * hard coded now, must be configurable later
                         */
                        deleteEntryArgs.elementSize   = ELEM_SIZE_16BIT;
                        deleteEntryArgs.endianism     = LITTLE_ENDIAN;
                        deleteEntryArgs.mixedSize     = MMU_TLBES;

                        status = OMAP3530_halMmuCtrl(object->halObject,
                            Processor_MmuCtrlCmd_DeleteEntry, &deleteEntryArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason(curTrace, GT_4CLASS,
                                "OMAP3530PROC_unmap", status,
                                "DSP MMU configuration failed");
                        }
#endif
                    }
                }
            }
        } /* for (...) */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PROC_unmap", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
