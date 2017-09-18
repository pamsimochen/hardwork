/** 
 *  @file   ClientNotifyMgr.h
 *
 *  @brief      Defines for interfaces for ClientNotifyMgr module.
 *
 *              TODO: write description
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



#ifndef CLIENTNOTIFYMGR_H_0x684e
#define CLIENTNOTIFYMGR_H_0x684e

#if defined (SYSLINK_BUILD_HLOS)
#include <ti/syslink/utils/IGateProvider.h>
#endif /* #if defined (SYSLINK_BUILD_HLOS) */

#if defined (SYSLINK_BUILD_RTOS)
#include <xdc/runtime/IGateProvider.h>
#endif /* #if defined (SYSLINK_BUILD_RTOS) */

#include <ti/syslink/inc/ClientNotifyMgr_config.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Notify.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define CLIENTNOTIFYMGR_INT_LINEID       0u /* TBD: Interrupt line ID hardcoded to 0. */

/* =============================================================================
 * macros & defines
 * =============================================================================
 */


/*!
 * @brief  Prototype of the ClientNotifyMgr call back function.
 *
 * @param  handle Runtime information provided  to the call back. It is actually the
 *         client handle passed by the registered client call.
 * @param  arg  Context  pointer passed to the call back.
 * @param  payLoad pay load passed to the call back.
 *
 */
typedef void (*ClientNotifyMgr_FnCbck) (Ptr handle, Ptr arg, UInt16 payLoad) ;

/*!
 *  @brief  A set of notification types.
 *
 */
typedef enum ClientNotifyMgr_NotifyType_Tag {
    ClientNotifyMgr_NOTIFICATION_NONE,
    /*!< No notification is required.*/
    ClientNotifyMgr_NOTIFICATION_ALWAYS,
    /*!< Notify whenever the other client sends data/frees up space.*/
    ClientNotifyMgr_NOTIFICATION_ONCE,
    /*!< Notify when the other side sends data/frees up space. Once the
     *   notification is done, the notification is disabled until it is
     *   enabled again.
     */
    ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ALWAYS,
    /*!< Notify whenever the other side sends data/frees up space.
     *   This notification is never disabled.
     */
    ClientNotifyMgr_NOTIFICATION_HDWRFIFO_ONCE
    /*!< Notify when the other side sends data/frees up space. Once the
     *   notification is done, the notification is disabled until it is
     *   enabled again. The notification is enabled once the watermark
     *   is crossed and does not require buffer to get full/empty.
     */
} ClientNotifyMgr_NotifyType;

/*!
 *  @brief   Enumeration of ClientNotifyMgr cache flags.
 *
 *  @field   ClientNotifyMgr_CONTROL_CACHEUSE
 *           If specified, ClientNotifyMgr does cache coherence operations  on
 *           it's shared control structures.
 *
 */
typedef enum ClientNotifyMgr_CacheFlags_Tag {
    ClientNotifyMgr_CONTROL_CACHEUSE    = 0x1,
} ClientNotifyMgr_CacheFlags;


/*!
 *  @brief  Structure defining config parameters for the ClientNotifyMgr module.
 */
typedef struct ClientNotifyMgr_Config_Tag {
    IGateProvider_Handle  gate; // It was GateMP_Handle
    /*!< config gate.
     */
    UInt32    numProcessors ;
    /*!< Number of processors this module is going to talk to  including
     * the local processor.
     */
    Ptr       procIds       ;
    /*!<Pointer to  array  processorids . This array size should be equal to
     * numProcessors.It Denotes the number of processors to which Client manager
     * will send notifications.
     */
    Ptr       drvHandles    ;
    /*!< Array of notify driver handles to  each procId mentioned in procIds.
     */
} ClientNotifyMgr_Config;


/*!
 *  @brief  Structure defining config parameters for the ClientNotifyMgr
 *          instances.
 */
typedef struct ClientNotifyMgr_Params_Tag {
     String        name ;
     /*!< Name of the instance */
     UInt32        eventNo ;
     /*!< Notify event no for use by this instance */
     Bool          openFlag ;
     /*!< Flag to indicate whether create should create the instance
      *   or open the instance
      */
     UInt32        creatorProcId ;
     /*!< instance creator processor id */
     Ptr           sharedAddr ;
     /*!<  Virtual shared Region Addres */
     UInt32        sharedAddrSize ;
     /*!<  Size of the shared memory */
     GateMP_Handle gate ;
     /*!< Gate for protection */
     UInt32        cacheFlags ;
     /*!<  Cache flags.See FrameQ_CacheFlags */
     UInt32        numNotifyEntries;
     /*!< Number of notify Entries */
     UInt32        numSubNotifyEntries;
     /*!<
      * Sub notifiy entries in each notifyParamInfo entry.
      */
      UInt16 regionId;
} ClientNotifyMgr_Params ;

/*!
 *  @brief  Structure defining  client notify information.
 *          ClientNotifyMgr_registerClient AI takes this as argument during
 *          call back function registration.
 */
typedef struct ClientNotifyMgr_registerNotifyParams_Tag {
    Ptr          clientHandle;
    /*!<  client instance handle which is going to be registered */
    UInt16       procId;
    /*!< Processor Id of the client */
    UInt32       notifyType ;
    /*!< Notification type*/
    ClientNotifyMgr_FnCbck  fxnPtr;
    /*!< Call back function*/
    Ptr          cbContext;
    /*!< Call back context pointer.Will be passed as an arg to call
     *   back function.
     */
    UInt32       watermarkCond1;
     /*!< water mark condition 1 */
    UInt32       watermarkCond2;
     /*!< water mark condition 2. Set it to -1 if not required */
} ClientNotifyMgr_registerNoitifyParams ;

/*! @brief Forward declaration of structure defining object for the
 * ClientNotifyMgr. */
typedef struct ClientNotifyMgr_Object ClientNotifyMgr_Object;

/*!
 *  @brief  Handle for the ClientNotifyMgr instance.
 */
typedef ClientNotifyMgr_Object * ClientNotifyMgr_Handle;



/* ============================================================================
 * EXPORT APIs
 * ============================================================================
 */

/*
 *  @brief  Function to initialize the config parameter structure  with default
 *          values.
 *
 *  @param  cfgParams
 *          Parameter structure to return the config parameters.
 *
 *  @sa    None.
 */
Void
ClientNotifyMgr_getConfig (ClientNotifyMgr_Config * cfgParams) ;

/*
 *  @brief   Function to find out the memory required for the module.
 *
 */
UInt32
ClientNotifyMgr_sharedMemModReq (void);

/*
 *  @brief  Initialize the ClientNotifyMgr module,
 *          there would be some default values for the parameters,
 *          but if  any change is required, change the cfgParams before calling
 *          this API.
 *
 *  @param  cfgParams
 *          Parameter structure to return the config parameters.
 *
 *  @sa    None.
 */
Int32
ClientNotifyMgr_setup (ClientNotifyMgr_Config * cfgParams) ;

/*
 *  @brief  Function to finalize module.
 *
 *  @sa     ClientNotifyMgr_setup.
 */
Int32
ClientNotifyMgr_destroy (void) ;

/*
 *  @brief  Function to find out the amount of shared memory required for
 *          creation of each instance.
 *          This API Can be used to make sure the sharedmemory provided is
 *          large  enough to create the instance.
 *          All parameters except sharedAddr and sharedAddrSize must be the same
 *          that will be used in the create.
 *
 *  @param  params
 *          FrameQ instance creation parameters.
 */
UInt32
ClientNotifyMgr_sharedMemReq (const ClientNotifyMgr_Params *params);

/*
 * @brief Function to get the default instance parameters
 *
 * @param handle handle of the instance. if specified as NULL it returns the default
 *               config params.
 * @param params  Instance create  params
 */
Void ClientNotifyMgr_Params_init (ClientNotifyMgr_Params *params) ;

/*  Function to create a ClientNotifyMgr */
ClientNotifyMgr_Handle
ClientNotifyMgr_create (ClientNotifyMgr_Params * params) ;

/*  Function to delete the created ClientNotifyMgr  instance*/
Int32
ClientNotifyMgr_delete (ClientNotifyMgr_Handle * pHandle) ;

/*
 * @brief Function to close the dynamically opened instance
 *
 * @param Pointer to instance handle.
 */
Int32
ClientNotifyMgr_close(ClientNotifyMgr_Handle *pHandle);

/*!
 *  @brief Function to register notification with the ClientNotifyMgr instance.
 *
 *  @param clientHandle  This will be returnd as a first argument  in the call
 *                       back functions.This represents the caller's instance
 *                       handle.
 *  @param regParams     Client  notify registration information.
 *
 *  @param id  Pointer to hold the identifier returned after successfully
 *             returning from the registerClient API.
 *
 */
Int32
ClientNotifyMgr_registerClient(ClientNotifyMgr_Handle      handle,
                               ClientNotifyMgr_registerNoitifyParams *regParams,
                               UInt32 *id) ;

/*
 * @brief Function to unregister the notification call back function.
 *
 * @param handle Instance handle.
 * @param Id  notifyId obtained through the registered client call.
 */
Int32
ClientNotifyMgr_unregisterClient (ClientNotifyMgr_Handle      handle,
                                  UInt32 id);

/*!
 *  @brief        Function to enable notification for the caller client.
 *
 *  @param handle Instance handle.
 *  @param id     Id obtained through registerClient call.
 *  @param subId  Sub notification entrys on which client wants to set
 *                notify flag to true and wants notification on that sub entry.
 */
Int32
ClientNotifyMgr_enableNotification (ClientNotifyMgr_Handle  handle,
                                    UInt32                  id,
                                    UInt32                  subId) ;

/*!
 *  @brief        Function to disable notification for the caller client.
 *
 * @param handle Instance handle.
 * @param id     Id obtained through registerClient call.
 * @param subId  Sub notification entrys on which client wants to disable
 *               notificatioon.
 */
Int32
ClientNotifyMgr_disableNotification (ClientNotifyMgr_Handle  handle,
                                     UInt32                  id,
                                     UInt32                  subId ) ;

/*!
 *  @brief      API to enable notification for subnotification entries of a
 *              given notifyid(key). It also sets waitOnMultiNotifyFlags flag to
 *              enable ClientNotifyMgr_call back function to call the APP call
 *              back function only when event flags of the all the mentioned
 *              subIds are set.
 * @param handle Instance handle.
 * @param id     Id obtained through registerClient call.
 * @param subId  Sub notification entries on which client wants to wait for
 *               notification.
 * @param numSubIds  Number of sub notify entries in the given subId array.
 */
Int32 ClientNotifyMgr_waitOnMultiSubEntryNotify (
                                              ClientNotifyMgr_Handle  handle,
                                              UInt32                  id,
                                              UInt32                  subId[],
                                              UInt32                  numSubIds);

/*!
 *  @brief Function to send notification to all the clients of ClientNotifyMgr
 *         instance except the caller client.
 *
 * @param handle Instance handle.
 * @param Id     Id obtained through registerClient call.
 * @param subId  Client notifies the other clients waiting on this subnotify
 *               entry id.
 * @param arg1   Value to checked against water mark. For FrameQBufMgr it should
 *               be number of free buffers whose size  is  arg2 value.
 * @param arg2   Size of the buffers.
 */
Int32
ClientNotifyMgr_sendNotificationMulti (ClientNotifyMgr_Handle  handle,
                                       UInt32                  id,
                                       UInt32                  subId,
                                       UInt32                  arg1 ,
                                       UInt32                  arg2 ) ;

/*!
 *  @brief     Function to send notification to the client identified the id.
 *
 *  @param  handle     Instance handle.
 *  @param Id         Id of the remote client to which notification needs to be
 *                     sent.see {@link #registerClient}.Caller client needs to
 *                     know the remote client's notifyId to be able to send
 *                     notification.
 * @param subId  Client notifies the other client waiting on this subnotify
 *               entry id.
 *  @param arg1       Value to checked against water mark cond1.
 *  @param arg2       Value to checked against water mark cond2 of the
 *                     other client.
 *
 */
Int32
ClientNotifyMgr_sendNotification (ClientNotifyMgr_Handle  handle,
                                  UInt32                  id,
                                  UInt32                  subId,
                                  UInt32                  arg1,
                                  UInt32                  arg2);

/*!
 *  @brief  Function to get ClientNotifyMgr Id.
 *
 *  @param  handle  Handle to the instance.
 *
 */
UInt32
ClientNotifyMgr_getId (ClientNotifyMgr_Handle handle);

/*!
 *  @brief     Function to send Force notification to the client  whose
 *             registration id is passed.
 *
 *  @param  handle     Instance handle.
 *  @param  Id         Id of the remote client to which notification needs to be
 *                     sent.see {@link #registerClient}.Caller client needs to
 *                     know the remote client's notifyId to be able to send
 *                     notification.
 *  @param  payLoad    payload that can be sent  to the remote client.
 *
 */
Int32
ClientNotifyMgr_sendForceNotification (ClientNotifyMgr_Handle  handle,
                                       UInt32                  Id,
                                       UInt16                  payLoad);
/*!
 *  @brief Function to send notification to all the clients of ClientNotifyMgr
 *         instance except the caller client.
 *
 * @param handle Instance handle.
 * @param Id     Id obtained through registerClient  call for the caller.
 * @param payLoad  16 bit payload that needs to be sent to the all the other
 *                 valid clients whose call back is registered.
 */
Int32
ClientNotifyMgr_sendForceNotificationMulti (ClientNotifyMgr_Handle  handle,
                                            UInt32                  Id,
                                            UInt16                  payLoad );

#if defined (__cplusplus)
}
#endif

#endif
