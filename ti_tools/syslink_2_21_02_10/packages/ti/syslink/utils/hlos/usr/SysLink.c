/*
 *  @file   SysLink.c
 *
 *  @brief      Initializes and finalizes user side SysLink
 *              Mainly created for future use. All setup/destroy APIs on user
 *              side will be call from this module.
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

/* User side utils headers */
#include <ti/syslink/SysLink.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/inc/usr/IpcUsr.h>
#include <ti/syslink/utils/MemoryOS.h>
#if defined(SYSLINK_BUILDOS_QNX)
#include <ti/syslink/utils/_Cache_qnx.h>
#endif

#include <stdlib.h>  /* for getenv() */

/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/* Function to initialize SysLink. */
Void SysLink_setup (Void)
{
    Ipc_Config config;

#if defined (SYSLINK_TRACE_ENABLE)
    char *pSL_DEBUG;
    UInt32 traceMask;

    /* enable tracing from SL_DEBUG env var */
    pSL_DEBUG = getenv("SL_DEBUG");
    if (pSL_DEBUG != NULL) {
        if (pSL_DEBUG[0] == '0') {
            /* disable everything */
            traceMask = GT_TraceState_Disable | GT_TraceSetFailure_Disable |
                    GT_TraceEnter_Disable;
        }
        else if (pSL_DEBUG[0] == '1') {
            traceMask = GT_TraceState_Enable | GT_TraceSetFailure_Enable |
                    (1 << (32 - GT_TRACECLASS_SHIFT));
        }
        else if (pSL_DEBUG[0] == '2') {
            traceMask = GT_TraceState_Enable | GT_TraceSetFailure_Enable |
                    GT_TraceEnter_Enable | (2 << (32 - GT_TRACECLASS_SHIFT));
        }
        else if (pSL_DEBUG[0] == '3') {
            traceMask = GT_TraceState_Enable | GT_TraceSetFailure_Enable |
                    GT_TraceEnter_Enable | (3 << (32 - GT_TRACECLASS_SHIFT));
        }

        GT_setTrace(traceMask, GT_TraceType_User);
    }
#endif

    GT_0trace(curTrace, GT_ENTER, "SysLink_setup");

    /* Initialize the MemoryOS module */
    MemoryOS_setup();

#if defined(SYSLINK_BUILDOS_QNX)
    Cache_setup();
#endif

    /* Get config for the Ipc */
    Ipc_getConfig(&config);

    /* Setup the Ipc */
    Ipc_setup(&config);

#if defined (SYSLINK_TRACE_ENABLE)
    /* now that the driver is initialized, initialize its tracing, too */
    if (pSL_DEBUG != NULL) {
        GT_setTrace(traceMask, GT_TraceType_Kernel);
    }
#endif

    GT_0trace(curTrace, GT_LEAVE, "SysLink_setup");
}


/* Function to finalize SysLink. */
Void SysLink_destroy (Void)
{
    GT_0trace(curTrace, GT_ENTER, "SysLink_destroy");

    /* Destroy the Ipc */
    Ipc_destroy();

#if defined(SYSLINK_BUILDOS_QNX)
    Cache_destroy();
#endif

    /* Finalize the MemoryOS module */
    MemoryOS_destroy();

    GT_0trace(curTrace, GT_LEAVE, "SysLink_destroy");
}
