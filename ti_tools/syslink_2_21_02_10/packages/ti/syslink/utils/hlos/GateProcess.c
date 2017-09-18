/*
 *  @file   GateProcess.c
 *
 *  @brief      Process level gate
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

/* Osal & Utility headers */
#include <ti/syslink/osal/OsalProcess.h>
#include <ti/syslink/inc/usr/OsalMutex.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>

/* Module level headers */
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateProcess.h>

/* TODO: wrap this in ti.syslink.osal.Mutex */
#include <errno.h>
#include <pthread.h>


/* -----------------------------------------------------------------------------
 *  Structs & Enums
 * -----------------------------------------------------------------------------
 */

#define GateProcess_Params_OPEN (0x01)

/* instance shared memory structure */
typedef struct {
    pthread_mutex_t     mutex;  /* _f1 */
} GateProcess_ShStruct;

/* instance object */
struct GateProcess_Object {
    IGateProvider_SuperObject;  /* inheritance from IGateProvider */
    UInt                    reserved;
    GateProcess_ShStruct *  shStruct;
};


/* -----------------------------------------------------------------------------
 *  Forward declarations
 * -----------------------------------------------------------------------------
 */

Int GateProcess_Instance_init(
        GateProcess_Object *        obj,
        Ptr                         shMemAddr,
        const GateProcess_Params *  params);

Void GateProcess_Instance_finalize(GateProcess_Object *obj, Int status);


/* -----------------------------------------------------------------------------
 *  Implementation
 * -----------------------------------------------------------------------------
 */

/*
 *  ======== GateProcess_Params_init ========
 */
Void GateProcess_Params_init(GateProcess_Params *params)
{
    params->reserved = 0;
}

/*
 *  ======== GateProcess_create ========
 */
GateProcess_Handle GateProcess_create(
        Ptr                         shMemAddr,
        const GateProcess_Params *  params)
{
    GateProcess_Params      localParams;
    GateProcess_Object *    obj;
    Int                     status;

    GT_1trace(curTrace, GT_ENTER, "GateProcess_create", shMemAddr);

    /* setup default params if none given */
    if (params == NULL) {
        GateProcess_Params_init(&localParams);
        params = &localParams;
    }

    /* allocate the object */
    obj = Memory_calloc(NULL, sizeof(GateProcess_Object), 0, NULL);

    /* initialize the object */
    if (obj != NULL) {
        status = GateProcess_Instance_init(obj, shMemAddr, params);

        /* if error, finalize and free object */
        if (status != 0) {
            GateProcess_Instance_finalize(obj, status);
            Memory_free(NULL, obj, sizeof(GateProcess_Object));
            obj = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "GateProcess_create", obj);

    return((GateProcess_Handle)obj);
}

/*
 *  ======== GateProcess_delete ========
 */
Int GateProcess_delete(GateProcess_Handle *pHandle)
{
    GateProcess_Object *    obj;
    Int                     status;

    GT_1trace(curTrace, GT_ENTER,
            "GateProcess_delete: *pHandle=0x%08x", *pHandle);

    status = GateProcess_S_SUCCESS;
    obj = (GateProcess_Object *)(*pHandle);

    /* ensure this object was not opened */
    if ((obj->reserved & GateProcess_Params_OPEN) > 0) {
        status = GateProcess_E_INVALIDSTATE;
    }

    /* finalize and free object */
    if (status == GateProcess_S_SUCCESS) {
        obj = (GateProcess_Object *)(*pHandle);
        GateProcess_Instance_finalize(obj, 0);
        Memory_free(NULL, obj, sizeof(GateProcess_Object));
        *pHandle = NULL;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateProcess_delete: [0x%08x]", status);

    return(status);
}

/*
 *  ======== GateProcess_open ========
 */
GateProcess_Handle GateProcess_open(Ptr shMemAddr)
{
    GateProcess_Params      localParams;
    GateProcess_Object *    obj;
    Int                     status;

    GT_1trace(curTrace, GT_ENTER, "GateProcess_open", shMemAddr);

    /* setup params with open flag */
    GateProcess_Params_init(&localParams);
    localParams.reserved |= GateProcess_Params_OPEN;

    /* allocate the object */
    obj = Memory_calloc(NULL, sizeof(GateProcess_Object), 0, NULL);

    /* initialize the object */
    if (obj != NULL) {
        status = GateProcess_Instance_init(obj, shMemAddr, &localParams);

        /* if error, finalize and free object */
        if (status != 0) {
            GateProcess_Instance_finalize(obj, status);
            Memory_free(NULL, obj, sizeof(GateProcess_Object));
            obj = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "GateProcess_create", obj);

    return((GateProcess_Handle)obj);
}

/*
 *  ======== GateProcess_close ========
 */
Int GateProcess_close(GateProcess_Handle *pHandle)
{
    GateProcess_Object *    obj;
    Int                     status;

    GT_1trace(curTrace, GT_ENTER,
            "GateProcess_close: *pHandle=0x%08x", *pHandle);

    status = GateProcess_S_SUCCESS;
    obj = (GateProcess_Object *)(*pHandle);

    /* ensure this object was opened */
    if ((obj->reserved & GateProcess_Params_OPEN) == 0) {
        status = GateProcess_E_INVALIDSTATE;
    }

    /* finalize and free object */
    if (status == GateProcess_S_SUCCESS) {
        obj = (GateProcess_Object *)(*pHandle);
        GateProcess_Instance_finalize(obj, 0);
        Memory_free(NULL, obj, sizeof(GateProcess_Object));
        *pHandle = NULL;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateProcess_close: [0x%08x]", status);

    return(status);
}

/*
 *  ======== GateProcess_Instance_init ========
 */
Int GateProcess_Instance_init(
        GateProcess_Object *        obj,
        Ptr                         shMemAddr,
        const GateProcess_Params *  params)
{
    pthread_mutex_t *   mutex;
    pthread_mutexattr_t attr;
    Int                 status;

    status = 0;

    /* assert the shared memory structure is large enough */
    GT_assert(curTrace,
        (sizeof(GateProcess_ShMem) >= sizeof(GateProcess_ShStruct)));

    /* initialize the inherited object */
    IGateProvider_ObjectInitializer(obj, GateProcess);

    /* store shMemAddr and reserved params in object */
    obj->reserved = params->reserved;
    obj->shStruct = shMemAddr;

    /* if opening the mutex, nothing else to be done */
    if (obj->reserved & GateProcess_Params_OPEN) {
        status = 1;
    }

    /* initialize the mutex create attrs */
    if (status == 0) {
        status = pthread_mutexattr_init(&attr);
        status = (status != EOK ? -1 : 0);
    }

    /* setup the mutex attributes for shared access */
    if (status == 0) {
        status = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        status = (status != EOK ? -1 : 0);
    }

    /* setup the mutex attributes for recursive access */
    if (status == 0) {
        status = pthread_mutexattr_setrecursive(&attr,
            PTHREAD_RECURSIVE_ENABLE);
        status = (status != EOK ? -1 : 0);
    }

    /* initialize the mutex object */
    if (status == 0) {
        mutex = &obj->shStruct->mutex;
        status = pthread_mutex_init(mutex, &attr);
        status = (status != EOK ? -1 : 0);
    }

    return(status >= 0 ? 0 : status);
}

/*
 *  ======== GateProcess_Instance_finalize ========
 */
Void GateProcess_Instance_finalize(GateProcess_Object *obj, Int status)
{
    /* nothing to be done */
}

/*
 *  ======== GateProcess_enter ========
 */
IArg GateProcess_enter(GateProcess_Handle handle)
{
    pthread_mutex_t *       mutex;
    GateProcess_Object *    obj;
    Int                     status;

#if defined (SYSLINK_TRACE_ENABLE)
    Osal_Pid    pid;
    Osal_Tid    tid;

    pid = OsalProcess_getPid();
    tid = OsalProcess_getTid();
#endif

    GT_1trace(curTrace, GT_ENTER, "GateProcess_enter:", handle);
    GT_2trace(curTrace, GT_1CLASS,
        "GateProcess_enter: pid=%d, tid=%d", pid, tid);

    status = GateProcess_S_SUCCESS;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = GateProcess_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateProcess_enter",
            status, "handle passed is null");
    }
#endif

    /* acquire the lock */
    if (status == GateProcess_S_SUCCESS) {
        obj = (GateProcess_Object *)handle;
        mutex = &obj->shStruct->mutex;

        status = pthread_mutex_lock(mutex);

        if (status != EOK) {
            GT_setFailureReason(curTrace, GT_4CLASS, "GateProcess_enter",
                GateProcess_E_FAIL, "mutex_lock failed");
        }
    }

    GT_2trace(curTrace, GT_1CLASS,
        "GateProcess_enter: pid=%d, tid=%d", pid, tid);
    GT_1trace(curTrace, GT_LEAVE, "GateProcess_enter:", handle);

    return((IArg)handle);
}

/*
 *  ======== GateProcess_leave ========
 */
Void GateProcess_leave(GateProcess_Handle handle, IArg key)
{
    pthread_mutex_t *       mutex;
    GateProcess_Object *    obj;
    Int                     status;

#if defined (SYSLINK_TRACE_ENABLE)
    Osal_Pid    pid;
    Osal_Tid    tid;

    pid = OsalProcess_getPid();
    tid = OsalProcess_getTid();
#endif

    GT_1trace(curTrace, GT_ENTER, "GateProcess_leave:", handle);
    GT_2trace(curTrace, GT_1CLASS,
        "GateProcess_leave: pid=%d, tid=%d", pid, tid);

    status = GateProcess_S_SUCCESS;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = GateProcess_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateProcess_leave",
            status, "handle passed is null");
    }
#endif

    /* release the lock */
    if (status == GateProcess_S_SUCCESS) {
        obj = (GateProcess_Object *)handle;
        mutex = &obj->shStruct->mutex;

        status = pthread_mutex_unlock(mutex);

        if (status != EOK) {
            GT_setFailureReason(curTrace, GT_4CLASS, "GateProcess_leave",
                GateProcess_E_FAIL, "mutex_unlock failed");
        }
    }

    GT_0trace(curTrace, GT_LEAVE, "GateProcess_leave");
}

/*
 *  ======== GateProcess_shMemSize ========
 */
SizeT GateProcess_shMemSize(Void)
{
    return(sizeof(pthread_mutex_t));
}
