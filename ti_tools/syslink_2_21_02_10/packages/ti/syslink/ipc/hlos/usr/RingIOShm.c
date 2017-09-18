/*
 *  @file   RingIOShm.c
 *
 *  @brief      RingIOShm module implementation
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




#include <ti/syslink/Std.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

#include <ti/syslink/RingIO.h>
//TBD : RingIOShm should not include RingIOShm.h it is a temporary solution
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/RingIOShmDrvDefs.h>
#include <ti/syslink/inc/usr/Linux/RingIOShmDrv.h>


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  RingIOShm Module state object
 */
typedef struct RingIOShm_ModuleObject_tag {
    UInt32          setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
    RingIOShm_Config   cfg;
    /* Current config */
} RingIOShm_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    RingIOShm_state
 *
 *  @brief  RingIOShm state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
RingIOShm_ModuleObject RingIOShm_state =
{
    .setupRefCount = 0
};
/* =============================================================================
 * APIS
 * =============================================================================
 */
/* Function to initialize the Config parameter structure with default values.*/
Void
RingIOShm_getConfig (RingIOShm_Config * cfg)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32 status = RingIOShm_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    RingIOShmDrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIOShm_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_getConfig",
                             RingIOShm_E_INVALIDARG,
                             "Argument of type (RingIOShm_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Temporarily open the handle to get the configuration. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIOShmDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIOShm_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.getConfig.config = cfg;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            RingIOShmDrv_ioctl (CMD_RINGIOSHM_GETCONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "RingIOShm_getConfig",
                                  status,
                                  "API (through IOCTL) failed on kernel-side!");

            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        RingIOShmDrv_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "RingIOShm_getConfig");
}


/* Function to setup the RingIOShm module. */
Int32
RingIOShm_setup (const RingIOShm_Config * config)
{
    Int32              status = RingIOShm_S_SUCCESS;
    RingIOShmDrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIOShm_setup", config);

    /* TBD: Protect from multiple threads. */
    RingIOShm_state.setupRefCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (RingIOShm_state.setupRefCount > 1) {
        status = RingIOShm_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "RingIOShm module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   RingIOShm_state.setupRefCount);
    }
    else {
        /* Open the driver handle. */
        status = RingIOShmDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIOShm_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.setup.config = (RingIOShm_Config *) config;
            status = RingIOShmDrv_ioctl (CMD_RINGIOSHM_SETUP, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "RingIOShm_setup",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Memory_copy ((Ptr) &RingIOShm_state.cfg,
                             (Ptr) config,
                             sizeof (RingIOShm_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "RingIOShm_setup", status);

    return status;
}


/* Function to destroy the RingIOShm module. */
Int32
RingIOShm_destroy (Void)
{
    Int32                 status = RingIOShm_S_SUCCESS;
    RingIOShmDrv_CmdArgs   cmdArgs;

    GT_0trace (curTrace, GT_ENTER, "RingIOShm_destroy");

    /* TBD: Protect from multiple threads. */
    RingIOShm_state.setupRefCount--;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (RingIOShm_state.setupRefCount >= 1) {
        status = RingIOShm_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "RingIOShm module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   RingIOShm_state.setupRefCount);
    }
    else {
        status = RingIOShmDrv_ioctl (CMD_RINGIOSHM_DESTROY, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIOShm_destroy",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Close the driver handle. */
        RingIOShmDrv_close ();
    }

    GT_1trace (curTrace, GT_LEAVE, "RingIOShm_destroy", status);

    return status;
}


/* Initialize this config-params structure with supplier-specified
 * defaults before instance creation.
 */
Void
RingIOShm_Params_init (Void * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32               status = RingIOShm_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    RingIOShmDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "RingIOShm_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (RingIOShm_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_Params_init",
                             RingIOShm_E_INVALIDSTATE,
                             "Modules is not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_Params_init",
                             RingIOShm_E_INVALIDARG,
                             "Argument of type (RingIOShm_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        RingIOShmDrv_ioctl (CMD_RINGIOSHM_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIOShm_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "RingIOShm_Params_init");

    return;
}
