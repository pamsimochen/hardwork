/** 
 *  @file   ResTrack.h
 *
 *  @brief      Resource Tracker utility module
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


#ifndef ti_syslink_utils_ResTrack__include
#define ti_syslink_utils_ResTrack__include

#include <ti/syslink/osal/OsalTypes.h>
#include <ti/syslink/utils/List.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

#define ResTrack_S_SUCCESS      0
#define ResTrack_E_FAIL         (-1) 
#define ResTrack_E_MEMORY       (-2) 
#define ResTrack_E_PID          (-3) 
#define ResTrack_E_NOTFOUND     (-4) 


/*!
 *  @brief Resource compare function type 
 */
typedef Bool (*ResTrack_Fxn)(Void *ptrA, Void *ptrB);

/*!
 *  @brief An instance handle type
 */
typedef struct ResTrack_Object *ResTrack_Handle;

/*!
 *  @brief  Structure defining params for creating an instance.
 */
typedef struct {
    Int temp;
} ResTrack_Params;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         ResTrack_create
 */
Void ResTrack_Params_init(ResTrack_Params *params);

/*!
 *  @brief      Create a resource tracker object.
 *
 *  @param      params  Pointer to creation parameters. If NULL is passed,
 *                      default parameters are used.
 *
 *  @retval     Handle to the object
 *
 *  @sa         ResTrack_delete
 */
ResTrack_Handle ResTrack_create(ResTrack_Params *params);

/*!
 *  @brief      Delete a resource tracker object.
 *
 *  @param      handlePtr   Pointer to resource tracker instance handle
 *
 *  @sa         ResTrack_create
 */
Void ResTrack_delete(ResTrack_Handle *handlePtr);

/*!
 *  @brief      Get first element from the resource list.
 *
 *  @param      handle  Resource tracker instance handle
 *  @param      pid     Process id
 *  @param      resPtr  Resource object return pointer, may be NULL
 *
 *  @retval     ResTrack_S_SUCCESS      Operation succeeded
 *  @retval     ResTrack_E_PID          No process object for given id
 *
 *  @sa         ResTrack_push, ResTrack_remove
 */
Int ResTrack_pop(ResTrack_Handle handle, Osal_Pid pid, List_Elem **resPtr);

/*!
 *  @brief      Add resouce element for given process id.
 *
 *  @param      handle  Resource tracker instance handle
 *  @param      pid     Process id
 *  @param      res     Resource object to be added
 *
 *  @retval     ResTrack_S_SUCCESS      Operation succeeded
 *  @retval     ResTrack_E_PID          No process object for given id
 *
 *  @sa         ResTrack_pop
 */
Int ResTrack_push(ResTrack_Handle handle, Osal_Pid pid, List_Elem *res);

/*!
 *  @brief      Register a new process with the resource tracker.
 *
 *  @param      handle  Resource tracker instance handle
 *  @param      pid     Process id to register
 *
 *  @retval     ResTrack_S_SUCCESS      Operation succeeded
 *  @retval     ResTrack_E_MEMORY       Memory unavailable
 *
 *  @sa         ResTrack_unregister
 */
Int ResTrack_register(ResTrack_Handle handle, Osal_Pid pid);

/*!
 *  @brief      Remove specific resource object from the resource list
 *
 *  @param      handle  Resource tracker instance handle
 *  @param      pid     Process id
 *  @param      ref     Reference resource object
 *  @param      cmpFxn  Compare function
 *  @param      resPtr  Pointer to return resource, may be NULL
 *
 *  @retval     ResTrack_S_SUCCESS      Operation succeeded
 *  @retval     ResTrack_E_PID          No process object for given id
 *  @retval     ResTrack_E_NOTFOUND     No resource found which matches
 *                                      the reference resource
 *
 *  @sa         ResTrack_push, ResTrack_pop
 */
Int ResTrack_remove(ResTrack_Handle handle, Osal_Pid pid, List_Elem *ref,
        ResTrack_Fxn cmpFxn, List_Elem **resPtr);

/*!
 *  @brief      Unregister the given process with the resource tracker.
 *
 *  @param      handle  Resource tracker instance handle
 *  @param      pid     Process id to unregister
 *
 *  @retval     ResTrack_S_SUCCESS      Operation succeeded
 *  @retval     ResTrack_E_PID          No process object for given id
 *
 *  @sa         ResTrack_register
 */
Int ResTrack_unregister(ResTrack_Handle handle, Osal_Pid pid);


#if defined (__cplusplus)
}
#endif

#endif /* ti_syslink_utils_ResTrack__include */
