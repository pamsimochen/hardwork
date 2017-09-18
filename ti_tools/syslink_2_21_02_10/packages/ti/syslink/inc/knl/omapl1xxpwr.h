/** 
 *  @file   omapl1xxpwr.h
 *
 *  @brief      Power Manager interface for OMAPL1XXPWR.
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



#ifndef OMAPL1XXPWR_H_0xa860
#define OMAPL1XXPWR_H_0xa860


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    OMAPL1XXPWR_MODULEID
 *  @brief  Module ID for OMAPL1XXPWR.
 */
#define OMAPL1XXPWR_MODULEID           (UInt16) 0xa860

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @def    OMAPL1XXPWR_STATUSCODEBASE
 *  @brief  Error code base for OMAPL1XXPWR.
 */
#define OMAPL1XXPWR_STATUSCODEBASE      (OMAPL1XXPWR_MODULEID << 12u)

/*!
 *  @def    OMAPL1XXPWR_MAKE_FAILURE
 *  @brief  Macro to make failure code.
 */
#define OMAPL1XXPWR_MAKE_FAILURE(x)    ((Int)(  0x80000000                    \
                                         | (OMAPL1XXPWR_STATUSCODEBASE + (x))))

/*!
 *  @def    OMAPL1XXPWR_MAKE_SUCCESS
 *  @brief  Macro to make success code.
 */
#define OMAPL1XXPWR_MAKE_SUCCESS(x)    (OMAPL1XXPWR_STATUSCODEBASE + (x))

/*!
 *  @def    OMAPL1XXPWR_E_OSFAILURE
 *  @brief  Failure in an OS-specific operation.
 */
#define OMAPL1XXPWR_E_OSFAILURE        OMAPL1XXPWR_MAKE_FAILURE(1)

/*!
 *  @def    OMAPL1XXPWR_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define OMAPL1XXPWR_E_INVALIDARG       OMAPL1XXPWR_MAKE_FAILURE(2)

/*!
 *  @def    OMAPL1XXPWR_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define OMAPL1XXPWR_E_MEMORY           OMAPL1XXPWR_MAKE_FAILURE(3)

/*!
 *  @def    OMAPL1XXPWR_E_HANDLE
 *  @brief  Invalid object handle specified
 */
#define OMAPL1XXPWR_E_HANDLE           OMAPL1XXPWR_MAKE_FAILURE(4)

/*!
 *  @def    OMAPL1XXPWR_E_ACCESSDENIED
 *  @brief  The operation is not permitted in this process.
 */
#define OMAPL1XXPWR_E_ACCESSDENIED     OMAPL1XXPWR_MAKE_FAILURE(5)

/*!
 *  @def    OMAPL1XXPWR_E_FAIL
 *  @brief  Generic failure.
 */
#define OMAPL1XXPWR_E_FAIL             OMAPL1XXPWR_MAKE_FAILURE(6)

/*!
 *  @def    OMAPL1XXPWR_E_INVALIDSTATE
 *  @brief  Module is in invalid state.
 */
#define OMAPL1XXPWR_E_INVALIDSTATE     OMAPL1XXPWR_MAKE_FAILURE(7)

/*!
 *  @def    OMAPL1XXPWR_SUCCESS
 *  @brief  Operation successful.
 */
#define OMAPL1XXPWR_SUCCESS            OMAPL1XXPWR_MAKE_SUCCESS(0)

/*!
 *  @def    OMAPL1XXPWR_S_ALREADYSETUP
 *  @brief  The OMAPL1XXPWR module has already been setup in this process.
 */
#define OMAPL1XXPWR_S_ALREADYSETUP     OMAPL1XXPWR_MAKE_SUCCESS(1)

/*!
 *  @def    OMAPL1XXPWR_S_OPENHANDLE
 *  @brief  Other OMAPL1XXPWR clients have still setup the OMAPL1XXPWR module.
 */
#define OMAPL1XXPWR_S_SETUP            OMAPL1XXPWR_MAKE_SUCCESS(2)

/*!
 *  @def    OMAPL1XXPWR_S_OPENHANDLE
 *  @brief  Other OMAPL1XXPWR handles are still open in this process.
 */
#define OMAPL1XXPWR_S_OPENHANDLE       OMAPL1XXPWR_MAKE_SUCCESS(3)

/*!
 *  @def    OMAPL1XXPWR_S_ALREADYEXISTS
 *  @brief  The OMAPL1XXPWR instance has already been created/opened in this
 *          process
 */
#define OMAPL1XXPWR_S_ALREADYEXISTS    OMAPL1XXPWR_MAKE_SUCCESS(4)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct OMAPL1XXPWR_Config {
    UInt32 reserved; /*!< Reserved field (not currently required) */
} OMAPL1XXPWR_Config;

/*!
 *  @brief  Configuration parameters specific to this processor.
 */
typedef struct OMAPL1XXPWR_Params_tag {
    UInt32 reserved; /*!< Reserved field (not currently required) */
} OMAPL1XXPWR_Params;

/*!
 *  @brief  Defines OMAPL1XXPWR object handle
 */
typedef struct OMAPL1XXPWR_Object * OMAPL1XXPWR_Handle;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Function to get the default configuration for the OMAPL1XXPWR module. */
Void OMAPL1XXPWR_getConfig (OMAPL1XXPWR_Config * cfg);

/* Function to setup the OMAPL1XXPWR module. */
Int OMAPL1XXPWR_setup (OMAPL1XXPWR_Config * cfg);

/* Function to destroy the OMAPL1XXPWR module. */
Int OMAPL1XXPWR_destroy (Void);

/* Function to initialize the parameters for this PwrMgr instance. */
Void OMAPL1XXPWR_Params_init (OMAPL1XXPWR_Handle   handle,
                              OMAPL1XXPWR_Params * params);

/* Function to create an instance of this PwrMgr. */
OMAPL1XXPWR_Handle OMAPL1XXPWR_create (       UInt16               procId,
                                       const  OMAPL1XXPWR_Params * params);

/* Function to delete an instance of this PwrMgr. */
Int OMAPL1XXPWR_delete (OMAPL1XXPWR_Handle * handlePtr);

/* Function to open an instance of this PwrMgr. */
Int OMAPL1XXPWR_open (OMAPL1XXPWR_Handle * handlePtr, UInt16 procId);

/* Function to close an instance of this PwrMgr. */
Int OMAPL1XXPWR_close (OMAPL1XXPWR_Handle * handlePtr);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* OMAPL1XXPWR_H_0xa860 */
