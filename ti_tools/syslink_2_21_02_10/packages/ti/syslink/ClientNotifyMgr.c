/*
 *  @file   ClientNotifyMgr.c
 *
 *  @brief      Implements ClientNotifyMgr functions on RTOS side.
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
/*-------------------------    XDC  specific includes ------------------------*/
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Error.h>
//#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <ti/sysbios/gates/GateSwi.h>
#ifdef xdc_target__isaCompatible_64P
#include <ti/sysbios/family/c64p/Cache.h>
#else
#include <ti/sysbios/hal/Cache.h>
#endif
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
/*-------------------------    BIOS  specific includes -----------------------*/
#include <ti/sysbios/gates/GateMutex.h>
/*-------------------------    Generic includes  -----------------------------*/
#include <string.h>
/*-------------------------    IPC module specific includes ----------------- */
#include <ti/sdo/utils/_MultiProc.h>
#include <ti/sdo/ipc/_GateMP.h>
#include <ti/sdo/utils/_NameServer.h>
#include <ti/sdo/ipc/_SharedRegion.h>
#include <ti/sdo/ipc/_Notify.h>
#include <ti/sdo/ipc/interfaces/INotifyDriver.h>
#include <ti/sdo/ipc/notifyDrivers/NotifyDriverShm.h>
#include <ti/sdo/utils/List.h>

#endif /* #if defined(SYSLINK_BUILD_RTOS) */

#if defined(SYSLINK_BUILD_HLOS)
/* Standard headers */
#include <ti/syslink/Std.h>

/* Standard headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/inc/knl/OsalThread.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/utils/List.h>
#endif /* #if defined(SYSLINK_BUILD_HLOS) */

/* Utilities & OSAL headers */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h>
/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/Notify.h>

#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/ClientNotifyMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr_config.h>
#include <ti/syslink/inc/_ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>


#if defined(SYSLINK_BUILD_RTOS)
#define Memory_copy memcpy
#define Memory_set  memset
#endif

String  CLIENTNOTIFYMGR_NAMESERVERNAME      = "ClientNotifyMgr";

#define CLIENTNOTIFYMGR_INSTNO_BITS            8u
#define CLIENTNOTIFYMGR_INSTNO_MASK            0xFF
#define CLIENTNOTIFYMGR_INSTNO_BITOFFSET       0u
#define CLIENTNOTIFYMGR_CREATORPROCID_BITS     8u
#define CLIENTNOTIFYMGR_CREATORPROCID_MASK     0xFF
#define CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET 8u

/* Macro to left shift the system key by 16bits and add to the eventNo */
#define CLIENTNOTIFYMGR_ADDSYSKEY(eventNo) \
                            ((UInt32)eventNo + (UInt32)(Notify_SYSTEMKEY << 16))



#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

#define MultiProc_MAXPROCESSORS 10 // TBD include _Multiproc.h

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*
 * @brief Structure for ClientNotifyMgr module state
 */
typedef struct ClientNotifyMgr_ModuleObject_Tag {
    UInt32                     refCount;
    /*!< Reference count */
    IGateProvider_Handle        gate;
    /*!< Handle to lock for protection */
    UInt32                      numProcessors;
    /*!< Number of supported processors including local processor.*/
    NameServer_Handle           nameServer  ;
    /*!< Handle to the local NameServer used for stoting ClientNotifyMgr instance
     *   information.
     */
    UInt32                      nameServerType;
    /*!< Name server create type. Dynamic or static*/
    Bool                        usrGate;
    /*!< Flag denotes if it is user provided gate or not */
    UInt32                      instRefCount[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances];
    /* List holding created objects */
    Bool                        isEventRegistered[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances][MultiProc_MAXPROCESSORS];
    /* List holding created objects */
    List_Handle                 objList;
    /* List holding created objects */
    List_Handle                 eventListenerList[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances];
    /* EventListener info for all the instances and its clients */
    IGateProvider_Handle        listLock;
    /* Lock to protect lists */
    ClientNotifyMgr_Config      cfg;
    /*!<  Current module wide config values*/
    ClientNotifyMgr_Config      defaultCfg;
    /*!< Default config values*/
    Ptr callBackCtxPtrs[MultiProc_MAXPROCESSORS][ClientNotifyMgr_maxInstances][MultiProc_MAXPROCESSORS];
    /* Place holder for the malloced call back context pointers of
     * the ClientNotifyMgr_callbacks
     */
     Bool isInstNoValid[ClientNotifyMgr_maxInstances];
    /*  Create call will check this flags and sets first invalid index to valid
     *  and  assigns this index to the first byte of obj->Id
     */
    ClientNotifyMgr_swiPostInfo          swiPostInfo;
    ClientNotifyMgr_notifyPayloadInfo    notifyPaloadEntry[CLIENTNOTIFYMGR_SWI_PAYLOAD_ENTRIES];
    ClientNotifyMgr_TaskPostInfo          taskPostInfo;
    ClientNotifyMgr_notifyPayloadInfo    cliTsknotifyPaloadEntry[CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES];
} ClientNotifyMgr_ModuleObject;

/*
 * @brief Structure defining internal object for the ClientNotifyMgr
 */
typedef struct ClientNotifyMgr_Obj_Tag {
    List_Elem                                  listElem;

    UInt32                                     type;
    /*!<  Type of the object. Dynamic create, Dynamic open,
     *    static create or static open
     */
    GateMP_Handle                              gate;
    /*!< Gate handle for protection */
    UInt16                                     regionId;       /* SharedRegion ID               */

    volatile ClientNotifyMgr_Attrs            *attrs;
    volatile ClientNotifyMgr_NotifyParamInfo **notifyInfoObj;
    Bool                                       cacheEnabled;
    /*!<  Flag indicating whether Cache coherence API needs to be called for
     *    instance control structures
     */
    UInt32                                     minAlign;

    UInt32                                     numNotifyEntries;

    Ptr                                        top;
    /* Pointer to the top Object */
    ClientNotifyMgr_Params                     params;
    /*!< Instance  config params */
    UInt16                                     Id;
    /*!< Id of the instance. MSB byte contains the creator procId and LSB
     * contains the instance no  on that creator processor.On each processor we
     * can create  256 instances .
     */
    Ptr                                        nsKey;
} ClientNotifyMgr_Obj;

/*
 * @brief Structure defining object for the ClientNotifyMgr.
 */
struct ClientNotifyMgr_Object {
    Ptr obj;
    /*!< Pointer to the ClientNotifyMgr internal object*/
   /*TODO: Add function table interface */
};

/*!
 *  @var    ClientNotifyMgr_module_obj
 *
 *  @brief  ClientNotifyMgr_module state variable.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
ClientNotifyMgr_ModuleObject ClientNotifyMgr_module_obj = {
    .refCount                     = 0,
    .gate                         = NULL,
    .numProcessors                = MultiProc_MAXPROCESSORS,
    .nameServer                   = NULL,
    .nameServerType               = 0,
    .usrGate                      = FALSE,
    .objList                      = NULL,
    .listLock                     = NULL,
    .cfg.gate                     = NULL,
    .cfg.numProcessors            = MultiProc_MAXPROCESSORS,
    .cfg.procIds                  = NULL,
    .cfg.drvHandles               = NULL,
    .defaultCfg.gate              = NULL,
    .defaultCfg.numProcessors     = MultiProc_MAXPROCESSORS,
    .defaultCfg.procIds           = NULL,
    .defaultCfg.drvHandles        = NULL,
    .swiPostInfo.swiHandle        = NULL,
    .swiPostInfo.notifyCnt        = 0,
    .swiPostInfo.readIndex        = 0,
    .swiPostInfo.writeIndex       = 0,
    .taskPostInfo.tskHandle       = NULL,
    .taskPostInfo.semHandle       = NULL,
    .taskPostInfo.notifyCnt       = 0,
    .taskPostInfo.readIndex       = 0,
    .taskPostInfo.writeIndex      = 0
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

/* ============================================================================
 * Internal API Prototypes
 * ============================================================================
 */

/* Function to create a nameServer.*/
static
void *
ClientNotifyMgr_setNameServer (String name , GateMP_Handle gate);

/* Function to create a nameServer. */
static
Int32
ClientNotifyMgr_deleteNameServer (NameServer_Handle * nsHandle);

/* Function to create a ClientNotifyMgr instance. */
ClientNotifyMgr_Handle
_ClientNotifyMgr_create (const Void * params);

static inline Void
ClientNotifyMgr_execCallBackFxn(UArg   arg,
                                UInt32 notifyPayload);
static inline
Int32  ClientNotifyMgr_registerWithNotify(Int32 lProtection,
                                          UInt16 procid,
                                          UInt16 lineId,
                                          UInt32 eventNo,
                                          UArg context);

static inline
Int32  ClientNotifyMgr_unregisterWithNotify(Int32 lProtection,
                                            UInt16 procid,
                                            UInt16 lineId,
                                            UInt32 eventNo,
                                            UArg context);
/*==============================================================================
 * API definations
 *==============================================================================
 */

/*!
 *  @brief      CallBack function of the ClientNotifyMgr which internally
 *              generates call backs for the registered clients on the local
 *              processor.
 *
 *  @param      procId   Remote processor id which send the event.
 *  @param      lineId   Interrupt line ID.
 *  @param      eventNo  Gate for protection.
 *  @param      arg      ClientNotifyMgr instance handle .
 *  @param      payload  PayLoad sent by sender of this notification.
 */
Void
ClientNotifyMgr_callBack(UInt16 procId,
                         UInt16 lineId,
                         UInt32 eventNo,
                         UArg   arg,
                         UInt32 notifyPayload)
{
    GT_5trace (curTrace,
              GT_ENTER,
              "ClientNotifyMgr_callBack",
              procId,
              lineId,
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
                                 "ClientNotifyMgr_callBack",
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
            ClientNotifyMgr_execCallBackFxn(arg,notifyPayload);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgr_callBack");
}

/*!
 *  @brief      Function  which internally
 *              generates call backs for the registered clients on the local
 *              processor.
 *
 *  @param      arg      ClientNotifyMgr instance callback context .
 *  @param      notifyPayload  PayLoad sent by sender of this notification.
 */
static inline Void
ClientNotifyMgr_execCallBackFxn(UArg   arg,
                                UInt32 notifyPayload)
{
    List_Elem                                *elem            = NULL;
    Bool                                      callAppCallBack = TRUE;
    Bool                                      waitOnSubNotify = FALSE;
    volatile ClientNotifyMgr_Attrs           *pAttrs;
    volatile ClientNotifyMgr_NotifyParamInfo *notifyInfoObj;
    ClientNotifyMgr_FnCbck           callBackFxn;
    UInt32                           entryId;
    UInt16                           instId;
    UInt32                           id;
    Ptr                              cbContext;
    ClientNotifyMgr_callbackCtx     *cliCallbackCtx;
    IArg                             key;
    UInt32                           i;
    UInt32                           j;
    UInt16                           payload;
    UInt16                           creProcId;
    UInt16                           instNo;
    UInt32                           size;

    GT_2trace (curTrace,
              GT_ENTER,
              "ClientNotifyMgr_execCallBackFxn",
              arg,
              notifyPayload);

    GT_assert (curTrace, (arg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (arg == NULL) {
            /* Call back context pointer passed to this is null */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_execCallBackFxn",
                                 ClientNotifyMgr_E_INVALIDARG,
                                 "Config passed is NULL!");
        }
        else {
#endif

            cliCallbackCtx     =  (ClientNotifyMgr_callbackCtx*)arg;
            pAttrs             =  cliCallbackCtx->attrs;
            /* App specific payload is in 2 MSBs and inst is in lower 2LSBs of
             * notifyPayLoad
             */
            entryId =  notifyPayload & 0xFFFF;
            payload =  ( notifyPayload >> 16) & 0xFFFF;

            /* Acquire lock. Local gate should be interrupt disable until call back is
             * made as SWI/TSK.
             */
            //key = GateMP_enter ((GateMP_Handle)cliCallbackCtx->gate);

            if (cliCallbackCtx->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)pAttrs, sizeof(ClientNotifyMgr_Attrs), Cache_Type_ALL,TRUE);
            }
            /* Instance Id  is a combination of Creator procId and Number of the
             * instance on the creator processor.
             */

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
                GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

                for ((elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                                        NULL)
                      );
                     elem != NULL;
                     elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                     elem)) {

                    key = GateMP_enter ((GateMP_Handle)cliCallbackCtx->gate);

                    id =((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId;
                    notifyInfoObj = (ClientNotifyMgr_NotifyParamInfo*)(
                                     ((UInt32)cliCallbackCtx->notifyInfoObjBaseAddr) +
                                     (ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo),
                                               cliCallbackCtx->minAlign)
                                       * id));

                    if (cliCallbackCtx->cacheEnabled == TRUE) {
                        Cache_inv ((Ptr)notifyInfoObj,
                                   sizeof(ClientNotifyMgr_NotifyParamInfo),
                                   Cache_Type_ALL,
                                   TRUE);
                    }

                    if ((notifyInfoObj->procId == MultiProc_self())) {
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
                        else {
                            if (notifyInfoObj->waitOnMultiNotifyFlags == TRUE) {
                                for (i = 0; i < pAttrs->numSubNotifyEntries; i++) {
                                    size = sizeof(notifyInfoObj->
                                                           bitFlag_notifySubEntries[0]);
                                    j = i / (size * 8u);
                                    /* Check  if clients are waiting on this sub notify
                                     * entry.
                                     */
                                    waitOnSubNotify =
                                        (  notifyInfoObj->bitFlag_notifySubEntries[j]
                                         & ( 1 << ( i % (size * 8u))));
                                    if (waitOnSubNotify == TRUE) {
                                        /* Client is waiting on this entry */
                                        if (notifyInfoObj->eventCount[i] == 0) {
                                            /* No need to call the call back. As this sub
                                             * notify event entry is not set
                                             */
                                             callAppCallBack = FALSE;
                                        }
                                    }
                               }
                               if (callAppCallBack == TRUE) {
                                    for (i = 0; i < pAttrs->numSubNotifyEntries; i++) {
                                        if (notifyInfoObj->eventCount[i] > 0) {
                                            notifyInfoObj->eventCount[i]--;
                                        }
                                        size = sizeof(notifyInfoObj->
                                                           bitFlag_notifySubEntries[0]);
                                        j = i / (size * 8u);
                                        /* Reset the bit in bitFlag_notifySubEntries */
                                        notifyInfoObj->bitFlag_notifySubEntries[j] =
                                        (notifyInfoObj->bitFlag_notifySubEntries[j]
                                         &  (~( 1 << ( i % (size *8)))));

                                    }
                                    notifyInfoObj->waitOnMultiNotifyFlags = FALSE;

                                    if (cliCallbackCtx->cacheEnabled == TRUE) {
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
                            }/*if (notifyInfoObj->waitOnMultiNotifyFlags == TRUE)*/
                            else {
                                for (i = 0; i < pAttrs->numSubNotifyEntries; i++) {
                                    if (notifyInfoObj->eventCount[i] > 0) {
                                        notifyInfoObj->eventCount[i]--;
                                        if (cliCallbackCtx->cacheEnabled == TRUE) {
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
                                        GT_assert (curTrace, (callBackFxn != NULL));
                                        if (callBackFxn != NULL) {
                                           /* Call the call back function */
                                            callBackFxn (
                                                    (Ptr)notifyInfoObj->clientHandle,
                                                     cbContext,
                                                     payload);
                                        }
                                    } /*if (notifyInfoObj->eventCount[i] > 0)*/
                                }/*for (i = 0; i < pAttrs->numSubNotifyEntries; i++)*/
                            }
                        }
                    }/*if ((notifyInfoObj->procId == MultiProc_self()))*/

                    GateMP_leave ((GateMP_Handle)cliCallbackCtx->gate, key);

                }/* for ((elem = List_next (ClientNotifyMgr_module->eventListenerList */
           }

           //GateMP_leave ((GateMP_Handle)cliCallbackCtx->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
#endif

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgr_callBack");
}



/*
 *  ======== ClientNotifyMgr_swiFxn ========
 */
Void ClientNotifyMgr_swiFxn(UArg arg0, UArg arg1)
{
    ClientNotifyMgr_callbackCtx     *cliCallbackCtx;
    UInt32                           notifyPayload;
    IArg key;

    key = Gate_enterSystem();
    /*  Loop to call the callbacks based on notifyCnt */
    while (ClientNotifyMgr_module->swiPostInfo.notifyCnt > 0) {
        ClientNotifyMgr_module->swiPostInfo.notifyCnt--;
        /* Get  the notifyPayload from the notifyPayload entry array */
        notifyPayload =  ClientNotifyMgr_module->notifyPaloadEntry[ClientNotifyMgr_module->swiPostInfo.readIndex].notifyPayload;
        cliCallbackCtx = ClientNotifyMgr_module->notifyPaloadEntry[ClientNotifyMgr_module->swiPostInfo.readIndex].cliCallbackCtx;
        ClientNotifyMgr_module->swiPostInfo.readIndex++;
        ClientNotifyMgr_module->swiPostInfo.readIndex =
            ClientNotifyMgr_module->swiPostInfo.readIndex % CLIENTNOTIFYMGR_SWI_PAYLOAD_ENTRIES;
        Gate_leaveSystem(key);

        /* This function calls the application call back functions */
        ClientNotifyMgr_execCallBackFxn ((UArg)cliCallbackCtx, notifyPayload);
        key = Gate_enterSystem();
    }

    Gate_leaveSystem(key);
}
/*
 *  ======== ClientNotifyMgr_cbFxn ========
 * If the instance  local protection is  TASKLET
 */
Void ClientNotifyMgr_cbFxn(UInt16 procId,
                           UInt16 lineId,
                           UInt32 eventId,
                           UArg   arg,
                           UInt32 notifyPayload)
{
#if defined (SYSLINK_BUILD_HLOS)
    OsalThread_Handle                swiHandle;
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
    Swi_Handle                       swiHandle;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
    ClientNotifyMgr_callbackCtx     *cliCallbackCtx;

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (procId >= MultiProc_getNumProcessors()) {
        /*! Notifydriver passed invalid procId the call back */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_cbFxn",
                             ClientNotifyMgr_E_INVALIDARG,
                             "procId is invalid!");
    }
    else if (arg == NULL) {
        /* Call back context pointer passed to this is null */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_cbFxn",
                             ClientNotifyMgr_E_INVALIDARG,
                             "arg passed is NULL!");
    }
    else {
#endif
        GT_assert (curTrace, (procId < MultiProc_getNumProcessors()));

        GT_assert (curTrace, (arg != NULL));

        cliCallbackCtx     =  (ClientNotifyMgr_callbackCtx*)arg;

#if defined (SYSLINK_BUILD_HLOS)
        /* Swi_Handle was passed as arg in register */
        swiHandle = (OsalThread_Handle)ClientNotifyMgr_module->swiPostInfo.swiHandle;
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
        /* Swi_Handle was passed as arg in register */
        swiHandle = (Swi_Handle)ClientNotifyMgr_module->swiPostInfo.swiHandle;
#endif /* #if defined (SYSLINK_BUILD_HLOS) */

        GT_assert (curTrace, (swiHandle != NULL));
        /* procId ,lineId and eventId can be ignored since we are not
         * passing them to application callback functions.
         */
        if (ClientNotifyMgr_module->swiPostInfo.notifyCnt == 0) {
            ClientNotifyMgr_module->swiPostInfo.readIndex = 0;
            ClientNotifyMgr_module->swiPostInfo.writeIndex = 0;
            ClientNotifyMgr_module->notifyPaloadEntry[ClientNotifyMgr_module->swiPostInfo.writeIndex].notifyPayload = notifyPayload;
            ClientNotifyMgr_module->notifyPaloadEntry[ClientNotifyMgr_module->swiPostInfo.writeIndex].cliCallbackCtx = cliCallbackCtx;
            ClientNotifyMgr_module->swiPostInfo.notifyCnt++;

        }
        else {
            ClientNotifyMgr_module->swiPostInfo.notifyCnt++;
            ClientNotifyMgr_module->swiPostInfo.writeIndex++;
            ClientNotifyMgr_module->swiPostInfo.writeIndex =
                                    ClientNotifyMgr_module->swiPostInfo.writeIndex %
                                           CLIENTNOTIFYMGR_SWI_PAYLOAD_ENTRIES;
            /* Raise assert when notifyCnt becomes equal to ClientNotifyMgr_SWI_PAYLOAD_ENTRIES.
             * It means that  Interrupts are coming very fast before executing SWI.
             * If assert is raised, increase the ClientNotifyMgr_SWI_PAYLOAD_ENTRIES and
             * and rebuild.
             */
            GT_assert (curTrace,
                      (ClientNotifyMgr_module->swiPostInfo.notifyCnt < CLIENTNOTIFYMGR_SWI_PAYLOAD_ENTRIES));

            ClientNotifyMgr_module->notifyPaloadEntry[ClientNotifyMgr_module->swiPostInfo.writeIndex].notifyPayload = notifyPayload;
            ClientNotifyMgr_module->notifyPaloadEntry[ClientNotifyMgr_module->swiPostInfo.writeIndex].cliCallbackCtx = cliCallbackCtx;
        }

        /* post the Swi */
#if defined (SYSLINK_BUILD_HLOS)
        OsalThread_activate(swiHandle);
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
        Swi_post(swiHandle);
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)   */
}

/* Endof Functions to handle SWI mode  for call back functions */
/*
 *  ======== ClientNotifyMgr_threadFxn ========
 */
Void ClientNotifyMgr_threadFxn(UArg arg0, UArg arg1)
{
    ClientNotifyMgr_callbackCtx     *cliCallbackCtx;
    UInt32                           notifyPayload;
    IArg key;

    while(1) {
#if defined (SYSLINK_BUILD_RTOS)
        /* Wait for the ClientNotifyMgr_thFxn to post the semaphore */
        Semaphore_pend (ClientNotifyMgr_module->taskPostInfo.semHandle, BIOS_WAIT_FOREVER);
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        key = Gate_enterSystem();
        /*  Loop to call the callbacks based on notifyCnt */
        while (ClientNotifyMgr_module->taskPostInfo.notifyCnt > 0) {
            ClientNotifyMgr_module->taskPostInfo.notifyCnt--;
            /* Get  the notifyPayload from the notifyPayload entry array */
            notifyPayload =  ClientNotifyMgr_module->cliTsknotifyPaloadEntry[ClientNotifyMgr_module->taskPostInfo.readIndex].notifyPayload;
            cliCallbackCtx = ClientNotifyMgr_module->cliTsknotifyPaloadEntry[ClientNotifyMgr_module->taskPostInfo.readIndex].cliCallbackCtx;
            ClientNotifyMgr_module->taskPostInfo.readIndex++;
            ClientNotifyMgr_module->taskPostInfo.readIndex =
            ClientNotifyMgr_module->taskPostInfo.readIndex % CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES;
            Gate_leaveSystem(key);
            /* This function calls the application call back functions */
            ClientNotifyMgr_execCallBackFxn ((UArg)cliCallbackCtx, notifyPayload);
            key = Gate_enterSystem();
        }

    Gate_leaveSystem(key);
    }
}
/*
 *  ======== ClientNotifyMgr_thFxn ========
 * If the instance  local protection is  TASKLET
 */
Void ClientNotifyMgr_thFxn(UInt16 procId,
                           UInt16 lineId,
                           UInt32 eventId,
                           UArg   arg,
                           UInt32 notifyPayload)
{
#if defined (SYSLINK_BUILD_HLOS)
    OsalThread_Handle osalThHandle;
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
    Semaphore_Handle semHandle;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

    ClientNotifyMgr_callbackCtx     *cliCallbackCtx;

    GT_assert (curTrace,(procId < MultiProc_getNumProcessors()));

    GT_assert(curTrace, (arg != NULL));

    cliCallbackCtx     =  (ClientNotifyMgr_callbackCtx*)arg;

#if defined (SYSLINK_BUILD_HLOS)
    osalThHandle = (OsalThread_Handle)ClientNotifyMgr_module->taskPostInfo.tskHandle;
    GT_assert(curTrace, (osalThHandle != NULL));
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
    semHandle = (Semaphore_Handle)ClientNotifyMgr_module->taskPostInfo.semHandle;
    GT_assert(curTrace, (semHandle != NULL));
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

    /* procId ,lineId and eventId can be ignored since we are not
     * passing them to application callback functions.
     */
    if (ClientNotifyMgr_module->taskPostInfo.notifyCnt == 0) {
        ClientNotifyMgr_module->taskPostInfo.readIndex = 0;
        ClientNotifyMgr_module->taskPostInfo.writeIndex = 0;
        ClientNotifyMgr_module->cliTsknotifyPaloadEntry[ClientNotifyMgr_module->taskPostInfo.writeIndex].notifyPayload = notifyPayload;
        ClientNotifyMgr_module->cliTsknotifyPaloadEntry[ClientNotifyMgr_module->taskPostInfo.writeIndex].cliCallbackCtx = cliCallbackCtx;
        ClientNotifyMgr_module->taskPostInfo.notifyCnt++;

    }
    else {
        ClientNotifyMgr_module->taskPostInfo.notifyCnt++;
        ClientNotifyMgr_module->taskPostInfo.writeIndex++;
        ClientNotifyMgr_module->taskPostInfo.writeIndex =
                                ClientNotifyMgr_module->taskPostInfo.writeIndex %
                                       CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES;
        /* Raise assert when notifyCnt becomes equal to CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES.
         * It means that  Interrupts are coming very fast before executing SWI.
         * If assert is raised, increase the CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES and
         * and rebuild.
         */
        GT_assert(curTrace, (ClientNotifyMgr_module->taskPostInfo.notifyCnt < CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES));

        ClientNotifyMgr_module->cliTsknotifyPaloadEntry[ClientNotifyMgr_module->taskPostInfo.writeIndex].notifyPayload = notifyPayload;
        ClientNotifyMgr_module->cliTsknotifyPaloadEntry[ClientNotifyMgr_module->taskPostInfo.writeIndex].cliCallbackCtx = cliCallbackCtx;
    }

#if defined (SYSLINK_BUILD_HLOS)
    OsalThread_activate(osalThHandle);
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
    Semaphore_post(semHandle);
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
}

/*=============================================================================
 *  Exported API
 *=============================================================================
 */

//TBD : Refcount
/*
 *  @brief  Function to get the default Module config parameters.
 *
 *  @param  config Configuration values.
 */
Void
ClientNotifyMgr_getConfig (ClientNotifyMgr_Config * config)
{

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_getConfig",config);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (config == NULL) {
        GT_setFailureReason (curTrace,
                         GT_4CLASS,
                         "ClientNotifyMgr_getConfig",
                         ClientNotifyMgr_E_INVALIDARG,
                         "Argument of type (ClientNotifyMgr_Config *) passed "
                         "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (ClientNotifyMgr_module->refCount == 0) {
            /* Return the default static config values */
            Memory_copy ((Ptr) config,
                         (Ptr) &(ClientNotifyMgr_module->defaultCfg),
                         sizeof (ClientNotifyMgr_Config));
        }
        else {
            Memory_copy ((Ptr) config,
                         (Ptr) &(ClientNotifyMgr_module->cfg),
                         sizeof (ClientNotifyMgr_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgr_getConfig");
}
/*
 *  ======== ClientNotifyMgr_sharedMemModReq ========
 *  API to find out the shared memory required for the  module.
 */
UInt32
ClientNotifyMgr_sharedMemModReq()
{
    UInt32  reqSize =  0;

    return (reqSize);
}
/*
 *  @brief  Function to setup(initialize) the ClientNotifyMgr module.
 *
 *  @param  config Config param structure.
 *  @sa         ClientNotifyMgr_destroy
 */
Int32
ClientNotifyMgr_setup(ClientNotifyMgr_Config* config)
{
    Int32                          status         = ClientNotifyMgr_S_SUCCESS;
    UInt32                         i;
    UInt32                         j;
    UInt32                         k;
    ClientNotifyMgr_Config         tmpCfg;
    List_Params                    listParams;
    IArg                           key;
    Error_Block                    eb;
#if defined (SYSLINK_BUILD_RTOS)
    GateSwi_Handle                 swiGate;
    Swi_Params                     swiParams;
    Task_Params                    taskParams;
    Semaphore_Params               semParams;
#endif

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_setup", config);

    GT_assert (curTrace, (config != NULL));
    Error_init (&eb);
    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    key = Gate_enterSystem();
    ClientNotifyMgr_module->refCount++;
    if (ClientNotifyMgr_module->refCount > 1) {
        status = ClientNotifyMgr_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "ClientNotifyMgr Module already initialized!");
        Gate_leaveSystem(key);
    }
    else {
        Gate_leaveSystem(key);

        if (config == NULL) {
            ClientNotifyMgr_getConfig (&tmpCfg);
            config = &tmpCfg;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (config->numProcessors > ClientNotifyMgr_module->numProcessors) {
            /*! @retval ClientNotifyMgr_E_INVALIDARG sharedAddrSize provided in
             * config is zero. or numProcessors specified is more than the
             * supported processors.
             */
            status = ClientNotifyMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_setup",
                                 status,
                                 "numProcessors specified is more than the "
                                 "supported processors!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (config->gate == NULL) {
                /* If user is not providing a local gate, create a local gate*/
                /*  Create local SWI gate  for module wide protection*/
#if defined(SYSLINK_BUILD_HLOS)
                ClientNotifyMgr_module->gate = (IGateProvider_Handle)
                             GateMutex_create ((GateMutex_Params*)NULL, &eb);
#endif /* if defined(SYSLINK_BUILD_HLOS) */
#if defined(SYSLINK_BUILD_RTOS)
                swiGate = GateSwi_create(NULL, NULL);
                if (swiGate != NULL) {
                    ClientNotifyMgr_module->gate = GateSwi_Handle_upCast(swiGate);
                }
#endif /* if defined(SYSLINK_BUILD_RTOS) */
                ClientNotifyMgr_module->usrGate = FALSE;
            }
            else {
                ClientNotifyMgr_module->usrGate = TRUE;
                ClientNotifyMgr_module->gate = config->gate;
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            if (ClientNotifyMgr_module->gate == NULL) {
                /*! @retval ClientNotifyMgr_E_CREATE_GATE
                 * Failed to createmodule gate
                 */
                status = ClientNotifyMgr_E_CREATE_GATE;
                GT_setFailureReason (
                               curTrace,
                               GT_4CLASS,
                               "ClientNotifyMgr_setup",
                               status,
                               "Failed to create module gate!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                ClientNotifyMgr_module->numProcessors =
                                                  config->numProcessors;
                for (k = 0; k < MultiProc_getNumProcessors();k++ ) {
                    for (i = 0; i < ClientNotifyMgr_maxInstances;i++ ) {
                        for (j = 0; j < MultiProc_getNumProcessors();j++ ) {
                            ClientNotifyMgr_module->
                                         isEventRegistered[k][i][j] = FALSE;
                            ClientNotifyMgr_module->
                                         callBackCtxPtrs[k][i][j] = NULL;
                        }
                        ClientNotifyMgr_module->instRefCount[k][i] = 0;
                    }
                }


                for (i = 0; i < ClientNotifyMgr_maxInstances; i++) {
                    ClientNotifyMgr_module->isInstNoValid[i] = FALSE;
                }

                for (j = 0; j < CLIENTNOTIFYMGR_SWI_PAYLOAD_ENTRIES; j++) {
                    ClientNotifyMgr_module->notifyPaloadEntry[j].cliCallbackCtx
                                                                   = NULL;
                    ClientNotifyMgr_module->notifyPaloadEntry[j].notifyPayload
                                                                   = 0xFFFFFFFF;
                }

                /* Create a SWI for the module that  calls the application call
                 * back functions in SWI mode
                 */
#if defined (SYSLINK_BUILD_RTOS)
                Swi_Params_init(&swiParams);
                /* Not specifying any arguments currently */
                swiParams.priority = CLIENTNOTIFYMGR_SWI_PRIORITY;
                ClientNotifyMgr_module->swiPostInfo.swiHandle = Swi_create(ClientNotifyMgr_swiFxn, &swiParams, NULL);

                for (j = 0; j < CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES; j++) {
                    ClientNotifyMgr_module->cliTsknotifyPaloadEntry[j].cliCallbackCtx = NULL;
                    ClientNotifyMgr_module->cliTsknotifyPaloadEntry[j].notifyPayload = 0xFFFFFFFF;
                }

                Semaphore_Params_init (&semParams);
                /* Not specifying any arguments currently */
                semParams.mode = Semaphore_Mode_BINARY;
                ClientNotifyMgr_module->taskPostInfo.semHandle =
                                          Semaphore_create (0, &semParams, NULL);
                /* Create a Task for the module that  calls the application call back
                 * functions in Thread context
                 */
                Task_Params_init(&taskParams);
                /* Not specifying any arguments currently */
                taskParams.priority = CLIENTNOTIFYMGR_TSK_PRIORITY;
                ClientNotifyMgr_module->taskPostInfo.tskHandle =
                                         Task_create (ClientNotifyMgr_threadFxn,
                                                      &taskParams,
                                                      NULL);

#endif /* #if defined (SYSLINK_BUILD_RTOS)  */

                /* Create Kernel threads and task lets */
                /* TODO: Check  if Name server is configured */
                if (ClientNotifyMgr_module->nameServer == NULL ) {
                    /* Create nameserver. let NameServer create gate for
                     * it
                     */
                    ClientNotifyMgr_module->nameServer =
                        ClientNotifyMgr_setNameServer(
                                 CLIENTNOTIFYMGR_NAMESERVERNAME,
                                 NULL);
                    if (ClientNotifyMgr_module->nameServer == NULL) {
                        /* Failed to crate the name server */
                        /*! @retval ClientNotifyMgr_E_CREATE_NAMESERVER
                         * Failed to create Name Server
                         */
                        status =  ClientNotifyMgr_E_CREATE_NAMESERVER;
                        GT_setFailureReason (
                                       curTrace,
                                       GT_4CLASS,
                                       "ClientNotifyMgr_setup",
                                       status,
                                       "Failed to create Name Server!");
                    }
                    else {
                        ClientNotifyMgr_module->nameServerType =
                                         ClientNotifyMgr_DYNAMIC_CREATE;
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            }
            if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Construct the list object */
                List_Params_init (&listParams);
                ClientNotifyMgr_module->objList =  List_create (&listParams, &eb);

                for (i= 0; i < MultiProc_getNumProcessors(); i++ ) {
                    for (j = 0; j < ClientNotifyMgr_maxInstances; j++) {
                       List_Params_init (&listParams);
                       ClientNotifyMgr_module->eventListenerList[i][j] =
                                        List_create (&listParams, &eb);
                    }
                }
                /* Store the config */
                Memory_copy ((Ptr) &ClientNotifyMgr_module->cfg,
                             (Ptr) config,
                             sizeof (ClientNotifyMgr_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            }
            else {
                if (   (ClientNotifyMgr_module->nameServer != NULL)
                    && (ClientNotifyMgr_module->nameServerType
                        == ClientNotifyMgr_DYNAMIC_CREATE)) {
                    status = ClientNotifyMgr_deleteNameServer (
                                  &ClientNotifyMgr_module->nameServer);
                    GT_assert (curTrace, (status >= 0));
                }
                ClientNotifyMgr_module->refCount = 0;
                /*! @retval ClientNotifyMgr_E_FAIL Failed to create the
                 * listLock
                 */
                status = ClientNotifyMgr_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ClientNotifyMgr_setup",
                                     status,
                                     "Failed to create the listLock!");
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_setup", status);

    /*!< ClientNotifyMgr_S_SUCCESS if module setup is done successfully */
    return (status);
}

/*
 *  @brief  Function to destroy(finalize) the ClientNotifyMgr module.
 *
 */
Int32
ClientNotifyMgr_destroy(void)
{
    Int32   status     = ClientNotifyMgr_S_SUCCESS;
    UInt32  i;
    UInt32  j;
    IArg    key;

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount < 0)
         {
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

        key = Gate_enterSystem();
        ClientNotifyMgr_module->refCount--;
        if (ClientNotifyMgr_module->refCount == 0) {
        Gate_leaveSystem(key);
            if (ClientNotifyMgr_module->usrGate != TRUE) {
                /* Delete the module wide local protection gate */
                if (ClientNotifyMgr_module->gate != NULL) {
#if defined (SYSLINK_BUILD_HLOS)
                    status = GateMutex_delete (
                             (GateMutex_Handle*)&ClientNotifyMgr_module->gate);
                    GT_assert (curTrace, (status >= 0));
#endif /* #if defined (SYSLINK_BUILD_HLOS) */
#if defined (SYSLINK_BUILD_RTOS)
                    IGateProvider_delete(&(ClientNotifyMgr_module->gate));
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                }
            }
#if defined (SYSLINK_BUILD_RTOS)
            Swi_delete((ti_sysbios_knl_Swi_Handle *)
                                &ClientNotifyMgr_module->swiPostInfo.swiHandle);
            Task_delete((ti_sysbios_knl_Task_Handle *)
                               &ClientNotifyMgr_module->taskPostInfo.tskHandle);
            Semaphore_delete((ti_sysbios_knl_Semaphore_Handle *)
                               &ClientNotifyMgr_module->taskPostInfo.semHandle);
#endif /* #if defined (SYSLINK_BUILD_HLOS) */

                /* Delete the name servers */
            if (  (ClientNotifyMgr_module->nameServerType ==
                   ClientNotifyMgr_DYNAMIC_CREATE)
                &&(ClientNotifyMgr_module->nameServer != NULL)){
                ClientNotifyMgr_deleteNameServer (
                                           &ClientNotifyMgr_module->nameServer);
            }
            /* Destruct the list object */
            List_delete (&ClientNotifyMgr_module->objList);
            for (i= 0; i < MultiProc_getNumProcessors(); i++ ) {
                for (j = 0; j < ClientNotifyMgr_maxInstances; j++) {
                    List_delete (&ClientNotifyMgr_module->eventListenerList[i][j]);
                }
            }

            /* Clear cfg area */
            Memory_set ((Ptr) &ClientNotifyMgr_module->cfg,
                        0,
                        sizeof (ClientNotifyMgr_Config));
        }
        else {
            /* Leave the gate here in case non-zero refcount */
            Gate_leaveSystem(key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)  && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

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

    GT_1trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_Params_init",
               params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_Params_init",
                             ClientNotifyMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_Params_init",
                             ClientNotifyMgr_E_FAIL,
                             "Argument of type (ClientNotifyMgr_Params *) is "
                             "NULL!");
   }
   else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Return the default config parameters */
            params->name                  = NULL;
            params->eventNo               = 0u;/* TODO: assign with invalid event no Get it from notify module*/
            params->openFlag              = FALSE;/* By default create it */
            params->creatorProcId         = MultiProc_self();
            params->sharedAddr            = NULL;
            params->sharedAddrSize         = 0u;
            params->gate                  = NULL;
            params->cacheFlags            = 0u;
            params->numSubNotifyEntries   = 1u;
            params->numNotifyEntries      = 1u;
            params->regionId              = 0;
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
   }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_Params_init");

}
/*!
 * @brief  Function to create a ClientNotifyMgr instance
 *
 * @param  instParams Instance param structure.
 */
ClientNotifyMgr_Handle
ClientNotifyMgr_create (ClientNotifyMgr_Params * params)
{
    ClientNotifyMgr_Handle  handle = NULL;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_create", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)  && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_Params_init",
                             ClientNotifyMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_create",
                             ClientNotifyMgr_E_INVALIDARG,
                             "Argument of type (ClientNotifyMgr_Params *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Call is from kernel space */
        handle = _ClientNotifyMgr_create (params);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_create", handle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */

    return (handle);

}

/*!
 * @brief  Function to delete a ClientNotifyMgr instance
 *
 * @param  pHandle Pointer to instance handle.
 */
Int32
ClientNotifyMgr_delete( ClientNotifyMgr_Handle *pHandle)
{
    Int32                            status = ClientNotifyMgr_S_SUCCESS;
//    UInt16                           lprocId;
    UInt32                           i;
    ClientNotifyMgr_Handle           handle;
    ClientNotifyMgr_Obj             *obj;
    IArg                             key;
    IArg                             key0;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;
    UInt32                           numNotifyEntries;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_delete", pHandle);

    GT_assert (curTrace, (pHandle != NULL));
    GT_assert (curTrace, ((pHandle != NULL) && (*pHandle != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_Params_init",
                             ClientNotifyMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (pHandle == NULL) {
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_create",
                             ClientNotifyMgr_E_INVALIDARG,
                             "Argument of type (ClientNotifyMgr_Params *) is "
                             "NULL!");
    }
    else if (*pHandle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_delete",
                             status,
                             "*pHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        handle = *pHandle;
        obj = (ClientNotifyMgr_Obj *) handle->obj;
        GT_assert (curTrace, (NULL != obj));
//        lprocId = MultiProc_self();

        switch (obj->type) {
        case ClientNotifyMgr_DYNAMIC_OPEN :
        {
            numNotifyEntries = obj->attrs->numNotifyEntries;

            key0 = IGateProvider_enter (ClientNotifyMgr_module->gate);
            /* Remove it from the local list */
            List_remove (ClientNotifyMgr_module->objList,
                         &obj->listElem);
            IGateProvider_leave (ClientNotifyMgr_module->gate, key0);

            if (obj->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)obj->attrs,
                             sizeof (ClientNotifyMgr_Attrs),
                             Cache_Type_ALL,
                             TRUE);
            }
            instId = obj->attrs->entryId & 0xFFFF;
            /* Find out the creator procid */
            creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                          & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
            instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                      & CLIENTNOTIFYMGR_INSTNO_MASK);
            GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

            if (instNo >= ClientNotifyMgr_maxInstances) {
                status = ClientNotifyMgr_E_INVALIDINSTID;
            }
            else {
                /* No need to unregister the notification.Will be taken care by the
                 * caller
                 */
                 /* Need to unregister event*/
                 key = IGateProvider_enter (ClientNotifyMgr_module->gate);
                 /* [creProcId][instNo] combination identifies the
                  * instane uniquely in the  system.
                  */
                 ClientNotifyMgr_module->instRefCount[creProcId][instNo] -= 1;
                 if (ClientNotifyMgr_module->instRefCount[creProcId][instNo] == 0) {
                /* Unregister event number with the notify drivers */
                    for (i = 0;
                         i < MultiProc_getNumProcessors (); i++ ) {
                        if (Notify_intLineRegistered (i,CLIENTNOTIFYMGR_INT_LINEID)) {
                            if (ClientNotifyMgr_module->
                                            isEventRegistered[creProcId][instNo][i]
                                == TRUE){
                                if(obj->cacheEnabled == TRUE) {
                                    Cache_inv ((Ptr)obj->attrs,
                                               sizeof(ClientNotifyMgr_Attrs),
                                               Cache_Type_ALL,
                                               TRUE);
                                }
                                ClientNotifyMgr_unregisterWithNotify(
                                            GateMP_getLocalProtect(obj->gate),
                                            i,
                                            CLIENTNOTIFYMGR_INT_LINEID,
                                            obj->attrs->eventNo,
                                            (UArg)ClientNotifyMgr_module->
                                            callBackCtxPtrs[creProcId][instNo][i]);
                                Memory_free (
                                          NULL,
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
                IGateProvider_leave (ClientNotifyMgr_module->gate, key);

                if (obj->notifyInfoObj != NULL) {
                    Memory_free (NULL,
                                obj->notifyInfoObj,
                                sizeof(UInt32) * numNotifyEntries);
                }

                /* Now free the handle */
                Memory_free (NULL, obj, sizeof (ClientNotifyMgr_Obj));
                obj = NULL;
                Memory_free (NULL, handle, sizeof (ClientNotifyMgr_Object));
                handle = NULL;
            }
            break;
        }

        case ClientNotifyMgr_DYNAMIC_CREATE :
        {
            numNotifyEntries = obj->attrs->numNotifyEntries;

            key0 = IGateProvider_enter (ClientNotifyMgr_module->gate);
            /* Remove it from the local list */
            List_remove (ClientNotifyMgr_module->objList,
                         &obj->listElem);
            IGateProvider_leave (ClientNotifyMgr_module->gate, key0);

            if (obj->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)obj->attrs,
                           sizeof (ClientNotifyMgr_Attrs),
                           Cache_Type_ALL,
                           TRUE);
            }
            instId = obj->attrs->entryId & 0xFFFF;
            /* Find out the creator procid */
            creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                          & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
            instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                      & CLIENTNOTIFYMGR_INSTNO_MASK);
            GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

            if (instNo >= ClientNotifyMgr_maxInstances) {
                status = ClientNotifyMgr_E_INVALIDINSTID;
            }
            else {
                /* Remove name from the nameServer */
                if (   (NULL != ClientNotifyMgr_module->nameServer)
                    && (obj->nsKey != 0)) {
                    NameServer_removeEntry((NameServer_Handle)ClientNotifyMgr_module->nameServer,
                                           obj->nsKey);
                }
                obj->attrs->status =  !ClientNotifyMgr_CREATED;
                if (obj->cacheEnabled == TRUE) {
                    Cache_wbInv ((Ptr)obj->attrs,
                                 sizeof (ClientNotifyMgr_Attrs),
                                 Cache_Type_ALL,
                                 TRUE);
                }

                key = IGateProvider_enter ( ClientNotifyMgr_module->gate);

                /*Make the entry invlid in the local list */
                ClientNotifyMgr_module->isInstNoValid[instNo] = FALSE;

                 /* Need to unregister event*/
                 ClientNotifyMgr_module->instRefCount[creProcId][instNo] -= 1;
                 if (ClientNotifyMgr_module->instRefCount[creProcId][instNo] == 0) {
                    /* Unregister event number with the notify drivers */
                    for (i = 0;
                         i < MultiProc_getNumProcessors ();
                         i++ ) {
                        if (Notify_intLineRegistered (i,CLIENTNOTIFYMGR_INT_LINEID)) {
                         /*[creProcId][instNo] combination identifies the unique
                          * instance in the system
                          */
                            if (ClientNotifyMgr_module->
                                        isEventRegistered[creProcId][instNo][i]
                                 == TRUE) {
                                if(obj->cacheEnabled == TRUE) {
                                    Cache_inv ((Ptr)obj->attrs,
                                               sizeof(ClientNotifyMgr_Attrs),
                                               Cache_Type_ALL,
                                               TRUE);
                                }
                                ClientNotifyMgr_unregisterWithNotify(
                                            GateMP_getLocalProtect(obj->gate),
                                            i,
                                            CLIENTNOTIFYMGR_INT_LINEID,
                                            obj->attrs->eventNo,
                                            (UArg)ClientNotifyMgr_module->
                                            callBackCtxPtrs[creProcId][instNo][i]);
                                Memory_free (
                                         NULL,
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

                IGateProvider_leave (ClientNotifyMgr_module->gate, key);
                if (obj->notifyInfoObj != NULL) {
                    Memory_free (NULL,
                                obj->notifyInfoObj,
                                sizeof(UInt32) * numNotifyEntries);
                }

                /* Now free the handle */
                Memory_free (NULL, obj, sizeof (ClientNotifyMgr_Obj));
                obj = NULL;
                Memory_free (NULL, handle, sizeof (ClientNotifyMgr_Object));
                handle = NULL;
            }
            break;
        }
        case  ClientNotifyMgr_STATIC_OPEN :
        {
            break;
        }

        case ClientNotifyMgr_STATIC_CREATE:
        {
            break;
        }
        default:
            status = ClientNotifyMgr_E_ACCESSDENIED;
            /*! @retval ClientNotifyMgr_E_ACCESSDENIED  Instance is not
             * dynamically created or opened one.
             */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_delete",
                                 status,
                                 "Invalid Handle!");

            break;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_delete", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS Operation successful */
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

    GT_assert (curTrace, (pHandle != NULL));
    GT_assert (curTrace, ((pHandle != NULL) && (*pHandle != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_close",
                             status,
                             "Module was not initialized!");
    }
    else if (pHandle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
     GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_close",
                             status,
                             "pHandle passed is NULL!");
    }
    else if (*pHandle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_close",
                             status,
                             "*pHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = ClientNotifyMgr_delete (pHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)&& defined (SYSLINK_BUILD_HLOS) */

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
                               ClientNotifyMgr_registerNoitifyParams *pRegParams,
                               UInt32                 *id)
{
    Int32                              status        = ClientNotifyMgr_S_SUCCESS;
    UInt32                             i             = 0;
    Bool                               found         = FALSE;
    volatile ClientNotifyMgr_Attrs             *pAttrs ;
    IArg                               key;
    IArg                               lkey;
    ClientNotifyMgr_Obj               *obj;
    ClientNotifyMgr_eventListenerInfo *listener;
    UInt32                             k;
    UInt16                             instId;
    UInt16                             instNo;
    UInt16                             creProcId;

    GT_2trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_registerClient",
               handle,
               pRegParams);

    GT_assert (curTrace,
               (   (NULL != handle)
                && (NULL != pRegParams->fxnPtr)
                && (NULL != id)));

    GT_assert (curTrace,
               (NULL != handle->obj));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "handle passed is NULL!");
    }
    else if (handle->obj == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_registerClient",
                             status,
                             "obj pointed by handle is NULL!");
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
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        obj     = handle->obj;
        *id = (UInt32)-1;
        pAttrs = obj->attrs;

        instId = obj->Id & 0xFFFF;
        /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                      & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);
        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

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
             listener = (ClientNotifyMgr_eventListenerInfo *)Memory_calloc (
                                     NULL,
                                     sizeof (ClientNotifyMgr_eventListenerInfo),
                                     0,
                                     NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
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
                lkey = IGateProvider_enter (ClientNotifyMgr_module->gate);
                List_put (
                    ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                   &(listener->listElem));
                IGateProvider_leave (ClientNotifyMgr_module->gate, lkey);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            }
#endif
        }
        else {
            /*! @retval ClientNotifyMgr_E_FAIL  Notification call back function
             * is already registered .Call unregister and then register again.
             */
            status = ClientNotifyMgr_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_delete",
                                 status,
                                 "Notification call back function is already "
                                 "registered .Call unregister and then register"
                                 "again.!");
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_registerClient", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully registered the call back
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
    IArg                             lkey;
    ClientNotifyMgr_Obj             *obj;
    List_Elem                       *elem;
    UInt32                           k;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;

    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_unregisterClient", handle);

    GT_assert (curTrace, (NULL != handle) );

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {
        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_unregisterClient",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_unregisterClient",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     = handle->obj;
        GT_assert (curTrace, (id < obj->numNotifyEntries) );
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (id >= obj->numNotifyEntries) {
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
            pAttrs = obj->attrs;
            instId = obj->Id & 0xFFFF;
            /* Find out the creator procid */
            creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                          & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
            instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                      & CLIENTNOTIFYMGR_INSTNO_MASK);
            GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

            if (instNo >= ClientNotifyMgr_maxInstances) {
                /*! @retval ClientNotifyMgr_E_INVALIDINSTID  instNo of the instance is more
                 * than the maxInstances configured.
                 */
                status = ClientNotifyMgr_E_INVALIDINSTID;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ClientNotifyMgr_unregisterClient",
                                     status,
                                     "instNo of the instance is more"
                                     "than the maxInstances configured.!");
            }
            else {
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
                    obj->notifyInfoObj[id]->notifyType     =
                                                  ClientNotifyMgr_NOTIFICATION_NONE;
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
                    for ((elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                         NULL));
                         elem != NULL;
                         elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                         elem)) {
                        if (   ((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId
                                == id) {
                            /* List is protecting itself */
                            lkey = IGateProvider_enter (ClientNotifyMgr_module->gate);
                            List_remove (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                                        elem);
                            IGateProvider_leave (ClientNotifyMgr_module->gate, lkey);
                            Memory_free (NULL,
                                     elem,
                                     sizeof (ClientNotifyMgr_eventListenerInfo));
                            break;
                        }
                    }
                }
                else {
                    /*! @retval ClientNotifyMgr_E_FAIL  Notification call back function
                     * is already uregistered or Not registred previousely.
                     */
                    status = ClientNotifyMgr_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ClientNotifyMgr_unregisterClient",
                                         status,
                                         "Notification call back function is already "
                                         "uregistered .Cr Not registred previousely.!");
                }
                GateMP_leave (obj->gate, key);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_unregisterClient", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully unregistered the call
     * back.
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
    IArg                                      key;
    ClientNotifyMgr_Obj                      *obj;

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_enableNotification",
               handle,
               id,
               subId);

    GT_assert (curTrace,(NULL != handle));
    GT_assert (curTrace,(subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_enableNotification",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_enableNotification",
                             status,
                             "handle passed is NULL!");
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
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        obj     = handle->obj;
        GT_assert(curTrace, (obj != NULL) );
        GT_assert (curTrace,(id < obj->numNotifyEntries));

        key = GateMP_enter (obj->gate);

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)obj->notifyInfoObj[id],
                       sizeof (ClientNotifyMgr_NotifyParamInfo),
                       Cache_Type_ALL,
                       TRUE);
        }
        obj->notifyInfoObj[id]->notifyFlag[subId]     = TRUE;
        if (obj->cacheEnabled == TRUE) {
            Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                         sizeof (ClientNotifyMgr_NotifyParamInfo),
                         Cache_Type_ALL,
                         TRUE);
        }

        GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_enableNotification", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully enabled notification.
     */
   return ( status);
}

/*
 *  ======== ClientNotifyMgr_waitOnMultiSubEntryNotify ========
 * API to enable notification for subnotification entries of a given notifyid
 * (key). It also sets waitOnMultiNotifyFlags flag to enable call back function
 * to call the APP call back function only when event flags of the all the
 * mentioned subIds are set.
 */
Int32 ClientNotifyMgr_waitOnMultiSubEntryNotify (
                                              ClientNotifyMgr_Handle  handle,
                                              UInt32                  id,
                                              UInt32                  subId[],
                                              UInt32                  numSubIds)
{
    Int32                           status = ClientNotifyMgr_S_SUCCESS;
    IArg                                      key;
    UInt32                           i;
    UInt32                           j;
    ClientNotifyMgr_Obj             *obj;

    GT_4trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_waitOnMultiSubEntryNotify",
               handle,
               id,
               subId,
               numSubIds);

    GT_assert (curTrace,(NULL != handle));
    GT_assert (curTrace,(NULL != subId));
    GT_assert (curTrace,(numSubIds < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_waitOnMultiSubEntryNotify",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_waitOnMultiSubEntryNotify",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        obj     = handle->obj;
        GT_assert(curTrace, (obj != NULL) );
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (id >= obj->numNotifyEntries) {
            /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
            status = ClientNotifyMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_SendForceNotification",
                                 status,
                                 "id is more than the numNotifyentries!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            key = GateMP_enter (obj->gate);

            if (obj->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)obj->notifyInfoObj[id],
                           sizeof (ClientNotifyMgr_NotifyParamInfo),
                           Cache_Type_ALL,
                           TRUE);
            }
            /* Wait on multiple events  */
            obj->notifyInfoObj[id]->waitOnMultiNotifyFlags = TRUE;
            for (i = 0; i < numSubIds; i++) {
                GT_assert (curTrace,
                           (subId[i] < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));
                obj->notifyInfoObj[id]->notifyFlag[subId[i]]     = TRUE;
                j = (  subId[i]
                     /(sizeof(obj->notifyInfoObj[id]->bitFlag_notifySubEntries[0]) * 8));
                /* Set the flag corresponding to the subNotifyEntry.Each element
                 * in bitFlag_notifySubEntries represent 32 subNotifyEntries
                 */
                obj->notifyInfoObj[id]->bitFlag_notifySubEntries[j] |= (1 << subId[i]);
            }

            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                             sizeof (ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
            }

            GateMP_leave (obj->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
    }
#endif

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_waitOnMultiSubEntryNotify",
               status);
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
    IArg                                      key;
    ClientNotifyMgr_Obj                      *obj;

    GT_1trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_disableNotification",
              id);

    GT_assert (curTrace,(NULL != handle));
    GT_assert (curTrace,(NULL != handle->obj));

    GT_assert (curTrace, (subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_disableNotification",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_disableNotification",
                             status,
                             "handle passed is NULL!");
    }
    else if (handle->obj == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_disableNotification",
                             status,
                             "obj pointed to by handle is NULL!");
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
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     = handle->obj;
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (id >= obj->numNotifyEntries) {
            /*! @retval ClientNotifyMgr_E_INVALIDARG id passed is mor than max
             * clients
             */
            status = ClientNotifyMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_disableNotification",
                                 status,
                                 "subId passed is more than "
                                 "the configured entries !");
        }
        else {
#endif
            key = GateMP_enter (obj->gate);
            if (obj->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)obj->notifyInfoObj[id],
                           sizeof (ClientNotifyMgr_NotifyParamInfo),
                           Cache_Type_ALL,
                           TRUE);
            }

            obj->notifyInfoObj[id]->notifyFlag[subId]  = FALSE;

            if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                             sizeof (ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
            }
            GateMP_leave (obj->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_disableNotification",
               status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully disabled notification
     */
     return (status);
}

/*!
 *  @brief     Function to send notification to the client identified the id.
 *
 *  @param  handle     Instance handle.
 *  @param  id         Id of the remote client to which notification needs to be
 *                     sent.see {@link #registerClient}.Caller client needs to
 *                     know the remote client's notifyId to be able to send
 *                     notification.
 *  @param  arg1       Value to checked against water mark cond1.
 *  @param  arg2       Value to checked against water mark cond2 of the
 *                     other client.
 *
 */
Int32
ClientNotifyMgr_sendNotification (ClientNotifyMgr_Handle handle,
                                  UInt32                  slot,
                                  UInt32                  subId,
                                  UInt32                  arg1,
                                  UInt32                  arg2)
{
    Int32                            status = ClientNotifyMgr_S_SUCCESS;
    UInt32                           i      = 0;
    volatile ClientNotifyMgr_Attrs           *pAttrs;
    IArg                             key;
    Bool                             sendEvent[10] ={0};
    UInt32                           procId;
    UInt32                           remProcId;
    UInt32                           notifyPayload;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;
    ClientNotifyMgr_Obj              *obj;
#if defined (SYSLINK_BUILD_RTOS)
    List_Elem                        *elem;
    Ptr                              cbContext;
    ClientNotifyMgr_FnCbck           callBackFxn;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

    GT_4trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendNotification",
               handle,
               slot,
               arg1 ,
               arg2);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (handle->obj != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotification",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotification",
                             status,
                             "handle passed is NULL!");
    }
    else if (handle->obj == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotification",
                             status,
                             "obj pointed by handle is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     =  handle->obj;

        GT_assert (curTrace, (slot < obj->numNotifyEntries));
        GT_assert (curTrace, (subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

        i = slot;
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
        remProcId =  obj->notifyInfoObj[i]->procId;
        /* Send notification to the  other client */
        /* For normal notificatios application spcific payload is zero.So
         * last two MSBs arezero in notifyPayload*/
        notifyPayload = pAttrs->entryId & 0xFFFF;
        instId = pAttrs->entryId & 0xFFFF;
        /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                     & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);

        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

        /* There is difference in calling callbacks on HLOS and RTOS hence the
         * the following define is needed.
         */

#if defined (SYSLINK_BUILD_RTOS)
        /* Check if the  remote client is on the local processor. if yes call
         * the registered call back function directly
         */
        if ((sendEvent[remProcId] == TRUE) && (procId == remProcId)) {
            if (obj->notifyInfoObj[i]->eventCount[subId] > 0) {
                /* Call the call back function Directly  since it is on the
                 * local processor.
                 */
                obj->notifyInfoObj[i]->eventCount[subId]--;
                if (obj->cacheEnabled == TRUE) {
                    Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                                 sizeof(ClientNotifyMgr_NotifyParamInfo),
                                 Cache_Type_ALL,
                                 TRUE);
                }
                for ((elem = List_next (ClientNotifyMgr_module->
                                           eventListenerList[creProcId][instNo],
                       NULL));
                      elem != NULL;
                      elem = List_next (ClientNotifyMgr_module->
                                           eventListenerList[creProcId][instNo],
                      elem)) {
                    if (   ((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId
                            == i) {
                        callBackFxn =
                        ((ClientNotifyMgr_eventListenerInfo*)elem)->cbckFxn;
                        cbContext =
                               ((ClientNotifyMgr_eventListenerInfo*)elem)->
                                                                  cbContext;
                        if (callBackFxn != NULL) {
                            /* Call to call back function */
                            callBackFxn ((Ptr)obj->notifyInfoObj[i]->clientHandle,
                                         cbContext,
                                         notifyPayload);
                        }
                        break;
                    }
                }
            }
        }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

        GateMP_leave (obj->gate, key);

            /* Send notification to the  other client identified by the id*/
        if (   (sendEvent[remProcId] == TRUE)
            && (Notify_intLineRegistered (remProcId, CLIENTNOTIFYMGR_INT_LINEID)))  {
                /* For normal notificatios application spcific payload is zero.So
                 * last two MSBs arezero in notifyPayload*/
#if defined (SYSLINK_BUILD_RTOS)
             if (procId != remProcId) {
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                    status = Notify_sendEvent (remProcId,
                                               CLIENTNOTIFYMGR_INT_LINEID,
                                               CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                               notifyPayload,
                                               TRUE );
#if defined (SYSLINK_BUILD_RTOS)
            }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_sendNotification", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully sends notifications.
     */
    return (status);
}



Int32
ClientNotifyMgr_sendNotificationMulti (ClientNotifyMgr_Handle  handle,
                                       UInt32                  slot,
                                       UInt32                  subId,
                                       UInt32                  arg1,
                                       UInt32                  arg2)

{

    Int32                            status  = ClientNotifyMgr_S_SUCCESS;
    UInt32                           counter = 0;
    volatile ClientNotifyMgr_Attrs           *pAttrs;
    IArg                             key;
    Bool                             sendEvent[10] = {0};/* TODO :Replace with max
                                                           processors*/
    UInt32                           procId;
    UInt32                           i;
    UInt32                           notifyPayload;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;
    ClientNotifyMgr_Obj             *obj;
#if defined (SYSLINK_BUILD_RTOS)
    List_Elem                        *elem;
    Ptr                              cbContext;
    ClientNotifyMgr_FnCbck           callBackFxn;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */


    GT_4trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendNotificationMulti",
               handle,
               slot,
               arg1 ,
               arg2);

    GT_assert (curTrace, (NULL != handle));
    GT_assert (curTrace, (handle->obj != NULL));
    GT_assert (curTrace, (subId < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotificationMulti",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotificationMulti",
                             status,
                             "handle passed is NULL!");
    }
    else if (handle->obj == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendNotificationMulti",
                             status,
                             "obj pointed by handle is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     =  handle->obj;

        pAttrs =   obj->attrs;

        key = GateMP_enter ((GateMP_Handle)obj->gate);

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs, sizeof (ClientNotifyMgr_Attrs), Cache_Type_ALL,TRUE);
        }

        instId = pAttrs->entryId & 0xFFFF;
         /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                     & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);
        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));
        for (i = 0;
             i < pAttrs->numNotifyEntries && counter < pAttrs->numActClients;
             i++) {
                if (i != slot) {
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
                                     GT_assert ( curTrace,
                                              (  procId
                                               < MultiProc_getNumProcessors()));
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
                                         GT_assert ( curTrace,
                                                  (  procId
                                                   < MultiProc_getNumProcessors()));
                                        sendEvent[procId] = 1u;
                                        obj->notifyInfoObj[i]->eventCount[subId]++;/* Event has to be sent */
                                        obj->notifyInfoObj[i]->notifyFlag[subId]= FALSE;
                                }

                            }
                            break;

                        case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS:
                            {
                                if (arg1 >= obj->notifyInfoObj[i]->waterMarkCond1) {
                                        procId = obj->notifyInfoObj[i]->procId;
                                     GT_assert ( curTrace,
                                              (  procId
                                               < MultiProc_getNumProcessors()));
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

        /* There is difference in calling callbacks on HLOS and RTOS hence the
         * the following define is needed.
         */
#if defined (SYSLINK_BUILD_RTOS)
        /* Call Registered call back functions of the clients on this processor */
        counter = 0;
        if (sendEvent[procId] == TRUE ) {
            for (i = 0;
                 i < pAttrs->numNotifyEntries && counter < pAttrs->numActClients;
                 i++) {
                if (obj->notifyInfoObj[i]->isValid == TRUE) {
                    counter ++;
                }
                if (   (obj->notifyInfoObj[i]->procId == procId)
                    && (obj->notifyInfoObj[i]->eventCount[subId] > 0)) {
                    obj->notifyInfoObj[i]->eventCount[subId]--;
                    if (obj->cacheEnabled == TRUE) {
                        Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                                     sizeof(ClientNotifyMgr_NotifyParamInfo),
                                     Cache_Type_ALL,
                                     TRUE);
                    }
                    for ((elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                           NULL));
                          elem != NULL;
                          elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                          elem)) {
                        if (   ((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId
                                == i) {
                           callBackFxn =
                            ((ClientNotifyMgr_eventListenerInfo*)elem)->cbckFxn;
                           cbContext =
                            ((ClientNotifyMgr_eventListenerInfo*)elem)->
                                                                      cbContext;
                            if (callBackFxn != NULL) {
                               /* Call to call back function */
                                callBackFxn ((Ptr)obj->notifyInfoObj[i]->clientHandle,
                                             cbContext,
                                             notifyPayload);
                           }
                            break;
                        }
                    }
                }
            }
        }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

        GateMP_leave ((GateMP_Handle)obj->gate, key);

        /* Send notification to all the eligible clients */
        for (i = 0; i < MultiProc_getNumProcessors(); i++) {
                /* Send notification to the  other client identified by the id*/
            if (   (sendEvent[i] == TRUE)
                && (Notify_intLineRegistered (i, CLIENTNOTIFYMGR_INT_LINEID)))  {
                    /* For normal notificatios application spcific payload is zero.So
                     * last two MSBs arezero in notifyPayload*/
#if defined (SYSLINK_BUILD_RTOS)
                 if (i != procId) {
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                    status = Notify_sendEvent (i,
                                               CLIENTNOTIFYMGR_INT_LINEID,
                                               CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                               notifyPayload,
                                               TRUE );
#if defined (SYSLINK_BUILD_RTOS)
                }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
//     GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_sendNotificationMulti", status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully sends notifications.
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
    UInt32                           remProcId;
    UInt32                           notifyPayload;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;
    ClientNotifyMgr_Obj              *obj;
#if defined (SYSLINK_BUILD_RTOS)
    List_Elem                        *elem;
    Ptr                              cbContext;
    ClientNotifyMgr_FnCbck           callBackFxn;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendForceNotification",
               handle,
               id,
               payLoad);

    GT_assert (curTrace, (NULL != handle));


#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotification",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotification",
                             status,
                             "handle passed is NULL!");
    }
    else if (handle->obj == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotification",
                             status,
                             "obj pointed by handle is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     =  (ClientNotifyMgr_Obj*)handle->obj;
        GT_assert (curTrace, (id < obj->numNotifyEntries));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (id >= obj->numNotifyEntries) {
            /*! @retval ClientNotifyMgr_E_INVALIDARG handle->obj passed is NULL*/
            status = ClientNotifyMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgr_SendForceNotification",
                                 status,
                                 "id is more than the numNotifyentries!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            pAttrs  =   obj->attrs;
            /* Acquire lock */
            key = GateMP_enter ((GateMP_Handle)obj->gate);

            if (obj->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)pAttrs, sizeof (ClientNotifyMgr_Attrs), Cache_Type_ALL,TRUE);
            }
            if (obj->cacheEnabled == TRUE) {
                Cache_inv ((Ptr)obj->notifyInfoObj[id],
                           sizeof(ClientNotifyMgr_NotifyParamInfo),
                           Cache_Type_ALL,
                           TRUE);
            }

            instId = pAttrs->entryId & 0xFFFF;
             /* Find out the creator procid */
            creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                         & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
            instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                      & CLIENTNOTIFYMGR_INSTNO_MASK);
            GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

            if (obj->notifyInfoObj[id]->isValid == TRUE)  {
                switch (obj->notifyInfoObj[id]->notifyType) {

                case ClientNotifyMgr_NOTIFICATION_ALWAYS:
                case ClientNotifyMgr_NOTIFICATION_ONCE:
                case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ONCE:
                case ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS:
                case ClientNotifyMgr_NOTIFICATION_NONE:
                    {
                        procId = obj->notifyInfoObj[id]->procId;
                        sendEvent = 1u;
                        /* Set force notify flag to TRUE to enable call back
                         * to treat the notification as force notification.
                         */
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
            /* Send notification to the  other client */
            /* For normal notificatios application spcific payload is zero.So
             * last two MSBs arezero in notifyPayload. Force notifications will
             * carry payload passed by  application.
             */
            notifyPayload = (   (((UInt32)payLoad) << 16)
                             +  (pAttrs->entryId & 0xFFFF));
            remProcId = obj->notifyInfoObj[id]->procId;

            /* There is difference in calling callbacks on HLOS and RTOS hence the
             * the following define is needed.
             */

#if defined (SYSLINK_BUILD_RTOS)
            if (    (sendEvent == TRUE)
                 && (procId == remProcId)
                 && (obj->notifyInfoObj[id]->forceNotify == TRUE)) {
                /* Call the call back function Directly  since it is on the
                 * local processor.
                 */

                obj->notifyInfoObj[id]->forceNotify = FALSE;
                if (obj->cacheEnabled == TRUE) {
                Cache_wbInv ((Ptr)obj->notifyInfoObj[id],
                             sizeof(ClientNotifyMgr_NotifyParamInfo),
                             Cache_Type_ALL,
                             TRUE);
                }
                for ((elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                       NULL));
                      elem != NULL;
                      elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                      elem)) {
                    if (   ((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId
                            == id) {
                        callBackFxn =
                        ((ClientNotifyMgr_eventListenerInfo*)elem)->cbckFxn;
                        cbContext =
                               ((ClientNotifyMgr_eventListenerInfo*)elem)->
                                                                  cbContext;
                        if (callBackFxn != NULL) {
                           /* Call to call back function */
                            callBackFxn ((Ptr)obj->notifyInfoObj[id]->clientHandle,
                                         cbContext,
                                         payLoad);
                        }
                        break;
                    }
                }
            }
#endif /* #if defined (SYSLINK_BUILD_RTOS)  */

            GateMP_leave ((GateMP_Handle)obj->gate, key);

            /* Send notification to the  other client identified by the id*/
            if (   (sendEvent == TRUE)
                && (Notify_intLineRegistered (remProcId, CLIENTNOTIFYMGR_INT_LINEID)))  {
                    /* For normal notificatios application spcific payload is zero.So
                     * last two MSBs arezero in notifyPayload*/
#if defined (SYSLINK_BUILD_RTOS)
                if (procId != remProcId) {
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                    status = Notify_sendEvent (remProcId,
                                               CLIENTNOTIFYMGR_INT_LINEID,
                                               CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                               notifyPayload,
                                               TRUE );
#if defined (SYSLINK_BUILD_RTOS)
                }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_sendForceNotification",
               status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully sends force
     *           notifications.
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
    Bool                             sendEvent[10] ={0};
    UInt32                           procId;
    UInt32                           i;
    UInt32                           notifyPayload;
    UInt16                           instId;
    UInt16                           instNo;
    UInt16                           creProcId;
    ClientNotifyMgr_Obj              *obj;
#if defined (SYSLINK_BUILD_RTOS)
    List_Elem                        *elem;
    Ptr                              cbContext;
    ClientNotifyMgr_FnCbck           callBackFxn;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

    GT_3trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_sendForceNotificationMulti",
               handle,
               id,
               payLoad);

    GT_assert (curTrace, (NULL != handle));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval ClientNotifyMgr_E_INVALIDSTATE Module was not initialized */
        status = ClientNotifyMgr_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotificationMulti",
                             status,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval ClientNotifyMgr_E_INVALIDARG handle passed is NULL*/
        status = ClientNotifyMgr_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_sendForceNotificationMulti",
                             status,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj     =  handle->obj;

        pAttrs  =  obj->attrs;

            /* Acquire lock */
        key = GateMP_enter ((GateMP_Handle)obj->gate);

        if (obj->cacheEnabled == TRUE) {
            Cache_inv ((Ptr)pAttrs, sizeof (ClientNotifyMgr_Attrs), Cache_Type_ALL,TRUE);
        }

        instId = pAttrs->entryId & 0xFFFF;
         /* Find out the creator procid */
        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                     & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                  & CLIENTNOTIFYMGR_INSTNO_MASK);

        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

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
                            GT_assert (curTrace,
                                     (  procId
                                      < MultiProc_getNumProcessors()));

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

        procId = MultiProc_self();
        /* For normal notificatios application spcific payload is zero.So
         * last two MSBs arezero in notifyPayload*/
        notifyPayload = (   (((UInt32)payLoad) << 16)
                         +  (pAttrs->entryId & 0xFFFF));

        if (numValidClients == 0) {
            /* Release lock */
            status = ClientNotifyMgr_E_FAIL;
        }
        else {
        /* There is difference in calling callbacks on HLOS and RTOS hence the
         * the following define is needed.
         */
#if defined (SYSLINK_BUILD_RTOS)
            /* Call Registered call back functions of the clients on this processor */
            counter = 0;
            if (sendEvent[procId] == TRUE ) {
                for ( i = 0;
                     (   (i < pAttrs->numNotifyEntries)
                      && (counter < pAttrs->numActClients));
                     i++) {
                    if (obj->notifyInfoObj[i]->isValid == TRUE) {
                        counter ++;
                    }
                    if (   (obj->notifyInfoObj[i]->procId == procId)
                        && (obj->notifyInfoObj[i]->forceNotify == TRUE)) {
                        obj->notifyInfoObj[i]->forceNotify = FALSE;
                        if (obj->cacheEnabled == TRUE) {
                            Cache_wbInv ((Ptr)obj->notifyInfoObj[i],
                                         sizeof(ClientNotifyMgr_NotifyParamInfo),
                                         Cache_Type_ALL,
                                         TRUE);
                        }
                        for ((elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                               NULL));
                              elem != NULL;
                              elem = List_next (ClientNotifyMgr_module->eventListenerList[creProcId][instNo],
                              elem)) {
                            if (   ((ClientNotifyMgr_eventListenerInfo*)elem)->notifyId
                                    == i) {
                                callBackFxn =
                                ((ClientNotifyMgr_eventListenerInfo*)elem)->cbckFxn;
                                cbContext =
                                ((ClientNotifyMgr_eventListenerInfo*)elem)->
                                                                          cbContext;
                                if (callBackFxn != NULL) {
                                   /* Call to call back function */
                                    callBackFxn ((Ptr)obj->notifyInfoObj[i]->clientHandle,
                                                 cbContext,
                                                 payLoad);
                               }
                                break;
                            }
                        }
                    }
                }
            }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        }

        GateMP_leave ((GateMP_Handle)obj->gate, key);

        if (status >= 0) {

            /* Send notification to all the eligible clients */
            for (i = 0; i < MultiProc_getNumProcessors(); i++) {
                    /* Send notification to the  other client identified by the id*/
                if (   (sendEvent[i] == TRUE)
                    && (Notify_intLineRegistered (i, CLIENTNOTIFYMGR_INT_LINEID)))  {
                        /* For normal notificatios application spcific payload is zero.So
                         * last two MSBs arezero in notifyPayload*/
#if defined (SYSLINK_BUILD_RTOS)
                     if (i != procId) {
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                        status = Notify_sendEvent (i,
                                                   CLIENTNOTIFYMGR_INT_LINEID,
                                                   CLIENTNOTIFYMGR_ADDSYSKEY(pAttrs->eventNo),
                                                   notifyPayload,
                                                   TRUE );
#if defined (SYSLINK_BUILD_RTOS)
                    }
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                }
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,
               GT_LEAVE,
               "ClientNotifyMgr_sendForceNotificationMulti",
               status);

    /*! @retval ClientNotifyMgr_S_SUCCESS if successfully sends notifications.
     */
    return (status);
}

/*!
 *  @brief  Function to get ClientNotifyMgr Id.
 *
 *  @param  handle  Handle to the instance.
 *
 */
UInt32
ClientNotifyMgr_getId (ClientNotifyMgr_Handle handle)
{
    UInt32 id = 0;
    ClientNotifyMgr_Obj  *obj;

   GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgr_getId", handle);

   GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (ClientNotifyMgr_module->refCount == 0) {

        /*! @retval 0 Module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_getId",
                             ClientNotifyMgr_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (handle == NULL) {
        /*! @retval 0 params pointer passed is NULL */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgr_getId",
                             ClientNotifyMgr_E_INVALIDARG,
                             "handle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj = handle->obj;
        GT_assert (curTrace, obj != NULL);
        id = obj->Id;
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

   GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_getId", id);

   /*! @retval id Instance ID*/
   return (id);
}

UInt32
ClientNotifyMgr_sharedMemReq (const ClientNotifyMgr_Params *params)
{
    UInt32  totalSize = 0;
    SizeT   minAlign;
    UInt16  regionId;

    GT_assert (curTrace, (NULL != params));

    if (params->sharedAddr) {
        regionId = SharedRegion_getId(params->sharedAddr);
    }
    else {
        regionId = params->regionId;
    }
    GT_assert(curTrace , (regionId != SharedRegion_INVALIDREGIONID));

    minAlign = Memory_getMaxDefaultTypeAlign();
    if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
        minAlign = SharedRegion_getCacheLineSize(regionId);
    }

    /* Get the shared attributes size */
    totalSize = ROUND_UP(sizeof (ClientNotifyMgr_Attrs), minAlign);

    totalSize += ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo), minAlign) *
                          params->numNotifyEntries;

    return (totalSize);

}

/*=============================================================================
 * Internal Functions
 *=============================================================================
 */
/*!
 * @brief  Internal Function to create a ClientNotifyMgr instance
 *
 * @param  instParams          Instance param structure.
 */
ClientNotifyMgr_Handle
_ClientNotifyMgr_create (const Void * params)
{
    Int32                                      status          = ClientNotifyMgr_S_SUCCESS;
    UInt32                                     i               = 0;
    UInt32                                     virtAddr        = 0;
    UInt32                                     addrOffset      = 0;
    ClientNotifyMgr_Handle                     handle          = NULL;
    ClientNotifyMgr_Obj                      * obj             = NULL;
    ClientNotifyMgr_Params                   * cliParams       = (ClientNotifyMgr_Params *)params;
    NameServer_Handle                          nameServerHandle;
    volatile ClientNotifyMgr_Attrs           * pAttrs;
    volatile ClientNotifyMgr_NotifyParamInfo * notifyInfoObj;
    UInt32                                     shmIndex;
    SharedRegion_SRPtr                         sharedShmBase;
    IArg                                       key;
    ClientNotifyMgr_callbackCtx    *           cliCallbackCtx;
    UInt32                                     k;
    UInt16                                     instId;
    UInt16                                     instNo;
    UInt16                                     creProcId;
// #if defined (SYSLINK_BUILD_RTOS)
    Int32                                      rc              = 0;
    UInt32                                     len;
    UInt32                                     sharedAddr;
// #endif /* #if defined (SYSLINK_BUILD_RTOS) */


    GT_1trace (curTrace, GT_ENTER, "_ClientNotifyMgr_create", params);

    GT_assert (curTrace, (params != NULL));


#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if ( NULL == params) {
         /*! @retval ClientNotifyMgr_E_INVALIDARG  Params passed is NULL
          */
         status = ClientNotifyMgr_E_INVALIDARG;
         GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_ClientNotifyMgr_create",
                             status,
                             "Params pointer is NULL!");

    }
#endif
     if (status >= 0) {
        /* Create the handle */
        /* Allocate memory for handle */
        handle = (ClientNotifyMgr_Handle) Memory_calloc
                                          (NULL,
                                           sizeof (ClientNotifyMgr_Object),
                                           0u,
                                           NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (handle == NULL) {
            /*! @retval ClientNotifyMgr_E_MEMORY  Failed to allocate
             * memory for  instance handle.
             */
            status = ClientNotifyMgr_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_ClientNotifyMgr_create",
                                 ClientNotifyMgr_E_MEMORY,
                                 "Memory allocation failed for handle!");
        }
        else
#endif
        {
            obj = (ClientNotifyMgr_Obj *) Memory_calloc (NULL,
                                          sizeof (ClientNotifyMgr_Obj),
                                          0u,
                                          NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
            if (obj == NULL) {
             /*! @retval ClientNotifyMgr_E_MEMORY  Failed to allocate
              * memory for  instance internal object.
              */
                status = ClientNotifyMgr_E_MEMORY;
                Memory_free (NULL, handle, sizeof (ClientNotifyMgr_Object));
                handle = NULL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_ClientNotifyMgr_create",
                                     ClientNotifyMgr_E_MEMORY,
                                     "Memory allocation failed for internal "
                                     "object!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
    }

    if (status >= 0) {
        if (cliParams->openFlag == TRUE ) {
            /*Open the  instance that  is already created.*/
            if (   (NULL != cliParams->name)
                && (NULL != ClientNotifyMgr_module->nameServer)){
// #if defined (SYSLINK_BUILD_RTOS)
                len = sizeof(UInt32);
                rc = NameServer_get ((NameServer_Handle)
                                     ClientNotifyMgr_module->nameServer,
                                     cliParams->name,
                                     (UInt32*)&(sharedAddr),
                                     &len,
                                     NULL);
                if (rc == NameServer_E_NOTFOUND) {
                    /* Name not found. */
                    /* Failed to get the information from the name server.
                     * It means it is not created.
                     */
                    status = ClientNotifyMgr_E_NOTFOUND;
                     GT_setFailureReason (curTrace,
                                          GT_4CLASS,
                                          "_ClientNotifyMgr_create",
                                          status,
                                          "Instance  not found!");
                }
                else if (rc == NameServer_E_FAIL) {
                    /* Error happened in NameServer. Pass the error up. */
                    status = ClientNotifyMgr_E_FAIL;
                }
                else {
                    /* Get the virtual address from the shared memory */
                    virtAddr = (UInt32) SharedRegion_getPtr (
                                               (SharedRegion_SRPtr)sharedAddr);
                }
// #endif /* #if defined (SYSLINK_BUILD_RTOS) */
            }

            if (virtAddr == (UInt32)NULL ) {
                if (cliParams->sharedAddr != NULL) {
                    virtAddr = (UInt32) cliParams->sharedAddr;
                    status = ClientNotifyMgr_S_SUCCESS;
                }
                else {
                    status = ClientNotifyMgr_E_INVALIDARG;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_ClientNotifyMgr_create",
                                         status,
                                         "virt Addr is NULL and passed "
                                         "params->sharedAddr is also NULL!");
                }
            }

            if (status >= 0) {
                handle->obj     = obj;
                obj->minAlign = Memory_getMaxDefaultTypeAlign();

                obj->regionId = SharedRegion_getId((Ptr)virtAddr);

                if (SharedRegion_getCacheLineSize(obj->regionId) > obj->minAlign) {
                    obj->minAlign = SharedRegion_getCacheLineSize(obj->regionId);
                }
                obj->attrs =  (ClientNotifyMgr_Attrs*) virtAddr;
                pAttrs     = obj->attrs;
                GT_assert (curTrace, (pAttrs != NULL));
                addrOffset +=  ROUND_UP(sizeof (ClientNotifyMgr_Attrs),
                                        obj->minAlign);
                /* Acquire  lock and invalidte it */
                obj->gate       = cliParams->gate;
                if (cliParams->cacheFlags & ClientNotifyMgr_CONTROL_CACHEUSE) {
                    obj->cacheEnabled = TRUE;
                }

                if (obj->cacheEnabled == TRUE) {
                    Cache_inv((Ptr)pAttrs,
                              sizeof(ClientNotifyMgr_Attrs),
                              Cache_Type_ALL,
                              TRUE);
                }

                /* Make sure the instance has been created */
                if (pAttrs->status != ClientNotifyMgr_CREATED) {
                    status = ClientNotifyMgr_E_NOTFOUND;
                     GT_setFailureReason (curTrace,
                                          GT_4CLASS,
                                          "_ClientNotifyMgr_create",
                                          status,
                                          "Instance  not created!");
                }
                else {
                    /* Update the readers information */
                    obj->notifyInfoObj = Memory_alloc(NULL,
                                                      sizeof(UInt32) * pAttrs->numNotifyEntries,
                                                      0,
                                                      NULL);
                    if (obj->notifyInfoObj == NULL) {
                        status = ClientNotifyMgr_E_MEMORY;
                        GT_setFailureReason (curTrace,
                                              GT_4CLASS,
                                              "_ClientNotifyMgr_create",
                                              status,
                                              "Memory alloc  failed!");
                    }
                    else {
                        /*Populate the notifyInfo entries */
                        for (i = 0; i < pAttrs->numNotifyEntries; i++) {
                            obj->notifyInfoObj[i] =
                                (ClientNotifyMgr_NotifyParamInfo *)(virtAddr + addrOffset);

                            addrOffset += ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo),
                                                   obj->minAlign);
                        }

                        obj->type     = ClientNotifyMgr_DYNAMIC_OPEN;
                        obj->numNotifyEntries = pAttrs->numNotifyEntries;
                        obj->Id = pAttrs->entryId;
                        instId  = pAttrs->entryId & 0xFFFF;
                        /* Find out the creator procid */
                        creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                                    & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
                        /* Find out the instance no on that creator procId */
                        instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                                  & CLIENTNOTIFYMGR_INSTNO_MASK);
                        GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));
                        //obj->cacheEnabled = params->cacheFlags;

                        if (    ClientNotifyMgr_module->instRefCount[creProcId][instNo]
                             == 0) {
                            ClientNotifyMgr_module->instRefCount[creProcId][instNo] = 1;
                            for (i = 0; i < MultiProc_getNumProcessors();
                                 i++ ) {
                                if (Notify_intLineRegistered (i,
                                                        CLIENTNOTIFYMGR_INT_LINEID)) {
                                    if (ClientNotifyMgr_module->
                                              isEventRegistered[creProcId][instNo][i]
                                    != TRUE){
                                    cliCallbackCtx = (ClientNotifyMgr_callbackCtx *)
                                        Memory_calloc(NULL,
                                                      sizeof(ClientNotifyMgr_callbackCtx),
                                                      0,
                                                      NULL);
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
                                            cliCallbackCtx->gate = obj->gate;
                                            cliCallbackCtx->cacheEnabled =
                                                               obj->cacheEnabled;
                                            cliCallbackCtx->attrs = obj->attrs;
                                            cliCallbackCtx->notifyInfoObjBaseAddr = (Ptr)
                                                             obj->notifyInfoObj[0];
                                            cliCallbackCtx->minAlign = obj->minAlign;
                                            cliCallbackCtx->instId = instId;
                                            ClientNotifyMgr_module->
                                                callBackCtxPtrs[creProcId][instNo][i]
                                                                   = cliCallbackCtx;
                                            if(obj->cacheEnabled == TRUE) {
                                                Cache_inv (
                                                     (Ptr)pAttrs,
                                                     sizeof(ClientNotifyMgr_Attrs),
                                                     Cache_Type_ALL,
                                                     TRUE);
                                            }
                                            status = ClientNotifyMgr_registerWithNotify
                                                   (GateMP_getLocalProtect((GateMP_Handle)
                                                             cliCallbackCtx->gate),
                                                    i,
                                                    CLIENTNOTIFYMGR_INT_LINEID,
                                                    pAttrs->eventNo,
                                                    (UArg)cliCallbackCtx);
                                        }
                                    }
                                    ClientNotifyMgr_module->
                                       isEventRegistered[creProcId][instNo][i] = TRUE;
                                }
                            }
                        }
                        else {
                            ClientNotifyMgr_module->instRefCount[creProcId][instNo] += 1;
                        }
                    }
                }
            }

        }
        else {
            /* Chek if shared memory provided is sufficient
                       * to create instance
                       */
            if( (cliParams->sharedAddr != NULL) &&  (cliParams->sharedAddrSize
                < (ClientNotifyMgr_sharedMemReq (cliParams)))) {
                status = ClientNotifyMgr_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "_ClientNotifyMgr_create",
                                      status,
                                      "Shared memory provided is less than the "
                                      "Requied shared memory for the instance.!");
            }

            if (status >= 0) {
                nameServerHandle = (NameServer_Handle)
                    ClientNotifyMgr_module->nameServer;
                if ((nameServerHandle  != NULL)
                    && (cliParams->name != NULL)) {
                    shmIndex = SharedRegion_getId( (Ptr)cliParams->sharedAddr);
                    sharedShmBase = SharedRegion_getSRPtr(
                                                     (Ptr)cliParams->sharedAddr,
                                                     shmIndex);
//#if defined (SYSLINK_BUILD_RTOS)
                    len = sizeof (UInt32);
                    rc = NameServer_get (nameServerHandle,
                                         cliParams->name,
                                         &sharedAddr,
                                         &len,
                                         NULL);
                    if (rc > 0) {
                        /* Already instance with the name exists */
                        status = ClientNotifyMgr_E_INST_EXISTS;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "ClientNotifyMgr_create",
                                             status,
                                             "Instance  already exists!");
                    }
                    else if (rc == NameServer_E_FAIL) {
                        /* Error happened in NameServer. Pass the error up. */
                        status = ClientNotifyMgr_E_FAIL;
                    }
                    else
//#endif /* #if defined (SYSLINK_BUILD_RTOS) */
                    {
                        /* Instance with the name is not created. So addit now */
                        obj->nsKey = NameServer_addUInt32(
                                             (NameServer_Handle)ClientNotifyMgr_module->nameServer,
                                             cliParams->name,
                                             (UInt32)(sharedShmBase));
                    }
                }

                key = IGateProvider_enter (ClientNotifyMgr_module->gate);
                if (status >= 0) {
                    /* Create  Object and shared control  structure for
                     *  the instance .
                     */
                    handle->obj     = obj;
                    obj->minAlign = Memory_getMaxDefaultTypeAlign();
                    obj->regionId = SharedRegion_getId((Ptr)cliParams->sharedAddr);

                    if (SharedRegion_getCacheLineSize(obj->regionId) > obj->minAlign) {
                        obj->minAlign = SharedRegion_getCacheLineSize(obj->regionId);
                    }

                    //obj->selfProcId = cliParams->creatorProcId;
                    virtAddr = (UInt32) cliParams->sharedAddr;
                    obj->attrs =  (ClientNotifyMgr_Attrs*) virtAddr;
                    pAttrs     = obj->attrs;
                    addrOffset +=  ROUND_UP(sizeof (ClientNotifyMgr_Attrs),
                                            obj->minAlign);
                    obj->gate       = cliParams->gate;
                    if (cliParams->cacheFlags & ClientNotifyMgr_CONTROL_CACHEUSE) {
                        obj->cacheEnabled = TRUE;
                    }
                    pAttrs->maxClients = ClientNotifyMgr_MAX_CLIENTS;
                    pAttrs->eventNo = cliParams->eventNo;
                    pAttrs->numActClients = 0;
                    pAttrs->numNotifyEntries = cliParams->numNotifyEntries;
                    pAttrs->numSubNotifyEntries = cliParams->numSubNotifyEntries;
                    obj->numNotifyEntries = pAttrs->numNotifyEntries;
                    /* Initialize  notification info objects of the instance
                     * Each object will be reserverd for one client who registers
                     * for notification with it
                     */
                    /* Update the readers information */
                    obj->notifyInfoObj = Memory_alloc(NULL,
                                                      sizeof(UInt32) * pAttrs->numNotifyEntries,
                                                      0,
                                                      NULL);
                    if (obj->notifyInfoObj == NULL) {
                        status = ClientNotifyMgr_E_MEMORY;
                        GT_setFailureReason (
                                           curTrace,
                                           GT_4CLASS,
                                           "ClientNotifyMgr_create",
                                           status,
                                           "Failure in NameServer_get!");
                    }
                    else {
                        /*Populate the notifyInfo entries */
                        for (i = 0; i < pAttrs->numNotifyEntries; i++) {
                            obj->notifyInfoObj[i] =
                                (ClientNotifyMgr_NotifyParamInfo *)(virtAddr + addrOffset);

                            addrOffset += ROUND_UP(sizeof (ClientNotifyMgr_NotifyParamInfo),
                                                   obj->minAlign);
                        }

                        for ( i = 0; i < pAttrs->numNotifyEntries; i++ ) {
                            notifyInfoObj = (ClientNotifyMgr_NotifyParamInfo*)
                                                    obj->notifyInfoObj[i];
                            notifyInfoObj->clientHandle   = (UInt32)-1;
                            notifyInfoObj->isValid        = FALSE;
                            notifyInfoObj->procId         = (UInt16)-1;
                            notifyInfoObj->notifyType     =
                                         ClientNotifyMgr_NOTIFICATION_NONE;
                            for (k = 0;
                                 k < CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES;
                                 k++) {
                                notifyInfoObj->notifyFlag[k]    = FALSE;
                                notifyInfoObj->eventCount[k]    = 0;
                            }

                            notifyInfoObj->waterMarkCond1 = (UInt32)-1;
                            notifyInfoObj->waterMarkCond2 = (UInt32)-1;
                            if (obj->cacheEnabled == TRUE) {
                                Cache_wbInv ((Ptr)notifyInfoObj,
                                             sizeof (ClientNotifyMgr_NotifyParamInfo),
                                             Cache_Type_ALL,
                                             TRUE);
                            }
                        }
                        for (i = 0; i < ClientNotifyMgr_maxInstances; i++) {
                             if (ClientNotifyMgr_module->isInstNoValid[i] == FALSE) {
                                ClientNotifyMgr_module->isInstNoValid[i] = TRUE;
                                pAttrs->entryId = ( (  MultiProc_self()
                                                      & CLIENTNOTIFYMGR_CREATORPROCID_MASK)
                                                   << CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                                                 +( (i &CLIENTNOTIFYMGR_INSTNO_MASK)
                                                << CLIENTNOTIFYMGR_INSTNO_BITOFFSET);
                                obj->Id = pAttrs->entryId;
                                break;
                             }
                        }

                        if (i == ClientNotifyMgr_maxInstances) {
                            /*! @retval ClientNotifyMgr_E_MAXINSTANCES
                             * Limit of ClientNotifyMgr_maxInstances reached.
                             */
                            status = ClientNotifyMgr_E_MAXINSTANCES;
                            status = ClientNotifyMgr_E_MAXINSTANCES;
                            GT_setFailureReason (curTrace,
                                                  GT_4CLASS,
                                                  "_ClientNotifyMgr_create",
                                                  status,
                                                  "Limit of ClientNotifyMgr_maxInstances!");
                        }
                        else {
                            pAttrs->status        = ClientNotifyMgr_CREATED;
                            pAttrs->version       = ClientNotifyMgr_VERSION;
                            pAttrs->creatorProcId = MultiProc_self();
                            //pAttrs->numSubNotifyEntries = params->numSubNotifyEntries;


                            if (obj->cacheEnabled == TRUE) {
                                Cache_wbInv ((Ptr)pAttrs,
                                             sizeof (ClientNotifyMgr_Attrs),
                                             Cache_Type_ALL,
                                             TRUE);
                            }
                            obj->gate  = cliParams->gate;
                            obj->type  = ClientNotifyMgr_DYNAMIC_CREATE;
                            obj->Id         =  pAttrs->entryId;

                            instId = pAttrs->entryId & 0xFFFF;
                             /* Find out the creator procid */
                            creProcId = (  (instId >> CLIENTNOTIFYMGR_CREATORPROCID_BITOFFSET)
                                         & CLIENTNOTIFYMGR_CREATORPROCID_MASK);
                            instNo = ( (instId >> CLIENTNOTIFYMGR_INSTNO_BITOFFSET)
                                      & CLIENTNOTIFYMGR_INSTNO_MASK);
                            GT_assert (curTrace, (instNo < ClientNotifyMgr_maxInstances));

                            if (    ClientNotifyMgr_module->instRefCount[creProcId][instNo]
                                 == 0) {
                                ClientNotifyMgr_module->instRefCount[creProcId][instNo] = 1;
                                for (i = 0; i < MultiProc_getNumProcessors();
                                     i++ ) {
                                    /* Register call back function  with all the procs
                                     * if  notify drivers for that processor does
                                     * exists.
                                     */
                                    if (Notify_intLineRegistered (i,CLIENTNOTIFYMGR_INT_LINEID)) {
                                        if (ClientNotifyMgr_module->
                                                  isEventRegistered[creProcId][instNo][i]
                                        != TRUE){
                                        cliCallbackCtx = (ClientNotifyMgr_callbackCtx *)
                                            Memory_calloc(NULL,
                                                    sizeof(ClientNotifyMgr_callbackCtx),
                                                          0,
                                                          NULL);
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
                                                cliCallbackCtx->gate = obj->gate;
                                                cliCallbackCtx->cacheEnabled =
                                                                   obj->cacheEnabled;
                                                cliCallbackCtx->attrs = obj->attrs;
                                                cliCallbackCtx->instId = instId;
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
                                                 status = ClientNotifyMgr_registerWithNotify
                                                   (GateMP_getLocalProtect((GateMP_Handle)cliCallbackCtx->gate),
                                                    i,
                                                    CLIENTNOTIFYMGR_INT_LINEID,
                                                    pAttrs->eventNo,
                                                    (UArg)cliCallbackCtx);
                                            }
                                        }
                                        ClientNotifyMgr_module->
                                           isEventRegistered[creProcId][instNo][i] = TRUE;
                                    }
                                }
                            }
                            else {
                                ClientNotifyMgr_module->instRefCount[creProcId][instNo] += 1;
                            }
                        }
                    }
                }
                IGateProvider_leave (ClientNotifyMgr_module->gate, key);
            }
        }
    }
//TBD
    if (status >= 0){
        /* Populate the cliParams member */
         Memory_copy ((Ptr) &obj->params,
                     (Ptr) cliParams,
                     sizeof (ClientNotifyMgr_Params));
         /* Put in the local list */
         key = IGateProvider_enter (ClientNotifyMgr_module->gate);
         List_elemClear (&obj->listElem);
         List_put (ClientNotifyMgr_module->objList,
                   &obj->listElem);
         IGateProvider_leave (ClientNotifyMgr_module->gate, key);
    }

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    /* Failed to crate /open the instance. Do clean up*/
    if (status < 0) {
        if (obj != NULL ) {
            Memory_free (NULL, obj, sizeof (ClientNotifyMgr_Obj));
        }
        if (handle != NULL) {
            Memory_free (NULL, handle, sizeof (ClientNotifyMgr_Object));
        }
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "_ClientNotifyMgr_create", handle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */

    return (handle);

}
/*!
 *  @brief      API to create the  name server instance for the ClientNotifyMgr
 *              module.
 *
 *  @param      name   Name of the NameServer instance to be created.
 *  @param      gate   Gate for protection.
 */
static
void *
ClientNotifyMgr_setNameServer(String name,
                              GateMP_Handle gate)
{
    NameServer_Handle       nameServerHandle = NULL;
    NameServer_Params       nameServerParams;

    GT_2trace (curTrace,
               GT_ENTER,
               "ClientNotifyMgr_setNameServer",
               name,
               gate);
    GT_assert (curTrace,(NULL != name));

    /* Get the default params for  the Name server */
    NameServer_Params_init(&nameServerParams);

    /* Update the nameServerParams  as per ClientNotifyMgr requirements */
    nameServerParams.maxRuntimeEntries = ClientNotifyMgr_maxInstances;
    nameServerParams.checkExisting     = TRUE; /* Checsk if exists */
    /* Length of  the ClientNotifyMgr  instance */
    nameServerParams.maxNameLen        = ClientNotifyMgr_maxNameLen;
    nameServerParams.maxValueLen       = sizeof (UInt32);

    nameServerHandle = NameServer_create(name,
                                         &nameServerParams);

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgr_setNameServer",nameServerHandle);
    return (nameServerHandle);
}

/*!
 *  @brief      API to delete the name server instance.
 *
 *  @param      pNameServerHandle Pointer to the NameServer Handle.
 */
static
Int32
ClientNotifyMgr_deleteNameServer(NameServer_Handle* pNameServerHandle)
{
    Int32   status = ClientNotifyMgr_S_SUCCESS;

    GT_1trace (curTrace,
               GT_ENTER,
               "_ClientNotifyMgr_deleteNameServer",
               pNameServerHandle);

    GT_assert (curTrace, (NULL !=  pNameServerHandle));

    status = NameServer_delete (pNameServerHandle);

    GT_1trace (curTrace, GT_LEAVE, "_ClientNotifyMgr_deleteNameServer", status);
    return (status);
}

static inline
Int32  ClientNotifyMgr_registerWithNotify(Int32 lProtection,
                                          UInt16 procid,
                                          UInt16 lineId,
                                          UInt32 eventNo,
                                          UArg context)
{

    Int32 status = ClientNotifyMgr_S_SUCCESS;
    switch  (lProtection)
    {
        case GateMP_LocalProtect_TASKLET:
#if defined (SYSLINK_BUILD_RTOS)
            /* swi created with lowest priority and fxn = swiFxn */
            /* Register the call back function and event Id with notify */
            status = Notify_registerEvent(
                procid,
                lineId,
                CLIENTNOTIFYMGR_ADDSYSKEY(eventNo),
                (Notify_FnNotifyCbck)ClientNotifyMgr_cbFxn,
                context);
        break;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        case GateMP_LocalProtect_THREAD:
        case GateMP_LocalProtect_PROCESS:
#if defined (SYSLINK_BUILD_RTOS)
            /* Register the call back function and event Id with notify  that sends info to Client callback thread*/
            status = Notify_registerEvent(
                procid,
                lineId,
                CLIENTNOTIFYMGR_ADDSYSKEY(eventNo),
                (Notify_FnNotifyCbck)ClientNotifyMgr_thFxn,
                context);
        break;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        case GateMP_LocalProtect_INTERRUPT:
            Notify_registerEvent (
                procid,
                lineId,
                CLIENTNOTIFYMGR_ADDSYSKEY(eventNo),
                ClientNotifyMgr_callBack,
                context);
        break;
        default:
            status = ClientNotifyMgr_E_INVALIDARG;
            GT_setFailureReason (
                        curTrace,
                        GT_4CLASS,
                        "ClientNotifyMgr_registerWithNotify",
                        status,
                        "Unknown local protection type!");
    }

    return (status);
}

static inline
Int32  ClientNotifyMgr_unregisterWithNotify(Int32 lProtection,
                                            UInt16 procid,
                                            UInt16 lineId,
                                            UInt32 eventNo,
                                            UArg context)
{
    Int32 status = ClientNotifyMgr_S_SUCCESS;
    switch  (lProtection)
    {

        case GateMP_LocalProtect_TASKLET:
#if defined (SYSLINK_BUILD_RTOS)
            /* swi created with lowest priority and fxn = swiFxn */
            /* Register the call back function and event Id with notify */
            status = Notify_unregisterEvent(
                procid,
                lineId,
                CLIENTNOTIFYMGR_ADDSYSKEY(eventNo),
                (Notify_FnNotifyCbck)ClientNotifyMgr_cbFxn,
                context);
        break;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        case GateMP_LocalProtect_THREAD:
        case GateMP_LocalProtect_PROCESS:
#if defined (SYSLINK_BUILD_RTOS)
            /* Register the call back function and event Id with notify  that sends info to Client callback thread*/
            status = Notify_unregisterEvent(
                procid,
                lineId,
                CLIENTNOTIFYMGR_ADDSYSKEY(eventNo),
                (Notify_FnNotifyCbck)ClientNotifyMgr_thFxn,
                context);
        break;
#endif /* #if defined (SYSLINK_BUILD_RTOS) */
        case GateMP_LocalProtect_INTERRUPT:
            Notify_unregisterEvent (
                procid,
                lineId,
                CLIENTNOTIFYMGR_ADDSYSKEY(eventNo),
                ClientNotifyMgr_callBack,
                context);
        break;

        default:
            status = ClientNotifyMgr_E_INVALIDARG;
           GT_setFailureReason (
                        curTrace,
                        GT_4CLASS,
                        "ClientNotifyMgr_unregisterWithNotify",
                        status,
                        "Unknown local protection type!");
    }
    return (status);
}

GateMP_Handle ClientNotifyMgr_getGate(ClientNotifyMgr_Handle handle)
{
    ClientNotifyMgr_Object * objectPtr = (ClientNotifyMgr_Object *)handle;
    ClientNotifyMgr_Obj    * objPtr    = objectPtr->obj;

    return (objPtr->gate);
}
