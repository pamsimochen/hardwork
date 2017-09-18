/*
 *  @file   OsalThread.c
 *
 *  @brief      Linux kernel thread interface definitions.
 *
 *              This interface abstracts the kernel side thread
 *              implementation. It will be realised using the kthread
 *              constructs present in Linux.
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


/* Standard Headers */
#include <ti/syslink/Std.h>

/* OSAL and kernel utils */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/knl/OsalThread.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* Linux specific header files */
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/hardirq.h>


/*!
 *  @brief  Structure for defining Thread objects.
 */
typedef struct OsalThread_Object_tag {
    List_Elem              elem;
    /* Used for creating a linkedlist */
    struct task_struct *   task;
    /* Handle to the kernel task object */
    OsalThread_CallbackFxn cbckFxn;
    /* Callback function which will be called.*/
    Ptr                    cbckData;
    /* Data associated with the function */
    atomic_t               count;
    /* Counter to count activation */
    atomic_t               enableFlag;
    /* Flag to indicate enabled or disabled states */
    atomic_t               exitFlag;
    /* Flag to kill the thread */
    struct completion      sem;
    /* Semaphore for waiting */
    OsalThread_Params      threadParams;
    /* Thread parameters for setting the priority*/
} OsalThread_Object;


/* Structure for defining Thread objects. */
typedef struct OsalThread_ModuleObject {
    List_Object  objList;
    /* List for maintaining internal objects */
    atomic_t     count;
    /* Counter for objects */
} OsalThread_ModuleObject;


/*!
 *  @var    OsalThread_state
 *
 *  @brief  State object for thread module.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
OsalThread_ModuleObject OsalThread_state;


/*!
 *  @brief   This function gets invoked by the OS when ISR triggers this
 *
 *  @param   arg ThreadHandle
 */
static
int
Thread_callback (Ptr arg);


/* =============================================================================
 *  Globals
 * =============================================================================
 */
#if defined  (SYSLINK_INT_LOGGING)
extern UInt32 SysLogging_intCount;
extern UInt32 SysLogging_checkAndClearCount;
extern UInt32 SysLogging_isrCount;
extern UInt32 SysLogging_threadCount;
extern UInt32 SysLogging_NotifyCallbackCount;
#endif /* if defined  (SYSLINK_INT_LOGGING) */


/*!
 *  @brief      Initialize the thread module.
 *
 *  @sa         OsalThread_destroy
 */
Int32
OsalThread_setup (Void)
{
    Int32 status = 0;
    List_Params  listparams;

    GT_0trace (curTrace, GT_ENTER, "OsalThread_setup");

    /* Construct the list object */
    List_Params_init (&listparams);
    List_construct (&OsalThread_state.objList, &listparams);

    /* Initialize the count */
    atomic_set (&OsalThread_state.count, 0);

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_setup");

    /*! @retval 0 operation successful */
    return status;
}


/*!
 *  @brief      Finalize the thread module.
 *
 *  @sa         OsalThread_setup
 */
Int32
OsalThread_destroy (Void)
{
    Int32 status = 0;

    GT_0trace (curTrace, GT_ENTER, "OsalThread_destroy");

    List_destruct (&OsalThread_state.objList);

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_destroy");

    /*! @retval 0 operation successful */
    return status;
}


/*!
 *  @brief      create the thread.
 *
 *  @param      fxn Function which will be executed after the thread creation.
 *  @param      fxnArgs data which the function requires.
 *  @sa         OsalThread_delete, Thread_callback
 */
OsalThread_Handle
OsalThread_create (OsalThread_CallbackFxn fxn,
                   Ptr                    fxnArgs,
                   OsalThread_Params *    params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                 status = OSALTHREAD_SUCCESS;
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/
    OsalThread_Object * obj    = NULL;
    UInt32              tid;

    GT_3trace (curTrace, GT_ENTER, "OsalThread_create", fxn, fxnArgs, params);

    GT_assert (curTrace, (NULL != fxn));

    obj = (OsalThread_Object*) Memory_alloc (NULL,
                                             sizeof (OsalThread_Object),
                                             0,
                                             NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (obj == NULL) {
        status = OSALTHREAD_E_MEMORY;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "OsalThread_create",
                            OSALTHREAD_E_MEMORY,
                            "Memory Allocation failure ");
    }
    else{
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/
        /*Do not put under SYSLINK_BUILD_OPTIMIZE */
        if (params != NULL){
            Memory_copy(&obj->threadParams,
                        params,
                        sizeof(OsalThread_Params));
        }
        /* Populate the state */
        atomic_set (&obj->count, 0);
        atomic_set (&obj->enableFlag, 1);
        atomic_set (&obj->exitFlag, 0);
        obj->cbckFxn  = fxn;
        obj->cbckData = fxnArgs;

        /* Initialize the semaphore */
        init_completion (&obj->sem);

        /* Get the thread id */
        tid = atomic_inc_return (&OsalThread_state.count);
        /* Create the thread */
        obj->task = kthread_create (Thread_callback,
                                    (Ptr) obj,
                                    "kernelThread_%d",
                                    (tid - 1));

        if (obj->threadParams.priorityType == OsalThread_PriorityType_Generic) {
            switch(obj->threadParams.priority) {
                case OsalThread_Priority_Low :
                {
                    set_user_nice (obj->task,-5);
                }
                break;

                case OsalThread_Priority_Medium :
                {
                    set_user_nice (obj->task,-10);
                }
                break;

                case OsalThread_Priority_High :
                {
                    set_user_nice (obj->task,-15);
                }
                break;

                case OsalThread_Priority_EndValue :
                {
                }
                break;
            }
        }
        else if (   obj->threadParams.priorityType
                 == OsalThread_PriorityType_OsSpecific) {
            /*TBD : set_user_nice ();*/
        }
        else {
            /* TBD: Error */
        }

        /* Start the thread */
        wake_up_process (obj->task);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "OsalThread_create", obj);

    return (OsalThread_Handle) obj;
}

/*!
 *  @brief      Destroys the thread.
 *
 *  @param      threadHandle which needs to be destroyed.
 *  @sa            OsalThread_create
 */
Int32
OsalThread_delete (OsalThread_Handle * threadHandle)
{
    Int32               status = OSALTHREAD_SUCCESS;
    OsalThread_Object * obj    = NULL;

    GT_1trace (curTrace, GT_ENTER, "OsalThread_delete", threadHandle);

    GT_assert (curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        /*! @retval OSALTHREAD_E_INVALIDARG threadHandle passed is NULL */
        status = OSALTHREAD_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_delete",
                             status,
                             "threadHandle passed is NULL!");
    }
    else if (*threadHandle == NULL) {
        /*! @retval OSALTHREAD_E_INVALIDARG *threadHandle passed is NULL */
        status = OSALTHREAD_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_delete",
                             status,
                             "*threadHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = (OsalThread_Object*) (*threadHandle);
        atomic_set (&obj->count, 0);
        atomic_set (&obj->enableFlag, 0);
        /* Indicate to kill */
        atomic_set (&obj->exitFlag, 1);
        /* Wake up the thread */
        complete (&obj->sem);
        kthread_stop (obj->task);

        /* Free the memory */
        Memory_free (NULL, obj, sizeof(OsalThread_Object));
        *threadHandle = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalThread_delete", status);

    /*! @retval OSALTHREAD_SUCCESS operation successful */
    return status;
}

/*!
 *  @brief      Disables the thread.
 *
 *  @param      threadHandle which needs to be disabled.
 *  @sa            OsalThread_enableThread
 */
Void
OsalThread_disableThread (OsalThread_Handle threadHandle)
{
    OsalThread_Object * obj = (OsalThread_Object*) threadHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalThread_disableThread", threadHandle);

    GT_assert (curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_disableThread",
                             OSALTHREAD_E_INVALIDARG,
                             "threadHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        atomic_set (&obj->enableFlag, 0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_disableThread");
}

/*!
 *  @brief      Enables the thread.
 *
 *  @param      threadHandle which needs to be disabled.
 *  @sa            OsalThread_disableThread
 */
Void
OsalThread_enableThread (OsalThread_Handle threadHandle)
{
    OsalThread_Object * obj = (OsalThread_Object*) threadHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalThread_enableThread", threadHandle);

    GT_assert (curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_enableThread",
                             OSALTHREAD_E_INVALIDARG,
                             "threadHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        atomic_set (&obj->enableFlag, 1);
        if (atomic_read (&obj->count) > 0) {
            complete (&obj->sem);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_enableThread");
}

/*!
 *  @brief      Disables all threads created using this module.
 *
 *  @sa            OsalThread_enable
 */
Void
OsalThread_disable (Void)
{
    List_Elem * obj ;

    GT_0trace (curTrace, GT_ENTER, "OsalThread_disable");

    List_traverse (obj, (List_Handle) &OsalThread_state.objList) {
        atomic_set (&((OsalThread_Object *)obj)->enableFlag, 0);
    }

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_disable");
}

/*!
 *  @brief      Enables all threads created using this module.
 *
 *  @sa           OsalThread_disable
 */
Void
OsalThread_enable (Void)
{
    List_Elem * obj ;

    GT_0trace (curTrace, GT_ENTER, "OsalThread_enable");

    List_traverse (obj, (List_Handle) &OsalThread_state.objList) {
        atomic_set (&((OsalThread_Object *)obj)->enableFlag, 1);
        if (atomic_read (&((OsalThread_Object *)obj)->count) > 0) {
            complete (&((OsalThread_Object *)obj)->sem);
        }
    }

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_enable");
}

/*!
 *  @brief      Activate this thread. This function may gets invoked from ISR context.
 *
 * @param     threadHandle the thread which needs to be activated
 *  @sa          OsalThread_yield
 */
Void
OsalThread_activate (OsalThread_Handle threadHandle)
{
    OsalThread_Object * obj = (OsalThread_Object*) threadHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalThread_activate", threadHandle);

    GT_assert (curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_activate",
                             OSALTHREAD_E_INVALIDARG,
                             "threadHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (atomic_read (&obj->enableFlag) == 1){
            atomic_inc (&obj->count);
            complete (&obj->sem);
        }
        else {
            atomic_inc (&obj->count);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_activate");
}

/*!
 *  @brief   This function may get invoked from ISR context
 *
 *  @param     arg ThreadHandle
 *  @sa      OsalThread_activate
 */
Void
OsalThread_yield (OsalThread_Handle threadHandle)
{
    OsalThread_Object * obj = (OsalThread_Object*) threadHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalThread_yield", threadHandle);

    GT_assert (curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_yield",
                             OSALTHREAD_E_INVALIDARG,
                             "threadHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        (Void) obj;
        schedule ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_yield");
}

/*!
 *  @brief   Sleep this thread for specific time in milli-seconds.
 *
 *  @param   arg ThreadHandle
 *  @sa      OsalThread_delay
 */
Void
OsalThread_sleep (UInt32 time)
{

    GT_0trace (curTrace, GT_ENTER, "OsalThread_sleep");

    msleep (time);

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_sleep");
}


/*!
 *  @brief   Delay this thread for specific time in milli-seconds. Does not schedule out
 *               this thread.
 *
 *  @param     arg ThreadHandle
 *  @sa      OsalThread_delay
 */
Void
OsalThread_delay (UInt32 time)
{
    GT_0trace (curTrace, GT_ENTER, "OsalThread_delay");

    udelay (time * 1000);

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_delay");
}


/*!
 *  @brief   Checks if current context is a thread context or ISR context.
 */
Bool
OsalThread_inThread (Void)
{
    Bool inThread = FALSE;

    GT_0trace(curTrace, GT_ENTER, "OsalThread_inThread");

    inThread = (in_interrupt()) ? FALSE : TRUE;

    GT_1trace (curTrace, GT_LEAVE, "OsalThread_inThread", inThread);

    return inThread;
}


/*!
 *  @brief   Wait for thread completion.
 *
 *  @param   arg ThreadHandle
 */
Void
OsalThread_waitForThread (OsalThread_Handle threadHandle)
{
    OsalThread_Object * obj = (OsalThread_Object*) threadHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalThread_waitForThread", threadHandle);

    GT_assert (curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalThread_waitForThread",
                             OSALTHREAD_E_INVALIDARG,
                             "threadHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        while (atomic_read (&obj->exitFlag) == 0) {
            schedule_timeout (msecs_to_jiffies (10)); /* Wait for 10msecs */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_waitForThread");
}


/*!
 *  @brief   This function gets invoked by the OS when ISR triggers this
 *
 *  @param   arg ThreadHandle
 *  @sa      OsalThread_create
 */
int
Thread_callback (Void * arg)
{
    OsalThread_Object * obj = (OsalThread_Object *) arg;
#if 0
    long status;
#endif

    GT_assert (curTrace, (obj != NULL));

    if (obj != NULL) {
        for (;!kthread_should_stop ();) {
            wait_for_completion (&obj->sem);
#if 0
            /*
             * Need to see if this block of code has to be enabled insted of
             * wait_for_completion(&obj->sem)
             */
            do
            {
                status = wait_for_completion_interruptible(&obj->sem);
            } while (status == -ERESTARTSYS);
#endif
            if (atomic_read (&obj->exitFlag) == 1) {
                while (!kthread_should_stop()) {
                    set_current_state(TASK_INTERRUPTIBLE);
                    schedule();
                }
                break;
            }

            while (atomic_read (&obj->count)) {
#if defined  (SYSLINK_INT_LOGGING)
                SysLogging_threadCount++;
#endif /* if defined  (SYSLINK_INT_LOGGING) */
                if (atomic_read (&obj->enableFlag) == 0) {
                    break;
                }
                /* Call the attached function */
                obj->cbckFxn (obj->cbckData) ;
                atomic_dec (&obj->count) ;
            }
            if (obj->threadParams.once == TRUE) {
                atomic_set (&obj->exitFlag, 1);
                break;
            }
        }
    }
    return 0;
}
