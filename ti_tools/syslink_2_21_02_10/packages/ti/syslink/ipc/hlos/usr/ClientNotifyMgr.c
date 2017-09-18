/*
 *  @file   ClientNotifyMgr.c
 *
 *  @brief      Implements ClientNotifyMgr  functions on user space.
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



/* Standard headers */
#include <ti/syslink/Std.h>

/* Osal And Utils  headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/IGateProvider.h>

/* Utilities & OSAL headers */
#include <ti/syslink/utils/Gate.h>

#include <ti/syslink/inc/_GateMP.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Notify.h>
#include <ti/syslink/inc/usr/_NotifyDefs.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_MultiProc.h>
/* Module level headers */
#include <ti/syslink/inc/ClientNotifyMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr_config.h>
#include <ti/syslink/inc/_ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/usr/ClientNotifyMgrDrv.h>
#include <ti/syslink/inc/ClientNotifyMgrDrvDefs.h>
#include <ti/syslink/inc/_SharedRegion.h>


#define CLIENTNOTIFYMGR_INSTNO_BITS            8u
#define CLIENTNOTIFYMGR_INSTNO_MASK            0xFF
#define CLIENTNOTIFYMGR_INSTNO_BITOFFSET       0u
#define CLIENTNOTIFYMGR_CREATORPROCID_BITS     8u
#define CLIENTNOTIFYMGR_CREATORPROCID_MASK     0xFF
#define CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET 8u

#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

/* Macro to left shift the system key by 16bits and add to the eventNo */
#define CLIENTNOTIFYMGR_ADDSYSKEY(eventNo) \
                            ((UInt32)eventNo + (UInt32)(Notify_SYSTEMKEY << 16))

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/* Structure defining object for the Gate Peterson */
struct ClientNotifyMgr_Object {
    List_Elem                      listElem;
    Ptr                            knlObject;
    /*!< Pointer to the kernel-side ClientNotifyMgr object. */
    UInt32                         Id;
    /*!< Id of the instance*/
    UInt32                         type;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    GateMP_Handle                  gate;
    /*!< Gate handle for protection.UserSpacegate */
    UInt16                          regionId;       /* SharedRegion ID               */
    volatile ClientNotifyMgr_Attrs  *attrs;
    /* Shared control attrs pointer*/
    volatile ClientNotifyMgr_NotifyParamInfo **notifyInfoObj;
    /* Shared notifyInfo Objects pointer*/
    Bool                            cacheEnabled;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    instance control structures
     */
    UInt32                          minAlign;
    /*!< Alignment of the control structure buffers */
    UInt32                          numNotifyEntries;
    Ptr                            top;
    /* Pointer to the top Object */
    ClientNotifyMgr_Params         params;
};

/*!
 *  @brief  FrameQ Module state object on user space
 */
typedef struct ClientNotifyMgr_ModuleObject_tag {
    UInt32              setupRefCount;
    /*!< Reference count for number of times setup/destroy were called in this
         process. */
    List_Object         objList;
    /* List holding created/opened  objects on this process*/
    IGateProvider_Handle listLock;
    /* Handle to lock for protecting objList */
    List_Object         eventListenerList[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances];
    /* eventListener info for all the instances and its clients */

    ClientNotifyMgr_Config cfg;
    /* Current config */
    UInt32                      numProcessors;
    /*!< Number of supported processors including local processor.*/

    UInt32                           instRefCount[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances];
    Bool
       isEventRegistered[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances][MultiProc_MAXPROCESSORS];
    Ptr callBackCtxPtrs [MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances][MultiProc_MAXPROCESSORS];
} ClientNotifyMgr_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    ClientNotifyMgr_module_obj
 *
 *  @brief  ClientNotifyMgr state object variable  in process
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
ClientNotifyMgr_ModuleObject ClientNotifyMgr_module_obj =
{
    .setupRefCount = 0
};

/*!
 *  @var    ClientNotifyMgr_module
 *
 *  @brief  Pointer to ClientNotifyMgr_module_obj .
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
ClientNotifyMgr_ModuleObject* ClientNotifyMgr_module = &ClientNotifyMgr_module_obj;


/*==============================================================================
 * API definations
 *==============================================================================
 */

/*!
 *  @brief  API to find out the shared memory required for the  module.
 *
 */
UInt32
ClientNotifyMgr_sharedMemModReq()
{
    UInt32                      reqSize    =  0 ;

    GT_0trace (curTrace, GT_ENTER, "ClientNotifyMgr_sharedMemModReq");

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_sharedMemModReq", reqSize);
    /*!< @retval positive value i.e size of shared memory required for module
     * setup
     */
    return (reqSize);
}

/*!
 * @brief  Function find out the shared memory required for the instance.
 * @param  params Instance create params
 */
UInt32
ClientNotifyMgr_sharedMemReq( const ClientNotifyMgr_Params* params )
{
    UInt32  totalSize = 0;
    SizeT   minAlign;
    UInt16  regionId;

    GT_0trace (curTrace, GT_ENTER, "ClientNotifyMgr_sharedMemReq");


    GT_assert (curTrace,(NULL != params));

    if (params->sharedAddr != NULL) {
        regionId = SharedRegion_getId(params->sharedAddr);
    }
    else {
        regionId = params->regionId;
    }
    GT_assert(curTrace, (regionId != SharedRegion_INVALIDREGIONID));

    minAlign = Memory_getMaxDefaultTypeAlign();
    if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize(regionId);
    }

    /* Get the shared attributes size */
    totalSize = ROUND_UP(sizeof (ClientNotifyMgr_Attrs), minAlign);


    totalSize += ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo), minAlign) *
                          params->numNotifyEntries;

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_sharedMemReq", totalSize);
    return (totalSize) ;


}


/*!
 *  @brief      CallBack function of the ClientNotifyMgr which internally
 *              generates call backs for the registered clients on the local
 *              processor.
 *
 *  @param      procId   Remote processor id which send the event.
 *  @param      eventNo  Gate for protection.
 *  @param      arg      ClientNotifyMgr instance handle .
 *  @param      payload  PayLoad sent by sender of this notification.
 */
Void
ClientNotifyMgr_callBack(UInt16 procId,
                         UInt16 lineId,
                         UInt32 eventNo,
                         UArg    arg,
                         UInt32 notifyPayload)
{
    List_Elem                                *elem = NULL;
    Bool                                      callAppCallBack = TRUE;
    Bool                                      waitOnSubNotify = FALSE;
    volatile ClientNotifyMgr_Attrs           *pAttrs ;
    volatile ClientNotifyMgr_NotifyParamInfo *notifyInfoObj;
    ClientNotifyMgr_FnCbck           callBackFxn;
    UInt32                           entryId;
    UInt16                           instId;
    UInt32                           id;
    UInt32                           i;
    UInt32                           j;
    Ptr                              cbContext;
    ClientNotifyMgr_callbackCtx     *cliCallbackCtx;
    IArg                             key;
    UInt32                           payload;
    UInt16                           creProcId;
    UInt16                           instNo;
    UInt32                           size;

    GT_4trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_callBack",
               procId,
               eventNo,
               arg,
               notifyPayload);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors()));
    GT_assert (curTrace, (arg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (procId >= MultiProc_getNumProcessors()) {
            /*! Notifydriver passed invalid procId the call back */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_callback",
                                 ClientNotifyMgr_E_INVALIDARG,
                                 "Config passed is NULL!");
        }
        else if (arg == NULL) {
            /* Call back context pointer passed to this is null */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_callBack",
                                 ClientNotifyMgr_E_INVALIDARG,
                                 "Config passed is NULL!");
        }
        else {
#endif
            cliCallbackCtx =  (ClientNotifyMgr_callbackCtx*)arg;

            pAttrs         =  cliCallbackCtx->attrs;

            entryId =  notifyPayload & 0xFFFF;
            payload =  ( notifyPayload >> 16) & 0xFFFF;

            //key = GateMP_enter (cliCallbackCtx->gate) ;

            if (cliCallbackCtx->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)pAttrs,
                           sizeof(ClientNotifyMgr_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }

            instId = pAttrs->entryId & 0xFFFF;

            if (instId == entryId) {
               /* Find out the creator procid */
                creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                            & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
                /* Find out the instance no of the instance cretare on
                 * creProcId
                 */
                instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                         & CLIENTNOTIFYMGR_INSTNO_MASK);

                GT_assert (curTrace,
                          (instNo < ClientNotifyMgr_maxInstances));

                for ((elem = List_next ((List_Handle)
                             &ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                     NULL));
                     elem != NULL;
                     elem = List_next ((List_Handle)
                             &ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                             elem)) {
                    key = GateMP_enter (cliCallbackCtx->gate) ;
                    id =((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId;
                    notifyInfoObj = (ClientNotifyMgr_NotifyParamInfo*)(
                                     (UInt32)cliCallbackCtx->notifyInfoObjBaseAddr +
                                     (ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo),
                                               cliCallbackCtx->minAlign)
                                       * id));

                    if (cliCallbackCtx->cacheEnabled == TRUE) {
                        Cache_inv ((Ptr)notifyInfoObj,
                                   sizeof(ClientNotifyMgr_NotifyParamInfo),
                                   Cache_Type_ALL,
                                   TRUE);
                    }

                    if (notifyInfoObj->procId == MultiProc_self()) {
                        if (notifyInfoObj->forceNotify == TRUE) {
                            /* Force notification received. Call the call back directly
                             */
                            notifyInfoObj->forceNotify = FALSE;
                            if (cliCallbackCtx->cacheEnabled == TRUE) {
                                Cache_wbInv (
                                           (Ptr)notifyInfoObj,
                                            sizeof(ClientNotifyMgr_NotifyParamInfo),
                                            Cache_Type_ALL,
                                            TRUE);
                            }
                            callBackFxn =
                                ((ClientNotifyMgr_eventListenerInfo*)elem)->cbckFxn;
                            cbContext =
                              ((ClientNotifyMgr_eventListenerInfo*)elem)->cbContext;
                            if (callBackFxn != NULL) {
                               /* Call the call back function */
                                callBackFxn ((Ptr)notifyInfoObj->clientHandle,
                                             cbContext,
                                             payload);
                            }
                        }
                        else  {
                            /* Check if client is waiting on multiple events */
                            if (   notifyInfoObj->waitOnMultiNotifyFlags
                                == TRUE) {
                                for (i = 0;
                                     i < pAttrs->numSubNotifyEntries;
                                     i++) {
                                    size = sizeof(notifyInfoObj->
                                                   bitFlag_notifySubEntries[0]);
                                    j = i / (size * 8u);
                                    /* Check  if clients is waiting on this sub
                                     * notify entry
                                     */
                                    waitOnSubNotify =
                                    (  notifyInfoObj->bitFlag_notifySubEntries[j]
                                     & ( 1 << ( i % (size * 8u))));
                                    if (waitOnSubNotify == TRUE) {
                                        /* Client is waiting on this entry */
                                        if (notifyInfoObj->eventCount[i] == 0) {
                                            /* No need to call the call back. As
                                             * this sub notify event entry is not
                                             * set.
                                             */
                                             callAppCallBack = FALSE;
                                        }
                                    }
                               }
                               if (callAppCallBack == TRUE) {
                                    for (i = 0;
                                         i < pAttrs->numSubNotifyEntries;
                                         i++) {
                                        if (   notifyInfoObj->eventCount[i]
                                            > 0) {
                                            notifyInfoObj->eventCount[i]--;
                                        }
                                        size = sizeof(
                                        notifyInfoObj->bitFlag_notifySubEntries[0]);
                                        j = i / (size * 8u);
                                        /* Reset the bit in bitFlag_notifySubEntries */
                                        notifyInfoObj->bitFlag_notifySubEntries[j] =
                                        (notifyInfoObj->bitFlag_notifySubEntries[j]
                                         &  (~( 1 << ( i % (size *8)))));

                                    }
                                    notifyInfoObj->waitOnMultiNotifyFlags =
                                                                          FALSE;

                                    if (   cliCallbackCtx->cacheEnabled
                                        == TRUE) {
                                        Cache_wbInv (
                                               (Ptr)notifyInfoObj,
                                               sizeof(ClientNotifyMgr_NotifyParamInfo),
                                               Cache_Type_ALL,
                                               TRUE);
                                    }
                                    callBackFxn =
                                           ((ClientNotifyMgr_eventListenerInfo*)
                                                                 elem)->cbckFxn;
                                    cbContext =
                                      ((ClientNotifyMgr_eventListenerInfo*)
                                                               elem)->cbContext;
                                    if (callBackFxn != NULL) {
                                       /* Call the call back function */
                                        callBackFxn (
                                            (Ptr)notifyInfoObj->clientHandle,
                                             cbContext,
                                             payload);
                                    }
                               }
                            }/*for (i = 0; i < pAttrs->numSubNotifyEntries; i++)*/
                            else {
                                for (i = 0;
                                     i < pAttrs->numSubNotifyEntries;
                                     i++) {
                                    if (   notifyInfoObj->eventCount[i]
                                        > 0) {
                                        notifyInfoObj->eventCount[i]--;

                                        if (   cliCallbackCtx->cacheEnabled
                                            == TRUE) {
                                            Cache_wbInv (
                                            (Ptr)notifyInfoObj,
                                            sizeof(ClientNotifyMgr_NotifyParamInfo),
                                            Cache_Type_ALL,
                                            TRUE);
                                        }
                                        callBackFxn =
                                        ((ClientNotifyMgr_eventListenerInfo*)
                                                                 elem)->cbckFxn;
                                        cbContext =
                                        ((ClientNotifyMgr_eventListenerInfo*)
                                                               elem)->cbContext;
                                       if (callBackFxn != NULL) {
                                           /* Call the call back function */
                                            callBackFxn (
                                            (Ptr)notifyInfoObj->clientHandle,
                                                 cbContext,
                                                 payload);
                                        }
                                    }
                                } /*for (i = 0; i < pAttrs->numSubNotifyEntries; i++)*/
                            }
                        }
                    }

                    GateMP_leave (cliCallbackCtx->gate, key) ;

                }  /* for ((elem = List_next ( ... */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgr_callBack");
}


/*=============================================================================
 *  Exported API
 *=============================================================================
 */

/*
 *  @brief  Function to get the default Module config parameters.
 *
 *  @param  config Configuration values.
 */
Void
ClientNotifyMgr_getConfig (ClientNotifyMgr_Config *config)
{
    Int32               status = ClientNotifyMgr_S_SUCCESS;
    ClientNotifyMgrDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_getConfig",config);

    GT_assert (curTrace, (config != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (config == NULL) {
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_getConfig",
                             status,
                             "Argument of type (ClientNotifyMgr_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Temporarily open the handle to get the configuration. */
        status = ClientNotifyMgrDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            /*! @retval ClientNotifyMgr_E_INVALIDSTATE  Failed to open driver handle.
             */
            status = ClientNotifyMgr_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            cmdArgs.args.getConfig.config = config;
            status = ClientNotifyMgrDrv_ioctl (CMD_CLIENTNOTIFYMGR_GETCONFIG,
                                               &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (
                                 curTrace,
                                 GT_4CLASS,
                                 "CLIENTNOTIFYMGR_getConfig",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");

            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        ClientNotifyMgrDrv_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgr_getConfig");
}


/*
 *  @brief  Function to setup(initialize) the ClientNotifyMgr module.
 *
 *  @param  config Config param structure.
 *  @sa         ClientNotifyMgr_destroy
 */
Int32
ClientNotifyMgr_setup (ClientNotifyMgr_Config * config)
{
    Int                           status = ClientNotifyMgr_S_SUCCESS;
    ClientNotifyMgrDrv_CmdArgs    cmdArgs;
    UInt32                        i;
    UInt32                        j;
    UInt32                        k;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_setup", config);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (config == NULL) {
            /*! @retval ClientNotifyMgr_E_INVALIDARG config passed is NULL */
            status = ClientNotifyMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_setup",
                                 status,
                                 "Config passed is NULL!");
     }
     else {
#endif
        /* TBD: Protect from multiple threads. */
        ClientNotifyMgr_module->setupRefCount++;
        /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
        if (ClientNotifyMgr_module->setupRefCount > 1) {
            /*! @retval ClientNotifyMgr_S_ALREADYSETUP Success: ClientNotifyMgr
             *  module has been already setup in this process
             */
            status = ClientNotifyMgr_S_ALREADYSETUP;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "ClientNotifyMgr module has been already setup"
                       "in this process.\n"
                       " RefCount: [%d]\n",
                       ClientNotifyMgr_module->setupRefCount);
        }
        else {
            /* Open the driver handle. */
            status = ClientNotifyMgrDrv_open ();
    #if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ClientNotifyMgr_setup",
                                     status,
                                     "Failed to open driver handle!");
            }
            else {
    #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                /* Intialize the module's call back handling information */
                for (k = 0; k < MultiProc_MAXPROCESSORS;k++ ) {
                    for (i = 0; i < ClientNotifyMgr_maxInstances;i++ ) {
                        for (j = 0; j < MultiProc_MAXPROCESSORS;j++ ) {
                            ClientNotifyMgr_module->
                                         isEventRegistered[k][i][j] = FALSE;
                        }
                        ClientNotifyMgr_module->instRefCount[k][i] = 0;
                    }
                }

                ClientNotifyMgr_module->numProcessors = config->numProcessors;

                cmdArgs.args.setup.config = (ClientNotifyMgr_Config *) config;

                status = ClientNotifyMgrDrv_ioctl (CMD_CLIENTNOTIFYMGR_SETUP,
                                                   &cmdArgs);
    #if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ClientNotifyMgr_setup",
                                         status,
                                         "API (through IOCTL) failed on"
                                         "kernel-side!");
                }
                else {
    #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    Memory_copy ((Ptr) &ClientNotifyMgr_module->cfg,
                                 (Ptr) config,
                                 sizeof (ClientNotifyMgr_Config));
                    /* Initialize the event listener info in this process */
                    for (i = 0; i < MultiProc_MAXPROCESSORS; i++) {
                        for (j = 0; j < ClientNotifyMgr_maxInstances; j++) {
                            List_construct (
                               &ClientNotifyMgr_module->eventListenerList[i][j],
                               NULL);
                        }
                    }
     #if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
    #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
     }
#endif
    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_setup", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS Operation successful */
    return (status);
}

/*!
 *  @brief      Function to destroy the ClientNotifyMgr module.
 *
 *  @sa         ClientNotifyMgr_setup
 */
Int32
ClientNotifyMgr_destroy (void)
{
    Int                     status = ClientNotifyMgr_S_SUCCESS;
    ClientNotifyMgrDrv_CmdArgs       cmdArgs;
    UInt32                           i;
    UInt32                           j;

    GT_0trace (curTrace, GT_ENTER, "ClientNotifyMgr_destroy");

    /* TBD: Protect from multiple threads. */
    ClientNotifyMgr_module->setupRefCount--;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (ClientNotifyMgr_module->setupRefCount > 1) {
        /*! @retval ClientNotifyMgr_S_ALREADYSETUP Success: ClientNotifyMgr module has been
                                           already setup in this process */
        status = ClientNotifyMgr_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "ClientNotifyMgr module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   ClientNotifyMgr_module->setupRefCount);
    }
    else {
        status = ClientNotifyMgrDrv_ioctl (CMD_CLIENTNOTIFYMGR_DESTROY,
                                           &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_destroy",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Close the driver handle. */
        ClientNotifyMgrDrv_close ();
        for (i = 0; i < MultiProc_MAXPROCESSORS; i++) {
            for (j = 0; j < ClientNotifyMgr_maxInstances; j++) {
                List_destruct (
                              &ClientNotifyMgr_module->eventListenerList[i][j]);
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_destroy", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS Operation successful */
    return (status);
}


/*
 * @brief Function to get the default instance parameters
 *
 * @param handle handle of the instance. if specified as NULL it returns the
 *               default config params.
 * @param params  Instance create  params
 */
Void ClientNotifyMgr_Params_init (ClientNotifyMgr_Params *params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                          status = ClientNotifyMgr_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    ClientNotifyMgrDrv_CmdArgs   cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (ClientNotifyMgr_module->setupRefCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_create",
                             ClientNotifyMgr_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_Params_init",
                             status,
                             "Argument of type (ClientNotifyMgr_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ClientNotifyMgrDrv_ioctl (CMD_CLIENTNOTIFYMGR_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgrBufMgr_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgr_Params_init");
}

/*!
 * @brief  Function to create a ClientNotifyMgr instance
 *
 * @param  instParams Instance param structure.
 */
ClientNotifyMgr_Handle
ClientNotifyMgr_create (ClientNotifyMgr_Params * params)
{
    Int32                           status          = ClientNotifyMgr_S_SUCCESS;
    UInt32                          i               = 0;
    ClientNotifyMgr_Handle          handle          = NULL;
    ClientNotifyMgr_Object         *obj             = NULL;
    UInt32                          addrOffset      = 0;
    UInt32                          sharedAddr;
    volatile ClientNotifyMgr_Attrs  *pAttrs;
    ClientNotifyMgrDrv_CmdArgs      cmdArgs;
    GateMP_Handle                   gate;
    UInt16                          index;
    ClientNotifyMgr_callbackCtx    *cliCallbackCtx;
    UInt16                          lprocId;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;
    Ptr                              virtAddr;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ( NULL == params) {
         /*! @retval ClientNotifyMgr_E_INVALIDARG  Params passed is NULL
          */
         status = ClientNotifyMgr_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "ClientNotifyMgr_create",
                              status,
                              "Params pointer is NULL!");

    }
    else if (   ( params->openFlag == FALSE)
             && ( params->sharedAddr == (UInt32)NULL) ) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_create",
                             ClientNotifyMgr_E_INVALIDARG,
                             "Please provided a valid shared region "
                             "address!");
    }
    else if (   ( params->openFlag == TRUE)
             && (   (params->sharedAddr == (UInt32)NULL)
                 && (params->name == NULL))) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_create",
                             ClientNotifyMgr_E_INVALIDARG,
                             "Please provided a valid shared region "
                             "address or instance name!");
    }
    else if (  (  params->sharedAddrSize
                < ClientNotifyMgr_sharedMemReq (params))
             && (params->openFlag == FALSE)) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_create",
                             ClientNotifyMgr_E_INVALIDARG,
                             "Shared memory size is less than required!");
    }
    else {
#endif
        if (params->name != NULL) {
            cmdArgs.args.create.nameLen = String_len (params->name) + 1;
        }
        else {
            cmdArgs.args.create.nameLen = 0;
        }

        /* Translate shared address to SRPtr */
        cmdArgs.args.create.params = (ClientNotifyMgr_Params *) params;

        /* Translate sharedAddr to SrPtr. */
        if (params->sharedAddr != NULL) {
            sharedAddr = (UInt32)params->sharedAddr;
            index      = SharedRegion_getId (params->sharedAddr);
            cmdArgs.args.create.sharedAddrSrPtr =
                                        SharedRegion_getSRPtr ((Ptr)sharedAddr,
                                                                 index);
            GT_assert (curTrace,
                       (    cmdArgs.args.create.sharedAddrSrPtr
                        !=  SharedRegion_INVALIDSRPTR));
        }

        gate = params->gate;
#if defined(__QNX__)
        cmdArgs.args.create.gateMPSrPtr = GateMP_getSharedAddr(gate);
#else
        cmdArgs.args.create.knlLockHandle = GateMP_getKnlHandle(gate);
#endif

//#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
//#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        ClientNotifyMgrDrv_ioctl (CMD_CLIENTNOTIFYMGR_CREATE, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_create",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Allocate memory for the handle */
            handle = (ClientNotifyMgr_Object*) Memory_calloc (NULL,
                                                sizeof (ClientNotifyMgr_Object),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (handle == NULL) {
                /*! @retval NULL Memory allocation failed for handle */
                status = ClientNotifyMgr_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ClientNotifyMgr_create",
                                     status,
                                     "Memory allocation failed for handle!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                obj = (ClientNotifyMgr_Object  *)handle;
                obj->knlObject = cmdArgs.args.create.handle;
                /* Incase of open, and shared addr is not provided by user
                 * ioctl call updates the cmdArgs.args.create.params->sharedAddr
                 * with shared control address values in the form of SRPtr
                 */
                obj->minAlign = Memory_getMaxDefaultTypeAlign();

                virtAddr = SharedRegion_getPtr(
                                       cmdArgs.args.create.sharedAddrSrPtr);
                obj->regionId = SharedRegion_getId(virtAddr);

                if (SharedRegion_getCacheLineSize(obj->regionId) > obj->minAlign) {
                    obj->minAlign = SharedRegion_getCacheLineSize(obj->regionId);
                }

                obj->attrs =  (ClientNotifyMgr_Attrs*) virtAddr;
                pAttrs     = obj->attrs;
                addrOffset +=  ROUND_UP(sizeof (ClientNotifyMgr_Attrs),
                                        obj->minAlign);
                /* Update the readers information */
                obj->notifyInfoObj = Memory_alloc(NULL,
                                                  sizeof(UInt32) * pAttrs->numNotifyEntries,
                                                  0,
                                                  NULL);
#if !defined (SYSLINK_BUILD_OPTIMIZE)
                if (obj->notifyInfoObj == NULL) {
                    status = ClientNotifyMgr_E_MEMORY;
                     /*! @retval ClientNotifyMgr_E_MEMORY
                      * Memory alloc failed.
                      */
                     status = ClientNotifyMgr_E_MEMORY;
                     GT_setFailureReason (curTrace,
                                          GT_4CLASS,
                                          "ClientNotifyMgr_create",
                                          status,
                                          "Memory alloc  failed!");
                }
                else {
#endif
                    /*Populate the notifyInfo entries */
                    for (i = 0; i < pAttrs->numNotifyEntries; i++) {
                        obj->notifyInfoObj[i] =
                            (ClientNotifyMgr_NotifyParamInfo *)(virtAddr + addrOffset);

                        addrOffset += ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo),
                                               obj->minAlign);
                    }
                    obj->numNotifyEntries = pAttrs->numNotifyEntries;
                    obj->Id   = cmdArgs.args.create.Id;
                    obj->gate = gate;
                    obj->cacheEnabled = params->cacheFlags;

                    instId = pAttrs->entryId & 0xFFFF;
                    /* Find out the creator procid */
                    creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                                & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
                    instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                          & CLIENTNOTIFYMGR_INSTNO_MASK);
                    GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

                    lprocId = MultiProc_self();

                    if (    ClientNotifyMgr_module->instRefCount[creProcId][instNo]
                        == 0) {
                        ClientNotifyMgr_module->instRefCount[creProcId][instNo] = 1;
                        for (i = 0; i < MultiProc_getNumProcessors ();
                            i++ ) {
                            /* Register the cli cal back funtion with the valid
                            * notify drivr handles. and also with the loop  back
                            * driver for local processor notifications.
                                */
                            if ( Notify_intLineRegistered (i,CLIENTNOTIFYMGR_INT_LINEID)
                                || (i == lprocId ) ) {
                                if (ClientNotifyMgr_module->
                                        isEventRegistered[creProcId][instNo][i]
                                != TRUE){
                                cliCallbackCtx = (ClientNotifyMgr_callbackCtx *)
                                    Memory_calloc(NULL,
                                                sizeof(ClientNotifyMgr_callbackCtx),
                                                0,
                                                NULL);
#if !defined (SYSLINK_BUILD_OPTIMIZE)
                                    if (cliCallbackCtx == NULL) {
                                        status = ClientNotifyMgr_E_MEMORY;
                                        GT_setFailureReason (
                                                    curTrace,
                                                    GT_4CLASS,
                                                    "ClientNotifyMgr_create",
                                                    status,
                                                    "Memory allocation failed"
                                                    "ClientNotifyMgr_callbackCtx!");
                                    }
                                    else  {
#endif
                                        cliCallbackCtx->gate = obj->gate;
                                        cliCallbackCtx->cacheEnabled =
                                                           obj->cacheEnabled;
                                        cliCallbackCtx->attrs = obj->attrs;
                                        cliCallbackCtx->notifyInfoObjBaseAddr = (Ptr)
                                                          obj->notifyInfoObj[0];
                                        cliCallbackCtx->minAlign = obj->minAlign;

                                        ClientNotifyMgr_module->
                                            callBackCtxPtrs[creProcId][instNo][i] =
                                                                    cliCallbackCtx;
                                        if(obj->cacheEnabled == TRUE) {
                                            Cache_inv ((Ptr)pAttrs,
                                                    sizeof(ClientNotifyMgr_Attrs),
                                                    Cache_Type_ALL,
                                                    TRUE);
                                        }
                                        /* On HLOS side , Handle  needs to be passed as
                                         * null for loop back driver
                                         */
                                        status = Notify_registerEvent (
                                            i,
                                            CLIENTNOTIFYMGR_INT_LINEID, /* Interrupt lineId*/
                                            CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                            ClientNotifyMgr_callBack,
                                            (Ptr)cliCallbackCtx);
#if !defined (SYSLINK_BUILD_OPTIMIZE)
                                        }
#endif

                                }
                                ClientNotifyMgr_module->
                                isEventRegistered[creProcId][instNo][i] = TRUE;
                            }
                        }
                    }
                    else {
                        ClientNotifyMgr_module->instRefCount[creProcId][instNo] += 1;
                    }

#if !defined (SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
    }
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* Failed to crate /open the instance. Do clean up*/
    if (status < 0) {
        if (handle != NULL) {
            Memory_free (NULL, handle, sizeof (ClientNotifyMgr_Object));
        }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_create", handle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */

    return (handle);
}

/*!
 *  @brief      Deletes a instance of ClientNotifyMgr module.
 *
 *  @param      gpHandle  Handle to previously created instance.
 *
 *  @sa         ClientNotifyMgr_create, ClientNotifyMgr_open, ClientNotifyMgr_close
 */
Int32
ClientNotifyMgr_delete (ClientNotifyMgr_Handle * gpHandle)
{
    Int32                      status = ClientNotifyMgr_S_SUCCESS;
    volatile ClientNotifyMgr_Attrs     *pAttrs;
    ClientNotifyMgrDrv_CmdArgs cmdArgs;
    UInt32                     id;
//    IArg                       key;
    UInt32                     i;
    UInt16                     lprocId;
    UInt16                     instId;
    UInt16                     instNo;
    UInt16                     creProcId;
    UInt32                     numNotifyEntries;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_delete", gpHandle);

    GT_assert (curTrace, (gpHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (ClientNotifyMgr_module->setupRefCount == 0) {
        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Modules is invalidstate*/
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_delete",
                             status,
                             "Modules is invalidstate!");
    }
    else if (gpHandle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_delete",
                             status,
                             "handle passed is NULL!");
    }
    else if (*gpHandle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG *handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_delete",
                             status,
                             "*handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        id = (*(gpHandle))->Id;
        numNotifyEntries = (*(gpHandle))->attrs->numNotifyEntries;
        cmdArgs.args.deleteInstance.handle = (*gpHandle)->knlObject;
        status = ClientNotifyMgrDrv_ioctl (CMD_CLIENTNOTIFYMGR_DELETE, &cmdArgs);
        lprocId = MultiProc_self();

        instId = id & 0xFFFF;
        /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                      & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);
        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

//        key = IGateProvider_enter (ClientNotifyMgr_module->listLock);

        ClientNotifyMgr_module->instRefCount[creProcId][instNo] -= 1;

         if (ClientNotifyMgr_module->instRefCount[creProcId][instNo] == 0) {
            /* Unregister event number with the notify drivers
             * including with the local processor
             */
            for (i = 0; i < MultiProc_getNumProcessors (); i++ ) {
                if (Notify_intLineRegistered (i, CLIENTNOTIFYMGR_INT_LINEID)
                     ||(lprocId == i)) {
                    if (ClientNotifyMgr_module->
                                isEventRegistered[creProcId][instNo][i]
                    == TRUE) {
                        pAttrs    = (*(gpHandle))->attrs;
                        if((*gpHandle)->cacheEnabled == TRUE) {
                            Cache_inv ((Ptr)pAttrs,
                                       sizeof(ClientNotifyMgr_Attrs),
                                       Cache_Type_ALL,
                                       TRUE);
                        }
                        Notify_unregisterEvent (i,
                                                CLIENTNOTIFYMGR_INT_LINEID,
                                                CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                                ClientNotifyMgr_callBack,
                                               (UArg)ClientNotifyMgr_module->
                                        callBackCtxPtrs[creProcId][instNo][i]);
                        Memory_free (NULL,
                                     ClientNotifyMgr_module->
                                           callBackCtxPtrs[creProcId][instNo][i],
                                     sizeof (ClientNotifyMgr_callbackCtx));
                        ClientNotifyMgr_module->
                                    isEventRegistered[creProcId][instNo][i] =
                                 FALSE;
                    }
                }
            }
         }

//        IGateProvider_leave (ClientNotifyMgr_module->listLock, key);
        if ((* gpHandle)->notifyInfoObj != NULL) {
            Memory_free (NULL,
                        (* gpHandle)->notifyInfoObj,
                        sizeof(UInt32) * numNotifyEntries);
        }

        Memory_free (NULL,
                    (*gpHandle),
                    sizeof (ClientNotifyMgr_Object));
        *gpHandle = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_delete", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS Operation successful */
    return (status);
}


/*!
 * @brief  Function to open a ClientNotifyMgr instance
 *
 * @param  instParams Instance param structure.
 */
Int32
ClientNotifyMgr_open (ClientNotifyMgr_Handle *pHandle,
                      String                  name,
                      Ptr                     sharedAddr,
                      UInt32                  cacheFlags,
                      GateMP_Handle             gate)
{
    Int32                           status = ClientNotifyMgr_S_SUCCESS;
    ClientNotifyMgr_Params          params;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_open", pHandle);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ( NULL == pHandle) {
         status = ClientNotifyMgr_E_INVALIDARG;
         /*! @retval ClientNotifyMgr_E_INVALIDARG  pHandle pointer is NULL
          */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "ClientNotifyMgr_open",
                              ClientNotifyMgr_E_INVALIDARG,
                              "pHandle pointer is NULL!");

    }
    else if ((name == NULL) && (sharedAddr == NULL)) {
         status = ClientNotifyMgr_E_INVALIDARG;
         /*! @retval ClientNotifyMgr_E_INVALIDARG  name and sharedAddr
          * provided are NULL.
          */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_open",
                             ClientNotifyMgr_E_INVALIDARG,
                             "name or sharedAddr is required.Both are passed"
                             "as null!");
    }
    else {
#endif
        params.name          = name;
        params.gate          = gate;
        params.sharedAddr = sharedAddr;
        params.cacheFlags    = cacheFlags;
        params.openFlag      = TRUE;

        *pHandle = ClientNotifyMgr_create(&params);
        if (*pHandle == NULL) {
            status = ClientNotifyMgr_E_FAIL;
        }

#if !defined (SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_open", pHandle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */

    return (status);

}

/*
 * @brief Function to close the dynamically opened instance
 *
 * @param Instance handle.
 */
Int32
ClientNotifyMgr_close(ClientNotifyMgr_Handle *pHandle)
{
    Int32 status = ClientNotifyMgr_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_close", pHandle);

    GT_assert(curTrace,(NULL != pHandle));
    GT_assert(curTrace,(NULL != *pHandle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (pHandle == NULL) {
         /*! @retval ClientNotifyMgr_E_INVALIDARG  pHandle pointer is NULL
          */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "ClientNotifyMgr_close",
                              ClientNotifyMgr_E_INVALIDARG,
                              "pHandle pointer is NULL!");
    }
    else {
#endif
        status = ClientNotifyMgr_delete (pHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_close", status);
    /* !< @retval ClientNotifyMgr_S_SUCCESS  if handle is closed successfully */
    return (status);
}

/*!
 *  @brief Function to register notification with the ClientNotifyMgr instance.
 *
 *  @param pRegParams  client notification registration parameters.
 *  @param id  Pointer to hold the identifier returned after successfully
 *             returning from the registerClient API.
 *
 */
Int32
ClientNotifyMgr_registerClient(ClientNotifyMgr_Handle handle,
                               ClientNotifyMgr_registerNoitifyParams  *pRegParams,
                               UInt32                 *id)
{
    Int32                              status        = ClientNotifyMgr_S_SUCCESS;
    UInt32                             i             = 0    ;
    Bool                               found         = FALSE;
    volatile ClientNotifyMgr_Attrs             *pAttrs;
    IArg                               key;
    ClientNotifyMgr_Object            *obj;
    ClientNotifyMgr_eventListenerInfo *listener;
    UInt32                             k;
    UInt16                             instId;
    UInt16                             instNo;
    UInt16                             creProcId;

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_registerClient",
               handle,
               pRegParams,
               id);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != pRegParams->fxnPtr)
                && (NULL != id)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "handle passed is NULL!");
    }
    else if (pRegParams == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "pRegParams passed is NULL!");
    }
    else if (pRegParams->fxnPtr == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "pRegParams->fxnPtr passed is NULL!");
    }
    else if (id == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "id passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        *id = (UInt32)-1;
        obj = (ClientNotifyMgr_Object*)handle;
        GT_assert (curTrace, (obj != NULL));

        instId = obj->Id & 0xFFFF;
        /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                      & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);
        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

        pAttrs = obj->attrs;

        key = GateMP_enter (obj->gate);
        if (obj->cacheEnabled == TRUE) {
            Cache_inv((Ptr)pAttrs,
                      sizeof(ClientNotifyMgr_Attrs),
                      Cache_Type_ALL,
                      TRUE);
        }
        for (i  = 0; i <  pAttrs->numNotifyEntries; i++ ) {
            if (obj->cacheEnabled == TRUE) {
                 Cache_inv ((Ptr)obj->notifyInfoObj[i],
                            sizeof (ClientNotifyMgr_NotifyParamInfo),
                            Cache_Type_ALL,
                            TRUE);
            }
           /* Search in the  array to find out the empty slot . Update the slot
            * with  the  notification info. and return the index of the slot to
            * the caller.
            */
            if (obj->notifyInfoObj[i]->isValid  == FALSE) {
                found = TRUE;
                *id = i;
                break;
             }
        }

        if (found == TRUE) {
            obj->notifyInfoObj[i]->isValid        = TRUE;
            obj->notifyInfoObj[i]->procId         = pRegParams->procId;
            obj->notifyInfoObj[i]->clientHandle   = (UInt32)pRegParams->clientHandle;
            obj->notifyInfoObj[i]->notifyType     = pRegParams->notifyType;
            obj->notifyInfoObj[i]->waitOnMultiNotifyFlags = FALSE;
            for (k = 0;
                 k < pAttrs->numSubNotifyEntries;
                 k++) {
                obj->notifyInfoObj[i]->notifyFlag[k]    = FALSE;
                obj->notifyInfoObj[i]->eventCount[k]    = 0;
            }
            for (k = 0;
                 k < CLIENTNOTIFYMGR_BIT_FLAG_SIZE;
                 k++) {
                obj->notifyInfoObj[i]->bitFlag_notifySubEntries[k]    = FALSE;
            }
            obj->notifyInfoObj[i]->waterMarkCond1 = pRegParams->watermarkCond1;
            obj->notifyInfoObj[i]->waterMarkCond2 = pRegParams->watermarkCond2;
            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                             sizeof (ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
            }
            pAttrs->numActClients += 1;
            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv((Ptr)pAttrs,
                            sizeof(ClientNotifyMgr_Attrs),
                            Cache_Type_ALL,
                            TRUE);
            }

            listener = (ClientNotifyMgr_eventListenerInfo *)Memory_calloc (NULL,
                                     sizeof (ClientNotifyMgr_eventListenerInfo),
                                     0,
                                     NULL);
    #if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (listener == NULL) {
                    status = ClientNotifyMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ClientNotifyMgr_registerClient",
                                         status,
                                         "Memory allocation failed for handle!");
            }
            else {
    #endif
                listener->cbckFxn  = pRegParams->fxnPtr;
                listener->notifyId = i;
                listener->cbContext = pRegParams->cbContext;
                List_elemClear (&listener->listElem);
                List_put ((List_Handle)
                   &ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                             &(listener->listElem));
    #if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
    #endif
        }
        else {
            /*! @retval ClientNotifyMgr_E_FAIL  Notification call back function is
             *  already registered .Call unregister and then register again.
             */
            status = ClientNotifyMgr_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_registerClient",
                                 status,
                                 "Notification call back function is already "
                                 "registered .Call unregister and then register"
                                 "again.!");
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_registerClient", status);
    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully registered the call back
     */
    return ( status);
}

/*
 * @brief Function to unregister the notification call back function.
 *
 * @param handle Instance handle.
 * @param id  notifyId obtained through the registered client call.
 */
Int32
ClientNotifyMgr_unregisterClient (ClientNotifyMgr_Handle  handle,
                                  UInt32                  id)
{
    Int32                           status = ClientNotifyMgr_S_SUCCESS;
    volatile ClientNotifyMgr_Attrs         * pAttrs ;
    IArg                             key;
    ClientNotifyMgr_Object          *obj;
    List_Elem                       *elem;
    UInt32                          k;
    UInt16                          lprocId;
    UInt16                          instId;
    UInt16                          instNo;
    UInt16                          creProcId;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_unregisterClient", handle);

    GT_assert (curTrace,
               ((NULL != handle)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_unregisterClient",
                             status,
                             "handle passed is NULL!");
    }
    else if (id >= handle->numNotifyEntries) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG id passed is mor than max
         * clients
         */
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_unregisterClient",
                             status,
                             "id passed is mor than max clients!");
    }
    else {
#endif
        obj = (ClientNotifyMgr_Object*)handle;

        lprocId = MultiProc_self();

        pAttrs  = obj->attrs;

        instId = obj->Id & 0xFFFF;
        /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                      & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);
        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

        key = GateMP_enter (obj->gate);
        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs,
                       sizeof(ClientNotifyMgr_Attrs),
                       Cache_Type_ALL,
                       TRUE);
            Cache_inv ((Ptr)obj->notifyInfoObj[id],
                       sizeof (ClientNotifyMgr_NotifyParamInfo),
                       Cache_Type_ALL,
                       TRUE);
        }
        if (obj->notifyInfoObj[id]->isValid == TRUE) {
            obj->notifyInfoObj[id]->isValid        = FALSE;
            obj->notifyInfoObj[id]->procId         = (UInt16)-1;
            obj->notifyInfoObj[id]->clientHandle   = 0;
            obj->notifyInfoObj[id]->notifyType     = ClientNotifyMgr_NOTIFICATION_NONE;
            obj->notifyInfoObj[id]->waitOnMultiNotifyFlags = FALSE;
            for (k = 0;
                 k < pAttrs->numSubNotifyEntries;
                 k++) {
                obj->notifyInfoObj[id]->notifyFlag[k]    = FALSE;
                obj->notifyInfoObj[id]->eventCount[k]    = 0;
            }
            for (k = 0;
                 k < CLIENTNOTIFYMGR_BIT_FLAG_SIZE;
                 k++) {
                obj->notifyInfoObj[id]->bitFlag_notifySubEntries[k]    = FALSE;
            }
            obj->notifyInfoObj[id]->waterMarkCond1 = (UInt32)-1;
            obj->notifyInfoObj[id]->waterMarkCond2 = (UInt32)-1;

            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                             sizeof (ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
            }
            pAttrs->numActClients -= 1;
            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv((Ptr)pAttrs,
                            sizeof(ClientNotifyMgr_Attrs),
                            Cache_Type_ALL,
                            TRUE);
            }

            /* Search and remove the listener element corrsponding to this id */
            List_traverse (elem,
                           (List_Handle)
                            &ClientNotifyMgr_module->eventListenerList[creProcId][instNo]) {
                if (   ((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId
                        == id) {
                   // lkey = IGateProvider_enter (ClientNotifyMgr_module->listLock);
                   List_remove ((List_Handle)
                                 &ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                                elem);
                   // IGateProvider_leave (ClientNotifyMgr_module->listLock, lkey);
                    break;
                }

            }
        }
        else {
            /*! @retval ClientNotifyMgr_E_FAIL  Notification call back function is
             *  already uregistered or Not registred previousely.
             */
            status = ClientNotifyMgr_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_unregisterClient",
                                 status,
                                 "Notification call back function is already "
                                 "uregistered .or Not registred previousely.!");
        }

        GateMP_leave (obj->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_unregisterClient", status);

    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully unregistered the call back
     */
    return ( status);
}

/*!
 *  @brief        Function to enable notification for the caller client.
 *
 *  @param handle Instance handle.
 *  @param  id    id obtained through registerClient call.
 */
Int32
ClientNotifyMgr_enableNotification (ClientNotifyMgr_Handle  handle,
                                    UInt32                  id,
                                    UInt32                  subId)
{

    Int32                           status = ClientNotifyMgr_S_SUCCESS;
    IArg                             key;
    ClientNotifyMgr_Object           *obj;

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_enableNotification",
               handle,
               id,
               subId);

    GT_assert (curTrace,(NULL != handle));

    GT_assert (curTrace,(id < handle->numNotifyEntries));

    GT_assert (curTrace,(subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
         /*! @retval ClientNotifyMgr_E_INVALIDARG  id pointer is NULL
          */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "ClientNotifyMgr_enableNotification",
                              ClientNotifyMgr_E_INVALIDARG,
                              "handle  passed is NULL !");
    }
    else if (id >= handle->numNotifyEntries) {
         /*! @retval ClientNotifyMgr_E_INVALIDARG  id pointer is NULL
          */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "ClientNotifyMgr_enableNotification",
                              ClientNotifyMgr_E_INVALIDARG,
                              "id  value is more than the"
                              "ClientNotifyMgr_MAX_CLIENTS limit !");
    }
    else if (subId >= CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG id passed is mor than max
         * clients
         */
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_enableNotification",
                             status,
                             "subId passed is more than "
                             "CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES !");
    }
    else {
#endif
        obj = (ClientNotifyMgr_Object*)handle;

        key = GateMP_enter (obj->gate) ;

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)obj->notifyInfoObj[id],
                       sizeof (ClientNotifyMgr_NotifyParamInfo),
                       Cache_Type_ALL,
                       TRUE);
        }
        obj->notifyInfoObj[id]->notifyFlag[subId] = TRUE;
        if (obj->cacheEnabled == TRUE) {
            Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                         sizeof (ClientNotifyMgr_NotifyParamInfo),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_enableNotification",
               status);
    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully enabled notification.
     */
   return ( status);
}

/*!
 *  @brief        Function to disable notification for the caller client.
 *
 * @param handle Instance handle.
 * @param  id    id obtained through registerClient call.
 */

Int32
ClientNotifyMgr_disableNotification (ClientNotifyMgr_Handle  handle,
                                     UInt32                  id,
                                     UInt32                  subId)
{
    Int32                           status = ClientNotifyMgr_S_SUCCESS;
    IArg                             key;
    ClientNotifyMgr_Object           *obj;

    GT_1trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_disableNotification",
               id);

    GT_assert (curTrace,(NULL != handle));
    GT_assert (curTrace,(id < handle->numNotifyEntries));
    GT_assert (curTrace, (subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_disableNotification",
                             status,
                             "handle passed is NULL!");
    }
    else if (id >= handle->numNotifyEntries) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG id passed is mor than max
         * clients
         */
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_disableNotification",
                             status,
                             "id passed is mor than max clients!");
    }
    else if (subId >= CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG id passed is mor than max
         * clients
         */
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_disableNotification",
                             status,
                             "subId passed is more than "
                             "CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES !");
    }
    else {
#endif
        obj = (ClientNotifyMgr_Object*)handle;

        key = GateMP_enter (obj->gate);

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)obj->notifyInfoObj[id],
                       sizeof (ClientNotifyMgr_NotifyParamInfo),
                       Cache_Type_ALL,
                       TRUE);
        }
        obj->notifyInfoObj[id]->notifyFlag[subId] = FALSE;
        if (obj->cacheEnabled == TRUE) {
            Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                         sizeof (ClientNotifyMgr_NotifyParamInfo),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_disableNotification", status);
    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully disabled notification
     */
     return (status);
}

/*!
 *  @brief     Function to send notification to the client identified the id.
 *
 *  @param  handle     Instance handle.
 *  @param  id         id of the remote client to which notification needs to be
 *                     sent.see {@link #registerClient}.Caller client needs to
 *                     know the remote client's notifyId to be able to send
 *                     notification.
 *  @param  arg1       Value to checked against water mark cond1.
 *  @param  arg2       Value to checked against water mark cond2 of the
 *                     other client.
 *
 */
Int32
ClientNotifyMgr_sendNotification (ClientNotifyMgr_Handle  handle,
                                  UInt32                  id,
                                  UInt32                  subId,
                                  UInt32                  arg1 ,
                                  UInt32                  arg2 )
{
    Int32                            status = ClientNotifyMgr_S_SUCCESS;
    UInt32                           i      = 0;
    volatile ClientNotifyMgr_Attrs   *pAttrs;
    IArg                             key;
    Bool                             sendEvent[MultiProc_MAXPROCESSORS] ={0};
    UInt32                           procId;
    ClientNotifyMgr_Object           *obj;
    UInt32                           notifyPayload;

    GT_4trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendNotification",
               handle,
               id,
               arg1 ,
               arg2);


    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotification",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif
        obj     =  (ClientNotifyMgr_Object*)handle;

        i = id;
        pAttrs  =   obj->attrs;

        key = GateMP_enter (obj->gate);
        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs,
                       sizeof (ClientNotifyMgr_Attrs),
                       Cache_Type_ALL,
                       TRUE);
        }
        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)obj->notifyInfoObj[i],
                       sizeof(ClientNotifyMgr_NotifyParamInfo),
                       Cache_Type_ALL,
                       TRUE);
        }

        if (obj->notifyInfoObj[i]->isValid == TRUE)  {
            switch (obj->notifyInfoObj[i]->notifyType) {

            case ClientNotifyMgr_NOTIFICATION_ALWAYS:
                {
                    if ((obj->notifyInfoObj[i]->notifyFlag[subId] == TRUE)
                        && (arg1 >= obj->notifyInfoObj[i]->waterMarkCond1)) {
                            procId = obj->notifyInfoObj[i]->procId;
                            GT_assert (curTrace,
                                       (procId < MultiProc_getNumProcessors()));
                            sendEvent[procId] = 1u;
                            obj->notifyInfoObj[i]->eventCount[subId]++;
                    }
                }
                break;

            case ClientNotifyMgr_NOTIFICATION_ONCE:
            case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ONCE:
                {
                    if (   (obj->notifyInfoObj[i]->notifyFlag[subId] == TRUE)
                        && (arg1 >= obj->notifyInfoObj[i]->waterMarkCond1)) {
                            procId = obj->notifyInfoObj[i]->procId;
                            GT_assert (curTrace,
                                       (procId < MultiProc_getNumProcessors()));
                            sendEvent[procId] = 1u;
                            obj->notifyInfoObj[i]->eventCount[subId]++;
                            obj->notifyInfoObj[i]->notifyFlag[subId] = FALSE;
                    }

                }
                break;

            case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS:
                {
                    if (arg1 >= obj->notifyInfoObj[i]->waterMarkCond1) {
                            procId = obj->notifyInfoObj[i]->procId;
                            GT_assert (curTrace,
                                       (procId < MultiProc_getNumProcessors()));
                            sendEvent[procId] = 1u;
                            obj->notifyInfoObj[i]->eventCount[subId]++;
                            obj->notifyInfoObj[i]->notifyFlag[subId] = TRUE;
                    }
                }
                break;

            case ClientNotifyMgr_NOTIFICATION_NONE:
                break;

            default:
                break;
            }

            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                             sizeof(ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
            }
        }

        procId = MultiProc_self();
        notifyPayload = pAttrs->entryId & 0xFFFF;

        GateMP_leave (obj->gate, key);

        /* Send notification to the  other client identified by the id*/
        if ((sendEvent[obj->notifyInfoObj[i]->procId] == TRUE)) {
            /* For normal notificatios application spcific payload is zero.So
             * last two MSBs arezero in notifyPayload*/

             if (  (procId != obj->notifyInfoObj[i]->procId)
                 &&(   Notify_intLineRegistered (i,CLIENTNOTIFYMGR_INT_LINEID))) {
                      Notify_sendEvent (
                                obj->notifyInfoObj[i]->procId,
                                           CLIENTNOTIFYMGR_INT_LINEID,
                                CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                notifyPayload,
                                TRUE );
             }
             else {
                /* Sending notification to the self procId */
                Notify_sendEvent (
                                  procId,
                                  CLIENTNOTIFYMGR_INT_LINEID,
                                  CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                  notifyPayload,
                                  TRUE );
             }
        }



#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_sendNotification", status);

    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully sends notifications.
     */
    return (status);
}

/*!
 *  @brief     Function to send Force notification to the client  whose
 *             registration id is passed.
 *
 *  @param  handle     Instance handle.
 *  @param  id         id of the remote client to which notification needs to be
 *                     sent.see {@link #registerClient}.Caller client needs to
 *                     know the remote client's notifyId to be able to send
 *                     notification.
 *  @param  payLoad    payload that can be sent  to the remote client.
 *
 */
Int32
ClientNotifyMgr_sendForceNotification (ClientNotifyMgr_Handle  handle,
                                       UInt32                  id,
                                       UInt16                  payLoad)
{
    Int32                            status    = ClientNotifyMgr_S_SUCCESS;
    Bool                             sendEvent = 0;
    volatile ClientNotifyMgr_Attrs           *pAttrs;
    IArg                             key;
    UInt32                           procId;
    ClientNotifyMgr_Object           *obj;
    UInt32                           notifyPayload;
    UInt32                           subId;

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendForceNotification",
               handle,
               id,
               payLoad);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace,
               (id < handle->numNotifyEntries));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotification",
                             status,
                             "handle passed is NULL!");
    }
    else if (id >= handle->numNotifyEntries) {
         /*! @retval ClientNotifyMgr_E_INVALIDARG  id pointer is NULL
          */
         GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "ClientNotifyMgr_sendForceNotification",
                              ClientNotifyMgr_E_INVALIDARG,
                              "remote client registration notification id is"
                              "more than the ClientNotifyMgr_MAX_CLIENTS "
                              "limit !");
    }
    else {
#endif
        obj     =  (ClientNotifyMgr_Object*)handle;

        pAttrs  =   obj->attrs;

        /* For the time being use subId zero for force notifications */
        subId = 0;

        /* Acquire lock */
        key = GateMP_enter (obj->gate);
        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs,
                       sizeof (ClientNotifyMgr_Attrs),
                       Cache_Type_ALL,
                       TRUE);
        }

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)obj->notifyInfoObj[id],
                       sizeof(ClientNotifyMgr_NotifyParamInfo),
                       Cache_Type_ALL,
                       TRUE);
        }

        if (obj->notifyInfoObj[id]->isValid == TRUE) {
            switch (obj->notifyInfoObj[id]->notifyType) {

            case ClientNotifyMgr_NOTIFICATION_ALWAYS:
            case ClientNotifyMgr_NOTIFICATION_ONCE:
            case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ONCE:
            case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS:
            case ClientNotifyMgr_NOTIFICATION_NONE:
                {
                        procId = obj->notifyInfoObj[id]->procId;
                        sendEvent = 1u;
                        obj->notifyInfoObj[id]->forceNotify =  TRUE;
                }
                break;
            default:
                break;
            }/* End of switch (obj->notifyInfoObj[id]->notifyType) */

            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                             sizeof(ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
            }
        }
        else {
            status = ClientNotifyMgr_E_FAIL;
        }

        procId = MultiProc_self();

        notifyPayload = (   (((UInt32)payLoad) << 16)
                          + (pAttrs->entryId & 0xFFFF));
        /* Release lock */
        GateMP_leave (obj->gate, key);

        /* Send notification to the  other client identified by the id*/
        if (sendEvent == TRUE) {
            /* Application specific payload will be in 2MSBs of payload that is
             * to be send through notify. ClientNotifyMgr instNo  occupies the
             * 2LSBs.
             */


            /* Check if the client is in the same process*/
            if (    (procId == obj->notifyInfoObj[id]->procId)
                && ( Notify_intLineRegistered (obj->notifyInfoObj[id]->procId,
                                            CLIENTNOTIFYMGR_INT_LINEID))) {

                /* Sending notification to the self procId */
                Notify_sendEvent (
                                  procId,
                                  CLIENTNOTIFYMGR_INT_LINEID,
                                  CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                  notifyPayload,
                                  TRUE );


            }
            else  {
                 if (Notify_intLineRegistered (obj->notifyInfoObj[id]->procId,
                                            CLIENTNOTIFYMGR_INT_LINEID)) {
                      Notify_sendEvent (
                                obj->notifyInfoObj[id]->procId,
                                CLIENTNOTIFYMGR_INT_LINEID,
                                CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                notifyPayload,
                                TRUE );
                 }
            }
        }



#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif
    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_sendForceNotification",
               status);

    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully sends force
     *           notifications.
     */
    return (status);
}

/*!
 *  @brief Function to send notification to all the clients of ClientNotifyMgr
 *         instance except the caller client.
 *
 * @param handle Instance handle.
 * @param id     id obtained through registerClient call.

 * @param arg1   Value to checked against water mark. For FrameQBufMgr it should
 *               be number of free buffers whose size  is  arg2 value.
 * @param arg2   Size of the buffers.
 */
Int32
ClientNotifyMgr_sendNotificationMulti (ClientNotifyMgr_Handle  handle,
                                       UInt32                  id,
                                       UInt32                  subId,
                                       UInt32                  arg1 ,
                                       UInt32                  arg2 )
{
    Int32                            status  = ClientNotifyMgr_S_SUCCESS;
    UInt32                           counter = 0;
    volatile ClientNotifyMgr_Attrs           *pAttrs;
    IArg                             key;
    Bool                             sendEvent[MultiProc_MAXPROCESSORS] ={0};
    UInt32                           procId;
    UInt32                           i     ;
    ClientNotifyMgr_Object           *obj;
    UInt32                           notifyPayload;

    GT_4trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendNotificationMulti",
               handle,
               id,
               arg1 ,
               arg2);

    GT_assert (curTrace, (NULL != handle));

    GT_assert (curTrace, (subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotificationMulti",
                             status,
                             "handle passed is NULL!");
    }
    else if (subId >= CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotificationMulti",
                             status,
                             "invalid subnotfy entry id passed!");
    }
    else {
#endif
        obj     =  (ClientNotifyMgr_Object*)handle;
        pAttrs =   obj->attrs;

        key = GateMP_enter (obj->gate);

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs,
                        sizeof (ClientNotifyMgr_Attrs),
                        Cache_Type_ALL,
                        TRUE);
        }

        for (i = 0;
             i < pAttrs->numNotifyEntries && counter < pAttrs->numActClients;
             i++) {
               if (i != id) {
                  if (obj->cacheEnabled == TRUE) {
                      Cache_inv ((Ptr)obj->notifyInfoObj[i],
                                 sizeof(ClientNotifyMgr_NotifyParamInfo),
                                 Cache_Type_ALL,
                                 TRUE);
                  }

                  if ((obj->notifyInfoObj[i]->isValid == TRUE)) {
                      counter ++;
                      switch (obj->notifyInfoObj[i]->notifyType) {

                      case ClientNotifyMgr_NOTIFICATION_ALWAYS:
                            {
                                if ( (obj->notifyInfoObj[i]->waterMarkCond1 <=1) ||
                                    ((obj->notifyInfoObj[i]->notifyFlag[subId] == TRUE)
                                    && (arg1 >= obj->notifyInfoObj[i]->waterMarkCond1))) {

                                        procId = obj->notifyInfoObj[i]->procId;
                                        GT_assert (curTrace,
                                                   (procId <
                                                    MultiProc_getNumProcessors()));

                                        sendEvent[procId] = 1u;
                                        obj->notifyInfoObj[i]->eventCount[subId]
++;
                                }
                            }
                            break;

                        case ClientNotifyMgr_NOTIFICATION_ONCE:
                        case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ONCE:
                            {
                                if (   (    obj->notifyInfoObj[i]->notifyFlag[subId]
                                         == TRUE)
                                    && (    arg1
                                         >= obj->notifyInfoObj[i]->waterMarkCond1)) {
                                        procId = obj->notifyInfoObj[i]->procId;
                                        GT_assert (curTrace,
                                                   (procId <
                                                    MultiProc_getNumProcessors()));
                                        sendEvent[procId] = 1u;
                                        obj->notifyInfoObj[i]->eventCount[subId]++;/* Event has to be sent */
                                        obj->notifyInfoObj[i]->notifyFlag[subId] =
                                                                          FALSE;
                                }

                            }
                            break;

                        case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS:
                            {
                                if (arg1 >= obj->notifyInfoObj[i]->waterMarkCond1) {
                                        procId = obj->notifyInfoObj[i]->procId;
                                        GT_assert (curTrace,
                                                   (procId <
                                                    MultiProc_getNumProcessors()));
                                        sendEvent[procId] = 1u;
                                        obj->notifyInfoObj[i]->eventCount[subId]
++;
                                        obj->notifyInfoObj[i]->notifyFlag[subId]
                                                                        =  TRUE;
                                }
                            }
                            break;

                        case ClientNotifyMgr_NOTIFICATION_NONE:
                            break;

                        default:
                            break;
                        }
                        if (obj->cacheEnabled == TRUE) {
                            Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                                         sizeof(ClientNotifyMgr_NotifyParamInfo),
                                         Cache_Type_ALL,
                                         TRUE);
                        }
                  }

               }
               else {
                   if (obj->notifyInfoObj[i]->isValid ==TRUE) {
                       counter++;
                   }
               }
        }

        procId = MultiProc_self();
        /* For normal notificatios application spcific payload is zero.So
         * last two MSBs arezero in notifyPayload*/
        notifyPayload = pAttrs->entryId & 0xFFFF;

        GateMP_leave (obj->gate, key);

        /* Send notofication to the all the eligible clients */
        for (i = 0; i < MultiProc_getNumProcessors(); i++) {
            /* Send notifications to the other processors */
                if ((i != procId) &&
                    (sendEvent[i] == 1u)
                    && (Notify_intLineRegistered (i,
                                            CLIENTNOTIFYMGR_INT_LINEID))) {
                    Notify_sendEvent (
                               i,
                               CLIENTNOTIFYMGR_INT_LINEID,
                               CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                               notifyPayload,
                               TRUE );
                }
        }

        /* Call Registered call back functions of the clients on this processor */
        if (sendEvent[procId] == TRUE ) {
            /* Sending notification to the self procId */
            Notify_sendEvent (
                              procId,
                              CLIENTNOTIFYMGR_INT_LINEID,
                              CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                              notifyPayload,
                              TRUE );
        }


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_sendNotificationMulti",
               status);
    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully sends notifications.
     */
    return (status);
}

/*!
 *  @brief Function to send notification to all the clients of ClientNotifyMgr
 *         instance except the caller client.
 *
 * @param handle Instance handle.
 * @param id     id obtained through registerClient  call for the caller.

 * @param payLoad  16 bit payload that needs to be sent to the all the other
 *                 valid clients whose call back is registered.
 */
Int32
ClientNotifyMgr_sendForceNotificationMulti (ClientNotifyMgr_Handle  handle,
                                            UInt32                  id,
                                            UInt16                  payLoad )
{
    Int32                            status  = ClientNotifyMgr_S_SUCCESS;
    UInt32                           counter = 0;
    UInt32                           numValidClients = 0;
    volatile ClientNotifyMgr_Attrs           *pAttrs;
    IArg                             key;
    Bool                             sendEvent[MultiProc_MAXPROCESSORS] ={0};
    UInt32                           procId;
    UInt32                           i     ;
    ClientNotifyMgr_Object           *obj;
    UInt32                           notifyPayload;
    UInt32                           subId;

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendForceNotificationMulti",
               handle,
               id,
               payLoad);

    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotificationMulti",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif
        obj     =  (ClientNotifyMgr_Object*)handle;
        pAttrs  =  obj->attrs;
        subId = 0;

        /* Acquire lock */
        key = GateMP_enter (obj->gate);

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs,
                       sizeof (ClientNotifyMgr_Attrs),
                       Cache_Type_ALL,
                       TRUE);
        }
        for (i = 0;
             (    (i < pAttrs->numNotifyEntries)
               && (counter < pAttrs->numActClients));
             i++) {
               if (i != id) {
                    if (obj->cacheEnabled == TRUE) {
                      Cache_inv ((Ptr)obj->notifyInfoObj[i],
                                 sizeof(ClientNotifyMgr_NotifyParamInfo),
                                 Cache_Type_ALL,
                                 TRUE);
                    }

                    if ((obj->notifyInfoObj[i]->isValid == TRUE)) {
                      counter ++;
                        switch (obj->notifyInfoObj[i]->notifyType) {
                        /* Enable the event flag for the valid notification
                         * types.
                         */
                        case ClientNotifyMgr_NOTIFICATION_ALWAYS:
                        case ClientNotifyMgr_NOTIFICATION_ONCE:
                        case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ONCE:
                        case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS:
                        case ClientNotifyMgr_NOTIFICATION_NONE:
                            {
                                procId = obj->notifyInfoObj[i]->procId;

                                sendEvent[procId] = 1u;
                                obj->notifyInfoObj[i]->forceNotify =  TRUE;
                                numValidClients += 1;
                            }
                            break;

                        default:
                            break;
                        }
                        /* Write back updated notifyInfoObj*/
                        if (obj->cacheEnabled == TRUE) {
                            Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                                         sizeof(ClientNotifyMgr_NotifyParamInfo),
                                         Cache_Type_ALL,
                                         TRUE);
                        }
                  }

               }
               else {
                   if (obj->notifyInfoObj[i]->isValid ==TRUE) {
                       counter++;
                   }
               }
        }

        if (numValidClients == 0) {
            /* Release lock */
            GateMP_leave (obj->gate, key);
            status = ClientNotifyMgr_E_FAIL;
        }
        else {
            procId = MultiProc_self();

            /* Application specific payload will be in 2MSBs of payload that
             * is to be send through notify. ClientNotifyMgr instNo  occupies
             * the 2LSBs.
             */
            notifyPayload = (   (((UInt32)payLoad) << 16)
                             +  (pAttrs->entryId & 0xFFFF));

            /* Release lock */
            GateMP_leave (obj->gate, key);

            /* Send notification to  all the eligible clients */
            for (i = 0; i < MultiProc_getNumProcessors(); i++) {
                /* Send notifications to the other processors */
                if ((i != procId) &&
                    (sendEvent[i] == 1u)) {
                            Notify_sendEvent(
                                                i,
                                                CLIENTNOTIFYMGR_INT_LINEID,
                                               CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                               notifyPayload,
                                               TRUE );
                }
            }

            /* Call Registered call back functions of the clients on this
             * processor
             */
            if (sendEvent[procId] == 1u ) {
                /* Sending notification to the self procId */

                status = Notify_sendEvent (
                                           procId,
                                           CLIENTNOTIFYMGR_INT_LINEID,
                                           CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                           notifyPayload,
                                           TRUE );

            }
        }


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_sendForceNotificationMulti",
               status);

    /*!< @retval ClientNotifyMgr_S_SUCCESS if successfully sends notifications.
     */
    return (status);
}
