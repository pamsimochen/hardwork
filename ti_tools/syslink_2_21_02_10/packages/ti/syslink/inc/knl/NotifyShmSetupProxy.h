/** 
 *  @file   NotifyShmSetupProxy.h
 *
 *  @brief      Proxy to connect Notify setup to device specific implementation
 *
 *
 */
/* 
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


#if !defined (_NOTIFYSHMSETUPPROXY_H)
#define _NOTIFYSHMSETUPPROXY_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Defaults to be used for all devices */

#if defined (SYSLINK_PLATFORM_OMAPL1XX)
/* Function that will be called in Notify_attach */
extern Int NotifySetupOmapl1xx_attach (UInt16 procId, Ptr sharedAddr);
#define Notify_SetupProxy_attach(procId, sharedAddr) NotifySetupOmapl1xx_attach(procId, sharedAddr)

/* Function that will be called in Notify_detach */
extern Int NotifySetupOmapl1xx_detach (UInt16 procId);
#define Notify_SetupProxy_detach NotifySetupOmapl1xx_detach

/* Shared Memory Required for Notify setup */
extern SizeT NotifySetupOmapl1xx_sharedMemReq (UInt16 procId, Ptr sharedAddr);
#define Notify_SetupProxy_sharedMemReq(procId, sharedAddr) NotifySetupOmapl1xx_sharedMemReq(procId, sharedAddr)

/* Is interrupt line available? */
extern Bool NotifySetupOmapl1xx_intLineAvailable(UInt16 remoteProcId);
#define Notify_SetupProxy_intLineAvailable(remoteProcId) NotifySetupOmapl1xx_intLineAvailable(remoteProcId)

/* numIntLines? */
extern UInt16 NotifySetupOmapl1xx_numIntLines(UInt16 remoteProcId);
#define Notify_SetupProxy_numIntLines(remoteProcId) NotifySetupOmapl1xx_numIntLines(remoteProcId)

#elif defined (SYSLINK_PLATFORM_OMAP3530)
/* Function that will be called in Notify_start */
extern Int NotifySetupOmap3530_attach (UInt16 procId, Ptr sharedAddr);
#define Notify_SetupProxy_attach(procId, sharedAddr) NotifySetupOmap3530_attach(procId, sharedAddr)

/* Function that will be called in Notify_stop */
extern Int NotifySetupOmap3530_detach (UInt16 procId);
#define Notify_SetupProxy_detach NotifySetupOmap3530_detach

/* Shared Memory Required for Notify setup */
extern SizeT NotifySetupOmap3530_sharedMemReq (UInt16 procId, Ptr sharedAddr);
#define Notify_SetupProxy_sharedMemReq(procId, sharedAddr) NotifySetupOmap3530_sharedMemReq(procId, sharedAddr)

/* Is interrupt line available? */
extern Bool NotifySetupOmap3530_intLineAvailable(UInt16 remoteProcId);
#define Notify_SetupProxy_intLineAvailable(remoteProcId) NotifySetupOmap3530_intLineAvailable(remoteProcId)

/* numIntLines? */
extern UInt16 NotifySetupOmap3530_numIntLines(UInt16 remoteProcId);
#define Notify_SetupProxy_numIntLines(remoteProcId) NotifySetupOmap3530_numIntLines(remoteProcId)

#elif defined (SYSLINK_PLATFORM_TI81XX)
extern Int NotifySetupDm8168_attach (UInt16 procId, Ptr sharedAddr);
#define Notify_SetupProxy_attach(procId, sharedAddr) NotifySetupDm8168_attach(procId, sharedAddr)

/* Function that will be called in Notify_stop */
extern Int NotifySetupDm8168_detach (UInt16 procId);
#define Notify_SetupProxy_detach NotifySetupDm8168_detach

/* Shared Memory Required for Notify setup */
extern SizeT NotifySetupDm8168_sharedMemReq (UInt16 procId, Ptr sharedAddr);
#define Notify_SetupProxy_sharedMemReq(procId, sharedAddr) NotifySetupDm8168_sharedMemReq(procId, sharedAddr)

/* Is interrupt line available? */
extern Bool NotifySetupDm8168_intLineAvailable(UInt16 remoteProcId);
#define Notify_SetupProxy_intLineAvailable(remoteProcId) NotifySetupDm8168_intLineAvailable(remoteProcId)

/* numIntLines? */
extern UInt16 NotifySetupDm8168_numIntLines(UInt16 remoteProcId);
#define Notify_SetupProxy_numIntLines(remoteProcId) NotifySetupDm8168_numIntLines(remoteProcId)

#else
#error Unknown platform
#endif

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif  /* !defined (_NOTIFYSHMSETUPPROXY_H) */
