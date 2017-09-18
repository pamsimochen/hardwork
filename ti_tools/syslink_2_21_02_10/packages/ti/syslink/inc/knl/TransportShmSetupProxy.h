/** 
 *  @file   TransportShmSetupProxy.h
 *
 *  @brief      Proxy to connect MessageQ Transport setup functions to
 *              SHM implementation.
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



#if !defined (_TRANSPORTSHMSETUPPROXY_H_)
#define _TRANSPORTSHMSETUPPROXY_H_


/* Module headers */
#include "TransportShmSetup.h"


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Function that will be called in MessageQ_attach */
#define MessageQ_SetupTransportProxy_attach(remoteProcId, sharedAddr) TransportShmSetup_attach(remoteProcId, sharedAddr)

/* Function that will be called in MessageQ_detach */
#define MessageQ_SetupTransportProxy_detach(remoteProcId) TransportShmSetup_detach(remoteProcId)

/* Shared memory req function */
#define MessageQ_SetupTransportProxy_sharedMemReq(sharedAddr) TransportShmSetup_sharedMemReq(sharedAddr)

/* isRegistered function */
#define MessageQ_SetupTransportProxy_isRegistered(remoteProcId) TransportShmSetup_isRegistered(remoteProcId)


#if defined (__cplusplus)
}
#endif


#endif
