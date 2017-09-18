/** 
 *  @file   omap3530proc.h
 *
 *  @brief      Processor interface for OMAP3530SLAVE.
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



#ifndef OMAP3530PROC_H_0xbbec
#define OMAP3530PROC_H_0xbbec


/* Module headers */
#include <ti/syslink/ProcMgr.h>
#include "ProcDefs.h"


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    OMAP3530PROC_MODULEID
 *  @brief  Module ID for OMAP3530SLAVE.
 */
#define OMAP3530PROC_MODULEID           (UInt16) 0xbbec

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @def    OMAP3530PROC_STATUSCODEBASE
 *  @brief  Error code base for ProcMgr.
 */
#define OMAP3530PROC_STATUSCODEBASE      (OMAP3530PROC_MODULEID << 12u)

/*!
 *  @def    OMAP3530PROC_MAKE_FAILURE
 *  @brief  Macro to make failure code.
 */
#define OMAP3530PROC_MAKE_FAILURE(x)    ((Int)(  0x80000000                    \
                                         | (OMAP3530PROC_STATUSCODEBASE + (x))))

/*!
 *  @def    OMAP3530PROC_MAKE_SUCCESS
 *  @brief  Macro to make success code.
 */
#define OMAP3530PROC_MAKE_SUCCESS(x)    (OMAP3530PROC_STATUSCODEBASE + (x))

/*!
 *  @def    OMAP3530PROC_E_MMUENTRYEXISTS
 *  @brief  Specified MMU entry already exists.
 */
#define OMAP3530PROC_E_MMUENTRYEXISTS   OMAP3530PROC_MAKE_FAILURE(1)

/*!
 *  @def    OMAP3530PROC_E_ISR
 *  @brief  Error occurred during ISR operation.
 */
#define OMAP3530PROC_E_ISR              OMAP3530PROC_MAKE_FAILURE(2)

/*!
 *  @def    OMAP3530PROC_E_MMUCONFIG
 *  @brief  Error occurred during MMU configuration
 */
#define OMAP3530PROC_E_MMUCONFIG        OMAP3530PROC_MAKE_FAILURE(3)

/*!
 *  @def    OMAP3530PROC_E_OSFAILURE
 *  @brief  Failure in an OS-specific operation.
 */
#define OMAP3530PROC_E_OSFAILURE        OMAP3530PROC_MAKE_FAILURE(4)

/*!
 *  @def    OMAP3530PROC_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define OMAP3530PROC_E_INVALIDARG       OMAP3530PROC_MAKE_FAILURE(5)

/*!
 *  @def    OMAP3530PROC_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define OMAP3530PROC_E_MEMORY           OMAP3530PROC_MAKE_FAILURE(6)

/*!
 *  @def    OMAP3530PROC_E_HANDLE
 *  @brief  Invalid object handle specified
 */
#define OMAP3530PROC_E_HANDLE           OMAP3530PROC_MAKE_FAILURE(7)

/*!
 *  @def    OMAP3530PROC_E_ACCESSDENIED
 *  @brief  The operation is not permitted in this process.
 */
#define OMAP3530PROC_E_ACCESSDENIED     OMAP3530PROC_MAKE_FAILURE(8)

/*!
 *  @def    OMAP3530PROC_E_FAIL
 *  @brief  Generic failure.
 */
#define OMAP3530PROC_E_FAIL             OMAP3530PROC_MAKE_FAILURE(9)

/*!
 *  @def    OMAP3530PROC_E_INVALIDSTATE
 *  @brief  Module is in invalid state.
 */
#define OMAP3530PROC_E_INVALIDSTATE     OMAP3530PROC_MAKE_FAILURE(0xA)

/*!
 *  @def    OMAP3530PROC_SUCCESS
 *  @brief  Operation successful.
 */
#define OMAP3530PROC_SUCCESS           OMAP3530PROC_MAKE_SUCCESS(0)

/*!
 *  @def    OMAP3530PROC_S_ALREADYSETUP
 *  @brief  The OMAP3530PROC module has already been setup in this process.
 */
#define OMAP3530PROC_S_ALREADYSETUP     OMAP3530PROC_MAKE_SUCCESS(1)

/*!
 *  @def    OMAP3530PROC_S_OPENHANDLE
 *  @brief  Other OMAP3530PROC clients have still setup the OMAP3530PROC module.
 */
#define OMAP3530PROC_S_SETUP            OMAP3530PROC_MAKE_SUCCESS(2)

/*!
 *  @def    OMAP3530PROC_S_OPENHANDLE
 *  @brief  Other OMAP3530PROC handles are still open in this process.
 */
#define OMAP3530PROC_S_OPENHANDLE       OMAP3530PROC_MAKE_SUCCESS(3)

/*!
 *  @def    OMAP3530PROC_S_ALREADYEXISTS
 *  @brief  The OMAP3530PROC instance has already been created/opened in this
 *          process
 */
#define OMAP3530PROC_S_ALREADYEXISTS    OMAP3530PROC_MAKE_SUCCESS(4)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct OMAP3530PROC_Config {
    UInt32 reserved; /*!< Reserved field (not currently required) */
} OMAP3530PROC_Config;

/*!
 *  @brief  Configuration parameters specific to this processor.
 */
typedef struct OMAP3530PROC_Params_tag {
    Bool                mmuEnable;
    /*!< Determines if mmu should be used (enabled) */
    UInt32              numMemEntries;
    /*!< Number of memory regions to be configured. */
    ProcMgr_AddrInfo    memEntries[ProcMgr_MAX_MEMORY_REGIONS];
    /*!< Array of information structures for memory regions to be configured. */
} OMAP3530PROC_Params;

/*!
 *  @brief  Defines OMAP3530PROC object handle
 */
typedef struct OMAP3530PROC_Object * OMAP3530PROC_Handle;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Function to get the default configuration for the OMAP3530PROC module. */
Void OMAP3530PROC_getConfig (OMAP3530PROC_Config * cfg);

/* Function to setup the OMAP3530PROC module. */
Int OMAP3530PROC_setup (OMAP3530PROC_Config * cfg);

/* Function to destroy the OMAP3530PROC module. */
Int OMAP3530PROC_destroy (Void);

/* Function to initialize the parameters for this processor instance. */
Void OMAP3530PROC_Params_init (OMAP3530PROC_Handle    handle,
                               OMAP3530PROC_Params *  params);

/* Function to create an instance of this processor. */
OMAP3530PROC_Handle OMAP3530PROC_create (      UInt16                procId,
                                         const OMAP3530PROC_Params * params);

/* Function to delete an instance of this processor. */
Int OMAP3530PROC_delete (OMAP3530PROC_Handle * handlePtr);

/* Function to open an instance of this processor. */
Int OMAP3530PROC_open (OMAP3530PROC_Handle * handlePtr, UInt16 procId);

/* Function to close an instance of this processor. */
Int OMAP3530PROC_close (OMAP3530PROC_Handle * handlePtr);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* OMAP3530PROC_H_0xbbec */
