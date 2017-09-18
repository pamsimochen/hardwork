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
 *  @file   ti/syslink/family/hlos/knl/omapl1xx/omapl1xxdsp/omapl1xxproc.c
 *
 * @brief   Processor implementation for OMAPL1XX.
 *
 *          This module is responsible for taking care of device-specific
 *          operations for the processor.
 *          The implementation is specific to OMAPL1XX.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
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
#include <ti/syslink/inc/knl/omapl1xxproc.h>
#include <ti/syslink/inc/knl/_omapl1xxproc.h>
#include <ti/syslink/inc/knl/omapl1xx_hal.h>
#include <ti/syslink/inc/knl/omapl1xx_hal_reset.h>
#include <ti/syslink/inc/knl/omapl1xx_hal_boot.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Checks if a value lies in given range.
 */
#define IS_RANGE_VALID(x,min,max) (((x) < (max)) && ((x) >= (min)))

/* Macro to make a correct module magic number with refCount */
#define OMAPL1XXPROC_MAKE_MAGICSTAMP(x)  ((OMAPL1XXPROC_MODULEID << 12u) | (x))

/*!
 *  @brief  OMAPL1XXPROC Module state object
 */
typedef struct OMAPL1XXPROC_ModuleObject_tag {
    UInt32                   configSize;
    /*!< Size of configuration structure */
    OMAPL1XXPROC_Config      cfg;
    /*!< OMAPL1XXPROC configuration structure */
    OMAPL1XXPROC_Config      defCfg;
    /*!< Default module configuration */
    OMAPL1XXPROC_Params      defInstParams;
    /*!< Default parameters for the OMAPL1XXPROC instances */
    Bool                     isSetup;
    /*!< Indicates whether the OMAPL1XXPROC module is setup. */
    OMAPL1XXPROC_Handle      procHandles [MultiProc_MAXPROCESSORS];
    /*!< Processor handle array. */
    IGateProvider_Handle              gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    Atomic                   refCount;
    /* Reference count */
} OMAPL1XXPROC_ModuleObject;

/*!
 *  @brief  OMAPL1XXPROC instance object.
 */
struct OMAPL1XXPROC_Object_tag {
    OMAPL1XXPROC_Params params;
    /*!< Instance parameters (configuration values) */
    Ptr                 halObject;
    /*!< Pointer to the Hardware Abstraction Layer object. */
    List_Object         mappedMemEntries;
    /*!< List of mapped entries */
};

/* Defines the OMAPL1XXPROC object type. */
typedef struct OMAPL1XXPROC_Object_tag OMAPL1XXPROC_Object;

/* Default memory regions */
static ProcMgr_AddrInfo OMAPL1XXPROC_defaultMemRegions [] =
    {
        {
            .addr   = { -1u, -1u, 0x11800000, 0x11800000, 0x11800000},
            .size   = 0x00040000,
        }, /* DSPIRAM */
        {
            .addr   = { -1u, -1u, 0x11F00000, 0x11F00000, 0x11F00000},
            .size   = 0x00008000,
        } /* DSPL1DRAM */
    };


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    OMAPL1XXPROC_state
 *
 *  @brief  OMAPL1XXPROC state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
OMAPL1XXPROC_ModuleObject OMAPL1XXPROC_state =
{
    .isSetup = FALSE,
    .configSize = sizeof (OMAPL1XXPROC_Config),
    .gateHandle = NULL,
    .defInstParams.numMemEntries = 2u
};


/* =============================================================================
 * APIs directly called by applications
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default configuration for the OMAPL1XXPROC
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to OMAPL1XXPROC_setup filled in by the
 *              OMAPL1XXPROC module with the default parameters. If the user
 *              does not wish to make any change in the default parameters, this
 *              API is not required to be called.
 *
 *  @param      cfg        Pointer to the OMAPL1XXPROC module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         OMAPL1XXPROC_setup
 */
Void
OMAPL1XXPROC_getConfig (OMAPL1XXPROC_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPROC_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_getConfig",
                             PROCESSOR_E_INVALIDARG,
                             "Argument of type (OMAPL1XXPROC_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (cfg,
                     &(OMAPL1XXPROC_state.defCfg),
                     sizeof (OMAPL1XXPROC_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_getConfig");
}


/*!
 *  @brief      Function to setup the OMAPL1XXPROC module.
 *
 *              This function sets up the OMAPL1XXPROC module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then OMAPL1XXPROC_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call OMAPL1XXPROC_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional OMAPL1XXPROC module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         OMAPL1XXPROC_destroy
 *              GateMutex_create
 */
Int
OMAPL1XXPROC_setup (OMAPL1XXPROC_Config * cfg)
{
    Int                 status = PROCESSOR_SUCCESS;
    OMAPL1XXPROC_Config tmpCfg;
    Error_Block eb;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPROC_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        OMAPL1XXPROC_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&OMAPL1XXPROC_state.refCount,
                            OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                            OMAPL1XXPROC_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&OMAPL1XXPROC_state.refCount)
        != OMAPL1XXPROC_MAKE_MAGICSTAMP(1u)) {
        status = OMAPL1XXPROC_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "OMAPL1XXPROC Module already initialized!");
    }
    else {
        /* Create a default gate handle for local module protection. */
        OMAPL1XXPROC_state.gateHandle = (IGateProvider_Handle)
                             GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (OMAPL1XXPROC_state.gateHandle == NULL) {
            /*! @retval PROCESSOR_E_FAIL Failed to create GateMutex! */
            status = PROCESSOR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_setup",
                                 status,
                                 "Failed to create GateMutex!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&OMAPL1XXPROC_state.cfg,
                         cfg,
                         sizeof (OMAPL1XXPROC_Config));

            /* Initialize the ID to handles mapping array. */
            Memory_set (&OMAPL1XXPROC_state.procHandles,
                        0,
                        (sizeof (OMAPL1XXPROC_Handle) * MultiProc_MAXPROCESSORS));
            OMAPL1XXPROC_state.isSetup = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_setup", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the OMAPL1XXPROC module.
 *
 *              Once this function is called, other OMAPL1XXPROC module APIs,
 *              except for the OMAPL1XXPROC_getConfig API cannot be called
 *              anymore.
 *
 *  @sa         OMAPL1XXPROC_setup
 *              GateMutex_delete
 */
Int
OMAPL1XXPROC_destroy (Void)
{
    Int    status = PROCESSOR_SUCCESS;
    UInt16 i;

    GT_0trace (curTrace, GT_ENTER, "OMAPL1XXPROC_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval OMAPL1XXPROC_E_INVALIDSTATE Module was not setup*/
        status = OMAPL1XXPROC_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_destroy",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&OMAPL1XXPROC_state.refCount)
            == OMAPL1XXPROC_MAKE_MAGICSTAMP(0)) {
            /* Check if any OMAPL1XXPROC instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                GT_assert (curTrace,
                          (OMAPL1XXPROC_state.procHandles [i] == NULL));
                if (OMAPL1XXPROC_state.procHandles [i] != NULL) {
                    OMAPL1XXPROC_delete (&(OMAPL1XXPROC_state.procHandles [i]));
                }
            }

            if (OMAPL1XXPROC_state.gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                        &(OMAPL1XXPROC_state.gateHandle));
            }

            OMAPL1XXPROC_state.isSetup = FALSE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_destroy", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for this Processor
 *              instance.
 *
 *  @param      params  Configuration parameters to be returned
 *
 *  @sa         OMAPL1XXPROC_create
 */
Void
OMAPL1XXPROC_Params_init (OMAPL1XXPROC_Handle   handle,
                          OMAPL1XXPROC_Params * params)
{
    OMAPL1XXPROC_Object * procObject = (OMAPL1XXPROC_Object *) handle;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPROC_Params_init", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_Params_init",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_Params_init",
                             PROCESSOR_E_INVALIDARG,
                             "Argument of type (OMAPL1XXPROC_Params *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (handle == NULL) {
            Memory_copy (params,
                         &(OMAPL1XXPROC_state.defInstParams),
                         sizeof (OMAPL1XXPROC_Params));
            Memory_copy ((Ptr) params->memEntries,
                         OMAPL1XXPROC_defaultMemRegions,
                         sizeof (OMAPL1XXPROC_defaultMemRegions));
        }
        else {
            /* Return updated OMAPL1XXPROC instance specific parameters. */
            Memory_copy (params,
                         &(procObject->params),
                         sizeof (OMAPL1XXPROC_Params));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_Params_init");
}

/*!
 *  @brief      Function to create an instance of this Processor.
 *
 *  @param      name    Name of the Processor instance.
 *  @param      params  Configuration parameters.
 *
 *  @sa         OMAPL1XXPROC_delete
 */
OMAPL1XXPROC_Handle
OMAPL1XXPROC_create (      UInt16                procId,
                     const OMAPL1XXPROC_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                   status    = PROCESSOR_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Processor_Object *    handle    = NULL;
    OMAPL1XXPROC_Object * object    = NULL;
    IArg                  key;
    List_Params           listParams;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPROC_create", procId, params);

    GT_assert (curTrace, IS_VALID_PROCID (procId));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_create",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /* Not setting status here since this function does not return status.*/
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_create",
                             PROCESSOR_E_INVALIDARG,
                             "Invalid procId specified");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_create",
                             PROCESSOR_E_INVALIDARG,
                             "params passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAPL1XXPROC_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if the Processor already exists for specified procId. */
        if (OMAPL1XXPROC_state.procHandles [procId] != NULL) {
            status = PROCESSOR_E_ALREADYEXIST;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OMAPL1XXPROC_create",
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
                                     "OMAPL1XXPROC_create",
                                     PROCESSOR_E_MEMORY,
                                     "Memory allocation failed for handle!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Populate the handle fields */
                handle->procFxnTable.attach        = &OMAPL1XXPROC_attach;
                handle->procFxnTable.detach        = &OMAPL1XXPROC_detach;
                handle->procFxnTable.start         = &OMAPL1XXPROC_start;
                handle->procFxnTable.stop          = &OMAPL1XXPROC_stop;
                handle->procFxnTable.read          = &OMAPL1XXPROC_read;
                handle->procFxnTable.write         = &OMAPL1XXPROC_write;
                handle->procFxnTable.control       = &OMAPL1XXPROC_control;
                handle->procFxnTable.translateAddr = &OMAPL1XXPROC_translate;
                handle->procFxnTable.map           = &OMAPL1XXPROC_map;
                handle->procFxnTable.unmap         = &OMAPL1XXPROC_unmap;
                handle->state = ProcMgr_State_Unknown;

                /* Allocate memory for the OMAPL1XXPROC handle */
                handle->object = Memory_calloc (NULL,
                                                sizeof (OMAPL1XXPROC_Object),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle->object == NULL) {
                    status = PROCESSOR_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                GT_4CLASS,
                                "OMAPL1XXPROC_create",
                                status,
                                "Memory allocation failed for handle->object!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    handle->procId = procId;
                    object = (OMAPL1XXPROC_Object *) handle->object;
                    object->halObject = NULL;
                    /* Copy params into instance object. */
                    Memory_copy (&(object->params),
                                 (Ptr) params,
                                 sizeof (OMAPL1XXPROC_Params));
                    List_Params_init (&listParams);
                    List_construct (&object->mappedMemEntries, &listParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Leave critical section protection. */
        IGateProvider_leave (OMAPL1XXPROC_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

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
                             sizeof (OMAPL1XXPROC_Object));
            }
            Memory_free (NULL, handle, sizeof (Processor_Object));
        }
        /*! @retval NULL Function failed */
        handle = NULL;
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_create", handle);

    /*! @retval Valid-Handle Operation successful */
    return (OMAPL1XXPROC_Handle) handle;
}


/*!
 *  @brief      Function to delete an instance of this Processor.
 *
 *              The user provided pointer to the handle is reset after
 *              successful completion of this function.
 *
 *  @param      handlePtr  Pointer to Handle to the Processor instance
 *
 *  @sa         OMAPL1XXPROC_create
 */
Int
OMAPL1XXPROC_delete (OMAPL1XXPROC_Handle * handlePtr)
{
    Int                   status = PROCESSOR_SUCCESS;
    OMAPL1XXPROC_Object * object = NULL;
    Processor_Object *    handle;
    IArg                  key;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPROC_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_delete",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (Processor_Object *) (*handlePtr);
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAPL1XXPROC_state.gateHandle);

        /* Reset handle in PwrMgr handle array. */
        GT_assert (curTrace, IS_VALID_PROCID (handle->procId));
        OMAPL1XXPROC_state.procHandles [handle->procId] = NULL;

        /* Free memory used for the OMAPL1XXPROC object. */
        if (handle->object != NULL) {
            object = (OMAPL1XXPROC_Object *) handle->object;
            if (object != NULL) {
                List_destruct (&object->mappedMemEntries);
                object = NULL;
            }
            Memory_free (NULL,
                         handle->object,
                         sizeof (OMAPL1XXPROC_Object));
            handle->object = NULL;
        }

        /* Free memory used for the Processor object. */
        Memory_free (NULL, handle, sizeof (Processor_Object));
        *handlePtr = NULL;

        /* Leave critical section protection. */
        IGateProvider_leave (OMAPL1XXPROC_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_delete", status);

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
 *  @sa         OMAPL1XXPROC_close
 */
Int
OMAPL1XXPROC_open (OMAPL1XXPROC_Handle * handlePtr, UInt16 procId)
{
    Int status = PROCESSOR_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPROC_open", handlePtr, procId);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, IS_VALID_PROCID (procId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_open",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid NULL handlePtr specified */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_open",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /*! @retval PROCESSOR_E_INVALIDARG Invalid procId specified */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_open",
                             status,
                             "Invalid procId specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Initialize return parameter handle. */
        *handlePtr = NULL;

        /* Check if the PwrMgr exists and return the handle if found. */
        if (OMAPL1XXPROC_state.procHandles [procId] == NULL) {
            /*! @retval PROCESSOR_E_NOTFOUND Specified instance not found */
            status = PROCESSOR_E_NOTFOUND;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_open",
                             status,
                             "Specified OMAPL1XXPROC instance does not exist!");
        }
        else {
            *handlePtr = OMAPL1XXPROC_state.procHandles [procId];
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_open", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to close a handle to an instance of this Processor.
 *
 *  @param      handlePtr  Pointer to Handle to the Processor instance
 *
 *  @sa         OMAPL1XXPROC_open
 */
Int
OMAPL1XXPROC_close (OMAPL1XXPROC_Handle * handlePtr)
{
    Int status = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPROC_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_close",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_close",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_close",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Nothing to be done for close. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_close", status);

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
 *  @sa         OMAPL1XXPROC_detach
 */
Int
OMAPL1XXPROC_attach (Processor_Handle handle, Processor_AttachParams * params)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;
    ProcMgr_AddrInfo *    me;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPROC_attach", handle, params);
    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_attach",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_attach",
                             status,
                             "Invalid handle specified");
    }
    else if (params == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_attach",
                                 status,
                                 "Invalid params specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* OMAPL1XX DSP is of architecture C64x+ */
        params->procArch = Processor_ProcArch_C64x;

        GT_0trace(curTrace, GT_2CLASS,
                "OMAPL1XXPROC_attach: Mapping memory regions");

        /* Populate the return params */
        me = object->params.memEntries;
        params->numMemEntries = object->params.numMemEntries;
        Memory_copy ((Ptr) params->memEntries,
                     (Ptr) object->params.memEntries,
                     sizeof (ProcMgr_AddrInfo) * params->numMemEntries);


        status = OMAPL1XX_halInit (&(object->halObject), NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_attach",
                                 status,
                                 "OMAPL1XX_halInit failed");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
                ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)) {
                /* Place the slave processor in reset. */
                status = OMAPL1XX_halResetCtrl (object->halObject,
                                               Processor_ResetCtrlCmd_Reset,
                                               NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OMAPL1XXPROC_attach",
                                         status,
                                         "Failed to reset the slave "
                                         "processor");
                }
                else {
#endif
                    GT_0trace(curTrace, GT_2CLASS,
                            "OMAPL1XXPROC_attach: slave is now in reset");
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "OMAPL1XXPROC_attach", status);

    return (status);
}


/*!
 *  @brief      Function to detach from the Processor.
 *
 *  @param      handle  Handle to the Processor instance
 *
 *  @sa         OMAPL1XXPROC_attach
 */
Int
OMAPL1XXPROC_detach (Processor_Handle handle)
{
    Int                   status       = PROCESSOR_SUCCESS;
    Int                   tmpStatus    = PROCESSOR_SUCCESS;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPROC_detach", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_detach",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_detach",
                             PROCESSOR_E_HANDLE,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));

        if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)) {
            /* Place the slave processor in reset. */
            tmpStatus = OMAPL1XX_halResetCtrl (object->halObject,
                                               Processor_ResetCtrlCmd_Reset,
                                               NULL);
            GT_0trace (curTrace,
                       GT_2CLASS,
                       "    OMAPL1XXPROC_detach: Slave processor is "
                       "now in reset\n");
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((tmpStatus < 0) && (status >= 0)) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAPL1XXPROC_detach",
                                     status,
                                     "Failed to reset the slave processor");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        tmpStatus = OMAPL1XX_halExit (object->halObject);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_detach",
                                 status,
                                 "Failed to finalize HAL object");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_detach", status);

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
 *  @sa         OMAPL1XXPROC_stop, OMAPL1XX_halBootCtrl, OMAPL1XX_halResetCtrl
 */
Int
OMAPL1XXPROC_start (Processor_Handle        handle,
                    UInt32                  entryPt,
                    Processor_StartParams * params)
{
    Int                   status        = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle    = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object        = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAPL1XXPROC_start",
               handle, entryPt, params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_start",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_start",
                             status,
                             "Invalid handle specified");
    }
    else if (params == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_start",
                                 status,
                                 "Invalid params specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_NoPwr)) {
            GT_1trace (curTrace,
                       GT_2CLASS,
                       "    OMAPL1XXPROC_start: Configuring boot register\n"
                       "        Reset vector [0x%x]!\n",
                       entryPt);
            /* Specify the DSP boot address in the boot config register */
            status = OMAPL1XX_halBootCtrl (object->halObject,
                                            Processor_BootCtrlCmd_SetEntryPoint,
                                            (Ptr) entryPt);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAPL1XXPROC_start",
                                     status,
                                     "Failed to set slave boot entry point");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Release the slave from reset */
                status = OMAPL1XX_halResetCtrl (object->halObject,
                                                Processor_ResetCtrlCmd_Release,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OMAPL1XXPROC_start",
                                         status,
                                         "Failed to release slave from reset");
                }
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* For NoBoot mode, send an interrupt to the slave.
         * TBD: How should omapl1xxproc interface with Notify for this?
         */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (status >= 0) {
        GT_0trace(curTrace, GT_2CLASS,
                "OMAPL1XXPROC_start: Slave successfully started");
    }
    else {
        GT_0trace(curTrace, GT_1CLASS,
                "OMAPL1XXPROC_start: Slave could not be started");
    }

    GT_1trace(curTrace, GT_LEAVE, "OMAPL1XXPROC_start", status);

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
 *  @sa         OMAPL1XXPROC_start, OMAPL1XX_halResetCtrl
 */
Int
OMAPL1XXPROC_stop (Processor_Handle handle)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPROC_stop", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_stop",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_stop",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        if (    (procHandle->bootMode == ProcMgr_BootMode_Boot)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_Pwr)
            ||  (procHandle->bootMode == ProcMgr_BootMode_NoLoad_NoPwr)) {
            /* Slave is to be stopped only for Boot mode and NoLoad mode. */
            /* Place the slave processor in reset. */
            status = OMAPL1XX_halResetCtrl (object->halObject,
                                            Processor_ResetCtrlCmd_Reset,
                                            NULL);
            GT_0trace(curTrace, GT_2CLASS,
                    "OMAPL1XXPROC_stop: Slave is now in reset");
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAPL1XXPROC_stop",
                                     status,
                                     "Failed to place slave in reset");
            }
#endif
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_stop", status);

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
 *  @sa         OMAPL1XXPROC_write
 */
Int
OMAPL1XXPROC_read (Processor_Handle   handle,
                   UInt32             procAddr,
                   UInt32 *           numBytes,
                   Ptr                buffer)
{
    Int       status   = PROCESSOR_SUCCESS ;
    UInt8  *  procPtr8 = NULL;

    GT_4trace (curTrace, GT_ENTER, "OMAPL1XXPROC_read",
               handle, procAddr, numBytes, buffer);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (procAddr != 0));
    GT_assert (curTrace, (numBytes != NULL));
    GT_assert (curTrace, (buffer   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_read",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_read",
                             status,
                             "Invalid handle specified");
    }
    else if (procAddr == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_read",
                                 status,
                                 "Invalid procAddr specified");
    }
    else if (numBytes == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_read",
                                 status,
                                 "Invalid numBytes specified");
    }
    else if (buffer == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_read",
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
                                 "OMAPL1XXPROC_read",
                                 status,
                                 "Failed in Memory_copy");
            *numBytes = 0;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_read",status);

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
 *  @sa         OMAPL1XXPROC_read, OMAPL1XXPROC_translateAddr
 */
Int
OMAPL1XXPROC_write (Processor_Handle handle,
                    UInt32           procAddr,
                    UInt32 *         numBytes,
                    Ptr              buffer)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;
    UInt8  *              procPtr8     = NULL;
    UInt8                 temp8_1;
    UInt8                 temp8_2;
    UInt8                 temp8_3;
    UInt8                 temp8_4;
    UInt32                temp;

    GT_4trace (curTrace, GT_ENTER, "OMAPL1XXPROC_write",
               handle, procAddr, numBytes, buffer);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (procAddr != 0));
    GT_assert (curTrace, (numBytes != NULL));
    GT_assert (curTrace, (buffer   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_write",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_write",
                             status,
                             "Invalid handle specified");
    }
    else if (procAddr == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_write",
                                 status,
                                 "Invalid procAddr specified");
    }
    else if (numBytes == 0) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_write",
                                 status,
                                 "Invalid numBytes specified");
    }
    else if (buffer == NULL) {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPROC_write",
                                 status,
                                 "Invalid buffer specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
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
                                     "OMAPL1XXPROC_write",
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

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_write", status);

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
OMAPL1XXPROC_control (Processor_Handle handle, Int32 cmd, Ptr arg)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAPL1XXPROC_control", handle, cmd, arg);

    GT_assert (curTrace, (handle   != NULL));
    /* cmd and arg can be 0/NULL, so cannot check for validity. */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPROC_state.refCount),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPROC_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_control",
                             OMAPL1XXPROC_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_control",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        /* No control operations currently implemented. */
        /*! @retval PROCESSOR_E_NOTSUPPORTED No control operations are supported
                                             for this device. */
        status = PROCESSOR_E_NOTSUPPORTED;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_control",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to translate slave physical address to master physical
 *              address.
 *
 *  @param      handle     Handle to the Processor object
 *  @param      dstAddr    Returned: master physical address.
 *  @param      srcAddr    Slave physical address.
 *
 *  @sa
 */
Int
OMAPL1XXPROC_translate (Processor_Handle handle,
                        UInt32 *         dstAddr,
                        UInt32           srcAddr)
{
    Int                       status       = PROCESSOR_SUCCESS ;
    Processor_Object *        procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAPL1XXPROC_translate",
               handle, dstAddr, srcAddr);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (dstAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_translate",
                             status,
                             "Invalid handle specified");
    }
    else if (dstAddr == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG sglist provided as NULL */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_translate",
                             status,
                             "dstAddr provided as NULL");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* For OMAPL1XX, masterPhys is same as slavePhys for all memory
         * regions.
         */
        *dstAddr = srcAddr;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_translate", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to map slave address to host address space
 *
 *              Map the provided slave address to master address space. This
 *              function also maps the specified address to slave space.
 *
 *  @param      handle      Handle to the Processor object
 *  @param      mapType     Type of mapping to be performed.
 *  @param      addrInfo    Structure containing map info.
 *  @param      srcAddrType Source address type.
 *
 *  @sa
 */
Int
OMAPL1XXPROC_map (Processor_Handle handle,
                  UInt32 *         dstAddr,
                  UInt32           nSegs,
                  Memory_SGList *  sglist)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;

    GT_4trace (curTrace, GT_ENTER, "OMAPL1XXPROC_map",
               handle, dstAddr, nSegs, sglist);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (sglist != NULL));
    GT_assert (curTrace, (nSegs > 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_map",
                             status,
                             "Invalid handle specified");
    }
    else if (sglist == NULL) {
        /*! @retval PROCESSOR_E_INVALIDARG sglist provided as NULL */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_map",
                             status,
                             "sglist provided as NULL");
    }
    else if (nSegs == 0) {
        /*! @retval PROCESSOR_E_INVALIDARG Number of segments provided is 0 */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_map",
                             status,
                             "Number of segments provided is 0");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        /* Nothing to be done since OMAPL1XX does not have a DSP MMU. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_map",status);

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
s *
 *  @sa
 */
Int
OMAPL1XXPROC_unmap (Processor_Handle handle,
                    UInt32           addr,
                    UInt32           size)
{
    Int                   status       = PROCESSOR_SUCCESS ;
    Processor_Object *    procHandle   = (Processor_Object *) handle;
    OMAPL1XXPROC_Object * object       = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAPL1XXPROC_unmap",
               handle, addr, size);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (size   != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval PROCESSOR_E_HANDLE Invalid argument */
        status = PROCESSOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_unmap",
                             status,
                             "Invalid handle specified");
    }
    else if (size == 0) {
        /*! @retval  PROCESSOR_E_INVALIDARG Size provided is zero */
        status = PROCESSOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPROC_unmap",
                             status,
                             "Size provided is zero");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPROC_Object *) procHandle->object;
        GT_assert (curTrace, (object != NULL));
        /* Nothing to be done since OMAPL1XX does not have a DSP MMU. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPROC_unmap",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
