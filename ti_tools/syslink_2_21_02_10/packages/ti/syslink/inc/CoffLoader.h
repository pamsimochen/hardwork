/** 
 *  @file   CoffLoader.h
 *
 *  @brief      Loader interface for COFF format.
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



#ifndef CoffLoader_H_0x1bc4
#define CoffLoader_H_0x1bc4


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    COFFLOADER_MODULEID
 *  @brief  Module ID for OMAP3530PWR.
 */
#define COFFLOADER_MODULEID           (UInt16) 0x1bc4

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */

/*!
 *  @def    COFFLOADER_STATUSCODEBASE
 *  @brief  Status code base for COFF loader.
 */
#define COFFLOADER_STATUSCODEBASE      (COFFLOADER_MODULEID << 12u)

/*!
 *  @def    COFFLOADER_MAKE_FAILURE
 *  @brief  Macro to make failure code.
 */
#define COFFLOADER_MAKE_FAILURE(x) ((Int)(  0x80000000                         \
                                          | (COFFLOADER_STATUSCODEBASE + (x))))

/*!
 *  @def    COFFLOADER_MAKE_SUCCESS
 *  @brief  Macro to make success code.
 */
#define COFFLOADER_MAKE_SUCCESS(x)    (COFFLOADER_STATUSCODEBASE + (x))

/*!
 *  @def    COFFLOADER_E_SIZE
 *  @brief  An invalid size is specified
 */
#define COFFLOADER_E_SIZE             COFFLOADER_MAKE_FAILURE(1)

/*!
 *  @def    COFFLOADER_E_FILEPARSE
 *  @brief  COFF file parsing error
 */
#define COFFLOADER_E_FILEPARSE        COFFLOADER_MAKE_FAILURE(2)

/*!
 *  @def    COFFLOADER_E_RANGE
 *  @brief  A value is out of range
 */
#define COFFLOADER_E_RANGE            COFFLOADER_MAKE_FAILURE(3)

/*!
 *  @def    COFFLOADER_E_OSFAILURE
 *  @brief  Failure in an OS-specific operation.
 */
#define COFFLOADER_E_OSFAILURE        COFFLOADER_MAKE_FAILURE(4)

/*!
 *  @def    COFFLOADER_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define COFFLOADER_E_INVALIDARG       COFFLOADER_MAKE_FAILURE(4)

/*!
 *  @def    COFFLOADER_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define COFFLOADER_E_MEMORY           COFFLOADER_MAKE_FAILURE(5)

/*!
 *  @def    COFFLOADER_E_HANDLE
 *  @brief  Invalid object handle specified
 */
#define COFFLOADER_E_HANDLE           COFFLOADER_MAKE_FAILURE(6)

/*!
 *  @def    COFFLOADER_E_ACCESSDENIED
 *  @brief  The operation is not permitted in this process.
 */
#define COFFLOADER_E_ACCESSDENIED     COFFLOADER_MAKE_FAILURE(7)

/*!
 *  @def    COFFLOADER_E_ACCESSDENIED
 *  @brief  The operation is not permitted in this process.
 */
#define COFFLOADER_E_FAIL             COFFLOADER_MAKE_FAILURE(8)

/*!
 *  @def    COFFLOADER_SUCCESS
 *  @brief  Operation successful.
 */
#define COFFLOADER_SUCCESS            COFFLOADER_MAKE_SUCCESS(0)

/*!
 *  @def    COFFLOADER_S_ALREADYSETUP
 *  @brief  The CoffLoader module has already been setup in this process.
 */
#define COFFLOADER_S_ALREADYSETUP     COFFLOADER_MAKE_SUCCESS(1)

/*!
 *  @def    COFFLOADER_S_OPENHANDLE
 *  @brief  Other CoffLoader clients have still setup the CoffLoader module.
 */
#define COFFLOADER_S_SETUP            COFFLOADER_MAKE_SUCCESS(2)

/*!
 *  @def    COFFLOADER_S_OPENHANDLE
 *  @brief  Other CoffLoader handles are still open in this process.
 */
#define COFFLOADER_S_OPENHANDLE       COFFLOADER_MAKE_SUCCESS(3)

/*!
 *  @def    COFFLOADER_S_ALREADYEXISTS
 *  @brief  The CoffLoader instance has already been created/opened in this
 *          process
 */
#define COFFLOADER_S_ALREADYEXISTS    COFFLOADER_MAKE_SUCCESS(4)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct CoffLoader_Config {
    UInt32 reserved;
    /*!< Reserved value. */
} CoffLoader_Config;

/*!
 *  @brief  Configuration parameters specific to this processor.
 */
typedef struct CoffLoader_Params_tag {
    UInt32 reserved;
    /*!< Reserved field (not currently required) */
} CoffLoader_Params;


/*!
 *  @brief  CoffLoader instance object.
 */
typedef struct CoffLoader_Object_tag {
    Ptr                     coffObject;
    /*!< Object used by the generic COFF parser. */
    Ptr                     coffLoaderObject;
    /*!< Internal object used by Coff Loader module. */
} CoffLoader_Object, *CoffLoader_Handle;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Function to get the default configuration for the CoffLoader module. */
Void CoffLoader_getConfig (CoffLoader_Config * cfg);

/* Function to setup the CoffLoader module. */
Int CoffLoader_setup (CoffLoader_Config * cfg);

/* Function to destroy the CoffLoader module. */
Int CoffLoader_destroy (Void);

/* Function to initialize the parameters for this loader instance. */
Void CoffLoader_Params_init (CoffLoader_Handle handle,
                             CoffLoader_Params * params);

/* Function to create an instance of this loader. */
CoffLoader_Handle CoffLoader_create (      UInt16              procId,
                                     const CoffLoader_Params * params);

/* Function to delete an instance of this loader. */
Int CoffLoader_delete (CoffLoader_Handle * handlePtr);

/* Function to open an instance of this loader. */
Int CoffLoader_open (CoffLoader_Handle * handlePtr, UInt16 procId);

/* Function to close an instance of this loader. */
Int CoffLoader_close (CoffLoader_Handle * handlePtr);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* CoffLoader_H_0x1bc4 */
