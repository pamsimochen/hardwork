/*
 *  @file   OsalThread.c
 *
 *  @brief      Qnx thread interface definitions.
 *
 *              This interface abstracts the kernel side thread
 *              implementation.
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
#include <ti/syslink/inc/Bitops.h>

#include <errno.h>
#include <pthread.h>
#include <unistd.h>

/*!
 *  @brief  Structure for defining Thread objects.
 */
typedef struct OsalThread_Object_tag {
    List_Elem              elem;
    /* Used for creating a linkedlist */
		pthread_t task;
    /* Handle to the kernel task object */
    OsalThread_CallbackFxn cbckFxn;
    /* Callback function which will be called.*/
    Ptr                    cbckData;
    /* Data associated with the function */
    Atomic               count;
    /* Counter to count activation */
    Atomic               enableFlag;
    /* Flag to indicate enabled or disabled states */
    Atomic               exitFlag;
    /* Flag to kill the thread */
    pthread_mutex_t lock;
    pthread_cond_t cond;
    /* completion synch object*/
    Atomic               held;
    /* thread held state */
    OsalThread_Params      threadParams;
    /* Thread parameters for setting the priority*/
} OsalThread_Object;


/* Structure for defining Thread objects. */
typedef struct OsalThread_ModuleObject {
    List_Object  objList;
    /* List for maintaining internal objects */
    Atomic     count;
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
static void * Thread_callback(Ptr arg);


/*
 * ======== OsalThread_setup ========
 */
Int32 OsalThread_setup(Void)
{
    Int32 status = 0;
    List_Params listparams;

    GT_0trace(curTrace, GT_ENTER, "OsalThread_setup");

    /* Construct the list object */
    List_Params_init(&listparams);
    List_construct(&OsalThread_state.objList, &listparams);

    /* Initialize the count */
    Atomic_set(&OsalThread_state.count, 0);

    GT_0trace(curTrace, GT_LEAVE, "OsalThread_setup");

    return (status);
}


/*
 * ======== OsalThread_destroy ========
 */
Int32 OsalThread_destroy (Void)
{
    Int32 status = 0;

    GT_0trace(curTrace, GT_ENTER, "OsalThread_destroy");

    List_destruct(&OsalThread_state.objList);

    GT_0trace(curTrace, GT_LEAVE, "OsalThread_destroy");

    return (status);
}

/*
 *  ======== OsalThread_create ========
 */
OsalThread_Handle OsalThread_create(
    OsalThread_CallbackFxn  fxn,
    Ptr                     fxnArgs,
    OsalThread_Params *     params)
{
    Int                 status = OSALTHREAD_SUCCESS;
    OsalThread_Object * obj = NULL;
    UInt32              tid;
    pthread_attr_t      attr;
    struct sched_param  sched_param;
    int                 curprio;
    Int                 ret = 0;
    UInt                state = 0;

    GT_3trace(curTrace, GT_ENTER, "OsalThread_create", fxn, fxnArgs, params);
    GT_assert(curTrace, (NULL != fxn));

    /* allocate the object */
    if (status == OSALTHREAD_SUCCESS) {
        obj = (OsalThread_Object *)Memory_alloc(NULL,
            sizeof(OsalThread_Object), 0, NULL);

        if (obj == NULL) {
            status = OSALTHREAD_E_MEMORY;
            GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_create",
                status, "memory allocation failure");
        }
    }

    /* initialize the object */
    if (status == OSALTHREAD_SUCCESS) {
        if (params != NULL) {
            Memory_copy(&obj->threadParams, params, sizeof(OsalThread_Params));
        }
        else {
            obj->threadParams.priorityType = OsalThread_PriorityType_Generic;
            obj->threadParams.priority = OsalThread_Priority_Medium;
            obj->threadParams.once = FALSE;
        }
        obj->count = 0;
        obj->enableFlag = 1;
        obj->exitFlag = 0;
        obj->cbckFxn = fxn;
        obj->cbckData = fxnArgs;
        obj->held = 1;
    }

    /* initialize the mutext */
    if (status == OSALTHREAD_SUCCESS) {
        status = pthread_mutex_init(&obj->lock , NULL);

        if (status == EOK) {
            status = OSALTHREAD_SUCCESS;
            state |= 0x1;  /* mutex is initialized */
        }
        else {
            status = OSALTHREAD_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_create",
                status, "pthread_mutex_init failed");
        }
    }

    /* initialize the condition variable */
    if (status == OSALTHREAD_SUCCESS) {
        status = pthread_cond_init(&obj->cond , NULL);

        if (status == EOK) {
            status = OSALTHREAD_SUCCESS;
            state |= 0x2;  /* condition variable is initialized */
        }
        else {
            status = OSALTHREAD_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_create",
                status, "pthread_cond_init failed");
        }
    }

    /* create the thread */
    if (status == OSALTHREAD_SUCCESS) {
        tid = Atomic_inc_return(&OsalThread_state.count);
        pthread_attr_init(&attr);
        pthread_getschedparam(pthread_self(), NULL, &sched_param);
        curprio = sched_param.sched_curpriority;

        /* take the lock before creating the thread */
        ret = pthread_mutex_lock(&obj->lock);
        GT_assert(curTrace, (ret == 0));

        status = pthread_create(&obj->task, &attr, Thread_callback, (Ptr)obj);

        if (status == EOK) {
            status = OSALTHREAD_SUCCESS;
            state |= 0x4;  /* thread has been created */
        }
        else {
            status = OSALTHREAD_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_create",
                status, "pthread_create failed");
        }
    }

    /* set the thread's priority */
    if (status == OSALTHREAD_SUCCESS) {
        if (obj->threadParams.priorityType == OsalThread_PriorityType_Generic) {

            switch(obj->threadParams.priority) {

                case OsalThread_Priority_Low:
                    pthread_setschedprio(obj->task, curprio - 1);
                    break;

                case OsalThread_Priority_Medium:
                    // nothing here, run at current priority
                    break;

                case OsalThread_Priority_High:
                    pthread_setschedprio(obj->task, curprio + 1);
                    break;

                default:
                    status = OSALTHREAD_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                        "OsalThread_create", status, "invalid thread priority");
                    break;
            }
        }
        else if (obj->threadParams.priorityType ==
            OsalThread_PriorityType_OsSpecific) {
            /*TBD : set_user_nice ();*/
        }
        else {
            /* TBD: Error */
        }
    }

    /* start the thread */
    if (status == OSALTHREAD_SUCCESS) {
        Atomic_set(&obj->held, 0);
        pthread_cond_signal(&obj->cond);
        ret = pthread_mutex_unlock(&obj->lock);
        GT_assert(curTrace, (ret == 0));
    }

    /* handle error condition */
    if (status < 0) {
        if (state & 0x4) {
            Atomic_set(&obj->held, 0);
            Atomic_set(&obj->exitFlag, 1);
            pthread_cond_signal(&obj->cond);
            pthread_mutex_unlock(&obj->lock);
            pthread_join(obj->task, NULL);
            pthread_mutex_destroy(&obj->lock);
        }
        if (state & 0x2) {
            pthread_cond_destroy(&obj->cond);
        }
        if (state & 0x1) {
            pthread_mutex_destroy(&obj->lock);
        }
        if (obj != NULL) {
            Memory_free(NULL, obj, sizeof(OsalThread_Object));
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "OsalThread_create", obj);

    return((OsalThread_Handle)obj);
}

/*
 *  ======== OsalThread_delete ========
 */
Int32 OsalThread_delete(OsalThread_Handle *threadHandle)
{
    Int32               status = OSALTHREAD_SUCCESS;
    OsalThread_Object * obj = NULL;

    GT_1trace(curTrace, GT_ENTER, "OsalThread_delete", threadHandle);
    GT_assert(curTrace, (NULL != threadHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (threadHandle == NULL) {
        /*! @retval OSALTHREAD_E_INVALIDARG threadHandle passed is NULL */
        status = OSALTHREAD_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_delete",
            status, "threadHandle passed is NULL!");
    }
    else if (*threadHandle == NULL) {
        /*! @retval OSALTHREAD_E_INVALIDARG *threadHandle passed is NULL */
        status = OSALTHREAD_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_delete",
            status, "*threadHandle passed is NULL!");
    }
#endif

    /* tell thread to exit */
    if (status == OSALTHREAD_SUCCESS) {
        obj = (OsalThread_Object*)(*threadHandle);
        Atomic_set(&obj->count, 0);
        Atomic_set(&obj->enableFlag, 0);
        Atomic_set(&obj->exitFlag, 1); /* set exit flag */
        Atomic_set(&obj->held, 0);
        pthread_cond_signal(&obj->cond); /* wake up the thread */
        pthread_join(obj->task, NULL);
    }

    /* delete the condition variable */
    if (status == OSALTHREAD_SUCCESS) {
        status = pthread_cond_destroy(&obj->cond);

        if (status == EOK) {
            status = OSALTHREAD_SUCCESS;
        }
        else {
            status = OSALTHREAD_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_delete",
                status, "pthread_cond_destroy failed");
        }
    }

    /* delete the mutex */
    if (status == OSALTHREAD_SUCCESS) {
        status = pthread_mutex_destroy(&obj->lock);

        if (status == EOK) {
            status = OSALTHREAD_SUCCESS;
        }
        else {
            status = OSALTHREAD_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "OsalThread_delete",
                status, "pthread_mutex_destroy failed");
        }
    }

    /* free the object memory */
    if (status == OSALTHREAD_SUCCESS) {
        Memory_free(NULL, obj, sizeof(OsalThread_Object));
        *threadHandle = NULL;
    }

    GT_1trace(curTrace, GT_LEAVE, "OsalThread_delete", status);

    return(status);
}

/*
 * ======== OsalThread_disableThread ========
 */
Void OsalThread_disableThread(OsalThread_Handle threadHandle)
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
        Atomic_set (&obj->enableFlag, 0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_disableThread");
}

/*
 * ======== OsalThread_enableThread ========
 */
Void OsalThread_enableThread(OsalThread_Handle threadHandle)
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
        Atomic_set (&obj->enableFlag, 1);
        if (Atomic_read (&obj->count) > 0) {
        		Atomic_set (&obj->held, 0);
            pthread_cond_signal (&obj->cond);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_enableThread");
}

/*
 * ======== OsalThread_disable ========
 */
Void OsalThread_disable(Void)
{
    List_Elem * obj ;

    GT_0trace (curTrace, GT_ENTER, "OsalThread_disable");

    List_traverse (obj, (List_Handle) &OsalThread_state.objList) {
        Atomic_set (&((OsalThread_Object *)obj)->enableFlag, 0);
    }

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_disable");
}

/*
 * ======== OsalThread_enable ========
 */
Void OsalThread_enable(Void)
{
    List_Elem * obj ;

    GT_0trace (curTrace, GT_ENTER, "OsalThread_enable");

    List_traverse (obj, (List_Handle) &OsalThread_state.objList) {
        Atomic_set (&((OsalThread_Object *)obj)->enableFlag, 1);
        if (Atomic_read (&((OsalThread_Object *)obj)->count) > 0) {
            Atomic_set (&((OsalThread_Object *)obj)->held, 0);
            pthread_cond_signal (&((OsalThread_Object *)obj)->cond);
        }
    }

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_enable");
}

/*
 * ======== OsalThread_activate ========
 */
Void OsalThread_activate(OsalThread_Handle threadHandle)
{
    OsalThread_Object * obj = (OsalThread_Object*) threadHandle;
    Int                 ret = 0;

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
        if (Atomic_read (&obj->enableFlag) == 1){
            ret = pthread_mutex_lock(&obj->lock);
            GT_assert (curTrace, (ret == 0));
            Atomic_inc_return (&obj->count);
            Atomic_set (&((OsalThread_Object *)obj)->held, 0);
            pthread_cond_signal (&obj->cond);
            ret = pthread_mutex_unlock(&obj->lock);
            GT_assert (curTrace, (ret == 0));
        }
        else {
            Atomic_inc_return (&obj->count);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_activate");
}

/*
 * ======== OsalThread_yield ========
 *
  * This function may get invoked from ISR context
 */
Void OsalThread_yield(OsalThread_Handle threadHandle)
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
        sched_yield ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_yield");
}

/*
 * ======== OsalThread_sleep ========
 */
Void OsalThread_sleep(UInt32 time)
{
    GT_0trace (curTrace, GT_ENTER, "OsalThread_sleep");
    delay(time);
    GT_0trace (curTrace, GT_LEAVE, "OsalThread_sleep");
}


/*
 * ======== OsalThread_delay ========
 */
Void
OsalThread_delay (UInt32 time)
{
    GT_0trace(curTrace, GT_ENTER, "OsalThread_delay");
    delay(time);
    GT_0trace(curTrace, GT_LEAVE, "OsalThread_delay");
}


/*
 * ======== OsalThread_inThread ========
 */
Bool OsalThread_inThread(Void)
{
    Bool inThread = FALSE;

    GT_0trace(curTrace, GT_ENTER, "OsalThread_inThread");

    /* always false for QNX */

    GT_1trace (curTrace, GT_LEAVE, "OsalThread_inThread", inThread);

    return (inThread);
}


/*
 * ======== OsalThread_waitForThread ========
 */
Void OsalThread_waitForThread (OsalThread_Handle threadHandle)
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
        while (Atomic_read (&obj->exitFlag) == 0) {
            delay (10); /* Wait for 10msecs */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalThread_waitForThread");
}


/*
 * ======== Thread_callback ========
 */
void * Thread_callback(Void *arg)
{
    OsalThread_Object * obj = (OsalThread_Object *)arg;
    Int                 ret = 0;

    GT_assert(curTrace, (obj != NULL));

    if (obj != NULL) {
        while (1) {
          ret = pthread_mutex_lock(&obj->lock);
          GT_assert(curTrace, (ret == 0));

          /*
           * Check the count as well in case OsalThread_activate was called
           * between end of 3rd while loop and the set of obj->held to 1.
           */
          while ((Atomic_read(&obj->held)) && (Atomic_read(&obj->count) == 0)) {
              pthread_cond_wait(&obj->cond, &obj->lock);
          }

          ret = pthread_mutex_unlock(&obj->lock);
          GT_assert(curTrace, (ret == 0));

          if (Atomic_read(&obj->exitFlag) == 1) {
              break;
          }

          while (Atomic_read(&obj->count)) {
              if (Atomic_read(&obj->enableFlag) == 0) {
                  break;
              }
              /* call the attached function */
              obj->cbckFxn(obj->cbckData);
              Atomic_dec_return(&obj->count);
          }

          if (obj->threadParams.once == TRUE) {
              break;
          }
          Atomic_set(&obj->held, 1);
        }
    }

    Atomic_set(&obj->exitFlag, 1);

    return(NULL);
}
