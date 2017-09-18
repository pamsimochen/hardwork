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
 *  @file   OsalDriver.h
 *
 *  @brief      Declarations of OS-specific functionality for NotifyDrv
 *
 *              This file contains declarations of OS-specific functions for
 *              NotifyDrv.
 */

#ifndef OSALDRIVER_H_0x010D
#define OSALDRIVER_H_0x010D

/* OSAL and utils */
#include <ti/syslink/utils/List.h>

/* Linux specific header files */
#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    OSALDRIVER_MODULEID
 *  @brief  Module ID for OsalDriver OSAL module.
 */
#define OSALDRIVER_MODULEID                 (UInt16) 0x010D

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
* @brief Stauts code base for MEMORY module.
*/
#define OSALDRIVER_STATUSCODEBASE      (OSALDRIVER_MODULEID << 12u)

/*!
 * @brief Convert to failure code.
 */
#define OSALDRIVER_MAKE_FAILURE(x)    ((Int) (0x80000000  \
                                       + (OSALDRIVER_STATUSCODEBASE + (x))))
/*!
 * @brief Convert to success code.
 */
#define OSALDRIVER_MAKE_SUCCESS(x)      (OSALDRIVER_STATUSCODEBASE + (x))

/*!
 * @brief Indicates OsalDriver alloc/free failure.
 */
#define OSALDRIVER_E_MEMORY             OSALDRIVER_MAKE_FAILURE(1)

/*!
 * @brief Invalid argument provided
 */
#define OSALDRIVER_E_INVALIDARG         OSALDRIVER_MAKE_FAILURE(2)

/*!
 * @brief Generic failure
 */
#define OSALDRIVER_E_FAIL               OSALDRIVER_MAKE_FAILURE(3)

/*!
 * @brief Indicates that specified value is out of range
 */
#define OSALDRIVER_E_OUTOFRANGE         OSALDRIVER_MAKE_FAILURE(4)

/*!
 *  @brief  Failure to open the file.
 */
#define OSALDRIVER_E_FILEOPEN           OSALDRIVER_MAKE_FAILURE(5)

/*!
 * @brief Module is invalidstate
 */
#define OSALDRIVER_E_INVALIDSTATE       OSALDRIVER_MAKE_FAILURE(6)

/*!
 * @brief Operation successfully completed
 */
#define OSALDRIVER_SUCCESS              OSALDRIVER_MAKE_SUCCESS(0)


/* =============================================================================
 *  Structure & Enums
 * =============================================================================
 */
/*!
 *  @brief   Kernel Driver object structure definition.
 */
#if 0
typedef struct OsalDriver_Object {
    List_Elem              elem;
    /*!< Used for creating a linkedlist */
    struct file_operations * fops;
    /*!< File operations function table */
    String                   name;
    /*!< Name of the driver */
    UInt32                   minor;
    /*!< Minor number of the driver */
    struct cdev              cdev;
    /*!< Character device structure */
} OsalDriver_Object;

/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Declaration for the OsalDriver object handle.
 *          Definition of OsalDriver_Object is not exposed.
 */
typedef struct OsalDriver_Object * OsalDriver_Handle;

/*!
 *  @brief  Handler for the driver IOCTL commands.
 */
typedef Int (*OsalDriver_HandlerFxn) (UInt32 cmdId, Ptr * args);
#endif

/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Initialize the Driver module. */
Int32 OsalDriver_setup (Void);

/* Finalize the Driver module */
Int32 OsalDriver_destroy (Void);

#if 0
/* function to register a driver with the OS kernel */
OsalDriver_Handle
OsalDriver_registerDriver (String                   name,
                           struct file_operations * fops,
                           UInt32                   minor);

/* function to unregister a driver */
Int32
OsalDriver_unregisterDriver (OsalDriver_Handle * drvHandle);
#endif

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* ifndef OSALDRIVER_H_0x010D */
