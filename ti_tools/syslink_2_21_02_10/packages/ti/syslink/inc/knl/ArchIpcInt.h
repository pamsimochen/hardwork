/*
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
 */

/**
 *  @file   ti/syslink/inc/knl/ArchIpcInt.h
 *
 *  @brief      Interface for Interrupt based architecture specific
 *              implementations
 */

#if !defined (ARCHIPCINT_INTERRUPT_H)
#define ARCHIPCINT_INTERRUPT_H


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    ARCHIPCINT_MODULEID
 *  @brief  Module ID for Notify.
 */
#define ARCHIPCINT_MODULEID           (UInt16) 0x5f85


/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */

/*!
 *  @def    ARCHIPCINT_STATUSCODEBASE
 *  @brief  Status code base for ARCHIPCINT module.
 */
#define ARCHIPCINT_STATUSCODEBASE    (ARCHIPCINT_MODULEID << 12u)

/*!
 *  @def    ARCHIPCINT_MAKE_FAILURE
 *  @brief  Macro to make error code.
 */
#define ARCHIPCINT_MAKE_FAILURE(x)    ((Int)(  0x80000000              \
                                    | (ARCHIPCINT_STATUSCODEBASE + (x))))

/*!
 *  @def    ARCHIPCINT_MAKE_SUCCESS
 *
 *  @brief  Macro to make success code.
 */
#define ARCHIPCINT_MAKE_SUCCESS(x)(ARCHIPCINT_STATUSCODEBASE +(x))

/*!
 *  @def    ARCHIPCINT_E_FAIL
 *  @brief  Generic failure.
 */
#define ARCHIPCINT_E_FAIL           ARCHIPCINT_MAKE_FAILURE(1)
/*!
 *  @def    ARCHIPCINT_SUCCESS
 *  @brief  Generic failure.
 */
#define ARCHIPCINT_SUCCESS           ARCHIPCINT_MAKE_SUCCESS(0)

/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* register to receive the given interrupt */
Int32 ArchIpcInt_interruptRegister(UInt16 procId, UInt32 intId,
        ArchIpcInt_CallbackFxn fxn, Ptr fxnArgs);

/* unregister the isr for the given interrupt */
Int32 ArchIpcInt_interruptUnregister(UInt16 procId, UInt32 intId, Ptr fxnArgs);

/* enable a registered interrupt (for receiving) */
Void ArchIpcInt_interruptEnable(UInt16 procId, UInt32 intId);

/* disable a registered interrupt */
Void ArchIpcInt_interruptDisable(UInt16 procId, UInt32 intId);

/* busy wait for remote processor to clear its interrupt */
Int32 ArchIpcInt_waitClearInterrupt(UInt16 procId, UInt32 intId);

/* raise the given interrupt to the remote processor */
Int32 ArchIpcInt_sendInterrupt(UInt16 procId, UInt32 intId, UInt32 value);

/* clear local interrupt */
UInt32 ArchIpcInt_clearInterrupt(UInt16 procId, UInt32 intId);


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (ARCHIPCINT_INTERRUPT_H) */
