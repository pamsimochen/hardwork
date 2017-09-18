/*
 *  @file   Dm8168IpcIntDrv.c
 *
 *  @brief      OMAP3530 DSP IPC interrupts.
 *              Defines necessary functions for Interrupt Handling.
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



/*  Defined to include MACROS EXPORT_SYMBOL. This must be done before including
 *  module.h
 */
#if !defined (EXPORT_SYMTAB)
#define EXPORT_SYMTAB
#endif

/* Standard headers */
#include <ti/syslink/Std.h>

/* Hardware Abstraction Layer */
#include <ti/syslink/inc/knl/Dm8168IpcInt.h>

/** ============================================================================
 *  Export Dm8168IpcInt functions
 *  ============================================================================
 */
EXPORT_SYMBOL(Dm8168IpcInt_setup);
EXPORT_SYMBOL(Dm8168IpcInt_destroy);


#if defined  (SYSLINK_INT_LOGGING)
extern UInt32 SysLogging_intCount;
extern UInt32 SysLogging_checkAndClearCount;
extern UInt32 SysLogging_isrCount;
extern UInt32 SysLogging_threadCount;
extern UInt32 SysLogging_NotifyCallbackCount;

EXPORT_SYMBOL(SysLogging_intCount);
EXPORT_SYMBOL(SysLogging_checkAndClearCount);
EXPORT_SYMBOL(SysLogging_isrCount);
EXPORT_SYMBOL(SysLogging_threadCount);
EXPORT_SYMBOL(SysLogging_NotifyCallbackCount);
#endif /* if defined  (SYSLINK_INT_LOGGING) */
