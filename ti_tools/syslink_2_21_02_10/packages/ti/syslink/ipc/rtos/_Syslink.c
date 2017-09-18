/*
 *  @file   _Syslink.c
 *
 *  @brief      Created to consolidate module initialization and finalization
 *
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



/*-------------------------    XDC  specific includes ----------------------  */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/syslink/SysLink.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/_RingIOShm.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>

#if defined(USE_PROCMGR)
#include <ti/syslink/inc/knl/Platform.h>
#endif

/* =============================================================================
 * APIs
 * =============================================================================
 */

/*
 *  ======== SysLink_setup ========
 *  Function to initialize SysLink.
 */
Void SysLink_setup (Void)
{
    Int32                  status = 0;
    RingIO_Config          ringIOConfig;
    RingIOShm_Config       ringIOShmConfig;
    FrameQ_Config          frameQConfig;
    FrameQBufMgr_Config    frameQBufMgrConfig;
    ClientNotifyMgr_Config clientConfig;

    RingIO_getConfig(&ringIOConfig);
    status = RingIO_setup(&ringIOConfig);
    if (status < 0) {
       System_printf("ERROR: RingIO_setup Failed\n");
    }

    RingIOShm_getConfig(&ringIOShmConfig);
    status = RingIOShm_setup(&ringIOShmConfig);
    if (status < 0) {
       System_printf("ERROR: RingIOShm_getConfig Failed\n");
    }

    ClientNotifyMgr_getConfig(&clientConfig);
    status = ClientNotifyMgr_setup(&clientConfig);
    if (status < 0) {
       System_printf("ERROR: RingIO_setup Failed\n");
    }

    FrameQ_getConfig(&frameQConfig);
    status = FrameQ_setup(&frameQConfig);
    if (status < 0) {
       System_printf("ERROR: FrameQ_setup Failed\n");
    }

    FrameQBufMgr_getConfig(&frameQBufMgrConfig);
    status = FrameQBufMgr_setup(&frameQBufMgrConfig);
    if (status < 0) {
       System_printf("ERROR: FrameQBufMgr_setup Failed\n");
    }

#if defined(USE_PROCMGR)
    Platform_setup();
#endif
}

/*
 *  ======== SysLink_destroy ========
 *  Function to finalize SysLink.
 */
Void SysLink_destroy (Void)
{
    FrameQBufMgr_destroy();
    FrameQ_destroy();
    ClientNotifyMgr_destroy();
    RingIOShm_destroy();
    RingIO_destroy();
#if defined(USE_PROCMGR)
    Platform_destroy();
#endif

}
