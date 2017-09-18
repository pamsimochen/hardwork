/*
 *  @file   SyslinkMemMgr.c
 *
 *  @brief   Implements front end for the SyslinkMemMgr implementations.
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
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>

#include <SyslinkMemMgr_errBase.h>
#include <_SyslinkMemMgr.h>
#include <SyslinkMemMgr.h>
#include <SharedMemoryMgr.h>
#include <SyslinkMemMgrDrv.h>
#include <SyslinkMemMgrDrvDefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


struct SyslinkMemMgr_Object {
   Ptr         knlObject;
   UInt8       name[MEMMGR_MAX_NAME_LENGTH];
   SyslinkMemMgr_Type interfaceType;
};
/*!
 *  @brief  ProcMgr Module state object
 */
typedef struct SyslinkMemMgr_ModuleObject_tag {
    UInt32              setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
} SyslinkMemMgr_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    SyslinkMemMgr_state
 *
 *  @brief  ProcMgr state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
SyslinkMemMgr_ModuleObject SyslinkMemMgr_state =
{
    .setupRefCount = 0
};

extern SyslinkMemMgr_Fxns SyslinkMemMgr_SharedMemory_fxns;
/* =============================================================================
 * APIs
 * =============================================================================
 */

/*!
 *  @brief      Function to setup the SyslinkMemMgr module.
 *
 *  @sa         SyslinkMemMgr_destroy
 */
Int32
SyslinkMemMgr_setup (Void)
{
    Int                  status = MEMMGR_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "SyslinkMemMgr_setup");

    /* TBD: Protect from multiple threads. */
    SyslinkMemMgr_state.setupRefCount++;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (SyslinkMemMgr_state.setupRefCount > 1) {
        /*! @retval MEMMGR_S_ALREADYSETUP Success: SyslinkMemMgr module has
                                          been already setup in this process */
        status = MEMMGR_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "SyslinkMemMgr module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   SyslinkMemMgr_state.setupRefCount);
    }
    else {
        /* Open the driver handle. */
        status = SyslinkMemMgrDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SyslinkMemMgr_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* setup in kernel space is not required */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "SyslinkMemMgr_setup", status);

    /*! @retval MEMMGR_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to setup the SyslinkMemMgr module.
 *
 *  @sa         SyslinkMemMgr_setup
 */
Int32
SyslinkMemMgr_destroy (void)
{
    Int                     status = MEMMGR_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "SyslinkMemMgr_destroy");

    /* TBD: Protect from multiple threads. */
    SyslinkMemMgr_state.setupRefCount--;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (SyslinkMemMgr_state.setupRefCount >= 1) {
        /*! @retval MEMMGR_S_ALREADYSETUP Success: ProcMgr module has been
                                           already setup in this process */
        status = MEMMGR_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "ProcMgr module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   SyslinkMemMgr_state.setupRefCount);
    }
    else {
        /* Close the driver handle. */
        SyslinkMemMgrDrv_close ();
    }

    GT_1trace (curTrace, GT_LEAVE, "SyslinkMemMgr_destroy", status);

    return (status);
}

/* Function to create a SyslinkMemMgr instance */
SyslinkMemMgr_Handle
SyslinkMemMgr_create (Ptr params)
{
    Int32               status = MEMMGR_SUCCESS;
    SyslinkMemMgr_Object       *obj = NULL;
    SyslinkMemMgr_Handle       handle = NULL;
    SyslinkMemMgr_CreateParams *commonCreParams = (SyslinkMemMgr_CreateParams *)params;
    SyslinkMemMgr_SharedMemory_Params  *shSyslinkMemMgrParams;
    SyslinkMemMgrDrv_CmdArgs      cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "SyslinkMemMgr_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (SyslinkMemMgr_state.setupRefCount == 0) {
        /*! @retval NULL Modules is invalidstate. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_create",
                             MEMMGR_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /*! @retval NULL params passed is null. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_create",
                             MEMMGR_E_INVALIDARG,
                             "params passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        switch (commonCreParams->type)
        {
            case SyslinkMemMgr_TYPE_SHAREDMEM:
            {
                shSyslinkMemMgrParams = (SyslinkMemMgr_SharedMemory_Params *)params;
                cmdArgs.args.create.type = SyslinkMemMgr_TYPE_SHAREDMEM;
                cmdArgs.args.create.params = Memory_alloc (NULL,
                                             sizeof(SyslinkMemMgr_SharedMemory_Params),
                                             0,
                                             NULL);

                Memory_copy(cmdArgs.args.create.params,
                            shSyslinkMemMgrParams,
                            sizeof(SyslinkMemMgr_SharedMemory_Params));

                if(shSyslinkMemMgrParams->heapHandle != NULL) {
                ((SyslinkMemMgr_SharedMemory_Params *)cmdArgs.args.create.params)->
                    heapHandle =  Heap_getKnlHandle (shSyslinkMemMgrParams->heapHandle);
                }
                else {
                    ((SyslinkMemMgr_SharedMemory_Params *)cmdArgs.args.create.params)->
                    heapHandle  = NULL;
                }

                status = SyslinkMemMgrDrv_ioctl(CMD_MEMMGR_CREATE, &cmdArgs);
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "SyslinkMemMgr_create",
                                         status,
                                         "API (through IOCTL) failed on kernel-side!");
                }
                else {
                    obj = (SyslinkMemMgr_Object *)Memory_alloc (NULL,
                                                         sizeof (SyslinkMemMgr_Object),
                                                         0,
                                                         NULL);
                    if (obj == NULL) {
                        status = MEMMGR_E_FAIL;
                    }
                    else {
//                        obj.fxns.create = &SyslinkMemMgr_SharedMemory_create;
                        String_cpy((String)obj->name,
                               (String)commonCreParams->name);
                        obj->interfaceType = SyslinkMemMgr_TYPE_SHAREDMEM;
                        obj->knlObject = cmdArgs.args.create.handle;
                    }
                }
            }
            break;

            case SyslinkMemMgr_TYPE_TILER:
            {   /* TODO*/
                status = MEMMGR_E_NOTIMPLEMENTED;
            }
            break;

            default:
                status = MEMMGR_E_INVALIDARG;
        }
        if (status >= 0) {
            handle = obj;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */
    return (handle);
}

/* Function to delete the created Memory Manager  instance*/
Int32
SyslinkMemMgr_delete (SyslinkMemMgr_Handle * pHandle)
{
    Int32 status = MEMMGR_SUCCESS;
    SyslinkMemMgr_Object * obj = *pHandle;
    SyslinkMemMgrDrv_CmdArgs      cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "SyslinkMemMgr_delete", pHandle);

    GT_assert (curTrace, (pHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (SyslinkMemMgr_state.setupRefCount == 0) {
        /*! @retval MEMMGR_E_INVALIDSTATE Modules is invalidstate */
        status = MEMMGR_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_delete",
                             MEMMGR_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (pHandle == NULL) {
        /*! @retval MEMMGR_E_INVALIDARG pointer to handle passed is null */
        status = MEMMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_delete",
                             MEMMGR_E_INVALIDARG,
                             "pointer to handle passed is null!");
    }
    else if (*pHandle == NULL) {
        /*! @retval MEMMGR_E_INVALIDARG handle passed is null */
        status = MEMMGR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_delete",
                             MEMMGR_E_INVALIDARG,
                             "params passed "
                             "is null!");
    }
    else if ((*pHandle)->knlObject == NULL) {
        /*! @retval MEMMGR_E_INVALIDARG kernel handle is null */
        status = MEMMGR_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_delete",
                             MEMMGR_E_INVALIDARG,
                             "pkernel handle is null !");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = *pHandle;

        cmdArgs.args.deleteInstance.handle = (*pHandle)->knlObject;

        status = SyslinkMemMgrDrv_ioctl (CMD_MEMMGR_DELETE, &cmdArgs);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SyslinkMemMgr_delete",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
            Memory_free (NULL,
                        (* pHandle),
                        sizeof (SyslinkMemMgr_Object));
            * pHandle = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    return (status);
}


/* Function to allocate memory from the SyslinkMemMgr.
 * TODO: implement it in user space.
 */
Ptr
SyslinkMemMgr_alloc (SyslinkMemMgr_Object * obj , UInt32 size, UInt32 align)
{
    Ptr   ptr   = NULL;

    return (ptr);
}


/* Function to allocate memory from the SyslinkMemMgr
 * TODO: implement it in user space.
 */
Int32
SyslinkMemMgr_free (SyslinkMemMgr_Object * obj, Ptr buf, UInt32 size)
{
    Int32 status = MEMMGR_SUCCESS;

    status = MEMMGR_E_NOTIMPLEMENTED;

    return (status);
}

/* Function to map. not used for shared memory.
 * TODO: implement it in user space.
 */
Ptr
SyslinkMemMgr_map (SyslinkMemMgr_Object * obj, Ptr arg)
{
    Ptr   ptr = NULL;

    return (ptr);
}

/* Function to do unmap
 * TODO: implement it in user space.
 */
Int32
SyslinkMemMgr_unmap (SyslinkMemMgr_Object * obj, Ptr arg)
{
    Int32 status = MEMMGR_SUCCESS;

    status = MEMMGR_E_NOTIMPLEMENTED;

    return (status);
}

/* Function to translate source address to destination address type.
 * TODO: implement it in user space.
 */
Ptr
SyslinkMemMgr_translate (SyslinkMemMgr_Object  *obj,
                  Ptr             srcAddr,
                  SyslinkMemMgr_AddrType srcAddrType,
                  SyslinkMemMgr_AddrType desAddrType)
{
    Ptr   ptr   = NULL;

    /*TODO: implement  */

//    if ( (obj != NULL) && (obj->handle != NULL)) {
//        if (obj->fxns->translate != NULL) {
//            ptr = obj->fxns->translate(obj->handle,
//                                       srcAddr,
//                                       srcAddrType,
//                                       desAddrType);
//        }
//    }

    return (ptr);
}

/*!
 *  @brief Function to get the kernel object pointer embedded in userspace heap.
 *         Some SyslinkMemMgr implementations return the kernel object handle.
 *         SyslinkMemMgrs which do not have kernel object pointer embedded return NULL.
 *
 *  @params handle handle to a heap instance
 *
 *  @sa
 */
Void *
SyslinkMemMgr_getKnlHandle (SyslinkMemMgr_Handle handle)
{
    SyslinkMemMgr_Object * Obj = (SyslinkMemMgr_Object *) handle;
    Ptr           knlHandle = NULL;

    GT_1trace (curTrace, GT_ENTER, "SyslinkMemMgr_getKnlHandle", handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval NULL Handle passed is invalid */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_getKnlHandle",
                             MEMMGR_E_INVALIDARG,
                             "Handle passed is invalid!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

           knlHandle = Obj->knlObject;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "SyslinkMemMgr_getKnlHandle");

    /*! @retval Kernel-object-handle Operation successfully completed. */
    return (knlHandle);
}

/*
 *  ======== SyslinkMemMgr_SharedMemory_Params_init ========
 *  Function to get the default parameters of the SharedMemory SyslinkMemMgr.
 *
 *  This function can be called by the application to get their
 *  default parameters.
 *
 */
void SyslinkMemMgr_SharedMemory_Params_init(SyslinkMemMgr_SharedMemory_Params *params)
{
    if(params != NULL) {
        params->commonCreateParams.type = SyslinkMemMgr_TYPE_SHAREDMEM;
        params->commonCreateParams.size = sizeof (SyslinkMemMgr_SharedMemory_Params);
        String_cpy((String)params->commonCreateParams.name,"\0");
        params->heapHandle = NULL;
    }
}

#if 0
/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation for  shared memory.
 *              NOTE: Only supports returning default params.ie. first arg handle
 *              is ignored.
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GatePeterson_create
 */
Void
SyslinkMemMgr_SharedMemory_Params_init (Ptr handle,
                                 SyslinkMemMgr_SharedMemory_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status  = FRAMEQ_SUCCESS;
#endif
    SyslinkMemMgrDrv_CmdArgs       cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "SyslinkMemMgr_SharedMemory_Params_init", params);
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (SyslinkMemMgr_state.setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_SharedMemory_Params_init",
                             FRAMEQ_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgr_SharedMemory_Params_init",
                             FRAMEQ_E_INVALIDARG,
                             "Argument of type (SyslinkMemMgr_SharedMemory_Params_init *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
        cmdArgs.args.ParamsInit.handle = NULL ;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        SyslinkMemMgrDrv_ioctl (CMD_MEMMGR_SHAREDMEM_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SyslinkMemMgr_SharedMemory_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "SyslinkMemMgr_SharedMemory_Params_init");
}

#endif

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
