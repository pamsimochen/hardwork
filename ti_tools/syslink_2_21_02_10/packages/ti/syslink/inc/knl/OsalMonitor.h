/** 
 *  @file   OsalMonitor.h
 *
 *  @brief      Kernel utils Monitor interface definitions.
 *
 *              This abstracts the Monitor interface in Kernel code and
 *              is implemented using the wait queues. It has interfaces
 *              for creating, destroying, waiting and triggering the Monitors.
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


#ifndef OSALMONITOR_H_0xF6D6
#define OSALMONITOR_H_0xF6D6


/* OSAL and utils */


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    OSALMONITOR_MODULEID
 *  @brief  Module ID for OsalMonitor OSAL module.
 */
#define OSALMONITOR_MODULEID                 (UInt16) 0xF6D6

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
* @def   OSALMONITOR_STATUSCODEBASE
* @brief Stauts code base for MEMORY module.
*/
#define OSALMONITOR_STATUSCODEBASE      (OSALMONITOR_MODULEID << 12u)

/*!
* @def   OSALMONITOR_MAKE_FAILURE
* @brief Convert to failure code.
*/
#define OSALMONITOR_MAKE_FAILURE(x)  ((Int) (0x80000000  \
                                        + (OSALMONITOR_STATUSCODEBASE +(x))))
/*!
* @def   OSALMONITOR_MAKE_SUCCESS
* @brief Convert to success code.
*/
#define OSALMONITOR_MAKE_SUCCESS(x)      (OSALMONITOR_STATUSCODEBASE + (x))

/*!
* @def   OSALMONITOR_E_MEMORY
* @brief Indicates OsalMonitor alloc/free failure.
*/
#define OSALMONITOR_E_MEMORY             OSALMONITOR_MAKE_FAILURE(1)

/*!
* @def   OSALMONITOR_E_INVALIDARG
* @brief Invalid argument provided
*/
#define OSALMONITOR_E_INVALIDARG         OSALMONITOR_MAKE_FAILURE(2)

/*!
* @def   OSALMONITOR_E_FAIL
* @brief Generic failure
*/
#define OSALMONITOR_E_FAIL               OSALMONITOR_MAKE_FAILURE(3)

/*!
* @def   OSALMONITOR_E_TIMEOUT
* @brief A timeout occurred
*/
#define OSALMONITOR_E_TIMEOUT            OSALMONITOR_MAKE_FAILURE(4)

/*!
 *  @def    OSALMONITOR_E_HANDLE
 *  @brief  Invalid handle provided
 */
#define OSALMONITOR_E_HANDLE             OSALMONITOR_MAKE_FAILURE(5)

/*!
 *  @def    OSALMONITOR_E_WAITNONE
 *  @brief  WAIT_NONE timeout value was provided, but semaphore was not
 *          available.
 */
#define OSALMONITOR_E_WAITNONE           OSALMONITOR_MAKE_FAILURE(6)

/*!
* @def   OSALMONITOR_SUCCESS
* @brief Operation successfully completed
*/
#define OSALMONITOR_SUCCESS              OSALMONITOR_MAKE_SUCCESS(0)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @def    OSALMONITOR_WAIT_FOREVER
 *  @brief  Indicates forever wait for APIs that can wait.
 */
#define OSALMONITOR_WAIT_FOREVER           (~((UInt32) 0u))

/*!
 *  @def    OSALMONITOR_WAIT_NONE
 *  @brief  Indicates zero wait for APIs that can wait.
 */
#define OSALMONITOR_WAIT_NONE               ((UInt32) 0u)

/*!
 *  @def    OSALMONITOR_TYPE_VALUE
 *  @brief  Returns the value of semaphore type (binary/counting)
 */
#define OSALMONITOR_TYPE_VALUE(type)        (type & 0x0000FFFF)

/*!
 *  @def    OSALMONITOR_INTTYPE_VALUE
 *  @brief  Returns the value of semaphore interruptability type
 */
#define OSALMONITOR_INTTYPE_VALUE(type)     (type & 0xFFFF0000)

/*!
 *  @brief  Declaration for the OsalMonitor object handle.
 *          Definition of OsalMonitor_Object is not exposed.
 */
typedef struct OsalMonitor_Object * OsalMonitor_Handle;

/*!
 *  @brief   Enumerates the types of semaphores
 */
typedef enum {
    OsalMonitor_Type_Binary          = 0x00000000,
    /*!< Binary semaphore */
    OsalMonitor_Type_Counting        = 0x00000001,
    /*!< Counting semaphore */
    OsalMonitor_Type_EndValue        = 0x00000002
    /*!< End delimiter indicating start of invalid values for this enum */
} OsalMonitor_Type;

/*!
 *  @brief   Enumerates the interruptible/non-interruptible types.
 */
typedef enum {
    OsalMonitor_IntType_Interruptible    = 0x00000000,
    /*!< Waits on this mutex are interruptible */
    OsalMonitor_IntType_Noninterruptible = 0x00010000,
    /*!< Waits on this mutex are non-interruptible */
    OsalMonitor_IntType_EndValue         = 0x00020000
    /*!< End delimiter indicating start of invalid values for this enum */
} OsalMonitor_IntType;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Creates the semaphore object. */
OsalMonitor_Handle OsalMonitor_create (OsalMonitor_Type semType);

/* Deletes the semaphore object */
Int OsalMonitor_delete (OsalMonitor_Handle * semHandle);

/* Wait on the said Monitor in the kernel thread context */
Int OsalMonitor_pend (OsalMonitor_Handle semHandle, UInt32 timeout);

/* Signal the semaphore and make it available for other threads. */
Int OsalMonitor_post (OsalMonitor_Handle semHandle);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* ifndef OSALMONITOR_H_0xF6D6 */
