/*
 *  @file   ResTrack.c
 *
 *  @brief      Resource Tracker module
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



/* package header files */
#include <ti/syslink/Std.h>
#include <ti/syslink/osal/OsalTypes.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Memory.h>

/* this module's header file */
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/ResTrack.h>

typedef struct {
    GateMutex_Handle    gate;
    List_Handle         procList;
} ResTrack_Object;

typedef struct {
    List_Elem           elem;
    List_Handle         resList;
    Osal_Pid            pid;
} ResTrack_Proc;

Int ResTrack_Instance_init(ResTrack_Object *, const ResTrack_Params *params);
Int ResTrack_Instance_finalize(ResTrack_Object *obj);


/*
 *  ======== ResTrack_Params_init ========
 */
Void ResTrack_Params_init(ResTrack_Params *params)
{
    params->temp = 0;
}

/*
 *  ======== ResTrack_create ========
 */
ResTrack_Handle ResTrack_create(ResTrack_Params *params)
{
    Int status;
    ResTrack_Object *obj;
    SizeT size;

    /* setup local context */
    status = ResTrack_S_SUCCESS;
    obj = NULL;
    size = sizeof(ResTrack_Object);

    /* allocate the instance object */
    obj = Memory_calloc(NULL, size, 0, NULL);

    if (obj == NULL) {
        goto leave;
    }

    /* object-specific initialization */
    status = ResTrack_Instance_init(obj, params);

    if (status < 0) {
        ResTrack_Instance_finalize(obj);
        Memory_free(NULL, obj, size);
        obj = NULL;
        goto leave;
    }

leave:
    return((ResTrack_Handle)obj);
}

/*
 *  ======== ResTrack_delete ========
 */
Void ResTrack_delete(ResTrack_Handle *handlePtr)
{
    Int status = ResTrack_S_SUCCESS;
    ResTrack_Object *obj;

    /* check for null pointer */
    if ((handlePtr == NULL) || (*handlePtr == NULL)) {
        /* log error */
        status = ResTrack_E_FAIL;
    }

    obj = (ResTrack_Object *)(*handlePtr);
    ResTrack_Instance_finalize(obj);

    /* delete the instance object */
    if (status == ResTrack_S_SUCCESS) {
        obj = (ResTrack_Object *)(*handlePtr);
        Memory_free(NULL, obj, sizeof(ResTrack_Object));
        *handlePtr = NULL;
    }
}

/*
 *  ======== ResTrack_pop ========
 */
Int ResTrack_pop(ResTrack_Handle handle, Osal_Pid pid, List_Elem **resPtr)
{
    Int status;
    ResTrack_Object *obj;
    List_Elem *elem;
    ResTrack_Proc *proc;
    IGateProvider_Handle gate;
    IArg key;

    /* setup local context */
    obj = (ResTrack_Object *)handle;
    gate = (IGateProvider_Handle)(obj->gate);

    /* enter gate */
    key = IGateProvider_enter(gate);

    /* search process list for the given pid */
    elem = NULL;
    while ((elem = List_next(obj->procList, elem)) != NULL) {
        proc = (ResTrack_Proc *)elem;
        if (proc->pid == pid) {
            break; /* found it */
        }
    }

    /* leave if process object was not found */
    if (elem == NULL) {
        status = ResTrack_E_PID;
        *resPtr = NULL;
        goto leave;
    }

    /* remove first element from the resource list */
    *resPtr = List_get(proc->resList);
    status = ResTrack_S_SUCCESS;

leave:
    /* leave gate */
    IGateProvider_leave(gate, key);

    return(status);
}

/*
 *  ======== ResTrack_push ========
 */
Int ResTrack_push(ResTrack_Handle handle, Osal_Pid pid, List_Elem *res)
{
    Int status;
    ResTrack_Object *obj;
    ResTrack_Proc *proc;
    List_Elem *elem;
    IGateProvider_Handle gate;
    IArg key;

    status = ResTrack_E_PID;
    obj = (ResTrack_Object *)handle;
    gate = (IGateProvider_Handle)(obj->gate);

    /* enter the gate */
    key = IGateProvider_enter(gate);

    /* search process list for the given pid */
    elem = NULL;
    while ((elem = List_next(obj->procList, elem)) != NULL) {
        proc = (ResTrack_Proc *)elem;

        if (proc->pid == pid) {
            List_putHead(proc->resList, res);
            status = ResTrack_S_SUCCESS;
            break;
        }
    }

    /* leave the gate */
    IGateProvider_leave(gate, key);

    return(status);
}

/*
 *  ======== ResTrack_register ========
 */
Int ResTrack_register(ResTrack_Handle handle, Osal_Pid pid)
{
    Int status;
    ResTrack_Object *obj;
    ResTrack_Proc *proc;

    /* set local context */
    status = ResTrack_S_SUCCESS;
    obj = (ResTrack_Object *)handle;
    proc = NULL;

    /* TODO: ensure pid is not already registered */

    /* allocate the process object */
    proc = Memory_alloc(NULL, sizeof(ResTrack_Proc), 0, NULL);

    if (proc == NULL) {
        status = ResTrack_E_MEMORY;
        goto leave;
    }

    /* set the pid */
    proc->pid = pid;

    /* create the resource list */
    proc->resList = List_create(NULL, NULL);

    if (proc->resList == NULL) {
        status = ResTrack_E_MEMORY;
        goto leave;
    }

    /* add proc object to processor list */
    List_putHead(obj->procList, (List_Elem *)(proc));

leave:
    if (status < 0) {
        if ((proc != NULL) && (proc->resList != NULL)) {
            List_delete(&proc->resList);
        }
        if (proc != NULL) {
            Memory_free(NULL, proc, sizeof(ResTrack_Proc));
        }
    }

    return(status);
}

/*
 *  ======== ResTrack_remove ========
 */
Int ResTrack_remove(ResTrack_Handle handle, Osal_Pid pid, List_Elem *ref,
        ResTrack_Fxn cmpFxn, List_Elem **resPtr)
{
    Int status;
    ResTrack_Object *obj;
    List_Elem *elem;
    ResTrack_Proc *proc;
    IGateProvider_Handle gate;
    IArg key;

    /* setup local context */
    obj = (ResTrack_Object *)handle;
    gate = (IGateProvider_Handle)(obj->gate);

    /* enter gate */
    key = IGateProvider_enter(gate);

    /* search process list for the given pid */
    elem = NULL;
    while ((elem = List_next(obj->procList, elem)) != NULL) {
        proc = (ResTrack_Proc *)elem;
        if (proc->pid == pid) {
            break; /* found it */
        }
    }

    /* leave if process object was not found */
    if (elem == NULL) {
        status = ResTrack_E_PID;
        *resPtr = NULL;
        goto leave;
    }

    /* search resource list for given resource */
    status = ResTrack_E_NOTFOUND;
    elem = NULL;
    while ((elem = List_next(proc->resList, elem)) != NULL) {

        /* invoke the supplied compare function */
        if ((*cmpFxn)((Void *)ref, (Void *)elem)) {
            List_remove(proc->resList, elem); /* found it, remove from list */
            break;
        }
    }

    /* if resource found, set return pointer */
    if (elem != NULL) {
        *resPtr = elem;
        status = ResTrack_S_SUCCESS;
    }

leave:
    /* leave gate */
    IGateProvider_leave(gate, key);

    return(status);
}

/*
 *  ======== ResTrack_unregister ========
 */
Int ResTrack_unregister(ResTrack_Handle handle, Osal_Pid pid)
{
    Int status;
    ResTrack_Object *obj;
    ResTrack_Proc *proc;
    List_Elem *elem;
    IGateProvider_Handle gate;
    IArg key;

    /* setup local context */
    status = ResTrack_S_SUCCESS;
    obj = (ResTrack_Object *)handle;
    gate = (IGateProvider_Handle)(obj->gate);

    /* enter the gate */
    key = IGateProvider_enter(gate);

    /* search process list for the given pid */
    elem = NULL;
    while ((elem = List_next(obj->procList, elem)) != NULL) {
        proc = (ResTrack_Proc *)elem;
        if (proc->pid == pid) {
            break; /* found it */
        }
    }

    /* leave if process object was not found */
    if (elem == NULL) {
        status = ResTrack_E_PID;
        goto leave;
    }

    /* remove proc object from process list */
    List_remove(obj->procList, elem);

    /* make sure resource list is empty */
    do {
        elem = List_dequeue(proc->resList);
        GT_assert(curTrace, (elem == NULL));
    } while (elem != NULL);

    /* destroy the list object */
    List_delete(&(proc->resList));

    /* free the resource process object */
    Memory_free(NULL, proc, sizeof(ResTrack_Proc));

leave:
    /* leave the gate */
    IGateProvider_leave(gate, key);

    return(status);
}

/*
 *  ======== ResTrack_Instance_finalize ========
 */
Int ResTrack_Instance_finalize(ResTrack_Object *obj)
{
    Int status = ResTrack_S_SUCCESS;

    /* TODO: process list should be empty */

    if (obj->procList != NULL) {
        List_delete(&obj->procList);
    }

    if (obj->gate != NULL) {
        GateMutex_delete(&obj->gate);
    }

    return(status);
}


/*
 *  ======== ResTrack_Instance_init ========
 */
Int ResTrack_Instance_init(ResTrack_Object *obj, const ResTrack_Params *params)
{
    Int status = ResTrack_S_SUCCESS;

    /* create the gate */
    obj->gate = GateMutex_create(NULL, NULL);

    if (obj->gate == NULL) {
        status = ResTrack_E_FAIL;
        goto leave;
    }

    /* create the process list */
    obj->procList = List_create(NULL, NULL);

    if (obj->procList == NULL) {
        status = ResTrack_E_FAIL;
        goto leave;
    }

leave:
    return(status);
}
