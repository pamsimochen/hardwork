/** 
 *  @file   IFrameQBufMgr.h
 *
 *  @brief      APIs to call implementation specific functions using plugged in
 *              function table interface.
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



#ifndef _IFRAMEQBUFMGR_H_
#define _IFRAMEQBUFMGR_H_

#include "_FrameQBufMgr.h"
#include <ti/syslink/FrameQDefs.h>


#if defined (__cplusplus)
extern "C" {
#endif

#define IFrameQBufMgr_AddrType    FrameQBufMgr_AddrType
#define IFrameQBufMgr_BufType     FrameQBufMgr_BufType

#define IFrameQBufMgr_FrameBufInfo       Frame_FrameBufInfo

#define IFrameQBufMgr_FrameHeader        Frame_FrameHeader

typedef Frame_FrameHeader       *IFrameQBufMgr_Frame;

typedef struct {
    FrameQBufMgr_Fxns  *fxns;
}IFrameQBufMgr_Object;

/*!
 *  @brief  Forward declaration of structure defining object for the
 *          FrameQBufMgr.
 */
//typedef struct IFrameQBufMgr_Object IFrameQBufMgr_Object;

/*!
 *  @brief  Handle for the FrameQBufMgr instance.
 */
typedef IFrameQBufMgr_Object * IFrameQBufMgr_Handle;

/* delete */
static inline
Void IFrameQBufMgr_delete(IFrameQBufMgr_Handle* pHandle)
{
    ((IFrameQBufMgr_Object*)*pHandle)->fxns->deleteInstance((Ptr)pHandle);

}

/* alloc */
static inline
Int32 IFrameQBufMgr_alloc(IFrameQBufMgr_Handle handle,
                          IFrameQBufMgr_Frame* frame)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->alloc((Void*)handle, frame);
}

/* allocv */
static inline
Int32 IFrameQBufMgr_allocv(IFrameQBufMgr_Handle handle,
                           IFrameQBufMgr_Frame  framePtr[],
                           UInt32               freeQId[],
                           UInt8               *numFrames)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->allocv((Void*)handle, framePtr, freeQId, numFrames);
}

/* free */
static inline
Int32 IFrameQBufMgr_free(IFrameQBufMgr_Handle handle,
                         IFrameQBufMgr_Frame frame)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->free((Void*)handle, frame);
}

/* freev */
static inline
Int32 IFrameQBufMgr_freev(IFrameQBufMgr_Handle handle,
                          IFrameQBufMgr_Frame  framePtr[],
                          UInt32               numFrames)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->freev((Void*)handle, framePtr, numFrames);
}

/* add */
static inline
Ptr IFrameQBufMgr_add(IFrameQBufMgr_Handle handle,
                      UInt8                freeQId)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->add((Void*)handle, freeQId);
}

/* remove */
static inline
Int32 IFrameQBufMgr_remove(IFrameQBufMgr_Handle handle,
                           UInt8                freeQId,
                           IFrameQBufMgr_Frame  framePtr)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->remove((Void*)handle, freeQId, framePtr);
}

/* registerNotifier */
static inline
Int32 IFrameQBufMgr_registerNotifier(IFrameQBufMgr_Handle handle,
                                     Ptr  notifyParams)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->registerNotifier((Void*)handle, notifyParams);
}

/* unregisterNotifier */
static inline
Int32 IFrameQBufMgr_unregisterNotifier(IFrameQBufMgr_Handle handle)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->unregisterNotifier((Void*)handle);
}

/* dup */
static inline
Int32 IFrameQBufMgr_dup(IFrameQBufMgr_Handle handle,
                        IFrameQBufMgr_Frame  framePtr,
                        IFrameQBufMgr_Frame  dupedFramePtr[],
                        UInt32               numDupedFrames)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->dup((Void*)handle,
                             framePtr,
                             dupedFramePtr,
                             numDupedFrames);
}

/* dupv */
static inline
Int32 IFrameQBufMgr_dupv(IFrameQBufMgr_Handle handle,
                         IFrameQBufMgr_Frame  framePtr[],
                         IFrameQBufMgr_Frame** dupedFramePtr,
                         UInt32               numDupedFrames,
                         UInt32               numFrames)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->dupv((Void*)handle,
                              framePtr,
                              dupedFramePtr,
                              numDupedFrames,
                              numFrames);
}

/* writeBack */
static inline
Int32 IFrameQBufMgr_writeBack(IFrameQBufMgr_Handle handle,
                              IFrameQBufMgr_Frame frame)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->writeBack((Void*)handle, frame);
}

/* invalidate */
static inline
Int32 IFrameQBufMgr_invalidate(IFrameQBufMgr_Handle handle,
                               IFrameQBufMgr_Frame  frame)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->invalidate((Void*)handle, frame);
}

/* writeBackHeaderBuf */
static inline
Int32 IFrameQBufMgr_writeBackHeaderBuf(IFrameQBufMgr_Handle handle,
                                       Ptr                  headerBuf)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->writeBackHeaderBuf((Void*)handle, headerBuf);
}

/* invalidateHeaderBuf */
static inline
Int32 IFrameQBufMgr_invalidateHeaderBuf(IFrameQBufMgr_Handle handle,
                                        Ptr                  headerBuf)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->invalidateHeaderBuf((Void*)handle, headerBuf);
}

/* writeBackFrameBuf */
static inline
Int32 IFrameQBufMgr_writeBackFrameBuf(IFrameQBufMgr_Handle handle,
                                      Ptr                  frameBuf,
                                      UInt32               size,
                                      UInt8                bufNumInFrame)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->writeBackFrameBuf((Void*)handle,
                                           frameBuf,
                                           size,
                                           bufNumInFrame);
}

/* invalidateFrameBuf */
static inline
Int32 IFrameQBufMgr_invalidateFrameBuf(IFrameQBufMgr_Handle handle,
                                       Ptr                  frameBuf,
                                       UInt32               size,
                                       UInt8                bufNumInFrame)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->invalidateFrameBuf((Void*)handle,
                                             frameBuf,
                                             size,
                                             bufNumInFrame);
}

/* translateAddr */
static inline
Int32 IFrameQBufMgr_translateAddr(IFrameQBufMgr_Handle   handle,
                                  Ptr*                   dstAddr,
                                  IFrameQBufMgr_AddrType dstAddrType,
                                  Ptr                    srcAddr,
                                  IFrameQBufMgr_AddrType srcAddrType,
                                  IFrameQBufMgr_BufType  bufType)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->translateAddr((Void*)handle,
                                       dstAddr,
                                       dstAddrType,
                                       srcAddr,
                                       srcAddrType,
                                       bufType);
}

/* getId */
static inline
UInt32 IFrameQBufMgr_getId(IFrameQBufMgr_Handle handle)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->getId((Void*)handle);
}

/* getCliNotifyMgrShAddr */
static inline
Ptr IFrameQBufMgr_getCliNotifyMgrShAddr(IFrameQBufMgr_Handle handle)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->getCliNotifyMgrShAddr((Void*)handle);
}

/* getCliNotifyMgrGateShAddr */
static inline
Ptr IFrameQBufMgr_getCliNotifyMgrGateShAddr(IFrameQBufMgr_Handle handle)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->getCliNotifyMgrGateShAddr((Void*)handle);
}

/* setNotifyId */
static inline
Int32 IFrameQBufMgr_setNotifyId(IFrameQBufMgr_Handle handle,UInt32 notifyId)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->setNotifyId((Void*)handle,notifyId);
}

/* resetNotifyId */
static inline
Int32 IFrameQBufMgr_resetNotifyId(IFrameQBufMgr_Handle handle,UInt32 notifyId)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->resetNotifyId((Void*)handle,notifyId);
}

/* control */
static inline
Int32 IFrameQBufMgr_control(IFrameQBufMgr_Handle handle,
                            Int32                cmd,
                            Ptr                  arg)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->control((Void*)handle, cmd, arg);
}

/* control */
static inline
Int32 IFrameQBufMgr_getNumFreeFrames(IFrameQBufMgr_Handle handle, UInt32 * numFreeFrames)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->getNumFreeFrames((Void*)handle, numFreeFrames);
}

static inline
Int32 IFrameQBufMgr_getvNumFreeFrames(IFrameQBufMgr_Handle  handle,
                                      UInt32                numFreeFrames[],
                                      UInt8                 freeFramePoolNo[],
                                      UInt8                 numFreeFramePools)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->getvNumFreeFrames((Void*)handle,
                                                                     numFreeFrames,
                                                                     freeFramePoolNo,
                                                                     numFreeFramePools);
}

/* control */
static inline
Bool IFrameQBufMgr_isCacheEnabledForHeaderBuf(IFrameQBufMgr_Handle handle)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->isCacheEnabledForHeaderBuf((Void*)handle);
}

/* control */
static inline
Bool IFrameQBufMgr_isCacheEnabledForFrameBuf(IFrameQBufMgr_Handle handle,UInt8 arg)
{
    return ((IFrameQBufMgr_Object*)handle)->fxns->isCacheEnabledForFrameBuf((Void*)handle,arg);
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /*_SYSLINK_H_*/


