/*
 *  @file   Dm8168NotifyCircSetup.c
 *
 *  @brief      Defines device-specific functions to setup the NotifyCirc module
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

/* Osal & utils headers*/
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_MultiProc.h>

/* Module headers */
#include <ti/ipc/Notify.h>
#include <ti/syslink/inc/knl/NotifyCircSetupProxy.h>
#include <ti/syslink/inc/NotifyDriverCirc.h>
#include <ti/syslink/inc/INotifyDriver.h>
#include <ti/syslink/inc/knl/_NotifyDefs.h>


/* =============================================================================
 *  Typdefs and structures
 * =============================================================================
 */


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*
 *  Handle to the NotifyDriver for line 0
 */
static INotifyDriver_Handle
                    NotifyCircSetup_driverHandle [MultiProc_MAXPROCESSORS];

/*
 *  Handle to the Notify objects
 */
static Notify_Handle NotifyCircSetup_notifyHandle [MultiProc_MAXPROCESSORS];


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/*!
 *  @brief      Function to perform device specific setup for Notify module.
 *              This function creates the Notify drivers.
 *
 *  @param[in]  sharedAddr Shared address base.
 *
 *  @sa         NotifyCircSetupDm8168_detach
 */
Int
NotifyCircSetupDm8168_attach (UInt16 procId, Ptr sharedAddr)
{
    Int32 status = Notify_S_SUCCESS;
    NotifyDriverCirc_Params notifyCircParams;

    GT_1trace (curTrace, GT_ENTER, "NotifyCircSetupDm8168_attach", sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));
    GT_assert (curTrace, (procId != MultiProc_self ()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (sharedAddr == NULL) {
        /*! @retval  Notify_E_INVALIDARG Invalid NULL sharedAddr argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupDm8168_attach",
                             status,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NotifyDriverCirc_Params_init (&notifyCircParams);

        /* Currently not supporting caching on host side. */
        notifyCircParams.cacheEnabled = FALSE;
        notifyCircParams.lineId       = 0;
        notifyCircParams.localIntId   = 77u;
        notifyCircParams.remoteIntId  = 0u;
        notifyCircParams.remoteProcId = procId;
        notifyCircParams.sharedAddr   = sharedAddr;

        NotifyCircSetup_driverHandle [procId] = NotifyDriverCirc_create
                                                          (&notifyCircParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NotifyCircSetup_driverHandle [procId] == NULL) {
            /*! @retval  Notify_E_FAIL NotifyDriverCirc_create failed */
            status = Notify_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyCircSetupDm8168_attach",
                                 status,
                                 "NotifyDriverCirc_create failed");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            NotifyCircSetup_notifyHandle [procId] = Notify_create (
                                        NotifyCircSetup_driverHandle [procId],
                                        procId,
                                        0u,
                                        NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (NotifyCircSetup_notifyHandle [procId] == NULL) {
                /*! @retval  Notify_E_FAIL Notify_create failed */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyCircSetupDm8168_attach",
                                     status,
                                     "Notify_create failed");
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyCircSetupDm8168_attach", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to perform device specific destroy for Notify module.
 *              This function deletes the Notify drivers.
 *
 *  @sa         NotifyCircSetupDm8168_attach
 */
Int
NotifyCircSetupDm8168_detach (UInt16 procId)
{
    Int32  status     = Notify_S_SUCCESS;
    Int32  tmpStatus  = Notify_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "NotifyCircSetupDm8168_detach");

    GT_assert (curTrace, (procId != MultiProc_self ()));

    /*
     *  Delete the notify driver to the DSP (Line 0)
     */
    status = Notify_delete (&(NotifyCircSetup_notifyHandle [procId]));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupDm8168_detach",
                             status,
                             "Notify_delete failed for line 0");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    tmpStatus = NotifyDriverCirc_delete (
                            &(NotifyCircSetup_driverHandle [procId]));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ((tmpStatus < 0) && (status >= 0)) {
        status = tmpStatus;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupDm8168_detach",
                             status,
                             "NotifyDriverCirc_delete failed for line 0");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyCircSetupDm8168_detach", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  ======== NotifyCircSetup_sharedMemReq ========
 *  Return the amount of shared memory required
 */
SizeT
NotifyCircSetupDm8168_sharedMemReq (UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                  memReq = 0x0;
    NotifyDriverCirc_Params params;

    GT_1trace (curTrace, GT_ENTER, "NotifyCircSetupDm8168_sharedMemReq",
               sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (sharedAddr == NULL) {
        /*! @retval  0 Invalid NULL sharedAddr argument provided. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupDm8168_sharedMemReq",
                             Notify_E_INVALIDARG,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NotifyDriverCirc_Params_init (&params);
        params.sharedAddr = sharedAddr;

        memReq = NotifyDriverCirc_sharedMemReq (&params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyCircSetupDm8168_sharedMemReq", memReq);

    /*! @retval Shared-Memory-Requirements Operation successful */
    return (memReq);
}

/*!
 * ======== NotifyCircSetup_numIntLines ========
 */
UInt16 NotifyCircSetupDm8168_numIntLines(UInt16 remoteProcId)
{
    return (1);
}
