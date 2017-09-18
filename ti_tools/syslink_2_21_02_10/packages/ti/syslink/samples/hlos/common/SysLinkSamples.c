/*
 *  @file   SysLinkSamples.c
 *
 *  @brief      Common features required by all SysLink samples
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

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/IpcHost.h>

/* Module level headers */
#include <ti/ipc/MultiProc.h>

/* App-specific header files */
#include <ti/syslink/samples/hlos/common/SysLinkSamples.h>
#include <ti/syslink/samples/hlos/common/SysLinkSamplesOS.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Array of processor IDs indicating whether they are available for
 *          usage with the sample applications.
 *          If the index is TRUE, that procId is available.
 */
Bool SysLinkSamples_availableProcIds [SysLinkSamples_MAXPROCESSORS];

/*!
 *  @brief  Array of processor IDs indicating whether the sample application
 *          should be run with the specific proc IDs.
 *          If the user has specified run with a particular proc ID, the value
 *          at that index is TRUE.
 */
Bool SysLinkSamples_runProcIds [SysLinkSamples_MAXPROCESSORS];


/** ============================================================================
 *  Forward declaration of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to set the specified procId as hidden
 */
Void SysLinkSamples_setHiddenProcId (UInt32 procId);


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to execute the startup for common SysLink sample
 *          application features
 */
Void SysLinkSamples_startup (Void)
{
    UInt32  i;

    /* Initialize for all as available. */
    for (i = 0; i < SysLinkSamples_MAXPROCESSORS; i++) {
        SysLinkSamples_availableProcIds [i] = TRUE;
    }

    /* Depending on supported platforms, set the available proc IDs */
#if defined (SYSLINK_SDK_EZSDK)
    SysLinkSamples_setHiddenProcId (MultiProc_getId ("VIDEO-M3"));
    SysLinkSamples_setHiddenProcId (MultiProc_getId ("VPSS-M3"));
#endif /* if defined (SYSLINK_SDK_EZSDK) */

    /* Now call OS-specific startup function */
    SysLinkSamples_osStartup ();
}


/*!
 *  @brief  Function to execute the shutdown for common SysLink sample
 *          application features
 */
Void SysLinkSamples_shutdown (Void)
{
    UInt32  i;

    /* Call OS-specific shutdown function */
    SysLinkSamples_osShutdown ();

    /* Finalize for all as available. */
    for (i = 0; i < SysLinkSamples_MAXPROCESSORS; i++) {
        SysLinkSamples_availableProcIds [i] = TRUE;
    }
}


/*!
 *  @brief  Function to set the specified procId as hidden
 */
Void SysLinkSamples_setHiddenProcId (UInt32 procId)
{
    SysLinkSamples_availableProcIds [procId] = FALSE;
}


/*!
 *  @brief  Function to check if specified procId is available
 */
Bool SysLinkSamples_isAvailableProcId (UInt32 procId)
{
    return (SysLinkSamples_availableProcIds [procId]);
}


/*!
 *  @brief  Function to set the specified procIds to be run in the sample app.
 *          If runAll is TRUE, then all are run as per the platform
 *          specification
 */
Int SysLinkSamples_setToRunProcIds (UInt16 numProcs,
                                    UInt16 procIds [],
                                    Bool   runAll)
{
    Int    status = 0;
    UInt16 i;
    UInt16 numProcsToRun;
    UInt16 procIdsToRun [SysLinkSamples_MAXPROCESSORS];

    if (runAll) {
        numProcsToRun = MultiProc_getNumProcessors () - 1;
        for (i = 0; i < numProcsToRun; i++) {
            procIdsToRun [i] = i;
        }
    }
    else {
        numProcsToRun = numProcs;
        for (i = 0; i < numProcsToRun; i++) {
            procIdsToRun [i] = procIds [i];
        }
    }

    for (i = 0; i < numProcsToRun; i++) {
        /* Check if available, and only then set 'to run'. */
        if (SysLinkSamples_isAvailableProcId (procIdsToRun [i])) {
            SysLinkSamples_runProcIds [procIdsToRun [i]] = TRUE;
        }
        else {
            /* If user is trying to run with an unavailable proc ID, then
             * flag error.
             */
            status = -1;
            Osal_printf ("Warning! Unsupported procID specified for running"
                         " the sample application! proc ID: [%d]\n",
                         procIdsToRun [i]);
            /* Don't break. Print all unsupported IDs in one go. */
        }
    }

    return 0;
}


/*!
 *  @brief  Function to check if specified procId is to be run in the sample app
 */
Bool SysLinkSamples_toRunProcId (UInt32 procId)
{
    return (SysLinkSamples_runProcIds [procId]);
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
