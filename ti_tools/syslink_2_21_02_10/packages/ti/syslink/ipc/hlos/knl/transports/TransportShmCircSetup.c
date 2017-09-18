/*
 *  @file   TransportShmCircSetup.c
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
#include <ti/syslink/inc/TransportShmCirc.h>
#include <ti/syslink/inc/knl/TransportShmCircSetup.h>


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for TransportShmCircSetup module state */
typedef struct TransportShmCircSetup_ModuleObject {
    TransportShmCirc_Handle handles [MultiProc_MAXPROCESSORS];
    /*!< Store a handle per remote proc */
} TransportShmCircSetup_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmCircSetup_ModuleObject TransportShmCircSetup_state =
{
    .handles [0] = NULL
};

/*!
 *  @var    TransportShmCircSetup_module
 *
 *  @brief  Pointer to the TransportShmCircSetup module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
TransportShmCircSetup_ModuleObject * TransportShmCircSetup_module =
                                                &TransportShmCircSetup_state;


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/* Function that will be called in MessageQ_attach */
Int
TransportShmCircSetup_attach (UInt16 remoteProcId, Ptr sharedAddr)
{
    Int                     status = MessageQ_S_SUCCESS;
    TransportShmCirc_Params params;
    TransportShmCirc_Handle handle;

    GT_2trace (curTrace, GT_ENTER, "TransportShmCircSetup_attach",
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
                             "TransportShmCircSetup_attach",
                             status,
                             "Invalid NULL sharedAddr provided.");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Init the transport parameters */
        TransportShmCirc_Params_init (&params);
        params.sharedAddr = sharedAddr;

        /* Make sure notify driver has been created */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (Notify_intLineRegistered (remoteProcId, 0) == FALSE)) {
            /*! @retval  MessageQ_E_FAIL Notify driver is not registered */
            status = MessageQ_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmCircSetup_attach",
                                 status,
                                 "Notify driver is not registered");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            handle = TransportShmCirc_create (remoteProcId, &params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (handle == NULL)) {
                /*! @retval  MessageQ_E_FAIL TransportShmCirc_create
                                                            failed */
                status = MessageQ_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "TransportShmCircSetup_attach",
                                     status,
                                     "TransportShmCirc_create failed");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                TransportShmCircSetup_module->handles [remoteProcId] = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCircSetup_attach", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/* Function that will be called in MessageQ_detach */
Int
TransportShmCircSetup_detach (UInt16 remoteProcId)
{
    Int                     status = MessageQ_S_SUCCESS;
    TransportShmCirc_Handle handle;

    GT_1trace (curTrace, GT_ENTER, "TransportShmCircSetup_detach",
                                   remoteProcId);

    GT_assert (curTrace, (remoteProcId < MultiProc_MAXPROCESSORS));

    handle = TransportShmCircSetup_module->handles [remoteProcId];
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (handle == NULL)) {
        /*! @retval  MessageQ_E_INVALIDARG TransportShmCirc handle is NULL */
        status = MessageQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "TransportShmCircSetup_detach",
                             status,
                             "TransportShmCirc handle is NULL");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        TransportShmCircSetup_module->handles [remoteProcId] = NULL;
        /* Delete the transport */
        status = TransportShmCirc_delete (&handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (status < 0)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmCircSetup_detach",
                                 status,
                                 "TransportShmCirc_delete failed");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCircSetup_detach", status);

    /*! @retval MessageQ_S_SUCCESS Operation successful */
    return (status);
}


/* Function that returns the amount of shared memory required */
SizeT
TransportShmCircSetup_sharedMemReq (Ptr sharedAddr)
{
    SizeT                   memReq = 0x0;
    TransportShmCirc_Params params;

    GT_1trace (curTrace, GT_ENTER, "TransportShmCircSetup_sharedMemReq",
               sharedAddr);

    /* Don't do anything if only 1 processor in system */
    if (EXPECT_TRUE (MultiProc_getNumProcessors () > 1)) {
        GT_assert (curTrace, (sharedAddr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (EXPECT_FALSE (sharedAddr == NULL)) {
            /*! @retval  0 Invalid NULL sharedAddr argument provided. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "TransportShmCircSetup_sharedMemReq",
                                 MessageQ_E_INVALIDARG,
                                 "Invalid NULL sharedAddr provided.");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            TransportShmCirc_Params_init (&params);
            params.sharedAddr = sharedAddr;

            memReq += TransportShmCirc_sharedMemReq (&params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "TransportShmCircSetup_sharedMemReq",
                                   memReq);

    /*! @retval Shared-Memory-Requirements Operation successful */
    return (memReq);
}


/* Determines if a transport has been registered to a remote processor */
Bool
TransportShmCircSetup_isRegistered (UInt16 remoteProcId)
{
    Bool registered;

    registered = (TransportShmCircSetup_module->handles[remoteProcId] != NULL);

    return (registered);
}
