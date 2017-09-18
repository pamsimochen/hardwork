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
 *  @file   ti/syslink/family/hlos/knl/omapl1xx/omapl1xxdsp/omapl1xxpwr.c
 *
 *  @brief  PwrMgr implementation for OMAPL1XX.
 *
 *          This module is responsible for handling power requests for
 *          the ProcMgr. The implementation is specific to OMAPL1XX.
 */

/*  OS Headers */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>

/* Module headers */
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>

#include <ti/syslink/inc/knl/PwrDefs.h>
#include <ti/syslink/inc/knl/PwrMgr.h>
#include <ti/syslink/inc/knl/omapl1xxpwr.h>
#include <ti/syslink/inc/knl/_omapl1xxpwr.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define OMAPL1XXPWR_MAKE_MAGICSTAMP(x)  ((OMAPL1XXPWR_MODULEID << 12u) | (x))

/*!
 *  @brief  States of PSC.
 */
#define PSC_ENABLE          (0x3)
#define PSC_DISABLE         (0x2)
#define PSC_SYNCRESET       (0x1)
#define PSC_SWRSTDISABLE    (0x0)

/*!
 *  @brief  GEM module number in PSC.
 */
#define LPSC_GEM            15u

/*!
 *  @brief  Offset of the PSC module registers from the base of the CFG
 *          memory.
 */
#define EPCPR               0x0070
#define EPCCR               0x0078
#define PTCMD               0x0120
#define PTSTAT              0x0128
#define PDSTAT              0x0200
#define PDSTAT1             0x0204
#define PDCTL               0x0300
#define PDCTL1              0x0304
#define MDSTAT_DSP          0x0800 + (4 * LPSC_GEM)
#define MDCTL_DSP           0x0A00 + (4 * LPSC_GEM)

/*!
 *  @brief  Offset of the PSC0 from the base of the CFG
 *          memory.
 */
#define  OFFSET_PSC0   0x10000u

#define REG(x)              *((volatile UInt32 *) (x))

/*!
 *  @brief  OMAPL1XXPWR Module state object
 */
typedef struct OMAPL1XXPWR_ModuleObject_tag {
    UInt32                   configSize;
    /*!< Size of configuration structure */
    OMAPL1XXPWR_Config       cfg;
    /*!< OMAPL1XXPWR configuration structure */
    OMAPL1XXPWR_Config       defCfg;
    /*!< Default module configuration */
    Bool                     isSetup;
    /*!< Indicates whether the OMAPL1XXPWR module is setup. */
    OMAPL1XXPWR_Handle       pwrHandles [MultiProc_MAXPROCESSORS];
    /*!< PwrMgr handle array. */
    IGateProvider_Handle              gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    Atomic                   refCount;
    /* Reference count */
} OMAPL1XXPWR_ModuleObject;

/*!
 *  @brief  OMAPL1XXPWR instance object.
 */
struct OMAPL1XXPWR_Object_tag {
    OMAPL1XXPWR_Params params;
    /*!< Instance parameters (configuration values) */
    UInt32             baseCfgBus;
    /*!< Virtual base address of the configuration bus peripherals memory
         range. */
};

/* Defines the OMAPL1XXPWR object type. */
typedef struct OMAPL1XXPWR_Object_tag OMAPL1XXPWR_Object;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    OMAPL1XXPWR_state
 *
 *  @brief  OMAPL1XXPWR state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
OMAPL1XXPWR_ModuleObject OMAPL1XXPWR_state =
{
    .isSetup = FALSE,
    .configSize = sizeof (OMAPL1XXPWR_Config),
    .defCfg.reserved = 0,
    .gateHandle = NULL,
};


/* =============================================================================
 * APIs directly called by applications
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default configuration for the OMAPL1XXPWR
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to OMAPL1XXPWR_setup filled in by the
 *              OMAPL1XXPWR module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      cfg        Pointer to the OMAPL1XXPWR module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         OMAPL1XXPWR_setup
 */
Void
OMAPL1XXPWR_getConfig (OMAPL1XXPWR_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPWR_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_getConfig",
                             PWRMGR_E_INVALIDARG,
                             "Argument of type (OMAPL1XXPWR_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (cfg,
                     &OMAPL1XXPWR_state.defCfg,
                     sizeof (OMAPL1XXPWR_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_getConfig");
}


/*!
 *  @brief      Function to setup the OMAPL1XXPWR module.
 *
 *              This function sets up the OMAPL1XXPWR module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then OMAPL1XXPWR_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call OMAPL1XXPWR_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional OMAPL1XXPWR module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         OMAPL1XXPWR_destroy
 *              GateMutex_create
 */
Int
OMAPL1XXPWR_setup (OMAPL1XXPWR_Config * cfg)
{
    Int              status = PWRMGR_SUCCESS;
    OMAPL1XXPWR_Config   tmpCfg;
    Error_Block eb;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPWR_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        OMAPL1XXPWR_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&OMAPL1XXPWR_state.refCount,
                            OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                            OMAPL1XXPWR_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&OMAPL1XXPWR_state.refCount)
        != OMAPL1XXPWR_MAKE_MAGICSTAMP(1u)) {
        status = OMAPL1XXPWR_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "OMAPL1XXPWR Module already initialized!");
    }
    else {
        /* Create a default gate handle for local module protection. */
        OMAPL1XXPWR_state.gateHandle = (IGateProvider_Handle)
                             GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (OMAPL1XXPWR_state.gateHandle == NULL) {
            /*! @retval PWRMGR_E_FAIL Failed to create GateMutex! */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPWR_setup",
                                 status,
                                 "Failed to create GateMutex!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&OMAPL1XXPWR_state.cfg,
                         cfg,
                         sizeof (OMAPL1XXPWR_Config));

            /* Initialize the name to handles mapping array. */
            Memory_set (&OMAPL1XXPWR_state.pwrHandles,
                    0,
                    (sizeof (OMAPL1XXPWR_Handle) * MultiProc_MAXPROCESSORS));
            OMAPL1XXPWR_state.isSetup = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_setup", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the OMAPL1XXPWR module.
 *
 *              Once this function is called, other OMAPL1XXPWR module APIs, except
 *              for the OMAPL1XXPWR_getConfig API cannot be called anymore.
 *
 *  @sa         OMAPL1XXPWR_setup
 *              GateMutex_delete
 */
Int
OMAPL1XXPWR_destroy (Void)
{
    Int    status = PWRMGR_SUCCESS;
    UInt16 i;

    GT_0trace (curTrace, GT_ENTER, "OMAPL1XXPWR_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval OMAPL1XXPWR_E_INVALIDSTATE Module was not setup*/
        status = OMAPL1XXPWR_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_destroy",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&OMAPL1XXPWR_state.refCount)
            == OMAPL1XXPWR_MAKE_MAGICSTAMP(0)) {
            /* Check if any OMAPL1XXPWR instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                GT_assert (curTrace,
                           (OMAPL1XXPWR_state.pwrHandles [i] == NULL));
                if (OMAPL1XXPWR_state.pwrHandles [i] != NULL) {
                    OMAPL1XXPWR_delete (&(OMAPL1XXPWR_state.pwrHandles [i]));
                }
            }

            if (OMAPL1XXPWR_state.gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                        &(OMAPL1XXPWR_state.gateHandle));
            }

            OMAPL1XXPWR_state.isSetup = FALSE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_destroy", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for this PwrMgr instance.
 *
 *  @param      params  Configuration parameters.
 *
 *  @sa         OMAPL1XXPWR_create
 */
Void
OMAPL1XXPWR_Params_init (OMAPL1XXPWR_Handle handle, OMAPL1XXPWR_Params * params)
{
    OMAPL1XXPWR_Object * pwrObject = (OMAPL1XXPWR_Object *) handle;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPWR_Params_init", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_Params_init",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_Params_init",
                             PWRMGR_E_INVALIDARG,
                             "Argument of type (OMAPL1XXPWR_Params *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (handle == NULL) {
            params->reserved = 0;
        }
        else {
            /* Return updated OMAPL1XXPWR instance specific parameters. */
            Memory_copy (params,
                         &(pwrObject->params),
                         sizeof (OMAPL1XXPWR_Params));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_Params_init");
}

/*!
 *  @brief      Function to create an instance of this PwrMgr.
 *
 *  @param      procId  Processor ID addressed by this PwrMgr instance.
 *  @param      params  Configuration parameters.
 *
 *  @sa         OMAPL1XXPWR_delete
 */
OMAPL1XXPWR_Handle
OMAPL1XXPWR_create (      UInt16               procId,
                    const OMAPL1XXPWR_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                  status = PWRMGR_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    PwrMgr_Object *      handle = NULL;
    IArg                 key;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPWR_create", procId, params);

    GT_assert (curTrace, IS_VALID_PROCID (procId));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_create",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /* Not setting status here since this function does not return status.*/
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_create",
                             PWRMGR_E_INVALIDARG,
                             "Invalid procId specified");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_create",
                             PWRMGR_E_INVALIDARG,
                             "params passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAPL1XXPWR_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if the PwrMgr already exists for specified procId. */
        if (OMAPL1XXPWR_state.pwrHandles [procId] != NULL) {
            status = PWRMGR_E_ALREADYEXIST;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPWR_create",
                                 status,
                                 "PwrMgr already exists for specified procId!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Allocate memory for the handle */
            handle = (PwrMgr_Object *) Memory_calloc (NULL,
                                                      sizeof (PwrMgr_Object),
                                                      0,
                                                      NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (handle == NULL) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAPL1XXPWR_create",
                                     PWRMGR_E_MEMORY,
                                     "Memory allocation failed for handle!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Populate the handle fields */
                handle->pwrFxnTable.attach = &OMAPL1XXPWR_attach;
                handle->pwrFxnTable.detach = &OMAPL1XXPWR_detach;
                handle->pwrFxnTable.on     = &OMAPL1XXPWR_on;
                handle->pwrFxnTable.off    = &OMAPL1XXPWR_off;
                /* TBD: Other functions */

                /* Allocate memory for the OMAPL1XXPWR handle */
                handle->object = Memory_calloc (NULL,
                                                sizeof (OMAPL1XXPWR_Object),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    status = PWRMGR_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "OMAPL1XXPWR_create",
                                        status,
                                        "Memory allocation failed for handle!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    handle->procId = procId;
                    OMAPL1XXPWR_state.pwrHandles [procId] =
                                                (OMAPL1XXPWR_Handle) handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Leave critical section protection. */
        IGateProvider_leave (OMAPL1XXPWR_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        if (handle !=  NULL) {
            if (handle->object != NULL) {
                Memory_free (NULL, handle->object, sizeof (OMAPL1XXPWR_Object));
            }
            Memory_free (NULL, handle, sizeof (PwrMgr_Object));
        }
        /*! @retval NULL Function failed */
        handle = NULL;
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_create", handle);

    /*! @retval Valid-Handle Operation successful */
    return (OMAPL1XXPWR_Handle) handle;
}


/*!
 *  @brief      Function to delete an instance of this PwrMgr.
 *
 *              The user provided pointer to the handle is reset after
 *              successful completion of this function.
 *
 *  @param      handlePtr  Pointer to Handle to the PwrMgr instance
 *
 *  @sa         OMAPL1XXPWR_create
 */
Int
OMAPL1XXPWR_delete (OMAPL1XXPWR_Handle * handlePtr)
{
    Int                  status = PWRMGR_SUCCESS;
    OMAPL1XXPWR_Object * object = NULL;
    PwrMgr_Object *      handle;
    IArg               key;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPWR_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_delete",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PWRMGR_E_INVALIDARG Invalid NULL handlePtr specified*/
        status = PWRMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_delete",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (PwrMgr_Object *) (*handlePtr);
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAPL1XXPWR_state.gateHandle);

        /* Reset handle in PwrMgr handle array. */
        GT_assert (curTrace, IS_VALID_PROCID (handle->procId));
        OMAPL1XXPWR_state.pwrHandles [handle->procId] = NULL;

        object = (OMAPL1XXPWR_Object *) handle->object;
        /* Free memory used for the OMAPL1XXPWR object. */
        if (handle->object != NULL) {
            Memory_free (NULL,
                         handle->object,
                         sizeof (OMAPL1XXPWR_Object));
            handle->object = NULL;
        }

        /* Free memory used for the PwrMgr object. */
        Memory_free (NULL, handle, sizeof (PwrMgr_Object));
        *handlePtr = NULL;

        /* Leave critical section protection. */
        IGateProvider_leave (OMAPL1XXPWR_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_delete", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to open a handle to an instance of this PwrMgr. This
 *              function is called when access to the PwrMgr is required from
 *              a different process.
 *
 *  @param      handlePtr   Handle to the PwrMgr instance
 *  @param      procId      Processor ID addressed by this PwrMgr instance.
 *
 *  @sa         OMAPL1XXPWR_close
 */
Int
OMAPL1XXPWR_open (OMAPL1XXPWR_Handle * handlePtr, UInt16 procId)
{
    Int status = PWRMGR_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPWR_open", handlePtr, procId);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, IS_VALID_PROCID (procId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_open",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid NULL handlePtr specified */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_open",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /*! @retval PWRMGR_E_INVALIDARG Invalid procId specified */
        status = PWRMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_open",
                             status,
                             "Invalid procId specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Initialize return parameter handle. */
        *handlePtr = NULL;

        /* Check if the PwrMgr exists and return the handle if found. */
        if (OMAPL1XXPWR_state.pwrHandles [procId] == NULL) {
            /*! @retval PWRMGR_E_NOTFOUND Specified instance not found */
            status = PWRMGR_E_NOTFOUND;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OMAPL1XXPWR_open",
                              status,
                              "Specified OMAPL1XXPWR instance does not exist!");
        }
        else {
            *handlePtr = OMAPL1XXPWR_state.pwrHandles [procId];
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_open", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to close a handle to an instance of this PwrMgr.
 *
 *  @param      handlePtr  Pointer to Handle to the PwrMgr instance
 *
 *  @sa         OMAPL1XXPWR_open
 */
Int
OMAPL1XXPWR_close (OMAPL1XXPWR_Handle * handlePtr)
{
    Int status = PWRMGR_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPWR_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_close",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PWRMGR_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = PWRMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_close",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_close",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Nothing to be done for close. */
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_close", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/* =============================================================================
 * APIs called by PwrMgr module (part of function table interface)
 * =============================================================================
 */
/*!
 *  @brief      Function to attach to the PwrMgr.
 *
 *  @param      handle  Handle to the PwrMgr instance
 *  @param      params  Attach parameters
 *
 *  @sa         OMAPL1XXPWR_detach
 */
Int
OMAPL1XXPWR_attach (PwrMgr_Handle handle, PwrMgr_AttachParams * params)
{
    Int status = PWRMGR_SUCCESS;
    PwrMgr_Object * pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAPL1XXPWR_Object * object  = NULL;
    Memory_MapInfo mapInfo;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPWR_attach", handle, params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_attach",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_attach",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* Map and get the virtual address for system control module */
        mapInfo.src      = IO_PHYS;
        mapInfo.size     = IO_SIZE;
        mapInfo.isCached = FALSE;
        status = Memory_map (&mapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPWR_attach",
                                 status,
                                 "Failure in mapping system control module");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            object->baseCfgBus = mapInfo.dst;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_attach",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to detach from the PwrMgr.
 *
 *  @param      handle  Handle to the PwrMgr instance
 *
 *  @sa         OMAPL1XXPWR_attach
 */
Int
OMAPL1XXPWR_detach (PwrMgr_Handle handle)
{
    Int status    = PWRMGR_SUCCESS;
    PwrMgr_Object * pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAPL1XXPWR_Object * object  = NULL;
    Memory_UnmapInfo unmapInfo;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPWR_detach", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_detach",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_detach",
                             PWRMGR_E_HANDLE,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* Unmap the virtual address for system control module */
        unmapInfo.addr = object->baseCfgBus;
        unmapInfo.size = IO_SIZE;
        unmapInfo.isCached = FALSE;
        if (unmapInfo.addr != 0) {
            status = Memory_unmap (&unmapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XXPWR_detach",
                                 status,
                                 "Failure in unmapping system control module");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_detach",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to power on the slave processor.
 *
 *  @param      handle    Handle to the PwrMgr instance
 *
 *  @sa         OMAPL1XXPWR_off
 */
Int
OMAPL1XXPWR_on (PwrMgr_Handle handle)
{
    Int                  status       = PWRMGR_SUCCESS ;
    PwrMgr_Object *      pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAPL1XXPWR_Object * object       = NULL;
    UInt32               domainBit    = 2 ; /* DSP power domain is 1 */
    UInt32               addr;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XXPWR_on", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_on",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_on",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        addr = object->baseCfgBus + OFFSET_PSC0;

        /* Wait for any outstanding transition to complete */
        while (REG (addr + PTSTAT) & domainBit) {
            /* Added for MISRAC compliance */
        }

        /* If we are already in that state, just return */
        if ((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_ENABLE) {
            /* Perform transition  */
            REG (addr +MDCTL_DSP) = (   (REG (addr +MDCTL_DSP)
                                       &  (0xFFFFFFE0)) | PSC_ENABLE );
            REG (addr + PTCMD) |= domainBit;

            /* Wait for transition to complete */
            while ((REG (addr + PTSTAT) & domainBit)) {
                /* Added for MISRAC compliance */
            }

            /* Wait and verify the state */
            while ((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_ENABLE) {
                /* Added for MISRAC compliance */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_on",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to power off the slave processor.
 *
 *              Turn the IVA power domain off. To ensure a clean power-off
 *              transition, the IVA subsystem must first be turned on so that
 *              the DSP can initiate an autonomous power-off transition.
 *
 *  @param      handle    Handle to the PwrMgr instance
 *  @param      force     Indicates whether power-off is to be forced
 *
 *  @sa         OMAPL1XXPWR_on
 */
Int
OMAPL1XXPWR_off (PwrMgr_Handle handle, Bool force)
{
    Int                  status       = PWRMGR_SUCCESS ;
    PwrMgr_Object *      pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAPL1XXPWR_Object * object       = NULL;
    UInt32               domainBit    = 2 ; /* DSP power domain is 1 */
    UInt32               addr;

    GT_2trace (curTrace, GT_ENTER, "OMAPL1XXPWR_off", handle, force);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAPL1XXPWR_state.refCount),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(0),
                                  OMAPL1XXPWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_off",
                             OMAPL1XXPWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAPL1XXPWR_off",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAPL1XXPWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        addr = object->baseCfgBus + OFFSET_PSC0;

        /* Wait for any outstanding transition to complete */
        while (REG (addr + PTSTAT) & domainBit) {
            /* Added for MISRAC compliance */
        }

        /* If we are already in that state, just return */
        if ((REG (addr + MDSTAT_DSP) & 0x1F) != PSC_DISABLE) {
            /* Perform transition  */
            REG (addr + MDCTL_DSP) = (   (REG (addr + MDCTL_DSP)
                                        &  (0xFFFFFFE0)) | PSC_DISABLE );
            REG (addr + PTCMD) |= domainBit;

            /* Wait for transition to complete */
            while (REG (addr + PTSTAT) & domainBit ) {
                /* Added for MISRAC compliance */
            }

            /* Wait and verify the state */
            while ((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_DISABLE) {
                /* Added for MISRAC compliance */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XXPWR_off",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}
