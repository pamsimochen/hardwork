/** 
 *  @file   _FrameQ.h
 *
 *  @brief      Defines for interfaces for FrameQ module.
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



#ifndef _FRAMEQ_H_
#define _FRAMEQ_H_

#include <ti/ipc/GateMP.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/FrameQDefs.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

/* =============================================================================
 * macros & defines
 * =============================================================================
 */
/*!
 *  @brief  Module ID for FrameQ.
 */
#define FrameQ_MODULEID        0x6e6f

/* Name of the reserved NameServer used for FrameQ. */
#define FRAMEQ_NAMESERVERNAME  "FrameQ"

/*!
 *  @brief  Constant value to indicate that FrameQ instance is created.
 */
#define FrameQ_CREATED       (0x05251995) /* Should be same as rtos
                                           * implementation
                                           */
/*!
 *  @brief  Version of FrameQ Module.
 */
#define FrameQ_VERSION                (1u) /* Should be same as rtos
                                            * implementation
                                            */

/*!
 *  @brief  One of the FrameQ instance create types.
 *          It indicates FrameQ instance is created statically.
 */
#define FrameQ_STATIC_CREATE          (0x1u) /* Should be same as rtos
                                              * implementation
                                              */
/*!
 *  @brief  One of the FrameQ instance create types.
 *          It indicates FrameQ instance is created statically using shared
 *          region.
 */
#define STATIC_CREATE_USEDREGION       (0x2)

/*!
 *  @brief  One of the FrameQ instance create types.
 *          It indicates FrameQ instance is created dynamically.
 */
#define FrameQ_DYNAMIC_CREATE          (0x4u) /* Should be same as rtos
                                               * implementation
                                               */

/*!
 *  @brief  One of the FrameQBufMgr instance open types.
 *          It indicates FrameQBufMgr instance is opened dynamically.
 */
#define FrameQ_DYNAMIC_CREATE_USEDREGION (0x8u)

/*!
 *  @brief  One of the FrameQ instance open types.
 *          It indicates FrameQ instance is opened statically.
 */
#define FrameQ_STATIC_OPEN            (0x10u) /* Should be same as rtos
                                               * implementation
                                               */

/*!
 *  @brief  One of the FrameQ instance open types.
 *          It indicates FrameQ instance is opened dynamically.
 */
#define FrameQ_DYNAMIC_OPEN           (0x20u) /* Should be same as rtos
                                            * implementation
                                            */

#define FrameQ_INVALID                (0xFF)



#define FrameQ_MODE_NONE              (0)


#define  FrameQ_FRAMEBUF_CACHEFLAGS_BITOFFSET      8

#define  FrameQ_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET 16

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
 /*!
 *  @brief  Structure defining open params.
 */
typedef struct FrameQ_OpenParams_tag {
    FrameQ_CommonOpenParams     commonOpenParams;
} FrameQ_OpenParams;


/*!
 *  @brief  Structure to maintain the queue related information.
 */
typedef struct FrameQ_FrameQueue_Tag {
    volatile UInt32          readerIndex;
    /*!< To identify the reader   to which, this quque is allocated.  */
    volatile UInt32          numFrames[FrameQ_MAXFILLEDQUEUS_FOR_READER];
    /*!< Number of frames available in this queue */
    volatile Bool            isAllocated;
    /*!< Flag  denoting whether queue is allocated to any reader or not.*/
} FrameQ_FrameQueue;

/*!
 *  @brief  Shared  control structure for the instance attributes.
 */
typedef struct FrameQ_Attrs_Tag {
    volatile UInt32      status;
    /*!< Flag denotes whether instance is created or not.*/
    volatile UInt32      version;
    /*!< Version of the  instance.*/
    volatile UInt32      localProtect;
    /*!< Max reader clients supported for this instance.*/
    volatile UInt32      remoteProtect;
    /*!< Max reader clients supported for this instance.*/
    volatile Ptr         gateMPAddr;
    /*!< GateMP address (shm safe)     */
    volatile UInt32      numReaders;
    /*!< Max reader clients supported for this instance.*/
    volatile UInt32      numActiveReaders;
    /*!< Denotes number of readers currenty opened this instance.*/
    volatile UInt32      numQueues;
    /*!< Denotes number of queues configured for each reader.*/
    volatile UInt32      primaryQueueId;
    /*!< QueueId to which writer sends frames if there are no active readers
     * or only one reader exist.
     */
    volatile UInt32      frmQBufMgrId;
    /* ID of the plugged in FrameQBufMgr */
    /*!< Array of frame queues.*/
}FrameQ_Attrs;

/*!
 *  @brief  Writer client structure in shared memory.
 */
typedef  struct FrameQ_WriterClient_Tag {
    volatile UInt32      isValid;
    /*!< if TRUE, denotes writer client is valid. If FALSE, denotes writer
     *   client is not valid
     */
    volatile UInt32      procId;
    /*!< Processor Id of the writer client */
    volatile UInt32      cacheFlags;
    /*!< Cache flags for the frame buffer and frame header.*/
    volatile Bool        isRegistered;
    /*!< Indicates whether writer has registered with FrameQBufMgr for
     *   notifications.
     */
} FrameQ_WriterClient;

/*!
 *  @brief  Reader client structure in shared memory.
 */
typedef  struct FrameQ_ReaderClient_Tag {
    volatile UInt32      isValid;
    /*!< if TRUE, denotes reader client is valid. If FALSE, denotes reader
     *   client is not valid.
     */
    volatile UInt32      procId;
    /*!< Processor Id of the reader client */
    volatile UInt32      cacheFlags;
    /*!< Cache flags for the frame buffer and frame header.*/
    volatile UInt32      frameQIndex;
    /*!< Id of the FrameQueue from which this reader client retrieves frames */
    volatile UInt32      notifyId;
    /*!<  Id  returned by the register client notification */
    volatile Bool        isRegistered;
    /*!< If TRUE, denotes reader has registered notification function */
} FrameQ_ReaderClient;



/*!
 *  @brief  Structure defining additional specific parameters that name server maintains
	 * along with the interface type for ShMem implementation.
 */
typedef  struct FrameQ_ShMem_InstInfoParams_tag  {
        Ptr        sharedAddr;
        UInt32     sharedAddrSize;
        Ptr        instGateMPAddr;
        Ptr        frameQBufMgrSharedAddr;
        UInt8      frameQBufMgrName[FrameQ_MAX_NAMELEN];
        Ptr        cliGateMPAddr;
}FrameQ_ShMem_InstInfoParams;

/*!
 *  @brief  Format of the  FrameQ instance entry in Nameserver.
 */
typedef struct FrameQ_NameServerEntry_tag {
   UInt32 interfaceType;
   /*!< FrameQBufMgr type */
	union {
        FrameQ_ShMem_InstInfoParams  shMemParams;
	} instParams;

}FrameQ_NameServerEntry;


/* =============================================================================
 *  Internal APIs
 * =============================================================================
 */

typedef Ptr
(*fq_create) (Ptr  params);

typedef Int32
(*fq_open) (Ptr openParams,Ptr *handlePtr);

/*
 * Type for function pointer to FrameQ_openByAddress function.
 */
typedef Int32 (*fq_openByAddress) (Ptr * pHandle,
                                   Ptr   sharedAddr,
                                   Ptr   openParams);

typedef Int32
(*fq_close) (Ptr *pHandle);


typedef Int32
(*fq_delete) (Ptr *pHandle);

/*
 * @brief Function to allocate a frame.
 *
 * @param  handle   Instance handle.
 * @param  framePtr Location to receive the allocated frame
 *
 */
typedef Int32
(*fq_alloc) (Ptr  handle,
              FrameQ_Frame   *framePtr);
/*
 * @brief   Function to allocate frames from multiple free pools of a plugged in
 *          FrameQBufMgr.
 *
 *          After API returns,numframes will denotes the number of successfully
 *          allocated frames.
 *
 * @param  handle   Instance handle.
 * @param  framePtr Location to receive the allocated frame
 *
 */
typedef Int32
(*fq_allocv) (Ptr  handle,
                     FrameQ_Frame   framePtr[],
                     UInt32                freeQId[],
                     UInt8               *numFrames);
/*
 *  @brief Function to free a frame.
 *         Allow only writers/readers to call  this API.
 *
 * @param framePtr    Frame to be duplicated.
 */
typedef Int32
(*fq_free) (Ptr handle, FrameQ_Frame  frame);
/*
 *  @brief Function to free multiple frames .
 *         Allow only writers/readers to call  this API.
 *
 * @param framePtr    Frame to be duplicated.
 */
typedef Int32
(*fq_freev) (Ptr  handle,
             FrameQ_Frame   framePtr[],
             UInt32               numFrames);


typedef Int32
(*fq_put) (Ptr handle, FrameQ_Frame frame);

typedef Int32
(*fq_putv) (Ptr handle,
                   FrameQ_Frame  framePtr[],
                   UInt32               filledQueueId[],
                   UInt8               numFrames);

typedef Int32
(*fq_get) (Ptr handle, FrameQ_Frame *framePtr);

typedef Int32
(*fq_getv) (Ptr handle,
                   FrameQ_Frame  pframe[],
                   UInt32              filledQueueId[],
                   UInt8               *numFrames);

typedef Int32
(*fq_dup) (Ptr handle,
                  FrameQ_Frame  frame,
                  FrameQ_Frame* dupedFrame);

typedef Int32
(*fq_registerNotifier) (
                            Ptr              handle,
                            FrameQ_NotifyParams *notifyParams);

typedef Int32
(*fq_unregisterNotifier) (Ptr handle);


typedef Int32
(*fq_sendNotify) (Ptr handle, UInt16 msg);


typedef Int32
(*fq_getNumFrames) (Ptr handle,
                          UInt32         *numFrames);


typedef Int32
(*fq_getvNumFrames) (Ptr     handle,
                            UInt32                  numFrames[],
                            UInt8                   filledQId[],
                            UInt8                   numFilledQids);


typedef Int32
(*fq_getNumFreeFrames) (Ptr handle,
                          UInt32         *numFreeFrames);


typedef Int32
(*fq_getvNumFreeFrames) (Ptr       handle,
                         UInt32    numFreeFrames[],
                         UInt8     freeQId[],
                         UInt8     numFreeQids);

typedef Int32
(*fq_control) (Ptr  handle,
                      Int32                cmd,
                      Ptr                  arg);

typedef Ptr
(*fq_getCliNotifyMgrHandle) (Ptr handle);

typedef Ptr
(*fq_getCliNotifyMgrGate) (Ptr handle);

/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 *
 * @param handle      FrameQ instance Handle.
 */
typedef Ptr
(*fq_getCliNotifyMgrShAddr) (Ptr handle);

typedef Ptr
(*fq_getCliNotifyMgrGateShAddr) (Ptr handle);

typedef Int32
(*fq_setNotifyId) (Ptr handle, UInt32 notifyId);

typedef Int32
(*fq_resetNotifyId) (Ptr handle, UInt32 notifyId);

/* Structure defining Interface functions for FrameQBufMgr */
typedef struct FrameQ_fxns_tag {
    fq_create               create;
    fq_delete               deleteInstance;
    fq_open                 open;
    fq_openByAddress        openByAddress;
    fq_close                close;
    fq_alloc                alloc;
    fq_allocv               allocv;
    fq_free                 free;
    fq_freev                freev;
    fq_put                  put;
    fq_putv                 putv;
    fq_get                  get;
    fq_getv                 getv;
    fq_dup                  dup;
    fq_registerNotifier     registerNotifier;
    fq_unregisterNotifier   unregisterNotifier;
    fq_sendNotify           sendNotify;
    fq_getNumFrames         getNumFrames;
    fq_getvNumFrames        getvNumFrames;
    fq_control              control;
    fq_getCliNotifyMgrHandle getCliNotifyMgrHandle;
    fq_getCliNotifyMgrGate   getCliNotifyMgrGate;
    fq_getCliNotifyMgrShAddr getCliNotifyMgrShAddr;
    fq_getCliNotifyMgrGateShAddr getCliNotifyMgrGateShAddr;
    fq_setNotifyId               setNotifyId;
    fq_resetNotifyId             resetNotifyId;
    fq_getNumFrames         getNumFreeFrames;
    fq_getvNumFrames        getvNumFreeFrames;
}FrameQ_Fxns;


Ptr _FrameQ_getCliNotifyMgrHandle (FrameQ_Handle handle);

/*!
 * @brief Function to get the gate used in  client notifyMgr of the
 *        FrameQ instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_getCliNotifyMgrGate (FrameQ_Handle handle);

/* Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for instance.
 */
Ptr _FrameQ_getCliNotifyMgrShAddr (FrameQ_Handle handle);

/* Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 */
Ptr _FrameQ_getCliNotifyMgrGateShAddr (FrameQ_Handle handle);

/* Function to set the notifyid received in userspace during call to
 *  ClientNotifyMgr_register client.
 */
Int32 _FrameQ_setNotifyId (FrameQ_Handle handle, UInt32 notifyId);

/* Function to set the notifyid received in userspace during call to
 * ClientNotifyMgr_unregister client.
 */
Int32 _FrameQ_resetNotifyId (FrameQ_Handle handle, UInt32 notifyId);

GateMP_Handle FrameQ_getGate(FrameQ_Handle handle);

#if defined (__cplusplus)
}
#endif


#endif /*FRAMEQ_H_0x6e6f*/
