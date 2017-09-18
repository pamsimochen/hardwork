/*
 *  @file   OsalEvent.c
 *
 *  @brief      Linux kernel Events interface definitions.
 *
 *              This is used inside kernel module to provide event based
 *              synchronisation. the implementation is done using the wait
 *              queues.
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



/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL and kernel utils */
#include <ti/syslink/inc/knl/OsalEvent.h>
#include <ti/syslink/inc/knl/OsalSpinlock.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* Wait header */
#include <linux/wait.h>
#include <linux/sched.h>


/*!
 *  @brief  This object is used for various event related API.
 */
typedef struct OsalEvent_Object_tag {
    UInt32                   signature;/*!<For identification of this object.*/
    wait_queue_head_t        list;      /*!< List for waiting processes.*/
    UInt32                   value;      /*!< Current value of the event.*/
    OsalSpinlock_Handle      lock;      /*!< Protect read/write event access */
} OsalEvent_Object;

/*!
 *  @brief      Creates and initializes an event object for thread
 *              synchronization. The event is initialized to a
 *              non-signaled state..
 *
 *  @param      Un-allocated event object.
 *  @sa         EventClose
 */
OsalEvent_Handle
OsalEvent_create (Void)
{
#if (!defined(SYSLINK_BUILD_OPTIMIZE)|| defined(SYSLINK_TRACE_ENABLE))
Int status = OSALEVENT_SUCCESS ;
#endif /*(!defined(SYSLINK_BUILD_OPTIMIZE)|| defined(SYSLINK_TRACE_ENABLE)) */

    OsalEvent_Object* event = NULL;

    GT_0trace (curTrace, GT_ENTER, "OsalEvent_create");

    event = Memory_alloc (NULL, sizeof (OsalEvent_Object), 0, NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (event == NULL) {
        status = OSALEVENT_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalEvent_create",
                             OSALEVENT_E_MEMORY,
                             "Memory allocation failed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        event->value = 0;
        event->lock = OsalSpinlock_create (OsalSpinlock_Type_Normal);
        init_waitqueue_head (&event->list) ;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GT_1trace (curTrace, GT_LEAVE, "OsalEvent_create", status);

    /*!@status The object created */
    return (OsalEvent_Handle) event;
}

/*!
 *  @brief      Closes the handle corresponding to an event. It also frees the
 *              resources allocated, if any, during call to OpenEvent ()
 *
 *  @param      allocated event object handle.
 *  @sa         OsalEvent_delete
 */
Int OsalEvent_delete (OsalEvent_Handle handle)
{
    Int status = OSALEVENT_SUCCESS ;
    OsalEvent_Object* event = (OsalEvent_Object*) handle;

    GT_1trace (curTrace, GT_ENTER,"OsalEvent_close",event );
    GT_assert (curTrace, (NULL != event) );

    status = OsalSpinlock_delete (&(event->lock));
    if (status >= 0)
    {
        event->signature = 0;
    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    else {
        status = OSALEVENT_E_SPINLOCK;
        GT_setFailureReason(curTrace, GT_4CLASS,
                            "OsalEvent_close",OSALEVENT_E_SPINLOCK,
                            "SpinLock Delete failed");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE,"Event_Close",status );

    /*!@status OSALEVENT_SUCCESS if call succeeded.*/
    /*!@status OSALEVENT_E_SPINLOCK when SpinLock operation fails.*/
    return status;
}

/*!
 *  @brief      Reset the synchronization object to non-signaled state.
 *
 *  @param      allocated event object handle.
 *  @sa         OsalEvent_set
 */
Int OsalEvent_reset (OsalEvent_Handle handle)
{
    Int status = OSALEVENT_SUCCESS ;
    OsalEvent_Object* event = (OsalEvent_Object*) handle;
    UInt32 lockKey = 0u;

    GT_1trace (curTrace, GT_ENTER,"OsalEvent_reset", event);

    GT_assert (curTrace, (NULL != event) );

    lockKey = OsalSpinlock_enter(event->lock);
    event->value = 0u;
    OsalSpinlock_leave(event->lock, lockKey);

    GT_1trace (curTrace, GT_LEAVE,"Event_Reset", status );

    /*! @retval OSALEVENT_SUCCESS as value get resetted.*/
    return status;
}

/*!
 *  @brief      Set the state of synchronization object to signaled and
 *              unblock all threads waiting for it.
 *
 *  @param      allocated event object handle.
 *  @sa         OsalEvent_reset
 */
Int OsalEvent_set (OsalEvent_Handle handle)
{
    Int status = OSALEVENT_SUCCESS ;
    OsalEvent_Object* event = (OsalEvent_Object*) handle;
    UInt32 lockKey = 0u;

    GT_1trace (curTrace, GT_ENTER,"OsalEvent_set", event );

    GT_assert (curTrace, (NULL != event) );

    lockKey = OsalSpinlock_enter(event->lock);
    event->value = 1u;
    wake_up_all (&event->list) ;
    OsalSpinlock_leave(event->lock, lockKey);

    GT_1trace (curTrace, GT_LEAVE,"OsalEvent_set", status );
    return status;
}

/*!
 *  @brief      Wait for an event to be signaled for some specified time.
 *              It is possible to wait infinitely, this calls Blocks and does
 *              not Spin.
 *
 *  @param      allocated event object handle.
 *
 *  @retval     Int as success if succedded otherwise failure
 */
Int OsalEvent_wait (OsalEvent_Handle handle, UInt32 timeout)
{
    Int status = OSALEVENT_SUCCESS ;
    Int32 osStatus = 0u;
    UInt32 lockKey = 0u;

    OsalEvent_Object* event = (OsalEvent_Object*) handle;

    GT_2trace (curTrace, GT_ENTER,"OsalEvent_wait", event, timeout );

    GT_assert (curTrace, (NULL != event) );

    if (OSAL_WAIT_NONE == timeout)
    {
        lockKey = OsalSpinlock_enter (event->lock) ;
        if (event->value == 0u) {
            /*! @retval OSALEVENT_SYNC_FAIL when value is found zero. */
            status = OSALEVENT_E_SYNCFAIL ;
            GT_setFailureReason(curTrace, GT_4CLASS,
                "OsalEvent_wait",OSALEVENT_E_SYNCFAIL,"Wrong combination");
        }
        OsalSpinlock_leave (event->lock, lockKey) ;
    }
    else {
        lockKey = OsalSpinlock_enter (event->lock) ;
        if (!event->value) {
            DECLARE_WAITQUEUE (wait, current) ;
            /* Add the current process to wait queue */
            add_wait_queue_exclusive (&event->list, &wait) ;
            /* Set the current task status as interrutible */
            do {
                set_current_state (TASK_INTERRUPTIBLE) ;
                /* Release the lock */
                OsalSpinlock_leave (event->lock, lockKey) ;
                /* Schedule out for given timeout */
                osStatus = schedule_timeout( \
                    (OSAL_WAIT_FOREVER == timeout)?MAX_SCHEDULE_TIMEOUT:\
                     msecs_to_jiffies (timeout) ) ;
                /* Take the lock again */
                lockKey = OsalSpinlock_enter (event->lock) ;
                /* Check for status */
                if (osStatus == 0) {
                    /*! @retval OSALEVENT_E_TIMEOUT when timeout occurs on
                        wait state */
                    status = OSALEVENT_E_TIMEOUT ;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "OsalEvent_wait",OSALEVENT_E_TIMEOUT,
                        "Timeout occured");
                    break ;
                }
                if (osStatus == -ERESTARTSYS) {
                    /*! @retval When we are inerrupted because of the this
                        system call*/
                    status = OSALEVENT_E_RESTARTSYS ;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "OsalEvent_wait",OSALEVENT_E_RESTARTSYS,
                        "Interrupted ");
                    break ;
                }
            } while (0) ;
            /* Remove from wait list */
            remove_wait_queue (&event->list, &wait) ;
            /* Set the current task status as running */
            set_current_state (TASK_RUNNING) ;
        }
        /* End the lock */
        OsalSpinlock_leave (event->lock, lockKey) ;
    }

    GT_1trace (curTrace, GT_LEAVE,"OsalEvent_wait",status);

    return status;
}
