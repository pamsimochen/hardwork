/** 
 *  @file   _Dm8168IpcInt.h
 *
 *  @brief      Internal header file for OMAP3530 DSP IPC interrupts
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




#if !defined (_DM8168IPCINT_H)
#define _DM8168IPCINT_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/* register to receive the given interrupt */
Int32 Dm8168IpcInt_interruptRegister(UInt16 procId, UInt32 intId,
        ArchIpcInt_CallbackFxn fxn, Ptr fxnArgs);

/* unregister the isr for the given interrupt */
Int32 Dm8168IpcInt_interruptUnregister(UInt16 procId, UInt32 intId,
        Ptr fxnArgs);

/* enable a registered interrupt (for receiving) */
Void Dm8168IpcInt_interruptEnable(UInt16 procId, UInt32 intId);

/* disable a registered interrupt */
Void Dm8168IpcInt_interruptDisable(UInt16 procId, UInt32 intId);

/* busy wait for remote processor to clear its interrupt */
Int32 Dm8168IpcInt_waitClearInterrupt(UInt16 procId, UInt32 intId);

/* raise the given interrupt to the remote processor */
Int32 Dm8168IpcInt_sendInterrupt(UInt16 procId, UInt32 intId, UInt32 value);

/* clear local interrupt */
UInt32 Dm8168IpcInt_clearInterrupt(UInt16 procId, UInt32 intId);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (DM8168IPCINT_H) */
