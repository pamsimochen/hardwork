/*
 *  @file   RingIOShm.c
 *
 *  @brief      RingIO Shared Memory transport
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
#include <ti/sdo/ipc/_GateMP.h>
#include <xdc/std.h>
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
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/_GateMP.h>
#endif /* #if defined(SYSLINK_BUILD_HLOS) */

#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/inc/IRingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/_RingIOShm.h>

#define CACHE_ALIGNED_SIZEOF(x,id) \
    (sizeof(x) + SharedRegion_getCacheLineSize(id) - (sizeof(x) % \
	SharedRegion_getCacheLineSize(id)))

/* =============================================================================
 * Typedefs and structures
 * =============================================================================
 */
/*!
 *  @var    RingIOShm_ModuleObject
 *
 *  @brief  RingIOShm module structure
 *
 *          If more fields are added to this structure, they need to be
 *          initialized in the RingIOShm_state structure or in
 *          RingIOShm_setup.
 */
typedef struct RingIOShm_ModuleObject {
    RingIOShm_Params       defaultInstParams;
    /*!< Default instance creation parameters */
    UInt32                 refCount;
    /*!< refcount for RingIOShm module */
    Bool                   isSetup;
    /*!< isSetup flag to indicate the state of module */
} RingIOShm_ModuleObject ;

/*!
 *  @var    RingIOShm_state
 *
 *  @brief  RingIOShm_state data structure
 */

#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
RingIOShm_ModuleObject RingIOShm_state =
{
    .refCount          = 0,
    .isSetup           = FALSE,
    .defaultInstParams.ctrlRegionId       = 0,
    .defaultInstParams.dataRegionId       = 0,
    .defaultInstParams.attrRegionId       = 0,
    .defaultInstParams.ctrlSharedAddr     = NULL,
    .defaultInstParams.dataSharedAddr     = NULL,
    .defaultInstParams.dataSharedAddrSize = 0,
    .defaultInstParams.footBufSize        = 0,
    .defaultInstParams.attrSharedAddr     = NULL,
    .defaultInstParams.attrSharedAddrSize = 0,
    .defaultInstParams.gateHandle         = NULL,
    .defaultInstParams.localProtect       = GateMP_LocalProtect_NONE,
    .defaultInstParams.remoteProtect      = GateMP_RemoteProtect_NONE,
    .defaultInstParams.remoteProcId       = 0
};


/*!
 *  @var    RingIO_moduleCfg
 *
 *  @brief  RingIO module configuration. This will be referred to by all
 *          delegates in order to get access to RingIO module level
 *          configuration information.
 */
RingIOShm_Config RingIOShm_moduleCfg;
extern RingIOShm_Config Syslink_RingIOShm_moduleCfg;


/*!
 *  @var    RingIOShm_module
 *
 *  @brief  Pointer to the RingIO module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
RingIOShm_ModuleObject * RingIOShm_module = &RingIOShm_state;


/* =============================================================================
 * Function Prototype
 * =============================================================================
 */
/* Sends a notification to client registered with given event Id */
Int
_RingIOShm_sendNotification (ClientNotifyMgr_Handle handle,
                            UInt32                 slot,
                            UInt32                 subId,
                            UInt32                 arg1,
                            UInt32                 arg2);
static
RingIO_Handle _RingIOShm_create(RingIO_Handle handle);

/* =============================================================================
 * APIs
 * =============================================================================
 */

/*
 *  ============= RingIOShm_delete ========
 *  Deletes an instance of RingIO module.
 */
Int RingIOShm_delete (RingIO_Handle * handlePtr)
{
    Int                   status = RingIOShm_S_SUCCESS;
    RingIO_Object       * handle = NULL;
    RingIOShm_Obj       * object = NULL;
    IArg                  key;
    UInt32                ctrlSharedAddrSize;
    SharedRegion_SRPtr    srPtrBufStart;
    SharedRegion_SRPtr    srPtrAttrBufStart;

    GT_assert(curTrace, (handlePtr  != NULL));
    GT_assert(curTrace, (*handlePtr != NULL));

    handle = (RingIO_Object *) (* handlePtr);
    object = (RingIOShm_Obj *)(handle->obj);
    GT_assert(curTrace, (object != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    srPtrBufStart = object->control->srPtrBufStart;
    srPtrAttrBufStart = object->control->srPtrAttrBufStart;

    if ((object != NULL) && (object->control != NULL)) {

        if (object->dataBufIntAllocated == TRUE) {
            Memory_free (SharedRegion_getHeap(SharedRegion_getId
                (SharedRegion_getPtr(srPtrBufStart))),
                SharedRegion_getPtr(srPtrBufStart),
                ROUND_UP(object->control->dataSharedAddrSize,
                SharedRegion_getCacheLineSize(SharedRegion_getId(
                SharedRegion_getPtr(srPtrBufStart)))));
        }

        if (object->attrBufIntAllocated == TRUE) {
            Memory_free (SharedRegion_getHeap(SharedRegion_getId
                (SharedRegion_getPtr(srPtrAttrBufStart))),
                SharedRegion_getPtr(srPtrAttrBufStart),
                ROUND_UP(object->control->srPtrAttrBufEnd + 1,
                SharedRegion_getCacheLineSize(SharedRegion_getId(
                SharedRegion_getPtr(srPtrAttrBufStart)))));
        }
        if (object->controlIntAllocated == TRUE) {
            ctrlSharedAddrSize = RingIOShm_sharedMemReq (NULL, NULL);
            Memory_free (SharedRegion_getHeap(SharedRegion_getId(object->ctrlSharedAddr)),
                         object->ctrlSharedAddr,
                         ctrlSharedAddrSize);
        }

        if (SharedRegion_isCacheEnabled(SharedRegion_getId(object->ctrlSharedAddr))) {
            Cache_inv((Ptr)object->control,
                      sizeof (RingIOShm_ControlStruct),
                      Cache_Type_ALL,
                      TRUE);
        }

        if (object->clientNotifyMgrHandle != NULL) {
            ClientNotifyMgr_delete (&(object->clientNotifyMgrHandle));
        }

        if (SharedRegion_isCacheEnabled(SharedRegion_getId(object->ctrlSharedAddr))) {
            Cache_wbInv((Ptr)object->control,
                        sizeof(RingIOShm_ControlStruct),
                        Cache_Type_ALL,
                        TRUE);
        }
    }
   GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    if (object != NULL) {
        if (object->name != NULL) {
            Memory_free(NULL, object->name, strlen(object->name) + 1);
        }

        Memory_free(NULL,
                    object,
                    sizeof (RingIOShm_Obj));
    }


   return status;
}


/*
 *  ============= RingIOShm_openByAddr ========
 *  Function to open the created RingIO instance.The instance must be
 *  created before opening it.
 */
Int RingIOShm_openByAddr (      Ptr                    ctrlSharedAddr,
                          const RingIO_openParams *    params,
                                RingIO_Handle  *       handlePtr)
{
    Int                         status    = RingIOShm_S_SUCCESS;
    Int                         tmpStatus = RingIOShm_S_SUCCESS;
    RingIOShm_Obj *             object    = NULL;
    volatile RingIOShm_Client * client    = NULL;
    ClientNotifyMgr_Params      clientMgrParams;
    Ptr                         sharedAddr;
    GateMP_Handle               gateHandle;
    IArg                        key;
    UInt16                      ctrlRegionId;

    GT_assert(curTrace, (handlePtr  != NULL));
    GT_assert(curTrace, (*handlePtr != NULL));
    GT_assert(curTrace, (params != NULL));

    _RingIOShm_create(*handlePtr);

    object = (RingIOShm_Obj * ) Memory_calloc (NULL,
                                               sizeof (RingIOShm_Obj),
                                               0u,
                                               NULL);

    if (object == NULL){
        status = RingIO_E_MEMORY;
        GT_setFailureReason (
                           curTrace,
                           GT_4CLASS,
                           "RingIOShm_openByAddr",
                           status,
                           "Failed to allocate RingIOShm object!");
    }
    else {
        ((RingIO_Object *)(*handlePtr))->obj = (Ptr)object;
        ((RingIO_Object *)(*handlePtr))->top = (Ptr)*handlePtr;
        object->flags    = params->flags;
        object->openMode = params->openMode;

        object->ctrlSharedAddr = object->control  = (Ptr) ((RingIOShm_ControlStruct *)ctrlSharedAddr);
        ((RingIO_Object *)(*handlePtr))->ctrlSharedAddr =  (UInt32)((RingIOShm_ControlStruct *)ctrlSharedAddr);
        ctrlRegionId = SharedRegion_getId(object->ctrlSharedAddr);
        if (SharedRegion_isCacheEnabled (ctrlRegionId)) {
            object->controlCacheUse = TRUE;
            Cache_inv((Ptr)object->control,
                      sizeof(RingIOShm_ControlStruct),
                      Cache_Type_ALL,
                      TRUE);
        }
        object->writer  = (Ptr) ((UInt32)object->control + CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, ctrlRegionId));
        object->reader  = (Ptr) ((UInt32)object->writer + CACHE_ALIGNED_SIZEOF(RingIOShm_Client, ctrlRegionId));

        if(object->controlCacheUse == TRUE) {
            Cache_inv((Ptr)object->writer,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
            Cache_inv((Ptr)object->reader,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);

        }

        if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
        {
            client = object->writer;
        }
        else if (object->openMode   == (UInt32) RingIO_MODE_READER)
        {
            client = object->reader;
        }
        GT_assert(curTrace, (client != NULL));

        if (   (object->openMode       == (UInt32) RingIO_MODE_WRITER)
            && (client->isValid        == TRUE))
        {
            status = RingIOShm_E_FAIL;
            GT_setFailureReason (
                           curTrace,
                           GT_4CLASS,
                           "RingIOShm_openByAddr",
                           status,
                           "Cannot open the same RingIO instance in writer mode more than once!");
        }
        else if (   (object->openMode   == (UInt32) RingIO_MODE_READER)
                 && (client->isValid    == TRUE))
        {
            status = RingIOShm_E_FAIL;
            GT_setFailureReason (
                           curTrace,
                           GT_4CLASS,
                           "RingIOShm_openByAddr",
                           status,
                           "Cannot open the same RingIO instance in reader mode more than once!");
        }
        else
        {
            if (status >= 0) {
                ClientNotifyMgr_Params_init (&clientMgrParams);
                clientMgrParams.regionId =
                                 SharedRegion_getId(object->control);
                /* NotifyEntries should be two as there are one reader and one
                 * writer that  need notification
                 */
                clientMgrParams.numNotifyEntries = RingIO_NOTIFYENTRIES;

                /* Set it for only one sub notify entry. as other client is only
                 * one and only waits on single ringIO data buffer.
                 */
                clientMgrParams.numSubNotifyEntries = RingIO_SUBNOTIFYENTRIES;
                sharedAddr = SharedRegion_getPtr(object->control->gateMPSharedAddr);
                tmpStatus = GateMP_openByAddr (sharedAddr,
                                               &gateHandle);

                if ((tmpStatus < 0) || gateHandle == NULL) {
                    status = tmpStatus;
                }
                else {
                    if (object->clientNotifyMgrGate == NULL) {
                        object->clientNotifyMgrGate = gateHandle;
                    }
                    client->clientNotifyMgrGate = gateHandle;
                    /*
                     * Pass the shared memory address to create the client
                     * manager instance. This address stars right after the
                     * size of (RingIO Control Structure) from the
                     * control shared address.
                     */
                    clientMgrParams.sharedAddr = (Ptr) (  (UInt32)object->control
					    + CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, ctrlRegionId)
						+ 2 * CACHE_ALIGNED_SIZEOF(RingIOShm_Client, ctrlRegionId));
                    clientMgrParams.sharedAddrSize = ClientNotifyMgr_sharedMemReq (&clientMgrParams);
                    clientMgrParams.gate          = client->clientNotifyMgrGate;

                    /* Pass the same name for ClientNotifyMgr */
                    if (((RingIO_Object *)(*handlePtr))->name != NULL)  {
                       clientMgrParams.name    = ((RingIO_Object *)(*handlePtr))->name ;
                    }
                    clientMgrParams.openFlag = TRUE;
                    if (object->controlCacheUse == TRUE) {
                        clientMgrParams.cacheFlags = ClientNotifyMgr_CONTROL_CACHEUSE;
                    }

                    client->clientNotifyMgrHandle = ClientNotifyMgr_create (&clientMgrParams);

                    if (object->controlCacheUse == TRUE) {
                        Cache_wbInv((Ptr)client,
                                sizeof(RingIOShm_Client),
                                Cache_Type_ALL,
                                TRUE);
                    }

                    if (NULL == client->clientNotifyMgrHandle) {
                        status = RingIOShm_E_FAIL;
                    }
                }
            }

            if (status >= 0) {
                /* Use the gate retrieved from gateMPSharedAddr */
                object->gateHandle = gateHandle;
                key = GateMP_enter ((GateMP_Handle)object->gateHandle);

                client->pDataStart = SharedRegion_getPtr ((SharedRegion_SRPtr)object->control->srPtrBufStart);
                client->pAttrStart = SharedRegion_getPtr ((SharedRegion_SRPtr)object->control->srPtrAttrBufStart);

                if (    SharedRegion_isCacheEnabled (SharedRegion_getId (client->pDataStart))
                    && ((object->flags & RingIO_DATABUF_MAINTAINCACHE) != 0)) {
                    object->dataBufCacheUse = TRUE;
                }

                if (SharedRegion_isCacheEnabled (SharedRegion_getId (client->pAttrStart))) {
                    object->attrBufCacheUse = TRUE;
                }

                if (client->refCount == 0) {
                    client->acqStart          = 0;
                    client->acqSize           = 0;

                    client->acqAttrStart      = 0;
                    client->acqAttrSize       = 0;
                }

                client->notifyParam       = NULL;
                client->notifyType        = RingIO_NOTIFICATION_NONE;
                client->notifyWaterMark   = 0;

                client->flags             = object->flags;
                client->refCount          = 1;

                /* TBD: Update Params in handle. This will require
                 *      1) Both openParams as well as
                 *      2) Reading of shared memory
                 */

                client->notifyId   = (UInt32) -1;
                client->isValid    = TRUE;

                if (object->controlCacheUse == TRUE) {
                    Cache_wbInv((Ptr)object->control,
                            sizeof(RingIOShm_ControlStruct),
                            Cache_Type_ALL,
                            TRUE);
                    Cache_wbInv((Ptr)client,
                            sizeof(RingIOShm_Client),
                            Cache_Type_ALL,
                            TRUE);
                }
                GateMP_leave ((GateMP_Handle)object->gateHandle, key);
            }
        }
    }


    return status;
}

/*
 *  ============= RingIOShm_close ========
 *  Function to close the dynamically opened instance
 */
Int RingIOShm_close (RingIO_Handle *handlePtr)
{
    Int                         status                = RingIOShm_S_SUCCESS;
    volatile RingIOShm_Client * client                = NULL;
    RingIO_Object             * handle                = NULL;
    RingIOShm_Obj             * object                = NULL;
    IArg                        key;

    GT_assert(curTrace, (handlePtr  != NULL));
    GT_assert(curTrace, (*handlePtr != NULL));

    handle = (RingIO_Object *) (* handlePtr);
    object = (RingIOShm_Obj *)(handle->obj);
    GT_assert(curTrace, (object != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)object->control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        client = object->writer;
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        client = object->reader;
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    if (status >= 0) {
        if (object->controlCacheUse == TRUE) {
            Cache_inv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
        }
        client->notifyParam       = NULL;
        client->notifyType        = RingIO_NOTIFICATION_NONE;
        client->notifyWaterMark   = 0;

        if (client->clientNotifyMgrHandle != NULL) {
            ClientNotifyMgr_delete ((ClientNotifyMgr_Handle *)&(client->clientNotifyMgrHandle));
        }

        if (object->clientNotifyMgrGate != NULL) {
            GateMP_close ((GateMP_Handle*)&(object->clientNotifyMgrGate));
        }
        client->isValid = FALSE;

        if (object->controlCacheUse == TRUE) {
            Cache_wbInv((Ptr)object->control,
                        sizeof(RingIOShm_ControlStruct),
                        Cache_Type_ALL,
                        TRUE);
            Cache_wbInv((Ptr)client,
                        sizeof(RingIOShm_Client),
                        Cache_Type_ALL,
                        TRUE);
        }
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    if (object != NULL) {
        Memory_free(NULL,
                    object,
                    sizeof (RingIOShm_Obj));
    }

    return status;
}

/*
 *  ============= RingIOShm_registerNotifier ========
 *  Function to un-register a call back function.
 */
Int
RingIOShm_registerNotifier (RingIO_Handle      handle,
                            RingIO_NotifyType  notifyType ,
                            UInt32             watermark,
                            RingIO_NotifyFxn   notifyFunc,
                            Ptr                cbContext)
{
    Int                                    status   = RingIOShm_S_SUCCESS;
    ClientNotifyMgr_Handle                 clientNotifyMgrHandle = NULL;
    RingIOShm_Obj                        * object                = NULL;
    ClientNotifyMgr_registerNoitifyParams  regParams;
    volatile RingIOShm_Client         *    client;
    UInt32                                 notifyId;
    IArg                      key;
    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    regParams.clientHandle   = (RingIO_Handle) handle;
    regParams.procId         = MultiProc_self();
    regParams.notifyType     = notifyType;
    regParams.fxnPtr         = (ClientNotifyMgr_FnCbck) notifyFunc;
    regParams.cbContext      = cbContext;
    regParams.watermarkCond1 = watermark;
    regParams.watermarkCond2 = (UInt32) -1;

    /* TBD: Protection */
    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    if (status >= 0) {

        if (object->controlCacheUse == TRUE) {
            key = GateMP_enter ((GateMP_Handle)object->gateHandle);
            Cache_inv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
            GateMP_leave((GateMP_Handle)object->gateHandle,key);
        }

        clientNotifyMgrHandle = client->clientNotifyMgrHandle;

        if (client->isRegistered != TRUE) {
            status = ClientNotifyMgr_registerClient(clientNotifyMgrHandle,
                    &regParams, &notifyId);
        }
        else {
            status = RingIOShm_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIOShm_registerNotifier",
                                 status,
                                 "A notifier function had already been registered.");
		}

        key = GateMP_enter ((GateMP_Handle)object->gateHandle);
        if (status >= 0) {
            client->isRegistered = TRUE;
            client->notifyId     = notifyId;
            client->notifyType   = notifyType;
            client->notifyWaterMark     = watermark;

            if (   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                || (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ALWAYS)) {

                ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                                    client->notifyId,
                                                    0);
            }
        }


        if (object->controlCacheUse == TRUE) {
            Cache_wbInv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
        }
        GateMP_leave((GateMP_Handle)object->gateHandle,key);
    }


    return (status);
}

/*
 *  ========== RingIOShm_unregisterNotifier ===========
 *  Function to un-register call back function.
 */
Int
RingIOShm_unregisterNotifier (RingIO_Handle      handle)
{
    Int                         status                = RingIOShm_S_SUCCESS;
    ClientNotifyMgr_Handle      clientNotifyMgrHandle = NULL;
    volatile RingIOShm_Client * client                = NULL;
    RingIOShm_Obj             * object                = NULL;


    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    if (status >= 0) {
        if (object->controlCacheUse == TRUE) {
            Cache_inv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
        }

        clientNotifyMgrHandle = client->clientNotifyMgrHandle;
        status = ClientNotifyMgr_unregisterClient (clientNotifyMgrHandle,
                                                   client->notifyId);
        if (status >= 0) {
            client->isRegistered = FALSE;
            client->notifyId     = (UInt32)-1;
            client->notifyType   = RingIO_NOTIFICATION_NONE;
            client->notifyWaterMark     = (UInt32) -1;

            if (   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                || (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ALWAYS)) {

                ClientNotifyMgr_disableNotification (client->clientNotifyMgrHandle,
                                                     client->notifyId,
                                                     0);
            }
        }

        if (object->controlCacheUse == TRUE) {
            Cache_wbInv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
        }
    }

    return (status);
}

/*
 *  ============== RingIOShm_setWatermark =========
 *  Function to set client's watermark
 */
Int
RingIOShm_setWaterMark (RingIO_Handle handle,
                        UInt32        watermark)
{
    Int                          status = RingIOShm_S_SUCCESS;
    volatile RingIOShm_Client * client  = NULL;
    RingIOShm_Obj             * object  = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }
    GT_assert(curTrace, (client  != NULL));
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                sizeof(RingIOShm_Client),
                Cache_Type_ALL,
                TRUE);
    }

    client->notifyWaterMark = watermark;

    return (status);
}

/*
 *  ============= RingIOShm_setNotifyType ==================
 *  Function to set notification type.
 */
Int
RingIOShm_setNotifyType (RingIO_Handle      handle,
                         RingIO_NotifyType  notifyType)
{
    Int                   status = RingIOShm_S_SUCCESS;
    volatile RingIOShm_Client  * client = NULL;
    IArg                      key;
    RingIOShm_Obj             * object  = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    if (status >= 0) {
        key = GateMP_enter ((GateMP_Handle)object->gateHandle);

        if (object->controlCacheUse == TRUE) {
            Cache_inv((Ptr)object->control,
                      sizeof(RingIOShm_ControlStruct),
                      Cache_Type_ALL,
                      TRUE);
            Cache_inv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
        }

        client->notifyType = notifyType;

        if (object->controlCacheUse == TRUE) {
            Cache_wbInv((Ptr)client,
                        sizeof(RingIOShm_Client),
                        Cache_Type_ALL,
                        TRUE);
        }

        GateMP_leave ((GateMP_Handle)object->gateHandle, key);
    }

    return (status);
}

/*
 *  ============= RingIOShm_acquire ========
 *  Acquire a buffer from the RingIOShm instance
 */
Int
RingIOShm_acquire (RingIO_Handle   handle,
                   RingIO_BufPtr * pData,
                   UInt32 *        pSize)
{
    Int                status = RingIOShm_S_SUCCESS;
    RingIOShm_Obj * object = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->openMode == (UInt32) RingIO_MODE_WRITER)
    {
        status = _RingIOShm_writerAcquire (object, pData, pSize);
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        status = _RingIOShm_readerAcquire (object, pData, pSize);
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    return status;
}

/*
 *  ============= RingIOShm_release ========
 *  Release a buffer to the RingIO instance
 */
Int
RingIOShm_release (RingIO_Handle  handle,
                   UInt32          size)
{
    Int                  status    = RingIOShm_S_SUCCESS;
    ClientNotifyMgr_Handle clientNotifyMgrHandle = NULL;
    UInt32                 slot                     = (UInt32) -1;
    UInt32                 arg1                     = (UInt32) -1;
    RingIOShm_Obj *        object                   = NULL;
    Bool                   isClientNotifyRegistered = FALSE;
    IArg                   key;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        key = GateMP_enter ((GateMP_Handle)object->gateHandle);
        status = _RingIOShm_writerRelease (object, size);

        clientNotifyMgrHandle  = object->writer->clientNotifyMgrHandle;

        if(object->reader->isRegistered == TRUE){
            isClientNotifyRegistered = TRUE;
            slot = object->reader->notifyId;
        }
        arg1 = object->control->validSize;
        GateMP_leave ((GateMP_Handle)object->gateHandle, key);
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        key = GateMP_enter ((GateMP_Handle)object->gateHandle);
        status = _RingIOShm_readerRelease (object,
                                                    size);
        clientNotifyMgrHandle  = object->reader->clientNotifyMgrHandle;
        if(object->writer->isRegistered == TRUE){
            isClientNotifyRegistered = TRUE;
            slot = object->writer->notifyId;
        }
        arg1 = object->control->emptySize;
        GateMP_leave ((GateMP_Handle)object->gateHandle, key);
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    if (status >= 0) {
        if(isClientNotifyRegistered){
            status = _RingIOShm_sendNotification (clientNotifyMgrHandle,
                                                 slot,
                                                 0,
                                                 arg1,
                                                 (UInt32)-1 );
        }
    }

    return status;
}

/*
 *  ============= RingIOShm_cancel ========
 *  Cancel the previous acquired buffer to the RingIO instance
 */
Int
RingIOShm_cancel (RingIO_Handle  handle)
{
    Int                  status    = RingIOShm_S_SUCCESS;
    RingIOShm_Obj * object = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        status = _RingIOShm_writerCancel (object);
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        status = _RingIOShm_readerCancel (object);
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    return status;
}

/*
 *  ============= RingIOShm_getvAttribute ========
 *  Get a variable sized attribute
 */
Int
RingIOShm_getvAttribute (RingIO_Handle  handle,
                         UInt16 *        type,
                         UInt32 *        param,
                         RingIO_BufPtr   vptr,
                         UInt32 *        pSize)
{
    Int                  status    = RingIOShm_S_SUCCESS;
    RingIOShm_Obj * object = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        status = RingIOShm_E_INVALIDSTATE;
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        status = _RingIOShm_readergetvAttribute (object,
                                                 type,
                                                 param,
                                                 vptr,
                                                 pSize);
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }
    return status;
}

/*
 *  ============= RingIOShm_setvAttribute ========
 *  This function sets an attribute with a variable sized payload at the
 *  offset 0 provided in the acquired data buffer.
 */
Int
RingIOShm_setvAttribute (RingIO_Handle  handle,
                         UInt16          type,
                         UInt32          param,
                         RingIO_BufPtr   pdata,
                         UInt32          size,
                         Bool            sendNotification)
{
    Int                  status    = RingIOShm_S_SUCCESS;
    RingIOShm_Obj * object = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));


    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        status = _RingIOShm_writersetvAttribute (handle,
                                                type,
                                                param,
                                                pdata,
                                                size,
                                                sendNotification);
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        status = RingIOShm_E_INVALIDSTATE;
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    return status;
}

/*!
 *  ============ RingIOShm_getAttribute ===============
 *  Get attribute from the RingIOShm instance
 */
Int
RingIOShm_getAttribute (RingIO_Handle handle,
                        UInt16 *      type,
                        UInt32 *      param)
{
    Int  status = RingIOShm_S_SUCCESS;

    GT_assert(curTrace, (handle  != NULL));
    GT_assert(curTrace, (type    != NULL));
    GT_assert(curTrace, (param   != NULL));

    status = RingIOShm_getvAttribute (handle,
                                      type,
                                      param,
                                      NULL,
                                      NULL);
    return status;
}


/*!
 *  ============ RingIOShm_setAttribute ===============
 *  Set attribute in the RingIOShm instance
 */
Int
RingIOShm_setAttribute (RingIO_Handle handle,
                        UInt16        type,
                        UInt32        param,
                        Bool          sendNotification)
{
    Int               status = RingIOShm_S_SUCCESS;

    status = RingIOShm_setvAttribute (handle,
                                      type,
                                      param,
                                      NULL,
                                      0,
                                      sendNotification);

    return status;
}

/*
 *  ============= RingIOShm_flush ========
 *  This function flushes the data buffer
 */
Int
RingIOShm_flush (RingIO_Handle  handle,
                 Bool           hardFlush,
                 UInt16 *       type,
                 UInt32 *       param,
                 UInt32 *       bytesFlushed)
{
    Int                    status                   = RingIOShm_S_SUCCESS;
    ClientNotifyMgr_Handle clientNotifyMgrHandle    = NULL;
    UInt32                 slot                     = (UInt32) -1;
    UInt32                 arg1                     = (UInt32) -1;
    RingIOShm_Obj *        object                   = NULL;
    Bool                   isClientNotifyRegistered = FALSE;
    IArg                   key;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        key = GateMP_enter ((GateMP_Handle)object->gateHandle);
        status = _RingIOShm_writerFlush (handle,
                                         hardFlush,
                                         type,
                                         param,
                                         bytesFlushed);
        clientNotifyMgrHandle  = object->writer->clientNotifyMgrHandle;
        if(object->reader->isRegistered == TRUE){
            isClientNotifyRegistered = TRUE;
            slot = object->reader->notifyId;
        }
        arg1 = object->control->validSize;
        GateMP_leave ((GateMP_Handle)object->gateHandle, key);
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        key = GateMP_enter ((GateMP_Handle)object->gateHandle);
        status = _RingIOShm_readerFlush (handle,
                                         hardFlush,
                                         type,
                                         param,
                                         bytesFlushed);
        clientNotifyMgrHandle  = object->reader->clientNotifyMgrHandle;
        if(object->writer->isRegistered == TRUE){
            isClientNotifyRegistered = TRUE;
            slot = object->writer->notifyId;
        }
        arg1 = object->control->emptySize;
        GateMP_leave ((GateMP_Handle)object->gateHandle, key);
    }
    else {
        status = RingIOShm_E_INVALIDSTATE;
    }

    if (status >= 0) {
        if(isClientNotifyRegistered){
            status = _RingIOShm_sendNotification (clientNotifyMgrHandle,
                                             slot,
                                             0,
                                             arg1,
                                             (UInt32)-1 );
        }
    }

    return status;
}

/*
 *  ============= RingIOShm_sendNotify ========
 *  Send a notification to the other client manually
 */
Int
RingIOShm_sendNotify (RingIO_Handle       handle,
                  RingIO_NotifyMsg    msg)
{
    Int                             status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_Client  * client  = NULL;
    volatile RingIOShm_Client  * otherClient  = NULL;
    ClientNotifyMgr_Handle          clientNotifyMgrHandle = NULL;
    RingIOShm_Obj *        object    = NULL;
    IArg                   key;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->openMode       == (UInt32) RingIO_MODE_WRITER)
    {
        client      = object->writer;
        otherClient = object->reader;
    }
    else if (object->openMode   == (UInt32) RingIO_MODE_READER)
    {
        client      = object->reader;
        otherClient = object->writer;
    }

    GT_assert(curTrace, (client != NULL));
    GT_assert(curTrace, (otherClient != NULL));

    if (object->controlCacheUse == TRUE) {
        key = GateMP_enter ((GateMP_Handle)object->gateHandle);
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
        Cache_inv((Ptr)otherClient,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
        GateMP_leave ((GateMP_Handle)object->gateHandle, key);
    }

    clientNotifyMgrHandle =  client->clientNotifyMgrHandle;
    if(otherClient->isRegistered == TRUE){
        status = ClientNotifyMgr_sendForceNotification ((ClientNotifyMgr_Object *)clientNotifyMgrHandle,
                                                        otherClient->notifyId,
                                                        msg);
    if (status >= 0) {
        status = RingIOShm_S_SUCCESS;
        }
        else {
            status = RingIO_E_NOTIFYFAIL;
        }
    }
    else {
        status = RingIO_E_NOTIFYFAIL;
    }

    return status;
}

/*
 *  ============= RingIOShm_getValidSize ==================
 *  Function to get valid size.
 */
UInt32
RingIOShm_getValidSize (RingIO_Handle  handle)
{
    IArg            key;
    UInt32          validSize = (UInt32)-1;
    RingIOShm_Obj * object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)object->control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    validSize = object->control->validSize;

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return (validSize);
}

/*
 *  =========== RingIOShm_getEmptySize ==========
 *  Function to get empty size.
 */
UInt32
RingIOShm_getEmptySize (RingIO_Handle  handle)
{
    IArg                 key;
    UInt32                  emptySize = (UInt32) -1;
    RingIOShm_Obj *        object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)object->control,
                sizeof(RingIOShm_ControlStruct),
                Cache_Type_ALL,
                TRUE);
    }

    emptySize = object->control->emptySize;

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return (emptySize);
}

/*
 *  ============= RingIOShm_getValidAttrSize ==========
 *  Function to get valid attribute size.
 */
UInt32
RingIOShm_getValidAttrSize (RingIO_Handle  handle)
{
    IArg                    key;
    UInt32                  validAttrSize = (UInt32)-1;

    RingIOShm_Obj *        object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)object->control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    validAttrSize = object->control->validAttrSize;

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return (validAttrSize);
}

/*
 *  ============= RingIOShm_getEmptyAttrSize ==========
 *  Function to get empty attribute size.
 */
UInt32
RingIOShm_getEmptyAttrSize (RingIO_Handle  handle)
{
    IArg                    key;
    UInt32                  emptyAttrSize = (UInt32)-1;
    RingIOShm_Obj *         object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)object->control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    emptyAttrSize = object->control->emptyAttrSize;

    GateMP_leave ((GateMP_Handle)(GateMP_Handle)object->gateHandle, key);

    return (emptyAttrSize);
}

/*
 *  =============== RingIOShm_getAcquiredOffset =========
 *  Function to get client's acquired offset
 */
UInt32
RingIOShm_getAcquiredOffset (RingIO_Handle  handle)
{
    volatile RingIOShm_Client         * client = NULL;
    UInt32                              acquiredOffset = (UInt32)-1;
    RingIOShm_Obj *                     object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert(curTrace, (client != NULL));

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
    }

    acquiredOffset = client->acqStart;

    return (acquiredOffset);
}

/*
 *  ================= RingIOShm_getAcquiredSize ========
 *  Function to get client's acquired size
 */
UInt32
RingIOShm_getAcquiredSize (RingIO_Handle  handle)
{
    volatile RingIOShm_Client         * client = NULL;
    UInt32                                 acquiredSize = (UInt32) -1;
    RingIOShm_Obj *        object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert(curTrace, (client != NULL));

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
    }

    acquiredSize = client->acqSize;

    return (acquiredSize);
}

/*
 *  ============== RingIOShm_getWatermark =========
 *  Function to get client's watermark
 */
UInt32
RingIOShm_getWaterMark (RingIO_Handle  handle)
{
    volatile RingIOShm_Client *  client    = NULL;
    UInt32                       watermark = (UInt32)-1;
    RingIOShm_Obj *              object    = NULL;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert (curTrace, (client != NULL));;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                sizeof(RingIOShm_Client),
                Cache_Type_ALL,
                TRUE);
    }

    watermark = client->notifyWaterMark;

    return (watermark);
}

/* =============================================================================
 * API's
 * =============================================================================
 */
/*!
 *  =========== RingIOShm_sharedMemReq ====================
 *  Get shared memory requirements
 */
/* RingIOShm_sharedMemReq function needs to be fixed to use getcacheline size
 * and remove padding completely
 */
UInt32
RingIOShm_sharedMemReq (const Void *                           params,
                              RingIO_sharedMemReqDetails  *    sharedMemReqDetails)
{
    UInt                          totalSize = 0;
    UInt32                        dataBufSize = 0;
    RingIOShm_Params       *      rshmParams = (RingIOShm_Params * )params;
    ClientNotifyMgr_Params        clientMgrParams;
    SizeT                         minAlign;
    UInt16                        regionId;

    /* The shared memory requirements
     *  a) 1 cache line for Control structure
     *  b) 1 cache line for Writer client
     *  c) 1 cache line for Reader client
     *  d) Client Notify
     *  e) Data Buffer
     *  f) Attribute Buffer
     */

    ClientNotifyMgr_Params_init(&clientMgrParams);
    minAlign = Memory_getMaxDefaultTypeAlign();

// TDB : Commented in vob
//   if (RingIOShm_useDefaultGate == FALSE) {
//         if (rshmParams != NULL) {
//             gateParams.remoteProtect = rshmParams->remoteProtect;
//             gateParams.localProtect  = rshmParams->localProtect;
//         }
//         /* Gate will be used for RingIO logic */
//         totalSize += ROUND_UP((UInt32)GateMP_sharedMemReq (&gateParams),
//                               minAlign);
//     }
    ClientNotifyMgr_Params_init(&clientMgrParams);
    clientMgrParams.regionId = 0 ;
    clientMgrParams.numNotifyEntries = RingIO_NOTIFYENTRIES;
    clientMgrParams.numSubNotifyEntries = RingIO_SUBNOTIFYENTRIES;

    if (rshmParams == NULL) {
        totalSize += CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, clientMgrParams.regionId)
			+ 2 * CACHE_ALIGNED_SIZEOF(RingIOShm_Client, clientMgrParams.regionId);
        totalSize += ROUND_UP((UInt32)ClientNotifyMgr_sharedMemReq (
                                   (ClientNotifyMgr_Params*)&clientMgrParams ),
                                    minAlign);

        if (sharedMemReqDetails != NULL) {
            ((RingIO_sharedMemReqDetails  *)sharedMemReqDetails)->ctrlSharedMemReq = totalSize;
            ((RingIO_sharedMemReqDetails  *)sharedMemReqDetails)->attrSharedMemReq = 0;
            ((RingIO_sharedMemReqDetails  *)sharedMemReqDetails)->dataSharedMemReq = 0;
        }
    }
    else {

        if (rshmParams->ctrlSharedAddr) {
            regionId = SharedRegion_getId (rshmParams->ctrlSharedAddr);
        }
        else {
            regionId = rshmParams->ctrlRegionId;
        }
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize(regionId);
        }

        clientMgrParams.regionId = regionId ;
        totalSize += CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, regionId)
			+ 2 * CACHE_ALIGNED_SIZEOF(RingIOShm_Client, regionId);
        totalSize += ROUND_UP((UInt32)ClientNotifyMgr_sharedMemReq (
           (ClientNotifyMgr_Params*)&clientMgrParams ),
           minAlign);

        if (((RingIO_sharedMemReqDetails  *)sharedMemReqDetails) != NULL) {
            ((RingIO_sharedMemReqDetails  *)sharedMemReqDetails)->ctrlSharedMemReq = totalSize;
        }

        if (rshmParams->dataSharedAddr) {
            regionId = SharedRegion_getId (rshmParams->dataSharedAddr);
        }
        else {
            regionId = rshmParams->dataRegionId;
        }
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize(regionId);
        }

        dataBufSize = rshmParams->dataSharedAddrSize + rshmParams->footBufSize;
        totalSize += (ROUND_UP(dataBufSize, minAlign));
        if (((RingIO_sharedMemReqDetails  *)sharedMemReqDetails) != NULL) {
            ((RingIO_sharedMemReqDetails  *)sharedMemReqDetails)->dataSharedMemReq = (ROUND_UP(dataBufSize, minAlign));
        }


        if (rshmParams->attrSharedAddr) {
            regionId = SharedRegion_getId(rshmParams->attrSharedAddr);
        }
        else {
            regionId = rshmParams->attrRegionId;
        }
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize(regionId);
        }

        totalSize += (ROUND_UP((UInt32)rshmParams->attrSharedAddrSize, minAlign));
        if (((RingIO_sharedMemReqDetails  *)sharedMemReqDetails) != NULL) {
            ((RingIO_sharedMemReqDetails  *)sharedMemReqDetails)->attrSharedMemReq = (ROUND_UP((UInt32)rshmParams->attrSharedAddrSize,
                                                     minAlign));
        }
    }
    return (totalSize);
}

/*
 *  ============= RingIOShm_create ==================
 *  Create a transport instance. This function waits for the remote
 *  processor to complete its transport creation. Hence it must be
 *  called only after the remote processor is running.
 */
Int
RingIOShm_create (const Void * params, RingIO_Handle * handlePtr)
{
    Int                                status              = RingIOShm_S_SUCCESS;
    RingIOShm_Obj *                    obj                 = NULL;
    volatile RingIOShm_ControlStruct * control             = NULL;
    GateMP_Handle                      clientNotifyMgrGate = NULL;
    Ptr                                dataSharedAddr      = NULL;
    Ptr                                attrSharedAddr      = NULL;
    RingIOShm_Params *                 rshmParams          =
                                                     (RingIOShm_Params *)params;
    RingIO_Handle                      handle              = (RingIO_Handle)(*handlePtr);
    UInt16                             dataRegionId        = 0;
    UInt16                             attrRegionId        = 0;
    UInt16                             ctrlRegionId        = 0;

    ClientNotifyMgr_Params             clientMgrParams;
    IArg                               key;
    UInt32                             ctrlSharedAddrSize;

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    Bool                               invalidEntry        = FALSE;
    SharedRegion_Entry                 entry;

    if (handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_create",
                             status,
                             "handle passed is NULL!");
    }
    else if (*handlePtr == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_create",
                             status,
                             "*handlePtr passed is NULL!");
    }
    else if (rshmParams == NULL) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             status,
                             "rshmParams passed is NULL!");
    }
    else if (   (rshmParams->ctrlSharedAddr == NULL)
             && (rshmParams->ctrlRegionId == SharedRegion_INVALIDREGIONID)) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             status,
                             "Please provide a valid shared region "
                             "address or id!");
    }
    else if (   (rshmParams->dataSharedAddr == NULL)
             && (rshmParams->dataRegionId == SharedRegion_INVALIDREGIONID)) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             status,
                             "Please provide a valid shared region "
                             "address or id!");
    }
    else if (   (rshmParams->attrSharedAddr == NULL)
             && (rshmParams->attrRegionId == SharedRegion_INVALIDREGIONID)) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             status,
                             "Please provide a valid shared region "
                             "address or id!");
    }
    else if (rshmParams->dataSharedAddrSize == 0) {
        status = RingIO_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_create",
                             status,
                             "Data buffer size is 0!");
    }
    else {
        /*
		 * Checking if SharedRegionId is valid. This is useful when we lift
		 * the restriction that RingIO only supports region id 0
		 */
	    if (rshmParams->ctrlRegionId != SharedRegion_INVALIDREGIONID) {
			SharedRegion_getEntry(rshmParams->ctrlRegionId, &entry);
		    if (entry.isValid == FALSE) {
			    invalidEntry = TRUE;
			}
		}
	    if (rshmParams->dataRegionId != SharedRegion_INVALIDREGIONID) {
			SharedRegion_getEntry(rshmParams->dataRegionId, &entry);
		    if (entry.isValid == FALSE) {
			    invalidEntry = TRUE;
			}
		}
	    if (rshmParams->attrRegionId != SharedRegion_INVALIDREGIONID) {
			SharedRegion_getEntry(rshmParams->attrRegionId, &entry);
		    if (entry.isValid == FALSE) {
			    invalidEntry = TRUE;
			}
		}
	    if (invalidEntry == TRUE) {
            status = RingIO_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIO_create",
                                 status,
                                 "Undefined region id passed in parameters!");
		}
		else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
	        GT_assert (curTrace, ((handlePtr != NULL) || (*handlePtr != NULL)));
	        _RingIOShm_create(handle);
	        obj = (RingIOShm_Obj * ) Memory_calloc (NULL,
	                                                sizeof (RingIOShm_Obj),
	                                                0u,
	                                                NULL);
	        GT_assert (curTrace, (obj != NULL));
	        if (obj != NULL) {
	            /* Copy the name */
	            if (rshmParams->commonParams.name == NULL) {
	                    status = RingIO_E_INVALIDARG;
	                    GT_setFailureReason (curTrace,
	                                         GT_4CLASS,
	                                         "RingIO_create",
	                                         status,
	                                         "RingIO name is NULL in create params!");
	            }
	            else {
	                ((RingIO_Object *) handle)->name = obj->name = (String) Memory_alloc (NULL,
	                                                                  strlen (rshmParams->commonParams.name) + 1u,
	                                                                   0,
	                                                                   NULL);
	                GT_assert (curTrace, (((RingIO_Object *) handle)->name != NULL));
	                if (((RingIO_Object *) handle)->name == NULL) {
	                    status = RingIO_E_MEMORY;
	                    GT_setFailureReason (curTrace,
	                                         GT_4CLASS,
	                                         "RingIO_create",
	                                         status,
	                                         "Memory_alloc for name failed!");
	                }
	                else {
	                    strncpy (((RingIO_Object *) handle)->name,
	                             rshmParams->commonParams.name,
	                             strlen (rshmParams->commonParams.name) + 1u);
	                    strncpy (obj->name,
	                             rshmParams->commonParams.name,
	                             strlen (rshmParams->commonParams.name) + 1u);

	                    if (rshmParams->gateHandle == NULL) {
	                        // TBD
	                        obj->gateHandle = GateMP_getDefaultRemote (); //TBD: (ti_sdo_ipc_GateMP_Handle) typecast
	                    }
	                    else {
	                        obj->gateHandle = rshmParams->gateHandle; //TBD: (ti_sdo_ipc_GateMP_Handle) typecast
	                    }
	                    key = GateMP_enter ((GateMP_Handle)obj->gateHandle);
	                    if (rshmParams->ctrlSharedAddr != NULL) {
	                        ctrlRegionId = (UInt16)SharedRegion_getId (rshmParams->ctrlSharedAddr);
	                        GT_assert (curTrace, (ctrlRegionId != SharedRegion_INVALIDREGIONID));
	                    }
	                    else {
                            ctrlRegionId = rshmParams->ctrlRegionId;
	                        GT_assert (curTrace, (ctrlRegionId != SharedRegion_INVALIDREGIONID));
	                        ctrlSharedAddrSize = RingIOShm_sharedMemReq (NULL, NULL);
	                        obj->ctrlSharedAddr = (Ptr)Memory_calloc(
	                                                        SharedRegion_getHeap(ctrlRegionId),
	                                                        ROUND_UP(ctrlSharedAddrSize, SharedRegion_getCacheLineSize(ctrlRegionId)),
	                                                        SharedRegion_getCacheLineSize(ctrlRegionId),
	                                                        NULL);
	                        GT_assert (curTrace, (obj->ctrlSharedAddr != NULL));
	                        obj->controlIntAllocated = TRUE;
	                    }
	                    if (rshmParams->attrSharedAddr != NULL) {
	                        attrRegionId = SharedRegion_getId (rshmParams->attrSharedAddr);
	                        GT_assert (curTrace, (attrRegionId != SharedRegion_INVALIDREGIONID));
	                    }
	                    else {
                            attrRegionId = rshmParams->attrRegionId;
	                        GT_assert (curTrace, (attrRegionId != SharedRegion_INVALIDREGIONID));
	                        attrSharedAddr = (Ptr)Memory_alloc(
	                                                        SharedRegion_getHeap(attrRegionId),
	                                                        ROUND_UP(rshmParams->attrSharedAddrSize, SharedRegion_getCacheLineSize(attrRegionId)),
	                                                        SharedRegion_getCacheLineSize(attrRegionId),
	                                                        NULL);
	                        GT_assert (curTrace, (attrSharedAddr != NULL));
	                        obj->attrBufIntAllocated = TRUE;
	                    }

	                    if (rshmParams->dataSharedAddr != NULL) {
	                        GT_assert (curTrace, (dataRegionId != SharedRegion_INVALIDREGIONID));
	                        dataRegionId= SharedRegion_getId (rshmParams->dataSharedAddr);
	                    }
	                    else {
                            dataRegionId = rshmParams->dataRegionId;
	                        GT_assert (curTrace, (dataRegionId != SharedRegion_INVALIDREGIONID));
	                        dataSharedAddr = (Ptr)Memory_alloc(
	                                                        SharedRegion_getHeap(dataRegionId),
	                                                        ROUND_UP(rshmParams->dataSharedAddrSize, SharedRegion_getCacheLineSize(dataRegionId)),
	                                                        SharedRegion_getCacheLineSize(dataRegionId),
	                                                        NULL);
	                        GT_assert (curTrace, (dataSharedAddr != NULL));
	                        obj->dataBufIntAllocated = TRUE;
	                    }

	                    /* Set up control, client structure addresses */
	                    obj->control = (Ptr) ((RingIOShm_ControlStruct *) obj->ctrlSharedAddr);
	                    obj->writer  = (Ptr) ((UInt32)obj->control +
						    CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, ctrlRegionId));
	                    obj->reader  = (Ptr) ((UInt32)obj->writer + CACHE_ALIGNED_SIZEOF(RingIOShm_Client, ctrlRegionId));

	                    control = obj->control;

	                    /* Define the interface type */
	                    control->interfaceAttrs.type = RingIO_Type_SHAREDMEM;

	                    /* Initialize the SrPtr for data buffer */
	                    control->srPtrBufStart     = SharedRegion_getSRPtr ((Ptr)dataSharedAddr,
	                                                                             dataRegionId);

	                    control->srPtrBufEnd       =    rshmParams->dataSharedAddrSize
	                                                  + rshmParams->footBufSize
	                                                  - 1;

	                    control->curBufEnd          = rshmParams->dataSharedAddrSize - 1;
	                    control->dataBufEnd         = rshmParams->dataSharedAddrSize - 1;
	                    control->dataSharedAddrSize = rshmParams->dataSharedAddrSize;
	                    control->footBufSize        = rshmParams->footBufSize;

	                    control->validSize         = 0;
	                    control->emptySize         = rshmParams->dataSharedAddrSize;

	                    /* Initialize the SrPtr for attr buffer */
	                    control->srPtrAttrBufStart =      SharedRegion_getSRPtr ((Ptr)attrSharedAddr,
	                                                                             attrRegionId);
	                    if (rshmParams->attrSharedAddrSize != 0) {
	                        control->srPtrAttrBufEnd = rshmParams->attrSharedAddrSize - 1;
	                    }
	                    else {
	                        control->srPtrAttrBufEnd = (UInt32) -1l;
	                    }
	                    control->curAttrBufEnd     = control->srPtrAttrBufEnd;

	                    control->validAttrSize     = 0;
	                    control->emptyAttrSize     = rshmParams->attrSharedAddrSize;

	                    control->prevAttrOffset    = RingIOShm_invalidOffset;

	                    /* Make sure both reader and writer are uninitialized */
	                    obj->reader->refCount   = 0;
	                    obj->reader->notifyId   = (UInt32) -1;
	                    obj->reader->isValid    = FALSE;
	                    obj->reader->isRegistered = FALSE;


	                    obj->writer->refCount   = 0;
	                    obj->writer->notifyId   = (UInt32) -1;
	                    obj->writer->isValid    = FALSE;
	                    obj->writer->isRegistered = FALSE;

	                    obj->openMode   = (UInt32) -1;

	                    if (SharedRegion_isCacheEnabled(ctrlRegionId)) {
	                        Cache_wbInv((Ptr)control,
	                                    sizeof(RingIOShm_ControlStruct),
	                                    Cache_Type_ALL,
	                                    TRUE);
	                        Cache_wbInv((Ptr)obj->writer,
	                                    sizeof(RingIOShm_Client),
	                                    Cache_Type_ALL,
	                                    TRUE);
	                        Cache_wbInv((Ptr)obj->reader,
	                                   sizeof(RingIOShm_Client),
	                                   Cache_Type_ALL,
	                                   TRUE);
	                    }

	                    GateMP_leave ((GateMP_Handle)obj->gateHandle, key);


	                    if (status >= 0) {
	                        ClientNotifyMgr_Params_init (&clientMgrParams);
	                        clientMgrParams.regionId = ctrlRegionId;
	                        /* NotifyEntries should be two as there are one reader and one
	                         * writer that  need notification
	                         */
	                        clientMgrParams.numNotifyEntries =RingIO_NOTIFYENTRIES;

	                        /* Set it for only one sub notify entry. as other client is only
	                         * one and only waits on single ringIO data buffer.
	                         */
	                        clientMgrParams.numSubNotifyEntries = RingIO_SUBNOTIFYENTRIES;

	                        /* Store RingIO gateHandle into shared memory */
	#if defined(SYSLINK_BUILD_RTOS)
	                        control->gateMPSharedAddr = ti_sdo_ipc_GateMP_getSharedAddr((ti_sdo_ipc_GateMP_Handle)obj->gateHandle);
	#else
	                        control->gateMPSharedAddr = GateMP_getSharedAddr(obj->gateHandle);
	#endif
	                        clientNotifyMgrGate = obj->gateHandle;
	                        if (NULL == clientNotifyMgrGate) {
	                            status = RingIOShm_E_FAIL;
	                        }
	                        else {
	                            obj->clientNotifyMgrGate = clientNotifyMgrGate; //TBD : (ti_sdo_ipc_GateMP_Handle) type cast
	                        }

	                        if (status >= 0) {
	                            /* Event no to be used for this instance */
	                            clientMgrParams.eventNo        = RingIOShm_moduleCfg.notifyEventNo;
	                            clientMgrParams.sharedAddr     = (Ptr) (  (UInt32)obj->ctrlSharedAddr +
								    CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, ctrlRegionId)
						            + 2 * CACHE_ALIGNED_SIZEOF(RingIOShm_Client, ctrlRegionId));
	                            clientMgrParams.sharedAddrSize = ClientNotifyMgr_sharedMemReq (&clientMgrParams);
	                            clientMgrParams.gate           = obj->clientNotifyMgrGate;                /* Pass the same name for ClientNotifyMgr */
	                            if (rshmParams->commonParams.name != NULL) {
	                                clientMgrParams.name    = rshmParams->commonParams.name;
	                            }
	                            clientMgrParams.openFlag = FALSE;

	                            if (SharedRegion_isCacheEnabled(ctrlRegionId)) {
	                                clientMgrParams.cacheFlags = ClientNotifyMgr_CONTROL_CACHEUSE;
	                            }

	                            obj->clientNotifyMgrHandle = ClientNotifyMgr_create (&clientMgrParams);
	                            if (NULL == obj->clientNotifyMgrHandle) {
	                                status = RingIOShm_E_FAIL;
	                            }

	                        }

	                        ((RingIO_Object *) handle)->top = (Ptr)handle;
	                        ((RingIO_Object *) handle)->obj = obj;

	                        if (rshmParams->ctrlSharedAddr != NULL) {
	                            ((RingIO_Object *) handle)->ctrlSharedAddr = (UInt32)rshmParams->ctrlSharedAddr;
	                            ((RingIO_Object *) handle)->ctrlRegionId = SharedRegion_getId (rshmParams->ctrlSharedAddr);
	                        }
	                        else {
	                            ((RingIO_Object *) handle)->ctrlRegionId   = rshmParams->ctrlRegionId;
	                            ((RingIO_Object *) handle)->ctrlSharedAddr = (UInt32)(obj->ctrlSharedAddr);
	                        }
	                    }
	                    if (status < 0) {
	                        RingIOShm_delete ((Ptr)&handle);
	                    }
	                    else {
	                        if (SharedRegion_isCacheEnabled(ctrlRegionId)) {
	                            Cache_wbInv ((Ptr)control,
	                                        sizeof(RingIOShm_ControlStruct),
	                                        Cache_Type_ALL,
	                                        TRUE);
	                        }
	                    }
	                }
	            }
	        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    return(status);
}

/*
 *  ============= RingIOShm_getCliNotifyMgrShAddr ==================
 *  Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *  instance used for this instance.
 */
Ptr RingIOShm_getCliNotifyMgrShAddr (RingIO_Handle handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    Int32           status = RingIOShm_S_SUCCESS;
#endif
    RingIOShm_Obj * object = NULL;
    Ptr             shAddr = NULL;
    UInt32          ctrlRegionId;

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getCliNotifyMgrShAddr",
               handle);

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        status = RingIOShm_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIO_getCliNotifyMgrGateShAddr",
                             RingIOShm_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif
        ctrlRegionId = SharedRegion_getId(object->ctrlSharedAddr);
        shAddr = (Ptr) ( (UInt32)object->ctrlSharedAddr +
		    CACHE_ALIGNED_SIZEOF(RingIOShm_ControlStruct, ctrlRegionId)
			+ 2 * CACHE_ALIGNED_SIZEOF(RingIOShm_Client, ctrlRegionId));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "RingIO_getCliNotifyMgrShAddr",
               shAddr );

    return (shAddr);
}

/*
 *  ============= RingIOShm_getCliNotifyMgrGateShAddr ==================
 *  Function to get the sharedMemBaseAddress of the clientNotifyMgr's
 *  gate instance used for this instance.
 */
Ptr RingIOShm_getCliNotifyMgrGateShAddr (RingIO_Handle handle)
{
    RingIOShm_Obj        * object = NULL;
    Ptr                    shAddr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    Int32                  status = RingIO_S_SUCCESS;
#endif

    GT_1trace (curTrace,
               GT_ENTER,
              "RingIO_getCliNotifyMgrGateShAddr",
               handle);


    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        status = RingIOShm_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_getCliNotifyMgrGateShAddr",
                             RingIOShm_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif

#if defined(SYSLINK_BUILD_RTOS)
    shAddr = SharedRegion_getPtr(ti_sdo_ipc_GateMP_getSharedAddr((ti_sdo_ipc_GateMP_Handle)object->clientNotifyMgrGate));
#else
    shAddr = SharedRegion_getPtr(GateMP_getSharedAddr(object->clientNotifyMgrGate));
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "_RingIO_getCliNotifyMgrGateShAddr",
               shAddr );

    return (shAddr);
}

/*
 *  ============= RingIOShm_setNotifyId ==================
 *  Sets the notification type
 */
Int
RingIOShm_setNotifyId (RingIO_Handle         handle,
                        UInt32               notifyId,
                        RingIO_NotifyType    notifyType,
                        UInt32               watermark)
{
    Int32                       status  = RingIOShm_S_SUCCESS;
    RingIOShm_Obj             * object = NULL;
    volatile RingIOShm_Client * client = NULL;
    IArg                     key;

    GT_4trace (curTrace,
               GT_ENTER,
              "RingIOShm_setNotifyId",
               handle,
               notifyId,
			   notifyType,
			   watermark);

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        status = RingIOShm_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_getCliNotifyMgrGateShAddr",
                             RingIOShm_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif
        if (RingIOShm_IS_WRITER (object)) {
            client = object->writer;
        }
        else if (RingIOShm_IS_READER (object)) {
            client = object->reader;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        else {
            status = RingIOShm_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "RingIOShm_setNotifyId",
                    status,
                    "Invalid RingIO handle as RingIO_open"
                    "is not yet called!");
        }

        if (status >= 0) {
#endif
            key = GateMP_enter (object->gateHandle);

            GT_assert (curTrace, (client != NULL));
            client->notifyId      = notifyId;
            client->notifyType    = notifyType;
            client->notifyWaterMark = watermark;
            if (   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                || (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ALWAYS)) {

                ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                                    client->notifyId,
                                                    0);
            }
            client->isRegistered  = TRUE;

            if (object->controlCacheUse == TRUE) {
                Cache_wbInv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
            }

            GateMP_leave (object->gateHandle, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }

    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "RingIOShm_setNotifyId", status);

    return (status);
}

/*
 *  ============= RingIOShm_resetNotifyId ==================
 *  Function to reset the notifyid received in userspace during call to
 *  ClientNotifyMgr_register client.
 */
Int
RingIOShm_resetNotifyId (RingIO_Handle handle, UInt32 notifyId)
{
    RingIOShm_Obj             * object = NULL;
    volatile RingIOShm_Client * client = NULL;
    IArg                     key;
    Int32                    status = RingIO_S_SUCCESS;

    GT_2trace (curTrace,
               GT_ENTER,
              "RingIOShm_resetNotifyId",
               handle,
               notifyId);

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        status = RingIOShm_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShm_resetNotifyId",
                             RingIOShm_E_INVALIDARG,
                             "Invalid NULL Handle passed");
    }
    else {
#endif
        if (RingIOShm_IS_WRITER (object)) {
            client = object->writer;
        }
        else if (RingIOShm_IS_READER (object)) {
            client = object->reader;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        else {
            status = RingIOShm_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "RingIOShm_resetNotifyId",
                    status,
                    "Invalid RingIO handle as RingIO_open"
                    "is not yet called!");
        }

        if (status >= 0) {
#endif
            key = GateMP_enter (object->gateHandle);

            GT_assert (curTrace, (client != NULL));
            client->notifyId      = notifyId;
            client->isRegistered  = FALSE;
            client->notifyType    = RingIO_NOTIFICATION_NONE;
            client->notifyWaterMark     = (UInt32)-1;

            if (   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                || (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ALWAYS)) {

                ClientNotifyMgr_disableNotification (client->clientNotifyMgrHandle,
                                                     client->notifyId,
                                                     0);
            }

            GateMP_leave (object->gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "RingIOShm_resetNotifyId", status);

    return (status);
}

/* =============================================================================
 * APIs called directly by applications
 * =============================================================================
 */
/*
 *  ============= RingIOShm_Params_init ==================
 *  Function that returns the default parameter structure.
 */
Void RingIOShm_Params_init(Void * params)
{
   RingIOShm_Params * sparams = (RingIOShm_Params *)params;

   GT_assert (curTrace, (NULL != params));

    memcpy (sparams,
            &(RingIOShm_module->defaultInstParams),
            sizeof (RingIOShm_Params));
    return;
}

/* =============================================================================
 * API's called internally by Ipc module
 * =============================================================================
 */
/*
 *  ============= RingIOShm_getConfig ==================
 *  Function to get the default configuration for the RingIOShm
 *  module.
 *
 *  This function can be called by the application to get their
 *  configuration parameter to RingIOShm_setup filled in by the
 *  RingIOShm module with the default parameters. If the user does
 *  not wish to make any change in the default parameters, this API
 *  is not required to be called.
 *
 *  cfgParams is the pointer to the RingIOShm module configuration structure
 *  in which the default config is to be returned.
 */
Void
RingIOShm_getConfig (RingIOShm_Config * cfg)
{
    GT_assert (curTrace, (NULL != cfg));

    if (cfg != NULL) {
        memcpy (cfg,
                &Syslink_RingIOShm_moduleCfg,
                sizeof (RingIOShm_moduleCfg));
    }
    return;
}

/*
 *  ============= RingIOShm_setup ==================
 *  Setup the RingIOShm module.
 *
 *  This function sets up the RingIOShm module. This
 *  function must be called before any other instance-level APIs can
 *  be invoked.
 *  Module-level configuration needs to be provided to this
 *  function. If the user wishes to change some specific config
 *  parameters, then RingIOShm_getConfig can be called to
 *  get the configuration filled with the default values. After
 *  this, only the required configuration values can be changed. If
 *  the user does not wish to make any change in the default
 *  parameters, the application can simply call
 *  RingIOShm_setup with NULL parameters. The default
 *  parameters would get automatically used.
 *
 *  cfg is the RingIOShm module configuration.
 *  If provided as NULL, default configuration is used.
 */
Int
RingIOShm_setup (const RingIOShm_Config * cfg)
{
    Int                status = RingIOShm_S_SUCCESS;
    RingIOShm_Config   tmpCfg;
    IArg               key;

    key = Gate_enterSystem();

    /* Initialize the commonParams field */
    RingIO_Params_init(&(RingIOShm_module->defaultInstParams.commonParams));

    RingIOShm_module->refCount++;
    if (RingIOShm_module->refCount > 1) {
        status = RingIOShm_S_ALREADYSETUP;
        Gate_leaveSystem(key);
    }
    else {
        Gate_leaveSystem(key);    /* Copy the user provided values into the state object. */
        if (cfg == NULL) {
            RingIOShm_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }
        memcpy (&RingIOShm_moduleCfg,
                cfg,
                sizeof (RingIOShm_Config));

        RingIOShm_module->isSetup = TRUE;
    }

    return (status);
}

/*
 *  ============= RingIOShm_destroy ==================
 *  Destroy the RingIOShm module.
 *
 *  Once this function is called, other RingIOShm module
 *  APIs, except for the RingIOShm_getConfig API cannot
 *  be called anymore.
 */
Int
RingIOShm_destroy (Void)
{
    Int status = RingIOShm_S_SUCCESS;
    IArg        key;

//     RingIOShm_module->cfg.reserved = 0;

    key = Gate_enterSystem();
    RingIOShm_module->refCount--;
    Gate_leaveSystem(key);
    if (RingIOShm_module->refCount == 0) {
        RingIOShm_module->isSetup = FALSE;
    }

    return (status);
}



/* =============================================================================
 * Internal API's
 * =============================================================================
 */
/*
 *  ============= _RingIOShm_sendNotification ==================
 *  Sends a notification to client registered with given event Id
 */
Int
_RingIOShm_sendNotification
                           (ClientNotifyMgr_Handle handle,
                            UInt32                 slot,
                            UInt32                 subId,
                            UInt32                 arg1,
                            UInt32                 arg2)
{
    Int                    status    = RingIOShm_S_SUCCESS;


    if (slot != (UInt32)-1) {
        status = ClientNotifyMgr_sendNotification (handle,
                                                   slot,
                                                   subId,
                                                   arg1,
                                                   arg2);
        if (status >= 0) {
            status = RingIOShm_S_SUCCESS;
        }
        else {
            status = RingIO_E_NOTIFYFAIL;
        }
    }
    else {
        status = RingIO_E_NOTIFYFAIL;
    }

    return status;
}

/*
 *  ============= _RingIOShm_writerAcquire ==================
 *  Acquire a buffer from the RingIOShm instance for the writer
 */
Int
_RingIOShm_writerAcquire (RingIOShm_Obj *    object,
                          RingIO_BufPtr *    pData,
                          UInt32 *           pSize)
{
    Int                                 status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct *  control = NULL;
    volatile RingIOShm_Client  *        client  = NULL;
    UInt32                              storedEmptySize = 0;
    UInt32                              bufSize;
    UInt32                              bytesTillEndOfBuffer;
    UInt32                              contiguousSpace;
    IArg                                key;

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    control = object->control;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    client  = object->writer;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }

    /* If no free space is available, return failure */
    if (control->emptySize == 0) {
        *pData = NULL;
        *pSize = 0;
        status = RingIOShm_E_BUFFULL;
    }
    else {
        /* Current pSize of the buffer */
        bufSize = RingIOShm_GET_CUR_DATA_END (object);
        storedEmptySize = control->emptySize;

        /*  Find total contiguous free space available. */
        bytesTillEndOfBuffer = (  bufSize
                                - ((  RingIOShm_ADJUST_MODULO (client->acqStart,
                                                     client->acqSize,
                                                     bufSize))));

        contiguousSpace = SYSLINK_min (bytesTillEndOfBuffer, control->emptySize);

        /* If the requested free space is not available as a contiguous chunk */
        if (*pSize > contiguousSpace) {
            /*  If the contiguousBufFlag is set, we need to satisfy the
             *  request only if a contiguous data buffer of requested pSize
             *  is found. This can be done as follows:
             *  1) Check if an early end to the buffer can be set and then a
             *     contiguous data buffer can be provided from the start
             *     of buffer.
             *  2) If step 1 does not give a contiguous buffer return
             *     failure.
             */
            if (client->flags & RingIO_NEED_EXACT_SIZE) {
                /*  Check if the data request can be satisfied from the
                 *  start of the buffer.
                 */
                if ((control->emptySize - contiguousSpace) < *pSize) {
                    *pSize   = 0;
                    *pData   = (RingIO_BufPtr) NULL;
                    status   = RingIOShm_E_FAIL;
                }
                else {
                    /* Mark early buf End */
                    control->curBufEnd -= contiguousSpace;

                    /* Reduce amount of empty pSize available due to early end */
                    control->emptySize -= contiguousSpace;
                }
            }
            else {
                if (*pSize > control->emptySize) {
                    status = RingIOShm_E_BUFFULL;
                }
                else {
                    status = RingIOShm_E_BUFWRAP;
                }
                *pSize = contiguousSpace;
            }
        }

        /* Buffer is to be returned even if status is failure with
         * RingIOShm_EBUFFULL or RingIOShm_EBUFWRAP.
         */
        if (status != RingIOShm_E_FAIL) {
            /* Calculate the current buffer pSize */
            bufSize = RingIOShm_GET_CUR_DATA_END (object);

            if (client->acqStart == bufSize) {
                client->acqStart = 0;
            }

            /* Get the next acquire address */
            *pData = (RingIO_BufPtr) (  (UInt32) client->pDataStart
                                        + ( RingIOShm_ADJUST_MODULO (client->acqStart,
                                                           client->acqSize,
                                                           bufSize)));

            client->acqSize    += *pSize;
            control->emptySize -= *pSize;
        }
    }

    /* On acquire failure notification is enabled for all types. */
    if (   ((status == RingIOShm_E_FAIL) || (status == RingIOShm_E_BUFFULL))
        && (client->notifyType != RingIO_NOTIFICATION_NONE)) {
        ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                            client->notifyId,
                                            0);
    }
    else if (*pSize != 0) {
        /* This check is to disable notification in RingIO_NOTIFICATION_ALWAYS
         * case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Empty Data was earlier above watermark and now falls below
         *    watermark.
         */
        if (client->notifyType == RingIO_NOTIFICATION_ALWAYS) {
            if (   (storedEmptySize >= client->notifyWaterMark)
                && (control->emptySize < client->notifyWaterMark)) {
                ClientNotifyMgr_disableNotification (client->clientNotifyMgrHandle,
                                                     client->notifyId,
                                                     0);
            }
        }
        /* This check is to enable notification in
         * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Empty Data Size falls below watermark.
         */
        else if ((client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE) &&
            (control->emptySize < client->notifyWaterMark)) {
            ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                                client->notifyId,
                                                0);
        }
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return status;
}

/*
 *  ============= _RingIOShm_readerAcquire ==================
 *  Acquire a buffer from the RingIOShm instance for the reader
 */
Int
_RingIOShm_readerAcquire (RingIOShm_Obj * object,
                          RingIO_BufPtr * pData,
                          UInt32 *        pSize)
{
    Int                  status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * client  = NULL;
    UInt32               bufSize;
    UInt32               contiguousSpace;
    RingIOShm_Attr *        attrBuf;
    UInt32               storedValidSize;
    UInt32               nextAcqOffset;
    UInt32               i;
    Char *               top;
    Char *               foot;
    Char *               footStart;
    UInt32               footSize;
    IArg                 key;

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    control = object->control;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    client = object->reader;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }


    /* Current size of the buffer */
    bufSize = RingIOShm_GET_CUR_DATA_END (object);

    if (control->validSize == 0) {
        storedValidSize = 0;
        *pData = NULL;
        *pSize = 0;
        if (control->validAttrSize == 0) {
            status = RingIOShm_E_BUFEMPTY;
        }
        else {
            /* Return RingIO_E_PENDINGATTRIBUTE
             * as an attribute is present at
             * the current read location and
             * no valid data
             */
            status = RingIO_E_PENDINGATTRIBUTE;
        }
    }
    else {
        storedValidSize = control->validSize;

        /* Initialize the nextAcqOffset */
        nextAcqOffset = RingIOShm_ADJUST_MODULO (client->acqStart,
                                       client->acqSize,
                                       bufSize);

        if (control->validAttrSize) {
            attrBuf = _RingIOShm_getFirstAttr (object);
            GT_assert (curTrace, (attrBuf != NULL));
            /* Handle the special wraparound case when there is an attribute at
             * the *very* end of the buffer
             *  If attributes are present, and there is an attribute present at the
             *  current read offset, return RingIO_E_PENDINGATTRIBUTE
             */
            if (   ((nextAcqOffset == 0) && (attrBuf->offset == bufSize))
                || (attrBuf->offset == nextAcqOffset)) {
                *pData = NULL;
                *pSize = 0;
                status = RingIO_E_PENDINGATTRIBUTE;
            }
        }

        if (status != RingIO_E_PENDINGATTRIBUTE) {
            status = _RingIOShm_getContiguousDataSize (object, &contiguousSpace);

            /* If requested data size is not available as a contiguous chunk */
            if (*pSize > contiguousSpace) {
                if (client->flags & RingIO_NEED_EXACT_SIZE) {
                    *pSize = 0;
                    *pData = (RingIO_BufPtr) NULL;
                    status = RingIOShm_E_FAIL;
                }
                else {
                    *pSize  = contiguousSpace;
                    if (status != RingIOShm_S_PENDINGATTRIBUTE) {
                        status = RingIO_S_NOTCONTIGUOUSDATA;
                    }
                }
            }
            else {
                status = RingIOShm_S_SUCCESS;
            }

            /* Buffer is to be returned even if status is failure with
             * RingIOShm_E_BUFEMPTY.
             */
            if (status != RingIOShm_E_FAIL) {
                /* Check if the buffer needs to be copied from top of
                 * the data buffer to the footer area
                 */
                if (*pSize > (bufSize - nextAcqOffset)) {
                    top  = (Char *) client->pDataStart;
                    foot = (Char *) (  ((UInt32) client->pDataStart)
                                       + bufSize);
                    footStart = foot;
                    footSize  = (*pSize - (bufSize - nextAcqOffset));

                    if (object->dataBufCacheUse == TRUE) {
                        Cache_inv((Ptr)top,
                                    footSize,
                                    Cache_Type_ALL,
                                    TRUE);
                        Cache_inv ((Ptr) (  (UInt32) footStart - ((UInt32) footStart % SharedRegion_getCacheLineSize(SharedRegion_getId(object->ctrlSharedAddr)))),
                                            ((UInt32) footStart %
                                             SharedRegion_getCacheLineSize(SharedRegion_getId(object->ctrlSharedAddr))),
                                             Cache_Type_ALL,
                                             TRUE);
                    }

                    for (i = 0; i < footSize; i++) {
                        *foot++ = *top++;
                    }

                    if (object->dataBufCacheUse == TRUE) {
                        /* Align footStart to cache line boundary. */
                        footSize  =   footSize
                                    + ((UInt32) footStart % SharedRegion_getCacheLineSize(SharedRegion_getId(object->ctrlSharedAddr))) ;
                        footStart = (Char *)
                                  (  (UInt32) footStart
                                   - ((UInt32) footStart % SharedRegion_getCacheLineSize(SharedRegion_getId(object->ctrlSharedAddr)))) ;
                        Cache_wbInv ((Ptr) footStart,
                                     footSize,
                                     Cache_Type_ALL,
                                     TRUE) ;
                    }
                }

                /* Reset acquire start in the case of wraparound.  */
                if (client->acqStart == bufSize) {
                    client->acqStart = 0;
                }

                /* Get the next acquire address */
                *pData = (RingIO_BufPtr) (  (UInt32) client->pDataStart
                                            + nextAcqOffset);

                client->acqSize    += *pSize;
                control->validSize -= *pSize;

                if (object->dataBufCacheUse == TRUE) {
                    Cache_inv ((Ptr) *pData,
                                *pSize,
                                Cache_Type_ALL,
                                TRUE);
                }
            }
        }
    }

    /* On acquire failure notification is enabled for all types. */
    if (   ((status == RingIOShm_E_FAIL) || (status == RingIOShm_E_BUFEMPTY))
        && (client->notifyType != RingIO_NOTIFICATION_NONE)) {
        ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                            client->notifyId,
                                            0);
    }
    else if (*pSize != 0) {
        /* This check is to disable notification in RingIO_NOTIFICATION_ALWAYS
         * case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Valid Data was earlier above watermark and now falls below
         *    watermark.
         */
        if (client->notifyType == RingIO_NOTIFICATION_ALWAYS) {
            if (   (storedValidSize >= client->notifyWaterMark)
                && (control->validSize < client->notifyWaterMark)) {
                ClientNotifyMgr_disableNotification (client->clientNotifyMgrHandle,
                                                     client->notifyId,
                                                     0);
            }
        }
        /* This check is to enable notification in
         * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Valid Data Size falls below watermark.
         */
        else if ((client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE) &&
            (control->validSize < client->notifyWaterMark)) {
            /* Enable the notifyFlag of the client in HDWRFIFO_ONCE_ case
             * if the amount of data available is less than watermark.
             */
                ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                                    client->notifyId,
                                                    0);
        }
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return status;
}

/*
 *  ============= _RingIOShm_writerRelease ==================
 *  Release a buffer to the RingIO instance from a writer
 */
Int
_RingIOShm_writerRelease (RingIOShm_Obj * object,
                          UInt32          size)
{
    Int                  status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_Client  * client  = NULL;
    volatile RingIOShm_ControlStruct * control = NULL;
    Bool                 isFull  = FALSE;
    UInt32               bufSize;
    UInt32               endBufOffset;
    RingIOShm_Attr *        attrBuf;
    UInt32               temp ;


    control = object->control;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    client = object->writer;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }


    /* Make sure that we are not trying to release what we haven't acquired */
    if (size > client->acqSize) {
        status = RingIOShm_E_FAIL;
    }
    else {
        /* Get the current data buffer Size */
        bufSize = RingIOShm_GET_CUR_DATA_END (object);

        /* TBD: This will not be needed as attribute will already be released
         *      So code needs to be removed
         */
        /* Starting from the first attribute release all attributes with
         * offsets between client->acqStart and (client->acqStart + size)
         */
        if (client->pAttrStart != NULL) {
            endBufOffset = (client->acqStart + size) % bufSize;

            /* Empty buffer size not done here as 'size' check in ringio.c
             * No extra check for (size == bufSize) as below condition
             * implies the same state.
             */
            if (client->acqStart == endBufOffset) {
                isFull = TRUE;
            }

            for (;; ) {
                attrBuf = _RingIOShm_getFirstAttr (object);

                /* Check if attribute is within size to be released.
                 * If offset is between acqStart and endBufOffset it
                 * must be released.
                 */
                if (    (attrBuf == NULL)
                    ||  (!_RingIOShm_isOffsetInBetween (attrBuf->offset,
                                                                 client->acqStart,
                                                                 endBufOffset,
                                                                 isFull))) {
                    break;
                }

                _RingIOShm_releaseAttribute (object, attrBuf);
            }
        }


        if (object->dataBufCacheUse == TRUE) {
            temp = ((UInt32) client->pDataStart) + client->acqStart ;
            Cache_wbInv((Ptr)temp,
                    size,
                    Cache_Type_ALL,
                    TRUE);
        }

        client->acqStart    = RingIOShm_ADJUST_MODULO (client->acqStart, size, bufSize);
        client->acqSize    -= size;
        control->validSize += size;
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }



    return status;
}

/*
 *  ============= _RingIOShm_readerRelease ==================
 *  Release a buffer to the RingIO instance from a reader
 */
Int
_RingIOShm_readerRelease (RingIOShm_Obj * object,
                          UInt32             size)
{
    Int                  status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * client  = NULL;
    volatile RingIOShm_Client  * otherClient  = NULL;
    Bool                 isFull  = FALSE;
    UInt32               bufSize;
    UInt32               storedAcqStart;
    UInt32               storedAttrAcqStart;
    UInt32               endBufOffset;
    RingIOShm_Attr *        attrBuf;


    control = object->control;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    client      = object->reader;
    otherClient = object->writer;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
        Cache_inv((Ptr)otherClient,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }


    /* Make sure that we are not trying to release what we haven't acquired */
    if (size > client->acqSize) {
        status = RingIOShm_E_FAIL;
    }
    else {
        storedAcqStart = client->acqStart;
        storedAttrAcqStart = client->acqAttrStart;

        bufSize = RingIOShm_GET_CUR_DATA_END (object);

        /* If attribute buffer is present, starting from the first attribute,
         * release all attributes with offsets between client->acqStart and
         * client->acqStart+size.
         */
        if (client->pAttrStart != NULL) {
            endBufOffset = (client->acqStart + size) % bufSize;

            /* Empty buffer size not done here as 'size' check in ringio.c
             * No extra check for (size == bufSize) as below condition
             * implies the same state.
             */
            if (client->acqStart == endBufOffset) {
                isFull = TRUE;
            }

            for (;; ) {
                attrBuf = _RingIOShm_getFirstAcquiredAttr (object);
                if (   (attrBuf != NULL)
                    && ( _RingIOShm_isOffsetInBetween (attrBuf->offset,
                                                    client->acqStart,
                                                    endBufOffset,
                                                    isFull))) {
                        _RingIOShm_releaseAttribute (object, attrBuf);
                }
                else {
                    break;
                }
            }

        }

        /* Release the data buffer */
        client->acqStart    = (client->acqStart + size) % bufSize;
        client->acqSize    -= size;
        control->emptySize += size;

        /* Reset the early end for the data buffer if reader has read past
         * the early end marker
         */
         if (   (RingIOShm_GET_CUR_DATA_END (object) != control->dataSharedAddrSize)
             && (RingIOShm_GET_CUR_DATA_END (object)) <= (storedAcqStart + size)) {
                /* Shift up all attributes set in the unused area
                 * (occurred due to  early end).
                 */
                _RingIOShm_updateAttributes (object, RingIOShm_GET_CUR_DATA_END (object));

                control->emptySize += (control->dataBufEnd
                                        - control->curBufEnd);
                control->curBufEnd  = control->dataBufEnd;
         }


        /* Reset the early end for attribute buffer if reader has read past
         * the early end marker
         */
        if (storedAttrAcqStart > client->acqAttrStart) {
            control->emptyAttrSize +=   control->srPtrAttrBufEnd
                                      - control->curAttrBufEnd;
            control->curAttrBufEnd  = control->srPtrAttrBufEnd;
        }

        /* Reset buffer poInters if there is no data in buffer */
        _RingIOShm_initInstance (object);
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
        Cache_wbInv((Ptr)otherClient,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }


    return status;
}

/*
 *  ============= _RingIOShm_writerCancel ==================
 *  Cancel the previous acquired buffer to the RingIO instance
 */
Int
_RingIOShm_writerCancel (RingIOShm_Obj * object)
{
    Int                status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * client  = NULL;
    volatile RingIOShm_Client  * otherClient  = NULL;
    RingIOShm_Attr *        attrBuf = NULL;
    IArg                    key;

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    control = object->control;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    client      = object->writer;
    otherClient = object->reader;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
        Cache_inv((Ptr)otherClient,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }


    if (client->acqSize == 0) {
        /* Nothing acquired */
        status = RingIOShm_E_FAIL;
    }
    else {
        /*
         * If any attributes had been set by the writer, update the
         * control structure appropriately to point to the
         * previous attribute buffer
         */
        attrBuf = _RingIOShm_getFirstAttr (object);
        if (attrBuf != NULL) {
            control->prevAttrOffset = attrBuf->prevoffset;
        }

        /* Clear early end for data buffer, if set, in the following two cases:
         * 1. Early end is set in the acquired range to be cancelled.
         * 2. Client acqStart is at the start of data buffer. In this case, last
         *    acquire was larger than size remaining in buffer, hence early end
         *    was set. For next acquire, this may not be the case, so early end
         *    can be reset.
         */

        if (    (control->curBufEnd != control->dataBufEnd)
                /* No check for isFull is needed here as this (client->acqStart
                 * + client->acqSize)) is not a modulo operation and they can
                 * never be equal at this point. The check for empty client->
                 * acqSize = 0 is done earlier.
                 */
            &&  (    _RingIOShm_isOffsetInBetween (control->curBufEnd,
                                                client->acqStart,
                                                (client->acqStart + client->acqSize),
                                                FALSE)
                 ||  (client->acqStart == 0))) {

            /* If acqStart has moved to the start of data buffer and early end
             * is being cleared, the acqStart can be set back to early end
             * before resetting the early end.
             */
            if (client->acqStart == 0) {
                client->acqStart = RingIOShm_GET_CUR_DATA_END (object);
                /* Any attributes with offset at start of data buffer need to
                 * be shifted to point to early end.
                 */
                _RingIOShm_updateAttributes (object, 0);
            }
            control->emptySize += (control->dataBufEnd - control->curBufEnd);
            control->curBufEnd = control->dataBufEnd;
        }

        control->emptySize     += client->acqSize;
        client->acqSize         = 0;

        /* Clear early end for attr buffer, if set, in the following two cases:
         * 1. Early end is set in the acquired range to be cancelled.
         * 2. Client acqAttrStart is at the start of data buffer. In this case,
         *    last acquire was larger than size remaining in buffer, hence early
         *    end was set. For next acquire, this may not be the case, so early
         *    end can be reset.
         * No check for isFull is needed here as this (client->acqStart
         * + client->acqSize)) is not a modulo operation and they can
         * never be equal at this point. The check for empty client->
         * acqSize = 0 is done earlier.
         */
        if (    (control->curAttrBufEnd != control->srPtrAttrBufEnd)
            &&  (    _RingIOShm_isOffsetInBetween (control->curAttrBufEnd,
                                    client->acqAttrStart,
                                    (client->acqAttrStart + client->acqAttrSize),
                                    FALSE)
                 ||  (client->acqAttrStart == 0))) {
            /* If acqAttrStart has moved to the start of data buffer and early
             * end is being cleared, the acqAttrStart can be set back to early
             * end before resetting the early end.
             */
            if (client->acqAttrStart == 0) {
                client->acqAttrStart = RingIOShm_GET_CUR_ATTR_END (object);
            }
            control->emptyAttrSize += (     control->srPtrAttrBufEnd
                                       -    control->curAttrBufEnd);
            control->curAttrBufEnd = control->srPtrAttrBufEnd;
        }

        control->emptyAttrSize += client->acqAttrSize;
        client->acqAttrSize     = 0;

        /* This check is to disable notification in
         * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Empty Data Size falls above watermark.
         */
        if (   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                 && (control->emptySize >= client->notifyWaterMark)) {
            ClientNotifyMgr_disableNotification (client->clientNotifyMgrHandle,
                                                 client->notifyId,
                                                 0);
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIOShm_initInstance (object);
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
        Cache_wbInv((Ptr)otherClient,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return status;
}

/*
 *  ============= _RingIOShm_readerCancel ==================
 *  Cancel the previous acquired buffer to the RingIO instance
 */
Int
_RingIOShm_readerCancel (RingIOShm_Obj * object)
{
    Int                status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * client  = NULL;
    volatile RingIOShm_Client  * otherClient  = NULL;
    IArg                         key;

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    control = object->control;
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    client = object->reader;
    otherClient = object->writer;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
        Cache_inv((Ptr)otherClient,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }


    if (client->acqSize == 0) {
        status = RingIOShm_E_FAIL;
    }
    else {
        control->validSize     += client->acqSize;
        control->validAttrSize += client->acqAttrSize;
        client->acqSize         = 0;
        client->acqAttrSize     = 0;

        /* This check is to disable notification in
         * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Valid Data Size falls above watermark.
         */
        if(   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                && (control->validSize >= client->notifyWaterMark)) {
            ClientNotifyMgr_disableNotification (client->clientNotifyMgrHandle,
                                                 client->notifyId,
                                                 0);
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIOShm_initInstance (object);
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
        Cache_wbInv((Ptr)otherClient,
                   sizeof(RingIOShm_Client),
                   Cache_Type_ALL,
                   TRUE);
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return status;
}

/*
 *  ============= _RingIOShm_readergetvAttribute ==================
 *  Get a variable sized attribute
 */
Int
_RingIOShm_readergetvAttribute (RingIOShm_Obj * object,
                                UInt16 *        type,
                                UInt32 *        param,
                                RingIO_BufPtr   vptr,
                                UInt32 *        pSize)
{
    Int                           status    = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control   = NULL;
    volatile RingIOShm_Client  * client    = NULL;
    volatile RingIOShm_Client  * otherClient = NULL;
    RingIOShm_Attr *        attrBuf;
    UInt32               bufSize;
    IArg                 key;

    control = object->control;

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)object->control,
                  sizeof (RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    client = object->reader;
    otherClient = object->writer;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
        Cache_inv((Ptr)otherClient,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
    }


    /* Check if attribute buffer has been provided. */
    if (client->pAttrStart == NULL) {
        status = RingIOShm_E_FAIL;
    }
    else {
        /* If there are no valid attributes, return failure */
        if (control->validAttrSize == 0) {
            status = RingIOShm_E_FAIL;
        }
    }

    if (status >=0) {
        /* Get the current buffer size */
        bufSize = RingIOShm_GET_CUR_DATA_END (object);

        /*  Since we have already made sure that there are attributes,
         *  no need to check attrBuf is NULL
         */
        attrBuf = _RingIOShm_getFirstAttr (object);
        /* If there is an attribute at the current read offset */
        if (   (attrBuf->offset == (client->acqStart + client->acqSize))
            || (RingIOShm_ADJUST_MODULO (client->acqStart, client->acqSize, bufSize)
                == attrBuf->offset)) {
            /* Copy the attribute type and attribute param */
            *type  = attrBuf->type;
            *param = attrBuf->param;

            if (pSize == NULL) {
                /* If the user hasn't supplied a buffer, but the attribute
                 * present is a variable attribute, return error
                 */
                if (attrBuf->size) {
                    status = RingIOShm_E_VARIABLEATTRIBUTE;
                }
            }
            else {
                /* If the user hasn't supplied a sufficient buffer for the
                 * payload return error, and return the correct size of the
                 * attr as well
                 */
                if (*pSize < attrBuf->size) {
                    *pSize = attrBuf->size;
                    status = RingIOShm_E_FAIL;
                }
                else {
                    /* Copy the payload and its size */
                    *pSize = attrBuf->size;
                    memcpy (vptr,
                            (Void *) ((UInt32) attrBuf + sizeof(RingIOShm_Attr)),
                            *pSize);
                }
            }

            if (status >=0) {
                control->validAttrSize -= (RingIOShm_SIZEOF_ATTR (attrBuf->size));

                /* Free up the attribute buffer in case the client has not
                 * acquired anything
                 */
                if (client->acqSize == 0) {
                    control->emptyAttrSize +=  sizeof (RingIOShm_Attr)
                                             + attrBuf->size;
                    client->acqAttrStart =   (   client->acqAttrStart
                                              +  (sizeof (RingIOShm_Attr)
                                              +  attrBuf->size))
                                          %  (RingIOShm_GET_CUR_ATTR_END (object));
                    client->acqAttrSize = 0;
                }
                else {
                    client->acqAttrSize    += sizeof (RingIOShm_Attr)
                                             + attrBuf->size;
                }

                /* We need to return RingIOShm_S_PENDINGATTRIBUTE if there are
                 * more attributes at the current read offset
                 */
                attrBuf = _RingIOShm_getFirstAttr (object);
                if (attrBuf != NULL) {
                    /* If there is an attribute at the current read offset */
                    if (RingIOShm_ADJUST_MODULO (client->acqStart,
                                       client->acqSize,
                                       bufSize)
                        == attrBuf->offset) {
                        status = RingIOShm_S_PENDINGATTRIBUTE;
                    }
                    attrBuf->prevoffset = RingIOShm_invalidOffset;
                }
                else {
                    otherClient = object->writer;

                    if (object->controlCacheUse == TRUE) {
                        Cache_inv((Ptr)otherClient,
                                  sizeof(RingIOShm_Client),
                                  Cache_Type_ALL,
                                  TRUE);
                    }

                    if (otherClient->acqAttrSize == 0) {
                        control->prevAttrOffset = RingIOShm_invalidOffset;
                    }
                }
            }
        }
        else {
            /* We need to read more data before reading an attribute */
            status = RingIOShm_E_PENDINGDATA;
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIOShm_initInstance (object);
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)client,
                    sizeof(RingIOShm_Client),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)otherClient,
                    sizeof(RingIOShm_Client),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    return status;
}

/*
 *  ============= _RingIOShm_writersetvAttribute ==================
 *  This function sets an attribute with a variable sized payload at the
 *  offset 0 provided in the acquired data buffer.
 */
Int
_RingIOShm_writersetvAttribute (RingIO_Handle    handle,
                                UInt16           type,
                                UInt32           param,
                                RingIO_BufPtr    pdata,
                                UInt32           size,
                                Bool             sendNotification)
{
    Int                           status     = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * client  = NULL;
    volatile RingIOShm_Client  * otherClient = NULL;
    RingIOShm_Obj              * object = NULL;
    UInt32               offset                 = 0;
    RingIOShm_Attr *        destAttrBuf = NULL;
    UInt32               bytesTillEndOfBuffer;
    UInt32               contiguousSpace;
    UInt32               bufSize;
    UInt32               attrSharedAddrSize;
    UInt32               attrOffset;
    IArg                 key;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    key = GateMP_enter ((GateMP_Handle)object->gateHandle);
    control = object->control;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    client      = object->writer;
    otherClient = object->reader;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
        Cache_inv((Ptr)otherClient,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
    }


    /* Check if attribute buffer has been provided. */
    if (client->pAttrStart == NULL) {
        status = RingIOShm_E_FAIL;
    }

    if (status >=0) {
        /* Data Buffer current Size */
        bufSize = RingIOShm_GET_CUR_DATA_END (object);
        if (    (control->validSize == (RingIOShm_GET_CUR_DATA_END (object)))
            ||  (   (control->emptySize == 0)
                 && (offset             == client->acqSize))) {
            /* Do not allow setting an attribute when it falls into reader
             * region. The following scenarios cover this condition:
             * - The buffer is completely full. In this case, attribute can only
             *   be set at offset 0. But offset 0 falls into reader region.
             * - The buffer is completely acquired by the writer. Part or none
             *   of this buffer may be released. Writer is attempting to set an
             *   attribute at the end of its acquired range. In this case, end
             *   of writer buffer is the same as beginning of reader buffer.
             *
             * If the reader has acquired and released some data, resulting in
             * its moving further such that its acquire start is not at the
             * same location where writer may be able to set an attribute, the
             * above conditions do not hold true, and the attribute is allowed
             * to be set.
             */
            status = RingIOShm_E_WRONGSTATE;
        }
        else if (client->acqSize) {
            /* Check if the offset provided is within acquired range. */
            if (offset > client->acqSize) {
                status = RingIOShm_E_FAIL;
            }
        }
        else {
            /*
             *  This is the case when no data has been acquired, but the
             *  writer wishes to set an attribute at the next buffer write
             *  location. To enable this we setup offset as 0.This makes
             *  sure that an attribute is set at an offset which corresponds
             *  to the next position which can be acquired next
             */
            offset = 0;
        }

        if (status >=0) {
            attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);
            bytesTillEndOfBuffer =   attrSharedAddrSize
                                   - (  (  client->acqAttrStart
                                         + client->acqAttrSize)
                                      % attrSharedAddrSize);

            contiguousSpace = SYSLINK_min (bytesTillEndOfBuffer,
                                          control->emptyAttrSize);

            if (contiguousSpace < (RingIOShm_SIZEOF_ATTR(size))) {
                /* Check if the attribute can be placed at the beginning of
                 * buffer
                 */
                if (  (control->emptyAttrSize - contiguousSpace)
                    < (RingIOShm_SIZEOF_ATTR(size))) {
                    status = RingIOShm_E_FAIL;
                }
                else {
                    control->curAttrBufEnd -= contiguousSpace;
                    control->emptyAttrSize -= contiguousSpace;
                }
            }
        }
    }

    if (status >=0) {
        /* Copy the attribute to the next available location and update
         * all fields
         */
        /* Attr Buffer current size */
        attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);

        attrOffset  = RingIOShm_ADJUST_MODULO (client->acqAttrStart,
                                     client->acqAttrSize,
                                     attrSharedAddrSize);

        destAttrBuf = (RingIOShm_Attr*)((UInt32) client->pAttrStart + attrOffset);

        destAttrBuf->offset     = (client->acqStart + offset) % bufSize;
        destAttrBuf->prevoffset = control->prevAttrOffset;
        destAttrBuf->size       = size;
        destAttrBuf->type       = type;
        destAttrBuf->param      = param;

        /* Copy data contents for variable attribute */
        if (size != 0) {
            memcpy ((Void *)((UInt32) destAttrBuf + sizeof(RingIOShm_Attr)),
                    pdata,
                    size);
        }

        control->prevAttrOffset = attrOffset;

        /* Now the attribute buffer has been acquired, and the
         * attribute has also been set. So update all the relevant fields
         */
        control->emptyAttrSize -= (RingIOShm_SIZEOF_ATTR(size));
        client->acqAttrSize    += (RingIOShm_SIZEOF_ATTR(size));

        /* If no data buffer has been acquired, the attribute can be
         * released now
         */
        if (client->acqSize == 0) {
            client->acqAttrStart    = RingIOShm_ADJUST_MODULO (client->acqAttrStart,
                                                     (RingIOShm_SIZEOF_ATTR(size)),
                                                     attrSharedAddrSize);

            client->acqAttrSize     = 0;
            control->validAttrSize += (RingIOShm_SIZEOF_ATTR(size));
        }

    }

    if (object->attrBufCacheUse == TRUE) {
        Cache_wbInv((Ptr)destAttrBuf,
                    RingIOShm_SIZEOF_ATTR (size),
                    Cache_Type_ALL,
                    TRUE);
    }

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)control,
                    sizeof(RingIOShm_ControlStruct),
                    Cache_Type_ALL,
                    TRUE);
        Cache_wbInv((Ptr)client,
                    sizeof(RingIOShm_Client),
                    Cache_Type_ALL,
                    TRUE);
    }

    GateMP_leave ((GateMP_Handle)object->gateHandle, key);

    if ((status >= 0) && (sendNotification == TRUE)) {
        status = RingIOShm_sendNotify (handle, 0);
    }

    return status;
}

/*
 *  ============= _RingIOShm_writerFlush ==================
 *  Flushes the data buffer
 */
Int
_RingIOShm_writerFlush (RingIO_Handle   handle,
                       Bool            hardFlush,
                       UInt16   *      type,
                       UInt32   *      param,
                       UInt32   *      bytesFlushed)
{
    Int                           status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    RingIOShm_Attr *                   attrBuf = NULL;
    volatile RingIOShm_Client *      client  = NULL;
    volatile RingIOShm_Client *      otherClient = NULL;
    RingIOShm_Obj * object   = NULL;
    UInt32               flushAttrSize = 0;
    UInt32               flushSize = 0;
    Bool                 isFull = FALSE;
    UInt32               attrOffset;
    UInt32               attrSharedAddrSize;
    UInt32               bufSize;
    UInt32               storedAcqStart;
    UInt32               storedAcqAttrStart;


    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    control = object->control;
    GT_assert(curTrace, (control  != NULL));
    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }

    client = object->writer;
    GT_assert(curTrace, (client  != NULL));
    otherClient = object->reader;
    GT_assert(curTrace, (otherClient  != NULL));

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
        Cache_inv((Ptr)otherClient,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
    }


    /* In the case when hardFlush is false, writer can only flush data starting
     * from first attribute (as seen from the reader side). In case there are
     * no attributes, we should return failure.
     */
    if ((control->validAttrSize == 0) && (hardFlush == FALSE)) {
        *type  = RingIOShm_invalidAttr;
        *param = (UInt32) 0;
        status = RingIOShm_E_FAIL;
    }
    else {
        bufSize = RingIOShm_GET_CUR_DATA_END (object);

        if (control->validAttrSize != 0) {
            /* We need to flush out all attributes starting from the last one,
             * till only one is left in the attribute buffer. The last set
             * attribute is present at prevAttrOffset. Note that prevAttrOffset
             * will be valid, as we have already checked that an attribute is
             * present in the attribute buffer
             */
            attrOffset = control->prevAttrOffset;
            do {
                attrBuf = (RingIOShm_Attr *) (  (UInt32)client->pAttrStart
                                          + attrOffset);

                if (object->attrBufCacheUse == TRUE) {
                    Cache_inv((Ptr)attrBuf,
                               sizeof (attrBuf),
                               Cache_Type_ALL,
                               TRUE);
                }

                flushAttrSize += (RingIOShm_SIZEOF_ATTR(attrBuf->size));
                attrOffset     = attrBuf->prevoffset;
            } while (attrBuf->prevoffset != RingIOShm_invalidOffset);

            /* Now we have reached the first attribute that is readable by the
             * reader. We should remove all attributes till this one, and
             * return info from this attribute.
             */
            control->emptyAttrSize += flushAttrSize;
            control->prevAttrOffset = RingIOShm_invalidOffset;
            attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);
            storedAcqAttrStart = client->acqAttrStart;

            /* All the attributes have been removed now */
            client->acqAttrStart =    (  client->acqAttrStart
                                       - (flushAttrSize - client->acqAttrSize)
                                       + attrSharedAddrSize)
                                  %   attrSharedAddrSize;
             /* Do isFull check before change in validAttrSize.
              * Buffer is full only if both conditions are
              * statisfied.
              */
            if (   (client->acqAttrStart   == storedAcqAttrStart)
                && (control->validAttrSize == attrSharedAddrSize)){
                isFull = TRUE;
            }
            control->validAttrSize -= (flushAttrSize - client->acqAttrSize);
            client->acqAttrSize  = 0;


            /* Check if early end needs to be reset in attribute buffer. */
            if (    (control->curAttrBufEnd != control->srPtrAttrBufEnd)
                &&  (_RingIOShm_isOffsetInBetween (control->curAttrBufEnd,
                                                client->acqAttrStart,
                                                storedAcqAttrStart,
                                                isFull))) {
                /* Early end was set in the region to be flushed. The early end
                 * needs to be reset and the size added to emptyAttrSize.
                 */
                control->emptyAttrSize += (     control->srPtrAttrBufEnd
                                           -    control->curAttrBufEnd);
                control->curAttrBufEnd = control->srPtrAttrBufEnd;
            }
        }

        /* Now we have to flush all the data starting attrBuf->offset
         * onwards in case hardFlush is false.
         */
        if (hardFlush == FALSE) {
            if (attrBuf->offset == client->acqStart) {
                /* Check if buffer is full */
                if (control->validSize == bufSize) {
                    flushSize = control->validSize;
                }
                else {
                    flushSize = 0;
                }
            }
            else if (attrBuf->offset < client->acqStart) {
                flushSize = client->acqStart - attrBuf->offset;
            }
            else {
                flushSize =   client->acqStart
                            - attrBuf->offset
                            + control->curBufEnd
                            + 1;
            }
        }
        /* Flush all data in case hardFlush is TRUE */
        else {
            flushSize = control->validSize;
        }

        control->emptySize +=  flushSize;
        control->validSize -=  flushSize;

        storedAcqStart = client->acqStart;
        client->acqStart = (client->acqStart - flushSize + bufSize) % bufSize;

        /* Return isFull is TRUE only when buffer is full */
        isFull = FALSE;
        if (   (flushSize == bufSize)
            && (client->acqStart == storedAcqStart)) {
            isFull = TRUE;
        }

        /* Check if early end needs to be reset. */
        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (_RingIOShm_isOffsetInBetween (control->curBufEnd,
                                            client->acqStart,
                                            storedAcqStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptySize.
             */
            control->emptySize += (control->dataBufEnd - control->curBufEnd);
            control->curBufEnd = control->dataBufEnd;
        }

        *bytesFlushed = flushSize;

        /* Return the attribute info in case hardFlush is FALSE */
        if (hardFlush == FALSE) {
            *type  = attrBuf->type;
            *param = attrBuf->param;
        }
        else {
            *type  = RingIOShm_invalidAttr;
            *param = (UInt32) 0;
        }

        if (flushSize != 0) {
            if (object->controlCacheUse == TRUE) {
                Cache_inv((Ptr)otherClient,
                          sizeof(RingIOShm_Client),
                          Cache_Type_ALL,
                          TRUE);
            }

            if (otherClient->isValid) {
                /* This check is to enable notification in
                 * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
                 * The pre-conditions are
                 * 1) Valid Data Size falls below watermark.
                 */
                if (   (otherClient->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
                   && (control->validSize < otherClient->notifyWaterMark)) {
                    ClientNotifyMgr_enableNotification (otherClient->clientNotifyMgrHandle,
                                                        otherClient->notifyId,
                                                        0);
                }
            }
        }

        /* This check is to disable notification in
         * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Empty Data Size falls above watermark.
         */
        if (   (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
            && (control->emptySize >= client->notifyWaterMark)) {
                ClientNotifyMgr_disableNotification (otherClient->clientNotifyMgrHandle,
                                                     otherClient->notifyId,
                                                     0);
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIOShm_initInstance (object);

        if (object->controlCacheUse == TRUE) {
            Cache_wbInv((Ptr)otherClient,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
            Cache_wbInv((Ptr)client,
                      sizeof(RingIOShm_Client),
                      Cache_Type_ALL,
                      TRUE);
            Cache_wbInv((Ptr)control,
                      sizeof(RingIOShm_ControlStruct),
                      Cache_Type_ALL,
                      TRUE);
        }
    }


    return status;
}

/*
 *  ============= _RingIOShm_readerFlush ==================
 *  Flushes the data buffer
 */
Int
_RingIOShm_readerFlush (RingIO_Handle  handle,
                       Bool          hardFlush,
                       UInt16 *      type,
                       UInt32 *      param,
                       UInt32 *      bytesFlushed)
{
    Int                  status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_Client *  client  = NULL;
    volatile RingIOShm_Client *  otherClient  = NULL;
    volatile RingIOShm_ControlStruct * control = NULL;
    RingIOShm_Attr *        attrBuf = NULL;
    RingIOShm_Obj * object   = NULL;
    Bool                 isFull  = FALSE;
    UInt32               flushSize  = 0;
    UInt32               flushAttrSize;
    UInt32               bufSize;
    UInt32               attrSharedAddrSize;
    UInt32               storedAcqStart;
    UInt32               storedAcqAttrStart;

    GT_assert(curTrace, (handle  != NULL));
    object =  (RingIOShm_Obj *)((RingIO_Object *)handle)->obj ;
    GT_assert(curTrace, (object  != NULL));

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }
    control = object->control;
    client = object->reader;
    otherClient = object->writer;

    if (object->controlCacheUse == TRUE) {
        Cache_inv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
        Cache_inv((Ptr)otherClient,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);
    }


    bufSize     = RingIOShm_GET_CUR_DATA_END (object);
    attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);
    storedAcqStart  = client->acqStart;
    storedAcqAttrStart = client->acqAttrStart;

    /* If there are no attributes, flush out all valid data */
    if (control->validAttrSize == 0) {
        client->acqStart = RingIOShm_ADJUST_MODULO (client->acqStart,
                                          control->validSize,
                                          bufSize);


        /* Return isFull is TRUE only when buffer is full */
        if (   (control->validSize == bufSize)
            && (client->acqStart == storedAcqStart)) {
            isFull = TRUE;
        }

        /* Check if early end needs to be reset. */
        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (_RingIOShm_isOffsetInBetween (control->curBufEnd,
                                            client->acqStart,
                                            storedAcqStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptySize.
             */
            control->emptySize += (control->dataBufEnd - control->curBufEnd);
            control->curBufEnd = control->dataBufEnd;
        }

        control->emptySize += control->validSize;
        *bytesFlushed       = control->validSize;
        flushSize           = control->validSize;
        control->validSize  = 0;
        *type               = RingIOShm_invalidAttr;
        *param              = (UInt32) 0;
    }
    else {
        if (hardFlush == TRUE) {
            flushSize = control->validSize;
            flushAttrSize = control->validAttrSize;
        }
        else {
            attrBuf = _RingIOShm_getFirstAttr (object);
            GT_assert(curTrace, (attrBuf != NULL));

            if (client->acqStart == attrBuf->offset) {
                /* Check if buffer is full */
                if (control->validSize == bufSize) {
                    flushSize = control->validSize;
                }
                else {
                    flushSize = 0;
                }
            }
            else if (client->acqStart < attrBuf->offset) {
                flushSize = attrBuf->offset - client->acqStart;
            }
            else {
                flushSize =   control->curBufEnd
                            - client->acqStart
                            + attrBuf->offset
                            + 1;
            }

            flushAttrSize = 0;
        }

        if (flushSize != 0) {
            client->acqStart    = RingIOShm_ADJUST_MODULO (client->acqStart,
                                                 flushSize,
                                                 bufSize);
            control->validSize -= flushSize;
            control->emptySize += flushSize;
        }

        if (flushAttrSize != 0) {
            client->acqAttrStart   =  RingIOShm_ADJUST_MODULO (client->acqAttrStart,
                                                     flushAttrSize,
                                                     attrSharedAddrSize);
            control->validAttrSize -= flushAttrSize;
            control->emptyAttrSize += flushAttrSize;
        }

        /* Return the attribute info in case hardFlush is FALSE */
        if (hardFlush == FALSE) {
            *type  = attrBuf->type;
            *param = attrBuf->param;
            *bytesFlushed = flushSize;
        }
        else {
            *type  = RingIOShm_invalidAttr;
            *param = (UInt32) 0;
            *bytesFlushed = flushSize;
        }

        /* Return isFull is TRUE only when buffer is full */
        isFull = FALSE;
        if (   (client->acqStart == storedAcqStart)
            && (flushSize == bufSize)){
            isFull = TRUE;
        }
        /* Check if early end needs to be reset. */
        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (_RingIOShm_isOffsetInBetween (control->curBufEnd,
                                            client->acqStart,
                                            storedAcqStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptySize.
             */
            control->emptySize += (control->dataBufEnd - control->curBufEnd);
            control->curBufEnd = control->dataBufEnd;
        }

        /* Check isFull is FALSE when attribute buffer is full.
         * One condition is enough as this is in the else
         * clause for empty attribute buffer check. Only full
         * buffer condition needs to be checked here.
         */
        isFull = FALSE;
        if (client->acqAttrStart == storedAcqAttrStart) {
            isFull = TRUE;
        }
        /* Check if early end needs to be reset in attribute buffer. */
        if (    (control->curAttrBufEnd != control->srPtrAttrBufEnd)
            &&  (_RingIOShm_isOffsetInBetween (control->curAttrBufEnd,
                                            client->acqAttrStart,
                                            storedAcqAttrStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptyAttrSize.
             */
            control->emptyAttrSize += (     control->srPtrAttrBufEnd
                                       -    control->curAttrBufEnd);
            control->curAttrBufEnd = control->srPtrAttrBufEnd;
        }

    }

    /* This check is to enable notification in
     * RingIO_NOTIFICATION_HDWRFIFO_ONCE case:
     * The pre-conditions are
     * 1) Valid Data Size falls below watermark.
     */
    if (flushSize != 0) {
        if(  (client->notifyType == RingIO_NOTIFICATION_HDWRFIFO_ONCE)
           && (control->validSize < client->notifyWaterMark)) {
            ClientNotifyMgr_enableNotification (client->clientNotifyMgrHandle,
                                                client->notifyId,
                                                0);
        }
    }

    /* The notifyFlag for the writer client for RingIO_NOTIFICATION_HDWRFIFO_ONCE
     * is not disabled here because * notification will be missed if the flag is disabled before sending
     * a notification. This will be disabled in function
     * _RingIOShm_checkNotifier which will be called as part of reader flush
     * transactions.
     */

    /* Reset buffer pointers if there is no data in buffer */
    _RingIOShm_initInstance (object);

    if (object->controlCacheUse == TRUE) {
        Cache_wbInv((Ptr)client,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);

        Cache_wbInv((Ptr)otherClient,
                  sizeof(RingIOShm_Client),
                  Cache_Type_ALL,
                  TRUE);

        Cache_wbInv((Ptr)control,
                  sizeof(RingIOShm_ControlStruct),
                  Cache_Type_ALL,
                  TRUE);
    }


    return status;
}

/*
 *  ============= _RingIOShm_isOffsetInBetween ==================
 *  This function checks whether offset lies between start and end
 *  offsets. Note that this includes the start and end offsets
 *  themselves. Note that no check is made to verify whether the
 *  offset lies in the ring buffer.
 */
Bool
_RingIOShm_isOffsetInBetween (UInt32 offset,
                              UInt32 start,
                              UInt32 end,
                              Bool   isFull)
{
    Bool retVal = FALSE;

    /* This condition checks when wraparound
     * has not occurred.
     */
    if (start < end) {
        if ((offset >= start) && (offset <= end)) {
            retVal = TRUE;
        }
    }

    /* This condition checks when wraparound
     * has occurred.
     */
    else if (start > end) {
        if ((offset >= start) || (offset <= end)) {
            retVal = TRUE;
        }
    }

    /* This condition implies that either buffer
     * is full or buffer is empty. The caller
     * knows whether buffer is full or empty and
     * passes it by means of this flag.
     */
    else if (start == end) {
        if (isFull == TRUE) {
            retVal = TRUE;
        }
    }

    return retVal;
}

/*
 *  ============= _RingIOShm_initInstance ==================
 *  This function initializes the RingIO instance identified by the
 *  control handle specified.
 */
Void
_RingIOShm_initInstance (RingIOShm_Obj * object)
{
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * reader  = NULL;
    volatile RingIOShm_Client  * writer  = NULL;


    control = object->control;
    GT_assert(curTrace, (control != NULL));
    reader = object->reader;
    GT_assert(curTrace, (reader != NULL));
    writer = object->writer;
    GT_assert(curTrace, (writer != NULL));

    if (   (control->emptySize == (control->curBufEnd + 1))
        && (control->validSize == 0)
        && (control->emptyAttrSize == (RingIOShm_GET_CUR_ATTR_END (object)))
        && (control->validAttrSize == 0)) {
        control->validSize      = 0;
        control->emptySize      = (UInt32) control->dataBufEnd + 1;
        control->curBufEnd      = (UInt32) control->dataBufEnd;

        control->validAttrSize  = 0;
        control->emptyAttrSize  = (UInt32) control->srPtrAttrBufEnd + 1;
        control->curAttrBufEnd  = (UInt32) control->srPtrAttrBufEnd;

        control->prevAttrOffset = RingIOShm_invalidOffset;

        if (reader->isValid == TRUE) {
            reader->acqStart = 0;
            reader->acqSize  = 0;
            reader->acqAttrStart = 0;
            reader->acqAttrSize  = 0;
        }

        if (writer->isValid == TRUE) {
            writer->acqStart = 0;
            writer->acqSize  = 0;
            writer->acqAttrStart = 0;
            writer->acqAttrSize  = 0;
        }

    }

    return;
}

/*
 *  ============= _RingIOShm_getContiguousDataSize ==================
 *  This function returns the amount of contiguous data available for
 *  the reader to acquire. This is the minimum of
 *  1) the number of bytes available before encountering next attribute
 *  2) the number of bytes before encountering end of buffer
 *  3) total valid data available in the buffer
 *  This should be called only by the reader.
 */
Int
_RingIOShm_getContiguousDataSize (RingIOShm_Obj * object,
                                  UInt32 *           size)
{
    Int                           status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control  = NULL;
    volatile RingIOShm_Client  * client   = NULL;
    UInt32               bufSize  = 0;
    UInt32               bytesTillEndOfBuffer;
    UInt32               nextAcqOffset;
    UInt32               contiguousBytes;
    UInt32               bytesTillNextAttr;
    UInt32               totalSpaceAvailable;
    RingIOShm_Attr *        attrBuf;

    control = object->control;
    GT_assert(curTrace, (control != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert (curTrace, (client != NULL));;

    bufSize  = RingIOShm_GET_CUR_DATA_END (object);

    nextAcqOffset = RingIOShm_ADJUST_MODULO (client->acqStart, client->acqSize, bufSize);
    bytesTillEndOfBuffer = control->curBufEnd - nextAcqOffset + 1;

    totalSpaceAvailable =  bytesTillEndOfBuffer;

    if (control->footBufSize != 0) {
        totalSpaceAvailable +=  control->footBufSize
                              + control->dataSharedAddrSize
                              - (control->curBufEnd + 1);
    }

    contiguousBytes = SYSLINK_min (totalSpaceAvailable, control->validSize);

    attrBuf = (RingIOShm_Attr *)_RingIOShm_getFirstAttr (object);
    if (attrBuf == NULL) {
        *size = contiguousBytes;
    }
    else {
        /*
         * Find number of valid bytes before next attribute
         * 1. Check if the attribute is *not* at the current acquire offset.
         * 2. Check if the buffer is wrapping around and that the attribute
         *    is set at the current 'end' (could be early-end) of the data
         *    buffer. In this case the attribute is *actually* set at end of
         *    buffer but due to early end it is logically set at offset '0'.
         */

        /* This is the wrap around case. Attribute is at the top of the buffer
         * and reader is still at the bottom of the buffer
         */
        if (nextAcqOffset > attrBuf->offset) {
            bytesTillNextAttr = attrBuf->offset + bytesTillEndOfBuffer;
        }
        /* Attribute set at the logical end of the buffer */
        else if ((nextAcqOffset == 0) && (attrBuf->offset == bufSize)) {
            bytesTillNextAttr = 0;
        }
        /* Regular case when attribute buffer is after reader acquire offset */
        else {
            bytesTillNextAttr = attrBuf->offset - nextAcqOffset;
        }

        *size = SYSLINK_min (contiguousBytes, bytesTillNextAttr);

        if (bytesTillNextAttr <= contiguousBytes) {
            status = RingIOShm_S_PENDINGATTRIBUTE;
        }
    }

    return status;
}

/*
 *  ============= _RingIOShm_updateAttributes ==================
 *  This function updates the attribute's offset in early end scenarios.
 */
Void
_RingIOShm_updateAttributes (RingIOShm_Obj * object, UInt32 offset)
{
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client *  client = NULL;
    volatile RingIOShm_Client *      otherClient = NULL;
    RingIOShm_Attr *        attrBuf   = NULL;
    UInt32               size;
    UInt32               attrSharedAddrSize;


    control = object->control;
    GT_assert(curTrace, (control != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
        otherClient = object->reader;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
        otherClient = object->writer;
    }

    GT_assert(curTrace, (client      != NULL));
    GT_assert(curTrace, (otherClient != NULL));

    size = control->validAttrSize;
    attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);

    /* Get the address of first released attribute, if any, that may need to be
     * updated.
     */
    if (size > 0) {
        if (RingIOShm_IS_WRITER (object)) {
            /* For writer, the first released attribute is to be taken. It
             * is the first attribute that can be acquired by the reader.
             */
            otherClient = object->reader;
            attrBuf = (RingIOShm_Attr *) (  (UInt32) client->pAttrStart
                                       + (  (  otherClient->acqAttrStart
                                             + otherClient->acqAttrSize)
                                          % attrSharedAddrSize));
        }
        else {
            /* For reader, the first released attribute is the first that can be
             * acquired by it.
             */
            attrBuf = (RingIOShm_Attr *) (  (UInt32) client->pAttrStart
                                       + (  (  client->acqAttrStart
                                             + client->acqAttrSize)
                                          % attrSharedAddrSize));
        }
    }

    while (size > 0) {
        if (object->attrBufCacheUse == TRUE) {
               Cache_inv((Ptr)attrBuf,
                         sizeof (attrBuf),
                         Cache_Type_ALL,
                         TRUE);
        }

        if (attrBuf->offset == offset) {
            if (offset == 0) {
                attrBuf->offset = (RingIOShm_GET_CUR_DATA_END (object));
            }
            else if (attrBuf->offset == RingIOShm_GET_CUR_DATA_END (object)) {
                attrBuf->offset = 0;
            }

            if (object->attrBufCacheUse == TRUE) {
                   Cache_wbInv((Ptr)attrBuf,
                             sizeof (attrBuf),
                             Cache_Type_ALL,
                             TRUE);
            }
        }

        size -= (RingIOShm_SIZEOF_ATTR(attrBuf->size));

        attrBuf = (RingIOShm_Attr *) (  (UInt32) client->pAttrStart
                                   + (  (  (  ((UInt32) attrBuf )
                                            - ((UInt32) client->pAttrStart))
                                         + attrBuf->size
                                         + sizeof (RingIOShm_Attr))
                                      % attrSharedAddrSize));
    }

    return;
}

/*
 *  ============= _RingIOShm_getFirstAcquiredAttr ==================
 *  This function returns the first attribute that is acquired by
 *  the reader or the writer
 */
RingIOShm_Attr *
_RingIOShm_getFirstAcquiredAttr (RingIOShm_Obj * object)
{
    Int                           status  = RingIOShm_S_SUCCESS;
    RingIOShm_Attr *                   attrBuf = NULL;
    volatile RingIOShm_Client  * client  = NULL;
    UInt32                          attrSharedAddrSize;

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert(curTrace, (client != NULL));

    if (status >= 0) {
        /* Check if attribute buffer has been provided. */
        if (client->pAttrStart != NULL) {
            if (client->acqAttrSize) {
                attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);
                attrBuf = (RingIOShm_Attr *)
                                   (  (UInt32) client->pAttrStart
                                    + ((client->acqAttrStart) % attrSharedAddrSize));
                GT_assert(curTrace, (attrBuf != NULL));
                if (object->attrBufCacheUse == TRUE) {
                       Cache_inv((Ptr)attrBuf,
                                 sizeof (attrBuf),
                                 Cache_Type_ALL,
                                 TRUE);
                }

            }
        }
    }

    return attrBuf;
}

/*
 *  ============= _RingIOShm_getFirstAttr ==================
 *  This function returns the next attribute that can be acquired by
 *  the reader or the writer
 */
RingIOShm_Attr *
_RingIOShm_getFirstAttr (RingIOShm_Obj * object)
{
    Int                               status  = RingIOShm_S_SUCCESS;
    RingIOShm_Attr *                       attrBuf = NULL;
    volatile RingIOShm_ControlStruct     * control = NULL;
    volatile RingIOShm_Client      * client  = NULL;
    UInt32                              attrSharedAddrSize;


    control = object->control;

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert (curTrace, (client != NULL));;

    if (status >= 0) {
       /* Check if attribute buffer has been provided. */
       if (client->pAttrStart != NULL) {
           attrSharedAddrSize = RingIOShm_GET_CUR_ATTR_END (object);

           if (RingIOShm_IS_WRITER (object)) {
               if (client->acqAttrSize != 0) {
                   attrBuf = (RingIOShm_Attr *) (  (UInt32) client->pAttrStart
                                              + (  (client->acqAttrStart)
                                                 % attrSharedAddrSize));
               }
           }
           else {
               if (control->validAttrSize) {
                   attrBuf = (RingIOShm_Attr *) (  (UInt32)client->pAttrStart
                                              + (  (  client->acqAttrStart
                                                    + client->acqAttrSize)
                                                 % attrSharedAddrSize));

                   if (object->attrBufCacheUse == TRUE) {
                       Cache_inv((Ptr)attrBuf,
                                 sizeof (attrBuf),
                                 Cache_Type_ALL,
                                 TRUE);
                   }
               }
           }
       }
    }

    return attrBuf;
}

/*
 *  ============= _RingIOShm_releaseAttribute ==================
 *  This function releases an attribute.
 */
Int
_RingIOShm_releaseAttribute (RingIOShm_Obj * object,
                             RingIOShm_Attr   * attrBuf)
{
    Int                           status  = RingIOShm_S_SUCCESS;
    volatile RingIOShm_ControlStruct * control = NULL;
    volatile RingIOShm_Client  * client  = NULL;

    control = object->control;
    GT_assert(curTrace, (control != NULL));

    if (RingIOShm_IS_WRITER (object)) {
        client = object->writer;
    }
    else if (RingIOShm_IS_READER (object)) {
        client = object->reader;
    }

    GT_assert (curTrace, (client != NULL));;

   if (status >= 0) {

       /* Check if attribute buffer has been provided. */
       if (client->pAttrStart == NULL) {
           status   = RingIOShm_E_FAIL;
       }
       else {
           /* Check if reader has read all valid attributes so far. In this case,
            * when releasing the new attribute, its prevOffset needs to be set
            * to invalid.
            *
            * For writer this condition  implies that if it is the first attribute
            * to be released the attrbuf and there are no valid attributes except
            * current attribute that is to be released.
            *
            * For reader this condition  implies that  there are no valid
            * attributes . Setting prevOffset to RingIOShm_invalidOffset
            * ( This is to allow for easier debugging).
            */
           if (control->validAttrSize == 0) {
               attrBuf->prevoffset = RingIOShm_invalidOffset;
           }

           client->acqAttrStart =  ((  client->acqAttrStart
                                     + (RingIOShm_SIZEOF_ATTR (attrBuf->size)))
                                   % (RingIOShm_GET_CUR_ATTR_END (object)));

           client->acqAttrSize  -= (RingIOShm_SIZEOF_ATTR (attrBuf->size));

           if (RingIOShm_IS_WRITER (object)) {
               control->validAttrSize += (RingIOShm_SIZEOF_ATTR (attrBuf->size));
           }
           else {
               control->emptyAttrSize += (RingIOShm_SIZEOF_ATTR (attrBuf->size));
           }

           if (status >= 0) {
               if (object->attrBufCacheUse == TRUE) {
                   Cache_wbInv((Ptr)attrBuf,
                           RingIOShm_SIZEOF_ATTR(attrBuf->size),
                           Cache_Type_ALL,
                           TRUE);
               }
           }
       }
   }

   return status;
}

/*
 *  ============= _RingIOShm_create ==================
 *  Internal function that populates RingIOShm Object
 */
RingIO_Handle
_RingIOShm_create (RingIO_Handle handle){

    GT_assert (curTrace, (handle != NULL));

    GT_0trace (curTrace, GT_ENTER, "_RingIOShm_create");

    ((RingIO_Object *) handle)->deleteFn           = (IRingIO_deleteFxn) &RingIOShm_delete;
    ((RingIO_Object *) handle)->close              = (IRingIO_closeFxn)  &RingIOShm_close;
    ((RingIO_Object *) handle)->registerNotifier   = (IRingIO_registerNotifierFxn)
                                                    &RingIOShm_registerNotifier;
    ((RingIO_Object *) handle)->unregisterNotifier = (IRingIO_unregisterNotifierFxn)
                                                  &RingIOShm_unregisterNotifier;
    ((RingIO_Object *) handle)->setNotifyType      = (IRingIO_setNotifyTypeFxn)
                                                    &RingIOShm_setNotifyType;
    ((RingIO_Object *) handle)->setWaterMark      = (IRingIO_setWaterMarkFxn)
                                                    &RingIOShm_setWaterMark;
    ((RingIO_Object *) handle)->acquire            = (IRingIO_acquireFxn) &RingIOShm_acquire;
    ((RingIO_Object *) handle)->release            = (IRingIO_releaseFxn) &RingIOShm_release;
    ((RingIO_Object *) handle)->cancel             = (IRingIO_cancelFxn) &RingIOShm_cancel;
    ((RingIO_Object *) handle)->getvAttribute      = (IRingIO_getvAttributeFxn)
                                                       &RingIOShm_getvAttribute;
    ((RingIO_Object *) handle)->setvAttribute      = (IRingIO_setvAttributeFxn)
                                                       &RingIOShm_setvAttribute;
    ((RingIO_Object *) handle)->getAttribute       = (IRingIO_getAttributeFxn)
                                                        &RingIOShm_getAttribute;
    ((RingIO_Object *) handle)->setAttribute       = (IRingIO_setAttributeFxn)
                                                        &RingIOShm_setAttribute;
    ((RingIO_Object *) handle)->flush              = (IRingIO_flushFxn) &RingIOShm_flush;
    ((RingIO_Object *) handle)->notify             = (IRingIO_sendNotifyFxn)&RingIOShm_sendNotify;
    ((RingIO_Object *) handle)->getValidSize       = (IRingIO_getValidSizeFxn)
                                                        &RingIOShm_getValidSize;
    ((RingIO_Object *) handle)->getEmptySize       = (IRingIO_getEmptySizeFxn)
                                                        &RingIOShm_getEmptySize;
    ((RingIO_Object *) handle)->getValidAttrSize   = (IRingIO_getValidAttrSizeFxn)
                                                    &RingIOShm_getValidAttrSize;
    ((RingIO_Object *) handle)->getEmptyAttrSize   = (IRingIO_getEmptyAttrSizeFxn)
                                                    &RingIOShm_getEmptyAttrSize;
    ((RingIO_Object *) handle)->getAcquiredOffset  = (IRingIO_getAcquiredOffsetFxn)
                                                   &RingIOShm_getAcquiredOffset;
    ((RingIO_Object *) handle)->getAcquiredSize    = (IRingIO_getAcquiredSizeFxn)
                                                     &RingIOShm_getAcquiredSize;
    ((RingIO_Object *) handle)->getWaterMark       = (IRingIO_getWaterMarkFxn)
                                                        &RingIOShm_getWaterMark;
    ((RingIO_Object *) handle)->getCliNotifyMgrShAddr       = (IRingIO_getCliNotifyMgrShAddrFxn)
                                                        &RingIOShm_getCliNotifyMgrShAddr;
    ((RingIO_Object *) handle)->getCliNotifyMgrGateShAddr       = (IRingIO_getCliNotifyMgrGateShAddrFxn)
                                                        &RingIOShm_getCliNotifyMgrGateShAddr;
    ((RingIO_Object *) handle)->setNotifyId       = (IRingIO_setNotifyIdFxn)
                                                        &RingIOShm_setNotifyId;
    ((RingIO_Object *) handle)->resetNotifyId       = (IRingIO_resetNotifyIdFxn)
                                                        &RingIOShm_resetNotifyId;

    GT_1trace (curTrace, GT_LEAVE, "_RingIOShm_create", handle);

    return ((RingIO_Handle)handle);

}

