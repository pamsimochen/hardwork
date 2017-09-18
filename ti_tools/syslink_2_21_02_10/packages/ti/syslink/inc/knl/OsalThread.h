/** 
 *  @file   OsalThread.h
 *
 *  @brief      Kernel utils thread interface definitions.
 *
 *              This interface abstracts the kernel side thread
 *              implementation.
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



#ifndef OSALTHREAD_H_0x6833
#define OSALTHREAD_H_0x6833


/* OSAL and utils */


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @brief  Module ID for OsalThread OSAL module.
 */
#define OSALTHREAD_MODULEID                 (UInt16) 0x6833

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 * @brief Status code base for OsalThread module.
 */
#define OSALTHREAD_STATUSCODEBASE      (OSALTHREAD_MODULEID << 12u)

/*!
 * @brief Convert to failure code.
 */
#define OSALTHREAD_MAKE_FAILURE(x)    ((Int) (0x80000000  \
                                       + (OSALTHREAD_STATUSCODEBASE + (x))))
/*!
 * @brief Convert to success code.
 */
#define OSALTHREAD_MAKE_SUCCESS(x)      (OSALTHREAD_STATUSCODEBASE + (x))

/*!
 * @brief Indicates OsalThread alloc/free failure.
 */
#define OSALTHREAD_E_MEMORY             OSALTHREAD_MAKE_FAILURE(1)

/*!
 * @brief Invalid argument provided
 */
#define OSALTHREAD_E_INVALIDARG         OSALTHREAD_MAKE_FAILURE(2)

/*!
 * @brief Generic failure
 */
#define OSALTHREAD_E_FAIL               OSALTHREAD_MAKE_FAILURE(3)

/*!
 * @brief Operation successfully completed
 */
#define OSALTHREAD_SUCCESS              OSALTHREAD_MAKE_SUCCESS(0)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Declaration for the OsalThread object handle.
 *          Definition of OsalThread_Object is not exposed.
 */
typedef struct OsalThread_Object * OsalThread_Handle;

/*!
 *  @brief  Callback function type
 */
typedef Void (*OsalThread_CallbackFxn) (Ptr fxnArgs);


/*!
 *  @brief   Enumerates the different values of thread priorities.
 */
typedef enum {
    OsalThread_Priority_Low       = 0u,
    /*!< Low priority */
    OsalThread_Priority_Medium    = 1u,
    /*!< Medium priority */
    OsalThread_Priority_High      = 2u,
    /*!< High priority */
    OsalThread_Priority_EndValue  = 3u
    /*!< End delimiter indicating start of invalid values for this enum */
} OsalThread_Priority;

/*!
 *  @brief   Enumerates the types of thread priorities.
 */
typedef enum {
    OsalThread_PriorityType_Generic    = 0u,
    /*!< Generic priority is to be used for fully portable code */
    OsalThread_PriorityType_OsSpecific = 1u,
    /*!< OS specific priority is to be used for higher granularity */
    OsalThread_PriorityType_EndValue   = 2u
    /*!< End delimiter indicating start of invalid values for this enum */
} OsalThread_PriorityType;


/*!
 *  @brief   Creation params for an OsalThread
 */
typedef struct OsalThread_Params_tag {
    OsalThread_PriorityType  priorityType;
    /*!< Priority type to be used. */
    UInt32  priority;
    /*!< Thread priority to be used. Interpretation of this value depends on
         priority type specified in this structure. */
    Bool    once;
    /*!< Indicates whether thread function has to be executed once or in a loop
     */
} OsalThread_Params;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*!
 *  @brief  Initialize the thread module
 *
 *  @retval 0 Success
 *
 *  @sa     OsalThread_destroy()
 */
Int32 OsalThread_setup(Void);

/*!
 *  @brief  Finalize the thread module
 *
 *  @retval 0 Success
 *
 *  @sa     OsalThread_setup()
 */
Int32 OsalThread_destroy(Void);

/*!
 *  @brief  Create the thread.
 *
 *  @param  fxn Function which will be executed after the thread creation.
 *  @param  fxnArgs data which the function requires.
 *
 *  @sa     OsalThread_delete()
 */
OsalThread_Handle OsalThread_create(OsalThread_CallbackFxn fxn,
        Ptr fxnArgs, OsalThread_Params *params);

/*!
 *  @brief  Destroys the thread.
 *
 *  @param  threadHandle which needs to be destroyed.
 *
 *  @retval OSALTHREAD_SUCCESS
 *  @retval OSALTHREAD_E_INVALIDARG
 *  @retval OSALTHREAD_E_FAIL
 *
 *  @sa     OsalThread_create()
 */
Int OsalThread_delete(OsalThread_Handle * threadHandle);

/*!
 *  @brief Disables the thread.
 *
 *  @param threadHandle which needs to be disabled.
 *
 *  @sa    OsalThread_enableThread()
 */
Void OsalThread_disableThread(OsalThread_Handle threadHandle);

/*!
 *  @brief  Enables the thread.
 *
 *  @param  threadHandle which needs to be disabled.
 *
 *  @sa     OsalThread_disableThread
 */
Void OsalThread_enableThread(OsalThread_Handle threadHandle);

/*!
 *  @brief  Disables all threads created using this module.
 *
 *  @sa     OsalThread_enable()
 */
Void OsalThread_disable(Void);

/*!
 *  @brief  Enables all threads created using this module.
 *
 *  @sa     OsalThread_disable()
 */
Void OsalThread_enable(Void);

/*!
 *  @brief  Activate this thread.
 *
 *  @remark This function may gets invoked from ISR context.
 *
 *  @param  threadHandle the thread which needs to be activated
 *
 *  @sa     OsalThread_yield()
 */
Void OsalThread_activate (OsalThread_Handle threadHandle);

/*!
 *  @brief  Yield the processor
 *
 *  @param  threadHandle    Thread handle
 *
 *  @sa     OsalThread_activate()
 */
Void OsalThread_yield(OsalThread_Handle threadHandle);

/*!
 *  @brief   Sleep the current thread for specific time in milli-seconds.
 *
 *  @param   time       milliseconds to sleep
 *
 *  @sa      OsalThread_delay()
 */
Void OsalThread_sleep(UInt32 time);

/*!
 *  @brief   Delay this thread for specific time in milli-seconds.
 *
 *  @remarks Does not schedule out this thread.
 *
 *  @param   time       milliseconds to delay
 *
 *  @sa      OsalThread_delay
 */
Void OsalThread_delay(UInt32 time);

/*!
 *  @brief   Checks if current context is a thread context or ISR context.
 */
Bool OsalThread_inThread(Void);

/*!
 *  @brief   Wait for thread completion.
 *
 *  @param   threadHandle       Thread Handle
 */
Void OsalThread_waitForThread(OsalThread_Handle threadHandle);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* ifndef OSALTHREAD_H_0x6833 */
