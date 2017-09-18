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
 *  @file   ti/syslink/family/hlos/knl/omap3530/omap3530dsp/omap3530pwr.c
 *
 *  @brief  PwrMgr implementation for OMAP3530.
 *
 *          This module is responsible for handling power requests for
 *          the ProcMgr. The implementation is specific to OMAP3530.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>

/* Module headers */
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/knl/PwrDefs.h>
#include <ti/syslink/inc/knl/PwrMgr.h>
#include <ti/syslink/inc/knl/omap3530pwr.h>
#include <ti/syslink/inc/knl/_omap3530pwr.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define OMAP3530PWR_MAKE_MAGICSTAMP(x)  ((OMAP3530PWR_MODULEID << 12u) | (x))

/*!
 *  @brief  OMAP3530 module and mmr addresses (physical)
 */
#define SYSTEM_CONTROL_ADDR     0x48002000
#define SYSTEM_CONTROL_SIZE     0xC00
#define CONTROL_REVISION        0x00000000
#define CONTROL_IVA2_BOOTADDR   0x00000400
#define CONTROL_IVA2_BOOTMOD    0x00000404

#define PRCM_IVA2_CM_ADDR       0x48004000
#define PRCM_IVA2_CM_SIZE       0x800
#define CM_FCLKEN_IVA2          0x00000000
#define CM_CLKEN_PLL_IVA2       0x00000004
#define CM_IDLEST_IVA2          0x00000020
#define CM_AUTOIDLE_PLL_IVA2    0x00000034
#define CM_CLKSEL1_PLL_IVA2     0x00000040
#define CM_CLKSEL2_PLL_IVA2     0x00000044
#define CM_CLKSTCTRL_IVA2       0x00000048

#define PRCM_IVA2_PRM_ADDR      0x48306000
#define PRCM_IVA2_PRM_SIZE      0x800
#define RM_RSTCTRL_IVA2         0x00000050
#define RM_RSTST_IVA2           0x00000058
#define PM_WKDEP_IVA2           0x000000C8
#define PM_PWSTCTRL_IVA2        0x000000E0
#define PM_PWSTST_IVA2          0x000000E4

#define MAX_WAIT_COUNT          0x50000

#define REG(x)              *((volatile UInt32 *) (x))
#define MEM(x)              *((volatile UInt32 *) (x))


#define LPM_DEV_COUNT   1

#define LPM_STATE_RESET         0
#define LPM_STATE_INITIALIZED   0x0001
#define LPM_STATE_CONNECTED     0x0002

/*!
 *  @brief  OMAP3530PWR Module state object
 */
typedef struct OMAP3530PWR_ModuleObject_tag {
    UInt32                   configSize;
    /*!< Size of configuration structure */
    OMAP3530PWR_Config       cfg;
    /*!< OMAP3530PWR configuration structure */
    OMAP3530PWR_Config       defCfg;
    /*!< Default module configuration */
    Bool                     isSetup;
    /*!< Indicates whether the OMAP3530PWR module is setup. */
    OMAP3530PWR_Handle       pwrHandles [MultiProc_MAXPROCESSORS];
    /*!< PwrMgr handle array. */
    IGateProvider_Handle              gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    Atomic                   refCount;
    /* Reference count */
} OMAP3530PWR_ModuleObject;

/*!
 *  @brief  OMAP3530PWR instance object.
 */
struct OMAP3530PWR_Object_tag {
    OMAP3530PWR_Params params;
    /*!< Instance parameters (configuration values) */
    UInt32             controlVA;
    /*!< Virtual address for system control module */
    UInt32             ivacmVA;
    /*!< Virtual address for iva clock manager */
    UInt32             ivaprmVA;
    /*!< Virtual address for iva power and reset manager */
};

/* Defines the OMAP3530PWR object type. */
typedef struct OMAP3530PWR_Object_tag OMAP3530PWR_Object;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    OMAP3530PWR_state
 *
 *  @brief  OMAP3530PWR state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
OMAP3530PWR_ModuleObject OMAP3530PWR_state =
{
    .isSetup = FALSE,
    .configSize = sizeof (OMAP3530PWR_Config),
    .defCfg.reserved = 0,
    .gateHandle = NULL,
};


/* =============================================================================
 * APIs directly called by applications
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default configuration for the OMAP3530PWR
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to OMAP3530PWR_setup filled in by the
 *              OMAP3530PWR module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      cfg        Pointer to the OMAP3530PWR module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         OMAP3530PWR_setup
 */
Void
OMAP3530PWR_getConfig (OMAP3530PWR_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "OMAP3530PWR_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_getConfig",
                             PWRMGR_E_INVALIDARG,
                             "Argument of type (OMAP3530PWR_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (cfg,
                     &OMAP3530PWR_state.defCfg,
                     sizeof (OMAP3530PWR_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAP3530PWR_getConfig");
}


/*!
 *  @brief      Function to setup the OMAP3530PWR module.
 *
 *              This function sets up the OMAP3530PWR module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then OMAP3530PWR_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call OMAP3530PWR_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional OMAP3530PWR module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         OMAP3530PWR_destroy
 *              GateMutex_create
 */
Int
OMAP3530PWR_setup (OMAP3530PWR_Config * cfg)
{
    Int              status = PWRMGR_SUCCESS;
    OMAP3530PWR_Config   tmpCfg;
    Error_Block eb;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PWR_setup", cfg);
    Error_init (&eb);

    if (cfg == NULL) {
        OMAP3530PWR_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&OMAP3530PWR_state.refCount,
                            OMAP3530PWR_MAKE_MAGICSTAMP(0),
                            OMAP3530PWR_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&OMAP3530PWR_state.refCount)
        != OMAP3530PWR_MAKE_MAGICSTAMP(1u)) {
        status = OMAP3530PWR_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "OMAP3530PWR Module already initialized!");
    }
    else {
        /* Create a default gate handle for local module protection. */
        OMAP3530PWR_state.gateHandle = (IGateProvider_Handle)
                             GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (OMAP3530PWR_state.gateHandle == NULL) {
            /*! @retval PWRMGR_E_FAIL Failed to create GateMutex! */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_setup",
                                 status,
                                 "Failed to create GateMutex!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            Memory_copy (&OMAP3530PWR_state.cfg,
                         cfg,
                         sizeof (OMAP3530PWR_Config));

            /* Initialize the name to handles mapping array. */
            Memory_set (&OMAP3530PWR_state.pwrHandles,
                    0,
                    (sizeof (OMAP3530PWR_Handle) * MultiProc_MAXPROCESSORS));
            OMAP3530PWR_state.isSetup = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_setup", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the OMAP3530PWR module.
 *
 *              Once this function is called, other OMAP3530PWR module APIs, except
 *              for the OMAP3530PWR_getConfig API cannot be called anymore.
 *
 *  @sa         OMAP3530PWR_setup
 *              GateMutex_delete
 */
Int
OMAP3530PWR_destroy (Void)
{
    Int    status = PWRMGR_SUCCESS;
    UInt16 i;

    GT_0trace (curTrace, GT_ENTER, "OMAP3530PWR_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval OMAP3530PWR_E_INVALIDSTATE Module was not setup*/
        status = OMAP3530PWR_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_destroy",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&OMAP3530PWR_state.refCount)
            == OMAP3530PWR_MAKE_MAGICSTAMP(0)) {
            /* Check if any OMAP3530PWR instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                GT_assert (curTrace,
                           (OMAP3530PWR_state.pwrHandles [i] == NULL));
                if (OMAP3530PWR_state.pwrHandles [i] != NULL) {
                    OMAP3530PWR_delete (&(OMAP3530PWR_state.pwrHandles [i]));
                }
            }

            if (OMAP3530PWR_state.gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                        &(OMAP3530PWR_state.gateHandle));
            }

            OMAP3530PWR_state.isSetup = FALSE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_destroy", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for this PwrMgr instance.
 *
 *  @param      params  Configuration parameters.
 *
 *  @sa         OMAP3530PWR_create
 */
Void
OMAP3530PWR_Params_init (OMAP3530PWR_Handle handle, OMAP3530PWR_Params * params)
{
    OMAP3530PWR_Object * pwrObject = (OMAP3530PWR_Object *) handle;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PWR_Params_init", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_Params_init",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_Params_init",
                             PWRMGR_E_INVALIDARG,
                             "Argument of type (OMAP3530PWR_Params *) "
                             "passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (handle == NULL) {
            params->reserved = 0;
        }
        else {
            /* Return updated OMAP3530PWR instance specific parameters. */
            Memory_copy (params,
                         &(pwrObject->params),
                         sizeof (OMAP3530PWR_Params));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OMAP3530PWR_Params_init");
}

/*!
 *  @brief      Function to create an instance of this PwrMgr.
 *
 *  @param      procId  Processor ID addressed by this PwrMgr instance.
 *  @param      params  Configuration parameters.
 *
 *  @sa         OMAP3530PWR_delete
 */
OMAP3530PWR_Handle
OMAP3530PWR_create (      UInt16               procId,
                    const OMAP3530PWR_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                  status = PWRMGR_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    PwrMgr_Object *      handle = NULL;
    IArg                 key;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PWR_create", procId, params);

    GT_assert (curTrace, IS_VALID_PROCID (procId));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_create",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /* Not setting status here since this function does not return status.*/
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_create",
                             PWRMGR_E_INVALIDARG,
                             "Invalid procId specified");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_create",
                             PWRMGR_E_INVALIDARG,
                             "params passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAP3530PWR_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if the PwrMgr already exists for specified procId. */
        if (OMAP3530PWR_state.pwrHandles [procId] != NULL) {
            status = PWRMGR_E_ALREADYEXIST;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_create",
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
                                     "OMAP3530PWR_create",
                                     PWRMGR_E_MEMORY,
                                     "Memory allocation failed for handle!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Populate the handle fields */
                handle->pwrFxnTable.attach = &OMAP3530PWR_attach;
                handle->pwrFxnTable.detach = &OMAP3530PWR_detach;
                handle->pwrFxnTable.on     = &OMAP3530PWR_on;
                handle->pwrFxnTable.off    = &OMAP3530PWR_off;
                /* TBD: Other functions */

                /* Allocate memory for the OMAP3530PWR handle */
                handle->object = Memory_calloc (NULL,
                                                sizeof (OMAP3530PWR_Object),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle == NULL) {
                    status = PWRMGR_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "OMAP3530PWR_create",
                                        status,
                                        "Memory allocation failed for handle!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    handle->procId = procId;
                    OMAP3530PWR_state.pwrHandles [procId] =
                                                (OMAP3530PWR_Handle) handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Leave critical section protection. */
        IGateProvider_leave (OMAP3530PWR_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        if (handle !=  NULL) {
            if (handle->object != NULL) {
                Memory_free (NULL, handle->object, sizeof (OMAP3530PWR_Object));
            }
            Memory_free (NULL, handle, sizeof (PwrMgr_Object));
        }
        /*! @retval NULL Function failed */
        handle = NULL;
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_create", handle);

    /*! @retval Valid-Handle Operation successful */
    return (OMAP3530PWR_Handle) handle;
}


/*!
 *  @brief      Function to delete an instance of this PwrMgr.
 *
 *              The user provided pointer to the handle is reset after
 *              successful completion of this function.
 *
 *  @param      handlePtr  Pointer to Handle to the PwrMgr instance
 *
 *  @sa         OMAP3530PWR_create
 */
Int
OMAP3530PWR_delete (OMAP3530PWR_Handle * handlePtr)
{
    Int                  status = PWRMGR_SUCCESS;
    OMAP3530PWR_Object * object = NULL;
    PwrMgr_Object *      handle;
    IArg                 key;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PWR_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_delete",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PWRMGR_E_INVALIDARG Invalid NULL handlePtr specified*/
        status = PWRMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_delete",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        handle = (PwrMgr_Object *) (*handlePtr);
        /* Enter critical section protection. */
        key = IGateProvider_enter (OMAP3530PWR_state.gateHandle);

        /* Reset handle in PwrMgr handle array. */
        GT_assert (curTrace, IS_VALID_PROCID (handle->procId));
        OMAP3530PWR_state.pwrHandles [handle->procId] = NULL;

        object = (OMAP3530PWR_Object *) handle->object;
        /* Free memory used for the OMAP3530PWR object. */
        if (handle->object != NULL) {
            Memory_free (NULL,
                         handle->object,
                         sizeof (OMAP3530PWR_Object));
            handle->object = NULL;
        }

        /* Free memory used for the PwrMgr object. */
        Memory_free (NULL, handle, sizeof (PwrMgr_Object));
        *handlePtr = NULL;

        /* Leave critical section protection. */
        IGateProvider_leave (OMAP3530PWR_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_delete", status);

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
 *  @sa         OMAP3530PWR_close
 */
Int
OMAP3530PWR_open (OMAP3530PWR_Handle * handlePtr, UInt16 procId)
{
    Int status = PWRMGR_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PWR_open", handlePtr, procId);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, IS_VALID_PROCID (procId));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_open",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid NULL handlePtr specified */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_open",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /*! @retval PWRMGR_E_INVALIDARG Invalid procId specified */
        status = PWRMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_open",
                             status,
                             "Invalid procId specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Initialize return parameter handle. */
        *handlePtr = NULL;

        /* Check if the PwrMgr exists and return the handle if found. */
        if (OMAP3530PWR_state.pwrHandles [procId] == NULL) {
            /*! @retval PWRMGR_E_NOTFOUND Specified instance not found */
            status = PWRMGR_E_NOTFOUND;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OMAP3530PWR_open",
                              status,
                              "Specified OMAP3530PWR instance does not exist!");
        }
        else {
            *handlePtr = OMAP3530PWR_state.pwrHandles [procId];
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_open", status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to close a handle to an instance of this PwrMgr.
 *
 *  @param      handlePtr  Pointer to Handle to the PwrMgr instance
 *
 *  @sa         OMAP3530PWR_open
 */
Int
OMAP3530PWR_close (OMAP3530PWR_Handle * handlePtr)
{
    Int status = PWRMGR_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PWR_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_close",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handlePtr == NULL) {
        /*! @retval PWRMGR_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = PWRMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_close",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid NULL *handlePtr specified */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_close",
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

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_close", status);

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
 *  @sa         OMAP3530PWR_detach
 */
Int
OMAP3530PWR_attach (PwrMgr_Handle handle, PwrMgr_AttachParams * params)
{
    Int                  status       = PWRMGR_SUCCESS ;
    PwrMgr_Object *      pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAP3530PWR_Object * object       = NULL;
    Memory_MapInfo       mapInfo;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PWR_attach", handle, params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_attach",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_attach",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* Map and get the virtual address for system control module */
        mapInfo.src      = SYSTEM_CONTROL_ADDR;
        mapInfo.size     = SYSTEM_CONTROL_SIZE;
        mapInfo.isCached = FALSE;
        status = Memory_map (&mapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_attach",
                                 status,
                                 "Failure in mapping system control module");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            object->controlVA = mapInfo.dst;

            /* Map and get the virtual address for iva clock manager */
            mapInfo.src      = PRCM_IVA2_CM_ADDR;
            mapInfo.size     = PRCM_IVA2_CM_SIZE;
            mapInfo.isCached = FALSE;
            status = Memory_map (&mapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PWR_attach",
                                     status,
                                     "Failure in mapping iva clock manager");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                object->ivacmVA = mapInfo.dst;

                /* Map and get the virtual address for iva power and reset
                 * manager
                 */
                mapInfo.src      = PRCM_IVA2_PRM_ADDR;
                mapInfo.size     = PRCM_IVA2_PRM_SIZE;
                mapInfo.isCached = FALSE;
                status = Memory_map (&mapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OMAP3530PWR_attach",
                              status,
                              "Failure in mapping iva power and reset manager");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    object->ivaprmVA = mapInfo.dst;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_attach",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to detach from the PwrMgr.
 *
 *  @param      handle  Handle to the PwrMgr instance
 *
 *  @sa         OMAP3530PWR_attach
 */
Int
OMAP3530PWR_detach (PwrMgr_Handle handle)
{
    Int                  status       = PWRMGR_SUCCESS ;
    Int                  tmpStatus    = PWRMGR_SUCCESS ;
    PwrMgr_Object *      pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAP3530PWR_Object * object       = NULL;
    Memory_UnmapInfo     unmapInfo;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PWR_detach", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_detach",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_detach",
                             PWRMGR_E_HANDLE,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        /* Unmap the virtual address for system control module */
        unmapInfo.addr = object->controlVA;
        unmapInfo.size = SYSTEM_CONTROL_SIZE;
        unmapInfo.isCached = FALSE;
        if (unmapInfo.addr != 0) {
            status = Memory_unmap (&unmapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PWR_detach",
                                     status,
                                     "Failure in unmapping system control module");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Unmap the virtual address for iva clock manager */
        unmapInfo.addr = object->ivacmVA;
        unmapInfo.size = PRCM_IVA2_CM_SIZE;
        unmapInfo.isCached = FALSE;
        if (unmapInfo.addr != 0) {
            tmpStatus = Memory_unmap (&unmapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((tmpStatus < 0) && (status >= 0)) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530PWR_detach",
                                     tmpStatus,
                                     "Failure in unmapping iva clock manager");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Unmap and the virtual address for iva power and reset manager */
        unmapInfo.addr = object->ivaprmVA;
        unmapInfo.size = PRCM_IVA2_PRM_SIZE;
        unmapInfo.isCached = FALSE;
        if (unmapInfo.addr != 0) {
            tmpStatus = Memory_unmap (&unmapInfo);
            if ((tmpStatus < 0) && (status >= 0)) {
                status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "OMAP3530PWR_detach",
                        status,
                        "Failure in unmapping iva power and reset manager");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_detach",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to power on the slave processor.
 *
 *              Power on the IVA subsystem, hold the DSP and SEQ in reset, and
 *              release IVA2_RST. This is a hostile reset of the IVA. If the IVA
 *              is already powered on, then it must be powered off in order to
 *              terminate all current activity and initiate a power-on-reset
 *              transition to bring the IVA to a know good state.
 *
 *  @param      handle    Handle to the PwrMgr instance
 *
 *  @sa         OMAP3530PWR_off
 */
Int
OMAP3530PWR_on (PwrMgr_Handle handle)
{
    Int                  status       = PWRMGR_SUCCESS ;
    PwrMgr_Object *      pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAP3530PWR_Object * object       = NULL;
    UInt32               i;
    UInt32               cm;
    UInt32               prm;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530PWR_on", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_on",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_on",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        cm      = object->ivacmVA;
        prm     = object->ivaprmVA;

        /* ensure a clean power-off transition */
        status = OMAP3530PWR_off (handle, TRUE);
        if (status < 0) {
            /*! @retval PWRMGR_E_FAIL Failed to complete initial power off */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_on",
                                 status,
                                 "Failed to complete initial power off");
            goto fail;
        }

        /* disable automatic clock gating */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* turn off IVA2 domain functional clock */
        REG(cm + CM_FCLKEN_IVA2) = 0;
        REG(cm + CM_FCLKEN_IVA2);

        /* assert reset on DSP, SEQ */
        REG(prm + RM_RSTCTRL_IVA2) = 0x5;
        REG(prm + RM_RSTCTRL_IVA2);

        /* assert reset on MMU, interconnect, etc. */
        REG(prm + RM_RSTCTRL_IVA2) |= (1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* set next IVA2 power state to ON */
        REG(prm + PM_PWSTCTRL_IVA2) |=
            (3 << 20) |     /* SharedL2CacheFlatOnState = ON */
            (3 << 0);       /* PowerState = ON */
        REG(prm + PM_PWSTCTRL_IVA2);

        /* start a software supervised wake-up transition */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0x2;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* wait for IVA2 power domain to switch ON */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + PM_PWSTST_IVA2) & 0x3) == 0x3) {
                break;
            }
        }
        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 power domain not ON */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_on",
                                 status,
                                 "IVA2 power domain not ON");
            goto fail;
        }

        /* clear power-on transition request */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* turn on IVA2 domain functional clock */
        REG(cm + CM_FCLKEN_IVA2) = 1;
        REG(cm + CM_FCLKEN_IVA2);

        /* ensure the IVA2_SW_RST2 status bit is cleared */
        REG(prm + RM_RSTST_IVA2) |= (1 << 9);
        REG(prm + RM_RSTST_IVA2);

        /* release IVA2_RST2 reset (MMU, SYSC, local interconnect, etc) */
        REG(prm + RM_RSTCTRL_IVA2) &= ~(1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* wait for IVA2_RST2 signal to be released */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + RM_RSTST_IVA2) & (1 << 9))) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2_RST2 signal was not released  */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_on",
                                 status,
                                 "IVA2_RST2 signal was not released");
            goto fail;
        }

        /* clear the IVA2_SW_RST2 status bit */
        REG(prm + RM_RSTST_IVA2) |= (1 << 9);
        REG(prm + RM_RSTST_IVA2);

        /* wait for IVA2 state to become active */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(cm + CM_IDLEST_IVA2) & 0x1) == 0) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 did not become active */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_on",
                                 status,
                                 "IVA2 did not become active");
            goto fail;
        }

        /* enable automatic hardware supervised transitions */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0x3;
        REG(cm + CM_CLKSTCTRL_IVA2);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

fail:
    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_on",status);

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
 *  @sa         OMAP3530PWR_on
 */
Int
OMAP3530PWR_off (PwrMgr_Handle handle, Bool force)
{
    Int                  status       = PWRMGR_SUCCESS ;
    PwrMgr_Object *      pwrMgrHandle = (PwrMgr_Object *) handle;
    OMAP3530PWR_Object * object       = NULL;
    UInt32               i;
    UInt32               cm;
    UInt32               prm;
    UInt32               control;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530PWR_off", handle, force);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(OMAP3530PWR_state.refCount),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(0),
                                  OMAP3530PWR_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_off",
                             OMAP3530PWR_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval PWRMGR_E_HANDLE Invalid argument */
        status = PWRMGR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530PWR_off",
                             status,
                             "Invalid handle specified");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = (OMAP3530PWR_Object *) pwrMgrHandle->object;
        GT_assert (curTrace, (object != NULL));

        cm      = object->ivacmVA;
        prm     = object->ivaprmVA;
        control = object->controlVA;

        /* MPU controlled ON + ACTIVE */
        /* clear power state change request */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* turn off IVA2 domain functional clock (required to assert reset) */
        REG(cm + CM_FCLKEN_IVA2) = 0;
        REG(cm + CM_FCLKEN_IVA2);

        /* assert reset on DSP, SEQ */
        REG(prm + RM_RSTCTRL_IVA2) = 0x5;
        REG(prm + RM_RSTCTRL_IVA2);

        /* assert reset on MMU, interconnect, etc. */
        REG(prm + RM_RSTCTRL_IVA2) |= (1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* set next IVA2 power state to ON */
        REG(prm + PM_PWSTCTRL_IVA2) |= (3 << 0);
        REG(prm + PM_PWSTCTRL_IVA2);

        /* start a software supervised wake-up transition */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0x2;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* wait for IVA2 power domain to switch ON */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + PM_PWSTST_IVA2) & 0x3) == 0x3) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_INVALIDSTATE Invalid state to call this API */
            status = PWRMGR_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 power domain not ON in OMAP3530PWR_on");
            goto fail;
        }

        /* clear power-on transition request */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* turn on IVA2 domain functional clock */
        REG(cm + CM_FCLKEN_IVA2) = 1;
        REG(cm + CM_FCLKEN_IVA2);

        /* ensure the IVA2_SW_RST2 status bit is cleared */
        REG(prm + RM_RSTST_IVA2) |= (1 << 9);
        REG(prm + RM_RSTST_IVA2);

        /* release IVA2_RST2 reset (MMU, SYSC, local interconnect, etc) */
        REG(prm + RM_RSTCTRL_IVA2) &= ~(1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* wait for IVA2_RST2 signal to be released */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + RM_RSTST_IVA2) & (1 << 9))) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2_RST2 signal was not released */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2_RST2 signal was not released");
            goto fail;
        }

        /* clear the IVA2_SW_RST2 status bit */
        REG(prm + RM_RSTST_IVA2) |= (1 << 9);
        REG(prm + RM_RSTST_IVA2);

        /* wait for IVA2 state to become active (~2 loops @ 312MHz) */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(cm + CM_IDLEST_IVA2) & 0x1) == 0) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 did not become active */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 did not become active");
            goto fail;
        }

        /* MPU controlled OFF */

        /* turn off IVA2 domain functional clock (required to assert reset) */
        REG(cm + CM_FCLKEN_IVA2) = 0;
        REG(cm + CM_FCLKEN_IVA2);

        /* assert reset on MMU, interconnect, etc. */
        REG(prm + RM_RSTCTRL_IVA2) |= (1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* wait for IVA2 state to become idle */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(cm + CM_IDLEST_IVA2) & 0x1) == 0x1) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 did not become idle */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 did not become idle");
            goto fail;
        }

        /* set next IVA2 power state to OFF */
        REG(prm + PM_PWSTCTRL_IVA2) &= ~(0x3);
        REG(prm + PM_PWSTCTRL_IVA2);

        /* start a software supervised sleep transition */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0x1;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* wait for IVA2 power domain to switch OFF */
        for (i = 0; i < MAX_WAIT_COUNT + 0x01; i++) {
            if ((REG(prm + PM_PWSTST_IVA2) & 0x3) == 0) {
                break;
            }
        }
        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 power domain not OFF */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 power domain not OFF");
            goto fail;
        }

        /* clear power state change request */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* MPU controlled ON + ACTIVE */

        /* set next IVA2 power state to ON */
        REG(prm + PM_PWSTCTRL_IVA2) |= (3 << 0);
        REG(prm + PM_PWSTCTRL_IVA2);

        /* start a software supervised wake-up transition */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0x2;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* wait for IVA2 power domain to switch ON */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + PM_PWSTST_IVA2) & 0x3) == 0x3) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_INVALIDSTATE Invalid state to call this API */
            status = PWRMGR_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 power domain not ON in OMAP3530PWR_on");
            goto fail;
        }

        /* clear power-on transition request */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* turn on IVA2 domain functional clock */
        REG(cm + CM_FCLKEN_IVA2) = 1;
        REG(cm + CM_FCLKEN_IVA2);

        /* ensure the IVA2_SW_RST2 status bit is cleared */
        REG(prm + RM_RSTST_IVA2) |= (1 << 9);
        REG(prm + RM_RSTST_IVA2);

        /* release IVA2_RST2 reset (MMU, SYSC, local interconnect, etc) */
        REG(prm + RM_RSTCTRL_IVA2) &= ~(1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* wait for IVA2_RST2 signal to be released */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + RM_RSTST_IVA2) & (1 << 9))) {
                break;
            }
        }
        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2_RST2 signal was not released */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2_RST2 signal was not released");
            goto fail;
        }

        /* clear the IVA2_SW_RST2 status bit */
        REG(prm + RM_RSTST_IVA2) |= (1 << 9);
        REG(prm + RM_RSTST_IVA2);

        /* first wait for IVA2 state to become active */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(cm + CM_IDLEST_IVA2) & 0x1) == 0) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 did not become active */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 did not become active");
            goto fail;
        }

        /* set DSP boot address and boot mode */
        REG(control + CONTROL_IVA2_BOOTADDR) = 0;
        REG(control + CONTROL_IVA2_BOOTADDR);
        REG(control + CONTROL_IVA2_BOOTMOD) = 1;
        REG(control + CONTROL_IVA2_BOOTMOD);

        /* enable automatic clock gating */
        REG(cm + CM_CLKSTCTRL_IVA2) = 3;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* set next IVA2 power state to OFF */
        REG(prm + PM_PWSTCTRL_IVA2) &= ~(0x3);
        REG(prm + PM_PWSTCTRL_IVA2);

        /* ensure the IVA2_SW_RST1 status bit is cleared */
        REG(prm + RM_RSTST_IVA2) |= (1 << 8);
        REG(prm + RM_RSTST_IVA2);

        /* release DSP from reset */
        REG(prm + RM_RSTCTRL_IVA2) &= ~(1 << 0);
        REG(prm + RM_RSTCTRL_IVA2);

        /* wait for IVA2_RST1 signal to be released */
        for (i = 0; i < MAX_WAIT_COUNT; i++) {
            if ((REG(prm + RM_RSTST_IVA2) & (1 << 8))) {
                break;
            }
        }
        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2_RST1 signal was not released */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2_RST1 signal was not released");
            goto fail;
        }

        /* clear the IVA2_SW_RST1 status bit */
        REG(prm + RM_RSTST_IVA2) |= (1 << 8);
        REG(prm + RM_RSTST_IVA2);

        /* wait for IVA2 power domain to switch OFF (~7 loops @ 312MHz) */
        for (i = 0; i < MAX_WAIT_COUNT + 1; i++) {
            if ((REG(prm + PM_PWSTST_IVA2) & 0x3) == 0) {
                break;
            }
        }

        if (i == MAX_WAIT_COUNT) {
            /*! @retval PWRMGR_E_FAIL IVA2 power domain not OFF */
            status = PWRMGR_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530PWR_off",
                                 status,
                                 "IVA2 power domain not OFF");
            goto fail;
        }

        /* assert reset on DSP, SEQ */
        REG(prm + RM_RSTCTRL_IVA2) = 0x5;
        REG(prm + RM_RSTCTRL_IVA2);

        /* assert reset on MMU, interconnect, etc. */
        REG(prm + RM_RSTCTRL_IVA2) |= (1 << 1);
        REG(prm + RM_RSTCTRL_IVA2);

        /* disable automatic clock gating */
        REG(cm + CM_CLKSTCTRL_IVA2) = 0;
        REG(cm + CM_CLKSTCTRL_IVA2);

        /* turn off IVA2 domain functional clock */
        REG(cm + CM_FCLKEN_IVA2) = 0;
        REG(cm + CM_FCLKEN_IVA2);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

fail:
    GT_1trace (curTrace, GT_LEAVE, "OMAP3530PWR_off",status);

    /*! @retval PWRMGR_SUCCESS Operation successful */
    return status;
}
