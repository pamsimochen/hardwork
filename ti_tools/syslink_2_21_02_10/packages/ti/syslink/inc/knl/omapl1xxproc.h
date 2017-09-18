/** 
 *  @file   omapl1xxproc.h
 *
 *  @brief      Processor interface for OMAPL1XXPROC.
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



#ifndef OMAPL1XXPROC_H_0xbbec
#define OMAPL1XXPROC_H_0xbbec


/* Module headers */
#include <ti/syslink/ProcMgr.h>
#include "ProcDefs.h"


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    OMAPL1XXPROC_MODULEID
 *  @brief  Module ID for OMAPL1XXSLAVE.
 */
#define OMAPL1XXPROC_MODULEID           (UInt16) 0xbbec

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @def    OMAPL1XXPROC_STATUSCODEBASE
 *  @brief  Error code base for ProcMgr.
 */
#define OMAPL1XXPROC_STATUSCODEBASE      (OMAPL1XXPROC_MODULEID << 12u)

/*!
 *  @def    OMAPL1XXPROC_MAKE_FAILURE
 *  @brief  Macro to make failure code.
 */
#define OMAPL1XXPROC_MAKE_FAILURE(x)    ((Int)(  0x80000000                    \
                                         | (OMAPL1XXPROC_STATUSCODEBASE + (x))))

/*!
 *  @def    OMAPL1XXPROC_MAKE_SUCCESS
 *  @brief  Macro to make success code.
 */
#define OMAPL1XXPROC_MAKE_SUCCESS(x)    (OMAPL1XXPROC_STATUSCODEBASE + (x))

/*!
 *  @def    OMAPL1XXPROC_E_MMUENTRYEXISTS
 *  @brief  Specified MMU entry already exists.
 */
#define OMAPL1XXPROC_E_MMUENTRYEXISTS   OMAPL1XXPROC_MAKE_FAILURE(1)

/*!
 *  @def    OMAPL1XXPROC_E_ISR
 *  @brief  Error occurred during ISR operation.
 */
#define OMAPL1XXPROC_E_ISR              OMAPL1XXPROC_MAKE_FAILURE(2)

/*!
 *  @def    OMAPL1XXPROC_E_MMUCONFIG
 *  @brief  Error occurred during MMU configuration
 */
#define OMAPL1XXPROC_E_MMUCONFIG        OMAPL1XXPROC_MAKE_FAILURE(3)

/*!
 *  @def    OMAPL1XXPROC_E_OSFAILURE
 *  @brief  Failure in an OS-specific operation.
 */
#define OMAPL1XXPROC_E_OSFAILURE        OMAPL1XXPROC_MAKE_FAILURE(4)

/*!
 *  @def    OMAPL1XXPROC_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define OMAPL1XXPROC_E_INVALIDARG       OMAPL1XXPROC_MAKE_FAILURE(5)

/*!
 *  @def    OMAPL1XXPROC_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define OMAPL1XXPROC_E_MEMORY           OMAPL1XXPROC_MAKE_FAILURE(6)

/*!
 *  @def    OMAPL1XXPROC_E_HANDLE
 *  @brief  Invalid object handle specified
 */
#define OMAPL1XXPROC_E_HANDLE           OMAPL1XXPROC_MAKE_FAILURE(7)

/*!
 *  @def    OMAPL1XXPROC_E_ACCESSDENIED
 *  @brief  The operation is not permitted in this process.
 */
#define OMAPL1XXPROC_E_ACCESSDENIED     OMAPL1XXPROC_MAKE_FAILURE(8)

/*!
 *  @def    OMAPL1XXPROC_E_FAIL
 *  @brief  Generic failure.
 */
#define OMAPL1XXPROC_E_FAIL             OMAPL1XXPROC_MAKE_FAILURE(9)

/*!
 *  @def    OMAPL1XXPROC_E_INVALIDSTATE
 *  @brief  Module is in invalid state.
 */
#define OMAPL1XXPROC_E_INVALIDSTATE     OMAPL1XXPROC_MAKE_FAILURE(0xA)

/*!
 *  @def    OMAPL1XXPROC_SUCCESS
 *  @brief  Operation successful.
 */
#define OMAPL1XXPROC_SUCCESS           OMAPL1XXPROC_MAKE_SUCCESS(0)

/*!
 *  @def    OMAPL1XXPROC_S_ALREADYSETUP
 *  @brief  The OMAPL1XXPROC module has already been setup in this process.
 */
#define OMAPL1XXPROC_S_ALREADYSETUP     OMAPL1XXPROC_MAKE_SUCCESS(1)

/*!
 *  @def    OMAPL1XXPROC_S_OPENHANDLE
 *  @brief  Other OMAPL1XXPROC clients have still setup the OMAPL1XXPROC module.
 */
#define OMAPL1XXPROC_S_SETUP            OMAPL1XXPROC_MAKE_SUCCESS(2)

/*!
 *  @def    OMAPL1XXPROC_S_OPENHANDLE
 *  @brief  Other OMAPL1XXPROC handles are still open in this process.
 */
#define OMAPL1XXPROC_S_OPENHANDLE       OMAPL1XXPROC_MAKE_SUCCESS(3)

/*!
 *  @def    OMAPL1XXPROC_S_ALREADYEXISTS
 *  @brief  The OMAPL1XXPROC instance has already been created/opened in this
 *          process
 */
#define OMAPL1XXPROC_S_ALREADYEXISTS    OMAPL1XXPROC_MAKE_SUCCESS(4)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct OMAPL1XXPROC_Config {
    UInt32 reserved; /*!< Reserved field (not currently required) */
} OMAPL1XXPROC_Config;

/*!
 *  @brief  Configuration parameters specific to this processor.
 */
typedef struct OMAPL1XXPROC_Params_tag {
    UInt32              numMemEntries;
    /*!< Number of memory regions to be configured. */
    ProcMgr_AddrInfo    memEntries[ProcMgr_MAX_MEMORY_REGIONS];
    /*!< Array of information structures for memory regions to be configured. */
} OMAPL1XXPROC_Params;

/*!
 *  @brief  Defines OMAPL1XXPROC object handle
 */
typedef struct OMAPL1XXPROC_Object * OMAPL1XXPROC_Handle;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Function to get the default configuration for the OMAPL1XXPROC module. */
Void OMAPL1XXPROC_getConfig (OMAPL1XXPROC_Config * cfg);

/* Function to setup the OMAPL1XXPROC module. */
Int OMAPL1XXPROC_setup (OMAPL1XXPROC_Config * cfg);

/* Function to destroy the OMAPL1XXPROC module. */
Int OMAPL1XXPROC_destroy (Void);

/* Function to initialize the parameters for this processor instance. */
Void OMAPL1XXPROC_Params_init (OMAPL1XXPROC_Handle    handle,
                               OMAPL1XXPROC_Params *  params);

/* Function to create an instance of this processor. */
OMAPL1XXPROC_Handle OMAPL1XXPROC_create (      UInt16                procId,
                                         const OMAPL1XXPROC_Params * params);

/* Function to delete an instance of this processor. */
Int OMAPL1XXPROC_delete (OMAPL1XXPROC_Handle * handlePtr);

/* Function to open an instance of this processor. */
Int OMAPL1XXPROC_open (OMAPL1XXPROC_Handle * handlePtr, UInt16 procId);

/* Function to close an instance of this processor. */
Int OMAPL1XXPROC_close (OMAPL1XXPROC_Handle * handlePtr);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* OMAPL1XXPROC_H_0xbbec */
