/** 
 *  @file   _FrameQ_ShMem.h
 *
 *  @brief  Internal header file for FrameQBufMgr ShMem.
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



#ifndef _INT_FRAMEQ_SHMEM_H
#define _INT_FRAMEQ_SHMEM_H


#include <ti/ipc/GateMP.h>
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/SysLink.h>

#include <ti/syslink/FrameQ.h>
#include <ti/syslink/FrameQ_ShMem.h>

#include <ti/syslink/inc/_FrameQ.h>


#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * Macros & defines
 * =============================================================================
 */
/* TODO - why do we duplicate all this?  Why not have the delegates use
 * definitions from the front end?  No one should be binding to these
 * delegate-specific definitions, right?
 */
#define FrameQ_ShMem_MAXNAMELEN     FrameQ_MAX_NAMELEN
#define FrameQ_ShMem_CREATED        FrameQ_CREATED
#define FrameQ_ShMem_VERSION        FrameQ_VERSION
#define FrameQ_ShMem_DYNAMIC_CREATE FrameQ_DYNAMIC_CREATE
#define FrameQ_ShMem_DYNAMIC_OPEN   FrameQ_DYNAMIC_OPEN

#define FrameQ_ShMem_NOTIFY_RESERVED_EVENTNO                           \
                                                 FrameQ_NOTIFY_RESERVED_EVENTNO
#define FrameQ_ShMem_MAX_NAMELEN FrameQ_MAX_NAMELEN
#define FrameQ_ShMem_CACHE_LINESIZE   FrameQ_CACHE_LINESIZE

#define FrameQ_ShMem_FRAMEHDR_ISCACHEENABLED                             \
        FrameQ_FRAMEHDR_ISCACHEENABLED

#define FrameQ_ShMem_CTLSTRUCT_ISCACHEENABLED                           \
        FrameQ_CTLSTRUCT_ISCACHEENABLED
#define FrameQ_ShMem_FRAMEBUF_CACHEFLAGS_BITOFFSET                      \
    FrameQ_FRAMEBUF_CACHEFLAGS_BITOFFSET

#define FrameQ_ShMem_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET                  \
    FrameQ_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET

#define FrameQ_ShMem_INTERFACE_SHAREDMEM FrameQ_INTERFACE_SHAREDMEM

#define FrameQ_ShMem_MAX_INST_READERS FrameQ_MAX_INST_READERS

#define FrameQ_ShMem_MAXFILLEDQUEUS_FOR_READER FrameQ_MAXFILLEDQUEUS_FOR_READER

#define FrameQ_ShMem_MODE_WRITER FrameQ_MODE_WRITER
#define FrameQ_ShMem_MODE_READER FrameQ_MODE_READER

#define FrameQ_ShMem_DYNAMIC_CREATE_USEDREGION FrameQ_DYNAMIC_CREATE_USEDREGION


typedef FrameQ_NotifyParams     FrameQ_ShMem_NotifyParams;
typedef FrameQ_Attrs            FrameQ_ShMem_Attrs;
typedef FrameQ_FrameBufInfo     FrameQ_ShMem_FrameBufInfo;
typedef FrameQ_ReaderClient     FrameQ_ShMem_ReaderClient;
typedef FrameQ_WriterClient     FrameQ_ShMem_WriterClient;
typedef FrameQ_FrameQueue       FrameQ_ShMem_FrameQueue;


typedef Frame_FrameHeader       *FrameQ_ShMem_Frame;



/*!
 *  @brief  Handle for the FrameQ instance.
 */
typedef  struct FrameQ_ShMem_Object_tag * FrameQ_ShMem_Handle;

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*
 *  @brief      Creates a new instance of FrameQ using ShMem.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         FrameQ_ShMem_delete, FrameQ_ShMem_open, FrameQ_ShMem_close
 */
FrameQ_ShMem_Handle
FrameQ_ShMem_create (FrameQ_ShMem_Params * params);

/*
 *  @brief  Function to Open the ShMem instance in read or write mode.
 *  @param  handlePtr  Pointer to return the  handle.
 *  @param  name       Name of the instance that needs to be opened.
 *  @param  sharedAddr Address of the shared memory. This is must if Name server
 *                     is not configured or name is not provided.
 *  @param  openParams Open parameters structure
 *  @param  gate       Gate for protection.
 */
Int32
FrameQ_ShMem_open (FrameQ_ShMem_OpenParams *openParams,
                   FrameQ_ShMem_Handle *handlePtr);
/*
 *  @brief  Function to delete an instance of FrameQ module.
 *  @param  handle     Pointer to handle for a previously created instance.
 */
Int32
FrameQ_ShMem_close (FrameQ_ShMem_Handle *pHandle);

/*
 *  @brief  Function to delete an instance of FrameQ module.
 *  @param  handle     Pointer to handle for a previously created instance.
 */
Int32
FrameQ_ShMem_delete (FrameQ_ShMem_Handle *pHandle);

/*
 * @brief Function to allocate a frame.
 *
 * @param  handle   Instance handle.
 * @param  framePtr Location to receive the allocated frame
 *
 */
Int32
FrameQ_ShMem_alloc (FrameQ_ShMem_Handle  handle,
                    FrameQ_ShMem_Frame   *framePtr);
/*
 * @brief   Function to allocate frames from multiple free pools of a plugged in
 *          FrameQBufMgr.
 *
 *          After API returns,numframes will denotes  the number of successfully
 *          allocated frames.
 *
 * @param  handle   Instance handle.
 * @param  framePtr Location to receive the allocated frame
 *
 */
Int32
FrameQ_ShMem_allocv (FrameQ_ShMem_Handle  handle,
                     FrameQ_ShMem_Frame   framePtr[],
                     UInt32                freeQId[],
                     UInt8               *numFrames);
/*
 *  @brief Function to free a frame .
 *         Allow only writers/readers to call  this API.
 * @param framePtr    Frame to be duplicated.
 *
 */
Int32
FrameQ_ShMem_free (FrameQ_ShMem_Handle handle, FrameQ_ShMem_Frame  frame);
/*
 *  @brief Function to free multiple frames .
 *         Allow only writers/readers to call  this API.
 * @param framePtr    Frame to be duplicated.
 *
 */
Int32
FrameQ_ShMem_freev (FrameQ_ShMem_Handle  handle,
                    FrameQ_ShMem_Frame   framePtr[],
                    UInt32               numFrames);

/*
 *  @brief Function to insert frame in to FrameQ .
 *         Allow only writers/readers to call  this API.
 *  @param framePtr    Frame to be duplicated.
 *
 */
Int32
FrameQ_ShMem_put (FrameQ_ShMem_Handle handle, FrameQ_ShMem_Frame frame);

/*
 *  @brief  Function to insert multiple frames  in to queues of a
 *          FrameQ reader client. If multiple readers exist, It dups the given
 *          frames and insert in to other reader queues.
 *
 *  @param handle       Instance handle.
 *  @param framePtr     Array of frames to be inserted.
 *  @param filledQueueId    Array of filled queues of reader.
 *  @param numFrames    Number of frames.
 */
Int32
FrameQ_ShMem_putv (FrameQ_ShMem_Handle handle,
                   FrameQ_ShMem_Frame  framePtr[],
                   UInt32              filledQueueId[],
                   UInt8               numFrames);

/*
 *  @brief Function to get frame from FrameQ .
 *         Allowed only readers to call  this API.
 *  @param handle      Frame to be duplicated.
 *  @param framePtr    Location to receive the frame.
 *
 */
Int32
FrameQ_ShMem_get (FrameQ_ShMem_Handle handle, FrameQ_ShMem_Frame *framePtr);

/*
 *  @brief Function to retrieve frames from  queues of a FrameQ Reader client.
 *         It returns frame if it is available in the queue.
 *  @param handle      Frame to be duplicated.
 *  @param framePtr    Location to receive the frame.
 *
 */
Int32
FrameQ_ShMem_getv (FrameQ_ShMem_Handle handle,
                   FrameQ_ShMem_Frame  pframe[],
                   UInt32              filledQueueId[],
                   UInt8               *numFrames);

/*
 *  @brief Function to duplicate the given frame.
 *  @param handle      Instance handle.
 *  @param frame       Frame to be duplicated.
 *  @param dupedFrame  Location to receive the duplicated frame.
 *
 */
Int32
FrameQ_ShMem_dup (FrameQ_ShMem_Handle handle,
                  FrameQ_ShMem_Frame  frame,
                  FrameQ_ShMem_Frame* dupedFrame);

/*
 * @brief   API to resister a call back function.
 *
 *          For reader :
 *          It registers notification function to notify about available frames
 *          in the FrameQ.
 *          For writer :
 *          It registers notification function to notify about available free
 *          frames in the Plugged in FrameQBufMgr.
 *
 * @param handle      Reader client Handle.
 * @param notifyType  Type of notification.
 * @param watermark   WaterMark
 * @param condition   Unused for FrameQ.
 * @param notifyFunc  Call back function
 * @param cbContext   Context pointer that needs to be passed to call back.
 */

Int32
FrameQ_ShMem_registerNotifier (
                            FrameQ_ShMem_Handle              handle,
                            FrameQ_ShMem_NotifyParams *notifyParams);
/*
 * @brief Function to un resister call back function.
 *
 *        Allowed  readers to call  this API.
 *        For reader :
 *        It Un registers notification function.
 *
 * @param handle      Reader client Handle.
 */
Int32
FrameQ_ShMem_unregisterNotifier (FrameQ_ShMem_Handle handle);

/*
 * @brief Function to send forced notification to the reader clients.
 *
 * @param handle    Reader client Handle.
 * @param msg       Payload.
 */
Int32
FrameQ_ShMem_sendNotify (FrameQ_ShMem_Handle handle, UInt16 msg);

/*
 * @brief Function to find out the number of available frames in a FrameQ .
 *
 *        For Writer:
 *        It returns the  number of frames in the primary reader's filled
 *        queue 0.
 *        For Reader:
 *        It returns the number of frames in the Reader's(caller)filledqueue 0.
 *
 * @param handle      Reader client Handle.
 * @param numFrames   Location to recieve the number of frames.
 */
Int32
FrameQ_ShMem_getNumFrames(FrameQ_ShMem_Handle handle,
                          UInt32         *numFrames);

/*
 * @brief For Reader this function is to get the available frames  from the queues
 * associated with it.
 * For writer this function is to get the available frames  from the queues
 * associated with the primary Reader(first reader).
 *
 * @param handle      Reader client Handle.
 * @param numFrames   Location to recieve the number of frames.
 */
Int32
FrameQ_ShMem_getvNumFrames (FrameQ_ShMem_Handle     handle,
                            UInt32                  numFrames[],
                            UInt8                   filledQId[],
                            UInt8                   numFilledQids);

Int32
FrameQ_ShMem_getNumFreeFrames(FrameQ_ShMem_Handle handle,
                              UInt32              *numFreeFrames);
Int32
FrameQ_ShMem_getvNumFreeFrames (FrameQ_ShMem_Handle     handle,
                                UInt32                  numFreeFrames[],
                                UInt8                   freeQId[],
                                UInt8                   numFreeQids);
/*
 * @brief Provides a hook to perform implementation dependent operation
 *
 * @param handle    Instance handle.
 * @param cmd       Command to perform.
 * @param arg       void * argument.
 */

Int32
FrameQ_ShMem_control (FrameQ_ShMem_Handle  handle,
                      Int32                cmd,
                      Ptr                  arg);


/* Function to return the clientNotifyMgr  handle  used in the instance */
Ptr _FrameQ_ShMem_getCliNotifyMgrHandle (FrameQ_ShMem_Handle handle);

/*!
 * @brief Function to get the gate used in  client notifyMgr of the
 *        FrameQ instance.
 * @param handle      FrameQ instance Handle.
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrGate (FrameQ_ShMem_Handle handle);

/* Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for instance.
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrShAddr (FrameQ_ShMem_Handle handle);

/* Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 */
Ptr _FrameQ_ShMem_getCliNotifyMgrGateShAddr (FrameQ_ShMem_Handle handle);

/* Function to set the notifyid received in userspace during call to
 *  ClientNotifyMgr_register client.
 */
Int32 _FrameQ_ShMem_setNotifyId (FrameQ_ShMem_Handle handle, UInt32 notifyId);

/* Function to set the notifyid received in userspace during call to
 * ClientNotifyMgr_unregister client.
 */
Int32 _FrameQ_ShMem_resetNotifyId (FrameQ_ShMem_Handle handle, UInt32 notifyId);

GateMP_Handle FrameQ_ShMem_getGate(FrameQ_ShMem_Handle handle);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* HEAPBUF_H_0x4CD5 */

