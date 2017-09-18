/*
 *  @file   OsalMonitor.c
 *
 *  @brief      Linux Monitor interface implementation.
 *
 *              This abstracts the kernel side Spin Lock which is used in
 *              halting the CPU for some specified amout of time. this
 *              interface covers all aspects of Monitor required in Syslink.
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


/* Linux specific header files */
#include <linux/spinlock.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL and kernel utils */
#include <OsalMonitor.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief   This object is used by spinlock APIs.
 */
typedef struct OsalMonitor_Object_tag {
    spinlock_t lock;
    /* !< handle to lock object used by OS.*/
    UInt32     reserved;
    /* TBD: Needed otherwise size of object comes 0. */
} OsalMonitor_Object;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/*!
 *  @brief      Creates the Monitor Object.
 *
 *  @param      type    Type of spinlock (Normal or Raw)
 *
 *  @sa         OsalMonitor_delete
 */
OsalMonitor_Handle
OsalMonitor_create (OsalMonitor_Type type)
{
    OsalMonitor_Object * lockObject = NULL;

    GT_1trace (curTrace, GT_ENTER, "OsalMonitor_create", type);

    GT_assert (curTrace, (type < OsalMonitor_Type_EndValue));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (type >= OsalMonitor_Type_EndValue) {
        /*! @retval NULL Invalid type provided */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalMonitor_create",
                             OSALMONITOR_E_INVALIDARG,
                             "Invalid type provided");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        lockObject = Memory_alloc (NULL, sizeof (OsalMonitor_Object), 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (lockObject == NULL) {
            /*! @retval NULL Failed to allocate memory for spinlock object. */
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OsalMonitor_create",
                              OSALMONITOR_E_MEMORY,
                              "Failed to allocate memory for spinlock object.");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            spin_lock_init(&(lockObject->lock));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalMonitor_create", lockObject);

    /*! @retval Monitor-handle Operation successfully completed. */
    return (OsalMonitor_Handle) lockObject;
}

/*!
 *  @brief      Delete the Monitor Object.
 *
 *  @param      lockHandle Monitor object handle which needs to be deleted.
 *
 *  @sa         OsalMonitor_create
 */
Int
OsalMonitor_delete (OsalMonitor_Handle * lockHandle)
{
    Int                   status     = OSALMONITOR_SUCCESS;
    OsalMonitor_Object * lockObject = NULL;

    GT_1trace (curTrace, GT_ENTER, "OsalMonitor_delete", lockHandle);

    GT_assert (curTrace, (lockHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (lockHandle == NULL) {
        /*! @retval OSALMONITOR_E_INVALIDARG NULL provided for argument
                                              lockHandle.*/
        status = OSALMONITOR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalMonitor_delete",
                             status,
                             "NULL provided for argument lockHandle");
    }
    else if (*lockHandle == NULL) {
        /*! @retval OSALMONITOR_E_HANDLE NULL spinlock handle provided. */
        status = OSALMONITOR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalMonitor_delete",
                             status,
                             "NULL spinlock handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        lockObject = (OsalMonitor_Object*) (*lockHandle);
        Memory_free (NULL, lockObject, sizeof (OsalMonitor_Object));
        *lockHandle = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalMonitor_delete", status);

    /*! @retval OSALMONITOR_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief      Begin protection of code through spin lock with all ISRs
 *              disabled. Calling this API protects critical regions of code
 *              from preemption by tasks, DPCs and all interrupts.
 *
 *  @param      lockHandle   Monitor object handle to be acquired.
 *
 *  @sa         OsalMonitor_leave
 */
UInt32
OsalMonitor_enter (OsalMonitor_Handle lockHandle)
{
    UInt32 flags = 0;

    OsalMonitor_Object * lockObject = (OsalMonitor_Object*) lockHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalMonitor_enter", lockHandle);

    GT_assert (curTrace, (lockHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (lockHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalMonitor_enter",
                             OSALMONITOR_E_HANDLE,
                             "NULL spinlock handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        spin_lock (&(lockObject->lock));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalMonitor_enter", flags);

    /*! @retval Flags Flags to be provided to corresponding leave call. */
    return flags;
}


/*!
 *  @brief      End protection of code through spin lock with all ISRs disabled.
 *
 *  @param      lockHandle   Monitor object handle to be released.
 *  @param      key          Key received from the corresponding enter call.
 *
 *  @sa         OsalMonitor_enter
 */
Void
OsalMonitor_leave (OsalMonitor_Handle lockHandle, UInt32 key)
{
    OsalMonitor_Object * lockObject = (OsalMonitor_Object*) lockHandle;

    GT_2trace (curTrace, GT_ENTER, "OsalMonitor_leave", lockHandle, key);

    GT_assert (curTrace, (lockHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (lockHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalMonitor_enter",
                             OSALMONITOR_E_HANDLE,
                             "NULL spinlock handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        spin_unlock (&(lockObject->lock));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalMonitor_leave");
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
