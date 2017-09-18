/*
 *  @file   Omapl1xxNotifySetup.c
 *
 *  @brief      Defines device-specific functions to setup the Notify module.
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

/* Module headers */
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/knl/NotifySetupProxy.h>
#include <ti/syslink/inc/INotifyDriver.h>
#include <ti/syslink/inc/NotifyDriverShm.h>
#include <ti/syslink/inc/knl/_NotifyDefs.h>


/* =============================================================================
 *  Typdefs and structures
 * =============================================================================
 */
/* Possible incoming interrupt IDs for DSP */
typedef enum NotifySetup_DSP_INT_tag {
    NotifySetup_DSP_INT0 = 5u,
    NotifySetup_DSP_INT1 = 67u
} NotifySetup_DSP_INT;

/* Possible incoming interrupt IDs for ARM */
typedef enum NotifySetup_ARM_INT_tag {
    NotifySetup_ARM_INT0 = 28u,
    NotifySetup_ARM_INT1 = 29u
} NotifySetup_ARM_INT;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*
 * Incoming interrupt ID for line #0 line on DSP
 *
 *  See NotifySetup_DSP_INT for possible values.
 */
static UInt NotifySetup_dspRecvIntId0 = NotifySetup_DSP_INT0;

/*
 *  Incoming interrupt ID for line #0 line on ARM
 *
 *  See NotifySetup_ARM_INT for possible values.
 */
static UInt NotifySetup_armRecvIntId0 = NotifySetup_ARM_INT0;

/* Enable the second interrupt line */
static Bool NotifySetup_useSecondLine = FALSE;

/*
 *  Incoming interrupt ID for line #1 line on DSP
 *
 *  See NotifySetup_DSP_INT for possible values.
 */
static UInt NotifySetup_dspRecvIntId1 = NotifySetup_DSP_INT1;

/*
 *  Incoming interrupt ID for line #1 line on ARM
 *
 *  See NotifySetup_ARM_INT for possible values.
 */
static UInt NotifySetup_armRecvIntId1 = NotifySetup_ARM_INT1;

/*
 *  Handle to the NotifyDriver for line 0
 */
static INotifyDriver_Handle NotifySetup_dspDriverHandle0 = NULL;

/*
 *  Handle to the NotifyDriver for line 1
 */
static INotifyDriver_Handle NotifySetup_dspDriverHandle1 = NULL;

/*
 *  Handle to the Notify object for line 0
 */
static Notify_Handle NotifySetup_notifyHandle0 = NULL;

/*
 *  Handle to the Notify object for line 1
 */
static Notify_Handle NotifySetup_notifyHandle1 = NULL;


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
 *  @sa         NotifySetupOmapl1xx_detach
 */
Int
NotifySetupOmapl1xx_attach (UInt16 procId, Ptr sharedAddr)
{
    Int32                  status    = Notify_S_SUCCESS ;
    NotifyDriverShm_Params notifyShmParams;

    GT_1trace (curTrace, GT_ENTER, "NotifySetupOmapl1xx_attach", sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (sharedAddr == NULL) {
        /*! @retval  Notify_E_INVALIDARG Invalid NULL sharedAddr argument
                                         provided. */
        status = Notify_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifySetupOmapl1xx_attach",
                             status,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /*
         *  Setup the notify driver to the DSP (Line 0)
         */
        NotifyDriverShm_Params_init (&notifyShmParams);
        notifyShmParams.cacheEnabled   = SharedRegion_isCacheEnabled(
            SharedRegion_getId((Ptr)sharedAddr));
        notifyShmParams.localIntId     = NotifySetup_armRecvIntId0;
        notifyShmParams.remoteIntId    = NotifySetup_dspRecvIntId0;
        notifyShmParams.remoteProcId   = procId;
        notifyShmParams.lineId         = 0u;
        notifyShmParams.sharedAddr     = sharedAddr;

        NotifySetup_dspDriverHandle0 = NotifyDriverShm_create(&notifyShmParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NotifySetup_dspDriverHandle0 == NULL) {
            /*! @retval  Notify_E_FAIL NotifyDriverShm_create failed for line
                                       0 */
            status = Notify_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifySetupOmapl1xx_attach",
                                 status,
                                 "NotifyDriverShm_create failed for line 0");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            NotifySetup_notifyHandle0 = Notify_create (
                                                   NotifySetup_dspDriverHandle0,
                                                   procId,
                                                   0u,
                                                   NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (NotifySetup_notifyHandle0 == NULL) {
                /*! @retval  Notify_E_FAIL Notify_create failed for line 0 */
                status = Notify_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifySetupOmapl1xx_attach",
                                     status,
                                     "Notify_create failed for line 0");
            }
        }

        if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (NotifySetup_useSecondLine) {
                /*
                 *  Setup the notify driver to the DSP (Line 1)
                 */
                NotifyDriverShm_Params_init (&notifyShmParams);
                notifyShmParams.localIntId     = NotifySetup_armRecvIntId1;
                notifyShmParams.remoteIntId    = NotifySetup_dspRecvIntId1;
                notifyShmParams.remoteProcId   = procId;
                notifyShmParams.lineId         = 1u;
                notifyShmParams.sharedAddr     = sharedAddr;/* To allow sharedegion
                                                             * Calculations
                                                             */
                notifyShmParams.sharedAddr     = (Ptr)((UInt32) sharedAddr +
                              NotifyDriverShm_sharedMemReq (&notifyShmParams));
                NotifySetup_dspDriverHandle1 = NotifyDriverShm_create (
                                                            &notifyShmParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (NotifySetup_dspDriverHandle1 == NULL) {
                    /*! @retval  Notify_E_FAIL NotifyDriverShm_create failed
                                               for line 1*/
                    status = Notify_E_FAIL;
                    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifySetupOmapl1xx_attach",
                                 status,
                                 "NotifyDriverShm_create failed for line 1");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    NotifySetup_notifyHandle1 = Notify_create (
                                                  NotifySetup_dspDriverHandle1,
                                                  procId,
                                                  1u,
                                                  NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (NotifySetup_notifyHandle1 == NULL) {
                        /*! @retval  Notify_E_FAIL Notify_create failed for
                                                 line 1*/
                        status = Notify_E_FAIL;
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NotifySetupOmapl1xx_attach",
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

    GT_1trace (curTrace, GT_LEAVE, "NotifySetupOmapl1xx_attach", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to perform device specific destroy for Notify module.
 *              This function deletes the Notify drivers.
 *
 *  @sa         NotifySetupOmapl1xx_attach
 */
Int
NotifySetupOmapl1xx_detach (UInt16 procId)
{
    Int status     = Notify_S_SUCCESS;
    Int tmpStatus  = Notify_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "NotifySetupOmapl1xx_detach");

    /*
     *  Delete the notify driver to the DSP (Line 0)
     */
    status = Notify_delete (&NotifySetup_notifyHandle0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifySetupOmapl1xx_detach",
                             status,
                             "Notify_delete failed for line 0");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    tmpStatus = NotifyDriverShm_delete (&NotifySetup_dspDriverHandle0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ((tmpStatus < 0) && (status >= 0)) {
        status = tmpStatus;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifySetupOmapl1xx_attach",
                             status,
                             "NotifyDriverShm_delete failed for line 0");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (NotifySetup_useSecondLine) {
        /*
         *  Delete the notify driver to the DSP (Line 1)
         */
        tmpStatus = Notify_delete (&NotifySetup_notifyHandle1);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifySetupOmapl1xx_detach",
                                 status,
                                 "Notify_delete failed for line 1");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        tmpStatus = NotifyDriverShm_delete (&NotifySetup_dspDriverHandle1);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NotifySetupOmapl1xx_detach",
                                 status,
                                 "NotifyDriverShm_delete failed for line 1");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "NotifySetupOmapl1xx_detach", status);

    /*! @retval Notify_S_SUCCESS Operation successful */
    return (status);
}


/*!
 *  ======== NotifySetup_sharedMemReq ========
 *  Return the amount of shared memory required
 */
SizeT
NotifySetupOmapl1xx_sharedMemReq (UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                  memReq = 0x0;
    NotifyDriverShm_Params params;

    GT_1trace (curTrace, GT_ENTER, "NotifySetupOmapl1xx_sharedMemReq",
               sharedAddr);

    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (sharedAddr == NULL) {
        /*! @retval  0 Invalid NULL sharedAddr argument provided. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifySetupOmapl1xx_sharedMemReq",
                             Notify_E_INVALIDARG,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NotifyDriverShm_Params_init (&params);
        params.sharedAddr = sharedAddr;

        if (!NotifySetup_useSecondLine) {
            memReq = NotifyDriverShm_sharedMemReq (&params);
        }
        else {
            memReq = 2 * NotifyDriverShm_sharedMemReq (&params);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifySetupOmapl1xx_sharedMemReq", memReq);

    /*! @retval Shared-Memory-Requirements Operation successful */
    return (memReq);
}


/*!
 * ======== NotifySetup_numIntLines ========
 */
UInt16 NotifySetupOmapl1xx_numIntLines(UInt16 remoteProcId)
{
    UInt16 numLines = 0;

    if (NotifySetup_useSecondLine) {
        numLines = 2;
    }
    else {
        numLines = 1;
    }

    return (numLines);
}
