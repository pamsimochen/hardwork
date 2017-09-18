/*
 *  @file   OsalSemaphore.c
 *
 *  @brief      Linux kernel Semaphore interface implementation.
 *
 *              This abstracts the Semaphore interface in Kernel code and
 *              is implemented using the wait queues. It has interfaces
 *              for creating, destroying, waiting and triggering the Semaphores.
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

/* OSAL and utils headers */
#include <ti/syslink/utils/OsalSemaphore.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* Linux specific header files */
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/sched.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief   Defines object to encapsulate the Semaphore.
 *           The definition is OS/platform specific.
 */
typedef struct OsalSemaphore_Object_tag {
    OsalSemaphore_Type  semType;
    /*!< Indicates the type of the semaphore (binary or counting). */
    wait_queue_head_t   list;
    /*!< List of waiting processes. */
    UInt32              value;
    /*!< Current status of semaphore (0,1) - binary and (0-n) counting. */
    struct mutex        lock;
    /*!< lock on which this Semaphore is based. */
} OsalSemaphore_Object;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/*!
 *  @brief      Creates an instance of Mutex object.
 *
 *  @param      semType Type of semaphore. This parameter is a mask of semaphore
 *                      type and interruptability type.
 *
 *  @sa         OsalSemaphore_delete
 */
OsalSemaphore_Handle
OsalSemaphore_create (UInt32 semType)
{
    OsalSemaphore_Object * semObj = NULL;

    GT_1trace (curTrace, GT_ENTER, "OsalSemaphore_create", semType);

    /* Check for semaphore type (binary/counting) */
    GT_assert (curTrace,
               (    (OSALSEMAPHORE_TYPE_VALUE(semType))
                <   OsalSemaphore_Type_EndValue));
    /* Check for semaphore interruptability */
    GT_assert (curTrace,
               (    (OSALSEMAPHORE_INTTYPE_VALUE(semType))
                <   OsalSemaphore_IntType_EndValue));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (OSALSEMAPHORE_TYPE_VALUE(semType) >= OsalSemaphore_Type_EndValue) {
        /*! @retval NULL Invalid semaphore type (OsalSemaphore_Type) provided */
        GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "OsalSemaphore_create",
                        OSALSEMAPHORE_E_INVALIDARG,
                        "Invalid semaphore type (OsalSemaphore_Type) provided");
    }
    else if (   OSALSEMAPHORE_INTTYPE_VALUE(semType)
             >= OsalSemaphore_IntType_EndValue) {
        /*! @retval NULL Invalid semaphore interruptability type
                         (OsalSemaphore_IntType) provided */
        GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "OsalSemaphore_create",
                        OSALSEMAPHORE_E_INVALIDARG,
                        "Invalid semaphore interruptability type "
                        "(OsalSemaphore_IntType) provided");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        semObj = Memory_calloc (NULL, sizeof (OsalSemaphore_Object), 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (semObj == NULL) {
            /*! @retval NULL Failed to allocate memory for semaphore object. */
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalSemaphore_create",
                             OSALSEMAPHORE_E_MEMORY,
                             "Failed to allocate memory for semaphore object.");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            semObj->semType = semType;
            semObj->value = 0u;
            mutex_init (&(semObj->lock));
            init_waitqueue_head (&semObj->list);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalSemaphore_create", semObj);

    /*! @retval Semaphore-handle Operation successfully completed. */
    return (OsalSemaphore_Handle) semObj;
}


/*!
 *  @brief      Deletes an instance of Semaphore object.
 *
 *  @param      mutexHandle   Semaphore object handle which needs to be deleted.
 *
 *  @sa         OsalSemaphore_create
 */
Int
OsalSemaphore_delete (OsalSemaphore_Handle * semHandle)
{
    Int status = OSALSEMAPHORE_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "OsalSemaphore_delete", semHandle);

    GT_assert (curTrace, (semHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (semHandle == NULL) {
        /*! @retval OSALSEMAPHORE_E_INVALIDARG NULL provided for argument
                                           semHandle.*/
        status = OSALSEMAPHORE_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalSemaphore_delete",
                             status,
                             "NULL provided for argument semHandle");
    }
    else if (*semHandle == NULL) {
        /*! @retval OSALSEMAPHORE_E_HANDLE NULL Semaphore handle provided. */
        status = OSALSEMAPHORE_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalSemaphore_delete",
                             status,
                             "NULL Semaphore handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_free (NULL, *semHandle, sizeof (OsalSemaphore_Object));
        *semHandle = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalSemaphore_delete", status);

    /*! @retval OSALSEMAPHORE_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief      Wait on the Semaphore in the kernel thread context
 *
 *  @param      semHandle   Semaphore object handle
 *  @param      timeout     Timeout (in msec). Special values are provided for
 *                          no-wait and infinite-wait.
 *
 *  @sa         OsalSemaphore_post
 */
Int
OsalSemaphore_pend (OsalSemaphore_Handle semHandle, UInt32 timeout)
{
    Int                     status      = OSALSEMAPHORE_SUCCESS;
    OsalSemaphore_Object *  semObj      = (OsalSemaphore_Object *) semHandle;
    int                     osStatus    = 0;
    UInt32                  timeoutVal  = 0u;

    GT_2trace (curTrace, GT_ENTER, "OsalSemaphore_pend", semHandle, timeout);

    GT_assert (curTrace, (semHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (semHandle == NULL) {
        /*! @retval OSALSEMAPHORE_E_HANDLE NULL Semaphore handle provided. */
        status = OSALSEMAPHORE_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalSemaphore_pend",
                             status,
                             "NULL Semaphore handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Different handling for no-timeout case. */
        if (timeout == OSALSEMAPHORE_WAIT_NONE) {
            osStatus = mutex_lock_interruptible(&(semObj->lock));
            GT_assert (curTrace, (osStatus == 0));
            /* TBD: Check if preempt_enable has to be used instead of mutex. */
            /* preempt_disable (); */
            if (semObj->value == 0u) {
                /*! @retval OSALSEMAPHORE_E_WAITNONE WAIT_NONE timeout value was
                            provided, but semaphore was not available. */
                status = OSALSEMAPHORE_E_WAITNONE ;
                GT_1trace (curTrace,
                           GT_4CLASS,
                           "OsalSemaphore_pend:    "
                           "WAIT_NONE timeout value was provided, but"
                           " semaphore was not available\n"
                           "    Handle [0x%x]\n",
                           semObj);
            }
            else {
                if (    OSALSEMAPHORE_TYPE_VALUE(semObj->semType)
                    ==  OsalSemaphore_Type_Binary) {
                    semObj->value = 0u;
                }
                else {
                    semObj->value--;
                }
            }
            mutex_unlock (&semObj->lock);
            /* TBD: Check if preempt_enable has to be used instead of mutex. */
            /* preempt_enable (); */
        }
        /* Finite and infinite timeout cases */
        else {
            DECLARE_WAITQUEUE (wait, current);
            /* Get timeout value in OS-recognizable format. */
            if (timeout == OSALSEMAPHORE_WAIT_FOREVER) {
                timeoutVal = MAX_SCHEDULE_TIMEOUT;
            }
            else {
                timeoutVal = msecs_to_jiffies (timeout);
            }
            /* Add the current process to wait queue */
            add_wait_queue_exclusive (&semObj->list, &wait);
            /* Set the current task status as interruptible */
            do {
                set_current_state (TASK_INTERRUPTIBLE);
                /* TBD: Check if preempt_enable has to be used instead of
                        mutex. */
                osStatus = mutex_lock_interruptible(&(semObj->lock));
                GT_assert (curTrace, (osStatus == 0));
                /* preempt_disable (); */
                if (semObj->value != 0) {
                    if (    OSALSEMAPHORE_TYPE_VALUE (semObj->semType)
                        ==  OsalSemaphore_Type_Binary) {
                        semObj->value = 0u;
                    }
                    else {
                        semObj->value--;
                    }

                    /* Release the lock */
                    mutex_unlock (&semObj->lock);
                    /* TBD: Check if preempt_enable has to be used instead of
                     * mutex.
                     */
                    /* preempt_enable (); */
                    break;
                }

                /* Release the lock */
                mutex_unlock (&semObj->lock);
                /* TBD: Check if preempt_enable has to be used instead of
                 * mutex.
                 */
                /* preempt_enable (); */
                osStatus = schedule_timeout (timeoutVal);
                /* Check for status. This run-time check must remain in code
                 * and must not be optimized out.
                 */
                if (osStatus == 0) { /* Timedout? */
                    /*! @retval OSALSEMAPHORE_E_TIMEOUT Timeout occurred on
                                                        semaphore pend */
                    status = OSALSEMAPHORE_E_TIMEOUT ;
                    GT_1trace (curTrace,
                               GT_4CLASS,
                               "OsalSemaphore_pend:    "
                               "Timeout occurred on semaphore pend\n"
                               "    Handle [0x%x]\n",
                               semObj);
                    break ;
                }

                if (signal_pending (current) && (osStatus != 0)) { /* Interrupted? */
                    /* Not a failure. Execution can be resumed. */
                    status = -ERESTARTSYS;
                    GT_1trace (curTrace,
                               GT_1CLASS,
                               "OsalSemaphore_pend:    "
                               "Semaphore pend interrupted\n"
                               "    Handle [0x%x]\n",
                               semObj);
                    break ;
                }
            } while (1);
            /* Remove from wait list */
            remove_wait_queue (&semObj->list, &wait);
            /* Set the current task status as running */
            set_current_state (TASK_RUNNING);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalSemaphore_pend", status);

    /*! @retval OSALSEMAPHORE_SUCCESS Operation successfully completed. */
    return status;
}

/*!
 *  @brief      Signals the semaphore and makes it available for other
 *              threads.
 *
 *  @param      semHandle Semaphore object handle
 *
 *  @sa         OsalSemaphore_pend
 */
Int
OsalSemaphore_post (OsalSemaphore_Handle semHandle)
{
    Int                     status      = OSALSEMAPHORE_SUCCESS;
    OsalSemaphore_Object *  semObj      = (OsalSemaphore_Object *) semHandle;
    int                     osStatus    = 0;

    GT_1trace (curTrace, GT_ENTER, "OsalSemaphore_post", semHandle);

    GT_assert (curTrace, (semHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (semHandle == NULL) {
        /*! @retval OSALSEMAPHORE_E_HANDLE NULL Semaphore handle provided. */
        status = OSALSEMAPHORE_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalSemaphore_post",
                             status,
                             "NULL Semaphore handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (    OSALSEMAPHORE_TYPE_VALUE (semObj->semType)
            ==  OsalSemaphore_Type_Binary) {
            osStatus = mutex_lock_interruptible (&(semObj->lock));
            GT_assert (curTrace, (osStatus == 0));
            /* TBD: Check if preempt_disable has to be used instead of mutex. */
            /* preempt_disable (); */
            semObj->value = 1u;
            wake_up_interruptible (&(semObj->list));
            mutex_unlock(&(semObj->lock));
            /* TBD: Check if preempt_enable has to be used instead of mutex. */
            /* preempt_enable (); */
        }
        else {
            osStatus = mutex_lock_interruptible (&(semObj->lock));
            GT_assert (curTrace, (osStatus == 0));
            /* TBD: Check if preempt_disable has to be used instead of mutex. */
            /* preempt_disable (); */
            semObj->value++;
            wake_up_interruptible (&semObj->list);
            mutex_unlock (&semObj->lock);
            /* TBD: Check if preempt_enable has to be used instead of mutex. */
            /* preempt_enable (); */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalSemaphore_post", status);

    /*! @retval OSALSEMAPHORE_SUCCESS Operation successfully completed. */
    return status;
}


#if defined (__cplusplus)
}
#endif /* defined (_cplusplus)*/
