/*
 *  @file   TransportShmSetup.c
 *
 *  @brief      Defines transport specific functions to setup the MessageQ
 *              Transport module.
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
#include <ti/syslink/inc/_MultiProc.h>

/* Module headers */
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/Notify.h>
#include <ti/syslink/inc/TransportShm.h>
#include <ti/syslink/inc/knl/TransportShmSetup.h>


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for TransportShmSetup module state */
typedef struct TransportShmSetup_ModuleObject {
    TransportShm_Handle handles [MultiProc_MAXPROCESSORS];
    /*!< Store a handle per remote proc */
} TransportShmSetup_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmSetup_ModuleObject TransportShmSetup_state =
{
    .handles [0] = NULL
};

/*!
 *  @var    TransportShmSetup_module
 *
 *  @brief  Pointer to the TransportShmSetup module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmSetup_ModuleObject * TransportShmSetup_module =
                                                    &TransportShmSetup_state;


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/* Function that will be called in MessageQ_attach */
Int
TransportShmSetup_attach (UInt16 remoteProcId, Ptr sharedAddr)
{
    Int                 status = MessageQ_S_SUCCESS;
    TransportShm_Params params;
    TransportShm_Handle handle;

    GT_2trace (curTrace, GT_ENTER, "TransportShmSetup_attach",
               remoteProcId, sharedAddr);

    GT_assert (curTrace, (remoteProcId < MultiProc_MAXPROCESSORS));
    GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (sharedAddr == NULL)) {
        /*! @retval  MessageQ_E_INVALIDARG Invalid NULL sharedAddr
                                         argument provided. */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmSetup_attach",
                             status,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Init the transport parameters */
        TransportShm_Params_init (&params);
        params.gate = GateMP_getDefaultRemote ();
        params.sharedAddr = sharedAddr;

        /* Make sure notify driver has been created */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (Notify_intLineRegistered (remoteProcId, 0) == FALSE)) {
            /*! @retval  MessageQ_E_FAIL Notify driver is not registered */
            status = MessageQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmSetup_attach",
                                 status,
                                 "Notify driver is not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* create if self < remoteProcId, otherwise open */
            if (MultiProc_self () < remoteProcId) {
                /* create the transport */
                handle = TransportShm_create (remoteProcId, &params);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (handle == NULL)) {
                    /*! @retval  MessageQ_E_FAIL TransportShm_create
                                                                failed */
                    status = MessageQ_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShmSetup_attach",
                                         status,
                                         "TransportShm_create failed");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    TransportShmSetup_module->handles [remoteProcId] = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else {
                status = TransportShm_openByAddr (params.sharedAddr,
                                                  &handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "TransportShmSetup_attach",
                                         status,
                                         "TransportShm_openByAddr failed");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    TransportShmSetup_module->handles [remoteProcId] =
                                                                    handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmSetup_attach", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/* Function that will be called in MessageQ_detach */
Int
TransportShmSetup_detach (UInt16 remoteProcId)
{
    Int                 status = MessageQ_S_SUCCESS;
    TransportShm_Handle handle;

    GT_1trace (curTrace, GT_ENTER, "TransportShmSetup_detach", remoteProcId);

    GT_assert (curTrace, (remoteProcId < MultiProc_MAXPROCESSORS));

    handle = TransportShmSetup_module->handles [remoteProcId];
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        /*! @retval  MessageQ_E_INVALIDARG TransportShm handle is NULL */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmSetup_detach",
                             status,
                             "TransportShm handle is NULL");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
       if (MultiProc_self () < remoteProcId) {
            /* Delete the transport */
            status = TransportShm_delete (&handle);
            TransportShmSetup_module->handles [remoteProcId] = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (status < 0)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmSetup_detach",
                                     status,
                                     "TransportShm_delete failed");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        else {
            status = TransportShm_close (&handle);
            TransportShmSetup_module->handles [remoteProcId] = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (status < 0)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmSetup_detach",
                                     status,
                                     "TransportShm_close failed");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmSetup_detach", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/* Function that returns the amount of shared memory required */
SizeT
TransportShmSetup_sharedMemReq (Ptr sharedAddr)
{
    SizeT               memReq = 0x0;
    TransportShm_Params params;

    GT_1trace (curTrace, GT_ENTER, "TransportShmSetup_sharedMemReq",
               sharedAddr);

    /* Don't do anything if only 1 processor in system */
    if (EXPECT_TRUE (MultiProc_getNumProcessors () != 1)) {
        GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (sharedAddr == NULL)) {
            /*! @retval  0 Invalid NULL sharedAddr argument provided. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmSetup_sharedMemReq",
                                 MessageQ_E_INVALIDARG,
                                 "Invalid NULL sharedAddr provided.");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            TransportShm_Params_init (&params);
            params.sharedAddr = sharedAddr;

            memReq += TransportShm_sharedMemReq (&params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "TransportShmSetup_sharedMemReq", memReq);

    /*! @retval Shared-Memory-Requirements Operation successful */
    return (memReq);
}


/* Determines if a transport has been registered to a remote processor */
Bool
TransportShmSetup_isRegistered (UInt16 remoteProcId)
{
    Bool registered;

    registered = (TransportShmSetup_module->handles[remoteProcId] != NULL);

    return (registered);
}
