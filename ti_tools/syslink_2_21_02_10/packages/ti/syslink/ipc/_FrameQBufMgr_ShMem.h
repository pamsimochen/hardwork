/** 
 *  @file   _FrameQBufMgr_ShMem.h
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


#ifndef _INT_FRAMEQBUFMGR_SHMEM_H
#define _INT_FRAMEQBUFMGR_SHMEM_H

#include <ti/ipc/GateMP.h>
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/SysLink.h>
#include "_FrameQBufMgr.h"
#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>

#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * Macros & defines
 * =============================================================================
 */
#define FrameQBufMgr_ShMem_CACHE_LINESIZE FrameQBufMgr_CACHE_LINESIZE

#define FrameQBufMgr_ShMem_MAX_FRAMEBUFS FrameQBufMgr_MAX_FRAMEBUFS

#define FrameQBufMgr_ShMem_MAX_POOLS FrameQBufMgr_MAX_POOLS

#define FrameQBufMgr_ShMem_INTERFACE_SHAREDMEM FrameQBufMgr_INTERFACE_SHAREDMEM

#define FrameQBufMgr_ShMem_BUF_FRAMEHEADER      FrameQBufMgr_BUF_FRAMEHEADER

#define FrameQBufMgr_ShMem_BUF_FRAMEBUF FrameQBufMgr_BUF_FRAMEBUF

#define FrameQBufMgr_ShMem_AddrType_Virtual FrameQBufMgr_AddrType_Virtual

#define FrameQBufMgr_ShMem_AddrType       FrameQBufMgr_AddrType
#define FrameQBufMgr_ShMem_BufType        FrameQBufMgr_BufType
#define FrameQBufMgr_ShMem_CREATED        FrameQBufMgr_CREATED
#define FrameQBufMgr_ShMem_VERSION        FrameQBufMgr_VERSION
#define FrameQBufMgr_ShMem_DYNAMIC_CREATE FrameQBufMgr_DYNAMIC_CREATE
#define FrameQBufMgr_ShMem_DYNAMIC_CREATE_USEDREGION FrameQBufMgr_DYNAMIC_CREATE_USEDREGION
#define FrameQBufMgr_ShMem_NOTIFY_RESERVED_EVENTNO FrameQBufMgr_NOTIFY_RESERVED_EVENTNO

#define FrameQBufMgr_ShMem_DYNAMIC_OPEN   FrameQBufMgr_DYNAMIC_OPEN
#define FrameQBufMgr_ShMem_POOL_DYNAMIC_MAXHDRS                               \
                                              FrameQBufMgr_POOL_DYNAMIC_MAXHDRS
#define FrameQBufMgr_ShMem_POOL_MAXHDRS FrameQBufMgr_POOL_MAXHDRS

#define FrameQBufMgr_ShMem_POOL_DYNAMIC_MAXFRAMES                             \
                                      FrameQBufMgr_POOL_DYNAMIC_MAXFRAMES

#define FrameQBufMgr_ShMem_POOL_MAXFRAMES    FrameQBufMgr_POOL_MAXFRAMES
#define FrameQBufMgr_ShMem_DYNAMIC_FRAME     FrameQBufMgr_DYNAMIC_FRAME
#define FrameQBufMgr_ShMem_STATIC_FRAME      FrameQBufMgr_STATIC_FRAME

#define FrameQBufMgr_ShMem_FRAMEHDR_ISCACHEENABLED                             \
        FrameQBufMgr_FRAMEHDR_ISCACHEENABLED

#define FrameQBufMgr_ShMem_CTLSTRUCT_ISCACHEENABLED                           \
        FrameQBufMgr_CTLSTRUCT_ISCACHEENABLED
#define FrameQBufMgr_ShMem_FRAMEBUF_CACHEFLAGS_BITOFFSET                      \
    FrameQBufMgr_FRAMEBUF_CACHEFLAGS_BITOFFSET

#define FrameQBufMgr_ShMem_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET                  \
    FrameQBufMgr_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET


//typedef FrameQBufMgr_Config           FrameQBufMgr_ShMem_Config ;
typedef FrameQBufMgr_NotifyParams     FrameQBufMgr_ShMem_NotifyParams;
typedef FrameQBufMgr_Attrs            FrameQBufMgr_ShMem_Attrs;
typedef FrameQBufMgr_FrameBufInfo     FrameQBufMgr_ShMem_FrameBufInfo;
typedef FrameQBufMgr_FreeFramePoolObj FrameQBufMgr_ShMem_FreeFramePoolObj;

#define FrameQBufMgr_ShMem_MAXNAMELEN         FrameQBufMgr_MAXNAMELEN


#define FrameQBufMgr_ShMem_FrameBufInfo       Frame_FrameBufInfo

#define FrameQBufMgr_ShMem_FrameHeader        Frame_FrameHeader

typedef Frame_FrameHeader                    *FrameQBufMgr_ShMem_Frame;


/*!
 *  @brief  Handle for the FrameQBufMgr instance.
 */
typedef struct FrameQBufMgr_ShMem_Object_tag* FrameQBufMgr_ShMem_Handle;

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Function to create a FrameQBufMgr.params should be of type specific to
 * implementation.
 * For example if apps want to create FrameQBufMgr using SharedMemory
 * implementation they have to pass FrameQBufMgr_ShMem_Params to this create
 * call.
 */
FrameQBufMgr_ShMem_Handle
FrameQBufMgr_ShMem_create (FrameQBufMgr_ShMem_Params *params);

/* Function to delete the created FrameQBufMgr instance*/
Int32
FrameQBufMgr_ShMem_delete (FrameQBufMgr_ShMem_Handle * handle);

/* Function to open the created FrameQ instance when name is known and
 * name server is configured.The instance must be created before opening it.
 * Application is expected to pass the implementation specific open params for
 * the openParams field.
 */
Int32
FrameQBufMgr_ShMem_open (FrameQBufMgr_ShMem_Handle *  handlePtr,
                         FrameQBufMgr_ShMem_OpenParams *openParams);

/* Function to close the dynamically opened shared memory
 * FrameQBufMgr instance.
 */
Int32
FrameQBufMgr_ShMem_close(FrameQBufMgr_ShMem_Handle* pHandle);

/*
 *  ======== FrameQBufMgr_ShMem_alloc ========
 *  Function to allocate frame from free frame queue zero .
 */
Int32
FrameQBufMgr_ShMem_alloc (FrameQBufMgr_ShMem_Handle   handle,
                          FrameQBufMgr_ShMem_Frame   *frame);

/*
 *  ======== FrameQBufMgr_ShMem_alloc ========
 *  Function to allocate multiple frames from the FrameQbufMgr instance.
 */
Int32
FrameQBufMgr_ShMem_allocv (FrameQBufMgr_ShMem_Handle     handle,
                           FrameQBufMgr_ShMem_Frame      framePtr[],
                           UInt32                        freeQId[],
                           UInt8                        *numFrames);
/*
 *  ======== FrameQBufMgr_ShMem_free ========
 *  Function to free frame.
 */
Int32
FrameQBufMgr_ShMem_free (FrameQBufMgr_ShMem_Handle  handle,
                         FrameQBufMgr_ShMem_Frame   frame);

/*
 * ======== FrameQBufMgr_ShMem_freev ========
 * Function to free frames.
 *
 * Function to free multiple frames  to the  FrameQBufMgr.
 */
Int32
FrameQBufMgr_ShMem_freev (FrameQBufMgr_ShMem_Handle     handle,
                          FrameQBufMgr_ShMem_Frame      framePtr[],
                          UInt32                        numFrames);

/*
 *  ======== FrameQBufMgr_ShMem_add ========
 * Function to add  free frames dynamically  which are allocated out side of
 * FrmaeQbufMgr through the  same memory manager.
 */
Ptr
FrameQBufMgr_ShMem_add (FrameQBufMgr_ShMem_Handle     handle,
                        UInt8                         freeQId);

/*
 *  ======== FrameQBufMgr_ShMem_remove ========
 * Function to remove frame from the FrameQbufMgr.Only allowed for dynamically
 * added frames.
 */
Int32
FrameQBufMgr_ShMem_remove (FrameQBufMgr_ShMem_Handle    handle,
                           UInt8                        freeQId,
                           FrameQBufMgr_ShMem_Frame     framePtr);

/*
 *  ======== FrameQBufMgr_ShMem_registerNotifier ========
 * Function to Register notification with the instance.  It internally registers
 * notification with the individual free queues( free frame pools).
 * If alloc on free queue 0 failed,Notification will be generated if free frames
 * in that queue becomes more than watermark(all the free frames in that queue).
 */
Int32
FrameQBufMgr_ShMem_registerNotifier (
                                FrameQBufMgr_ShMem_Handle        handle,
                                FrameQBufMgr_ShMem_NotifyParams *notifyParams);

/* Function to unregister the notification call back function.*/
Int32
FrameQBufMgr_ShMem_unregisterNotifier (FrameQBufMgr_ShMem_Handle    handle);

/*
 *  ======== FrameQBufMgr_ShMem_dup ========
 * API to duplicate the given frame i.e it increments the corresponding
 * frames 's reference count.It internally allocates  headers for the duped
 * frames.It fails if it is not able to dup all the frames.
 */
Int32
FrameQBufMgr_ShMem_dup (FrameQBufMgr_ShMem_Handle     handle,
                        FrameQBufMgr_ShMem_Frame      framePtr,
                        FrameQBufMgr_ShMem_Frame      dupedFramePtr[],
                        UInt32                        numDupedFrames);

/*!
 * @brief   API to duplicate the given frames i.e it increments the
 *          corresponding frames 's reference count.It internally allocates
 *          headers for the duped frames.API fails if it is not able to dup all
 *          the frames.
 */
Int32
FrameQBufMgr_ShMem_dupv (FrameQBufMgr_ShMem_Handle   handle,
                         FrameQBufMgr_ShMem_Frame    framePtr[],
                         FrameQBufMgr_ShMem_Frame  **dupedFramePtr,
                         UInt32                      numDupedFrames,
                         UInt32                      numFrames);

/*
 *  ======== FrameQBufMgr_ShMem_writeBack ========
 * Write back the contents  of a frame including frame buffers
 */
Int32
FrameQBufMgr_ShMem_writeBack(FrameQBufMgr_ShMem_Handle handle,
                             FrameQBufMgr_ShMem_Frame  frame);

/*
 *  ======== FrameQBufMgr_ShMem_invalidate ========
 * Invalidate the contents  of a frame including frame buffers.
 */
Int32
FrameQBufMgr_ShMem_invalidate(FrameQBufMgr_ShMem_Handle  handle,
                              FrameQBufMgr_ShMem_Frame   frame);

/*
 *  ======== FrameQBufMgr_ShMem_writeBackHeaderBuf ========
 * Write back the contents  of buffer . buffe is treated as frame  buffer.
 * bufNumInFrame denotes  buffer number in the frame.This is  to identify
 * the cache flags and cpu access flags for the  buffer.
 */
Int32
FrameQBufMgr_ShMem_writeBackHeaderBuf(FrameQBufMgr_ShMem_Handle     handle,
                                      Ptr                           headerBuf);
/*
 *  ======== FrameQBufMgr_ShMem_invalidateHeaderBuf ========
 * Invalidate the contents  of frame buffer .
 *
 * bufIndexInFrame denotes which buffer it is  to identify the cache flags and
 * cpu access flags for the  buffer. This is the sequence number of frame buffer
 * in the frame.
 */
Int32
FrameQBufMgr_ShMem_invalidateHeaderBuf(FrameQBufMgr_ShMem_Handle     handle,
                                       Ptr                           headerBuf);

/*
 *  ======== FrameQBufMgr_ShMem_writeBackFrameBuf ========
 * Write back the contents  of buffer . buffe is treated as frame  buffer.
 * bufNumInFrame denotes  buffer number in the frame.This is  to identify
 * the cache flags and cpu access flags for the  buffer.
 */
Int32
FrameQBufMgr_ShMem_writeBackFrameBuf(FrameQBufMgr_ShMem_Handle  handle,
                                     Ptr                        frameBuf,
                                     UInt32                     size,
                                     UInt8                      bufNoInFrame);

/*
 *  ======== FrameQBufMgr_ShMem_ShMem_invalidateFrameBuf ========
 * Invalidate the contents  of frame buffer .
 *
 * bufNumInFrame denotes which buffer it is  to identify the cache flags and
 * cpu access flags for the  buffer. This is the sequence number of frame buffer
 * in the frame.
 */
Int32
FrameQBufMgr_ShMem_invalidateFrameBuf(FrameQBufMgr_ShMem_Handle  handle,
                                      Ptr                        frameBuf,
                                      UInt32                     size,
                                      UInt8                      bufNoInFrame);
/*
 *  ======== FrameQBufMgr_ShMem_translateAddr ========
 * Function to translate address betweem different types.
 * see also FrameQBufMgr_ShMem_AddrType
 */
Int32
FrameQBufMgr_ShMem_translateAddr (FrameQBufMgr_ShMem_Handle     handle,
                                  Ptr *                         dstAddr,
                                  FrameQBufMgr_ShMem_AddrType   dstAddrType,
                                  Ptr                           srcAddr,
                                  FrameQBufMgr_ShMem_AddrType   srcAddrType,
                                  FrameQBufMgr_ShMem_BufType    bufType);
/*
 *  ======== FrameQBufMgr_ShMem_translateAddr ========
 * Function to get FrameQbufMgr Id.
 */
UInt32
FrameQBufMgr_ShMem_getId (FrameQBufMgr_ShMem_Handle handle);

/*
 *  ======== FrameQBufMgr_ShMem_getHandle ========
 *  Internal API to return the FrameQbufMgr Id.
 */
Ptr
FrameQBufMgr_ShMem_getHandle (UInt32 Id);

/*
 *  ======== FrameQBufMgr_ShMem_getBaseHeaderSize ========
 *  API to get the base frame heade if number of frame buffers known.
 */
UInt32
FrameQBufMgr_ShMem_getBaseHeaderSize(UInt8 numFrameBufs);

/*!
 *  @brief  Provides a hook to perform implementation dependent operation.
 *
 */
Int32
FrameQBufMgr_ShMem_control (FrameQBufMgr_ShMem_Handle handle,
                            Int32                     cmd,
                            Ptr                       arg);

GateMP_Handle FrameQBufMgr_ShMem_getGate(FrameQBufMgr_ShMem_Handle handle);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* HEAPBUF_H_0x4CD5 */

