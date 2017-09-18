/*
 *  @file   Omapl1xxNotifyCircSetup.c
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
/* Possible incoming interrupt IDs for DSP */
typedef enum NotifyCircSetup_DSP_INT_tag {
    NotifyCircSetup_DSP_INT0 = 5u,
    NotifyCircSetup_DSP_INT1 = 67u
} NotifyCircSetup_DSP_INT;

/* Possible incoming interrupt IDs for ARM */
typedef enum NotifyCircSetup_ARM_INT_tag {
    NotifyCircSetup_ARM_INT0 = 28u,
    NotifyCircSetup_ARM_INT1 = 29u
} NotifyCircSetup_ARM_INT;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*
 * Incoming interrupt ID for line #0 line on DSP
 *
 *  See NotifyCircSetup_DSP_INT for possible values.
 */
static UInt NotifyCircSetup_dspRecvIntId0 = NotifyCircSetup_DSP_INT0;

/*
 *  Incoming interrupt ID for line #0 line on ARM
 *
 *  See NotifyCircSetup_ARM_INT for possible values.
 */
static UInt NotifyCircSetup_armRecvIntId0 = NotifyCircSetup_ARM_INT0;

/* Enable the second interrupt line */
static Bool NotifyCircSetup_useSecondLine = FALSE;

/*
 *  Incoming interrupt ID for line #1 line on DSP
 *
 *  See NotifyCircSetup_DSP_INT for possible values.
 */
static UInt NotifyCircSetup_dspRecvIntId1 = NotifyCircSetup_DSP_INT1;

/*
 *  Incoming interrupt ID for line #1 line on ARM
 *
 *  See NotifyCircSetup_ARM_INT for possible values.
 */
static UInt NotifyCircSetup_armRecvIntId1 = NotifyCircSetup_ARM_INT1;

/*
 *  Handle to the NotifyDriver for line 0
 */
static INotifyDriver_Handle NotifyCircSetup_dspDriverHandle0 = NULL;

/*
 *  Handle to the NotifyDriver for line 1
 */
static INotifyDriver_Handle NotifyCircSetup_dspDriverHandle1 = NULL;

/*
 *  Handle to the Notify object for line 0
 */
static Notify_Handle NotifyCircSetup_notifyHandle0 = NULL;

/*
 *  Handle to the Notify object for line 1
 */
static Notify_Handle NotifyCircSetup_notifyHandle1 = NULL;


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
 *  @sa         NotifyCircSetupOmapl1xx_detach
 */
Int
NotifyCircSetupOmapl1xx_attach (UInt16 procId, Ptr sharedAddr)
{
    Int32 status = Notify_S_SUCCESS;
    NotifyDriverCirc_Params notifyCircParams;

    GT_1trace (curTrace, GT_ENTER, "NotifyCircSetupOmapl1xx_attach", sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));
    GT_assert (curTrace, (procId != MultiProc_self ()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (sharedAddr == NULL) {
        /*! @retval  Notify_E_INVALIDARG Invalid NULL sharedAddr argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupOmapl1xx_attach",
                             status,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /*
         *  Setup the notify driver to the DSP (Line 0)
         */
        NotifyDriverCirc_Params_init (&notifyCircParams);

        /* Currently not supporting caching on host side. */
        notifyCircParams.cacheEnabled = FALSE;
        notifyCircParams.localIntId   = NotifyCircSetup_armRecvIntId0;
        notifyCircParams.remoteIntId  = NotifyCircSetup_dspRecvIntId0;
        notifyCircParams.remoteProcId = procId;
        notifyCircParams.lineId       = 0;
        notifyCircParams.sharedAddr   = sharedAddr;

        NotifyCircSetup_dspDriverHandle0 = NotifyDriverCirc_create
                                                          (&notifyCircParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NotifyCircSetup_dspDriverHandle0 == NULL) {
            /*! @retval  Notify_E_FAIL NotifyDriverCirc_create failed */
            status = Notify_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyCircSetupOmapl1xx_attach",
                                 status,
                                 "NotifyDriverCirc_create failed");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            NotifyCircSetup_notifyHandle0 = Notify_create (
                                                NotifyCircSetup_dspDriverHandle0,
                                                procId,
                                                0u,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (NotifyCircSetup_notifyHandle0 == NULL) {
                /*! @retval  Notify_E_FAIL Notify_create failed */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyCircSetupOmapl1xx_attach",
                                     status,
                                     "Notify_create failed");
            }
        }

        if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (NotifyCircSetup_useSecondLine) {
                /*
                 *  Setup the notify driver to the DSP (Line 1)
                 */
                NotifyDriverCirc_Params_init (&notifyCircParams);
                notifyCircParams.localIntId     = NotifyCircSetup_armRecvIntId1;
                notifyCircParams.remoteIntId    = NotifyCircSetup_dspRecvIntId1;
                notifyCircParams.remoteProcId   = procId;
                notifyCircParams.lineId         = 1u;
                notifyCircParams.sharedAddr     = sharedAddr;/* To allow sharedegion
                                                             * Calculations
                                                             */
                notifyCircParams.sharedAddr     = (Ptr)((UInt32) sharedAddr +
                              NotifyDriverCirc_sharedMemReq (&notifyCircParams));
                NotifyCircSetup_dspDriverHandle1 = NotifyDriverCirc_create (
                                                            &notifyCircParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (NotifyCircSetup_dspDriverHandle1 == NULL) {
                    /*! @retval  Notify_E_FAIL NotifyDriverCirc_create failed
                                               for line 1*/
                    status = Notify_E_FAIL;
                    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyCircSetupOmapl1xx_attach",
                                 status,
                                 "NotifyDriverCirc_create failed for line 1");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    NotifyCircSetup_notifyHandle1 = Notify_create (
                                                  NotifyCircSetup_dspDriverHandle1,
                                                  procId,
                                                  1u,
                                                  NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (NotifyCircSetup_notifyHandle1 == NULL) {
                        /*! @retval  Notify_E_FAIL Notify_create failed for
                                                 line 1*/
                        status = Notify_E_FAIL;
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NotifyCircSetupOmapl1xx_attach",
                                         status,
                                         "Notify_create failed for line 1");
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyCircSetupOmapl1xx_attach", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to perform device specific destroy for Notify module.
 *              This function deletes the Notify drivers.
 *
 *  @sa         NotifyCircSetupOmapl1xx_attach
 */
Int
NotifyCircSetupOmapl1xx_detach (UInt16 procId)
{
    Int status     = Notify_S_SUCCESS;
    Int tmpStatus  = Notify_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "NotifyCircSetupOmapl1xx_detach");

    /*
     *  Delete the notify driver to the DSP (Line 0)
     */
    status = Notify_delete (&NotifyCircSetup_notifyHandle0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupOmapl1xx_detach",
                             status,
                             "Notify_delete failed for line 0");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    tmpStatus = NotifyDriverCirc_delete (&NotifyCircSetup_dspDriverHandle0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ((tmpStatus < 0) && (status >= 0)) {
        status = tmpStatus;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupOmapl1xx_attach",
                             status,
                             "NotifyDriverCirc_delete failed for line 0");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (NotifyCircSetup_useSecondLine) {
        /*
         *  Delete the notify driver to the DSP (Line 1)
         */
        tmpStatus = Notify_delete (&NotifyCircSetup_notifyHandle1);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyCircSetupOmapl1xx_detach",
                                 status,
                                 "Notify_delete failed for line 1");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        tmpStatus = NotifyDriverCirc_delete (&NotifyCircSetup_dspDriverHandle1);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifyCircSetupOmapl1xx_detach",
                                 status,
                                 "NotifyDriverCirc_delete failed for line 1");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "NotifyCircSetupOmapl1xx_detach", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  ======== NotifyCircSetup_sharedMemReq ========
 *  Return the amount of shared memory required
 */
SizeT
NotifyCircSetupOmapl1xx_sharedMemReq (UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                  memReq = 0x0;
    NotifyDriverCirc_Params params;

    GT_1trace (curTrace, GT_ENTER, "NotifyCircSetupOmapl1xx_sharedMemReq",
               sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (sharedAddr == NULL) {
        /*! @retval  0 Invalid NULL sharedAddr argument provided. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyCircSetupOmapl1xx_sharedMemReq",
                             Notify_E_INVALIDARG,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NotifyDriverCirc_Params_init (&params);
        params.sharedAddr = sharedAddr;

        if (!NotifyCircSetup_useSecondLine) {
            memReq = NotifyDriverCirc_sharedMemReq (&params);
        }
        else {
            memReq = 2 * NotifyDriverCirc_sharedMemReq (&params);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyCircSetupOmapl1xx_sharedMemReq", memReq);

    /*! @retval Shared-Memory-Requirements Operation successful */
    return (memReq);
}

/*!
 * ======== NotifyCircSetup_numIntLines ========
 */
UInt16 NotifyCircSetupOmapl1xx_numIntLines(UInt16 remoteProcId)
{
    UInt16 numLines = 0;

    if (NotifyCircSetup_useSecondLine) {
        numLines = 2;
    }
    else {
        numLines = 1;
    }

    return (numLines);
}
