/** 
 *  @file   Omap3530IpcInt.h
 *
 *  @brief      Header file for OMAP3530 DSP IPC interrupts
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




#if !defined (OMAP3530IPCINT_H)
#define OMAP3530IPCINT_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*!
 *  @def    OMAP3530IPCINT_MODULEID
 *  @brief  Module ID for Notify.
 */
#define OMAP3530IPCINT_MODULEID           (UInt16) 0x5f85


/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */

/*!
 *  @def    OMAP3530IPCINT_STATUSCODEBASE
 *  @brief  Status code base for OMAP3530IPCINT module.
 */
#define OMAP3530IPCINT_STATUSCODEBASE    (OMAP3530IPCINT_MODULEID << 12u)

/*!
 *  @def    OMAP3530IPCINT_MAKE_FAILURE
 *  @brief  Macro to make error code.
 */
#define OMAP3530IPCINT_MAKE_FAILURE(x)    ((Int)(  0x80000000              \
                                    | (OMAP3530IPCINT_STATUSCODEBASE + (x))))

/*!
 *  @def    OMAP3530IPCINT_MAKE_SUCCESS
 *
 *  @brief  Macro to make success code.
 */
#define OMAP3530IPCINT_MAKE_SUCCESS(x)(OMAP3530IPCINT_STATUSCODEBASE +(x))

/*!
 *  @def    OMAP3530IPCINT_E_FAIL
 *  @brief  Generic failure.
 */
#define OMAP3530IPCINT_E_FAIL              OMAP3530IPCINT_MAKE_FAILURE(1)

/*!
 *  @def    OMAP3530IPCINT_E_INVALIDSTATE
 *  @brief  Generic failure.
 */
#define OMAP3530IPCINT_E_INVALIDSTATE      OMAP3530IPCINT_MAKE_FAILURE(2)
/*!
 *  @def    OMAP3530IPCINT_SUCCESS
 *  @brief  Generic failure.
 */
#define OMAP3530IPCINT_SUCCESS             OMAP3530IPCINT_MAKE_SUCCESS(0)
/*!
 *  @def    OMAP3530IPCINT_S_ALREADYSETUP
 *  @brief  Set up already called.
 */
#define OMAP3530IPCINT_S_ALREADYSETUP      OMAP3530IPCINT_MAKE_SUCCESS(1)

/*!
 *  @def    OMAP3530IPCINT_S_ALREADYREGISTERED
 *  @brief  ISR already registered.
 */
#define OMAP3530IPCINT_S_ALREADYREGISTERED OMAP3530IPCINT_MAKE_SUCCESS(2)
/* =============================================================================
 * Structures and enums
 * =============================================================================
 */
typedef struct Omap3530IpcInt_Config_tag {
    UInt16    procId;
    /*!< Processor id of destination processor. */
    UInt32    recvIntId;
    /* recevive interrupt id */
} Omap3530IpcInt_Config ;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to setup interrupts for omap3530 */
Void Omap3530IpcInt_setup (Omap3530IpcInt_Config * cfg);

/* Function to destroy interrupt setup for omap3530 */
Void Omap3530IpcInt_destroy (Void);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (OMAP3530IPCINT_H) */
