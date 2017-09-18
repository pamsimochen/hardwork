/** 
 *  @file   _ClientNotifyMgr.h
 *
 *  @brief      Defines for interfaces for ClientNotifyMgr module.
 *
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



#ifndef _CLIENTNOTIFYMGR_H_0x684e
#define _CLIENTNOTIFYMGR_H_0x684e


#if defined(SYSLINK_BUILD_HLOS)
#include <ti/syslink/utils/List.h>
#endif
#if defined(SYSLINK_BUILD_RTOS)
#include <ti/sdo/utils/List.h>
#endif


#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/ClientNotifyMgr_config.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>

#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * macros & defines
 * =============================================================================
 */

/*!
 *  @brief  Constant value to indicate that ClientNotifyMgr instance is created.
 */
#define CLIENTNOTIFYMGR_CREATED            (0x05251995)

/*!
 *  @brief  Version of ClientNotifyMgr Module.
 */
#define CLIENTNOTIFYMGR_VERSION            (1u)

/*!
 *  @brief  One of the CLIENTNOTIFYMGR instance create types.
 *          It indicates ClientNotifyMgr instance is created statically.
 */
#define CLIENTNOTIFYMGR_STATIC_CREATE      (1u)

/*!
 *  @brief  One of the ClientNotifyMgr instance create types.
 *          It indicates ClientNotifyMgr instance is created dynamically.
 */
#define CLIENTNOTIFYMGR_DYNAMIC_CREATE     (2u)

/*!
 *  @brief  One of the ClientNotifyMgr instance open types.
 *          It indicates ClientNotifyMgr instance is opened statically.
 */
#define CLIENTNOTIFYMGR_STATIC_OPEN        (4u)

/*!
 *  @brief  One of the ClientNotifyMgr instance open types.
 *          It indicates CLIENTNOTIFYMGR instance is opened dynamically.
 */
#define CLIENTNOTIFYMGR_DYNAMIC_OPEN        (8u)


#define CLIENTNOTIFYMGR_SWI_PAYLOAD_ENTRIES  (1024)

#define CLIENTNOTIFYMGR_TSK_PAYLOAD_ENTRIES  (1024)

#define CLIENTNOTIFYMGR_SWI_PRIORITY         (0)

#define CLIENTNOTIFYMGR_TSK_PRIORITY         (2)

#define  CLIENTNOTIFYMGR_BIT_FLAG_SIZE                                         \
                         ( 1+ ((CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES - 1)/32))

#define  CLIENTNOTIFYMGR_NotifyParamInfo_elemsSize                             \
                          (  (4 * 3)                                           \
                           + (2 * 2)                                           \
                           +  2                                                \
                           +  1                                                \
                           +  CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES            \
                           +  CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES            \
                           + (CLIENTNOTIFYMGR_BIT_FLAG_SIZE *4)                \
                           +  1)

#define  CLIENTNOTIFYMGR_NotifyParamInfo_ALIGNEDSIZE                           \
                           ( (  (  (  CLIENTNOTIFYMGR_NotifyParamInfo_elemsSize \
                                     + ClientNotifyMgr_CACHE_LINESIZE)         \
                                  - 1)                                         \
                               / ClientNotifyMgr_CACHE_LINESIZE)               \
                             * ClientNotifyMgr_CACHE_LINESIZE)                 \

/*!
 *  @brief  Padding  for  NotifyParamInfo struct.
 */
#define CLIENTNOTIFYMGR_NotifyParamInfo_STRUCT_PADDING                         \
                   ( (  CLIENTNOTIFYMGR_NotifyParamInfo_ALIGNEDSIZE            \
                      - CLIENTNOTIFYMGR_NotifyParamInfo_elemsSize)             \
                    /2)

/*!
 *  @brief  Padding required for the Attrs attribute structure.
 */
#define  CLIENTNOTIFYMGR_Attrs_STRUCT_PADDING                         \
                  (  (   ClientNotifyMgr_CACHE_LINESIZE               \
                       - ((4 * 7)+ ClientNotifyMgr_maxNameLen + 2))   \
                    / 2)

/*!
 *  @brief  Notification param info structure. For each registered client
 *          ClientNotifyMgr instance maintains an object of this structure type
 *          in shared memory.
 */
typedef struct ClientNotifyMgr_NotifyParamInfo_Tag {
    volatile UInt32 clientHandle;
  /*!< Handle that needs to passed to call back function. Clients are expected
     * to pass ClientNotifyMgr/FrameQ reader handle in register client function
     */
    volatile UInt32 waterMarkCond1;
    /*!< water mark condition 1.*/
    volatile UInt32 waterMarkCond2;
    /*!< Water mark condition 2.
     *       Set to -1 for second water mark in register client if second
     *       water mark condition is not required
     */
    volatile UInt32 bitFlag_notifySubEntries[CLIENTNOTIFYMGR_BIT_FLAG_SIZE];
    /*!< Indicates On which sub notification entries client is waiting on for
     * notification.
     */
    volatile UInt16 procId;
    /*!< Processor id of the client */
    volatile UInt16 notifyType;
    /*!< Notification type */
    volatile UInt16 isValid;
   /*!< Indicated whether this notification entry is valid or not */
    volatile UInt8  waitOnMultiNotifyFlags;
    /*!< Indicates Client is waiting for notification only when multiple sub
     * notify events are set
     */
    volatile UInt8  notifyFlag[CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES];
    /*!< Flag indicates whether notification needs to be send or not.
     *   If TRUE notification will be sent to client. No notification will
     *   be sent if it false.  one entry for each sub notify entry.
     */
    volatile UInt8  eventCount[CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES];
    /*!< Flag indicates whether callback function is to be called in
     *   ClientNotifyMgr_callback fxn. If non-zero Callback function will be
     *   called one entry for each sub notify entry.
     */
    volatile UInt8  forceNotify;
    /*!< set to TRUE by the remote clients who wish to send force notification
     * this client. ClientNotifMgr_callback checks this flag and calls callback
     * function directly.
     */
 } ClientNotifyMgr_NotifyParamInfo;

/*!
 *  @brief  Shared control strucrue for the instance attributes.
 */
typedef struct ClientNotifyMgr_Attrs_Tag {
    volatile UInt32   status;
    /*!< Flag denotes whether instance is created or not.*/
    volatile UInt32   version;
    /*!< Version of the  instance.*/
    volatile UInt32   creatorProcId;
    /*!< Processor id of the instance creator */
    volatile UInt32   eventNo;
    /*!<  Notify eventNo that this instance uses
     */
    volatile UInt32   maxClients;
    /*!< Maximun number of clients that  can use this instance */
    volatile UInt32   numNotifyEntries;
    /*!< Number of Notify entries */
    volatile UInt32   numSubNotifyEntries;
    /*!< Denotes the number of sub entries present in each NotifyParamInfo filed
     *   of the instannce. Enables client of the instance to use same notify
     *   info( such as water mark) to check different conditions in the other
     *   clients. This allows FrameQBufMgr to use same call back function to
     *   handle notifications for individual FramePools separately by having
     *   registered with clientNotifyMgr once and Passing subNotifyEntryId
     *   (equivalent to FramePoolNo) as additional arg to enable/disable/
     *   sendNotification multi functions.
     */
    volatile UInt32   numActClients;
    /*!< Number of clients currently using this instance */
    volatile UInt16   entryId;
    /*!< Id of the instance. MSB byte contains the creator procId and LSB
     * contains the instance no  on the creator processor.On each processor we
     * can create  256 instances only.
     */
} ClientNotifyMgr_Attrs;

/*!
 *  @brief  ClientNotifyMgr_eventListenerInfo structure
 */
typedef struct ClientNotifyMgr_eventListenerInfo_tag
{
    List_Elem               listElem;
    ClientNotifyMgr_FnCbck  cbckFxn;
    /*!< Call back function.Gets called when there is a notification*/
    UInt32                  notifyId;
    /*!< Notify registratin id of the client. This is actually an index to
     *   notifyparaminfo list where this clients other notification info is
     *   available.
     */
    Ptr cbContext;
    /*!< Context pointer paseed to call back function*/
}ClientNotifyMgr_eventListenerInfo;


typedef struct ClientNotifyMgr_callbackCtx_tag {
    GateMP_Handle                    gate;
    /*!< Gate handle for protection. Same gate that is provided to
     * instance create/open call.
     */
    volatile ClientNotifyMgr_Attrs  *attrs;
    /* Instance's shared control info pointer*/
    volatile Ptr                  notifyInfoObjBaseAddr;
    /* Instance notify entries base address */
    UInt32                        minAlign;
    Bool                           cacheEnabled;
    /*!<  Flag indicating whether cache coherence API needs to be called for
     *    instance control structures
     */
    UInt32                          instId;
}ClientNotifyMgr_callbackCtx;

typedef struct ClientNotifyMgr_swiPostInfo_tag {
    Ptr                             swiHandle;
    UInt32                          notifyCnt;
    UInt32                          readIndex;
    UInt32                          writeIndex;
}ClientNotifyMgr_swiPostInfo;


typedef struct ClientNotifyMgr_TaskPostInfo_tag {
    Ptr                             tskHandle;
    Ptr                             semHandle;
    UInt32                          notifyCnt;
    UInt32                          readIndex;
    UInt32                          writeIndex;
}ClientNotifyMgr_TaskPostInfo;

typedef struct ClientNotifyMgr_notifyPayloadInfo_tag {
    Ptr                             cliCallbackCtx;
    UInt32                          notifyPayload;
}ClientNotifyMgr_notifyPayloadInfo;

/* =============================================================================
 * Defines for compatibility with SYSBIOS
 * =============================================================================
 */
#define ClientNotifyMgr_CREATED            CLIENTNOTIFYMGR_CREATED
#define ClientNotifyMgr_VERSION            CLIENTNOTIFYMGR_VERSION
#define ClientNotifyMgr_STATIC_CREATE      CLIENTNOTIFYMGR_STATIC_CREATE
#define ClientNotifyMgr_DYNAMIC_CREATE     CLIENTNOTIFYMGR_DYNAMIC_CREATE
#define ClientNotifyMgr_STATIC_OPEN        CLIENTNOTIFYMGR_STATIC_OPEN
#define ClientNotifyMgr_DYNAMIC_OPEN       CLIENTNOTIFYMGR_DYNAMIC_OPEN
#define ClientNotifyMgr_shModCtrlInfo_STRUCT_PADDING                          \
                                 CLIENTNOTIFYMGR_shModCtrlInfo_STRUCT_PADDING
#define ClientNotifyMgr_NotifyParamInfo_STRUCT_PADDING                        \
                                 CLIENTNOTIFYMGR_NotifyParamInfo_STRUCT_PADDING
#define  ClientNotifyMgr_Attrs_STRUCT_PADDING                                 \
                                 CLIENTNOTIFYMGR_Attrs_STRUCT_PADDING

#if defined (__cplusplus)
}
#endif

#endif
