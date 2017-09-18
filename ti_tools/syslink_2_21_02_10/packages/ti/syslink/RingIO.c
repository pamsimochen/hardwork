/*
 *  @file   RingIO.c
 *
 *  @brief      RingIO module implementation
 *
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



#if defined(SYSLINK_BUILD_RTOS)
/*-------------------------    XDC  specific includes ----------------------  */
#include <ti/sdo/ipc/Ipc.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Gate.h>
#ifdef xdc_target__isaCompatible_64P
#include <ti/sysbios/family/c64p/Cache.h>
#else
#include <ti/sysbios/hal/Cache.h>
#endif
/*-------------------------  BIOS includes ---------------------------------- */
#include <ti/sysbios/gates/GateMutex.h>
/*-------------------------    Generic includes  ---------------------------- */
#include <string.h>
/*-------------------------    IPC module specific includes ------------------*/
#include <ti/sdo/utils/_NameServer.h>
#include <ti/sdo/utils/List.h>
#endif /* #if defined(SYSLINK_BUILD_RTOS) */

#if defined(SYSLINK_BUILD_HLOS)
/* Standard headers */
#include <ti/syslink/Std.h>

/* Standard headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/OsalSemaphore.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/List.h>
#endif /* #if defined(SYSLINK_BUILD_HLOS) */


#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/inc/IRingIO.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>

/* =============================================================================
 * Defines
 * =============================================================================
 */
#define RINGIO_MODULE_NAME             "RingIO_Module"
#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

/* =============================================================================
 * Internal structures
 * =============================================================================
 */

/*!
 *  @var    RingIO_ModuleObject
 *
 *  @brief  RingIO information data structure to be added to Name Server
 */
// TBD : gate handle removed use xdcruntime gate for local protection
typedef struct RingIO_ModuleObject {
    UInt32              refCount;
    /* TBD : can not be atomic */
    List_Handle         createObjList;
    /* List holding created objects */
    List_Handle         openObjList;
    /* List holding created objects */
    RingIO_Params       defaultInstParams;
    /*!< Default config values */
    RingIO_Handle       localRingIOHandle;
    /*!< Default config values */
    NameServer_Handle   nsHandle;
    /*!< Namesever handle */
} RingIO_ModuleObject ;

/* =============================================================================
 * Globals
 * =============================================================================
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
RingIO_ModuleObject RingIO_state =
{
    .refCount = 0,
    .defaultInstParams.name = NULL,
    .defaultInstParams.interfaceType = RingIO_Type_SHAREDMEM
};

/*!
 *  @var    RingIO_moduleCfg
 *
 *  @brief  RingIO module configuration. This will be referred to by all
 *          delegates in order to get access to RingIO module level
 *          configuration information.
 */
RingIO_Config       RingIO_moduleCfg;

/*!
 *  @var    Syslink_RingIO_moduleCfg
 *
 *  @brief  C based RingIO configuration
 */
extern RingIO_Config       Syslink_RingIO_moduleCfg;

/*!
 *  @var    RingIO_module
 *
 *  @brief  Pointer to the RingIO module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
RingIO_ModuleObject * RingIO_module = &RingIO_state;



/*
 *  ======== RingIO_delete ========
 *  Delete RingIO instance.
 */
Int RingIO_delete(RingIO_Handle * handlePtr)
{

    Int              status = RingIO_S_SUCCESS;
    List_Elem *      elem   = NULL;
    IArg             key;

    GT_1trace (curTrace, GT_ENTER, "RingIO_delete", handlePtr);

    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "handle passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else if (((RingIO_Object *)(*handlePtr))->created != TRUE) {
        status = RingIO_E_INVALIDCONTEXT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_delete",
                             status,
                             "*handlePtr passed is not a valid handle!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        if (   (NULL != RingIO_module->nsHandle) && (((RingIO_Object * )(*handlePtr))->nsKey != 0)) {
            NameServer_removeEntry ((NameServer_Handle) RingIO_module->nsHandle,
                                    ((RingIO_Object * )(*handlePtr))->nsKey);
        }

        /* Take the local lock */
        key = Gate_enterSystem();

        /* Close any RingIO Opened objects that have not been closed so far.
         */
        for ((elem = List_next (RingIO_module->openObjList, NULL));
              elem != NULL;
              elem = List_next (RingIO_module->openObjList, elem)){
            if (strcmp (((RingIO_Object *)elem)->name,
                       ((RingIO_Object * )(*handlePtr))->name) == 0) {
                    status = RingIO_close  ((RingIO_Handle*)
                                 &(((RingIO_Object *)elem)->top));
            }
        }

        List_remove (RingIO_module->createObjList,
                     &(((RingIO_Object * )(*handlePtr))->listElem));
        Gate_leaveSystem(key);

        /* Delete the RingIO after closing it */
        if(status >= 0) {
            status = IRingIO_delete ((IRingIO_Handle *)handlePtr);
        }

        /* Release the local lock */
        if (((RingIO_Object * )(*handlePtr)) != NULL) {
            Memory_free(NULL,
                        ((RingIO_Object * )(*handlePtr)),
                        sizeof (RingIO_Object));
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_delete", status);

    return status;
}

/*
 *  ======== RingIO_openByAddr ========
 *  Function to open a RingIO.params should be of type specific to
 *  implementation.
 */
Int
RingIO_openByAddr (      Ptr                 ctrlSharedAddr,
                   const RingIO_openParams * params,
                         RingIO_Handle     * handlePtr)

{
    Int                     status     = RingIO_S_SUCCESS;
    RingIO_Object*          handle     = NULL;
    RingIO_InterfaceAttrs * interfaceAttrs;
    IArg                    key;
    UInt32                  index;
    UInt16                  type;


    GT_3trace (curTrace,
               GT_ENTER,
               "RingIO_openByAddr",
               ctrlSharedAddr,
               params,
               handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (params    != NULL));
    /* procIds can be NULL */

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_openByAddr",
                             status,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_openByAddr",
                             status,
                             "Structure of type RingIO_Params passed is NULL!");
    }
    else if (handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_openByAddr",
                             status,
                             "Structure of type (RingIO_Handle *) passed is NULL!");
    }
    else if (params->openMode > RingIO_MODE_WRITER) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_openByAddr",
                             status,
                             "Invalid value of openMode passed!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        interfaceAttrs = (RingIO_InterfaceAttrs * ) ctrlSharedAddr;
        /* Assert that sharedAddr is cache aligned */
        index = SharedRegion_getId(interfaceAttrs);
        /* Assert that the buffer is in a valid shared region */
        GT_assert (curTrace, (index != SharedRegion_INVALIDREGIONID));
        GT_assert (curTrace, ((UInt32)ctrlSharedAddr %
                              SharedRegion_getCacheLineSize(index) == 0));

       if (SharedRegion_isCacheEnabled(index)) {
            /* Invalidating the entry located in shared memory. The memory
             * provided by this entry should be in cache aligend size
             */
            key = Gate_enterSystem();
            Cache_inv (interfaceAttrs,
                       sizeof (RingIO_InterfaceAttrs),
                       Cache_Type_ALL,
                       TRUE);
            Gate_leaveSystem (key);
        }
        type = interfaceAttrs->type;
        handle =  (RingIO_Object *) Memory_calloc (NULL,
                                                   sizeof (RingIO_Object),
                                                   0u,
                                                   NULL);

        GT_assert (curTrace, (handle != NULL));
        if (handle != NULL) {
            status = IRingIO_openByAddr (ctrlSharedAddr,
                                         params,
                                         (RingIO_Handle *)&handle,
                                         type);
            if(status >= 0) {
                *handlePtr = (RingIO_Handle )handle;
                handle->created = FALSE;
            }
        }
        else {
            status = RingIO_E_FAIL;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_openByAddr",
                             status,
                             "Memory Allocation Failed!");
        }

        /* Add object to the list since handle is different when open is called */
        if (status >= 0) {
            /* Take the local lock */
            key = Gate_enterSystem();
            /* Put in the local list */
            List_elemClear (&(((RingIO_Object * )(*handlePtr))->listElem));
            List_put (RingIO_module->openObjList, &(((RingIO_Object * )(*handlePtr))->listElem));
            Gate_leaveSystem (key);
        }
        else {
            if (handle != NULL) {
                Memory_free(NULL, handle, sizeof (RingIO_Object));
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_openByAddr", status);

    return status;
}

/*!
 *  ======== RingIO_close ========
 *  Close previously opened/created instance of RingIO module.
 */
Int
RingIO_close (RingIO_Handle * handle)
{
    Int  status = RingIO_S_SUCCESS;
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "RingIO_close", handle);

    GT_assert (curTrace, ((handle != NULL) && (*handle != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDARG,
                             "Pointer to RingIO Handle passed is null!");
    }
    else if (*handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDARG,
                             "RingIO Handle passed is null!");
    }
    else if (((RingIO_Object *)*handle)->created != FALSE) {
        status = RingIO_E_INVALIDCONTEXT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_close",
                             RingIO_E_INVALIDCONTEXT,
                             "Handle passed is not a valid handle!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        status = IRingIO_close ((IRingIO_Handle *)handle);

        /* Remove the object from local list */
        if(status >= 0) {
            /* Take the local lock */
            key = Gate_enterSystem();
            List_remove (RingIO_module->openObjList,
                         &(((RingIO_Object * )(*handle))->listElem));
            Gate_leaveSystem(key);
            /* Free the memory allocated for name in RingIO_open*/
            if (((RingIO_Object * )(*handle)) != NULL) {
                Memory_free(NULL,
                            ((RingIO_Object * )(*handle))->name,
                            strlen(((RingIO_Object * )(*handle))->name) + 1);
            }
            /* Free the memory allocated for handle*/
            if (((RingIO_Object * )(*handle)) != NULL) {
                Memory_free(NULL,
                            ((RingIO_Object * )(*handle)),
                            sizeof (RingIO_Object));
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_close", status);

    return status;
}
/*
 *  ============= RingIO_registerNotifier ========
 *  Function to register a call back function.
 */
Int
RingIO_registerNotifier (RingIO_Handle      handle,
                         RingIO_NotifyType  notifyType,
                         UInt32             watermark,
                         RingIO_NotifyFxn   notifyFunc,
                         Ptr                cbContext)
{
    Int status = RingIO_S_SUCCESS;

    GT_5trace (curTrace,
               GT_ENTER,
              "RingIO_registerNotifier",
               handle,
               notifyType,
               watermark,
               notifyFunc,
               cbContext);

    GT_assert (curTrace, handle != NULL);
    GT_assert (curTrace, notifyFunc != NULL);
    GT_assert (curTrace, cbContext != NULL);


#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
   if (RingIO_module->refCount == 0){
       /*! @retval RingIO_E_INVALIDSTATE
        *          Module was not initialized
        */
       status = RingIO_E_INVALIDSTATE;
       GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "RingIO_registerNotifier",
                            status,
                            "Module was not initialized!");
   }
   else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_registerNotifier",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else if (notifyFunc == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_registerNotifier",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL notifyFunc passed");
    }
    else if (cbContext == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_registerNotifier",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL cbContext passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    status = IRingIO_registerNotifier ((IRingIO_Handle)handle,
                                       notifyType,
                                       watermark,
                                       notifyFunc,
                                       cbContext);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

   GT_1trace (curTrace, GT_LEAVE, "RingIO_registerNotifier", status);

    return (status);
}
/*
 *  ========== RingIO_unregisterNotifier ===========
 *  Function to un-register call back function.
 */
Int
RingIO_unregisterNotifier (RingIO_Handle      handle)
{
    Int status = RingIO_S_SUCCESS;
    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_unregisterNotifier",
               handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
   if (RingIO_module->refCount == 0){
       /*! @retval RingIO_E_INVALIDSTATE
        *          Module was not initialized
        */
       status = RingIO_E_INVALIDSTATE;
       GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "RingIO_unregisterNotifier",
                            status,
                            "Module was not initialized!");
   }
    if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_unregisterNotifier",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    status = IRingIO_unregisterNotifier ((IRingIO_Handle)handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    return (status);
}

/*
 *  ============== RingIO_setWatermark =========
 *  Function to set client's watermark
 */
Int
RingIO_setWaterMark (RingIO_Handle handle,
                     UInt32        watermark)
{
    Int status = RingIO_S_SUCCESS;

    GT_2trace (curTrace,
               GT_ENTER,
              "RingIO_setWaterMark",
               handle,
               watermark);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setWaterMark",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL handle passed");
    }
    else {
#endif
        status = IRingIO_setWaterMark((IRingIO_Handle)handle,
                                      watermark);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "RingIO_setWaterMark", status);

    return (status);
}

/*
 *  ============= RingIO_setNotifyType ==================
 *  Function to set notification type.
 */
Int
RingIO_setNotifyType (RingIO_Handle      handle,
                      RingIO_NotifyType  notifyType)
{
    Int status = RingIO_S_SUCCESS;
    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_setNotifyType",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setNotifyType",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL handle passed");
    }
    else {
#endif
    status = IRingIO_setNotifyType ((IRingIO_Handle)handle,
                                    notifyType);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "RingIO_setNotifyType", status);

    return (status);
}
/*!
 *  ======== RingIO_acquire ==============
 *  Acquire data from the RingIO instance
 */
Int
RingIO_acquire (RingIO_Handle       handle,
                RingIO_BufPtr     * pData,
                UInt32            * pSize)
{
    Int status = RingIO_S_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "RingIO_acquire", handle, pData, pSize);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (pData != NULL));
    GT_assert (curTrace, (pSize != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_acquire",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_acquire",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else if (pData == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_acquire",
                             status,
                             "Pointer of type RingIO_BufPtr passed is NULL!");
    }
    else if (pSize == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_acquire",
                             status,
                             "Pointer of type UInt32 for size passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    status = IRingIO_acquire ((IRingIO_Handle)handle, pData, pSize);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_acquire", status);

    return status;
}
/*!
 *  =========== RingIO_release ================
 *  Release data from the RingIO instance
 */
Int
RingIO_release (RingIO_Handle handle,
                UInt32        size)
{
    Int status = RingIO_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "RingIO_release", handle, size);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_release",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_release",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        status = IRingIO_release ((IRingIO_Handle)handle, size);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_release", status);

    return (status);
}
/*!
 *  ================= RingIO_cancel ==========
 *  Cancel the previous acquired buffer to
 *  the RingIO instance
 */
Int
RingIO_cancel (RingIO_Handle handle)
{
    Int status = RingIO_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "RingIO_cancel", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_cancel",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_cancel",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        status = IRingIO_cancel ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_cancel", status);

    return status;
}

/*!
 *  ============ RingIO_getvAttribute ===============
 *  Get attribute from the RingIO instance
 */
Int
RingIO_getvAttribute (RingIO_Handle handle,
                      UInt16 *      type,
                      UInt32 *      param,
                      RingIO_BufPtr vptr,
                      UInt32 *      pSize)
{
    Int status = RingIO_S_SUCCESS;

    GT_5trace (curTrace, GT_ENTER, "RingIO_getvAttribute", handle,
                                                           type,
                                                           param,
                                                           vptr,
                                                           pSize);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (type   != NULL));
    GT_assert (curTrace, (param  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getvAttribute",
                             status,
                             "Module was not initialized!");
    }
    else if(handle == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getvAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else if(type == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getvAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL type passed");
    }
    else if(param == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getvAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL param passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    status = IRingIO_getvAttribute ((IRingIO_Handle)handle,
                                    type,
                                    param,
                                    vptr,
                                    pSize);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getvAttribute", status);

    return status;
}

/*!
 *  ============ RingIO_setvAttribute ===============
 *  Set attribute from the RingIO instance
 */
Int
RingIO_setvAttribute (RingIO_Handle  handle,
                      UInt16         type,
                      UInt32         param,
                      RingIO_BufPtr  pData,
                      UInt32         size,
                      Bool           sendNotification)
{
    Int status = RingIO_S_SUCCESS;

    GT_6trace (curTrace, GT_ENTER, "RingIO_setvAttribute", handle,
                                                           type,
                                                           param,
                                                           pData,
                                                           size,
														   sendNotification);
    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setvAttribute",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    status = IRingIO_setvAttribute ((IRingIO_Handle)handle,
                                    type,
                                    param,
                                    pData,
                                    size,
                                    sendNotification);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setvAttribute", status);

    return status;
}


/*!
 *  ============ RingIO_getAttribute ===============
 *  Get attribute from the RingIO instance
 */
Int
RingIO_getAttribute (RingIO_Handle  handle,
                     UInt16 *       type,
                     UInt32 *       param)
{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (type   != NULL));
    GT_assert (curTrace, (param  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAttribute",
                             status,
                             "Module was not initialized!");
    }
    else if(handle == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else if(type == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL type passed");
    }
    else if(param == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL param passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        status = IRingIO_getAttribute ((IRingIO_Handle)handle,
                                       type,
                                       param);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getAttribute", status);

    return(status);
}


/*!
 *  ============ RingIO_setAttribute ===============
 *  Set attribute in the RingIO instance
 */
Int
RingIO_setAttribute (RingIO_Handle  handle,
                     UInt16         type,
                     UInt32         param,
                     Bool           sendNotification)
{
    Int               status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0){
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setAttribute",
                             status,
                             "Module was not initialized!");
    }
    else if(handle == NULL){
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setAttribute",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        status = IRingIO_setAttribute ((IRingIO_Handle)handle,
                                       type,
                                       param,
                                       sendNotification);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif  /*  #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)*/

    GT_1trace (curTrace, GT_LEAVE, "RingIO_setAttribute", status);

    return(status);
}

/*!
 *  ================= RingIO_flush ===============
 *  Flush the buffer in the RingIO instance
 */
Int
RingIO_flush (RingIO_Handle  handle,
              Bool           hardFlush,
              UInt16 *       type,
              UInt32 *       param,
              UInt32 *       bytesFlushed)
{
    Int                       status = RingIO_S_SUCCESS;

    GT_5trace (curTrace, GT_ENTER, "RingIO_flush", handle,
                                                   hardFlush,
                                                   type,
                                                   param,
                                                   bytesFlushed);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (type != NULL));
    GT_assert (curTrace, (param != NULL));
    GT_assert (curTrace, (bytesFlushed != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_flush",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_flush",
                             status,
                             "Invalid NULL Handle passed");
    }
    else if (type == NULL) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_flush",
                             status,
                             "Invalid NULL type passed");
    }
    else if (param == NULL) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_flush",
                             status,
                             "Invalid NULL param passed");
    }
    else if (bytesFlushed == NULL) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_flush",
                             status,
                             "Invalid NULL bytesFlushed passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
    status = IRingIO_flush ((IRingIO_Handle)handle,
                            hardFlush,
                            type,
                            param,
                            bytesFlushed);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_flush", status);

    return status;
}

/*!
 *  ==================RingIO_sendNotify ===============
 *  Send a force notification to the other client
 */
Int
RingIO_sendNotify (RingIO_Handle     handle,
               RingIO_NotifyMsg  msg)
{
    Int  status = RingIO_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "RingIO_sendNotify", handle,
                                                    msg);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_sendNotify",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_sendNotify",
                             status,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    status = IRingIO_sendNotify ((IRingIO_Handle)handle,
                             msg);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_sendNotify", status);

    return status;
}

/*
 *  ============= RingIO_getValidSize ==================
 *  Function to get valid size.
 */
UInt32
RingIO_getValidSize (RingIO_Handle handle)
{
    UInt32 validSize = (UInt32) -1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getValidSize",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getValidSize",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getValidSize",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
    validSize = IRingIO_getValidSize ((IRingIO_Handle)handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getValidSize", validSize);
    return (validSize);
}

/*
 *  =========== RingIO_getEmptySize ==========
 *  Function to get empty size.
 */
UInt32
RingIO_getEmptySize (RingIO_Handle handle)
{
    UInt32 validSize = (UInt32) -1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getEmptySize",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getEmptySize",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getEmptySize",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        validSize = IRingIO_getEmptySize ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getEmptySize", validSize);
    return (validSize);
}

/*
 *  ============= RingIO_getValidAttrSize ==========
 *  Function to get valid attribute size.
 */
UInt32
RingIO_getValidAttrSize (RingIO_Handle handle)
{
    UInt32 validSize = (UInt32) -1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getValidAttrSizev",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getValidAttrSize",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getValidAttrSize",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        validSize = IRingIO_getValidAttrSize ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getValidAttrSize", validSize);
    return (validSize);
}

/*
 *  ============= RingIO_getEmptyAttrSize ==========
 *  Function to get empty attribute size.
 */
UInt32
RingIO_getEmptyAttrSize (RingIO_Handle handle)
{
    UInt32 validSize = (UInt32) -1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getEmptyAttrSize",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getEmptyAttrSize",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getEmptyAttrSize",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        validSize = IRingIO_getEmptyAttrSize ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getEmptyAttrSize", validSize);
    return (validSize);

}

/*
 *  =============== RingIO_getAcquiredOffset =========
 *  Function to get client's acquired offset
 */
UInt32
RingIO_getAcquiredOffset (RingIO_Handle handle)
{
    UInt32 acquiredOffset = (UInt32) -1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getAcquiredOffset",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAcquiredOffset",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAcquiredOffset",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        acquiredOffset = IRingIO_getAcquiredOffset ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getAcquiredOffset", acquiredOffset);

    return (acquiredOffset);
}

/*
 *  ================= RingIO_getAcquiredSize ========
 *  Function to get client's acquired size
 */
UInt32
RingIO_getAcquiredSize (RingIO_Handle handle)
{
    UInt32 acquiredSize = (UInt32)-1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getAcquiredSize",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAcquiredSize",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getAcquiredSize",
                             RingIO_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        acquiredSize = IRingIO_getAcquiredSize ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getAcquiredSize", acquiredSize);

    return (acquiredSize);
}

/*
 *  ============== RingIO_getWatermark =========
 *  Function to get client's watermark
 */
UInt32
RingIO_getWaterMark (RingIO_Handle handle)
{
    UInt32 watermark = (UInt32)-1;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getWaterMark",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getWaterMark",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getWaterMark",
                             RingIO_E_INVALIDSTATE,
                             "Invalid NULL Handle passed");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        watermark = IRingIO_getWaterMark ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getWaterMark", watermark);

    return (watermark);
}

/* =============================================================================
 * APIS
 * =============================================================================
 */
/*
 *  ======== RingIO_getConfig ========
 *  Return the module wide default config params.
 */
Void
RingIO_getConfig (RingIO_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "RingIO_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getConfig",
                             RingIO_E_INVALIDARG,
                             "Argument of type (RingIO_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        memcpy (cfg,
                &Syslink_RingIO_moduleCfg,
                sizeof (RingIO_Config));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_0trace (curTrace, GT_ENTER, "RingIO_getConfig");
}


/*
 *  ======== RingIO_setup ========
 *  Function to setup the RingIO module.
 */
Int RingIO_setup(const RingIO_Config *cfg)
{
    Int32 status = RingIO_S_SUCCESS;
    NameServer_Params nsParams;
    RingIO_Config     tmpCfg;
    List_Params listParams;
    IArg key;
    Error_Block eb;

    GT_1trace(curTrace, GT_ENTER, "RingIO_setup", cfg);
    Error_init(&eb);

    key = Gate_enterSystem();
    RingIO_module->refCount++;
    if (RingIO_module->refCount > 1) {
        status = RingIO_S_ALREADYSETUP;
        Gate_leaveSystem(key);
    }
    else {
        Gate_leaveSystem(key);

        if (cfg == NULL) {
            RingIO_getConfig(&tmpCfg);
            cfg = &tmpCfg;
        }
        memcpy(&RingIO_moduleCfg, cfg, sizeof(RingIO_Config));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (cfg->maxNameLen == 0) {
            status = RingIO_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS, "RingIO_setup", status,
                    "cfg->maxNameLen passed is 0!");
        }
        else if (cfg->maxInstances == 0) {
            status = RingIO_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS, "RingIO_setup", status,
                    "cfg->maxInstances passed is 0!");
        }
        else {
#endif

            /* Initialize the parameters */
            NameServer_Params_init(&nsParams);
            nsParams.maxNameLen        = RingIO_moduleCfg.maxNameLen;
            nsParams.maxRuntimeEntries = RingIO_moduleCfg.maxInstances;
            nsParams.checkExisting     = TRUE;
            nsParams.maxValueLen       = sizeof(RingIO_NameServerEntry);

            /* Create the nameserver for modules */
            RingIO_module->nsHandle = NameServer_create(RINGIO_MODULE_NAME,
                    &nsParams);
            GT_assert(curTrace, (NULL != RingIO_module->nsHandle));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            if (RingIO_module->nsHandle == NULL) {
                status = RingIO_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "RingIO_setup",
                        status, "Failed to create the RingIO NameServer");
            }
            else {
#endif

                /* Construct the list object */
                List_Params_init(&listParams);
                RingIO_module->createObjList =  List_create (&listParams, &eb);
                GT_assert(curTrace, (NULL != RingIO_module->createObjList));
                List_Params_init (&listParams);
                RingIO_module->openObjList =  List_create (&listParams, &eb);
                GT_assert (curTrace, (NULL !=  RingIO_module->openObjList));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            }
        }
#endif
        if (status < 0) {
            RingIO_destroy ();
        }
    }
    return status;
}

/*
 *  ======== RingIO_destroy ========
 *  Function to destroy the RingIO module.
 */
Int RingIO_destroy(Void)
{
    Int32 status = RingIO_S_SUCCESS;
    IArg key;

    GT_0trace(curTrace, GT_ENTER, "RingIO_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace, GT_4CLASS, "RingIO_destroy", status,
                "Module was not initialized!");
    }
    else {
#endif
        key = Gate_enterSystem();
        RingIO_module->refCount--;
        if (RingIO_module->refCount == 0) {
            Gate_leaveSystem(key);
            /* Check for RingIO Opened objects that have not been closed */
            if (!List_empty(RingIO_module->openObjList)) {
                GT_0trace(curTrace, GT_1CLASS, "RingIO_destroy called but there"
                    " are still opened instances that were not closed.");
            }
            /* Check for RingIO created objects that have not been deleted */
            if (!List_empty(RingIO_module->createObjList)) {
                GT_0trace(curTrace, GT_1CLASS, "RingIO_destroy called but there"
                    " are still created instances that were not deleted.");
            }

            /* Delete the nameserver for modules */
            if (RingIO_module->nsHandle != NULL) {
                status = NameServer_delete (&RingIO_module->nsHandle);
            }
            GT_assert (curTrace, (status >= 0));

            /* Delete the list objects */
            List_delete(&RingIO_module->openObjList);
            List_delete(&RingIO_module->createObjList);
        }
        else {
            Gate_leaveSystem(key);
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "RingIO_destroy", status);

    return (status);
}

/*
 *  ======== RingIO_create ========
 *  Function to create a RingIO. params should be of type specific to
 *  implementation.
 */
RingIO_Handle
RingIO_create (const Void *  params)
{
    Int                    status       = RingIO_S_SUCCESS;
    RingIO_Object *        handle       = NULL;
    IArg                   key;
    RingIO_NameServerEntry nameServerEntry;
    SharedRegion_SRPtr     srPtr;

    GT_1trace (curTrace, GT_ENTER, "RingIO_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason(curTrace, GT_4CLASS, "RingIO_create",
                RingIO_E_INVALIDSTATE, "Module was not initialized!");
    }
    else {
#endif
        handle = (RingIO_Object *)Memory_calloc(NULL, sizeof(RingIO_Object),
                0u, NULL);

        GT_assert(curTrace, (handle != NULL));
        if (handle == NULL ) {
            GT_setFailureReason(curTrace, GT_4CLASS, "IRingIO_create",
                    RingIO_E_FAIL, "RingIO create failed");
        }
        else {
            status = IRingIO_create(params, (RingIO_Handle *)&handle);

            if (status >= 0) {
                srPtr = SharedRegion_getSRPtr ((Ptr)handle->ctrlSharedAddr,
                                           handle->ctrlRegionId);
                nameServerEntry.ctrlSharedSrPtr  = srPtr;
                nameServerEntry.procId           = MultiProc_self();
                handle->ownerProcId        = nameServerEntry.procId;
                handle->created            = TRUE;
                handle->nsKey              = NameServer_add (
                                     (NameServer_Handle)RingIO_module->nsHandle,
                                      handle->name,
                                      &nameServerEntry,
                                      sizeof(RingIO_NameServerEntry));

                if (handle->nsKey != NULL){
                    /* Take the local lock */
                    key = Gate_enterSystem();
                    /* Put in the local list */
                    List_elemClear (&(handle->listElem));
                    List_put (RingIO_module->createObjList, &(handle->listElem));

                    Gate_leaveSystem (key);
                }
                else {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "RingIO_create",
                                         RingIO_E_FAIL,
                                         "NameServer_add failed");
                    status = IRingIO_delete ((IRingIO_Handle *)&handle);
                    if (status >= 0) {
                        /* Free the memory allocated for handle */
                        if (handle != NULL) {
                            Memory_free(NULL, handle, sizeof(RingIO_Object));
                        }
                        handle = NULL;
                    }
                }
            }
            else {
                /* Free the memory allocated for handle */
                if (handle != NULL) {
                    Memory_free(NULL, handle, sizeof(RingIO_Object));
                }
                handle = NULL;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "RingIO_create", handle);
    return ((RingIO_Handle)handle);
}


/*
 *  ======== RingIO_open ========
 *  Function to open a RingIO. params should be of type specific to
 *  implementation.
 */
Int
RingIO_open (      String              name,
             const RingIO_openParams * params,
                   UInt16            * procIds,
                   RingIO_Handle     * handlePtr)

{
    Int                    status            = RingIO_S_SUCCESS;
    Ptr                    sharedAddr        = NULL;
    Int                    rc                = 0;
    List_Elem *            elem              = NULL;
    Bool                   doneFlag          = FALSE;
    UInt32                 len;
    RingIO_NameServerEntry nameServerEntry;

    GT_4trace (curTrace, GT_ENTER, "RingIO_open", name, params, procIds, handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, (params    != NULL));
    /* procIds can be NULL */

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_open",
                             status,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_open",
                             status,
                             "Structure of type RingIO_Params passed is NULL!");
    }
    else if (handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_open",
                             status,
                             "Structure of type (RingIO_Handle *) passed is NULL!");
    }
    else if (params->openMode > RingIO_MODE_WRITER) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_open",
                             status,
                             "Invalid value of openMode passed!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        *handlePtr = NULL;
        /* First check in the local list */
        for ((elem = List_next (RingIO_module->createObjList, NULL));
              elem != NULL;
              elem = List_next (RingIO_module->createObjList, elem)){
            if (name != NULL) {
                if (strcmp (((RingIO_Object *)elem)->name,
                                  name) == 0) {
                    *handlePtr = (((RingIO_Object *)elem)->top);
                    //handle    = (RingIO_Object *) (*handlePtr);
                    sharedAddr = (Ptr)
                             (((RingIO_Object *)elem)->ctrlSharedAddr);
                    doneFlag = TRUE;

                    break;
                }
            }
        }
        /* Check in the remote list */
        if (doneFlag == FALSE) {
            len = sizeof (RingIO_NameServerEntry);
            rc = NameServer_get (RingIO_module->nsHandle,
                                 name,
                                 &nameServerEntry,
                                 &len,
                                 procIds);
            if (rc == NameServer_E_NOTFOUND) {
                /* Name not found. */
                /* Failed to get the information from the name server.
                 * It means it is not created.
                 */
                status = RingIO_E_NOTFOUND;
            }
            else if (rc == NameServer_E_FAIL) {
                /* Error happened in NameServer. Pass the error up. */
                status = RingIO_E_FAIL;
            }
            else {
                /* Get the virtual address from the shared memory */
               sharedAddr = (Ptr) SharedRegion_getPtr (nameServerEntry.ctrlSharedSrPtr) ;
               GT_assert(curTrace, (sharedAddr != NULL));
               doneFlag = TRUE;
            }
        }

        if (status >= 0 && doneFlag ) {
            status = RingIO_openByAddr (sharedAddr,
                                        params,
                                        handlePtr);

            /* Copy name to opened object */
            if (status >= 0) {
                ((RingIO_Object *)(*handlePtr))->name =
                                 (String) Memory_alloc (NULL,
                                                        (strlen(name) + 1),
                                                        0u,
                                                        NULL);
                strcpy(((RingIO_Object *)(*handlePtr))->name, name);
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
    GT_1trace (curTrace, GT_LEAVE, "RingIO_open", status);

    return status;
}

/*!
 *  =========== RingIO_sharedMemReq ====================
 *  Get shared memory requirements
 */
UInt32
RingIO_sharedMemReq (const Void *               params,
                           RingIO_sharedMemReqDetails  * sharedMemReqDetails)
{
    UInt32 totalSize = 0;

    GT_1trace (curTrace, GT_ENTER, "RingIO_sharedMemReq", params);

    GT_assert(curTrace,(NULL != params));
    GT_assert(curTrace,(NULL != sharedMemReqDetails));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_sharedMemReq",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_sharedMemReq",
                             RingIO_E_INVALIDARG,
                             "Structure of type params passed is NULL!");
    }
    else if (sharedMemReqDetails == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_sharedMemReq",
                             RingIO_E_INVALIDARG,
                             "Structure of type sharedMemReqDetails passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        totalSize = IRingIO_sharedMemReq (params, sharedMemReqDetails);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace,
               GT_LEAVE,
               "RingIO_sharedMemReq",
               totalSize);

    return (totalSize);
}

/*
 *  ======== RingIO_Params_init ========
 *  Initialize parameter structure with default values.
 */
Void RingIO_Params_init(Void * params)
{
    RingIO_Params * sparams = (RingIO_Params *)params;
    GT_1trace (curTrace, GT_ENTER, "RingIO_Params_init", params);

    GT_assert (curTrace, (params != NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_Params_init",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (sparams == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_Params_init",
                             RingIO_E_INVALIDARG,
                             "Structure of type params passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        memcpy (sparams,
                &(RingIO_module->defaultInstParams),
                sizeof (RingIO_Params));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */


    GT_0trace (curTrace, GT_LEAVE, "RingIO_Params_init");
}

/*!
 *  ======== RingIO_getCliNotifyMgrShAddr ========
 *  Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *  instance used for this instance.
 */
Ptr RingIO_getCliNotifyMgrShAddr (RingIO_Handle handle)
{

    Ptr shAddr = NULL;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getCliNotifyMgrShAddr",
               handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getCliNotifyMgrShAddr",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getCliNotifyMgrShAddr",
                             RingIO_E_INVALIDARG,
                             "Structure of handle passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        shAddr = IRingIO_getCliNotifyMgrShAddr ((IRingIO_Handle)handle);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getCliNotifyMgrShAddr", shAddr);

    return (shAddr);
}

/*!
 *  ======== RingIO_getCliNotifyMgrGateShAddr ========
 *  Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *  instance used for this instance.
 */
Ptr RingIO_getCliNotifyMgrGateShAddr (RingIO_Handle handle)
{
    Ptr shAddr = NULL;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getCliNotifyMgrGateShAddr",
               handle);

    GT_assert (curTrace, (handle != NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getCliNotifyMgrGateShAddr",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getCliNotifyMgrGateShAddr",
                             RingIO_E_INVALIDARG,
                             "Structure of handle passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        shAddr = IRingIO_getCliNotifyMgrGateShAddr ((IRingIO_Handle)handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "RingIO_getCliNotifyMgrGateShAddr", shAddr);

    return (shAddr);
}

/*!
 *  =========== RingIO_setNotifyId ====================
 *  Sets the notification id
 */
Int
RingIO_setNotifyId (RingIO_Handle        handle,
                    UInt32               notifyId,
                    RingIO_NotifyType    notifyType,
                    UInt32               watermark)
{
    Int  status  = RingIO_S_SUCCESS;

    GT_4trace (curTrace,
               GT_ENTER,
              "RingIO_setNotifyId",
               handle,
               notifyId,
               notifyType,
               watermark);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setNotifyId",
                             RingIO_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_setNotifyIdv",
                             RingIO_E_INVALIDARG,
                             "Structure of handle passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        status = IRingIO_setNotifyId((IRingIO_Handle)handle,
                                 notifyId,
                                 notifyType,
                                 watermark);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */


    GT_1trace (curTrace, GT_LEAVE, "RingIO_setNotifyId", status);
    return (status);
}

/*!
 *  =========== RingIO_resetNotifyId ====================
 *  Function to reset the notifyid received in userspace during call to
 *  sClientNotifyMgr_register client.
 */
Int
RingIO_resetNotifyId (RingIO_Handle handle, UInt32 notifyId)
{
    Int32                    status = RingIO_S_SUCCESS;

    GT_2trace (curTrace,
               GT_ENTER,
              "RingIO_resetNotifyId",
               handle,
               notifyId);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (RingIO_module->refCount == 0) {
        status = RingIO_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_resetNotifyId",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_resetNotifyId",
                             status,
                             "Structure of handle passed is NULL!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        status = IRingIO_resetNotifyId((IRingIO_Handle)handle,
                                       notifyId);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */


    GT_1trace (curTrace, GT_LEAVE, "RingIO_resetNotifyId", status);
    return (status);

}
