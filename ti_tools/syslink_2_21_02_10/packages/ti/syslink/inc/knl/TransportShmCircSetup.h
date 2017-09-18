/** 
 *  @file   TransportShmCircSetup.h
 *
 *  @brief      Declares transport specific functions to setup the MessageQ
 *              Circular Buffer Transport module.
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



#ifndef TRANSPORTSHMCIRCSETUP_H_
#define TRANSPORTSHMCIRCSETUP_H_


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/*!
 *  @brief      Function that will be called in MessageQ_attach
 *
 *  @param      remoteProcId    Remote processor ID
 *  @param      sharedAddr      Shared memory address where the transport(s) are
 *                              to be created.
 *
 *  @sa         TransportShmCircSetup_detach
 */
Int TransportShmCircSetup_attach (UInt16 remoteProcId, Ptr sharedAddr);

/*!
 *  @brief      Function that will be called in MessageQ_detach
 *
 *  @param      remoteProcId    Remote processor ID
 *
 *  @sa         TransportShmCircSetup_detach
 */
Int TransportShmCircSetup_detach (UInt16 remoteProcId);

/*!
 *  @brief      Function that returns the amount of shared memory required
 *
 *  @param      sharedAddr      Shared memory address where the transport(s) are
 *                              to be created.
 *
 *  @sa         TransportShmCircSetup_attach
 */
SizeT TransportShmCircSetup_sharedMemReq (Ptr sharedAddr);

/*!
 *  @brief      Function that determines if a transport has been registered to a
 *              remote processor
 *
 *  @param      remoteProcId    Remote processor ID
 *
 *  @sa         TransportShmCircSetup_attach
 */
Bool TransportShmCircSetup_isRegistered (UInt16 remoteProcId);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* TRANSPORTSHMCIRCSETUP_H_ */


